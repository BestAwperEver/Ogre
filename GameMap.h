#pragma once
#ifndef _Game_Map_h
#define _Game_Map_h

#include "MapContainer.h"
class LightMap;
class AbstractUnit;
#include "FieldOfView.h"
class Player;

struct MyUnit {
	MyUnit(AbstractUnit* unit = 0, bool is_active = false): ref(unit), isActive(is_active) {}
	AbstractUnit* ref;
	bool isActive;
};

class GameMap
{
	MapContainer	m_Array;		// используется для расчета перемещения
	MapContainer	m_OriginalMap;	// без юнитов, нужна для восстановления
	MapContainer	m_Map;			// как есть на самом деле
	MapContainer	m_UserMap;		// как видит игрок
//	MapContainer	m_ServerMap;	// карта, копирующая серверную
	FieldOfView		m_LightMap;		// общая зона видимости игрока

	LightMap*		m_pLightMap;	// зоны видимости всех клеток (теперь считается динамически)

	array2<AbstractUnit*> m_Units;
//	Сделать карту вида юнит - поле зрения
	int				m_PlayerID;
	char			m_Team;
	//int				m_teamForFinding; // костыльчик, вроде не мешает особо
	//std::map<int, Player_ptr> m_Players;
	std::vector<int>	m_UnitInTeamCount;
	bool m_bInitialized;
public:
	std::list<std::list<int>> m_Unions;
	// bool отвечает за то, активен ли этот юнит; сейчас за это отвечает второе поле
	std::list<MyUnit> m_MyUnits;
	bool has_units() const { return !m_MyUnits.empty(); }
	//std::map<AbstractUnit*, bool> m_MyUnits;
	int team_count() const;

	GameMap();
	GameMap(int Height, int Width);
	GameMap(const MapContainer& _Map);

	void recalculateLightMaps(char min_range, char max_range);

	array2<bool> getLightMap(CellCoordinates cc, int range); // считает поле видимости из клетки

//	LightMap* getLightMapPtr() { return m_pLightMap; } // возвращает массив с мапами (теперь считается динамически)

	FieldOfView* getCurFOV() {return &m_LightMap;} // возвращает текущую мапу

//	GameMap(int Height, int Width, Ogre::String PathToFileMap);

//	bool loadFromFile(Ogre::String Path);
//	bool saveToFile(Ogre::String Path);
	//bool getPath(CellCoordinates from, CellCoordinates to, std::deque<CellCoordinates>& Path, 
	//	int maxPathLength = 30);
	bool getPath(CellCoordinates from, CellCoordinates to, std::vector<CellCoordinates>& Path, 
		int maxPathLength = 30);
	void getRealPath(std::vector<CellCoordinates>& Path, int view_range);

	const MapContainer&	getMapContainerConstRef() const {return m_Map;}
	MapContainer		getMapContainerTempObj() const {return m_Map;}
	MapContainer&		getMapContainer() {return m_Map;}
	MapContainer*		getMapContainerPtr() {return &m_Map;}

	void	setMapContainer(const MapContainer& NewMap);
	void	setMapContainer(MapContainer&& NewMap);

//	void copyMapToArray();
	bool in_map(CellCoordinates) const;
	bool in_map(char x, char y) const;
	bool passable(CellCoordinates, bool as_user_sees = false) const;
	bool passable(char x, char y, bool as_user_sees = false) const;
	bool visible(CellCoordinates) const;
	bool visible(char x, char y) const;
	//bool visible(CellCoordinates);
private:
	bool direct_los_exists(CellCoordinates from, CellCoordinates to) const;
	
	bool new_enemy_or_closed_door(CellCoordinates where, FieldOfView& FoV, const array2<bool>& lightmap);
public:
	bool line_of_sight_exists(CellCoordinates from, CellCoordinates to) const;
	//bool visible_now(CellCoordinates);
	bool already_seen(CellCoordinates);
	void addUnit(CellCoordinates where, AbstractUnit* unit);
	void removeUnit(CellCoordinates where);
	void moveUnit(CellCoordinates from, CellCoordinates to);
	static char calculate_distance(CellCoordinates from, CellCoordinates to);
	void clear_units();
	void set_default_state();

	bool inUnion(int First_Team, int Second_Team);
	bool inUnion(AbstractUnit* unit1, AbstractUnit* unit2);

	bool inRange(char m, char n) const { return m_Map.getArrayConstRef().inRange(m, n); }
	bool inRange(CellCoordinates cc) const { return m_Map.getArrayConstRef().inRange(cc); }
	char operator() (int,int) const;
	char& operator() (int,int);
	char operator() (CellCoordinates cc) const {return m_Map(cc);}
	char& operator() (CellCoordinates cc) {return m_Map(cc);}
	AbstractUnit* getUnit(CellCoordinates cc) const {return m_Units(cc);}
	int getHeight() const {return m_Map.getHeight();}
	int getWidth() const {return m_Map.getWidth();}

	int getPlayerID() const { return m_PlayerID; }
	char getTeam() const { return m_Team; }
	void setTeam(char team) { m_Team = team; }
	void setPlayerID(int player_id) { m_PlayerID = player_id; }

	void setActiveUnit(AbstractUnit* unit);
	void deselectActiveUnit();
	AbstractUnit* getMyFirstUnit();

	~GameMap(void);
};

#endif