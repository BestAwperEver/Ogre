#include "MapContainer.h"

MapContainer::MapContainer(int Height, int Width):
	m_Array(Height, Width, static_cast<char>(MAP_SYMBOL::BLANK))
		//m_Height(Height)
		//, m_Width(Width)
		//, m_bCleared(false)
{
#ifdef _USE_ASSERTS_
	assert( Height > 0 && Width > 0 );
#endif
	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = 0;
	//	}
	//}
}

MapContainer::MapContainer(int Height, int Width, int ObjectCount):
		m_Array(Height, Width, static_cast<char>(MAP_SYMBOL::BLANK))
		//m_Height(Height)
		//, m_Width(Width)
		//, m_bCleared(false)
		, m_Objects(ObjectCount)
{
#ifdef _USE_ASSERTS_
	assert( Height > 0 && Width > 0 && ObjectCount >= 0);
#endif
	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = BLANK;
	//	}
	//}
}

MapContainer::MapContainer(int Height, int Width, const std::vector<MapObjectDef>& Objects):
		m_Array(Height, Width, static_cast<char>(MAP_SYMBOL::BLANK))
		//m_Height(Height)
		//, m_Width(Width)
		//, m_bCleared(false)
		, m_Objects(Objects)
{
#ifdef _USE_ASSERTS_
	assert( Height > 0 && Width > 0 );
#endif
	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = BLANK;
	//	}
	//}
}

MapContainer::MapContainer(int Height, int Width, std::vector<MapObjectDef>&& Objects):
		m_Array(Height, Width, static_cast<char>(MAP_SYMBOL::BLANK))
		//m_Height(Height)
		//, m_Width(Width)
		//, m_bCleared(false)
		, m_Objects( Objects )
{
#ifdef _USE_ASSERTS_
	assert( Height > 0 && Width > 0 );
#endif
	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = BLANK;
	//	}
	//}
}

MapContainer::MapContainer()
	//: m_Height(0)
	//, m_Width(0)
	//, m_bCleared(true)
	//, m_pArray(0)
{ }

MapContainer::MapContainer(const MapContainer& mc)
		: m_Array(mc.m_Array)
		//m_Height(mc.m_Height)
		//, m_Width(mc.m_Width)
		//, m_bCleared(false)
		, m_Objects(mc.m_Objects)
{
	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = mc.m_pArray[i][j];
	//	}
	//}
}

MapContainer::MapContainer(MapContainer&& mc)
		: m_Array(std::move( mc.m_Array ))
		//m_Height(mc.m_Height)
		//, m_Width(mc.m_Width)
		//, m_bCleared(false)
		, m_Objects(std::move( mc.m_Objects ))
{
	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = mc.m_pArray[i][j];
	//	}
	//}
}

void MapContainer::operator = (const MapContainer& mc) {

	//for (int i = 0; i < m_Height; ++i) {
	//	delete[] m_pArray[i];
	//}
	//delete[] m_pArray;
	//m_pArray = 0;

	//m_Height = mc.m_Height;
	//m_Width = mc.m_Width;

	m_Objects = mc.m_Objects;
	m_Array = mc.m_Array;

	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = mc.m_pArray[i][j];
	//	}
	//}

	//m_bCleared = false;

}

void MapContainer::operator = (MapContainer&& mc) {

	//for (int i = 0; i < m_Height; ++i) {
	//	delete[] m_pArray[i];
	//}
	//delete[] m_pArray;
	//m_pArray = 0;

	//m_Height = mc.m_Height;
	//m_Width = mc.m_Width;

	m_Objects = std::move (mc.m_Objects);
	m_Array = std::move (mc.m_Array);

	//m_pArray = new char* [m_Height];
	//for (int i = 0; i < m_Height; ++i) {
	//	m_pArray[i] = new char [m_Width];
	//	for (int j = 0; j < m_Width; ++j) {
	//		m_pArray[i][j] = mc.m_pArray[i][j];
	//	}
	//}

	//m_bCleared = false;

}
//
//char*& MapContainer::operator [] (int i) {
//	if (m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to get access to a cleared array", "MapContainer.cpp");
//	}
//#ifdef _USE_ASSERTS_
//	assert( i < m_Height );
//#endif
//	return m_pArray[i];
//}
//
//char* MapContainer::operator [] (int i) const {
//	if (m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to get access to a cleared array", "MapContainer.cpp");
//	}
//#ifdef _USE_ASSERTS_
//	assert( i < m_Height );
//#endif
//	return m_pArray[i];
//}
//
//char& MapContainer::operator() (int i, int j) {
//	if (m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to get access to a cleared array", "MapContainer.cpp");
//	}
//#ifdef _USE_ASSERTS_
//	assert( i < m_Height && j < m_Width);
//#endif
//	return m_pArray[i][j];
//}
//char MapContainer::operator() (int i, int j) const {
//	if (m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to get access to a cleared array", "MapContainer.cpp");
//	}
//#ifdef _USE_ASSERTS_
//	assert( i < m_Height && j < m_Width );
//#endif
//	return m_pArray[i][j];
//}
//char& MapContainer::operator() (CellCoordinates cc) {
//	if (m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to get access to a cleared array", "MapContainer.cpp");
//	}
//#ifdef _USE_ASSERTS_
//	assert( cc.x < m_Height && cc.y < m_Width);
//#endif
//	return m_pArray[cc.x][cc.y];
//}
//char MapContainer::operator() (CellCoordinates cc) const {
//	if (m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to get access to a cleared array", "MapContainer.cpp");
//	}
//#ifdef _USE_ASSERTS_
//	assert( cc.x < m_Height && cc.y < m_Width);
//#endif
//	return m_pArray[cc.x][cc.y];
//}

//void MapContainer::clear() {
//	if (m_bCleared) {
//		return;
//	}
//
//	for (int i = 0; i < m_Height; ++i) {
//		delete[] m_pArray[i];
//	}
//	delete[] m_pArray;
//	m_pArray = 0;
//
//	m_Height = 0;
//	m_Width = 0;
//
//	m_Objects.clear();
//
//	m_bCleared = true;
//}

//void MapContainer::setHeight(char Height) {
//	if (!m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to set height in a non-cleared array", "MapContainer.cpp");
//	}
//	m_Height = Height;
//}