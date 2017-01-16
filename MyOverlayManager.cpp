#include "MyOverlayManager.h"


MyOverlayManager::MyOverlayManager(OgreBites::SdkTrayManager* stm): m_pTrayManager(stm)
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
	m_pTrayManager->frameRenderingQueued(evt);
	return true;
}
