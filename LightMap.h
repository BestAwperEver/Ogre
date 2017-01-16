#pragma once
#ifndef _LIGHT_MAP_H_
#define _LIGHT_MAP_H_

#include "..\..\Database0\Database0\CellCoordinates.h"
#include "array2.h"
#include <map>

//#define FORCE_MAKE_VISIBLE
//#define TOUCH_MASK_BOTH // это значит, что оба алгоритма должны увидеть клетку

class GameMap;

class LightMap {
public:

	enum LM_CAST_TYPE {
		LM_RAYCAST,
		LM_SHADOWCAST
	};

	~LightMap() {}
	LightMap(const GameMap& map, char min_range, char max_range,
		LM_CAST_TYPE ct = LM_RAYCAST);
public:

	char getCastType() const {return m_CastType;}
	void setCastType(LM_CAST_TYPE ct) {m_CastType = ct;}

	array2<bool>& getLightMap(char i, char j, char range) {return m_LightMaps(i,j)[range];}
	array2<bool>& getLightMap(CellCoordinates cc, char range) {return m_LightMaps(cc.x,cc.y)[range];}

//	const array2<bool>& getLightMapConstRef(char i, char j, char range) const {return m_LightMaps(i,j)[range];}
//	const array2<bool>& getLightMapConstRef(CellCoordinates cc, char range) const {return m_LightMaps(cc.x,cc.y)[range];}
	
	array2<bool>* getLightMapPtr(char i, char j, char range) {return &m_LightMaps(i,j)[range];}
	array2<bool>* getLightMapPtr(CellCoordinates cc, char range) {return &m_LightMaps(cc.x,cc.y)[range];}
	
	array2<bool> getLightMapTempObj(char i, char j, char range) const {return m_LightMaps(i,j)[range];}
	array2<bool> getLightMapTempObj(CellCoordinates cc, char range) const {return m_LightMaps(cc.x,cc.y)[range];}

private:
	typedef unsigned char uchar;

	// raycasting
	inline void make_light_map(char cell_x, char cell_y, char range, bool allow_diagonal_steps,
		uchar touch_mask, array2<uchar>& light_map);
	bool cast_ray(char cell_x, char cell_y, char range, char dir_x, char dir_y, bool allow_diagonal_steps,
		uchar touch_mask, array2<uchar>& light_map);
	bool visible(char from_x, char from_y, char range, char dir_x, char dir_y, bool allow_diagonal_steps);
	// shadowcasting
	inline void make_light_map(char cell_x, char cell_y, char range, 
		uchar touch_mask, array2<uchar>& light_map);
	void ScanOctant(char cell_x, char cell_y, char range, 
		uchar touch_mask, array2<uchar>& light_map,
		int pDepth, int pOctant, double pStartSlope, double pEndSlope);
	inline int GetVisDistance(char x1, char y1, char x2, char y2);
	inline double GetSlope(double pX1, double pY1, double pX2, double pY2, bool pInvert);

	inline void execute_light_map(char cell_x, char cell_y, char range,
		uchar touch_mask, array2<uchar>& light_map);
private:
	array2<std::map<char, array2<bool>>> m_LightMaps;
	const GameMap& map;
	char m_CastType;
};

#endif