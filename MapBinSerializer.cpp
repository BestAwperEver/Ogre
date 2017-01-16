#include "MapBinSerializer.h"
#include "MapBin.h"
#include "MapBinParser.h"

MapBinSerializer::MapBinSerializer()
{
}

MapBinSerializer::~MapBinSerializer()
{
}

void MapBinSerializer::exportMapBin(const MapBin *pMap, const Ogre::String &fileName)
{

	std::ofstream f(fileName, std::ios::trunc | std::ios::binary);

	//if (!f.is_open()) 
	//	return false;

	char H = pMap->getArray().getHeight();
	char W = pMap->getArray().getWidth();
	f.write(&H, 1);
	f.write(&W, 1);

	for (int i = 0; i < H; i++) {
		f.write(pMap->getArray()[i], W);
		//if (f.bad()) {
		//	return false;
		//}
	}

	f.close();

}

void MapBinSerializer::importMapBin(Ogre::DataStreamPtr &stream, MapBin *pDest)
{
	char H,W; short OC; // height, width, object count

	stream->read(&H,1);
	stream->read(&W,1);
	stream->read((char*)(&OC),sizeof(short));

	std::vector<MapObjectDef> vec(OC);

	CellCoordinates Oc; // object coordinates
	char OI; // object index

	for (int i = 0; i < OC; ++i) {
		stream->read(&Oc.x,1);
		stream->read(&Oc.y,1);
		stream->read(&OI,1);
		vec[i] = MapObjectDef( Oc, MAP_OBJECT_INDEX( OI ));
	}

	pDest->setArray(MapBinParser::toMapContainer(H,W,vec));
}