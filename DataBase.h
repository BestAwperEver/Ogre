#pragma once
#ifndef _DATABASE_H_
#define _DATABASE_H_

#define _DATA DataBase::getInstance()

#include "shared_enums.h"
#include "UnitDef.h"

#ifndef _Ogre_H__
#include "../../Database0/Database0/1/Vector3.h"
#include <string>
#endif

class DataBase
{
public:

#ifdef _Ogre_H__
	typedef Ogre::String String;
	typedef Ogre::Vector3 Vector3;
#else
	typedef std::string String;
	typedef Vector3 Vector3;
#endif

	DataBase(void);
	~DataBase(void);

	static DataBase& getInstance();

public: //Get primary key
	char getWeaponID(const String& weapon_string) const;
	char getArmorID(const String& armor_string) const;
	char getTypeID(const String& type_string) const;
	char getClassID(const String& class_string) const;
	char getMapID(const String& map_string) const;

public: //Maps

	String getMapName(char map_id) const;

public: //Unit
	char getDefaultWeapon(char type_id) const;
	char getDefaultArmor(char type_id) const;
	char getDefaultClass(char type_id) const;
	char getDefaultAP(char type_id) const;	// Active points - очки действия
	char getDefaultViewRange(char type_id) const;
	short getDefaultHP(char type_id) const;	// Hit points
	short getDefaultAC(char type_id) const;	// Armor class
	float getTypeSpeed(char type_id) const;
	UnitDef getDefaultUD(char type_id) const;

/*
	Начальное направление - направление, куда направлено "лицо" модели,
	если его просто поставить на поле. Хз, откуда берется это значение,
	возможно, Леха знает, как это узнать
	Scale - вектор со значениями растяжений исходной модельки в направлениях
	координатных осей
*/
	Vector3	getInitialDirection(char type_id) const;
	Vector3	getScale(char type_id) const;
/*
	Модель юнита состоит из трех частей: .mesh, .material и .skeleton
	.mesh - собственно, модель
	.material - текстура
	.skeleton - вроде бы анимации
	В базе нужно хранить первые два значения, третье цепляется само
*/
	String getMesh(char type_id) const;
	String getMaterial(char type_id) const;
	String getDefaultName(char type_id) const;
	String getTypeString(char type_id) const {return getDefaultName(type_id);}

public: // Class
	// Armor Class от класса персонажа
	short getClassACBonus(char Class) const;
	// Модефикатор скорости, умножается на базовую
	float getClassSpeedMod(char Class) const;

	String	getClassString(char Class) const;

public: //Armor
	// Armor Class от брони
	short calculateArmorClass(UnitDef) const;
	short getArmorAC(char Armor) const;
	// Light, medium, heavy
	char getArmorType(char Armor) const;
	String getArmorString(char Armor) const;

public: //Weapon
	String getWeaponString(char Weapon) const;
	char getWeaponType(char Weapon) const;
/*
	Механика: 
	Использование оружия требует затрат очков действия. 
	У каждого оружия есть максимальная и минимальная дистанция
	нанесения урона. В зависимости от расстояния, меняется урон оружия и точность
	стрельбы (вероятность попадания) из него. Рассчитывать это я предлагаю так:
	держать в базе значения урона и точности на минимальной и максимальной дистанции,
	значение урона на некой "средней" дистанции, значение точности на некоторой,
	возможно, другой, средней дистанции. Между этими заданными дистанциями значения
	точности и урона линейно(?) интерполируются средствами с++.
	Т.о., всего в базе хранится 14 значений:
		Расход очков действия на выстрел
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
	char getShootCost(char Weapon) const;
	short getMinDamage(char Weapon, unsigned char Distance) const;
	short getMaxDamage(char Weapon, unsigned char Distance) const;
	char getAccuracy(char Weapon, unsigned char Distance) const;
	char getMinDistance(char Weapon) const;
	char getMaxDistance(char Weapon) const;

	String getPropertyString(char Property) const;
};

#endif