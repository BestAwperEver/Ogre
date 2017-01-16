// stdafx.h
#define _USE_ASSERTS_

#define ABSTRACT_UNIT_QUERY_FLAG 1
#define ROBOT_QUERY_FLAG 2
#define NINJA_QUERY_FLAG 4

#include <deque>
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <OgreException.h>
#include <Ogre.h>
#include <OgreRoot.h>
#include <OgreConfigFile.h>

#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreSingleton.h>
#include <OgreSharedPtr.h>
#include <OgreString.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
//#include <OgreMeshLodGenerator.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

//#include <CEGUI/CEGUI.h>
//#include <CEGUI/SchemeManager.h>
//#include <CEGUI/RendererModules/Ogre/Renderer.h>
//#include "TimerManager.h"

#include <SdkTrays.h>
#include <SdkCameraMan.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif