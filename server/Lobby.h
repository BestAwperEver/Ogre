#pragma once
#ifndef _LOBBY_H_
#define _LOBBY_H_

#include "GameMap.h"
#include "Player.h"
#include "tcp_connection.h"
#include <list>

class tcp_server;

class Lobby: public boost::enable_shared_from_this<Lobby>, public boost::noncopyable
{
	typedef boost::shared_ptr<talk_to_client> client_ptr;

	boost::asio::io_service* m_pService;

	int m_ID;
	char m_MapID;
	//GameMap* m_pMap;
	GameMap::ptr m_pMap;
	std::map<int, Player::ptr> m_Players;
	//int m_HostID;
	Player::ptr m_Host;
	bool m_bGameStarted;
	
	// количество игроков в командах
	int m_Team1;
	int m_Team2;

	void post_all(std::vector<char>&& msg, int except_player = 0);
	void post_all(const std::vector<char>& msg, int except_player = 0);

	Lobby(boost::asio::io_service* service, int lobby_id, Player::ptr host, int map_id);

public:

	typedef boost::shared_ptr<Lobby> ptr;

	static Lobby::ptr create(boost::asio::io_service* service, int lobby_id, Player::ptr host, int map_id = -1);

	//Lobby(boost::asio::io_service* service, int lobby_id, int host_id = -1, int map_id = -1);
	//Lobby();
	//Lobby(Lobby&&);
	~Lobby(void);

	void operator = (Lobby&&);

	bool game_started() const {return m_bGameStarted;}

	bool change_map(int map_id);
	bool add_player(int player_id);
	bool add_player(Player::ptr);
	bool remove_player(int player_id);
	void remove_all_players();
	void do_remove_player(int player_id);
	void do_remove_player(Player::ptr);
	//bool start_game();
	GameMap::ptr get_game(int game_id);
	bool has_players() const { return !m_Players.empty(); }
	std::map<int, Player::ptr>& get_players() { return m_Players; }


	void set_map_id(char map_id) { m_MapID = map_id; }
	char get_map_id() const { return m_MapID; }

	void set_id(int lobby_id) { m_ID = lobby_id; }
	int	get_id() const { return m_ID; }

	void set_host(Player::ptr host) { m_Host = host; }
	Player::ptr get_host() const { return m_Host; }
	int get_host_id() const;

	void change_team(int player_id, char team);
	void change_team(Player::ptr, char team);
};

#endif

