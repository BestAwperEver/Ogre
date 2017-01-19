#include "MapObject.h"
#include "EntityMaterialInstance.h"

std::map<MAP_OBJECT_INDEX, MapObject> MapObject::StandartObjects;
//Ogre::Real MapObject::def_transp = 0.8;

//bool MapObject::initialize() {
//#ifdef _USE_ASSERTS_
//	assert ( m_pSceneManager );
//#endif
//
//}

MapObject::MapObject(MAP_OBJECT_INDEX Index,
	const array2<char>& Mask,
	const Ogre::String& Mesh,
	const Ogre::String& Name,
	const Ogre::Vector3& Scale,
	const Ogre::Vector3& Shift,
	Ogre::Quaternion Orientation,
	bool Dynamic,
	Transparentability TA)
	: m_Index(Index)
	, m_Mask(Mask)
	, m_Mesh(Mesh)
	, m_Name(Name)
	, m_Scale(Scale)
	, m_Shift(Shift)
	, m_Orientation(Orientation)
	, m_bDynamic(Dynamic)
	, m_TransparentType(TA)
	, m_pEntity(0)
	, m_pEmi(0) 
	, m_pSceneManager(0)
{}

MapObject::MapObject(MapObject &&MO):
	m_Index(MO.m_Index),
	m_Mask( std::move(MO.m_Mask) ),
	m_Mesh( std::move(MO.m_Mesh) ),
	m_Name( std::move(MO.m_Name) ),
	m_Scale( MO.m_Scale ),
	m_Shift( MO.m_Shift ),
	m_Orientation( MO.m_Orientation ),
	m_bDynamic( MO.m_bDynamic ),
	m_TransparentType( MO.m_TransparentType ),
	m_pEntity( MO.m_pEntity ),
	m_pEmi( MO.m_pEmi ),
	m_pSceneManager( MO.m_pSceneManager )
{
	MO.m_pEntity = 0;
	MO.m_pEmi = 0;
}

MapObject::MapObject(const MapObject& MO):
	m_Index(MO.m_Index),
	m_Mask( MO.m_Mask ),
	m_Mesh( MO.m_Mesh ),
	m_Name( MO.m_Name ),
	m_Scale( MO.m_Scale ),
	m_Shift( MO.m_Shift ),
	m_Orientation( MO.m_Orientation ),
	m_bDynamic( MO.m_bDynamic ),
	m_TransparentType( MO.m_TransparentType ),
	m_pEntity( MO.m_pEntity ),
	m_pEmi( 0 ),
	m_pSceneManager( MO.m_pSceneManager )
{
	if (MO.m_pEmi)
		m_pEmi = new EntityMaterialInstance(*MO.m_pEmi);
}

void MapObject::setEntity(Ogre::Entity* pEnt) {
	if (m_pEmi)
		delete m_pEmi;
	m_pEntity = pEnt;
	m_pEmi = new EntityMaterialInstance(pEnt);
}

void MapObject::setScnMgr(Ogre::SceneManager* pScnMgr) {
	m_pSceneManager = pScnMgr;
}

MapObject::~MapObject() {
	if (m_pEmi) {
		delete m_pEmi;
		m_pEmi = 0;
	}

	if (m_pEntity) {

		auto Node = m_pEntity->getParentSceneNode();
		if (Node) {
			Node->detachObject(m_pEntity);
			m_pSceneManager->destroySceneNode(Node);
		}

		m_pSceneManager->destroyEntity(m_pEntity);

		m_pEntity = 0;
	}
	m_pSceneManager = 0;
}

void MapObject::setTransparency(Ogre::Real tp) {
	m_pEmi->setTransparency(tp);
	//int N = m_pEntity->getNumSubEntities();
	//for (int i = 0; i < N; ++i) {
	//	int M = m_pEntity->getSubEntity(i)->getMaterial()->getNumTechniques();
	//	for (int j = 0; j < M; ++j) {
	//		int K = m_pEntity->getSubEntity(i)->getMaterial()->getTechnique(j)->getNumPasses();
	//		for (int k = 0; k < K; ++k) {
	//			m_pEntity->getSubEntity(i)->getMaterial()->getTechnique(j)->getPass(k)->setDiffuse(0.5f,0.5f,0.5f,1-tp);
	//		}
	//	}
	//}
}

