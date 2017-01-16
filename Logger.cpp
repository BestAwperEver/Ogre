#include "StdAfx.h"
#include "Logger.h"


Logger::Logger(void)
{
	m_bGameLogCreated = createGameLogFile();
}

Logger::~Logger(void)
{
	if (game_log_file.is_open()) {
		game_log_file.close();
	}
}

Logger& Logger::getInstance() {
	static Logger instance;
	return instance;
}

bool Logger::createGameLogFile() {
	boost::mutex::scoped_lock lock(m_Mutex);

	game_log_file.open(GameLogFilePath);
	if (!game_log_file.is_open()) {
		OgreLog("Can't create " GameLogFilePath " file");
		return false;
	}
	std::time_t t = time(nullptr);
	std::tm* aTm = localtime(&t);
	game_log_file << aTm->tm_mday << "." << std::setw(2) << std::setfill('0') << aTm->tm_mon+1
		<< "." << aTm->tm_year+1900 << std::endl;
	//	delete aTm;
	game_log_file.close();
	return true;
}

void Logger::GameLog(const Ogre::String& msg, bool palki) {
#ifdef ENABLE_GAME_LOG
	if (!m_bGameLogCreated) {
		return;
	}


	boost::mutex::scoped_lock lock(m_Mutex);

	game_log_file.open(GameLogFilePath, std::ios_base::app);

	std::time_t t = time(nullptr);
	std::tm* aTm = localtime(&t);
	game_log_file << std::setw(2) << std::setfill('0') << aTm->tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_min <<":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_sec << ":";
	if (palki) {
		game_log_file << " -- " << msg << " --";
	}
	else {
		game_log_file << " " << msg;
	}
	game_log_file << std::endl;
	//	delete aTm;

	game_log_file.close();

#endif
}
void Logger::OgreLog(const Ogre::String& msg) {
	Ogre::LogManager::getSingletonPtr()->logMessage(msg);
}
void Logger::Log(const Ogre::String& msg) {
	OgreLog(msg);
	GameLog(msg, false);
}

void Logger::GameLog(Ogre::String&& msg, bool palki) {
#ifdef ENABLE_GAME_LOG
	if (!m_bGameLogCreated) {
		return;
	}

	boost::mutex::scoped_lock lock(m_Mutex);

	game_log_file.open(GameLogFilePath, std::ios_base::app);

	std::time_t t = time(nullptr);
	std::tm* aTm = localtime(&t);
	game_log_file << std::setw(2) << std::setfill('0') << aTm->tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_min <<":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_sec << ":";
	if (palki) {
		game_log_file << " -- " << msg << " --";
	}
	else {
		game_log_file << " " << msg;
	}
	game_log_file << std::endl;
	//	delete aTm;

	game_log_file.close();

#endif
}
void Logger::OgreLog(Ogre::String&& msg) {
	Ogre::LogManager::getSingletonPtr()->logMessage(msg);
}
void Logger::Log(Ogre::String&& msg) {
	OgreLog(msg);
	GameLog(msg, false);
}