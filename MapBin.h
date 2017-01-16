#ifndef __MAPBIN_H__
#define __MAPBIN_H__

#include <OgreResourceManager.h>
#include "MapContainer.h"
#include "MapBinSerializer.h"

class MapBin : public Ogre::Resource
{
//	Ogre::String mString;
	MapContainer m_Array;
protected:

	// must implement these from the Ogre::Resource interface
	void loadImpl();
	void unloadImpl();
	size_t calculateSize() const;

public:

	//typedef Ogre::SharedPtr<MapBin> ptr;

	MapBin(Ogre::ResourceManager *creator, const Ogre::String &name, 
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false, 
		Ogre::ManualResourceLoader *loader = 0);

	virtual ~MapBin();

//	void setString(const Ogre::String &str);
	void setArray(const MapContainer&);
//	const Ogre::String &getString() const;
	const MapContainer& getArray() const;
};

//class MapBinPtr : public Ogre::SharedPtr<MapBin> 
//{
//public:
//	MapBinPtr() : Ogre::SharedPtr<MapBin>() {}
//	explicit MapBinPtr(MapBin *rep) : Ogre::SharedPtr<MapBin>(rep) {}
//	MapBinPtr(const MapBinPtr &r) : Ogre::SharedPtr<MapBin>(r) {} 
//	//MapBinPtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<MapBin>()
//	//{
//	//	if( r.isNull() )
//	//		return;
//	//	// lock & copy other mutex pointer
//	//	//OGRE_LOCK_MUTEX((*r).OGRE_AUTO_MUTEX_NAME);
//	//	//	OGRE_COPY_AUTO_SHARED_MUTEX(r->OGRE_AUTO_MUTEX_NAME);
//	//		pRep = static_cast<MapBin*>(r.getPointer());
//	//		this->setUseCount(r.useCount());
//	//		//pUseCount = r.useCountPointer();
//	//	//useFreeMethod = r.freeMethod();
//	//	if (useCount())
//	//	{
//	//		setUseCount(useCount()+1);
//	//	}
//	//}
//
//	/// Operator used to convert a ResourcePtr to a MapBinPtr
//	//MapBinPtr& operator=(const Ogre::ResourcePtr& r)
//	//{
//	//	if(pRep == static_cast<MapBin*>(r.getPointer()))
//	//		return *this;
//	//	release();
//	//	if( r.isNull() )
//	//		return *this;
//	//	pRep = static_cast<MapBin*>(r.getPointer());
//	//	pInfo = r.pInfo;
//	//	r.
//	//	setUseCount(r.useCount());
//	//	if (useCount())
//	//	{
//	//		setUseCount(useCount()+1);
//	//	}
//	//	return *this;
//	//}
//};

#endif