#pragma once
#ifndef _LOBBY_H_
#define _LOBBY_H_

#include <list>

class Lobby
{
	int m_ID;
	char m_MapID;
	int m_HostID;

	std::list<int> m_Players;

	// количество игроков в командах
	int m_Team1;
	int m_Team2;

	bool m_bGameStarted;

public:
	Lobby(void);
	Lobby(int host_id, char map_id, int lobby_id);
	~Lobby(void);

	void set_id(int lobby_id) { m_ID = lobby_id; }
	int get_id() const { return m_ID; }
	
	void set_host_id(int host_id) { m_HostID = host_id; }
	int get_host_id() const { return m_HostID; }

	void set_map_id(char map_id) { m_MapID = map_id; }
	char get_map_id() const { return m_MapID; }

	void set_game_started(bool started) { m_bGameStarted = started; }

	bool add_player(int player_id);
	bool remove_player(int player_id);
	void remove_players() { m_Players.clear(); }
	std::list<int>& get_players() { return m_Players; }

	void change_team(int player_id, char team);

};

#endif
