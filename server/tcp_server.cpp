#include "tcp_server.h"
#include "Logger.h"
#include <boost/lexical_cast.hpp>

using namespace boost::asio;
using namespace boost::posix_time;

#define TIME_TO_ACCEPT boost::posix_time::seconds(6000) // при отсутствии запросов на подключение вырубается
#define DEFAULT_PORT 27015

#define INT(x) *( (int*)(&x) )
#define UD(x) *( (UnitDef*)(&x) )

boost::asio::io_service g_Service;

tcp_server::tcp_server(unsigned short port)
	: m_Timer(g_Service, TIME_TO_ACCEPT)
	//, m_TurnTimer(g_Service)
	, m_Acceptor(g_Service, ip::tcp::endpoint(ip::tcp::v4(), port))
	//, m_GameMap("testmap.mapdef")
	//, online_clients(_CLDATA.getClientsCount() + 1, false)
	//, m_bGameStarted(false)
	, m_GamesCount(0)
	, m_LobbiesCount(0)
	//, m_bLogFileCreated(false)
{
	//std::list<int> Union0, Union1, Union2;
	//Union0.push_back(1);
	//Union0.push_back(2);
	//Union1.push_back(3);
	//Union1.push_back(4);
	//Union2.push_back(5);
	//Union2.push_back(6);
	//Union2.push_back(7);
	//m_GameMap.addUnion(Union0);
	//m_GameMap.addUnion(Union1);
	//m_GameMap.addUnion(Union2);

	//m_Maps[0] = "testmap";

//#ifdef ENABLE_LOG
//	m_bLogFileCreated = createGameLogFile();
//#endif
}
tcp_server::~tcp_server() {
//#ifdef ENABLE_LOG
//	if (log_file.is_open()) {
//		log_file.close();
//	}
//#endif
}

//bool tcp_server::createGameLogFile() {
//	log_file.open("server.log");
//	if (!log_file.is_open()) {
//		//Log("Can't create server.log file");
//		std::cout << "Can't create server.log file" << std::endl;
//		return false;
//	}
//	std::time_t t = time(nullptr);
//	std::tm* aTm = localtime(&t);
//	log_file << aTm->tm_mday << "." << std::setw(2) << std::setfill('0') << aTm->tm_mon+1
//		<< "." << aTm->tm_year+1900 << std::endl;
////	delete aTm;
//	return true;
//}

void tcp_server::handle_timer(const boost::system::error_code& err) {
	if (err) {
		return;
	}
	m_Acceptor.cancel();
}
void tcp_server::handle_accept(talk_to_client::ptr client, const boost::system::error_code & err) {
	if (err) {
		std::cout << "acception failed" << std::endl;
		return;
	}

	m_Timer.cancel();

	client->start();
	talk_to_client::ptr new_client = talk_to_client::new_(g_Service);

	m_Timer.expires_from_now(TIME_TO_ACCEPT);
	m_Acceptor.async_accept(new_client->sock(), boost::bind(&tcp_server::handle_accept,this,new_client,_1));
	m_Timer.async_wait( boost::bind( &tcp_server::handle_timer, this, _1) );
}

void tcp_server::update_clients_changed() {
	//for( auto b = clients.begin(), e = clients.end(); b != e; ++b)
	//	b->second->set_clients_changed();
}

tcp_server& tcp_server::getInstance() {
	static tcp_server instance(DEFAULT_PORT);
	return instance;
}

void tcp_server::start() {
	talk_to_client::ptr client = talk_to_client::new_(g_Service);

	m_Acceptor.async_accept(client->sock(), boost::bind(&tcp_server::handle_accept,this,client,_1));
	m_Timer.async_wait( boost::bind(&tcp_server::handle_timer, this, _1) );
		
	g_Service.run();
}

void tcp_server::Log(std::string msg) {
#ifdef ENABLE_LOG
//	if (!m_bLogFileCreated) {
//		return;
//	}
//	std::time_t t = time(nullptr);
//	std::tm* aTm = localtime(&t);
//	log_file << std::setw(2) << std::setfill('0') << aTm->tm_hour << ":"
//		<< std::setw(2) << std::setfill('0') << aTm->tm_min <<":"
//		<< std::setw(2) << std::setfill('0') << aTm->tm_sec << ":" << " " << msg << std::endl;
////	delete aTm;
	_LOG(msg);
#endif
}

//std::string tcp_server::getClientListStr() {
//	std::vector<char> msg;
//	for( auto b = clients.begin(), e = clients.end() ; b != e; ++b)
//		msg += b->second->username() + " ";
//	return msg;
//}

bool tcp_server::start_game(Player::ptr player) {
#ifdef _USE_ASSERTS_
	assert(player.get() && "Player doesn't exist");
#endif
	Lobby::ptr lobby = player->get_lobby();
	if (lobby->get_host() != player) {
		return false;
	}
	if (lobby->get_map_id() < 1 || _DATA.getMapName(lobby->get_map_id()) == "undefined") {
		return false;
	}
	if (player->get_merk() == nullptr) {
		return false;
	}

	add_game(lobby->get_game(++m_GamesCount))->start_game();
	drop_lobby(lobby);

	return true;
}
bool tcp_server::start_game(int lobby_id, int player_id) {
	return start_game(get_player(player_id));
}
//GameMap* tcp_server::add_game(int map_id) {
//	//if (m_Maps[map_id].length() < 2) return nullptr; // карта не найдена
//	if (map_id < 1 || _DATA.getMapName(map_id) == "undefined") {
//		return nullptr;
//	}
//	Log("Add game (id " + boost::lexical_cast<std::string>(m_GamesCount) + ") on map "
//		+ _DATA.getMapName(map_id) + " (id " + boost::lexical_cast<std::string>(map_id) + ")");
//	{
//		boost::mutex::scoped_lock lk(m_Mutex);
//		m_Games[m_GamesCount].operator=( std::move(GameMap(map_id, &g_Service, m_GamesCount)) );
//		//auto asd(GameMap(map_id, &g_Service, m_GamesCount));
//		//auto dsa(std::move(asd));
//		//asd = std::move(GameMap(map_id, &g_Service, m_GamesCount));
//
//		return &m_Games[m_GamesCount++];
//	}
//}
GameMap::ptr tcp_server::add_game(GameMap::ptr game) {
	//if (m_Maps[map_id].length() < 2) return nullptr; // карта не найдена
	Log("Add game (id " + boost::lexical_cast<std::string>(m_GamesCount) + ") on map "
		+ _DATA.getMapName(game->get_map_id())
		+ " (id " + boost::lexical_cast<std::string>(game->get_map_id()) + ")");
	//game->set_id(++m_GamesCount);
	{
		boost::mutex::scoped_lock lk(m_Mutex);
		//m_Games[m_GamesCount].operator=( std::move(GameMap(map_id, &g_Service, m_GamesCount)) );
		m_Games[m_GamesCount] = game;
		//auto asd(GameMap(map_id, &g_Service, m_GamesCount));
		//auto dsa(std::move(asd));
		//asd = std::move(GameMap(map_id, &g_Service, m_GamesCount));

		//return &m_Games[m_GamesCount++];
	}
	return game;
}
bool tcp_server::drop_game(int game_id) {
	if (game_id > m_GamesCount) return false;
	auto it = m_Games.find(game_id);
	if (it == m_Games.end()) return false;
	Log("Drop game " + boost::lexical_cast<std::string>(game_id));
	it->second->remove_players();
	{
		boost::mutex::scoped_lock lk(m_Mutex);
		m_Games.erase(it);
	}
	return true;
}

Lobby::ptr tcp_server::add_lobby(int host_id, char map_id) {
	//Lobby new_lobby(&g_Service, ++m_LobbiesCount, host_id, map_id);
	//Log("Add lobby (id " + boost::lexical_cast<std::string>(m_LobbiesCount) + ") hosted by " 
	//	+ _CLDATA.getClientName(host_id) + " (id " + boost::lexical_cast<std::string>(host_id) +")");
	//{
	//	boost::mutex::scoped_lock lk(m_Mutex);
	//	m_Lobbies[m_LobbiesCount] = new_lobby.shared_from_this();
	//}
	//return new_lobby.shared_from_this();
	return add_lobby(get_player(host_id), map_id);
}
Lobby::ptr tcp_server::add_lobby(Player::ptr host, char map_id) {
#ifdef _USE_ASSERTS_
	assert(host.get() && "Player doesn't exist");
#endif
	Lobby::ptr new_lobby = Lobby::create(&g_Service, ++m_LobbiesCount, host, map_id);
	Log("Add lobby (id " + boost::lexical_cast<std::string>(m_LobbiesCount) + ") hosted by " 
		+ _CLDATA.getClientName(host->get_id()) + " (id " + boost::lexical_cast<std::string>(host->get_id()) +")");
	{
		boost::mutex::scoped_lock lk(m_Mutex);
		m_Lobbies[m_LobbiesCount] = new_lobby;
	}

	const int N = sizeof(char) + sizeof(int) + sizeof(char) + sizeof(int);
	std::vector<char> msg(N, 0);

	int i = 0;
	msg[i++] = static_cast<char>(REQUEST_TYPE::CREATE_LOBBY);
	INT(msg[i]) = host->get_id();
	i += sizeof(int);
	msg[i++] = map_id;
	INT(msg[i]) = m_LobbiesCount;
	i += sizeof(int);

	//post(host, msg);
	post_all(msg);

	return new_lobby;
}
bool tcp_server::drop_lobby(Lobby::ptr lobby) {

	int lobby_id = lobby->get_id();

	return drop_lobby(lobby_id);
}
bool tcp_server::drop_lobby(int lobby_id) {

	auto it = m_Lobbies.find(lobby_id);

	if (lobby_id > m_LobbiesCount || it == m_Lobbies.end()) {
		return false;
	}

	int host_id = it->second->get_host_id();

	Log("Drop lobby (id " + boost::lexical_cast<std::string>(lobby_id) + ") hosted by " 
		+ _CLDATA.getClientName(host_id) + " (id " + boost::lexical_cast<std::string>(host_id) +")");

	it->second->remove_all_players();

	{
		boost::mutex::scoped_lock lk(m_Mutex);
		m_Lobbies.erase(it);
	}

	std::vector<char> msg(sizeof(char) + sizeof(int), 0);
	msg[0] = static_cast<char>(REQUEST_TYPE::DROP_LOBBY);
	INT(msg[1]) = lobby_id;
	post_all(msg);

	return true;
}
bool tcp_server::drop_from_lobby(Player::ptr player) {
	//if (lobby_id > m_LobbiesCount || m_Lobbies.find(lobby_id) == m_Lobbies.end()) {
	//	return false;
	//}
	if (!player->is_in_lobby()) {
		return false;
	}

	//Lobby::ptr lobby = m_Lobbies[lobby_id];
	Lobby::ptr lobby = player->get_lobby();
	if (lobby.get() == nullptr) {
		return false;
	}

	if (!lobby->remove_player(player->get_id())) {
		return false;
	}

	if (lobby->has_players() == false || lobby->get_host() == player) {
		drop_lobby(lobby->get_id());
	}

	Log("Droping " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")"
		+ " from lobby (id " + boost::lexical_cast<std::string>(lobby->get_id()) + ")"
		+ " hosted by " + _CLDATA.getClientName(lobby->get_host_id()));

	return true;
}
bool tcp_server::drop_from_game(Player::ptr player) {
	if (!player->is_in_game()) {
		return false;
	}
	GameMap::ptr game = player->get_game();

	//if (game) game->remove_player(player->get_id());

	if (!game->remove_player(player->get_id())) {
		return false;
	}

	post(player, static_cast<char>(REQUEST_TYPE::EXIT_GAME));
	
	player->set_is_in_game(false);

	Log("Droping " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")"
		+ " from game (id " + boost::lexical_cast<std::string>(game->get_id()) + ")");

	if (!game->has_players()) {
		drop_game(game->get_id());
	}

	return true;
}
bool tcp_server::resume_in_game(Player::ptr player) {
	auto game = player->get_game();
	if (game == nullptr) {
		return false; // bug
	}
	if (m_Games.find(game->get_id()) == m_Games.end()) {
		return false; // bug
	}
	post(player, game->get_map_create_msg()); // got to be tested
	return true;
}
bool tcp_server::join_lobby(int player_id, int lobby_id) {
	return join_lobby(m_Players[player_id], lobby_id);
}
bool tcp_server::join_lobby(Player::ptr player, int lobby_id) {

#ifdef _USE_ASSERTS_
	assert(player.get() && "Player doesn't exist");
#endif

	if (lobby_id > m_LobbiesCount || lobby_id < 1) {
		return false;
	}

	if (player->is_in_lobby()) {
		//char ans[2];
		std::vector<char> ans(2, 0);
		ans[0] = static_cast<char>(REQUEST_TYPE::JOIN_LOBBY);
		ans[1] = static_cast<char>(SERVER_ANSWER::LOGIN_ALREADY);
		post(player, std::move( ans ));
		return true;
	}

	auto it = m_Lobbies.find(lobby_id);

	if (it == m_Lobbies.end()) {
		//char ans[2];
		std::vector<char> ans(2, 0);
		ans[0] = static_cast<char>(REQUEST_TYPE::JOIN_LOBBY);
		ans[1] = static_cast<char>(SERVER_ANSWER::LOGIN_FAILED);
		post(player, std::move( ans ));
		return true;
	}

	Lobby::ptr lobby = it->second;
	int player_id = player->get_id();

	Log("Adding " + _CLDATA.getClientName(player_id)
		+ " (id " + boost::lexical_cast<std::string>(player_id) + ")"
		+ " to lobby (id " + boost::lexical_cast<std::string>(lobby_id) + ")"
		+ " hosted by " + _CLDATA.getClientName(lobby->get_host_id()));

	if (!lobby->add_player(player_id)) return false;

	//char ans[2 + sizeof(int)];
	std::vector<char> ans(2 + sizeof(int), 0);
	ans[0] = static_cast<char>(REQUEST_TYPE::JOIN_LOBBY);
	ans[1] = static_cast<char>(SERVER_ANSWER::LOGIN_OK);
	//*( (int*)(ans+2) ) = lobby_id;
	INT(ans[2]) = lobby_id;
	post(player, std::move( ans ));

	do_post_players_in_lobby(player, lobby);

	return true;
}
bool tcp_server::send_players_in_lobby_list(int player_id, int lobby_id) {
	return send_players_in_lobby_list(m_Players[player_id], lobby_id);
}
bool tcp_server::send_players_in_lobby_list(Player::ptr player, int lobby_id) {

#ifdef _USE_ASSERTS_
	assert(player.get() && "Player doesn't exist");
#endif

	if (lobby_id < 1 || lobby_id > m_LobbiesCount) {
		return false;
	}

	auto it = m_Lobbies.find(lobby_id);
	if (it == m_Lobbies.end()) {
		return false;
	}
	Lobby::ptr lobby = it->second;

	do_post_players_in_lobby(player, lobby);

	return true;
}
void tcp_server::do_post_players_in_lobby(Player::ptr player, Lobby::ptr lobby) {
	//int lobby_id = player->get_lobby_id();
	auto& players = lobby->get_players();
	int players_count = players.size();

	int N = 2*sizeof(char) + (sizeof(char)+sizeof(int))*players_count;
	std::vector<char> ans(N, 0);
	//ans.resize(N);

	int i = 0;
	ans[i++] = static_cast<char>(REQUEST_TYPE::PLAYERS_IN_LOBBY);
	ans[i++] = players_count;
	for (auto it = players.begin(); it != players.end(); ++it) {
		INT(ans[i]) = it->first;
		i += sizeof(int);
		ans[i++] = it->second->get_team();
	}

	Log("Sending players in lobby list to " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")");

	post(player, ans);
}
bool tcp_server::send_player_list(Player::ptr player) {
	int size = m_Players.size();
	int N = sizeof(char) + sizeof(int) + size*(sizeof(int) + sizeof(char) + MAX_NICK_LENGTH);
	std::vector<char> ans(N, 0);
	//ans.resize(N);
	int i = 0;
	ans[i++] = static_cast<char>(REQUEST_TYPE::PLAYER_LIST);
	INT(ans[i]) = size;
	i += sizeof(int);
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		INT(ans[i]) = it->second->get_id();
		i += sizeof(int);
		std::string nick = _CLDATA.getClientName(it->second->get_id());
		ans[i++] = nick.size();
		memcpy_s(&ans[i], nick.size(), nick.c_str(), nick.size());
		i += nick.size();
	}
	ans.resize(i);
	Log("Sending player list to " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")");
	post(player, ans);
	return true;
}
bool tcp_server::send_lobby_list(Player::ptr player) {
	int size = m_Lobbies.size();
	int N = sizeof(char) + sizeof(int) + size*(sizeof(int) + sizeof(int) + sizeof(char));
	std::vector<char> ans(N, 0);
	//ans.resize(N);
	int i = 0;
	ans[i++] = static_cast<char>(REQUEST_TYPE::LOBBY_LIST);
	INT(ans[i]) = size;
	i += sizeof(int);
	for (auto it = m_Lobbies.begin(); it != m_Lobbies.end(); ++it) {
		INT(ans[i]) = it->second->get_id();
		i += sizeof(int);
		INT(ans[i]) = it->second->get_host_id();
		i += sizeof(int);
		ans[i++] = it->second->get_map_id();
	}
	if (i != N) {
		int a = 5;
	}
	Log("Sending lobby list to " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")");
	post(player, ans);
	return true;
}
bool tcp_server::change_map(Player::ptr player, char map_id) {
	//Lobby::ptr lobby = player->get_lobby();
	//if (!player->get_lobby()->change_map(map_id)) {
	//	return false;
	//}

	return player->get_lobby()->change_map(map_id);
}
bool tcp_server::send_merk_list(Player::ptr player) {
	Log("Sending merk list to " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")");
	player->send_merk_list();
	return true;
}
bool tcp_server::create_merk(Player::ptr player, std::string name, UnitDef ud) {
	if (player->get_merk_count() >= MAX_MERK_COUNT) {
		return false;
	}
	if (!player->add_merk(name, ud)) {
		return false;
	}
	int merk_id = player->get_max_merk_id();
	Log("Player " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")"
		+ " just added merk " + name
		+ " (id " + boost::lexical_cast<std::string>(merk_id) + ")");
/*
Создание мерка (ответ):
	1) CREATE_MERK
	2) merk_id (int)
	3) UnitDef
	4) количество символов в имени
	5) имя
*/
	int N = sizeof(char)*2 + sizeof(int) + sizeof(UnitDef) + name.size();
	std::vector<char> msg(N, 0);
	int i = 0;
	msg[i++] = static_cast<char>(REQUEST_TYPE::CREATE_MERK);
	INT(msg[i]) = merk_id;
	i += sizeof(int);
	UD(msg[i]) = player->get_merk(merk_id)->get_ud();
	i += sizeof(UnitDef);
	msg[i++] = name.size();
	memcpy_s(&msg[i], name.size(), name.c_str(), name.size());
	i += name.size();
	if (i != N) {
		int a = 5;
	}
	post(player, msg);
	return true;
}
bool tcp_server::delete_merk(Player::ptr player, int merk_id) {
	if (player->merk_exist(merk_id)) {
		std::string name = player->get_merk(merk_id)->get_name();
		if (!player->delete_merk(merk_id)) {
			return false;
		}
		if (player->get_merk_count() == 1) {
			return false; // нельзя удалить последнего мерка
		}
		Log("Player " + _CLDATA.getClientName(player->get_id())
			+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")"
			+ " just deleted merk " + name
			+ " (id " + boost::lexical_cast<std::string>(merk_id) + ")");
		std::vector<char> msg(5, 0);
		msg[0] = static_cast<char>(REQUEST_TYPE::DELETE_MERK);
		INT(msg[1]) = merk_id;
		post(player, std::move( msg ));
		return true;
	} else {
		return false;
	}
}
bool tcp_server::choose_merk(Player::ptr player, int merk_id) {
	if (!player->set_merk_id(merk_id)) {
		return false;
	}
	Log("Player " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")"
		+ " just choosed merk " + player->get_merk()->get_name());
	std::vector<char> msg(5, 0);
	msg[0] = static_cast<char>(REQUEST_TYPE::CHOOSE_MERK);
	INT(msg[1]) = merk_id;
	post(player, std::move( msg ));
	return true;
}
bool tcp_server::change_weapon(Player::ptr player, int merk_id, char weapon_id) {
	if (!player->merk_exist(merk_id)) {
		return false;
	}
	if (player->is_in_game()) {
		return true; // тут надо корректно поменять оружие в бою
	}
	
	player->get_merk(merk_id)->set_weapon_id(weapon_id);

	Log("Player " + _CLDATA.getClientName(player->get_id())
		+ " (id " + boost::lexical_cast<std::string>(player->get_id()) + ")"
		+ " just changed weapon on merk " + player->get_merk(merk_id)->get_name()
		+ " to " + _DATA.getWeaponString(weapon_id));

/*
Сменить оружие:
	1) CHANGE_WEAPON
	2) merk_id (int)
	3) weapon_id (char)
*/
	std::vector<char> msg(6, 0);
	msg[0] = static_cast<char>(REQUEST_TYPE::CHANGE_WEAPON);
	INT(msg[1]) = merk_id;
	msg[5] = weapon_id;
	post(player, msg);

	return true;
}

void tcp_server::add_client(client_ptr cl) {
	std::string name = cl->username();
	int player_id = _CLDATA.getClientID(name);
	//cl->set_id(player_id);

	Player::ptr player = get_player(player_id);

	if (player.get() == nullptr) {

		player =  Player::create(player_id, cl);

		{
			boost::mutex::scoped_lock lk(m_Mutex);
			//clients[cl->get_id()] = cl;
			//online_clients[cl_id] = true;
			//m_Players[cl_id].set_client(cl);
			//m_Players[cl_id].set_id(cl_id);
			//m_Players[cl_id].set_online(true);
			m_Players[player_id] = player;
		}

		cl->set_player(player);

	} else {
		player->set_client(cl);
		player->set_online(true);

		cl->set_player(player);
	}

	send_merk_list(player);
	send_player_list(player);

	if (player->is_in_game()) {
		resume_in_game(player);
	}

	std::vector<char> msg(sizeof(char)*2 + sizeof(int) + name.size(), 0);
	int i = 0;
	msg[i++] = static_cast<char>(REQUEST_TYPE::NEW_PLAYER);

	INT(msg[i]) = player_id;
	i += sizeof(int);

	msg[i++] = name.size();
	memcpy_s(&msg[i], name.size(), name.c_str(), name.size());
	i += name.size();

	if (i != sizeof(char)*2 + sizeof(int) + name.size()) {
		int a = 5;
	}

	post_all(msg, cl);

	Log("Client " + cl->get_address_string() + " was logged in as "
		+ cl->username() + " (id " + boost::lexical_cast<std::string>(player_id) + ")");
	//if (!client_reconnected(cl)) create_start_unit(cl);

	//if (m_bGameStarted) {
	//	post(char(NEW_PLAYER) + (char(cl->username().size()) + cl->username())
	//		+ (char)_CLDATA.getClientID(cl->username()), cl);// + unit_create_string);
	//}
}
void tcp_server::drop_client(client_ptr cl) {

	Log("Droping client " + cl->get_address_string() + " ("
		+ cl->username() + ", id " + boost::lexical_cast<std::string>(cl->get_id()) + ")");

	//int cl_id = _CLDATA.getClientID(cl->username());
	//Player::ptr p = m_Players[cl->get_id()];
	Player::ptr p = cl->get_player();

	//{
	//	boost::mutex::scoped_lock lk(m_Mutex);

	////auto it = std::find(clients.begin(), clients.end(), cl);
	////if (it != clients.end()) clients.erase(it);

	////for(auto iter = clients.begin(); iter != clients.end(); ) {
	////	if (iter->second == cl) {
	////		online_clients[_CLDATA.getClientID(cl->username())] = false;
	////		iter = clients.erase(iter);
	////	} else {
	////		++iter;
	////	}
	////}

	//	//online_clients[cl->get_id()] = false;
	//	p.set_online(false);
	//	p.set_client(nullptr);
	//}

	if (p->is_in_lobby()) {
		//if (p->is_host_of_the_lobby()) {
		//	drop_lobby(p->get_lobby_id());
		//} else {
		//	//p->set_is_in_lobby(false);
		//	drop_from_lobby(p);
		//}

		drop_from_lobby(p);
		
		// TO DO
		// продумать, как правильно отключать игрока:
		// когда оставлять в игре, когда исключать
	}
	if (p->is_in_game()) {
		drop_from_game(p); // может и не получиться, если есть живые персы
	}

	p->set_online(false);
	p->set_client(nullptr);

	if (cl->started()) {
		cl->stop();
	}

	const int N = sizeof(char) + sizeof(int);
	std::vector<char> msg(N, 0);;
	int i = 0;
	msg[0] = static_cast<char>(REQUEST_TYPE::DROP_PLAYER);
	INT(msg[1]) = p->get_id();

	post_all(msg, cl);

	//update_clients_changed();
}

void tcp_server::post_all(std::vector<char> msg, client_ptr except) {
	if (except) {
		int except_id = except->get_id();
		// надо заменить на player_id и смотреть по нему
		//for(auto b = clients.begin(), e = clients.end(); b != e; ++b) {
		//	if (b->second != except) b->second->post(msg);
		//}
		for(auto b = m_Players.begin(), e = m_Players.end(); b != e; ++b) {
			if (b->second->is_online() && b->first != except_id) {
				b->second->get_client()->post(msg);
			}
		}
	} else {
		for(auto b = m_Players.begin(), e = m_Players.end(); b != e; ++b) {
			if (b->second->is_online()) {
				b->second->get_client()->post(msg);
			}
		}
	}
}
void tcp_server::post(int player_id, std::vector<char> msg) {
	//m_Players[player_id]->get_client()->post(msg);
	post(m_Players[player_id], msg);
}
void tcp_server::post(int player_id, char command) {
	//m_Players[player_id]->get_client()->post(command);
	post(m_Players[player_id], command);
}
void tcp_server::post(Player::ptr player, std::vector<char> msg) {
	if (player->is_online()) player->get_client()->post(msg);
}
void tcp_server::post(Player::ptr player, char command) {
	if (player->is_online()) player->get_client()->post(command);
}
bool tcp_server::do_compare(char reg[], const std::string& con) {
	int sz = strlen(reg);
	if (sz != con.size()) {
		return false;
	}
	for (char* ch = reg, k = 0; *ch; ++ch, ++k) {
		if (std::tolower(con[k]) != std::tolower(*ch)) {
			return false;
		}
	}
	return true;
}

bool tcp_server::client_registered(std::string& login) {
	auto id = _CLDATA.getClientID(login);

	if (id == -1) return false;

	login = _CLDATA.getClientName(id);

	return true;
}
bool tcp_server::already_logged_in(const std::string& login) {
	//for( auto b = clients.begin(), e = clients.end(); b != e; ++b)
	//	if (b->second->username() == login) {
	//		return true;
	//	}

	int cl_id = _CLDATA.getClientID(login);

	if (m_Players.find(cl_id) == m_Players.end()) {
		return false;
	}

	return m_Players[cl_id]->is_online();

/*
	Если сделать vector<bool> logged_in_clients размерности базы все существующих клиентов,
	а это в случае с bool в любом случае будет крайне мало по занимаемому объему данных,
	то можно просто проверять logged_in_clients[client_id]
*/
	//return false;
}

GameMap::ptr tcp_server::get_game(int game_id) {
	auto it = m_Games.find(game_id);
	if (it == m_Games.end()) {
		return nullptr;
	}
	return it->second;
}
Lobby::ptr tcp_server::get_lobby(int lobby_id) {
	//return m_Lobbies[lobby_id];
	auto it = m_Lobbies.find(lobby_id);
	if (it == m_Lobbies.end()) {
		return nullptr;
	}
	return it->second;
}
Player::ptr tcp_server::get_player(int player_id) {
	//return m_Players[player_id];
	auto it = m_Players.find(player_id);
	if (it == m_Players.end()) {
		return nullptr;
	}
	return it->second;
}