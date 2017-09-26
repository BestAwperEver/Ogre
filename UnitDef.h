#pragma once
#ifndef _UNITDEF_H_
#define _UNITDEF_H_

typedef struct UnitDef {
	char type_id;
	char Class;
	char Weapon;
	char Armor;
	char AP;
	short HP;
	int PlayerID;
	char ViewRange;
} UnitDef;

#endif