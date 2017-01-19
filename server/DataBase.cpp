#include "DataBase.h"

DataBase::DataBase(void){}
DataBase::~DataBase(void){}

DataBase& DataBase::getInstance() {
	static DataBase instance;
	return instance;
}

char DataBase::getWeaponID(const DataBase::String& type_string) const {
	if (type_string == "bow") {
		return WEAPON::BOW;
	} else if (type_string == "sword") {
		return WEAPON::SWORD;
	} else if (type_string == "none") {
		return WEAPON::WEAPON_NONE;
	}
	return WEAPON::UNDEFINED_WEAPON;
}
char DataBase::getArmorID(const DataBase::String& type_string) const {
	if (type_string == "chain_mail") {
		return ARMOR::CHAIN_MAIL;
	} else if (type_string == "cloak") {
		return ARMOR::CLOAK;
	} else if (type_string == "none") {
		return ARMOR::ARMOR_NONE;
	}
	return ARMOR::UNDEFINED_ARMOR;
}
char DataBase::getUnitID(const DataBase::String& Type) const {
	if (Type == "robot")
		return UNIT_TYPE::ROBOT;
	else if (Type == "dwarf")
		return UNIT_TYPE::DWARF;
	else if (Type == "human")
		return UNIT_TYPE::HUMAN;
	else if (Type == "halfring")
		return UNIT_TYPE::HALFRING;
	else if (Type == "ogre")
		return UNIT_TYPE::OGRE;
	else if (Type == "goblin")
		return UNIT_TYPE::GOBLIN;
	else if (Type == "elf")
		return UNIT_TYPE::ELF;
	else if (Type == "orc")
		return UNIT_TYPE::ORC;
	else if (Type == "gnome")
		return UNIT_TYPE::GNOME;
	else
		return UNIT_TYPE::UNDEFINED_TYPE;
}
char DataBase::getClassID(const DataBase::String& Class) const {
	if (Class == "mage")
		return HERO_CLASS::MAGE;
	else if (Class == "warrior")
		return HERO_CLASS::WARRIOR;
	else if (Class == "monk")
		return HERO_CLASS::MONK;
	else if (Class == "rogue")
		return HERO_CLASS::ROGUE;
	else
		return HERO_CLASS::UNDEFINED_CLASS;
}

char DataBase::getDefaultWeapon(char Type) const {
	switch (Type) {
	case GOBLIN:
	case HALFRING:
		return WEAPON::BOW;
	default:
		return WEAPON::SWORD;
	}
}
char DataBase::getDefaultArmor(char Type) const {
	switch (Type) {
	case DWARF:
	case OGRE:
		return ARMOR_TYPE::HEAVY;
	default:
		return ARMOR_TYPE::LIGHT;
	}
}
char DataBase::getDefaultAP(char Type) const {
	return DEFAULT_AP;
}
short DataBase::getDefaultHP(char Type) const {
	switch (Type) {
	case HALFRING:
		return 20;
	case DWARF:
		return 30;
	case OGRE:
		return 50;
	case GOBLIN:
		return 20;
	default:
		return DEFAULT_HP;
	}
}
short DataBase::getDefaultAC(char Type) const {
	switch (Type) {
	case OGRE:
		return 12;
	case HALFRING:
	case DWARF:
		return 11;
	default:
		return 10;
	}
}
char DataBase::getDefaultClass(char Type) const {
	switch (Type) {
	case GOBLIN:
	case HALFRING:
		return HERO_CLASS::ROGUE;
	default:
		return HERO_CLASS::WARRIOR;
	}
}
float DataBase::getTypeSpeed(char Type) const {
	float speed = HUMAN_DEFAULT_SPEED;
	switch (Type) {
	case DWARF:
		speed *= 0.9;
		break;
	case GNOME:
		speed *= 0.9;
		break;
	case OGRE:
		speed *= 0.9;
		break;
	}
	return speed;
}

DataBase::String DataBase::getDefaultName(char Type) const {
	switch (Type) {
	case	ROBOT:
		return "robot";
		break;
	case	HERO:
		return "hero";
		break;
	case	HUMAN:
		return "human";
		break;
	case	DWARF:
		return "dwarf";
		break;
	case	GNOME:
		return "gnome";
		break;
	case	HALFRING:
		return "halfring";
		break;
	case	ELF:
		return "elf";
		break;
	case	OGRE:
		return "ogre";
		break;
	case	ORC:
		return "orc";
		break;
	case	GOBLIN:
		return "goblin";
		break;
	default:
		return "undefined";
	}
}

short DataBase::getClassACBonus(char Class) const {
	switch (Class) {
	case WARRIOR:
		return 2;
	case ROGUE:
		return 1;
	case MAGE:
		return -1;
	default:
		return 0;
	}
}
float DataBase::getClassSpeedMod(char Class) const {
	float speed;
	switch (Class) {
	case WARRIOR:
		speed = 0.95;
		break;
	case ROGUE:
		speed = 1.1;
		break;
	default:
		speed = 1;
	}
	return speed;
}
DataBase::String DataBase::getClassString(char Class) const {
	switch (Class) {
	case MAGE:
		return "mage";
		break;
	case WARRIOR:
		return "warrior";
		break;
	case MONK:
		return "monk";
		break;
	case ROGUE:
		return "rogue";
		break;
	default:
		return "undefined";
	}
}

short DataBase::getArmorAC(char Armor) const {
	switch (Armor) {
	case ARMOR::ARMOR_NONE:
		return 0;
	case ARMOR::CHAIN_MAIL:
		return 6;
	case ARMOR::CLOAK:
		return 4;
	default:
		return -1;
	}
}
char DataBase::getArmorType(char Armor) const {
	switch (Armor) {
	case ARMOR::ARMOR_NONE:
		return ARMOR_TYPE::SKIN;
	case ARMOR::CHAIN_MAIL:
		return ARMOR_TYPE::HEAVY;
	case ARMOR::CLOAK:
		return ARMOR_TYPE::LIGHT;
	default:
		return ARMOR_TYPE::UNDEFINED_ARMOR_TYPE;
	}
}
DataBase::String DataBase::getArmorString(char Armor) const {
	switch (Armor) {
	case ARMOR::CHAIN_MAIL:
		return "chain_mail";
	case ARMOR::CLOAK:
		return "cloak";
	case ARMOR::ARMOR_NONE:
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
	case WEAPON::BOW:
		return 1;
	case WEAPON::SWORD:
		return 2;
	case WEAPON::WEAPON_NONE:
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
	case WEAPON::BOW:
		return 6;
	case WEAPON::SWORD:
		return 12;
	case WEAPON::WEAPON_NONE:
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
	case WEAPON::BOW:
		return 85;
	case WEAPON::SWORD:
		return 95;
	case WEAPON::WEAPON_NONE:
		return 50;
	default:
		return -1;
	}
}	
char DataBase::getMinDistance(char Weapon) const {
	switch (Weapon) {
	case WEAPON::BOW:
		return 2;
	case WEAPON::SWORD:
		return 1;
	case WEAPON::WEAPON_NONE:
		return 1;
	default:
		return -1;
	}
}	
char DataBase::getMaxDistance(char Weapon) const {
	switch (Weapon) {
	case WEAPON::BOW:
		return 10;
	case WEAPON::SWORD:
		return 1;
	case WEAPON::WEAPON_NONE:
		return 1;
	default:
		return -1;
	}
}
char DataBase::getWeaponType(char Weapon) const {
	switch (Weapon) {
	case WEAPON::BOW:
		return WEAPON_TYPE::RIFFLE;
	case WEAPON::SWORD:
		return WEAPON_TYPE::MELEE;
	case WEAPON::WEAPON_NONE:
		return WEAPON_TYPE::MELEE;
	default:
		return WEAPON_TYPE::UNDEFINED_WEAPON_TYPE;
	}
}
DataBase::String DataBase::getWeaponString(char Weapon) const {
	switch (Weapon) {
	case WEAPON::BOW:
		return "bow";
	case WEAPON::SWORD:
		return "sword";
	case WEAPON::WEAPON_NONE:
		return "none";
	default:
		return "undefined";
	}
}