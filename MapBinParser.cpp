#include "MapBinParser.h"
#include "MapObject.h"

MapBinParser::MapBinParser(void)
{
}
MapBinParser::~MapBinParser(void)
{
}

MapContainer MapBinParser::toMapContainer(int H, int W, const std::vector<MapObjectDef>& vec) {
	MapContainer mc(H,W,vec);

	for (std::vector<MapObjectDef>::iterator it = mc.getObjects().begin(); it !=  mc.getObjects().end(); ++it) {
		for (int i = 0; i < MapObject::StandartObjects[it->objectIndex].getMask().getM(); ++i) {
			for (int j = 0; j < MapObject::StandartObjects[it->objectIndex].getMask().getN(); ++j) {
				mc[it->objectCoordinates.x + i][it->objectCoordinates.y + j] 
					= MapObject::StandartObjects[it->objectIndex].getMask()[i][j];
			}
		}
	}

	return mc;

}