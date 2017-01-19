#include "StdAfx.h"
#include "Player.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include "GameMap.h"
#include "lobby.h"

#define INT(x) *( (int*)(&x) )
#define UD(x) *( (UnitDef*)(&x) )

Player::ptr Player::create(int player_id, client_ptr cl, bool online) {
	Player::ptr new_player(new Player(player_id, cl, online));
	new_player->load_merks();
	return new_player;
}
Player::Player(int player_id, client_ptr cl, bool online)
	: m_ID(player_id)
	, m_Team(0)
	, m_Lobby(0)
	, m_GameMap(0)
	, m_cl(cl) 
	, m_bIsOnline(online)
	, m_bIsActive(false)
	, m_bIsInLobby(false)
	, m_bIsInGame(false)
	, m_bHasUnits(false)
	, m_UnitsOnMapCount(0)
	, m_pCurMerk(0)
	, m_MaxMerkID(0)
{
}

//
//Player::~Player(void)
//{
//}

void Player::load_merks() {
	auto merks = _CLDATA.getClientMerks(m_ID);
	m_MaxMerkID = merks.second;
	m_Merks = std::move( merks.first );

	set_merk_id(m_Merks.begin()->first);
}

void Player::set_is_in_game(bool in_game) { 
	m_bIsInGame = in_game; 
	if (!in_game) {
		m_GameMap = nullptr;
		m_UnitsOnMapCount = 0;
		set_has_units(false);
	}
}

void Player::set_is_in_lobby(bool in_lobby) { 
	m_bIsInLobby = in_lobby; 
	if (!in_lobby) {
		m_Lobby = nullptr;
	}
}

void Player::loose_unit() {
	if (--m_UnitsOnMapCount == 0) {
		set_has_units(false);
	}
}
void Player::new_unit() {
	if (m_UnitsOnMapCount++ == 0) {
		set_has_units(true);
	}
}

void Player::set_online(bool online) {
	m_bIsOnline = online;
	if (!online) {
		_CLDATA.saveClientMerks(m_ID, m_Merks);
	}
}
void Player::set_lobby_id(int lobby_id) {
	//m_LobbyID = lobby_id;
	m_Lobby = _SERVER.get_lobby(lobby_id);
	m_bIsInLobby = m_Lobby.get();
}
void Player::set_game_id(int game_id) {
	//m_GameID = game_id;
	m_GameMap = _SERVER.get_game(game_id);
	m_bIsInGame = m_GameMap.get();
}

void Player::set_lobby(Lobby::ptr lobby) {
	//m_LobbyID = lobby_id;
	m_Lobby = lobby;
	m_bIsInLobby = m_Lobby.get();
}
void Player::set_game(GameMap::ptr game) {
	//m_GameID = game_id;
	m_GameMap = game;
	m_bIsInGame = m_GameMap.get();
}

int	Player::get_lobby_id() const {
	return m_Lobby->get_id();
}
int	Player::get_game_id() const {
	return m_GameMap->get_id();
}

bool Player::is_host_of_the_lobby() const {
	return is_in_lobby() && m_Lobby->get_host() == shared_from_this();
}

int Player::get_merk_id() const {
	if (!m_pCurMerk) {
		return -1;
	}
	return m_pCurMerk->get_id();
}
Merk::ptr Player::get_merk() {
	if (!m_pCurMerk) {
		int a = 5;
	}
	return m_pCurMerk;
}
Merk::ptr Player::get_merk(int merk_id) {
	auto it = m_Merks.find(merk_id);
	if (it == m_Merks.end()) {
		return nullptr;
	}
	return it->second;
}

bool Player::set_merk_id(int merk_id){
	auto it = m_Merks.find(merk_id);
	if (it == m_Merks.end()) {
		return false;
	}
	if (is_in_game()) {
		return false;
	}
	m_pCurMerk = it->second;
	return true;
}

bool Player::add_merk(std::string name, UnitDef ud) {
	//auto it = m_Merks.find(merk_id);
	//if (it != m_Merks.end()) {
	//	return false;
	//}
	if (is_in_lobby() || is_in_game()) {
		return false;
	}
	m_Merks[++m_MaxMerkID] = Merk::create(m_MaxMerkID, name, ud);
	//it->second->set_id(merk_id);
	//it->second->set_name(name);
	//it->second->set_ud(ud);
	return true;
}
bool Player::delete_merk(int merk_id) {
	auto it = m_Merks.find(merk_id);

	if (it == m_Merks.end()) {
		return false;
	}

	if (is_in_lobby() || is_in_game()) {
		return false;
	}

	m_Merks.erase(it);
	return true;
}

void Player::send_merk_list() {
	int N = sizeof(char)*2 + (sizeof(int) + sizeof(UnitDef) + sizeof(char) + MAX_NICK_LENGTH)*m_Merks.size();
	std::vector<char> msg(N, 0);
	int i = 0;
	msg[i++] = static_cast<char>(REQUEST_TYPE::MERK_LIST);
	msg[i++] = m_Merks.size();
	for (auto it = m_Merks.begin(), e = m_Merks.end(); it != e; ++it) {
		INT(msg[i]) = it->second->get_id();
		i += sizeof(int);
		UD(msg[i]) = it->second->get_ud();
		i += sizeof(UnitDef);
		std::string name = it->second->get_name();
		msg[i++] = name.size();
		memcpy_s(&msg[i], name.size(), name.c_str(), name.size());
		i += name.size();
	}
	msg.resize(i);
	m_cl->post(msg);
}

//void Player::choose_start_merk() {
//	set_merk_id(m_Merks.begin()->first);
//
//	std::string msg(5, 0);
//	msg[0] = REQUEST_TYPE::CHOOSE_MERK;
//	INT(msg[1]) = m_Merks.begin()->first;
//
//	_SERVER.post(shared_from_this(), std::move( msg ));
//}