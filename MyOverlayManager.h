#pragma once

#ifndef _My_Overlay_Manager_
#define _My_Overlay_Manager_

class MyOverlayManager: public Ogre::FrameListener
{
	OgreBites::TrayManager*	m_pTrayManager;
public:
	MyOverlayManager(OgreBites::TrayManager*);
	~MyOverlayManager(void);

	bool frameRenderingQueued(const Ogre::FrameEvent&);
};

#endif