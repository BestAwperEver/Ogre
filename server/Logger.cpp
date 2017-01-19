#include "StdAfx.h"
#include "Logger.h"


Logger::Logger(void)
{
	m_bGameLogCreated = createLogFile();
}

Logger::~Logger(void)
{
	if (log_file.is_open()) {
		log_file.close();
	}
}

Logger& Logger::getInstance() {
	static Logger instance;
	return instance;
}

bool Logger::createLogFile() {
	log_file.open(LogFilePath);
	if (!log_file.is_open()) {
		ConsoleLog("Can't create " LogFilePath " file");
		return false;
	}
	std::time_t t = time(nullptr);
	std::tm zaTm;
	std::tm* aTm = &zaTm;
	localtime_s(aTm, &t);
	log_file << aTm->tm_mday << "." << std::setw(2) << std::setfill('0') << aTm->tm_mon+1
		<< "." << aTm->tm_year+1900 << std::endl;
//	delete aTm;
	log_file.close();
	return true;
}

void Logger::FileLog(const std::string& msg) {
#ifdef ENABLE_GAME_LOG
	if (!m_bGameLogCreated) {
		return;
	}

	boost::mutex::scoped_lock(m_Mutex);

	log_file.open(LogFilePath, std::ios_base::app);

	std::time_t t = time(nullptr);
	std::tm zaTm;
	std::tm* aTm = &zaTm;
	localtime_s(aTm, &t);
	log_file << std::setw(2) << std::setfill('0') << aTm->tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_min <<":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_sec << ":" << " " << msg << std::endl;
//	delete aTm;

	log_file.close();
#endif
}
void Logger::ConsoleLog(const std::string& msg) {
	boost::mutex::scoped_lock(m_Mutex);
	std::cout << msg << std::endl;
}
void Logger::Log(const std::string& msg) {
	ConsoleLog(msg);
	FileLog(msg);
}

void Logger::FileLog(std::string&& msg) {
#ifdef ENABLE_GAME_LOG
	if (!m_bGameLogCreated) {
		return;
	}

	boost::mutex::scoped_lock(m_Mutex);

	log_file.open(LogFilePath, std::ios_base::app);

	std::time_t t = time(nullptr);
	std::tm zaTm;
	std::tm* aTm = &zaTm;
	localtime_s(aTm, &t);
	log_file << std::setw(2) << std::setfill('0') << aTm->tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_min <<":"
		<< std::setw(2) << std::setfill('0') << aTm->tm_sec << ":" << " " << msg << std::endl;
//	delete aTm;

	log_file.close();
#endif
}
void Logger::ConsoleLog(std::string&& msg) {
	boost::mutex::scoped_lock(m_Mutex);
	std::cout << msg << std::endl;
}
void Logger::Log(std::string&& msg) {
	ConsoleLog(msg);
	FileLog(msg);
}