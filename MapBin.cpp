#include "MapBin.h"

MapBin::MapBin(Ogre::ResourceManager* creator, const Ogre::String &name, 
	Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual, 
	Ogre::ManualResourceLoader *loader) :
Ogre::Resource(creator, name, handle, group, isManual, loader)
{
	/* If you were storing a pointer to an object, then you would set that pointer to NULL here.
	*/

	/* For consistency with StringInterface, but we don't add any parameters here
	That's because the Resource implementation of StringInterface is to
	list all the options that need to be set before loading, of which 
	we have none as such. Full details can be set through scripts.
	*/ 
	createParamDictionary("MapBin");
}

MapBin::~MapBin()
{
	unload();
}

void MapBin::loadImpl()
{
	MapBinSerializer serializer;
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
	serializer.importMapBin(stream, this);
}

void MapBin::unloadImpl()
{
	/* If you were storing a pointer to an object, then you would check the pointer here,
	and if it is not NULL, you would destruct the object and set its pointer to NULL again.
	*/

	m_Array.clear();
}

size_t MapBin::calculateSize() const
{
	return 4 + 4 * m_Array.getHeight() + (m_Array.getHeight() * m_Array.getWidth()) \
		+ m_Array.getObjectsConstRef().capacity() * sizeof(MapObjectDef) + sizeof(char);
	/* указатель на массив с указателями чаров
	указатели на массивы с чарами
	сами чары
	то, что занимает хлам из вектора
	булевская переменная (вроде байт весит) */
}

void MapBin::setArray(const MapContainer& mc)
{
	m_Array = mc;
}

const MapContainer& MapBin::getArray() const
{
	return m_Array;
}