#pragma once
#ifndef _BASE_MAP_OBJECT_H_
#define _BASE_MAP_OBJECT_H_
#include "array2.h"
class BaseMapObject
{
protected:
	//bool m_bAlwaysVisible;
	bool m_bVisibleInFog;
	bool m_bPassable;

	//array2<bool> m_Mask;
	//bool m_HasNontrivialMask;

public:
	BaseMapObject(bool VisibleInFog, bool Passable);
	virtual ~BaseMapObject(void);
};
#endif
