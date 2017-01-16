#pragma once
#ifndef _Hero_h_
#define _Hero_h_

#include "AbstractUnit.h"

class Hero: public AbstractUnit {

public:
//	void stop();
//	void setActive(bool Active);
////	bool setPosition(Ogre::Vector3 Position);
//	void setPlayerID(int player_id);
//
//	void getShooted(AbstractUnit* fromWho);

//public:
	//Robot(
	//	Ogre::SceneManager *SM,
	//	Ogre::Vector3 StartDestination,
	//	Ogre::Vector3 Scale,
	//	Ogre::String Name,
	//	Ogre::String Mesh,
	//	Ogre::Vector3 InitialDirection,
	//	Ogre::Real WalkSpeed,
	//	Ogre::Vector3 StartDirection,
	//	Ogre::uint32 QueryFlags,
	//	Ogre::SceneNode *SN = 0);
//	Robot(const Robot&);
//	~Robot();
//public:
//	bool operator == (const Robot&);

	Hero(const Hero&);

	typedef Ogre::SharedPtr<Hero> ptr;

	//static Ogre::Real	DataBase::getInstance().getClassSpeedMod(char Class);
	//static Ogre::String	DataBase::getInstance().getClassString(char Class);
	//static char			getClassID(const Ogre::String& class_string);

	Hero(	Ogre::SceneManager *SM,
			Ogre::Vector3 StartDestination,
			//int player_id,
			//char type_id,
			//char Class,
			//char Weapon,
			//char Armor,
			//char AP,
			//short HP,
			UnitDef ud,
			const Ogre::String& Name,
//			const Ogre::String& Mesh,
//			const Ogre::String& Material,
//			Ogre::Vector3 Scale,
//			Ogre::Vector3 InitialDirection,
//			Ogre::Real WalkSpeed,
			Ogre::Vector3 StartDirection);
//			Ogre::uint32 QueryFlags = 0,
//			Ogre::SceneNode *SN = 0);

	bool checkStatus(Ogre::Real timeSinceLastFrame);

};

#endif