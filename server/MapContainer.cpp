#include "MapContainer.h"
//void MapContainer::setHeight(char Height) {
//	if (!m_bCleared) {
//		throw Ogre::Exception(843, "MapContainer: trying to set height in a non-cleared array", "MapContainer.cpp");
//	}
//	m_Height = Height;
//}
MapContainer::MapContainer(int Height, int Width):
		m_Height(Height)
		, m_Width(Width)
		, m_bCleared(false)
{
#ifdef _USE_ASSERTS_
	assert( Height > 0 && Width > 0 );
#endif
	m_pArray = new char* [m_Height];
	for (int i = 0; i < m_Height; ++i) {
		m_pArray[i] = new char [m_Width];
		for (int j = 0; j < m_Width; ++j) {
			m_pArray[i][j] = 0;
		}
	}
}

MapContainer::MapContainer(int Height, int Width, int ObjectCount):
		m_Height(Height)
		, m_Width(Width)
		, m_bCleared(false)
		, m_Objects(ObjectCount)
{
#ifdef _USE_ASSERTS_
	assert( Height > 0 && Width > 0 );
#endif
	m_pArray = new char* [m_Height];
	for (int i = 0; i < m_Height; ++i) {
		m_pArray[i] = new char [m_Width];
		for (int j = 0; j < m_Width; ++j) {
			m_pArray[i][j] = static_cast<char>(MAP_SYMBOL::BLANK);
		}
	}
}

MapContainer::MapContainer(int Height, int Width, std::vector<MapObjectDef> Objects):
		m_Height(Height)
		, m_Width(Width)
		, m_bCleared(false)
		, m_Objects(Objects)
{
#ifdef _USE_ASSERTS_
	assert( Height > 0 && Width > 0 );
#endif
	m_pArray = new char* [m_Height];
	for (int i = 0; i < m_Height; ++i) {
		m_pArray[i] = new char [m_Width];
		for (int j = 0; j < m_Width; ++j) {
			m_pArray[i][j] = static_cast<char>(MAP_SYMBOL::BLANK);
		}
	}
}

MapContainer::MapContainer():
	m_Height(0)
	, m_Width(0)
	, m_bCleared(true)
	, m_pArray(0)
{ }

MapContainer::MapContainer(const MapContainer& mc):
	m_Height(mc.m_Height)
	, m_Width(mc.m_Width)
	, m_bCleared(false)
	, m_Objects(mc.m_Objects)
{
	m_pArray = new char* [m_Height];
	for (int i = 0; i < m_Height; ++i) {
		m_pArray[i] = new char [m_Width];
		for (int j = 0; j < m_Width; ++j) {
			m_pArray[i][j] = mc.m_pArray[i][j];
		}
	}
}

MapContainer::MapContainer(MapContainer&& mc):
	m_Height(mc.m_Height)
	, m_Width(mc.m_Width)
	, m_bCleared(false)
	, m_Objects( std::move(mc.m_Objects) )
	, m_pArray( mc.m_pArray )
{
	mc.m_pArray = 0;
	mc.m_bCleared = true;
}

MapContainer::~MapContainer(void)
{
	clear();
}

void MapContainer::operator = (const MapContainer& mc) {

	for (int i = 0; i < m_Height; ++i) {
		delete[] m_pArray[i];
	}
	delete[] m_pArray;
	m_pArray = 0;

	m_Height = mc.m_Height;
	m_Width = mc.m_Width;

	m_Objects = mc.m_Objects;

	m_pArray = new char* [m_Height];
	for (int i = 0; i < m_Height; ++i) {
		m_pArray[i] = new char [m_Width];
		for (int j = 0; j < m_Width; ++j) {
			m_pArray[i][j] = mc.m_pArray[i][j];
		}
	}

	m_bCleared = false;

}
void MapContainer::operator = (MapContainer&& mc) {


	m_Height = mc.m_Height;
	m_Width = mc.m_Width;

	m_Objects = std::move( mc.m_Objects );

	m_pArray = mc.m_pArray;

	mc.m_pArray = 0;
	mc.m_bCleared = true;

	m_bCleared = false;
}

char*& MapContainer::operator [] (int i) {
#ifdef _USE_ASSERTS_
	assert( i < m_Height );
	assert( !m_bCleared	);
#endif
	return m_pArray[i];
}
char* MapContainer::operator [] (int i) const {
#ifdef _USE_ASSERTS_
	assert( i < m_Height );
	assert( !m_bCleared );
#endif
	return m_pArray[i];
}

char& MapContainer::operator() (int i, int j) {
#ifdef _USE_ASSERTS_
	assert( i < m_Height && j < m_Width);
	assert( !m_bCleared );
#endif
	return m_pArray[i][j];
}
char MapContainer::operator() (int i, int j) const {
#ifdef _USE_ASSERTS_
	assert( i < m_Height && j < m_Width );
	assert( !m_bCleared );
#endif
	return m_pArray[i][j];
}
char& MapContainer::operator() (CellCoordinates cc) {
#ifdef _USE_ASSERTS_
	assert( cc.x < m_Height && cc.y < m_Width);
	assert( !m_bCleared );
#endif
	return m_pArray[cc.x][cc.y];
}
char MapContainer::operator() (CellCoordinates cc) const {
#ifdef _USE_ASSERTS_
	assert( cc.x < m_Height && cc.y < m_Width);
	assert( !m_bCleared );
#endif
	return m_pArray[cc.x][cc.y];
}

void MapContainer::clear() {
	if (m_bCleared) {
		return;
	}

	for (int i = 0; i < m_Height; ++i) {
		delete[] m_pArray[i];
	}
	delete[] m_pArray;
	m_pArray = 0;

	m_Height = 0;
	m_Width = 0;

	m_Objects.clear();

	m_bCleared = true;
}