#include "tcp_connection.h"
#include "Logger.h"
using namespace boost::asio;

//bool talk_to_svr::m_bEnableLog = ENABLE_NET_LOG;
//void talk_to_svr::enableLog(bool enable) {
//	m_bEnableLog = enable;
//}

bool talk_to_svr::m_bOnlyImportant = ONLY_IMPORTANT_MESSAGES_TO_LOG;

Ogre::uint32 talk_to_svr::m_MinPing = 60;
Ogre::uint32 talk_to_svr::m_MaxPing = 90;

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

talk_to_svr::talk_to_svr(io_service& service, OgreConsole<myapp>* console_) 
	: sock_(service)
	, m_bStarted(false)
	//, username_(username)
	, timer_(service)
	, m_timer(service)
	, console(console_)
	, m_bIsReading(false)
	, m_bIsWriting(false)
	, m_bConnected(false)
	, last_error("No error")
	, disconnect_error("No error")
	//, m_bLoggedIn(false)
	, m_Sended(0)
	, m_Recieved(0)
	//, m_PlayerID(0)
	, m_Player(0)
{}
talk_to_svr::ptr talk_to_svr::start(io_service& service, ip::tcp::endpoint ep
	, const std::string & username, OgreConsole<myapp>* console_) 
{
	ptr new_(new talk_to_svr(service, console_));
	new_->start(ep, username);
	return new_;
}
void talk_to_svr::start(ip::tcp::endpoint ep, const std::string & username) {
	if (m_bStarted) {
#ifdef ENABLE_NET_LOG
		log("Can't start a connection to the server: connection already exists", true);
#endif
		//_GAMELOG("Can't start a connection to the server: connection already exists");
		return;
	}
#ifdef ENABLE_NET_LOG
	log("start");
#endif
	//_GAMELOG("Trying to connect to the server");
	f_log("Trying to connect to " + ep.address().to_string() + ":" + boost::to_string(ep.port()));
	//_LOG("Trying to connect to " + ep.address().to_string() + ":" + boost::to_string(ep.port()));

	username_ = username;
	m_bStarted = true;
	m_timer.expires_from_now( boost::posix_time::seconds(10));
	m_timer.async_wait( MEM_FN2( handle_timer, "connection failed", _1));
	sock_.async_connect(ep, MEM_FN1(on_connect,_1));
}
void talk_to_svr::stop() {
	if ( !m_bStarted ) {
#ifdef ENABLE_NET_LOG
		log("can't stop the connection: already stopped", true);
#endif
		//_GAMELOG("can't stop the connection: already stopped");
		return;
	}
	disconnect_error = last_error;
#ifdef ENABLE_NET_LOG
	log("stop", true);
#endif
	//_GAMELOG("stopping the connection");
	//m_bLoggedIn = false;
	m_bStarted = false;
	m_IncomingMsgs.clear();
	m_OutcomingMsgs.clear();
	try {
		sock_.close();
	}
	catch (const boost::system::error_code& e) {
		std::cerr << "Wtf?! I just tryed to close my socket!" << e.message() << std::endl;
	}
	if (m_bConnected) {
		m_bConnected = false;
		f_log("Connection closed");
	}

	if (m_Player) m_Player->reset();

	m_timer.cancel();
	timer_.cancel();
}

void talk_to_svr::setEnableUnimportantMessages(bool enable) {
	m_bOnlyImportant = !enable;
}
bool talk_to_svr::getEnableUnimportantMessages() {
	return !m_bOnlyImportant;
}

void talk_to_svr::set_emulate_ping(uint min, uint max) {
	m_MinPing = min;
	m_MaxPing = max;
}
void talk_to_svr::get_emulate_ping(uint& min, uint& max) {
	min = m_MinPing;
	max = m_MaxPing;
}

bool talk_to_svr::incoming(std::vector<char>& msg) {
	if (m_IncomingMsgs.size()) {
		msg = m_IncomingMsgs.front();
		do_delete_incoming_msg();
		return true;
	}
	return false;
}
bool talk_to_svr::post(const std::vector<char>& msg) {
	if (m_bConnected) {
		do_add_outcoming_msg(msg);
		return true;
	}
	return false;
}

void talk_to_svr::log(const std::string& msg, const boost::system::error_code& error) {
#ifndef ENABLE_NET_LOG
	return;
#endif
	//if ( !console ) {
	//	return;
	//}
	last_error = error.message();
	std::stringstream out;
	//out << msg << " (" << username_ << ") " << error << std::endl;
	out << "Connection: " << msg << ' ' << error;// << std::endl;
	//console->print(out.str());
	{
		boost::mutex::scoped_lock lk(m_Mutex);
#ifdef ENABLE_CONSOLE_NET_LOG
		_LOG(out.str());
#else
		_GAMELOG(out.str());
#endif
	}
}
void talk_to_svr::log(const std::string& msg, bool is_important) {
#ifndef ENABLE_NET_LOG
	return;
#endif
	//if ( !console ) {
	//	return;
	//}
#ifdef ONLY_IMPORTANT_MESSAGES_TO_LOG
	if (is_important) {
#endif
		//std::stringstream out;
		//out << msg << " (" << username_ << ")" << std::endl;
		//console->print(msg);
		{
			boost::mutex::scoped_lock lk(m_Mutex);
#ifdef ENABLE_CONSOLE_NET_LOG
			_LOG("Connection: " + msg);
#else
			_GAMELOG("Connection: " + msg);
#endif
		}
#ifdef ONLY_IMPORTANT_MESSAGES_TO_LOG
	}
#endif
}
void talk_to_svr::f_log(const std::string& msg) {
	//Ogre::LogManager::getSingletonPtr()->logMessage(msg);
	boost::mutex::scoped_lock lk(m_Mutex);
	_LOG("Connection: " + msg);
}

void talk_to_svr::handle_timer(const std::string& error_msg, const boost::system::error_code& e) {
	if (e) {
		if (e.value() != 995) {
#ifdef ENABLE_NET_LOG
			log("handle_timer", e);
#endif
			//_GAMELOG("Connection: handle_timer " + e.message);
		} else {
#ifdef ENABLE_NET_LOG
			log("timer cancelled");
#endif
			//_GAMELOG("Connection: timer cancelled");
		}
		return;
	}
#ifdef ENABLE_NET_LOG
	log("handle_timer");
#endif
	f_log(error_msg);
	stop();
}

void talk_to_svr::on_connect(const error_code & err) {	
	if ( !err) {
		m_bConnected = true;
#ifdef ENABLE_NET_LOG
		log("on_connect");
#endif
		//_GAMELOG("Connection: on_connect");
		m_timer.cancel();
		login();
	}
	else {
		m_bConnected = false;
#ifdef ENABLE_NET_LOG
		log("on_connect", err);
#endif
		//_GAMELOG("Connection error: on_connect " + err.message);
		f_log("can't connect to the server");
		last_error = "can't connect";
		stop();
	}
}
void talk_to_svr::on_read_size(const error_code & err) {
	m_timer.cancel();
	m_Recieved += 2;
	if ( err) {
#ifdef ENABLE_NET_LOG
		log("on_read_size", err);
#endif
		stop();
		return;
	}
	if ( !started() ) {
#ifdef ENABLE_NET_LOG
		log("on_read_size: return, isn't started", true);
#endif
		return;
	}
	if (*((short*)read_buffer_) > max_in_msg) {
		f_log("too big message size: " + boost::to_string(*((short*)read_buffer_)));
		server_is_bastard();
		return;
	}
	do_read(false);
}
void talk_to_svr::on_read(const error_code & err, size_t bytes) {
	m_bIsReading = false;
	m_timer.cancel();
	m_Recieved += bytes;
	if ( err) {
#ifdef ENABLE_NET_LOG
		log("on_read", err);
#endif
		stop();
		return;
	}

	if ( !started() ) {
#ifdef ENABLE_NET_LOG
		log("on_read: return, isn't started", true);
#endif
		return;
	}
#ifdef ENABLE_NET_LOG
	log("on_read");
#endif

	check_out_message();

	if (bytes == 3 && *read_buffer_ == PING) on_ping(*((short*)(read_buffer_+1))); else {

		if (read_buffer_[bytes-1] != END_MESSAGE) {
			f_log("Message is not ended by END_MESSAGE");
			server_is_bastard();
			return;
		}
		//std::string msg(read_buffer_, bytes);
		add_incoming_message(std::vector<char>(read_buffer_, read_buffer_ + bytes));

	}
	//if ( msg.find("ping") == 0) on_ping(msg);
	//else if ( !m_bLoggedIn && msg.find("login ") == 0) on_login(msg);
	//else if ( msg.find("clients ") == 0) on_clients(msg);
	//else add_incoming_message(msg);//std::cerr << "invalid msg " << msg << std::endl;

}
//void talk_to_svr::on_login(std::vector<char>& msg) {
//	msg.back() = ' ';
//	std::istringstream in(msg);
//	std::string answer;
//	int player_id;
//	in >> answer >> answer;
//	if (answer == "ok") {
//		in >> username_;
//		in >> PlayerID;
//		f_log("Succesfully logged in as: " + username_ );
//		m_PlayerID = player_id;
//		m_bLoggedIn = true;
//	} else if (answer == "failed") {
//		f_log("Incorrect login data");
//		last_error = "incorrect login";
//		stop();
//	} else if (answer == "already") {
//		f_log("Login failed: user with name " + username_ + " is already logged in");
//		last_error = "already logged in";
//		stop();
//	}
//	//do_ask_clients();
//	//check_out_message();
//}
void talk_to_svr::on_ping(short ping) {//std::vector<char>& msg) {
#ifdef ENABLE_NET_LOG
	log("on_ping " + boost::to_string(ping));
#endif
	//msg.back() = ' ';
	//std::istringstream in(msg);
	//std::string answer;
	//in >> answer >> answer;
	//if ( answer == "client_list_changed") do_ask_clients();
	//else postpone_ping();
	//check_out_message();
	//if ( answer == "high") {
	if (ping < 0) {
		f_log("disconnected due to a high ping");
		stop();
		return;
	}
	m_Ping = ping;
}
//void talk_to_svr::on_clients(const std::vector<char> & msg) {
//	std::string clients = msg.substr(8);
//#ifdef ENABLE_NET_LOG
//	log("new client list: " + clients);
//#endif
//	postpone_ping();
//}
void talk_to_svr::on_write(const error_code & err, size_t bytes) {
	m_bIsWriting = false;
	m_Sended += bytes;
	if (!err) {
#ifdef ENABLE_NET_LOG
		log("on_write");
#endif
		do_read();
	}
	else {
#ifdef ENABLE_NET_LOG
		log("on_write", err);
#endif
		stop();
	}
}

void talk_to_svr::add_incoming_message(std::vector<char>& msg) {
#ifdef ENABLE_NET_LOG
	log("New message from server");
#endif
	boost::mutex::scoped_lock lk(m_Mutex);
	m_IncomingMsgs.push_back( std::move(msg) );
	//check_out_message();
}
void talk_to_svr::check_out_message() {
#ifdef ENABLE_NET_LOG
	log("check_out_message");
#endif
	boost::mutex::scoped_lock lk(m_Mutex);
	if (!m_OutcomingMsgs.empty()) {
		write(m_OutcomingMsgs.front());
		m_OutcomingMsgs.pop_front();
	} else {
		ping();
	}
}
void talk_to_svr::server_is_bastard() {
	log("server is damn bastard", true);
	stop();
}
void talk_to_svr::ping() {
#ifdef ENABLE_NET_LOG
	log("ping");
#endif
	//char req = PING;
	write(std::vector<char>(1, PING), true);
}
void talk_to_svr::postpone_ping() {
	int millis; 
	if (m_MaxPing == m_MinPing) millis = m_MinPing;
	else millis = m_MinPing + rand() % (m_MaxPing - m_MinPing);

	timer_.expires_from_now(boost::posix_time::millisec(millis));
	timer_.async_wait( MEM_FN( ping ));
#ifdef ENABLE_NET_LOG
	std::stringstream out;
	out << username_ << " postponing ping " << millis << " ms";
	if (millis > 5000) {
		log(out.str(), true);
	} else {
		log(out.str());
	}
#endif
	//do_ping();
}
void talk_to_svr::write(const std::vector<char> & msg, bool ping) {
	if ( !m_bStarted) {
#ifdef ENABLE_NET_LOG
		log("write: return, isn't started", true);
#endif
		return;
	}
#ifdef ENABLE_NET_LOG
	//log("writing " + msg);
	log("write");
#endif
	//std::string len_str(sizeof(short),0);
	char len_str[sizeof(short) + 1] = {0};

	short size;
	if (ping) size = 1;
	else size = msg.size() + 1;

	std::memcpy(len_str, &size, sizeof(short));

#ifdef EMULATE_PING
	int millis; 
	if (m_MaxPing == m_MinPing) millis = m_MinPing;
	else millis = m_MinPing + rand() % (m_MaxPing - m_MinPing);

	timer_.expires_from_now(boost::posix_time::millisec(millis));
	if (ping) {

		std::vector<char> final_msg(sizeof(short) + msg.size(), 0);
		memcpy(&final_msg[0], &size, sizeof(short));
		memcpy(&final_msg[sizeof(short)], &msg[0], msg.size());

		//timer_.async_wait( MEM_FN1(do_write, len_str + msg));
		timer_.async_wait( MEM_FN1(do_write, std::move( final_msg )));
	} else {

		std::vector<char> final_msg(sizeof(short) + msg.size() + sizeof(char), 0);
		memcpy(&final_msg[0], &size, sizeof(short));
		memcpy(&final_msg[sizeof(short)], &msg[0], msg.size());
		final_msg[sizeof(short) + msg.size()] = REQUEST_TYPE::END_MESSAGE;

		//timer_.async_wait( MEM_FN1(do_write, len_str + msg + char(END_MESSAGE)));
		timer_.async_wait( MEM_FN1(do_write, std::move( final_msg )));
	}
#else
	if (ping) {

		std::vector<char> final_msg(sizeof(short) + msg.size(), 0);
		memcpy(&final_msg[0], &size, sizeof(short));
		memcpy(&final_msg[sizeof(short)], &msg[0], msg.size());
				
		do_write(std::move( final_msg ));
	} else {

		std::vector<char> final_msg(sizeof(short) + msg.size() + sizeof(char), 0);
		memcpy(&final_msg[0], &size, sizeof(short));
		memcpy(&final_msg[sizeof(short)], &msg[0], msg.size());
		final_msg[sizeof(short) + msg.size()] = REQUEST_TYPE::END_MESSAGE;

		do_write(std::move( final_msg ));
	}
#endif
}
void talk_to_svr::login() {
	std::vector<char> login_msg;
	login_msg.push_back(LOGIN);
	login_msg.push_back(username_.size());
	login_msg += username_;
	write(login_msg);
}

void talk_to_svr::do_delete_incoming_msg() {
	boost::mutex::scoped_lock lk(m_Mutex);
	m_IncomingMsgs.pop_front();
}
void talk_to_svr::do_add_outcoming_msg(const std::vector<char>& msg) {
	boost::mutex::scoped_lock lk(m_Mutex);
	m_OutcomingMsgs.push_back(msg);
}
//void talk_to_svr::do_ask_clients() {
//#ifdef ENABLE_NET_LOG
//	log("do_ask_clients");
//#endif
//	do_write("ask_clients\n");
//}
void talk_to_svr::do_read(bool only_size) {
	if ( !m_bStarted) {
#ifdef ENABLE_NET_LOG
		log("do_read: return, isn't started");
#endif
		return;
	}
#ifdef ENABLE_NET_LOG
	log("do_read");
#endif
	m_bIsReading = true;

	if (only_size) {
#ifdef CHECK_PING
		m_timer.expires_from_now( boost::posix_time::seconds(MAX_SERVER_RESPONDONG_TIME) );
		m_timer.async_wait( MEM_FN2( handle_timer, "Server is not responding", _1 ));
#endif
		async_read(sock_, buffer(read_buffer_), 
			//MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
			transfer_exactly(sizeof(short)), MEM_FN1(on_read_size, _1));
	} else {
#ifdef CHECK_PING
		m_timer.expires_from_now( boost::posix_time::seconds(5) );
		m_timer.async_wait( MEM_FN2( handle_timer, "Server is not responding", _1 ));
#endif
		async_read(sock_, buffer(read_buffer_), 
			transfer_exactly(*((short*)(read_buffer_))), MEM_FN2(on_read,_1,_2));
	}
	//m_timer.expires_from_now( boost::posix_time::seconds(MAX_SERVER_RESPONDONG_TIME) );
	//m_timer.async_wait( MEM_FN2( handle_timer, "Server is not responding", _1 ));
	//async_read(sock_, buffer(read_buffer_), 
	//	MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
}
void talk_to_svr::do_write(const std::vector<char> & msg) {
#ifdef _USE_ASSERTS_
	assert( msg.size() < max_out_msg );
#endif
	std::copy(msg.begin(), msg.end(), write_buffer_);
	m_bIsWriting = true;
	//sock_.async_write_some( buffer(write_buffer_, msg.size()), 
	//	MEM_FN2(on_write,_1,_2));
	async_write(sock_, buffer(write_buffer_, msg.size()), transfer_exactly(msg.size()), MEM_FN2(on_write,_1,_2));
}

size_t talk_to_svr::read_complete(const boost::system::error_code & err, size_t bytes) {
	if ( err) return 0;
	bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
	// we read one-by-one until we get to enter, no buffering
	return found ? 0 : 1;
}