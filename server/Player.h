#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

class talk_to_client;
class Lobby;
class GameMap;

#include "Merk.h"

class Player: public boost::enable_shared_from_this<Player>, public boost::noncopyable {

	typedef boost::shared_ptr<talk_to_client> client_ptr;
	typedef boost::shared_ptr<Lobby> Lobby_ptr;
	typedef boost::shared_ptr<GameMap> GameMap_ptr;

	friend Lobby;

	int			m_ID;
	client_ptr	m_cl;

	char	m_Team;
	//int		m_LobbyID;
	//int		m_GameID;
	Lobby_ptr m_Lobby;
	GameMap_ptr m_GameMap;

	bool	m_bIsActive;
	bool	m_bHasUnits;
	char	m_UnitsOnMapCount;

	bool	m_bIsOnline; // надо ли? в клиенте есть логгед_ин
	bool	m_bIsInLobby; // надо ли? можно просто ид лобби в нуль
	bool	m_bIsInGame; // аналогично

	void	set_team(char Team) { m_Team = Team; }
	void	set_has_units(bool has_units) { m_bHasUnits = has_units; }

	Merk::ptr m_pCurMerk;
	std::map<int, Merk::ptr> m_Merks;
	int m_MaxMerkID;

	void load_merks();

	Player(int player_id, client_ptr cl, bool online);

public:

	typedef boost::shared_ptr<Player> ptr;

	static Player::ptr Player::create(int player_id, client_ptr cl, bool online = true);

	bool has_units() const { return m_bHasUnits; }

	//void choose_start_merk();

	void loose_unit();
	void new_unit();

	bool is_active() const { return m_bIsActive; }
	void set_active(bool active) { m_bIsActive=active; }

	bool is_online() const {return m_bIsOnline;}
	void set_online(bool online);

	bool is_in_game() const { return m_bIsInGame; }
	void set_is_in_game(bool in_game);

	bool is_in_lobby() const { return m_bIsInLobby; }
	void set_is_in_lobby(bool in_lobby);

	void set_client(client_ptr cl) { m_cl = cl; }
	client_ptr get_client() const {return m_cl;}

	void set_id(int id) { m_ID = id; }
	int get_id() const {return m_ID;}

	int get_merk_id() const;
	int get_max_merk_id() const { return m_MaxMerkID; }
	Merk::ptr get_merk();
	Merk::ptr get_merk(int merk_id);
	int get_merk_count() const { return m_Merks.size(); }
	bool set_merk_id(int merk_id);

	bool add_merk(std::string name, UnitDef ud);
	bool merk_exist(int merk_id) const { return m_Merks.find(merk_id) != m_Merks.end(); }
	bool delete_merk(int merk_id);
	void change_weapon(char weapon_id) { m_pCurMerk->set_weapon_id(weapon_id); }

	void send_merk_list();

	bool is_host_of_the_lobby() const;

	char	get_team() const {return m_Team;}
	int		get_lobby_id() const;
	int		get_game_id() const;

	Lobby_ptr	get_lobby() const {return m_Lobby;}
	GameMap_ptr	get_game() const {return m_GameMap;}

	void	set_lobby_id(int lobby_id);
	void	set_game_id(int game_id);

	void	set_lobby(Lobby_ptr);
	void	set_game(GameMap_ptr);

};

#endif

