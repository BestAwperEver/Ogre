#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

#define _LOG(x) Logger::getInstance().Log(x)
#define _OGRELOG(x) Logger::getInstance().OgreLog(x)
#define _GAMELOG(x) Logger::getInstance().GameLog(x)
#define _LOG_COMMAND(x) Logger::getInstance().GameLog(x, false)

#define ENABLE_GAME_LOG

#define GameLogFilePath "Game.log"

class Logger
{
	boost::mutex m_Mutex;

	std::ofstream game_log_file;
	bool m_bGameLogCreated;
	Logger(void);

	bool createGameLogFile();
public:

	~Logger(void);

	static Logger& getInstance();

	void GameLog(const Ogre::String& msg, bool only = true);
	void OgreLog(const Ogre::String& msg);
	void Log(const Ogre::String& msg);

	void GameLog(Ogre::String&& msg, bool only = true);
	void OgreLog(Ogre::String&& msg);
	void Log(Ogre::String&& msg);

};

#endif
