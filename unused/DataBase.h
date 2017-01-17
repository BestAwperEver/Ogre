#pragma once
#ifndef _DATABASE_H_
#define _DATABASE_H_

#define _DATA DataBase::getInstance()

#include "shared_enums.h"

#ifndef _Ogre_H__
#include "Vector3.h"
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
#endif

	DataBase(void);
	~DataBase(void);

	static DataBase& getInstance();

public: //Get primary key
	char getWeaponID(const std::string& weapon_string) const;
	char getArmorID(const std::string& armor_string) const;
	char getUnitID(const std::string& type_string) const;
	char getClassID(const std::string& class_string) const;

public: //Unit
	char getDefaultWeapon(char unit_type) const;
	char getDefaultArmor(char unit_type) const;
	char getDefaultClass(char unit_type) const;
	char getDefaultAP(char unit_type) const;	// Active points - ���� ��������
	short getDefaultHP(char unit_type) const;	// Hit points
	short getDefaultAC(char unit_type) const;	// Armor class
	float getTypeSpeed(char unit_type) const;

/*
	��������� ����������� - �����������, ���� ���������� ���� ���������,
	���� ��� ������ ��������� �� ����. ��, ������ ������� ��� ��������,
	��������, ���� �����
	Scale - ������ �� ���������� ���������� �������� �������� � ������������
	������������ ����
*/
	Vector3	getInitialDirection(char unit_type) const;
	Vector3	getScale(char unit_type) const;
/*
	������ ����� ������� �� ���� ������: .mesh, .material � .skeleton
	.mesh - ����������, ������
	.material - ��������
	.skeleton - ����� �� ��������
	� ���� ����� ������� ������ ��� ��������, ������ ��������� ����
*/
	String getMesh(char unit_type) const;
	String getMaterial(char unit_type) const;
	String getDefaultName(char unit_type) const;
	String getTypeString(char unit_type) const {return getDefaultName(unit_type);}

public: // Class
	// Armor Class �� ������ ���������
	short getClassACBonus(char Class) const;
	// ����������� ��������, ���������� �� �������
	float getClassSpeedMod(char Class) const;

	String	getClassString(char Class) const;

public: //Armor
	// Armor Class �� �����
	short getArmorAC(char Armor) const;
	// Light, medium, heavy
	char getArmorType(char Armor) const;
	String getArmorString(char Armor) const;

public: //Weapon
	String getWeaponString(char Weapon) const;
	char getWeaponType(char Weapon) const;
/*
	��������: � ������� ������ ���� ������������ � ����������� ���������
	��������� �����. � ����������� �� ����������, �������� ���� ������ � ��������
	�������� (����������� ���������) �� ����. ������������ ��� � ��������� ���:
	������� � ���� �������� ����� � �������� �� ����������� � ������������ ���������,
	�������� ����� �� ����� "�������" ���������, �������� �������� �� ���������,
	��������, ������, ������� ���������. ����� ����� ��������� ����������� ��������
	�������� � ����� �������(?) ��������������� ���������� �++.
	�.�., ����� � ���� �������� 13 ��������:
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
	short getMinDamage(char Weapon, unsigned char Distance) const;
	short getMaxDamage(char Weapon, unsigned char Distance) const;
	char getAccuracy(char Weapon, unsigned char Distance) const;
	char getMinDistance(char Weapon) const;
	char getMaxDistance(char Weapon) const;
};

#endif