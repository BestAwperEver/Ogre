#pragma once
#ifndef _Hero_Factory_h_
#define _Hero_Factory_h_

#include "AbstractFactory.h"
#include "Hero.h"

class HeroFactory : public AbstractFactory {

public:

	AbstractUnit::ptr createUnit(
		Ogre::Vector3 StartDestination,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::String Name,
		Ogre::Vector3 StartDirection);
	AbstractUnit::ptr createUnit(
		Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::String Name,
		Ogre::Vector3 StartDirection);

	AbstractUnit::ptr createUnit(
		Ogre::Vector3 StartDestination,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::String Name);
	AbstractUnit::ptr createUnit(
		Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::String Name);

	static HeroFactory& getInstance();
};

#endif