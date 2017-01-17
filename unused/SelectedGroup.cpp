#include "SelectedGroup.h"


SelectedGroup::SelectedGroup(void)
{
}


SelectedGroup::~SelectedGroup(void)
{
	deselectUnits();
}

void SelectedGroup::addUnit(AbstractUnit* u) {
	m_Selected.push_back(u);
}

bool SelectedGroup::removeUnit(AbstractUnit* u) {
	m_Selected.remove(u);
	return true;
}

void SelectedGroup::deselectUnits() {
	std::list<AbstractUnit*>::iterator iter;
 
	for (iter = m_Selected.begin(); iter != m_Selected.end(); iter++)
	{
		(*iter)->setActive(false);
	}

	m_Selected.clear();
}

Ogre::int32 SelectedGroup::size() const {
	return m_Selected.size();
}

Ogre::Vector3 SelectedGroup::getGroupCenter() {
#ifdef _USE_ASSERTS_
	assert ( !m_Selected.empty() );
#endif
	Ogre::Vector3 center = Ogre::Vector3::ZERO;
	for (std::list<AbstractUnit*>::iterator iter = m_Selected.begin(); iter != m_Selected.end(); iter++) {
		center += (*iter)->getPosition();
	}
	return center/m_Selected.size();
}

void SelectedGroup::moveSelectedTo(Ogre::Vector3 dest, MoveMode moveMode) {
if (m_Selected.size() == 0) {
	return;
}
if (m_Selected.size() == 1) {
		(*m_Selected.begin())->moveTo(dest);
	}
	else {
		switch (moveMode) {

		case SaveRelativePosition:	{
			Ogre::Vector3 center = getGroupCenter();
			for (std::list<AbstractUnit*>::iterator iter = m_Selected.begin(); iter != m_Selected.end(); iter++) {
				(*iter)->moveTo(dest + ((*iter)->getPosition() - center));
			}
		} break;

		default:
			break;
		}
	}
}

void SelectedGroup::setActive(bool set) {
	m_Active = set;
}