#pragma once
#ifndef _MAPOBJECT_H_
#define _MAPOBJECT_H_

#include "array2.h"
#include "shared_enums.h"

struct MapObjectDef {
	CellCoordinates objectCoordinates;
	MAP_OBJECT_INDEX objectIndex;

	MapObjectDef(char posX = -1, char posY = -1, MAP_OBJECT_INDEX _objectIndex = MAP_OBJECT_INDEX::BlankObject):
		objectCoordinates(posX, posY), objectIndex(_objectIndex) {}
	MapObjectDef(CellCoordinates _objectCoordinates, MAP_OBJECT_INDEX _objectIndex):
		objectCoordinates(_objectCoordinates), objectIndex(_objectIndex) {}
};

const array2<char> OgreHeadMask = array2<char>(2, 2, static_cast<char>(MAP_SYMBOL::WALL));
const array2<char> BlankCellMask = array2<char>(1, 1, static_cast<char>(MAP_SYMBOL::BLANK));
const array2<char> WallMask = array2<char>(1, 1, static_cast<char>(MAP_SYMBOL::WALL));

enum Transparentability {
	Full,
	Partially,
	None
};

class EntityMaterialInstance;

class MapObject {
	MAP_OBJECT_INDEX	m_Index;
	array2<char>		m_Mask;
	Ogre::String		m_Mesh;
	Ogre::String		m_Name;
	Ogre::Vector3		m_Scale;
	Ogre::Vector3		m_Shift;
	Ogre::Quaternion	m_Orientation;
	//Ogre::SceneManager*	m_pSceneManager;
	//bool				m_bInitialized;
	bool				m_bDynamic;
	Transparentability	m_TransparentType;
	Ogre::Entity*		m_pEntity;
	EntityMaterialInstance* m_pEmi;
	Ogre::SceneManager*	m_pSceneManager;
public:

	bool						isDynamic() const {return m_bDynamic;}
	const Ogre::String&			getMesh() const {return m_Mesh;}
	const Ogre::String&			getName() const {return m_Name;}
	const array2<char>&			getMask() const {return m_Mask;}
	Ogre::Entity*				getEntity() const {return m_pEntity;}
	Ogre::Vector3				getScale() const {return m_Scale;}
	Ogre::Vector3				getShift() const {return m_Shift;}
	Ogre::Quaternion			getOrientation() const {return m_Orientation;}
	Transparentability			getTransparentType() const {return m_TransparentType;}

	void						setScnMgr(Ogre::SceneManager* pScnMgr);
	void						setEntity(Ogre::Entity* pEnt);
	void						setTransparency(Ogre::Real tp);
	//void						setDefaultTransp();

	static std::map<MAP_OBJECT_INDEX, MapObject> StandartObjects;
	//static Ogre::Real def_transp;

	MapObject(MAP_OBJECT_INDEX Index = MAP_OBJECT_INDEX::BlankObject, const array2<char>& Mask = BlankCellMask,
		const Ogre::String& Mesh = /*Ogre::StringUtil::BLANK*/ Ogre::BLANKSTRING, const Ogre::String& Name = Ogre::BLANKSTRING,
		const Ogre::Vector3& Scale = Ogre::Vector3(1.0f), const Ogre::Vector3& Shift = Ogre::Vector3(0.0f),
		Ogre::Quaternion Orientation = Ogre::Quaternion::IDENTITY, bool Dynamic = false, Transparentability TA = None);

	MapObject(MapObject &&MO);
	MapObject(const MapObject& MO);
	MapObject& operator = (const MapObject&) = default;

	~MapObject(void);

	MAP_OBJECT_INDEX getIndex() const {return m_Index;}
	void setIndex(MAP_OBJECT_INDEX i) {m_Index=i;}

	array2<char> getMaskTempObj() const {return m_Mask;}
	void setMask(const array2<char> a) {m_Mask = a;}

	//bool initialize();
};

#endif