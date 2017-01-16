#pragma once
#ifndef _MAPBIN_PARSE_H_
#define _MAPBIN_PARSE_H_

#include "MapContainer.h"

class MapBinParser
{
public:
	MapBinParser(void);
	~MapBinParser(void);

	static MapContainer toMapContainer(int H, int W, const std::vector<MapObjectDef>& ObjectsDefs);
};
#endif
