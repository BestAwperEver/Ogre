#ifndef __MAPBINMANAGER_H__
#define __MAPBINMANAGER_H__

#include <OgreResourceManager.h>
#include "MapBin.h"

class MapBinManager : public Ogre::ResourceManager, public Ogre::Singleton<MapBinManager>
{
protected:

	// must implement this from ResourceManager's interface
	Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle, 
		const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader, 
		const Ogre::NameValuePairList *createParams);

public:

	MapBinManager();
	virtual ~MapBinManager();

	//virtual MapBinPtr load(const Ogre::String &name, const Ogre::String &group);

	static MapBinManager &getSingleton();
	static MapBinManager *getSingletonPtr();
};

#endif