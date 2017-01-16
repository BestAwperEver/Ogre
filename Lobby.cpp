#include "StdAfx.h"
#include "Lobby.h"
#include "Logger.h"
#include <boost/lexical_cast.hpp>


Lobby::Lobby(void)
	: m_ID(-1)
	, m_MapID(-1)
	, m_HostID(-1)
	, m_bGameStarted(false)
{
}

Lobby::Lobby(int host_id, char map_id, int lobby_id)
	: m_ID(lobby_id)
	, m_MapID(map_id)
	, m_HostID(host_id)
	, m_bGameStarted(false)
{
	add_player(host_id);
}

Lobby::~Lobby(void)
{
}

bool Lobby::add_player(int player_id) {
	if (std::find(m_Players.begin(), m_Players.end(), player_id) == m_Players.end()) {
		m_Players.push_back(player_id);
		return true;
	}
	else {
		return false;
	}
}
bool Lobby::remove_player(int player_id) {
	if (std::find(m_Players.begin(), m_Players.end(), player_id) != m_Players.end()) {
		m_Players.remove(player_id);
		return true;
	} else {
		return false;
	}
}