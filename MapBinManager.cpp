#include "MapBinManager.h"

template<> MapBinManager *Ogre::Singleton<MapBinManager>::msSingleton = 0;

MapBinManager *MapBinManager::getSingletonPtr()
{
	return msSingleton;
}

MapBinManager &MapBinManager::getSingleton()
{  
	assert(msSingleton);  
	return(*msSingleton);
}

MapBinManager::MapBinManager()
{
	mResourceType = "MapBin";

	// low, because it will likely reference other resources
	mLoadOrder = 30.0f;

	// this is how we register the ResourceManager with OGRE
	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

MapBinManager::~MapBinManager()
{
	// and this is how we unregister it
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

//MapBinPtr MapBinManager::load(const Ogre::String &name, const Ogre::String &group)
//{
//	MapBinPtr mapptr = getResourceByName(name, group);
//
//	if (mapptr.isNull())
//		mapptr = createResource(name, group);
//
//	mapptr->load();
//	return mapptr;
//}

Ogre::Resource *MapBinManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle, 
	const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader, 
	const Ogre::NameValuePairList *createParams)
{
	return new MapBin(this, name, handle, group, isManual, loader);
}