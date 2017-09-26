#include "StdAfx.h"
#include "Merk.h"
//#include "../../Database0/Database0/DataBase.h"

Merk::Merk(int merk_id, String name, UnitDef ud)
	: m_MerkID(merk_id)
	, m_Name(name)
	, m_UnitDef(ud)
{
}

Merk::ptr Merk::create(int player_id, int merk_id, String name, char type_id, char weapon_id) {
	UnitDef ud = _DATA.getDefaultUD(type_id);
	ud.PlayerID = player_id;
	if (weapon_id != static_cast<char>(WEAPON::UNDEFINED_WEAPON)) {
		ud.Weapon = weapon_id;
	}
	return Merk::ptr(new Merk(merk_id, name, ud));
}

Merk::ptr Merk::create(int merk_id, String name, UnitDef ud) {
	return Merk::ptr(new Merk(merk_id, name, ud));
}

//Merk::~Merk(void)
//{
//}
