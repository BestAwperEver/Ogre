#include "ClientDataBase.h"
#include <cassert>
#include <fstream>

int ClientsCount = 7;
char* registered_clients[] = {"Radagast", "Patamaniack", "Dart", "Imran", "Lanin", "Redgar", "lxpbl", 0};
std::map<std::string, int> map_login_id; // костыль; по идее должно быть в базе
bool map_login_id_loaded = false;

ClientDataBase::ClientDataBase() {}
ClientDataBase::~ClientDataBase() {}

ClientDataBase& ClientDataBase::getInstance() {
	static ClientDataBase instance;
	// костыли-костыльчики
	if (!map_login_id_loaded) {
		std::ifstream clf("clients.txt");
		if (!clf.is_open()) {
			assert(false);
		}
		clf >> ClientsCount;
		std::string cl;
		for (int i = 0; i < ClientsCount; ++i) {
			clf >> cl;
			map_login_id[cl] = i;
		}
		map_login_id_loaded = true;
	}
	return instance;
}

bool ClientDataBase::do_compare(char reg[], const std::string& con) const {
	std::locale loc;
	int sz = strlen(reg);
	if (sz != con.size()) {
		return false;
	}
	for (char* ch = reg, k = 0; *ch; ++ch, ++k) {
		if (std::tolower(con[k], loc) != std::tolower(*ch, loc)) {
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
UnitDef ClientDataBase::getClientStartHero(int client_id) const {
#ifdef _USE_ASSERTS_
	assert( client_id > 0 && client_id <= ClientsCount );
#endif
	UnitDef u;
	u.PlayerID = client_id;
	switch (client_id) {
	case 1:
		//return UnitDef(static_cast<char>(UNIT_TYPE::ROBOT), static_cast<char>(HERO_CLASS::WARRIOR), static_cast<char>(WEAPON::CROSSBOW), static_cast<char>(ARMOR::CHAIN_MAIL), 10, 29, CellCoordinates(9,25));
		u.type_id = static_cast<char>(UNIT_TYPE::ROBOT);
		u.Class = static_cast<char>(HERO_CLASS::WARRIOR);
		u.Weapon = static_cast<char>(WEAPON::CROSSBOW);
		u.Armor = static_cast<char>(ARMOR::CHAIN_MAIL);
		break;
	case 2:
		u.type_id = static_cast<char>(UNIT_TYPE::HUMAN);
		u.Class = static_cast<char>(static_cast<char>(HERO_CLASS::MAGE));
		u.Weapon = static_cast<char>(WEAPON::SWORD);
		u.Armor = static_cast<char>(ARMOR::CLOAK);
		//return UnitDef(static_cast<char>(UNIT_TYPE::HUMAN), static_cast<char>(HERO_CLASS::MAGE), static_cast<char>(WEAPON::SWORD), static_cast<char>(ARMOR::CLOAK), 8, 29);
		break;
	case 3:
		u.type_id = static_cast<char>(UNIT_TYPE::GNOME);
		u.Class = static_cast<char>(static_cast<char>(HERO_CLASS::MONK));
		u.Weapon = static_cast<char>(WEAPON::SWORD);
		u.Armor = static_cast<char>(ARMOR::CHAIN_MAIL);
		//return UnitDef(static_cast<char>(UNIT_TYPE::GNOME), static_cast<char>(HERO_CLASS::MONK), static_cast<char>(WEAPON::SWORD), static_cast<char>(ARMOR::CHAIN_MAIL), 2 + client_id, 29);
		break;
	case 4:
		u.type_id = static_cast<char>(UNIT_TYPE::HALFLING);
		u.Class = static_cast<char>(static_cast<char>(HERO_CLASS::ROGUE));
		u.Weapon = static_cast<char>(WEAPON::BOW);
		u.Armor = static_cast<char>(ARMOR::CLOAK);
		//return UnitDef(static_cast<char>(UNIT_TYPE::HALFLING), static_cast<char>(HERO_CLASS::ROGUE), static_cast<char>(WEAPON::BOW), static_cast<char>(ARMOR::CLOAK), 2 + client_id, 29);
		break;
	case 5:
		u.type_id = static_cast<char>(UNIT_TYPE::HUMAN);
		u.Class = static_cast<char>(HERO_CLASS::MAGE);
		u.Weapon = static_cast<char>(WEAPON::SWORD);
		u.Armor = static_cast<char>(ARMOR::CLOAK);
		//return UnitDef(static_cast<char>(UNIT_TYPE::HUMAN), static_cast<char>(HERO_CLASS::MAGE), static_cast<char>(WEAPON::SWORD), static_cast<char>(ARMOR::CLOAK), 11 + client_id, 4);
		break;
	case 6:
		u.type_id = static_cast<char>(UNIT_TYPE::DWARF);
		u.Class = static_cast<char>(HERO_CLASS::WARRIOR);
		u.Weapon = static_cast<char>(WEAPON::SWORD);
		u.Armor = static_cast<char>(ARMOR::CHAIN_MAIL);
		//return UnitDef(static_cast<char>(UNIT_TYPE::DWARF), static_cast<char>(HERO_CLASS::WARRIOR), static_cast<char>(WEAPON::SWORD), static_cast<char>(ARMOR::CHAIN_MAIL), 11 + client_id, 4);
		break;
	case 7:
		u.type_id = static_cast<char>(UNIT_TYPE::HUMAN);
		u.Class = static_cast<char>(HERO_CLASS::ROGUE);
		u.Weapon = static_cast<char>(WEAPON::CROSSBOW);
		u.Armor = static_cast<char>(ARMOR::ARMOR_NONE);
		//return UnitDef(static_cast<char>(UNIT_TYPE::HUMAN), static_cast<char>(HERO_CLASS::ROGUE), static_cast<char>(WEAPON::CROSSBOW), static_cast<char>(ARMOR::ARMOR_NONE), 11 + client_id, 4);
		break;
	}
	u.AP = _DATA.getDefaultAP(u.type_id);
	u.HP = _DATA.getDefaultHP(u.type_id);
	u.ViewRange = _DATA.getDefaultViewRange(u.type_id);
	return u;
}
ClientDataBase::String ClientDataBase::getClientName(int client_id) const {
	return registered_clients[client_id-1];
}
unsigned int ClientDataBase::getClientsCount() const {
	return ClientsCount;
}
std::pair<std::map<int, Merk::ptr>, int> ClientDataBase::getClientMerks(int player_id) const {
	std::ifstream f("merks\\" + getClientName(player_id) + ".txt");
	if (!f.is_open()) {
		std::ofstream of("merks\\" + getClientName(player_id) + ".txt");
		if (!of.is_open()) {
			int a = 5;
		}
		UnitDef ud = getClientStartHero(player_id);
		of << 1 << std::endl; // количество
		of << 1 << std::endl; // айдишник
		of << getClientName(player_id) << std::endl;
		of << _DATA.getTypeString(ud.type_id) << std::endl;
		of << _DATA.getWeaponString(ud.Weapon) << std::endl;
		of.close();
		std::map<int, Merk::ptr> merks;
		Merk::ptr merk = Merk::create(player_id, 1, getClientName(player_id), ud.type_id);
		merks[1] = merk;
		return std::pair<std::map<int, Merk::ptr>, int>(std::move( merks ), 1);
	}
	std::map<int, Merk::ptr> merks;
	int max_id = 0;
	int count;
	f >> count;
	for (int j = 0; j < count; ++j) {
		int merk_id;
		f >> merk_id;
		if (merk_id > max_id) max_id = merk_id;
		std::string name;
		f >> name;
		std::string type;
		f >> type;
		std::string weapon;
		f >> weapon;
		char type_id = _DATA.getTypeID(type);
		UnitDef ud = _DATA.getDefaultUD(type_id);
		char weapon_id = _DATA.getWeaponID(weapon);
		ud.Weapon = weapon_id;
		ud.PlayerID = player_id;
		Merk::ptr merk = Merk::create(merk_id, name, ud);
		//merk.set_id(merk_id);
		//merk.set_name(name);
		//merk.set_ud(ud);
		merks[merk_id] = merk;
	}
	f.close();
	return std::pair<std::map<int, Merk::ptr>, int>(std::move( merks ), max_id);
}
void ClientDataBase::saveClientMerks(int client_id, std::map<int, Merk::ptr> merks) const {
	std::ofstream of("merks\\" + getClientName(client_id) + ".txt");
	if (!of.is_open()) {
		int a = 5;
	}
	of << merks.size() << std::endl;
	for (auto it = merks.begin(); it != merks.end(); ++it) {
		of << it->first << std::endl;
		of << it->second->get_name() << std::endl;
		of << _DATA.getTypeString(it->second->get_type_id()) << std::endl;
		of << _DATA.getWeaponString(it->second->get_weapon_id()) << std::endl;
	}
	of.close();
}