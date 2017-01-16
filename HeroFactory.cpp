#include "HeroFactory.h"

AbstractUnit::ptr HeroFactory::createUnit(
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
		Ogre::Vector3 StartDirection) 
{
#ifdef _USE_ASSERTS_
	assert(m_bConfigured);
#endif

	m_Units.push_back(AbstractUnit::ptr(new Hero(m_pSceneManager,
		StartDestination, ud, //PlayerID, type_id, Class, Weapon, Armor, AP, HP, 
		Name, StartDirection)));

	m_Units.back()->setAnimation("Idle");

	return m_Units.back();
}
AbstractUnit::ptr HeroFactory::createUnit(
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
		Ogre::Vector3 StartDirection)
{ return createUnit(X, Y, Z, ud, /*PlayerID,type_id,Class,Weapon,Armor,AP,HP,*/ Name, StartDirection); }

AbstractUnit::ptr HeroFactory::createUnit(
		Ogre::Vector3 StartDestination,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::String Name)
{
	return createUnit(StartDestination, ud, //PlayerID, type_id, Class, Weapon, Armor, AP, HP, 
		Name, _DATA.getInitialDirection(ud.type_id)); 
}
AbstractUnit::ptr HeroFactory::createUnit(
		Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::String Name)
{ 
	return createUnit(X, Y, Z, ud, //PlayerID, type_id, Class, Weapon, Armor, AP, HP, 
		Name, _DATA.getInitialDirection(ud.type_id)); 
}

HeroFactory& HeroFactory::getInstance(void)
{  
    static HeroFactory instance;
	return instance;
}