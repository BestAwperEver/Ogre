#pragma once
#ifndef _MERK_H_
#define _MERK_H_

#include "DataBase.h"
//#include "../../Database0/Database0/UnitDef.h"
#include "shared_enums.h"

#ifndef _Ogre_H__
#include <string>
#endif

class Merk: public std::enable_shared_from_this<Merk>
{
#ifdef _Ogre_H__
	typedef Ogre::String String;
#else
	typedef std::string String;
#endif

	int m_MerkID;
	String m_Name;
	UnitDef m_UnitDef;

	Merk(int merk_id, String name, UnitDef ud);
	Merk(const Merk&) = delete;

public:
	//Merk(void);
	//~Merk(void);

	typedef boost::shared_ptr<Merk> ptr;

	static Merk::ptr create(int player_id, int merk_id,
		String name, char type_id, char weapon_id = static_cast<char>(WEAPON::UNDEFINED_WEAPON));
	static Merk::ptr create(int merk_id, String name, UnitDef ud);

	UnitDef&	get_ud() { return m_UnitDef; }
	UnitDef		get_ud() const { return m_UnitDef; }
	char		get_type_id() const { return m_UnitDef.type_id; }
	char		get_weapon_id() const { return m_UnitDef.Weapon; }
	void		set_ud(UnitDef ud) { m_UnitDef = ud; }
	void		set_weapon_id(char weapon_id) { m_UnitDef.Weapon = weapon_id; }

	String	get_name() const { return m_Name; }
	void	set_name(const String& name) { m_Name = name; }
	void	set_name(String&& name) { m_Name = name; }

	int		get_id() const { return m_MerkID; }
	void	set_id(int merk_id) { m_MerkID = merk_id; }

};

#endif
