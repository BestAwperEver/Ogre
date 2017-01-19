#pragma once
#ifndef _CLIENT_DATABASE_H_
#define _CLIENT_DATABASE_H_

#define _CLDATA ClientDataBase::getInstance()

#include "HeroDef.h"
#include "..\..\Ogre2\Ogre2\shared_enums.h"
#include <string>

class ClientDataBase {

	typedef std::string String;
	bool do_compare(char reg[], const std::string& con) const;

	ClientDataBase();

public:

	~ClientDataBase();
	static ClientDataBase& getInstance();

public:
	int getClientID(const String& client_name) const;
	HeroDef getClientStartHero(int client_id) const;
	unsigned int getClientsCount() const;
	String getClientName(int client_id) const;
};

#endif