#pragma once
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#define _SERVER tcp_server::getInstance()

#define ENABLE_LOG
#define ENABLE_CONSOLE_NET_LOG

#include <vector>
#include <map>

#include "..\..\DataBase0\DataBase0\CellCoordinates.h"
#include "tcp_connection.h"
#include "GameMap.h"
#include "lobby.h"
//#include "Hero.h"
//#include "..\..\DataBase0\DataBase0\DataBase.h"
#include "..\..\DataBase0\DataBase0\ClientDataBase.h"

class tcp_server
{
	//friend talk_to_client;
	//friend GameMap;
	//friend Lobby;
	//friend Player;

	typedef boost::shared_ptr<talk_to_client> client_ptr;
	typedef tcp_server self_type;
//	typedef std::vector<client_ptr> array;
	typedef talk_to_client client;

	void handle_timer(const boost::system::error_code&);	
	void handle_accept(client::ptr, const boost::system::error_code&);
	

	tcp_server(unsigned short Port);
	~tcp_server();

	bool do_compare(char registered_user[], const std::string& connected_user);

	bool client_reconnected(client_ptr);

	//inline bool do_add_hero(std::string Name, CellCoordinates where, char type_id, char Class, char Weapon, char Armor, int player_id,
	//	char HP, char AP, CellCoordinates looks_to);
	//inline bool do_add_unit(CellCoordinates where, char type_id, char Class, char Weapon, char Armor, int player_id,
	//	char HP, char AP, CellCoordinates looks_to);
	//inline bool do_move_unit(std::vector<CellCoordinates>& Path, bool& unblock_after);
	//inline bool do_add_union(std::list<int> new_union);

	//inline short calculate_damage(CellCoordinates who_is_shooting, CellCoordinates to);
	//std::string create_hero_msg(client_ptr cl, UnitDef h,
	//	CellCoordinates where, CellCoordinates looks_to);
	//void create_start_unit(client_ptr);
	//void remove_unit(AbstractUnit*);

	//void restore_active_points(int player_id, bool post_message = true);
	//bool next_turn(bool post_message = true);

	//GameMap m_GameMap;

	int m_GamesCount; // это не количество, а последний заюзанный айдишник
	int m_LobbiesCount;
	std::map<int, GameMap::ptr> m_Games;
	std::map<int, Lobby::ptr> m_Lobbies;
	std::map<int, Player::ptr> m_Players;
	std::map<int, std::string> m_Maps; // должно быть в базе

	std::map<int, GameMap::ptr>& games() { return m_Games; }
	std::map<int, Lobby::ptr>& lobbies() { return m_Lobbies; }
	std::map<int, Player::ptr>& players() { return m_Players; }
	std::map<int, std::string>& maps() { return m_Maps; }

	//std::list<std::list<unsigned char>> m_Unions; // теперь внутри GameMap
	//std::map<int, client_ptr> clients;
	
	//std::vector<bool> online_clients;

	//bool m_bGameStarted;

	void Log(std::string msg);
	//bool createGameLogFile();

public:

	static tcp_server& getInstance();

	//bool add_hero(std::string Name, CellCoordinates where, char type_id, char Class, char Weapon, char Armor, int player_id,
	//	char HP, char AP, CellCoordinates looks_to);
	//bool add_unit(CellCoordinates where, char type_id, char Class, char Weapon, char Armor, int player_id,
	//	char HP, char AP, CellCoordinates looks_to);
	//bool move_unit(std::vector<CellCoordinates>& Path, bool& unblock_after);
	//bool shoot(CellCoordinates from, CellCoordinates to, bool& unblock_after);
	//bool add_union(std::list<int> new_union);
	//bool force_next_turn(unsigned int client_id, bool post_message = true);

	//std::string get_map_create_msg();
	//inline std::string get_move_string(std::vector<CellCoordinates>& Path);
	//char get_team(const std::string& username);

	void start();
	//void start_game();
	//bool game_started() const {return m_bGameStarted;}

	GameMap::ptr get_game(int game_id);
	Lobby::ptr get_lobby(int lobby_id);
	Player::ptr get_player(int player_id);

	void post_all(std::vector<char> msg, client_ptr except = nullptr);
	void post_map(std::vector<char> msg, client_ptr except = nullptr); // вроде нигде не используется
	void post(int player_id, std::vector<char> msg);
	void post(int player_id, char command);
	void post(Player::ptr, std::vector<char> msg);
	void post(Player::ptr, char command);

	void add_client(client_ptr);
	void drop_client(client_ptr);
	void update_clients_changed();
	//std::string getClientListStr();
	bool client_registered(std::string& login);
	bool already_logged_in(const std::string& login);

	bool start_game(int lobby_id, int player_id);
	bool start_game(Player::ptr);
	//GameMap* add_game(int map_id);
	GameMap::ptr add_game(GameMap::ptr);
	bool drop_game(int game_id);

	Lobby::ptr add_lobby(int host_id, char map_id);
	Lobby::ptr add_lobby(Player::ptr, char map_id);

	bool drop_lobby(int lobby_id);
	bool drop_lobby(Lobby::ptr);

	bool join_lobby(int player_id, int lobby_id);
	bool join_lobby(Player::ptr, int lobby_id);

	bool send_players_in_lobby_list(int player_id, int lobby_id);
	bool send_players_in_lobby_list(Player::ptr, int lobby_id);
	bool send_player_list(Player::ptr);
	bool send_lobby_list(Player::ptr);
	bool send_merk_list(Player::ptr);

	void do_post_players_in_lobby(Player::ptr, Lobby::ptr);

	bool drop_from_lobby(int lobby_id, int player_id);
	bool drop_from_lobby(Player::ptr);
	bool drop_from_game(Player::ptr);
	bool resume_in_game(Player::ptr);

	bool change_map(Player::ptr, char map_id);

	bool create_merk(Player::ptr, std::string name, UnitDef ud);
	bool delete_merk(Player::ptr, int merk_id);
	bool choose_merk(Player::ptr, int merk_id);
	bool change_weapon(Player::ptr, int merk_id, char weapon_id);

private:
	boost::asio::deadline_timer m_Timer;
	//boost::asio::deadline_timer m_TurnTimer;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	boost::mutex m_Mutex;
	//boost::mutex m_LogMutex;
//	array clients;
	//bool m_bLogFileCreated;
	//std::ofstream log_file;
};

#endif
