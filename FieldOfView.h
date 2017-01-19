#pragma once
#ifndef _FIELD_OF_VIEW_
#define _FIELD_OF_VIEW_

class GameMap;
#include "array2.h"

class FieldOfView: public array2<char> {
	GameMap* m_pGameMap;
public:
	FieldOfView(GameMap* pGameMap, char DefaultEl = -1);
	FieldOfView(GameMap* game_map, char Height, char Width, char DefaultEl = -1);
	FieldOfView(const FieldOfView&) = default;
	FieldOfView(FieldOfView&&);
	void operator = (FieldOfView&&);
	void decrement(CellCoordinates where, const array2<bool>& LightMask, bool affect_gamemap = true);
	void increment(CellCoordinates where, const array2<bool>& LightMask, bool affect_gamemap = true);
	// for internal use, but not dangerous
	char getRange(const array2<bool>& LightMask) const { return (LightMask.getM() - 1)/2; }
};

#endif