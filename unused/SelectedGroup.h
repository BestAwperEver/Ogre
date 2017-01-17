#pragma once
#ifndef _Selected_Group_h_
#define _Selected_Group_h_

#include "AbstractUnit.h"

extern std::list<AbstractUnit*> g_Units;

class SelectedGroup {
private:
	std::list<AbstractUnit*> m_Selected;
	bool m_Active;
public:
	enum MoveMode {
		SaveRelativePosition,
		CompactGroup,
		ThinnedGroup,
		END
	};
public:
	SelectedGroup(void);
	~SelectedGroup(void);
public:
	void addUnit(AbstractUnit*);
	bool removeUnit(AbstractUnit*);
	void deselectUnits();
	Ogre::int32 size() const;

	void setActive(bool set);

	Ogre::Vector3 getGroupCenter();

	void moveSelectedTo(Ogre::Vector3 Destination, MoveMode = SaveRelativePosition);
};

#endif