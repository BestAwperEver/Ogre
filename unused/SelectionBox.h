#ifndef __SelectionBox_h_
#define __SelectionBox_h_
 
#include "OgreManualObject.h"
 
class SelectionBox : public Ogre::ManualObject
{
	Ogre::Vector2 m_v2Start;
	Ogre::Vector2 m_v2Stop;

	bool m_bSelecting;
public :
	SelectionBox(const Ogre::String& name);
	~SelectionBox(void);
 
	void setCorners(float left, float top, float right, float bottom);
	void setCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight);

	Ogre::Vector2 getStart() const {
		return m_v2Start;
	}
	Ogre::Vector2 getStop() const {
		return m_v2Stop;
	}

	bool Selecting() const {
		return m_bSelecting;
	}

	void startSelection(Ogre::Vector2);
	void update(Ogre::Vector2);
	void finishSelection();
};
 
#endif