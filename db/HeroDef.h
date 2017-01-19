#pragma once
#ifndef _HERODEF_H_
#define _HERODEF_H_

#include "CellCoordinates.h"

struct HeroDef {
	HeroDef(char _Type, char _Class, char _Weapon, char _Armor)
		//, CellCoordinates _Pos, CellCoordinates _Look = CellCoordinates())
		: type_id(_Type)
		, Class(_Class)
		, Weapon(_Weapon)
		, Armor(_Armor) 
		//, Pos(_Pos)
		//, Look(_Look) 
	{}
	HeroDef(char _Type, char _Class, char _Weapon, char _Armor, char X, char Y)
		//, CellCoordinates _Look = CellCoordinates())
		: type_id(_Type)
		, Class(_Class)
		, Weapon(_Weapon)
		, Armor(_Armor) 
		//, Pos(X,Y)
		//, Look(_Look) 
	{}
	char type_id;
	char Class;
	char Weapon;
	char Armor;
	//CellCoordinates Pos;
	//CellCoordinates Look;
};

#endif