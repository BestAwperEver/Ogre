#include "Hero.h"
//#include "SelectedGroup.h"

//bool Robot::setPosition(Ogre::Vector3 pos) {
//
//	if ( !AbstractUnit::setPosition(pos) ) return false;
//
//	//if (m_pLight) {
//	//	Ogre::Vector3 ololo = getPosition();
//	//	ololo.y += m_pEntity->getBoundingBox().getSize().y*2;
//	//	m_pLight->setPosition(ololo);
//	//}
//
//	return true;
//}

Hero::Hero(const Hero& h):
	AbstractUnit(h) {}

Hero::Hero(	Ogre::SceneManager *SM,
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
//		const Ogre::String& Mesh,
//		const Ogre::String& Material,
//		Ogre::Vector3 Scale,
//		Ogre::Vector3 InitialDirection,
//		Ogre::Real WalkSpeed,
		Ogre::Vector3 StartDirection):
//		Ogre::uint32 QueryFlags,
//		Ogre::SceneNode *SN):
	AbstractUnit(SM,
		StartDestination,
		//PlayerID, type_id, Class, Weapon, Armor, AP, HP, 
		ud,
		Name,
		StartDirection)
//	m_Class(Class) 
{
	//setClass(Class);	
}

bool Hero::checkStatus(Ogre::Real time) {

	switch (m_UnitDef.type_id) {
	//case UNIT_TYPE::DWARF:
	//case UNIT_TYPE::OGRE:
	//	time *= 0.9;
	//	break;
	case UNIT_TYPE::HALFLING:
		time *= 1.2;
		break;
	}

	switch (m_UnitDef.Class) {
	case HERO_CLASS::ROGUE:
		time *= 1.2;
		break;
	//case HERO_CLASS::MONK:
	//	time *= 0.9;
	//	break;
	}

	return AbstractUnit::checkStatus(time);
}