#include "ClientDataBase.h"

const int ClientsCount = 6;
char* registered_clients[] = {"Radagast", "Patamaniack", "Dart", "Imran", "Lanin", "Redgar", 0};

ClientDataBase::ClientDataBase() {}
ClientDataBase::~ClientDataBase() {}

ClientDataBase& ClientDataBase::getInstance() {
	static ClientDataBase instance;
	return instance;
}

bool ClientDataBase::do_compare(char reg[], const std::string& con) const {
	int sz = strlen(reg);
	if (sz != con.size()) {
		return false;
	}
	for (char* ch = reg, k = 0; *ch; ++ch, ++k) {
		if (std::tolower(con[k]) != std::tolower(*ch)) {
			return false;
		}
	}
	return true;
}

int ClientDataBase::getClientID(const ClientDataBase::String& client_name) const {
	for (char **ch = registered_clients, k = 0; *ch; ++ch, ++k) {
		if (do_compare(*ch,client_name)) {
			return k+1;
		}
	}
	return -1;
}
HeroDef ClientDataBase::getClientStartHero(int client_id) const {
#ifdef _USE_ASSERTS_
	assert( client_id > 0 && client_id <= ClientsCount );
#endif
	switch (client_id) {
	case 1:
		return HeroDef(ROBOT, WARRIOR, BOW, CHAIN_MAIL, 10, 25, CellCoordinates(9,25));
	case 2:
		return HeroDef(HUMAN, MAGE, SWORD, CLOAK, 5, 25 - client_id);
	case 3:
		return HeroDef(GNOME, MONK, SWORD, CHAIN_MAIL, 5, 25 - client_id);
	case 4:
		return HeroDef(HALFRING, ROGUE, BOW, CLOAK, 5, 25 - client_id);
	case 5:
		return HeroDef(HUMAN, MAGE, SWORD, CLOAK, 5, 25 - client_id);
	case 6:
		return HeroDef(DWARF, WARRIOR, SWORD, CHAIN_MAIL, 5, 25 - client_id);
	}
}
ClientDataBase::String ClientDataBase::getClientName(int client_id) const {
	return registered_clients[client_id-1];
}
unsigned int ClientDataBase::getClientsCount() const {
	return ClientsCount;
}