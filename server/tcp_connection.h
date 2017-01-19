#pragma once
#ifndef _TCP_CONNECTION_H_
#define _TCO_CONNECTION_H_

#define MEM_FN(x)			boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)		boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)		boost::bind(&self_type::x, shared_from_this(),y,z)
#define MEM_FN3(x,y,z,u)	boost::bind(&self_type::x, shared_from_this(),y,z,u)

#define ONLY_IMPORTANT_LOG
//#define CHECK_PING
#define TIME_TO_PING 500

#include <ctime>

class tcp_server;
class Player;

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>, boost::noncopyable {

	// check_ping result
	enum CP_RESULT {
		CP_OK,
		CP_TOO_BIG = 5000
	};

	typedef talk_to_client self_type;
	typedef boost::shared_ptr<Player> Player_ptr;

	talk_to_client(boost::asio::io_service& service);

public:

	~talk_to_client();

	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<talk_to_client> ptr;

	void start();
	static ptr new_(boost::asio::io_service&);
	void stop();

	std::string get_address_string() const;

	bool post(std::vector<char>&& msg);
	bool post(const std::vector<char>& msg);
	bool post(char command);

	bool started() const { return started_; }
	bool logged_in() const { return loggedin_; }
	int get_id() const;
	void set_id(int id);
	Player_ptr get_player() const;
	void set_player(Player_ptr p);
	//bool is_active() const { return m_bIsActive; }
	//void set_active(bool active);
	boost::asio::ip::tcp::socket & sock() { return sock_;}
	std::string username() const;
	//void set_clients_changed() { clients_changed_ = true; }

private:
	
	void log(const std::string& msg, const error_code& e) const;
	void log(const std::string& msg, bool is_important = false) const;

	void write(std::vector<char> msg);
	//void write(std::vector<char>&& msg);

	void on_write(const error_code & err, size_t bytes, bool disconnect_after_writing = false);
	void on_read(const error_code & err, size_t bytes);
	void on_read_size(const error_code & err);

	void on_login(std::vector<char> & msg);
	void on_ping();
	void on_clients();

	void check_out_message();

	void parse_message(const std::vector<char>& msg);
	bool parse_create(const std::vector<char>& msg);
//	bool parse_login(const std::vector<char>& msg);
	bool parse_create_union(const std::vector<char>& msg);
	bool parse_shoot(const std::vector<char>& msg);
	bool parse_move(const std::vector<char>& msg);
	bool parse_turn_end(const std::vector<char>& msg);
	bool parse_create_lobby(const std::vector<char>& msg);
	bool parse_game_start(const std::vector<char>& msg);
	bool parse_req_name(const std::vector<char>& msg);
	bool parse_drop_from_lobby(const std::vector<char>& msg);
	bool parse_join_lobby(const std::vector<char>& msg);
	bool parse_players_in_lobby(const std::vector<char>& msg);
	bool parse_player_list(const std::vector<char>& msg);
	bool parse_lobby_list(const std::vector<char>& msg);
	bool parse_change_map(const std::vector<char>& msg);
	bool parse_create_merk(const std::vector<char>& msg);
	bool parse_delete_merk(const std::vector<char>& msg);
	bool parse_merk_list(const std::vector<char>& msg);
	bool parse_choose_merk(const std::vector<char>& msg);
	bool parse_change_weapon(const std::vector<char>& msg);
	bool parse_exit_game(const std::vector<char>& msg);


	void ping();
	void do_ping();
	void do_ask_clients();
	void do_add_outcoming_msg(const std::vector<char>& msg);
	void do_add_outcoming_msg(std::vector<char>&& msg);

	void client_is_bastard();
	void post_check_ping(int time_to_disconnect = 30);
	void disconnect_timer_handler(const boost::system::error_code&);

	CP_RESULT check_ping();

	void do_read(bool only_size = true);
	void do_write(std::vector<char> msg, bool disconnect_after = false);
	//void do_write(std::string && msg, bool disconnect_after = false);
	size_t read_complete(const error_code & err, size_t bytes);

private:

	boost::asio::ip::tcp::socket sock_;
	enum { max_msg = 1024*10 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	bool loggedin_;
	std::string username_;
	boost::asio::deadline_timer m_DisconnectTimer;
	boost::asio::deadline_timer m_Timer;
	clock_t last_sended;
	//boost::posix_time::ptime av_ping;
	clock_t last_ping;
	//bool clients_changed_;
	//int id_; // id игрока
	Player_ptr m_Player;

	std::deque<std::vector<char>> m_OutcomingMsgs;
	boost::mutex m_Mutex;
};

#endif
