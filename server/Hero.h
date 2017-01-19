#pragma once
#ifndef _HERO_H_
#define _HERO_H_

#include "abstractunit.h"

class Hero: public AbstractUnit
{
	std::string m_Name;
	Hero(std::string Name, UnitDef ud, CellCoordinates looks_to);
public:
//	typedef boost::shared_ptr<Hero> ptr;

	void setName(const std::string& Name) {m_Name = Name;}

	std::string getName() const {return m_Name;}

	//static std::list<ptr> m_Units;
	//static ptr _new(std::string Name, char type_id, char Class, char Weapon, char Armor,
	//				int player_id, short HP, char AP, CellCoordinates looks_to);
	static ptr create(std::string Name, UnitDef ud, CellCoordinates looks_to);
	//static void remove(Hero*);
};

#endif

