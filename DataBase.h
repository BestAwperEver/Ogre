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
	char getDefaultAP(char type_id) const;	// Active points - ���� ��������
	char getDefaultViewRange(char type_id) const;
	short getDefaultHP(char type_id) const;	// Hit points
	short getDefaultAC(char type_id) const;	// Armor class
	float getTypeSpeed(char type_id) const;
	UnitDef getDefaultUD(char type_id) const;

/*
	��������� ����������� - �����������, ���� ���������� "����" ������,
	���� ��� ������ ��������� �� ����. ��, ������ ������� ��� ��������,
	��������, ���� �����, ��� ��� ������
	Scale - ������ �� ���������� ���������� �������� �������� � ������������
	������������ ����
*/
	Vector3	getInitialDirection(char type_id) const;
	Vector3	getScale(char type_id) const;
/*
	������ ����� ������� �� ���� ������: .mesh, .material � .skeleton
	.mesh - ����������, ������
	.material - ��������
	.skeleton - ����� �� ��������
	� ���� ����� ������� ������ ��� ��������, ������ ��������� ����
*/
	String getMesh(char type_id) const;
	String getMaterial(char type_id) const;
	String getDefaultName(char type_id) const;
	String getTypeString(char type_id) const {return getDefaultName(type_id);}

public: // Class
	// Armor Class �� ������ ���������
	short getClassACBonus(char Class) const;
	// ����������� ��������, ���������� �� �������
	float getClassSpeedMod(char Class) const;

	String	getClassString(char Class) const;

public: //Armor
	// Armor Class �� �����
	short calculateArmorClass(UnitDef) const;
	short getArmorAC(char Armor) const;
	// Light, medium, heavy
	char getArmorType(char Armor) const;
	String getArmorString(char Armor) const;

public: //Weapon
	String getWeaponString(char Weapon) const;
	char getWeaponType(char Weapon) const;
/*
	��������: 
	������������� ������ ������� ������ ����� ��������. 
	� ������� ������ ���� ������������ � ����������� ���������
	��������� �����. � ����������� �� ����������, �������� ���� ������ � ��������
	�������� (����������� ���������) �� ����. ������������ ��� � ��������� ���:
	������� � ���� �������� ����� � �������� �� ����������� � ������������ ���������,
	�������� ����� �� ����� "�������" ���������, �������� �������� �� ���������,
	��������, ������, ������� ���������. ����� ����� ��������� ����������� ��������
	�������� � ����� �������(?) ��������������� ���������� �++.
	�.�., ����� � ���� �������� 14 ��������:
		������ ����� �������� �� �������
		����������� ���������
		������������ ���������
		"�������" ��������� ��� �����
		"�������" ��������� ��� ��������
		����������� ���� �� ����������� ���������
		������������ ���� �� ����������� ���������
		����������� ���� �� "�������" ���������
		������������ ���� �� "�������" ���������
		����������� ���� �� ������������ ���������
		������������ ���� �� ������������ ���������
		�������� �� ����������� ���������
		�������� �� "�������" ���������
		�������� �� ������������ ���������
	�� ��� ���������, � �������, ������ ������ ��� �� �����
	� ��� ��������� (���� ��� ��� ��������� � ������� ������� ����).
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