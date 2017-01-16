#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Merk.h"
class talk_to_svr;

class Player {

	int		m_ID;
	Ogre::String m_Nick;

	char	m_Team;
	int		m_LobbyID;
	int		m_GameID;
	bool	m_bIsActive;
	bool	m_bHasUnits;
	char	m_UnitsOnMapCount;

	bool	m_bIsOnline;
	bool	m_bIsInLobby;	// надо ли? можно просто ид лобби в нуль
	bool	m_bIsInGame;	// аналогично

	void set_has_units(bool has_units);

	Merk::ptr m_pCurMerk;
	std::map<int, Merk::ptr> m_Merks;

public:

	//typedef boost::shared_ptr<Player> ptr;

	Player();
	Player(int ID, Ogre::String Nick);

	void reset();

	bool has_name() const { return !m_Nick.empty(); }

	bool has_units() const { return m_bHasUnits; }

	//bool hosts_a_lobby() const;

	void loose_unit();
	void new_unit();

	bool is_active() const { return m_bIsActive; }
	void set_active(bool active) { m_bIsActive=active; }

	bool is_online() const {return m_bIsOnline;}
	void set_online(bool online) { m_bIsOnline = online; }

	bool is_in_game() const { return m_bIsInGame; }
	void set_is_in_game(bool in_game);

	bool is_in_lobby() const { return m_bIsInLobby; }
	void set_is_in_lobby(bool in_lobby);

	void set_id(int id) { m_ID = id; }
	int get_id() const { return m_ID; }

	void set_nick(Ogre::String Nick) { m_Nick = Nick; }
	Ogre::String get_nick() const;

	char	get_team() const { return m_Team; }
	int		get_lobby_id() const { return m_LobbyID; }
	int		get_game_id() const { return m_GameID; }
	int		get_merk_id() const;
	int		get_merk_count() const { return m_Merks.size(); }

	Merk::ptr get_merk() { return m_pCurMerk; }
	Merk::ptr get_merk(int merk_id);
	
	void	set_team(char Team) { m_Team = Team; }
	void	set_lobby_id(int lobby_id) { m_LobbyID = lobby_id; m_bIsInLobby = !(lobby_id == -1); }
	void	set_game_id(int game_id) { m_GameID = game_id; m_bIsInGame = !(game_id == -1); }
	void	set_merk_id(int merk_id);

	void	add_merk(Merk::ptr);
	bool	merk_exist(int merk_id) const { return m_Merks.find(merk_id) != m_Merks.end(); }
	bool	merk_choosen() const { return m_pCurMerk != nullptr; }
	void	delete_merk(int merk_id);
	void	change_weapon(int merk_id, char weapon_id);

	void	show_merk_list() const;

};

#endif