#pragma once
#ifndef _ABSTRACT_UNIT_H_
#define _ABSTRACT_UNIT_H_

#include "MapObject.h"
#include "..\..\DataBase0\DataBase0\UnitDef.h"
#include "shared_enums.h"

class AbstractUnit: public boost::enable_shared_from_this<AbstractUnit>
{
public:
	virtual void	recalculateArmorClass();

	virtual void	setPlayer(int player_id) {m_UnitDef.PlayerID = player_id;}
	virtual void	setCoordinates(CellCoordinates cc) {m_Coord = cc;}
	virtual void	setLook(const CellCoordinates cc) {m_LooksTo = cc;}
	virtual void	setActivePoints(unsigned char points) {m_ActivePoints = points;}
	virtual void	setHitPoints(short points) {m_HitPoints = points;}
	virtual void	setArmor(char Armor);
	virtual void	setClass(char Class);
	virtual void	setUnitDef(UnitDef ud) {m_UnitDef = ud;}

	virtual CellCoordinates		getCoordinates() const {return m_Coord;}
	virtual CellCoordinates		getLook() const {return m_LooksTo;}
	virtual char				getAP() const {return m_ActivePoints;}
	virtual short				getHP() const {return m_HitPoints;}
//	virtual short&				getHP_ref() {return m_HitPoints;}
	virtual short				getAC() const {return m_ArmorClass;}
	virtual char				getArmor() const {return m_UnitDef.Armor;}
	virtual char				getWeapon() const {return m_UnitDef.Weapon;}
	virtual int					getPlayerID() const {return m_UnitDef.PlayerID;}
	virtual char				getType() const {return m_UnitDef.type_id;}
	virtual char				getClass() const {return m_UnitDef.Class;}
	virtual UnitDef				getUnitDef() const {return m_UnitDef;}

	void setName(const std::string& Name) {m_Name = Name;}
	std::string getName() const {return m_Name;}

protected:

	//char m_Class;
	char m_ActivePoints;
	//char m_Type;
	short m_HitPoints;
	short m_ArmorClass;
	//char m_Armor;
	//char m_Weapon;
	//char m_Team;
	UnitDef m_UnitDef;
	CellCoordinates m_Coord;
	CellCoordinates	m_LooksTo;
	std::string m_Name;

	AbstractUnit(UnitDef ud, CellCoordinates looks_to, std::string name);
public:

	typedef boost::shared_ptr<AbstractUnit> ptr;

	//static std::list<ptr> m_Units;
	static ptr create(UnitDef ud, CellCoordinates looks_to, std::string name = std::string());
	//static void remove(AbstractUnit*);

	virtual ~AbstractUnit(void);
};
#endif
