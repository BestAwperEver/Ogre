#pragma once
#ifndef _CLIENT_DATABASE_H_
#define _CLIENT_DATABASE_H_

#define _CLDATA ClientDataBase::getInstance()

#include "UnitDef.h"
#include "shared_enums.h"
#include <string>
#include <map>
#include <locale>
#include "DataBase.h"
#include <boost\shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../../Server0/Server0/Merk.h"

/*
	Часть бд, используемая только сервером, называется клиент ибо данные о них
*/

class ClientDataBase {

	typedef std::string String;
	bool do_compare(char reg[], const std::string& con) const;

	ClientDataBase();

	UnitDef getClientStartHero(int client_id) const;

public:

	~ClientDataBase();
	static ClientDataBase& getInstance();

public:
	int getClientID(const String& client_name) const;
	// возвращает список и максимальный айдишник
	std::pair<std::map<int, Merk::ptr>, int> getClientMerks(int client_id) const;
	void saveClientMerks(int client_id, std::map<int, Merk::ptr> merks) const;
	unsigned int getClientsCount() const;
	String getClientName(int client_id) const;
};

#endif