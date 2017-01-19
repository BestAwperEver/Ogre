#include "AbstractFactory.h"

AbstractFactory::AbstractFactory():
	m_bConfigured(false)
{}

AbstractFactory::~AbstractFactory() {}

void AbstractFactory::configure(Ogre::SceneManager* SM) {//, 
//	const Ogre::String& mesh, 
//	const Ogre::String& material,
//	const Ogre::String& name, 
//	char type_id) {
//	Ogre::Real speed, 
//	const Ogre::Vector3& scale) {
#ifdef _USE_ASSERTS_
	assert( SM );
//	assert(mesh != Ogre::BLANKSTRING);//, "MeshName can't be blank string");
//	assert(name != Ogre::BLANKSTRING);//, "UnitName can't be blank string");
//	assert(speed);
//	assert(scale != Ogre::Vector3::ZERO);//, "Scale can't be zero vector");
#endif

	m_pSceneManager = SM;

//	m_sMesh = _DATA.getMesh(type_id);
//	m_sMaterial = _DATA.getMaterial(type_id);
//	m_sName = name;
//	m_v3Scale = _DATA.getScale(type_id);
//	m_fSpeed = _DATA.getTypeSpeed(type_id);
//	m_Type = type_id;

	m_bConfigured = true;
}

//void AbstractFactory::configure(Ogre::SceneManager* SM, 
//	const Ogre::String& mesh, 
//	const Ogre::String& material,
//	const Ogre::String& name, 
//	char type_id, 	char Weapon, char Armor
//	Ogre::Real speed, 
//	Ogre::Real scaleX, Ogre::Real scaleY, Ogre::Real scaleZ) 
//{
//	configure(SM,mesh,material,name,type_id,Class,Weapon,Armor,Armor,AP,HP,speed,Ogre::Vector3(scaleX,scaleY,scaleZ));
//}

//void AbstractFactory::setSpeed(Ogre::Real speed) {
//#ifdef _USE_ASSERTS_
//	assert( speed != 0 );
//#endif
//	m_fSpeed = speed;
//	for (auto it = m_Units.begin(); it != m_Units.end(); ++it) {
//		it->get()->setSpeed(speed);
//	}
//}
void AbstractFactory::setScale(Ogre::Vector3 scale) {
#ifdef _USE_ASSERTS_
	assert( scale != Ogre::Vector3::ZERO );
#endif
//	m_v3Scale = scale;
	for (auto it = AbstractUnit::g_Units.begin(); it != AbstractUnit::g_Units.end(); ++it) {
		(*it)->setScale(scale);
	}
}
void AbstractFactory::setScale(Ogre::Real X, Ogre::Real Y, Ogre::Real Z) {
	setScale(Ogre::Vector3(X,Y,Z));
}

void AbstractFactory::removeUnit(AbstractUnit* u) {
	//m_Units.remove(u);
	for (auto it = m_Units.begin(); it != m_Units.end(); ) {
		if (it->get() == u) {
			it = m_Units.erase(it);
		} else {
			++it;
		}
	}
}
void AbstractFactory::removeAllUnits() {
	for (auto it = m_Units.begin(); it != m_Units.end(); ++it)
		AbstractUnit::g_Units.remove(it->getPointer());
	m_Units.clear();
}

AbstractUnit::ptr AbstractFactory::createUnit(
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

	//if (Name == m_sName) {
	//	char buf[4];
	//	_itoa_s<4>(m_Units.size(), buf, 16);
	//	Name += buf;
	//}

	m_Units.push_back(AbstractUnit::ptr(new
	AbstractUnit(m_pSceneManager, 
			StartDestination,
			//PlayerID,
			//type_id,
			//Class,
			//Weapon,
			//Armor,
			//AP,
			//HP,
			ud,
			Name,
//			m_sMesh,
//			m_sMaterial,
//			m_v3Scale,  
//			InitialDirection, 
//			m_fSpeed, 
			StartDirection)));

	m_Units.back()->setAnimation("Idle");

	return m_Units.back();
}
AbstractUnit::ptr AbstractFactory::createUnit(
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
{
	return createUnit(Ogre::Vector3(X,Y,Z), ud, /*PlayerID, type_id, Class, Weapon, Armor, AP, HP, Name,*/ StartDirection);
}
AbstractUnit::ptr AbstractFactory::createUnit(
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
	return createUnit(StartDestination, ud, /*PlayerID, type_id, Class, Weapon, Armor, AP, HP, Name,*/ 
		_DATA.getInitialDirection(ud.type_id));
}
AbstractUnit::ptr AbstractFactory::createUnit(
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
	return createUnit(X,Y,Z, ud, /*PlayerID, type_id, Class, Weapon, Armor, AP, HP, Name,*/ 
		_DATA.getInitialDirection(ud.type_id));	
}
AbstractUnit::ptr AbstractFactory::createUnit(
	Ogre::Vector3 StartDestination,
	//int player_id,
	//char type_id,
	//char Class,
	//char Weapon,
	//char Armor,
	//char AP,
	//short HP,
	UnitDef ud,
	Ogre::Vector3 StartDirection) 
{
	return createUnit(StartDestination, ud, //PlayerID, type_id, Class, Weapon, Armor, AP, HP,
		_DATA.getDefaultName(ud.type_id), StartDirection);
}
AbstractUnit::ptr AbstractFactory::createUnit(
	Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
	//int player_id,
	//char type_id,
	//char Class,
	//char Weapon,
	//char Armor,
	//char AP,
	//short HP,
	UnitDef ud,
	Ogre::Vector3 StartDirection) 
{
	return createUnit(X,Y,Z, ud, //PlayerID, type_id, Class, Weapon, Armor, AP, HP,
		_DATA.getDefaultName(ud.type_id), StartDirection);
}
AbstractUnit::ptr AbstractFactory::createUnit(
	Ogre::Vector3 StartDestination,
	//int player_id,
	//char type_id,
	//char Class,
	//char Weapon,
	//char Armor,
	//char AP,
	//short HP) 
	UnitDef ud)
{
	return createUnit(StartDestination, ud, //PlayerID, type_id, Class, Weapon, Armor, AP, HP,
		_DATA.getDefaultName(ud.type_id), _DATA.getInitialDirection(ud.type_id));
}
AbstractUnit::ptr AbstractFactory::createUnit(
	Ogre::Real X, Ogre::Real Y, Ogre::Real Z,
	//int player_id,
	//char type_id,
	//char Class,
	//char Weapon,
	//char Armor,
	//char AP,
	//short HP) 
	UnitDef ud)
{
	return createUnit(X,Y,Z, ud, //PlayerID, type_id, Class, Weapon, Armor, AP, HP,
		_DATA.getDefaultName(ud.type_id), _DATA.getInitialDirection(ud.type_id));
}

AbstractFactory& AbstractFactory::getInstance() {
	static AbstractFactory instance;
	return instance;
}