#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>

#define _LOG(x) Logger::getInstance().Log(x)
#define _CONCOLELOG(x) Logger::getInstance().ConsoleLog(x)
#define _FILELOG(x) Logger::getInstance().FileLog(x)

#define ENABLE_GAME_LOG

#define LogFilePath "Server.log"

class Logger
{
	boost::mutex m_Mutex;

	std::ofstream log_file;
	bool m_bGameLogCreated;
	Logger(void);

	bool createLogFile();
public:

	~Logger(void);

	static Logger& getInstance();

	void FileLog(const std::string& msg);
	void ConsoleLog(const std::string& msg);
	void Log(const std::string& msg);

	void FileLog(std::string&& msg);
	void ConsoleLog(std::string&& msg);
	void Log(std::string&& msg);

};

#endif