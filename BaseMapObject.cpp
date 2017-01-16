#include "BaseMapObject.h"


BaseMapObject::BaseMapObject(bool VisibleInFog, bool Passable)
	: m_bVisibleInFog(VisibleInFog)
	, m_bPassable(Passable)
{
}


BaseMapObject::~BaseMapObject(void)
{
}
