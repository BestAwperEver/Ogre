#ifndef __MAPBIN_SERIALIZER_H__
#define __MAPBIN_SERIALIZER_H__

#include <OgreSerializer.h>

class MapBin; // forward declaration

class MapBinSerializer : public Ogre::Serializer
{
public:
	MapBinSerializer();
	virtual ~MapBinSerializer();

	void exportMapBin(const MapBin *pText, const Ogre::String &fileName);
	void importMapBin(Ogre::DataStreamPtr &stream, MapBin *pDest);
};

#endif