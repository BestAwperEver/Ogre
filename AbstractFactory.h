#pragma once
#ifndef _Abstract_Factory_h_
#define _Abstract_Factory_h_

#include <Type_traits>
#include "AbstractUnit.h"
//extern std::list<AbstractUnit*> g_Units;
//static_assert(std::is_base_of<AbstractUnit, T>::value, "template parameter must be derived from AbstractUnit");

class AbstractFactory {//: public AbstractFactory<Robot> {
protected:
	std::list<AbstractUnit::ptr>	m_Units;
//	Ogre::uint32			m_nCount;
	bool					m_bConfigured;

//	Ogre::String			m_sMesh;
//	Ogre::String			m_sMaterial;
//	Ogre::String			m_sName;
//	Ogre::Vector3			m_v3Scale;
//	Ogre::Real				m_fSpeed;
	Ogre::SceneManager*		m_pSceneManager;

	AbstractFactory();
public:

//	virtual Ogre::String	getMesh() const {return m_sMesh;}
//	virtual Ogre::String	getMaterial() const {return m_sMaterial;}
//	virtual Ogre::String	getName() const {return m_sName;}

	virtual ~AbstractFactory(void);

	virtual AbstractUnit::ptr createUnit(
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
	virtual AbstractUnit::ptr createUnit(
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

	virtual AbstractUnit::ptr createUnit(
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
	virtual AbstractUnit::ptr createUnit(
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

	virtual AbstractUnit::ptr createUnit(
		Ogre::Vector3 StartDestination,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::Vector3 StartDirection);
	virtual AbstractUnit::ptr createUnit(
		Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP,
		UnitDef ud,
		Ogre::Vector3 StartDirection);

	virtual AbstractUnit::ptr createUnit(
		Ogre::Vector3 StartDestination,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP);
		UnitDef ud);
	virtual AbstractUnit::ptr createUnit(
		Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
		//int player_id,
		//char type_id,
		//char Class,
		//char Weapon,
		//char Armor,
		//char AP,
		//short HP);
		UnitDef ud);

	virtual void removeUnit(AbstractUnit*);
	virtual void removeAllUnits();

	virtual void setScale(Ogre::Vector3 new_scale);
	virtual void setScale(Ogre::Real X, Ogre::Real Y, Ogre::Real Z);
//	virtual void setSpeed(Ogre::Real new_speed);

//	virtual Ogre::Vector3	getScale() const {return m_v3Scale;}
//	virtual Ogre::Real		getSpeed() const {return m_fSpeed;}

	static AbstractFactory& getInstance();

	virtual void configure(Ogre::SceneManager* SM);//,
//		const Ogre::String& default_mesh,
//		const Ogre::String& default_material,
//		const Ogre::String& default_name,
//		char type_id,
//		char Class);

//	virtual void configure(Ogre::SceneManager* SM,
//		const Ogre::String& default_mesh,
//		const Ogre::String& default_material,
//		const Ogre::String& default_name,
//		char type_id);
//		Ogre::Real default_speed,
//		Ogre::Real scaleX, Ogre::Real scaleY, Ogre::Real scaleZ);
};

#endif