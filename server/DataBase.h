#pragma once
#ifndef _DATABASE_H_
#define _DATABASE_H_

#define _DATA DataBase::getInstance()

#include <string>

class DataBase
{
	typedef std::string String;

	DataBase(void);

public:

	~DataBase(void);

	static DataBase& getInstance();

public: //Get primary key
	char getWeaponID(const String& weapon_string) const;
	char getArmorID(const String& armor_string) const;
	char getUnitID(const String& type_string) const;
	char getClassID(const String& class_tring) const;

public: //Unit
	char getDefaultWeapon(char unit_type) const;
	char getDefaultArmor(char unit_type) const;
	char getDefaultClass(char unit_type) const;
	char getDefaultAP(char unit_type) const;	// Active points - очки действия
	short getDefaultHP(char unit_type) const;	// Hit points
	short getDefaultAC(char unit_type) const;	// Armor class
	float getTypeSpeed(char unit_type) const;

	String getDefaultName(char unit_type) const;
	String getTypeString(char unit_type) const {return getDefaultName(unit_type);}

public: // Class
	// Armor Class от класса персонажа
	short getClassACBonus(char Class) const;
	// Модефикатор скорости, умножается на базовую
	float getClassSpeedMod(char Class) const;

	String	getClassString(char Class) const;

public: //Armor
	// Armor Class от брони
	short getArmorAC(char Armor) const;
	// Light, medium, heavy
	char getArmorType(char Armor) const;
	String getArmorString(char Armor) const;

public: //Weapon
	String getWeaponString(char Weapon) const;
	char getWeaponType(char Weapon) const;
/*
	Механика: у каждого оружия есть максимальная и минимальная дистанция
	нанесения урона. В зависимости от расстояния, меняется урон оружия и точность
	стрельбы (вероятность попадания) из него. Рассчитывать это я предлагаю так:
	держать в базе значения урона и точности на минимальной и максимальной дистанции,
	значение урона на некой "средней" дистанции, значение точности на некоторой,
	возможно, другой, средней дистанции. Между этими заданными дистанциями значения
	точности и урона линейно(?) интерполируются средствами с++.
	Т.о., всего в базе хранится 13 значений:
		Минимальная дистанция
		Максимальная дистанция
		"Средняя" дистанция для урона
		"Средняя" дистанция для точности
		Минимальный урон на минимальной дистанции
		Максимальный урон на минимальной дистанции
		Минимальный урон на "средней" дистанции
		Максимальный урон на "средней" дистанции
		Минимальный урон на максимальной дистанции
		Максимальный урон на максимальной дистанции
		Точность на минимальной дистанции
		Точность на "средней" дистанции
		Точность на максимальной дистанции
	На эти параметры, в будущем, должны влиять так же класс
	и тип персонажа (если они оба останутся в будущих версиях игры).
*/
	short getMinDamage(char Weapon, unsigned char Distance) const;
	short getMaxDamage(char Weapon, unsigned char Distance) const;
	char getAccuracy(char Weapon, unsigned char Distance) const;
	char getMinDistance(char Weapon) const;
	char getMaxDistance(char Weapon) const;
};

#endif