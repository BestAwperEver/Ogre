#include "RobotFactory.h"

//RobotFactory::RobotFactory():
//	m_bConfigured(false)
//{}
//
//RobotFactory::~RobotFactory(void) {
//
//}
//
//void RobotFactory::configure(Ogre::SceneManager* SM, 
//	const Ogre::String& mesh, const Ogre::String& name, Ogre::Real speed, const Ogre::Vector3& scale) {
//#ifdef _USE_ASSERTS_
//	assert( SM );
//	assert(mesh != Ogre::StringUtil::BLANK);//, "MeshName can't be blank string");
//	assert(name != Ogre::StringUtil::BLANK);//, "UnitName can't be blank string");
//	assert(speed);
//	assert(scale != Ogre::Vector3::ZERO);//, "Scale can't be zero vector");
//#endif
//
//	m_pSceneManager = SM;
//
//	m_sMesh = mesh;
//	m_sName = name;
//	m_v3Scale = scale;
//	m_fSpeed = speed;
//
//	m_bConfigured = true;
//}
//
//void RobotFactory::configure(Ogre::SceneManager* SM,
//		const Ogre::String& DefMesh, const Ogre::String& DefName, Ogre::Real DefSpeed,
//		Ogre::Real scaleX, Ogre::Real scaleY, Ogre::Real scaleZ) {
//	configure(SM,DefMesh,DefName,DefSpeed,Ogre::Vector3(scaleX,scaleY,scaleZ));
//}
//
//void RobotFactory::removeUnit(AbstractUnit::ptr u) {
//	m_Units.remove(u);
//}
//
//void RobotFactory::removeAllUnits() {
//	m_Units.clear();
//}
//
//AbstractUnit::ptr RobotFactory::createUnit(
//		Ogre::Vector3 StartDestination,
//		Ogre::Vector3 InitialDirection,
//		Ogre::Vector3 StartDirection) 
//{
//#ifdef _USE_ASSERTS_
//	assert(m_bConfigured);
//#endif
//
//	char buf[4];
//	_itoa_s<4>(m_Units.size(), buf, 16);
//	Ogre::String Name = m_sName + buf;
//
//	m_Units.push_front(AbstractUnit::ptr(new
//	AbstractUnit(SM, StartDestination, Scale, Name, Mesh, InitialDirection, Speed, StartDirection, QueryFlags, SN)));
//
//	m_Units.front()->setAnimation("Idle");
//
//	return m_Units.front();
//}
//
//AbstractUnit::ptr RobotFactory::createUnit(
//	Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
//	Ogre::Vector3 InitialDirection,
//	Ogre::Vector3 StartDirection) 
//{
//return createUnit(SM,Ogre::Vector3(X,Y,Z),Scale,Speed,Name,Mesh,InitialDirection,StartDirection,QueryFlags,SN);
//}


void RobotFactory::configure(Ogre::SceneManager* SM) 
{ AbstractFactory::configure(SM,"Robot",UNIT_TYPE::ROBOT); }

AbstractUnit::ptr RobotFactory::createUnit(
		Ogre::Vector3 StartDestination,
		char Team,
		Ogre::Vector3 StartDirection)
//		Ogre::Vector3 InitialDirection) 
{ return AbstractFactory::createUnit(StartDestination, Team, UNIT_TYPE::ROBOT, "Robot", StartDirection); }

AbstractUnit::ptr RobotFactory::createUnit(
	Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
	char Team,
	Ogre::Vector3 StartDirection)
//	Ogre::Vector3 InitialDirection)
{ return AbstractFactory::createUnit(X,Y,Z, Team, UNIT_TYPE::ROBOT, "Robot", StartDirection); }

RobotFactory& RobotFactory::getInstance(void)
{  
    static RobotFactory instance;
	return instance;
}