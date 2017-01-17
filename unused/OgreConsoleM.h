#pragma once
#ifndef _Ogre_Console_h_
#define _Ogre_Console_h_

class MyManager : public Ogre::Singleton<MyManager>
{
public:
    MyManager();
    void doSomething();
    static MyManager& getSingleton(void);
    static MyManager* getSingletonPtr(void);
};


#include <list>
#include <vector>
#include "GameMap.h"
 
class OgreConsoleM: public Ogre::Singleton<OgreConsoleM>, Ogre::FrameListener, Ogre::LogListener
{
private:
	bool	m_bVisible;
	bool	m_bInitialized;

	Ogre::Root				*m_pRoot;
	Ogre::SceneManager		*m_pSceneManager;
	Ogre::Rectangle2D		*m_pRect;

	Gorilla::Rectangle		*m_pDecoration;

	Ogre::SceneNode			*m_pNode;
	Ogre::OverlayElement	*m_pTextbox;
	Ogre::Overlay			*m_pOverlay;
 
	Ogre::Real	m_fHeight;
	bool		m_bUpdateOverlay;
	int			m_nStartLine;

	std::list<Ogre::String>		m_Lines;
	Ogre::String				m_sPrompt;

	std::map<Ogre::String, void (*)(std::vector<Ogre::String>&)>	commands;
public:
	OgreConsoleM();
	~OgreConsoleM();

	//OgreConsoleM& operator << (int);
	//OgreConsoleM& operator << (double);
	//OgreConsoleM& operator << (float);
	OgreConsoleM& operator << (const Ogre::String&);
	OgreConsoleM& operator << (const GameMap&);

	void init(Ogre::Root *root);
	void shutdown();
 
	void setVisible(bool visible);
	bool isVisible(){return m_bVisible;}
 
	void print(const Ogre::String &text);
 
	bool frameStarted(const Ogre::FrameEvent &evt);
	bool frameEnded(const Ogre::FrameEvent &evt);
 
	void injectKeyDown(const OIS::KeyEvent&);
 
	void addCommand(const Ogre::String &command, void (*)(std::vector<Ogre::String>&));
	void removeCommand(const Ogre::String &command);
 
	////log
	void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel, bool maskDebug, const Ogre::String &logName, bool& skip) {
		print(logName + ": " + message);
	}
};

#endif