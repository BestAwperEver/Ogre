#pragma once
#ifndef _MAPOBJECT_H_
#define _MAPOBJECT_H_

#include "array2.h"
#include "shared_enums.h"

struct MapObjectDef {
	CellCoordinates objectCoordinates;
	MAP_OBJECT_INDEX objectIndex;

	MapObjectDef(char posX = -1, char posY = -1,
		MAP_OBJECT_INDEX _objectIndex = MAP_OBJECT_INDEX::BlankObject):
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

class MapObject {
	MAP_OBJECT_INDEX		m_Index;
	array2<char>	m_Mask;
	std::string			m_Name;

public:

	const std::string&			getName() const {return m_Name;}
	const array2<char>&	getMask() const {return m_Mask;}

	static std::map<MAP_OBJECT_INDEX, MapObject> StandartObjects;

	MapObject(MAP_OBJECT_INDEX Index = MAP_OBJECT_INDEX::BlankObject,
		const array2<char>& Mask = BlankCellMask, 
		const std::string& Name = "Blank"):
		m_Index(Index), 
		m_Mask(Mask),
		m_Name(Name)
	{}

	MapObject(MapObject &&MO):
			m_Index(MO.m_Index),
			m_Mask( std::move(MO.m_Mask) ),
			m_Name( std::move(MO.m_Name) )
			{}

	~MapObject(void) {}

	MapObject& operator = (const MapObject&) = default;

	char getIndex() const {return static_cast<char>(m_Index);}
	void setIndex(MAP_OBJECT_INDEX i) {m_Index=i;}

	array2<char> getMaskTempObj() const {return m_Mask;}
	void setMask(const array2<char> a) {m_Mask = a;}

	//bool initialize();
};

#endif