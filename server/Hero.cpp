#include "Hero.h"

//std::list<AbstractUnit::ptr> Hero::m_Units;

Hero::Hero(std::string Name, UnitDef ud, CellCoordinates looks_to)
	: AbstractUnit(ud, looks_to),
	m_Name(Name)
{
}

AbstractUnit::ptr Hero::create(std::string Name, UnitDef ud, CellCoordinates looks_to) {
	AbstractUnit::ptr new_unit(new Hero(Name, ud, looks_to));
	//m_Units.push_back(AbstractUnit::ptr(new Hero(Name, ud, looks_to)));
	//return m_Units.back();
	//return ptr(new Hero(Name, type_id, Class, Weapon, Armor, PlayerID));
	return new_unit;
}
//void Hero::remove(Hero* unit) {
//	for (auto it = m_Units.begin(); it != m_Units.end(); ) {
//		if (it->get() == unit) {
//			it = m_Units.erase(it);
//		} else {
//			++it;
//		}
//	}
//}