#include "Player.h"
#include "tcp_connection.h"
#include "Logger.h"
#include "..\..\Database0\Database0\DataBase.h"
#include <boost\lexical_cast.hpp>

Player::Player(int ID, Ogre::String Nick):
	m_ID(ID)
	, m_Team(0)
	, m_LobbyID(0)
	, m_GameID(0)
	, m_Nick(Nick)
	, m_bIsOnline(false)
	, m_bIsActive(false)
	, m_bIsInLobby(false)
	, m_bIsInGame(false)
	, m_bHasUnits(false)
	, m_UnitsOnMapCount(0)
	, m_pCurMerk(0)
{
}

Player::Player():
	m_ID(-1)
	//, m_Nick("")
	, m_Team(0)
	, m_LobbyID(-1)
	, m_GameID(-1)
	//, m_cl(nullptr) 
	, m_bIsOnline(false)
	, m_bIsActive(false)
	, m_bIsInLobby(false)
	, m_bIsInGame(false)
	, m_bHasUnits(false)
	, m_UnitsOnMapCount(0)
	, m_pCurMerk(nullptr)
{
}

Ogre::String Player::get_nick() const {
	if (!m_Nick.empty())
		return m_Nick; 
	return boost::lexical_cast<Ogre::String>(m_ID);
}

void Player::set_is_in_lobby(bool in_lobby) {
	//if (m_bIsInLobby && !in_lobby) {
	//	_LOG("You aren't in lobby anymore");
	//}
	m_bIsInLobby = in_lobby;
	if (!in_lobby) {
		m_LobbyID = -1;
	}
}

void Player::set_is_in_game(bool in_game) {
	m_bIsInGame = in_game;
	if (!in_game) {
		set_has_units(false);
		m_GameID = -1;
	}
}

void Player::set_has_units(bool has_units) {
	m_bHasUnits = has_units;
	if (!has_units) {
		m_UnitsOnMapCount = 0;
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
void Player::reset() {
	m_ID = -1;
	m_Nick.clear();
	m_Team = 0;
	m_LobbyID = -1;
	m_GameID = -1;
	m_bHasUnits = false;
	m_bIsOnline = false;
	m_bIsActive = false;
	m_bIsInLobby = false;
	m_bIsInGame = false;
	m_UnitsOnMapCount = 0;
	m_Merks.clear();
	m_pCurMerk = 0;
}

int Player::get_merk_id() const {
#ifdef _USE_ASSERTS_
	assert( m_pCurMerk && "Trying to get access to unexisted merk");
#endif
	return m_pCurMerk->get_id();
}

void Player::set_merk_id(int merk_id){
	auto it = m_Merks.find(merk_id);
#ifdef _USE_ASSERTS_
	assert( it != m_Merks.end() && "Trying to get access to unexisted merk");
#endif
	m_pCurMerk = it->second;
}

void Player::add_merk(Merk::ptr merk) {
	m_Merks[merk->get_id()] = merk;
}
void Player::delete_merk(int merk_id) {
	auto it = m_Merks.find(merk_id);
#ifdef _USE_ASSERTS_
	assert( it != m_Merks.end() && "Trying to delete unexisted merk");
#endif
	if (m_pCurMerk == it->second) {
		m_pCurMerk = nullptr;
		_LOG("Merk " + it->second->get_name()
			+ " is no longer selected");
	}
	m_Merks.erase(it);
}
void Player::show_merk_list() const {
	if (m_Merks.empty()) {
		_LOG("You have no merks. Create one");
		return;
	}
	_LOG("Merk list:");
	int i = 1;
	for (auto it = m_Merks.begin(); it != m_Merks.end(); ++it) {
		if (m_pCurMerk == it->second) {
			_LOG(boost::lexical_cast<Ogre::String>(i++) + ") " + it->second->get_name()
				+ " (id " + boost::lexical_cast<Ogre::String>(it->first) + ") - current merk :");
		} else {
			_LOG(boost::lexical_cast<Ogre::String>(i++) + ") " + it->second->get_name()
				+ " (id " + boost::lexical_cast<Ogre::String>(it->first) + "):");
		}
		_LOG("   type_id: " + _DATA.getTypeString(it->second->get_type_id()));
		_LOG("   Weapon: " + _DATA.getWeaponString(it->second->get_weapon_id()));
		_LOG("   HP: " + boost::lexical_cast<Ogre::String>(it->second->get_ud().HP));
		_LOG("   AP: " + boost::lexical_cast<Ogre::String>(int(it->second->get_ud().AP)));
		_LOG("   View range: " + boost::lexical_cast<Ogre::String>(int(it->second->get_ud().ViewRange)));
	}
}

Merk::ptr Player::get_merk(int merk_id) {
	auto it = m_Merks.find(merk_id);
	if (it == m_Merks.end()) {
		return nullptr;
	}
	return it->second;
}

void Player::change_weapon(int merk_id, char weapon_id) {
	auto it = m_Merks.find(merk_id);
#ifdef _USE_ASSERTS_
	assert( it != m_Merks.end() && "Trying to change weapon on unexisted merk");
#endif
	it->second->set_weapon_id(weapon_id);
}

//bool Player::hosts_a_lobby() const {
//
//}