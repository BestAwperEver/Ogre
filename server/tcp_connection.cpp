#include "tcp_connection.h"
#include "tcp_server.h"
#include "Logger.h"
#include "Player.h"
#include <boost/lexical_cast.hpp>
#include "../../Ogre5/Ogre5/Merk.h"

using namespace boost::asio;
using namespace boost::posix_time;
typedef tcp_server Server;

#define INT(x) *( (int*)(&x) )
#define UD(x) *( (UnitDef*)(&x) )
#define SHORT(x) *( (short*)(&x) )

std::vector<char> operator + (std::vector<char> v, std::string s) {
	std::vector<char> res(v.size() + s.size(), 0);
	memcpy(&res[0], &v[0], v.size());
	memcpy(&res[v.size()], &s[0], s.size());
	return std::move( res );
}
void operator += (std::vector<char>& v, std::string s) {
	int old_size(v.size());
	v.resize(old_size + s.size());
	memcpy(&v[old_size], &s[0], s.size());
}

talk_to_client::talk_to_client(io_service& service) : sock_(service), started_(false), 
	m_DisconnectTimer(service), 
	m_Timer(service),
	//clients_changed_(false),
	loggedin_(false)
	//id_(-1)
	, m_Player(0)
	//m_bIsActive(false)
{}
talk_to_client::~talk_to_client() {

}
void talk_to_client:: log(const std::string& msg, const boost::system::error_code& e) const {
	//std::cout << msg << " (" << username_ << ") " << e << std::endl;
	//boost::mutex::scoped_lock(m_Mutex);
	if (m_Player) {
		_LOG(msg + " (" + username_ + ", id " + boost::lexical_cast<std::string>(m_Player->get_id())
			+ ") error: " + e.message());
	} else {
		_LOG(msg + " (" + username_ + ") error: " + e.message());
	}
}
void talk_to_client:: log(const std::string& msg, bool is_important) const {
#ifdef ONLY_IMPORTANT_LOG
	if (is_important) {
#endif
		//std::cout << msg << " (" << username_ << ")" << std::endl;
		//boost::mutex::scoped_lock(m_Mutex);
		if (m_Player) {
		_LOG(msg + " (" + username_ + ", id "
			+ boost::lexical_cast<std::string>(m_Player->get_id()) + ")");
		} else {
			_LOG(msg + " (" + username_ + ")");
		}
#ifdef ONLY_IMPORTANT_LOG
	}
#endif
}

talk_to_client::ptr talk_to_client::new_(io_service& service) {
	ptr new_(new talk_to_client(service));
	return new_;
}

void talk_to_client:: start() {
	started_ = true;
	last_sended = clock();//boost::posix_time::microsec_clock::local_time();
	// first, we wait for client to login
	//post_check_ping();
	do_read();
}
void talk_to_client:: stop() {
	if ( !started_) {
		log("stop: returned, not started", true);
		return;
	}
	log("stop", true);
	started_ = false;
	if (loggedin_) {
		_SERVER.drop_client( shared_from_this() );
	}
	sock_.close();

	//ptr self = shared_from_this();
	//_SERVER.drop_client(shared_from_this());
}

std::string talk_to_client::get_address_string() const {
	return sock_.remote_endpoint().address().to_string();
}

bool talk_to_client:: post(std::vector<char>&& msg) {
	if (started_) {
		do_add_outcoming_msg(msg);
		return true;
	}
	return false;
}
bool talk_to_client:: post(const std::vector<char>& msg) {
	if (started_) {
		do_add_outcoming_msg(msg);
		return true;
	}
	return false;
}
bool talk_to_client:: post(char command) {
	if (started_) {
		do_add_outcoming_msg(std::vector<char>(1, command));
		return true;
	}
	return false;
}

void talk_to_client:: on_read_size(const error_code & err) {
	m_DisconnectTimer.cancel();
	if ( err) {
		log("on_read_size", err);
		stop();
		return;
	}
	if ( !started_ ) return;
	if (*((short*)read_buffer_) > max_msg) {
		log("Too big message size: " + boost::to_string(*((short*)read_buffer_)), true);
		client_is_bastard();
		return;
	}
	do_read(false);
}
void talk_to_client:: on_read(const error_code & err, size_t bytes) {
	m_DisconnectTimer.cancel();
	if ( err) {
		log("on_read",err);
		stop();
		return;
	}
	if ( !started_ ) return;

	if (check_ping() == CP_TOO_BIG) {
		log("stopping client due to a high ping", true);
		//write("ping high");
		do_ping();
		stop();
		return;
	}

	if (*read_buffer_ == static_cast<char>(REQUEST_TYPE::PING)) {
		on_ping();
		return;
	}

	// process the msg
	if (read_buffer_[bytes-1] != static_cast<char>(REQUEST_TYPE::END_MESSAGE)) {
		log("Message is not ended by REQUEST_TYPE::END_MESSAGE", true);
		client_is_bastard();
	}
	std::vector<char> msg(read_buffer_, read_buffer_ + bytes);
	if ( msg[0] == static_cast<char>(REQUEST_TYPE::LOGIN) ) on_login(msg);
	else {
		if (!loggedin_) {
			log("Client is not logged in", true);
			client_is_bastard();
			return;
		}
		//if ( msg[0] == static_cast<char>(REQUEST_TYPE::PING) ) on_ping();
		//else if ( msg.find("ask_clients") == 0) on_clients();
		//else 
		parse_message(msg);//std::cerr << "invalid msg " << msg << std::endl;
	}
}
void talk_to_client:: on_login(std::vector<char>& msg) {
	if (msg.size() < sizeof(short)) {
		client_is_bastard();
		return;
	}

	char len = msg[1];
	
	if (msg.size() != sizeof(short) + len + 1) {
		client_is_bastard();
		return;
	}

	username_ = std::string(&msg[sizeof(short)], &msg[sizeof(short)] + len);

	if (_SERVER.client_registered(username_)) {
		if (_SERVER.already_logged_in(username_)) {
			log("Client already logged in", true);
			//char ans[3] = {static_cast<char>(REQUEST_TYPE::LOGIN), static_cast<char>(REQUEST_TYPE::LOGIN)_ALREADY, 0};
			std::vector<char> ans(2, 0);
			ans[0] = static_cast<char>(REQUEST_TYPE::LOGIN);
			ans[1] = static_cast<char>(SERVER_ANSWER::LOGIN_ALREADY);
			write(ans);
			return;
		} else if (username_ == "Radagast"
			&& sock_.remote_endpoint().address().to_string() != "127.0.0.1")
		{
			log("Detected trying to log as Radagast from " + sock_.remote_endpoint().address().to_string(), true);
			//char ans[3] = {static_cast<char>(REQUEST_TYPE::LOGIN), static_cast<char>(REQUEST_TYPE::LOGIN)_FAILED, 0};
			std::vector<char> ans(2, 0);
			ans[0] = static_cast<char>(REQUEST_TYPE::LOGIN);
			ans[1] = static_cast<char>(SERVER_ANSWER::LOGIN_ALREADY);
			write(ans);
			return;
		}

		_SERVER.add_client( shared_from_this() );

		loggedin_ = true;

		//id_ = _CLDATA.getClientID(username_);

		const int N = 3*sizeof(char) + sizeof(int);
		//char ans[N];
		std::vector<char> ans(N, 0);
		int i = 0;
		ans[i++] = static_cast<char>(REQUEST_TYPE::LOGIN);
		ans[i++] = static_cast<char>(SERVER_ANSWER::LOGIN_OK);
		//*( (int*)(ans+i) ) = get_id();
		INT(ans[i]) = get_id();
		i += sizeof(int);
		ans[i++] = username_.size();
		//ans[i] = 0;
		//std::string ans;
		//ans.push_back(static_cast<char>(REQUEST_TYPE::LOGIN));
		//ans.push_back(static_cast<char>(REQUEST_TYPE::LOGIN)_OK);
		//ans.push_back(username_.size());
		//ans += username_;
		//ans.push_back(_SERVER.get_team(username_)); // теперь в лобби

		//// костыльчик =(^_^)=
		//ans.push_back(LOAD_MAP);
		//ans.push_back(7);
		//ans += "testmap";

		write(ans + username_);

		_SERVER.update_clients_changed();

		//if (_SERVER.game_started()) do_add_outcoming_msg(_SERVER.get_map_create_msg());

	} else {
		log("Client dropped: unregistered", true);
		//char ans[3] = {static_cast<char>(REQUEST_TYPE::LOGIN), static_cast<char>(REQUEST_TYPE::LOGIN)_FAILED, 0};
		std::vector<char> ans(2, 0);
		ans[0] = static_cast<char>(REQUEST_TYPE::LOGIN);
		ans[1] = static_cast<char>(SERVER_ANSWER::LOGIN_FAILED);
		write(ans);
	}
}
void talk_to_client:: on_ping() {
	log("on_ping");
	//do_write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
	check_out_message();
	//clients_changed_ = false;
}
void talk_to_client:: on_clients() {
#ifdef _USE_ASSERTS_
	assert(false && "wrong request");
#endif
	log("on_clients");
	//std::vector<char> msg = _SERVER.getClientListStr();
	//do_write("clients " + msg);
}
void talk_to_client:: on_write(const error_code & err, size_t bytes, bool disconnect_after_writing) {
	last_sended = clock();
	//last_sended = boost::posix_time::microsec_clock::local_time();
	if (err) {
		log("on_write", err);
		stop();
		return;
	}
	log("on_write");
	if (disconnect_after_writing) {
		stop();
	} else {
		do_read();
	}
}

void talk_to_client:: write(std::vector<char> msg) {
	//std::string len_str(sizeof(short),0);
	short size = msg.size() + 1;
	//std::memcpy(&len_str[0], &size, sizeof(short));

	std::vector<char> final_msg(sizeof(short) + msg.size() + sizeof(char), 0);
	memcpy(&final_msg[0], &size, sizeof(short));
	memcpy(&final_msg[sizeof(short)], &msg[0], msg.size());
	final_msg[sizeof(short) + msg.size()] = static_cast<char>(REQUEST_TYPE::END_MESSAGE);

	//do_write(len_str + msg + char(static_cast<char>(REQUEST_TYPE::END_MESSAGE)));
	do_write(std::move( final_msg ));
}
//void talk_to_client:: write(std::vector<char>&& msg) {
//	do_write(char(msg.size()+1) + msg + char(static_cast<char>(REQUEST_TYPE::END_MESSAGE)));
//}
void talk_to_client:: ping() {
	log("ping");
	if (last_ping < TIME_TO_PING - 10) {
		m_Timer.expires_from_now(boost::posix_time::milliseconds(TIME_TO_PING - last_ping));
		m_Timer.async_wait( MEM_FN(do_ping));
	} else {
		do_ping();
	}
}
void talk_to_client:: disconnect_timer_handler(const boost::system::error_code& e) {
	if (e) {
		if (e.value() == 995) {
			log("disconnect_timer calcelled", true);
			return;
		}
	log("disconnect_timer_handler", e);
	return;
	}
	log("Client is not responding", true);
	stop();
}
void talk_to_client:: post_check_ping(int time_to_disconnect) {
	log("post_check_ping");
#ifdef CHECK_PING
	m_DisconnectTimer.expires_from_now(boost::posix_time::seconds(time_to_disconnect));
	m_DisconnectTimer.async_wait( MEM_FN1(disconnect_timer_handler, _1));
#endif
}
talk_to_client::CP_RESULT talk_to_client::check_ping() {
	//boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	last_ping = clock() - last_sended;//(now - last_sended).total_microseconds() / 1000;
#ifdef CHECK_PING
	if ( last_ping > CP_TOO_BIG ) {
		log("check ping: " + boost::to_string(last_ping), true);
		return CP_TOO_BIG;
	} else {
		log("check_ping: " + boost::to_string(last_ping));
	}
#endif
	return CP_OK;
}

void talk_to_client:: parse_message(const std::vector<char>& msg) {
	bool res = false;
	if (msg[0] == static_cast<char>(REQUEST_TYPE::MOVE)) {
		res = parse_move(msg);
		//if (res) {
		//	if (_SERVER.game_started())
		//		_SERVER.post(msg.substr(0, msg.size()-1));
		//	else
		//		post(msg.substr(0, msg.size()-1));
		//}
		//char ans = static_cast<char>(REQUEST_TYPE::UNBLOCK);
		//post(std::string(&ans,1));

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::REQ_SHOOT)) {
		res = parse_shoot(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::CREATE_UNIT)) {
		if (!m_Player->is_in_game()) {
			res = false;
		}
		res = parse_create(msg);
		//if (res) {
		//	if (_SERVER.game_started())
		//		_SERVER.post(msg.substr(0, msg.size()-1));
		//	//else
		//	//	post(msg.substr(0, msg.size()-1));
		//}
		//char ans = static_cast<char>(static_cast<char>(REQUEST_TYPE::UNBLOCK));
		//post(std::string(&ans,1));

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::CREATE_UNION)) {
		res = false;
		//res = parse_create_union(msg);
		//if (res) _SERVER.post_all(msg.substr(0, msg.size()-1));
		//char ans = static_cast<char>(REQUEST_TYPE::UNBLOCK);
		//post(std::string(&ans,1));

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::TURN_END)) {
		res = parse_turn_end(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::CREATE_LOBBY)) {
		res = parse_create_lobby(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::GAME_STARTED)) {
		res = parse_game_start(msg);
		//if (username_ == "Radagast") 
		//	_SERVER.start_game();

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::WANT_TO_DISCONNECT)) {
		res = true;
		//stop();
		_SERVER.drop_client(shared_from_this());

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::REQ_NAME)) {
		res = parse_req_name(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::DROP_FROM_LOBBY)) {
		res = parse_drop_from_lobby(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::JOIN_LOBBY)) {
		res = parse_join_lobby(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::PLAYERS_IN_LOBBY)) {
		res = parse_players_in_lobby(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::PLAYER_LIST)) {
		res = parse_player_list(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::LOBBY_LIST)) {
		res = parse_lobby_list(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::CHANGE_MAP)) {
		res = parse_change_map(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::CREATE_MERK)) {
		res = parse_create_merk(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::DELETE_MERK)) {
		res = parse_delete_merk(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::CHOOSE_MERK)) {
		res = parse_choose_merk(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::CHANGE_WEAPON)) {
		res = parse_change_weapon(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::MERK_LIST)) {
		res = parse_merk_list(msg);

	} else if (msg[0] == static_cast<char>(REQUEST_TYPE::EXIT_GAME)) {
		res = parse_exit_game(msg);
	}
	//if (res) _SERVER.post(msg.substr(0, msg.size()-1) + char(static_cast<char>(REQUEST_TYPE::UNBLOCK)));
	//else {
	if (!res) {
//		char ans[sizeof(short)];
//		ans[0] = msg[0];
		_LOG("Sending GTFO to " + username_
			+ " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
		std::vector<char> ans(2, 0);
		ans[0] = static_cast<char>(REQUEST_TYPE::DENIED); // GTFO
		ans[1] = static_cast<char>(REQUEST_TYPE::UNBLOCK);
		post(std::move( ans ));
	}
	check_out_message();
}
//bool talk_to_client:: parse_login(const std::vector<char>& msg) {
//	return true;
//}
bool talk_to_client:: parse_turn_end(const std::vector<char>& msg) {
	_FILELOG("parse_turn_end from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	//Player::ptr me = _SERVER.get_player(id_);
	if (!m_Player->is_in_game())
		return false;
	//return _SERVER.force_next_turn(_CLDATA.getClientID(username_));
	//return _SERVER.games()[me->get_game_id()]->force_next_turn(id_);
	return m_Player->get_game()->force_next_turn(get_id());
}
bool talk_to_client:: parse_create_union(const std::vector<char>& msg) {
	_FILELOG("parse_create_union from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	//// Этого тут ничего быть не должно, юнионы должен создавать сервер
	//char count = msg[1];
	//std::list<int> Union(count);
	//for (int j = 0; j < count; j += sizeof(int)) {
	//	Union.push_back(*((int*)(&msg[2+j])));
	//}
	////return _SERVER.add_union( std::move(Union) );
	//Player::ptr me = _SERVER.players()[id_];
	//return _SERVER.games()[me->get_game_id()]->addUnion( std::move(Union) );
	return false;
}
bool talk_to_client:: parse_move(const std::vector<char>& msg) {
	_FILELOG("parse_move from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	//Player::ptr me = _SERVER.get_player(id_);

	if (!m_Player->is_in_game())
		return false;

	GameMap::ptr game = m_Player->get_game();

	int N = msg[1];
	std::vector<CellCoordinates> Path((CellCoordinates*)&msg[2], (CellCoordinates*)&msg[N*2+4]);
//	Path.resize(N + 1);
//	CellCoordinates *path = new CellCoordinates[N+1];
//	for (int i = 0; i < N+1; i+=2) {
//		Path.push_back(CellCoordinates(msg[2+i], msg[3+i]));
//	}
//	CellCoordinates from(msg[2], msg[3]);
//	CellCoordinates to(msg[N*2+2], msg[N*2+3]);
	bool unblock;
	if (!game->move_unit(Path, unblock)) {
		return false;
	}
	if (unblock) {
		//char ans = static_cast<char>(REQUEST_TYPE::UNBLOCK);
		post(std::vector<char>(1, static_cast<char>(REQUEST_TYPE::UNBLOCK)));
	}
	return true;
}
bool talk_to_client:: parse_shoot(const std::vector<char>& msg) {
	_FILELOG("parse_shoot from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	//Player::ptr me = _SERVER.get_player(id_);

	if (!m_Player->is_in_game())
		return false;

	GameMap::ptr game = m_Player->get_game();

	bool unblock = true;
	if (!game->shoot(*((CellCoordinates*)&msg[1]), *((CellCoordinates*)&msg[3]), unblock)) {
		return false;
	}
	if (unblock) {
		post(std::vector<char>(1, static_cast<char>(REQUEST_TYPE::UNBLOCK)));
	}
	return true;
}

bool talk_to_client:: parse_create(const std::vector<char>& msg) {
	_FILELOG("parse_create from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	int i = 1;
	//char type_id = msg[i++];
	//if (type_id == UNIT_TYPE::HERO) {
		//int NameSize = msg[i++];
		//std::string Name = msg.substr(i, NameSize);
		//i += NameSize;
		//char type_id = msg[i++];
		//char Class = msg[i++];
		//int player_id = msg[i++];
		//short HP = *( (short*)&msg[i] );
		//i += sizeof(short);
		//char AP = msg[i++];
		//char Weapon = msg[i++];
		//char Armor = msg[i++];
		//CellCoordinates where(msg[i], msg[i+1]);
		//i += 2;
		//CellCoordinates looks_to(msg[i], msg[i+1]);
		//i += 2;
		UnitDef ud = UD(msg[i]);
		i += sizeof(ud);
		int NameSize = msg[i++];
		std::string Name(&msg[i], &msg[i + NameSize]);//.substr(i, NameSize);
		i += NameSize;
		//UnitDef ud;
		//ud.type_id = msg[i++];
		//ud.Class = msg[i++];
		//ud.PlayerID = msg[i++];
		//ud.HP = *( (short*)&msg[i] );
		//i += sizeof(short);
		//ud.AP = msg[i++];
		//ud.Weapon = msg[i++];
		//ud.Armor = msg[i++];
		//// костыль мод он
		//ud.ViewRange = _DATA.getDefaultViewRange(ud.type_id);
		//// костыль мод офф
		//ud.type_id = msg[i++];
		//ud.Class = msg[i++];
		//ud.PlayerID = msg[i++];
		//ud.HP = *( (short*)&msg[i] );
		//i += sizeof(short);
		//ud.AP = msg[i++];
		//ud.Weapon = msg[i++];
		//ud.Armor = msg[i++];
		//// костыль мод он
		//ud.ViewRange = _DATA.getDefaultViewRange(ud.type_id);
		//// костыль мод офф
		CellCoordinates where(msg[i], msg[i+1]);
		i += 2;
		CellCoordinates looks_to(msg[i], msg[i+1]);
		i += 2;

		//int game_id = _SERVER.players()[id_]->get_game_id();
		//return _SERVER.games()[game_id]->add_hero(Name, ud, where, looks_to);
		//return _SERVER.get_player(id_)->get_game()->add_hero(Name, ud, where, looks_to);
		//return _SERVER.add_hero(id_, Name, ud);
		return m_Player->get_game()->add_hero(Name, ud, where, looks_to);
	//} else {
	//	//char Class = msg[i++];
	//	//int player_id = msg[i++];
	//	//short HP = *( (short*)&msg[i] );
	//	//i += sizeof(short);
	//	//char AP = msg[i++];
	//	//char Weapon = msg[i++];
	//	//char Armor = msg[i++];
	//	//CellCoordinates where(msg[i], msg[i+1]);
	//	//i += 2;
	//	//CellCoordinates looks_to(msg[i], msg[i+1]);
	//	//i += 2;
	//	//UnitDef ud;
	//	//ud.type_id = type_id;
	//	//ud.Class = msg[i++];
	//	//ud.PlayerID = msg[i++];
	//	//ud.HP = *( (short*)&msg[i] );
	//	//i += sizeof(short);
	//	//ud.AP = msg[i++];
	//	//ud.Weapon = msg[i++];
	//	//ud.Armor = msg[i++];
	//	//// костыль мод он
	//	//ud.ViewRange = _DATA.getDefaultViewRange(ud.type_id);
	//	//// костыль мод офф
	//	UnitDef ud = *( (UnitDef*)&msg[i] );
	//	//ud.type_id = msg[i++];
	//	//ud.Class = msg[i++];
	//	//ud.PlayerID = msg[i++];
	//	//ud.HP = *( (short*)&msg[i] );
	//	//i += sizeof(short);
	//	//ud.AP = msg[i++];
	//	//ud.Weapon = msg[i++];
	//	//ud.Armor = msg[i++];
	//	//// костыль мод он
	//	//ud.ViewRange = _DATA.getDefaultViewRange(ud.type_id);
	//	//// костыль мод офф
	//	i += sizeof(ud);
	//	CellCoordinates where(msg[i], msg[i+1]);
	//	i += 2;
	//	CellCoordinates looks_to(msg[i], msg[i+1]);
	//	i += 2;

	//	//int game_id = _SERVER.players()[id_]->get_game_id();
	//	//return _SERVER.games()[game_id]->add_unit(ud, where, looks_to);
	//	//return _SERVER.get_player(id_)->get_game()->add_unit(ud, where, looks_to);
	//	//return _SERVER.add_unit(id_, ud);
	//	return m_Player->get_game()->add_unit(ud, where, looks_to);
	//}
}
bool talk_to_client:: parse_create_lobby(const std::vector<char>& msg) {
	_FILELOG("parse_create_lobby from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	//Player::ptr me = _SERVER.get_player(id_);
	if (msg.size() != 3) {
		return false;
	}
	if (m_Player->is_in_lobby() || m_Player->is_in_game()) {
		return false;
	} else {
		_SERVER.add_lobby(m_Player, msg[1]); // msg[1] - это map_id
	}
	return true;
}
bool talk_to_client:: parse_join_lobby(const std::vector<char>& msg) {
	//if (msg.size() != 2*sizeof(char) + sizeof(int)) {
	//	return false;
	//}
	_FILELOG("parse_join_lobby from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	int lobby_id = INT(msg[1]);
	return _SERVER.join_lobby(m_Player, lobby_id);
}
bool talk_to_client:: parse_game_start(const std::vector<char>& msg) {
	_FILELOG("parse_game_start from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	//Player::ptr me = _SERVER.get_player(id_);
	if (m_Player->is_in_lobby() == false) {
		return false;
	} else {
		//_SERVER.lobbies()[me->get_lobby_id()].start_game();
		//return _SERVER.start_game(me->get_lobby_id(), id_);
		return _SERVER.start_game(m_Player);
	}
	//return true;
}
bool talk_to_client:: parse_req_name(const std::vector<char>& msg) {
	_FILELOG("parse_req_name from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	if (msg.size() != sizeof(char) + sizeof(int)) {
		return false;
	}

	int player_id = INT(msg[1]);
	if (!_SERVER.get_player(player_id)) {
		return false;
	}
	
	const int N = 2*sizeof(char) + sizeof(int);
	//char ans[N];
	std::vector<char> ans(N, 0);
	int i = 0;

	ans[i++] = static_cast<char>(REQUEST_TYPE::PLAYER_NAME);
	INT(ans[i]) = player_id;
	i += sizeof(int);

	std::string Name = _CLDATA.getClientName(player_id);
	ans[i++] = Name.size();

	return post(ans + Name);
}
bool talk_to_client:: parse_drop_from_lobby(const std::vector<char>& msg) {
	_FILELOG("parse_drop_from_lobby from " + username_ + " (id " + boost::lexical_cast<std::string>(get_id()) + ")");
	//Player::ptr me = _SERVER.get_player(id_);
	if (!m_Player->is_in_lobby()) {
		return false;
	}
	//return _SERVER.drop_from_lobby(me->get_lobby_id(), id_);
	return _SERVER.drop_from_lobby(m_Player);
}
bool talk_to_client:: parse_players_in_lobby(const std::vector<char>& msg) {
	int lobby_id = INT(msg[1]);
	return _SERVER.send_players_in_lobby_list(m_Player, lobby_id);
}
bool talk_to_client:: parse_player_list(const std::vector<char>& msg) {
	return _SERVER.send_player_list(m_Player);
}
bool talk_to_client:: parse_lobby_list(const std::vector<char>& msg) {
	return _SERVER.send_lobby_list(m_Player);
}
bool talk_to_client:: parse_change_map(const std::vector<char>& msg) {
	if (!m_Player->is_host_of_the_lobby()) {
		return false;
	}
	return _SERVER.change_map(m_Player, msg[1]);
}
bool talk_to_client:: parse_create_merk(const std::vector<char>& msg) {
/*
	1) static_cast<char>(REQUEST_TYPE::CREATE_MERK)
	2) type (char)
	3) weapon_id (char)
	4) количество символов в имени (char)
	5) имя
*/
	char type_id = msg[1];
	char weapon_id = msg[2];
	char len = msg[3];
	std::string name(&msg[4], &msg[4+len]);
	UnitDef ud = _DATA.getDefaultUD(type_id);
	//ud.type_id = type_id;
	ud.Weapon = weapon_id;
	ud.PlayerID = m_Player->get_id();
	return _SERVER.create_merk(m_Player, name, ud);
}
bool talk_to_client:: parse_delete_merk(const std::vector<char>& msg) {
	int merk_id = INT(msg[1]);
	return _SERVER.delete_merk(m_Player, merk_id);
}
bool talk_to_client:: parse_merk_list(const std::vector<char>& msg) {
	return _SERVER.send_merk_list(m_Player);
}
bool talk_to_client:: parse_choose_merk(const std::vector<char>& msg) {
	int merk_id = INT(msg[1]);
	return _SERVER.choose_merk(m_Player, merk_id);
}
bool talk_to_client:: parse_change_weapon(const std::vector<char>& msg) {
	int merk_id = INT(msg[1]);
	char weapon_id = msg[5];
	return _SERVER.change_weapon(m_Player, merk_id, weapon_id);
}
bool talk_to_client:: parse_exit_game(const std::vector<char>& msg) {
	return _SERVER.drop_from_game(m_Player);
}

void talk_to_client:: check_out_message() {
#ifdef ENABLE_CONSOLE_NET_LOG
	log("check_out_message");
#endif
	if (!started()) {
#ifdef ENABLE_CONSOLE_NET_LOG
	log("can't check outcoming messages: connection isn't started", true);
#endif
		return;
	}
	boost::mutex::scoped_lock lk(m_Mutex);
	if (m_OutcomingMsgs.size()) {
		write(m_OutcomingMsgs.front());
		m_OutcomingMsgs.pop_front();
	} else {
		ping();
	}
}
void talk_to_client:: client_is_bastard() {
	log("Client " + sock_.remote_endpoint().address().to_string() + " is damn bastard", true);
	stop();
}

void talk_to_client::	do_read(bool only_size) {
	if ( !started_) {
		log("do_read: returned, not started", true);
		return;
	}
	log("do_read");
	if (only_size) {
		post_check_ping();
		async_read(sock_, buffer(read_buffer_), 
			//MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
			transfer_exactly(sizeof(short)), MEM_FN1(on_read_size, _1));
	} else {
		post_check_ping(5);
		async_read(sock_, buffer(read_buffer_), 
			transfer_exactly(*((short*)(read_buffer_))), MEM_FN2(on_read,_1,_2));
	}
}
void talk_to_client::	do_write(std::vector<char> msg, bool disconnect_after) {
	if ( !started_) {
		log("do_write: returned, not started", true);
		return;
	}
	log("do_write");
#ifdef _USE_ASSERTS_
	assert( msg.size() < max_msg );
#endif
	std::copy(msg.begin(), msg.end(), write_buffer_);
//	sock_.async_write_some( buffer(write_buffer_, msg.size()), 
//		MEM_FN3(on_write,_1,_2,disconnect_after));
	async_write(sock_, buffer(write_buffer_, msg.size()), transfer_exactly(msg.size()), 
		MEM_FN3(on_write,_1,_2,disconnect_after));
}
//void talk_to_client::	do_write(std::string && msg, bool disconnect_after) {
//	if ( !started_) {
//		log("do_write: returned, not started", true);
//		return;
//	}
//	log("do_write");
//#ifdef _USE_ASSERTS_
//	assert( msg.size() < max_msg );
//#endif
//	std::copy(msg.begin(), msg.end(), write_buffer_);
////	sock_.async_write_some( buffer(write_buffer_, msg.size()), 
////		MEM_FN3(on_write,_1,_2,disconnect_after));
//	async_write(sock_, buffer(write_buffer_, msg.size()), transfer_exactly(msg.size()), 
//		MEM_FN3(on_write,_1,_2,disconnect_after));
//}
void talk_to_client::	do_ping() {
	log("do_ping " + boost::to_string(last_ping));
	//write("ping " + boost::to_string(last_ping));
	//char ans[sizeof(short)*2 + 1];
	std::vector<char> ans(sizeof(short)*2 + sizeof(char), 0);
	SHORT(ans[0]) = sizeof(short) + sizeof(char);
	ans[sizeof(short)] = static_cast<char>(REQUEST_TYPE::PING);
	SHORT(ans[sizeof(short) + sizeof(char)]) = short(last_ping);
	do_write(std::move( ans ));
}
void talk_to_client::	do_ask_clients() {
	assert(false && "do_ask_clients");
	return;
	//log("do_ask_clients");
	//do_write("ask_clients\n");
}
void talk_to_client::	do_add_outcoming_msg(const std::vector<char>& msg) {
	boost::mutex::scoped_lock lk(m_Mutex);
#ifdef _USE_ASSERTS_
	assert(!msg.empty() && "Try to add empty message");
#endif
	m_OutcomingMsgs.push_back(msg);
}
void talk_to_client::	do_add_outcoming_msg(std::vector<char>&& msg) {
	boost::mutex::scoped_lock lk(m_Mutex);
#ifdef _USE_ASSERTS_
	assert(!msg.empty() && "Try to add empty message");
#endif
	m_OutcomingMsgs.push_back(msg);
}

size_t talk_to_client::read_complete(const boost::system::error_code & err, size_t bytes) {
	if ( err) return 0;
	bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
	// we read one-by-one until we get to enter, no buffering
	return found ? 0 : 1;
}

int talk_to_client::get_id() const {
	return m_Player->get_id();
}
void talk_to_client::set_id(int id) {
	m_Player = _SERVER.get_player(id);
}
Player::ptr talk_to_client::get_player() const {
	return m_Player;
}
void talk_to_client::set_player(Player::ptr p) {
	m_Player = p;
}

std::string talk_to_client::username() const {
	return username_;
}
//void talk_to_client::set_active(bool active) {
//	m_bIsActive = active;
//	//do_add_outcoming_msg(active ? TURN_BEGIN : TURN_END);
//}

//void talk_to_client:: on_check_ping() {
//	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
//	int last_ping = (now - last_sended).total_milliseconds();
//	log("on_check_ping");
//	if ( last_ping > 5000) {
//		std::cout << "stopping " << username_ << " due to a high ping" << std::endl;
//		stop();
//	}
//	else if (last_ping > 1000) {
//		std::cout << "ping " << ping << " from " << username_ << std::endl;
//	}
//	last_sended = boost::posix_time::microsec_clock::local_time();
//}