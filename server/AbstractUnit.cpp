#include "AbstractUnit.h"
#include "DataBase.h"

//std::list<AbstractUnit::ptr> AbstractUnit::m_Units;

AbstractUnit::AbstractUnit(UnitDef ud, CellCoordinates looks_to, std::string name)
	: m_UnitDef(ud)
	//: m_Type(type_id)
	, m_HitPoints(ud.HP)
	//, m_Team(player_id)
	, m_LooksTo(looks_to)
	, m_ActivePoints(ud.AP)
	//, m_Class(Class)
	//, m_Weapon(Weapon)
	//, m_Armor(Armor)
	, m_Name(name)
{
	if (name.empty()) {
		m_Name = _DATA.getDefaultName(ud.type_id);
	}
	recalculateArmorClass();
}

AbstractUnit::~AbstractUnit(void)
{
}

AbstractUnit::ptr AbstractUnit::create(UnitDef ud, CellCoordinates looks_to, std::string name) {
	ptr new_unit(new AbstractUnit(ud, looks_to, name));
	//m_Units.push_back(ptr(new AbstractUnit(ud, looks_to)));
	//return m_Units.back();
	return new_unit;
}
//void AbstractUnit::remove(AbstractUnit* unit) {
//	for (auto it = m_Units.begin(); it != m_Units.end(); ) {
//		if (it->get() == unit) {
//			it = m_Units.erase(it);
//		} else {
//			++it;
//		}
//	}
//}

void AbstractUnit::recalculateArmorClass() {
	m_ArmorClass =
		_DATA.getDefaultAC(m_UnitDef.type_id) +
		_DATA.getClassACBonus(m_UnitDef.Class) +
		_DATA.getArmorAC(m_UnitDef.Armor);
}

void AbstractUnit::setArmor(char Armor) {
	m_UnitDef.Armor = Armor;
	recalculateArmorClass();
}
void AbstractUnit::setClass(char Class) {
	m_UnitDef.Class = Class;
	recalculateArmorClass();
}