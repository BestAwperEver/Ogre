#include "MyOverlayManager.h"


MyOverlayManager::MyOverlayManager(OgreBites::TrayManager* stm): m_pTrayManager(stm)
{
#ifdef _USE_ASSERTS_
	assert( stm );
#endif
	Ogre::Root::getSingleton().addFrameListener(this);
}

MyOverlayManager::~MyOverlayManager(void)
{
	Ogre::Root::getSingleton().removeFrameListener(this);
}

bool MyOverlayManager::frameRenderingQueued(const Ogre::FrameEvent& evt) {
#if OGRE_VERSION == ((2 << 16) | (0 << 8) | 0)
	m_pTrayManager->frameRenderingQueued(evt);
#else
	m_pTrayManager->frameRendered(evt);
#endif
	return true;
}
