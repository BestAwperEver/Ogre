#pragma once
#ifndef _Robot_Factory_h_
#define _Robot_Factory_h_

#include "AbstractFactory.h"

class RobotFactory : public AbstractFactory {
	//std::list<AbstractUnit::ptr>	m_Units;
//	Ogre::uint32			m_nCount;
	//bool					m_bConfigured;

	//Ogre::String			m_sMesh;
	//Ogre::String			m_sName;
	//Ogre::Vector3			m_v3Scale;
	//Ogre::Real				m_fSpeed;
	//Ogre::SceneManager*		m_pSceneManager;

	//RobotFactory();
public:

	//~RobotFactory(void);

	AbstractUnit::ptr createUnit(
		Ogre::Vector3 StartDestination,
		char Team,
//		Ogre::SceneManager *SM,
//		Ogre::Vector3 Scale = m_v3DefaultScale,
//		Ogre::Real WalkSpeed = m_fDefaultSpeed,
//		Ogre::String Name = m_sDefaultName,
//		Ogre::String Mesh = m_sDefaultMesh,
		Ogre::Vector3 StartDirection = Ogre::Vector3::UNIT_X);
//		Ogre::Vector3 InitialDirection = Ogre::Vector3::UNIT_X);
//		Ogre::uint32 QueryFlags = ROBOT_QUERY_FLAG,
//		Ogre::SceneNode *SN = 0);

	AbstractUnit::ptr createUnit(
		Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
		char Team,
//		Ogre::SceneManager *SM,
//		Ogre::Vector3 Scale = m_v3DefaultScale,
//		Ogre::Real WalkSpeed = m_fDefaultSpeed,
//		Ogre::String Name = m_sDefaultName,
//		Ogre::String Mesh = m_sDefaultMesh,
		Ogre::Vector3 StartDirection = Ogre::Vector3::UNIT_X);
//		Ogre::Vector3 InitialDirection = Ogre::Vector3::UNIT_X);
//		Ogre::uint32 QueryFlags = ROBOT_QUERY_FLAG,
//		Ogre::SceneNode *SN = 0);

	//void removeUnit(AbstractUnit::ptr);
	//void removeAllUnits();

	//void setScale(Ogre::Vector3 new_scale);
	//Ogre::Vector3 getScale() const {return m_v3Scale;}

	//void setSpeed(Ogre::Real new_speed);
	//Ogre::Real getSpeed() const {return m_fSpeed;}

	static RobotFactory& getInstance();

	void configure(Ogre::SceneManager* SM);
//		const Ogre::String& default_mesh = "robot.mesh",
//		const Ogre::String& default_material = "Robot",
//		const Ogre::String& default_name = "Robot",
//		char Type = UNIT_TYPE::ROBOT,
//		Ogre::Real default_speed = 50,
//		const Ogre::Vector3& default_scale = Ogre::Vector3(1.0f));

	//void configure(Ogre::SceneManager* SM,
	//	const Ogre::String& default_mesh = "robot.mesh",
	//	const Ogre::String& default_name = "Robot",
	//	Ogre::Real default_speed = 50,
	//	Ogre::Real scaleX = 1, Ogre::Real scaleY = 1, Ogre::Real scaleZ = 1);
};

#endif