#include "DataBase.h"

DataBase::DataBase(void)
{
}
DataBase::~DataBase(void)
{
}

DataBase& DataBase::getInstance() {
	static DataBase instance;
	return instance;
}

char DataBase::getWeaponID(const DataBase::String& type_string) const {
	if (type_string == "bow") {
		return static_cast<char>(WEAPON::BOW);
	} else if (type_string == "sword") {
		return static_cast<char>(WEAPON::SWORD);
	} else if (type_string == "none") {
		return static_cast<char>(WEAPON::WEAPON_NONE);
	} else if (type_string == "crossbow") {
		return static_cast<char>(WEAPON::CROSSBOW);
	}
	return static_cast<char>(WEAPON::UNDEFINED_WEAPON);
}
char DataBase::getArmorID(const DataBase::String& type_string) const {
	if (type_string == "chain_mail") {
		return static_cast<char>(ARMOR::CHAIN_MAIL);
	} else if (type_string == "cloak") {
		return static_cast<char>(ARMOR::CLOAK);
	} else if (type_string == "none") {
		return static_cast<char>(ARMOR::ARMOR_NONE);
	}
	return static_cast<char>(ARMOR::UNDEFINED_ARMOR);
}
char DataBase::getTypeID(const DataBase::String& type_string) const {
	if (type_string == "robot")
		return static_cast<char>(UNIT_TYPE::ROBOT);
	else if (type_string == "dwarf")
		return static_cast<char>(UNIT_TYPE::DWARF);
	else if (type_string == "human")
		return static_cast<char>(UNIT_TYPE::HUMAN);
	else if (type_string == "halfling")
		return static_cast<char>(UNIT_TYPE::HALFLING);
	else if (type_string == "ogre")
		return static_cast<char>(UNIT_TYPE::OGRE);
	else if (type_string == "goblin")
		return static_cast<char>(UNIT_TYPE::GOBLIN);
	else if (type_string == "elf")
		return static_cast<char>(UNIT_TYPE::ELF);
	else if (type_string == "orc")
		return static_cast<char>(UNIT_TYPE::ORC);
	else if (type_string == "gnome")
		return static_cast<char>(UNIT_TYPE::GNOME);
	else
		return static_cast<char>(UNIT_TYPE::UNDEFINED_TYPE);
}
char DataBase::getClassID(const DataBase::String& Class) const {
	if (Class == "mage")
		return static_cast<char>(HERO_CLASS::MAGE);
	else if (Class == "warrior")
		return static_cast<char>(HERO_CLASS::WARRIOR);
	else if (Class == "monk")
		return static_cast<char>(HERO_CLASS::MONK);
	else if (Class == "rogue")
		return static_cast<char>(HERO_CLASS::ROGUE);
	else
		return static_cast<char>(HERO_CLASS::UNDEFINED_CLASS);
}
char DataBase::getMapID(const DataBase::String& map) const {
	if (map == "testmap") {
		return 1;
	} else if (map == "testmap2") {
		return 2;
	} else if (map == "testmap3") {
		return 3;
	} else if (map == "nomap") {
		return 0;
	} 
	return -1;
}

char DataBase::getDefaultWeapon(char type_id) const {
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::GOBLIN):
	case static_cast<char>(UNIT_TYPE::HALFLING):
		return static_cast<char>(WEAPON::BOW);
	case static_cast<char>(UNIT_TYPE::ORC):
		return static_cast<char>(WEAPON::CROSSBOW);
	default:
		return static_cast<char>(WEAPON::SWORD);
	}
}
char DataBase::getDefaultArmor(char type_id) const {
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::DWARF):
	case static_cast<char>(UNIT_TYPE::OGRE):
		return static_cast<char>(ARMOR_TYPE::HEAVY);
	default:
		return static_cast<char>(ARMOR_TYPE::LIGHT);
	}
}
char DataBase::getDefaultAP(char type_id) const {
	return DEFAULT_AP;
}
short DataBase::getDefaultHP(char type_id) const {
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::HALFLING):
		return 20;
	case static_cast<char>(UNIT_TYPE::DWARF):
		return 30;
	case static_cast<char>(UNIT_TYPE::OGRE):
		return 50;
	case static_cast<char>(UNIT_TYPE::GOBLIN):
		return 20;
	default:
		return DEFAULT_HP;
	}
}
short DataBase::getDefaultAC(char type_id) const { // это хуйня, на самом деле надо считать
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::OGRE):
		return 12;
	case static_cast<char>(UNIT_TYPE::HALFLING):
	case static_cast<char>(UNIT_TYPE::DWARF):
		return 11;
	default:
		return 10;
	}
}
char DataBase::getDefaultClass(char type_id) const {
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::GOBLIN):
	case static_cast<char>(UNIT_TYPE::HALFLING):
		return static_cast<char>(HERO_CLASS::ROGUE);
	default:
		return static_cast<char>(HERO_CLASS::WARRIOR);
	}
}
char DataBase::getDefaultViewRange(char type_id) const {
	return DEFAULT_VIEW_RANGE;
}
UnitDef DataBase::getDefaultUD(char type_id) const {
	UnitDef ud;
	ud.AP = getDefaultAP(type_id);
	ud.Armor = getDefaultArmor(type_id);
	ud.Class = getDefaultClass(type_id);
	ud.HP = getDefaultHP(type_id);
	ud.type_id = type_id;
	ud.ViewRange = getDefaultViewRange(type_id);
	ud.Weapon = getDefaultWeapon(type_id);
	return std::move( ud );
}

DataBase::Vector3	DataBase::getInitialDirection(char type_id) const {
	return DataBase::Vector3::UNIT_X;
}
DataBase::String	DataBase::getMesh(char type_id) const {
	return "robot.mesh";
}
DataBase::String	DataBase::getMaterial(char type_id) const {
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::ROBOT):
		return "Robot";
		break;
	case static_cast<char>(UNIT_TYPE::HUMAN):
		return "Orangebot";
		break;
	case static_cast<char>(UNIT_TYPE::DWARF):
		return "Brownbot";
		break;
	case static_cast<char>(UNIT_TYPE::GNOME):
		return "Bluebot";
		break;
	case static_cast<char>(UNIT_TYPE::HALFLING):
		return "Redbot";
		break;
	case static_cast<char>(UNIT_TYPE::OGRE):
		return "Yellowbot";
		break;
	case static_cast<char>(UNIT_TYPE::ORC):
		return "Greenbot";
		break;
	case static_cast<char>(UNIT_TYPE::GOBLIN):
		return "Greenbot";
		break;
	default:
		return "Robot";
	}
}
float				DataBase::getTypeSpeed(char type_id) const {
	float speed = HUMAN_DEFAULT_SPEED;
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::DWARF):
		speed *= 0.9f;
		break;
	case static_cast<char>(UNIT_TYPE::GNOME):
		speed *= 0.9f;
		break;
	case static_cast<char>(UNIT_TYPE::OGRE):
		speed *= 0.9f;
		break;
	}
	return speed;
}
DataBase::Vector3	DataBase::getScale(char type_id) const {
	const float DEF = HUMAN_DEFAULT_SCALE;
	switch (type_id) {
	case static_cast<char>(UNIT_TYPE::DWARF):
		return DEF*DataBase::Vector3(1.05f,0.7f,1.1f);
		break;
	case static_cast<char>(UNIT_TYPE::GNOME):
		return DEF*DataBase::Vector3(1.0f,0.7f,1.0f);
		break;
	case static_cast<char>(UNIT_TYPE::HALFLING):
		return DEF*DataBase::Vector3(0.7f,0.7f,0.7f);
		break;
	case static_cast<char>(UNIT_TYPE::OGRE):
		return DEF*DataBase::Vector3(1.1f,1.1f,1.15f);
		break;
	case static_cast<char>(UNIT_TYPE::ORC):
		return DEF*DataBase::Vector3(1.0f,0.9f,1.0f);
		break;
	case static_cast<char>(UNIT_TYPE::GOBLIN):
		return DEF*DataBase::Vector3(1.0f,0.9f,1.0f)*0.9f;
		break;
	default:
		return DataBase::Vector3(DEF);
	}
}
DataBase::String	DataBase::getDefaultName(char type_id) const {
	switch (type_id) {
	case	static_cast<char>(UNIT_TYPE::ROBOT):
		return "robot";
		break;
	//case	HERO:
	//	return "hero";
	//	break;
	case	static_cast<char>(UNIT_TYPE::HUMAN):
		return "human";
		break;
	case	static_cast<char>(UNIT_TYPE::DWARF):
		return "dwarf";
		break;
	case	static_cast<char>(UNIT_TYPE::GNOME):
		return "gnome";
		break;
	case	static_cast<char>(UNIT_TYPE::HALFLING):
		return "halfling";
		break;
	case	static_cast<char>(UNIT_TYPE::ELF):
		return "elf";
		break;
	case	static_cast<char>(UNIT_TYPE::OGRE):
		return "ogre";
		break;
	case	static_cast<char>(UNIT_TYPE::ORC):
		return "orc";
		break;
	case	static_cast<char>(UNIT_TYPE::GOBLIN):
		return "goblin";
		break;
	default:
		return "undefined";
	}
}

short DataBase::getClassACBonus(char Class) const {
	switch (Class) {
	case static_cast<char>(HERO_CLASS::WARRIOR):
		return 2;
	case static_cast<char>(HERO_CLASS::ROGUE):
		return 1;
	case static_cast<char>(HERO_CLASS::MAGE):
		return -1;
	default:
		return 0;
	}
}
float DataBase::getClassSpeedMod(char Class) const {
	float speed;
	switch (Class) {
	case static_cast<char>(HERO_CLASS::WARRIOR):
		speed = 0.95f;
		break;
	case static_cast<char>(HERO_CLASS::ROGUE):
		speed = 1.1f;
		break;
	default:
		speed = 1.0f;
	}
	return speed;
}
DataBase::String DataBase::getClassString(char Class) const {
	switch (Class) {
	case static_cast<char>(HERO_CLASS::MAGE):
		return "mage";
		break;
	case static_cast<char>(HERO_CLASS::WARRIOR):
		return "warrior";
		break;
	case static_cast<char>(HERO_CLASS::MONK):
		return "monk";
		break;
	case static_cast<char>(HERO_CLASS::ROGUE):
		return "rogue";
		break;
	default:
		return "undefined";
	}
}

short DataBase::getArmorAC(char Armor) const {
	switch (Armor) {
	case static_cast<char>(ARMOR::ARMOR_NONE):
		return 0;
	case static_cast<char>(ARMOR::CHAIN_MAIL):
		return 6;
	case static_cast<char>(ARMOR::CLOAK):
		return 4;
	default:
		return -1;
	}
}
short DataBase::calculateArmorClass(UnitDef ud) const {
	return getDefaultAC(ud.type_id) + getClassACBonus(ud.Class) + getArmorAC(ud.Armor);
}
char DataBase::getArmorType(char Armor) const {
	switch (Armor) {
	case static_cast<char>(ARMOR::ARMOR_NONE):
		return static_cast<char>(ARMOR_TYPE::SKIN);
	case static_cast<char>(ARMOR::CHAIN_MAIL):
		return static_cast<char>(ARMOR_TYPE::HEAVY);
	case static_cast<char>(ARMOR::CLOAK):
		return static_cast<char>(ARMOR_TYPE::LIGHT);
	default:
		return static_cast<char>(ARMOR_TYPE::UNDEFINED_ARMOR_TYPE);
	}
}
DataBase::String DataBase::getArmorString(char Armor) const {
	switch (Armor) {
	case static_cast<char>(ARMOR::CHAIN_MAIL):
		return "chain_mail";
	case static_cast<char>(ARMOR::CLOAK):
		return "cloak";
	case static_cast<char>(ARMOR::ARMOR_NONE):
		return "none";
	default:
		return "undefined";
	}
}

short DataBase::getMinDamage(char Weapon, unsigned char Distance) const {
	if (Distance > getMaxDistance(Weapon)
		|| Distance < getMinDistance(Weapon)) 
		return 0;

	switch (Weapon) {
	case static_cast<char>(WEAPON::BOW):
		return 1;
	case static_cast<char>(WEAPON::CROSSBOW):
		if (Distance <= 3)
			return 4;
		if (Distance <= 5)
			return 2;
		if (Distance <= 7)
			return 1;
		return 1;
	case static_cast<char>(WEAPON::SWORD):
		return 2;
	case static_cast<char>(WEAPON::WEAPON_NONE):
		return 0;
	default:
		return -1;
	}
}
short DataBase::getMaxDamage(char Weapon, unsigned char Distance) const {
	if (Distance > getMaxDistance(Weapon)
		|| Distance < getMinDistance(Weapon)) 
		return 0;

	switch (Weapon) {
	case static_cast<char>(WEAPON::BOW):
		return 6;
	case static_cast<char>(WEAPON::CROSSBOW):
		if (Distance <= 3)
			return 9;
		if (Distance <= 5)
			return 8;
		if (Distance <= 7)
			return 6;
		return 5;
	case static_cast<char>(WEAPON::SWORD):
		return 10;
	case static_cast<char>(WEAPON::WEAPON_NONE):
		return 3;
	default:
		return -1;
	}
}	
char DataBase::getAccuracy(char Weapon, unsigned char Distance) const {
	if (Distance > getMaxDistance(Weapon)
		|| Distance < getMinDistance(Weapon)) 
		return 0;

	switch (Weapon) {
	case static_cast<char>(WEAPON::CROSSBOW):
		return 85;
	case static_cast<char>(WEAPON::BOW):
		return 83;
	case static_cast<char>(WEAPON::SWORD):
		return 95;
	case static_cast<char>(WEAPON::WEAPON_NONE):
		return 50;
	default:
		return -1;
	}
}	
char DataBase::getMinDistance(char Weapon) const {
	switch (Weapon) {
	case static_cast<char>(WEAPON::BOW):
		return 2;
	case static_cast<char>(WEAPON::CROSSBOW):
		return 1;
	case static_cast<char>(WEAPON::SWORD):
		return 1;
	case static_cast<char>(WEAPON::WEAPON_NONE):
		return 1;
	default:
		return -1;
	}
}	
char DataBase::getMaxDistance(char Weapon) const {
	switch (Weapon) {
	case static_cast<char>(WEAPON::BOW):
		return 10;
	case static_cast<char>(WEAPON::CROSSBOW):
		return 7;
	case static_cast<char>(WEAPON::SWORD):
		return 1;
	case static_cast<char>(WEAPON::WEAPON_NONE):
		return 1;
	default:
		return -1;
	}
}
char DataBase::getWeaponType(char Weapon) const {
	switch (Weapon) {
	case static_cast<char>(WEAPON::BOW):
		return static_cast<char>(WEAPON_TYPE::RIFFLE);
	case static_cast<char>(WEAPON::CROSSBOW):
		return static_cast<char>(WEAPON_TYPE::RIFFLE);
	case static_cast<char>(WEAPON::SWORD):
		return static_cast<char>(WEAPON_TYPE::MELEE);
	case static_cast<char>(WEAPON::WEAPON_NONE):
		return static_cast<char>(WEAPON_TYPE::MELEE);
	default:
		return static_cast<char>(WEAPON_TYPE::UNDEFINED_WEAPON_TYPE);
	}
}
char DataBase::getShootCost(char Weapon) const {
	switch (Weapon) {
	case static_cast<char>(WEAPON::BOW):
		return 4;
	case static_cast<char>(WEAPON::CROSSBOW):
		return 5;
	case static_cast<char>(WEAPON::SWORD):
		return 4;
	default:
		return 4;
	}
}

DataBase::String DataBase::getWeaponString(char Weapon) const {
	switch (Weapon) {
	case static_cast<char>(WEAPON::BOW):
		return "bow";
	case static_cast<char>(WEAPON::SWORD):
		return "sword";
	case static_cast<char>(WEAPON::CROSSBOW):
		return "crossbow";
	case static_cast<char>(WEAPON::WEAPON_NONE):
		return "none";
	default:
		return "undefined";
	}
}
DataBase::String DataBase::getPropertyString(char Property) const {
	switch (Property) {
	case static_cast<char>(PROPERTY::AP): 
		return "ap";
	case static_cast<char>(PROPERTY::ARMOR):
		return "armor";
	case static_cast<char>(PROPERTY::HP):
		return "hp";
	case static_cast<char>(PROPERTY::WEAPON):
		return "weapon";
	default:
		return "undefined";
	}
}
DataBase::String DataBase::getMapName(char map_id) const {
	switch (map_id) {
	case 1:
		return "testmap";
	case 2:
		return "testmap2";
	case 3:
		return "testmap3";
	case 0:
		return "nomap";
	default:
		return "undefined";
	}
}