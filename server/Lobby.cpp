#include "StdAfx.h"
#include "lobby.h"
#include "tcp_server.h"
#include "Logger.h"

#define INT(x) *( (int*)(&x) )

//extern boost::asio::io_service g_Service;

Lobby::ptr Lobby::create(boost::asio::io_service* service, int lobby_id, Player::ptr host, int map_id) 
{
	Lobby::ptr new_lobby(new Lobby(service, lobby_id, host, map_id));
	new_lobby->add_player(host);
	return new_lobby;
}

Lobby::Lobby(boost::asio::io_service* service, int lobby_id, Player::ptr host, int map_id): 
		m_pService(service)
		, m_ID(lobby_id)
		, m_MapID(map_id)
		, m_pMap(nullptr)
		//, m_HostID(host_id)
		, m_Host(host)
{
	change_map(map_id);

	//this->add_player(host); // ибо возникали ошибки с нулевым shared_from_this


	//req[i] = 0;

	//std::string Req = std::string(req, N-1);
	//_SERVER.players()[host_id]->set_lobby_id(lobby_id);
	//_SERVER.players()[host_id]->set_is_in_lobby(true);
	//m_Players.begin()->second->set_lobby_id(lobby_id);
	//_SERVER.post(host->get_id(), std::string(req, N));
	//_SERVER.post(host_id, Req);
}
//Lobby::Lobby(boost::asio::io_service* service, int lobby_id, int host_id, int map_id): 
//		m_pService(service)
//		, m_ID(lobby_id)
//		, m_MapID(map_id)
//		, m_pMap(nullptr)
//		, m_HostID(host_id)
//{
//	change_map(map_id);
//
//	this->add_player(host_id);
//
//	const int N = sizeof(char) + sizeof(int) + sizeof(char) + sizeof(int);// + 1;
//	char req[N];
//
//	int i = 0;
//	req[i++] = CREATE_LOBBY;
//	*( (int*)(req+i) ) = host_id;
//	i += sizeof(int);
//	req[i++] = map_id;
//	*( (int*)(req+i) ) = lobby_id;
//	i += sizeof(int);
//	//req[i] = 0;
//
//	//std::string Req = std::string(req, N-1);
//	//_SERVER.players()[host_id]->set_lobby_id(lobby_id);
//	//_SERVER.players()[host_id]->set_is_in_lobby(true);
//	m_Players.begin()->second->set_lobby_id(lobby_id);
//	_SERVER.post(host_id, std::string(req, N));
//	//_SERVER.post(host_id, Req);
//}
//Lobby::Lobby(): m_pService(nullptr), m_pMap(nullptr) {}
//Lobby::Lobby(Lobby&& lobby)
//	: m_pService(lobby.m_pService)
//	, m_ID(lobby.m_ID)
//	, m_pMap(lobby.m_pMap)
//	, m_Players( std::move(lobby.m_Players) )
//	, m_Host(lobby.m_Host)
//	, m_bGameStarted(lobby.m_bGameStarted)
//{}
//void Lobby::operator = (Lobby&& lobby) {
//	m_pService = lobby.m_pService;
//	lobby.m_pService = 0;
//	m_ID = lobby.m_ID;
//	lobby.m_ID = -1;
//	m_pMap = lobby.m_pMap;
//	lobby.m_pMap = 0;
//	m_Players = std::move(lobby.m_Players);
//	m_Host = lobby.m_Host;
//	lobby.m_Host = nullptr;
//	m_bGameStarted = lobby.m_bGameStarted;
//	lobby.m_bGameStarted = false;
//}
Lobby::~Lobby(void)
{
	remove_all_players();
}

void Lobby::post_all(std::vector<char>&& msg, int except_player) {
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		if (it->first != except_player) {
			_SERVER.post(it->second, msg);
		}
	}
}
void Lobby::post_all(const std::vector<char>& msg, int except_player) {
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		if (it->first != except_player) {
			_SERVER.post(it->second, msg);
		}
	}
}

bool Lobby::change_map(int map_id) {

	//if (_SERVER.maps()[map_id].length() < 2) {
	if (map_id < 1 || _DATA.getMapName(map_id) == "undefined") {
		return false;
	}

	m_MapID = map_id;

	std::vector<char> msg(sizeof(char)*2 + sizeof(int), 0);
	msg[0] = static_cast<char>(REQUEST_TYPE::CHANGE_MAP);
	INT(msg[1]) = get_id();
	msg[1+sizeof(int)] = map_id;
	
	post_all(msg);

	//if (m_pMap) {
	//	m_pMap->loadMap(map_id);
	//} else {
	//	m_pMap = _SERVER.add_game(map_id);
	//}

	return true;
}

bool Lobby::add_player(Player::ptr player) {

	//for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
	//	if (*it == player_id)
	//		return false;
	//}

	int player_id = player->get_id();

	if (m_Players.find(player_id) != m_Players.end()) {
		return false;
	}

	//Player::ptr player = _SERVER.get_player(player_id);
	//m_Players[player_id] = player;

	if (m_Team1 > m_Team2) {
		//_SERVER.players()[player_id].set_team(2);
		player->set_team(2);
		++m_Team2;
	} else {
		//_SERVER.players()[player_id].set_team(1);
		player->set_team(1);
		++m_Team1;
	}

	player->set_lobby(shared_from_this()); // почему-то даЄт нулевой пойнтер
	// потому что вызываю add_player из конструктора, а этого делать нельз€,
	// т.к. ещЄ не существует ни одной копии shared_ptr на этот экземпл€р

	const int N = sizeof(char) + sizeof(int) + sizeof(char) + sizeof(char);
	//char msg[N];
	std::vector<char> msg(N, 0);

/*
 *	1) LOBBY_NEW_PLAYER
 *	2) id (int)
 *	3) команда (char)
 *	4) цвет (сделаю, но использовать пока не буду)
*/
	int i = 1;
	msg[0] = static_cast<char>(REQUEST_TYPE::LOBBY_NEW_PLAYER);
	INT(msg[i]) = player_id;
	i += sizeof(int);
	msg[i++] = player->get_team();
	msg[i] = 1; // цвет

	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		_SERVER.post(it->second, msg);
	}

	m_Players[player_id] = player;

	return true;
}
bool Lobby::add_player(int player_id) {
//
//	//for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
//	//	if (*it == player_id)
//	//		return false;
//	//}
//
//	if (m_Players.find(player_id) != m_Players.end()) {
//		return false;
//	}
//
//	Player::ptr player = _SERVER.get_player(player_id);
//	//m_Players[player_id] = player;
//
//	if (m_Team1 > m_Team2) {
//		//_SERVER.players()[player_id].set_team(2);
//		player->set_team(2);
//		++m_Team2;
//	} else {
//		//_SERVER.players()[player_id].set_team(1);
//		player->set_team(1);
//		++m_Team1;
//	}
//
//	player->set_lobby_id(m_ID);
//
//	const int N = sizeof(char) + sizeof(int) + sizeof(char) + sizeof(char);
//	char msg[N];
//
///*
// *	1) LOBBY_NEW_PLAYER
// *	2) id (int)
// *	3) команда (char)
// *	4) цвет (сделаю, но использовать пока не буду)
//*/
//	int i = 1;
//	*msg = LOBBY_NEW_PLAYER;
//	INT(msg[i]) = player_id;
//	i += sizeof(int);
//	msg[i++] = player->get_team();
//	msg[i] = 1;
//
//	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
//		_SERVER.post(it->second, std::string(msg, N));
//	}
//
//	m_Players[player_id] = player;
//
//	return true;
	return add_player(_SERVER.get_player(player_id));
}
bool Lobby::remove_player(int player_id) {

	auto it = m_Players.find(player_id);

	if (it == m_Players.end()) {

		return false;

	} else {

		it->second->set_is_in_lobby(false);
		do_remove_player(it->second);
		m_Players.erase(it);

		return true;
	}

	//for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
	//	if (*it == player_id) {
	//		do_remove_player(player_id);
	//		m_Players.erase(it);
	//		return true;
	//	}
	//}

	//return false;
}
void Lobby::remove_all_players() {
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		it->second->set_is_in_lobby(false);
		do_remove_player(it->second);
	}
	m_Players.clear();
}
void Lobby::do_remove_player(int player_id) {
	m_Players[player_id]->set_is_in_lobby(false);
	//char msg[5];
	std::vector<char> msg(sizeof(char) + sizeof(int), 0);
	msg[0] = static_cast<char>(REQUEST_TYPE::DROP_FROM_LOBBY);
	INT(msg[1]) = player_id;
	//_SERVER.post(player_id, std::string(msg, 5));
	post_all(std::move( msg ));
}
void Lobby::do_remove_player(Player::ptr player) {
	player->set_is_in_lobby(false);
	std::vector<char> msg(sizeof(char) + sizeof(int), 0);
	msg[0] = static_cast<char>(REQUEST_TYPE::DROP_FROM_LOBBY);
	INT(msg[1]) = player->get_id();;
	//_SERVER.post(player_id, std::string(msg, 5));
	post_all(std::move( msg ));
}


void Lobby::change_team(int player_id, char team) {
	m_Players[player_id]->set_team(team);
}
void Lobby::change_team(Player::ptr player, char team) {
	player->set_team(team);
}

GameMap::ptr Lobby::get_game(int game_id) {
	return GameMap::create(m_MapID, game_id, m_pService, m_Players);
}

int Lobby::get_host_id() const {
#ifdef _USE_ASSERTS_
	assert(m_Host.get() && "Trying to get access to unexisted host");
#endif
	return m_Host->get_id();
}

//bool Lobby::start_game() {
//	//if (m_pMap) {
//	//	m_pMap->loadMap(m_MapID);
//	//} else {
//	//	m_pMap = new GameMap(m_MapID, m_pService, m_Players);
//	//	m_pMap = _SERVER.add_game(m_MapID);
//	//}
//	// TO DO
//	return true;
//}

//void GameMap::start_game() {
//	m_bGameStarted = true;
//	post(get_map_create_msg());
//	next_turn(true);
//}