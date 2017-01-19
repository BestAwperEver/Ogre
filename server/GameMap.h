#pragma once
#ifndef _Game_Map_h
#define _Game_Map_h

#include <list>

#include "MapContainer.h"
#include "AbstractUnit.h"
#include "tcp_connection.h"
#include "Player.h"
#include "..\..\DataBase0\DataBase0\DataBase.h"

class tcp_server;

class GameMap: public boost::enable_shared_from_this<GameMap>, public boost::noncopyable
{
	typedef boost::shared_ptr<talk_to_client> client_ptr;

	boost::asio::io_service* m_pService;

	MapContainer	m_Array;	// используется для расчета перемещения
	MapContainer	m_Map;
	array2<AbstractUnit*> m_UnitMap;
	//std::map<int, Player::ptr>::iterator m_ActivePlayer;
	std::list<Player::ptr>::iterator m_ActivePlayerIt;
	bool m_bGameStarted;
	int m_ID;
	char m_MapID;
	//bool m_bInitialised;
	
	boost::asio::deadline_timer* m_pTurnTimer;

	std::map<int, Player::ptr> m_Players;
	std::list<Player::ptr> m_PlayersTurn; // в том порядке, в каком ходят
	//std::list<std::list<int>> m_Unions;
	std::list<AbstractUnit::ptr> m_Units;

private:

	bool getPath(CellCoordinates from, CellCoordinates to, std::vector<CellCoordinates>& Path, 
		int maxPathLength = 30);
	bool addUnit(CellCoordinates where, AbstractUnit::ptr unit, bool add_to_g = true);
	//bool addUnion(std::list<int> Union);
	bool removeUnit(CellCoordinates where);
	// возвращает индекс следующего ходящего
	// -1 в случае ошибки
	int nextTurn();

	inline bool do_add_hero(std::string Name, UnitDef ud, CellCoordinates where, CellCoordinates looks_to);
	inline bool do_add_unit(UnitDef ud, CellCoordinates where, CellCoordinates looks_to);
	inline bool do_move_unit(std::vector<CellCoordinates>& Path, bool& unblock_after);
	//inline bool do_add_union(std::list<int> new_union);

	inline short calculate_damage(CellCoordinates who_is_shooting, CellCoordinates to);
	std::vector<char> create_hero_msg(std::string name, UnitDef h,
		CellCoordinates where, CellCoordinates looks_to);
	void create_start_unit(Player::ptr);
	//void remove_unit(AbstractUnit*);
	//void player_is_dead(Player::ptr);

	void restore_active_points(int player_id, bool post_message = true);

	GameMap(int map_id, int game_id, boost::asio::io_service*, const std::map<int, Player::ptr>& Players);

public:

	typedef boost::shared_ptr<GameMap> ptr;

	//GameMap(int map_id, boost::asio::io_service* service, int id);
	//
	////GameMap(const std::string& map_file, boost::asio::io_service* service, int id);
	//GameMap();
	//GameMap(GameMap&&);

	static GameMap::ptr create(int map_id, int game_id, 
		boost::asio::io_service*, const std::map<int, Player::ptr>& Players);

	void set_id(int game_id) { m_ID = game_id; }
	int get_id() const { return m_ID; }

	void set_map_id(int map_id) { m_MapID = map_id; }
	int get_map_id() const { return m_MapID; }

	void operator= (GameMap&&);

	//GameMap(const GameMap&);
	//GameMap();
	//void initialise(const std::string& map_file, boost::asio::io_service& service);
	//GameMap(int Height, int Width);
	//GameMap(const MapContainer& _Map);
//	GameMap(int Height, int Width, std::string PathToFileMap);

	bool loadFromFile(std::string Path);
	bool loadMap(int map_id);

//	bool saveToFile(std::string Path);
	//bool getPath(CellCoordinates from, CellCoordinates to, std::deque<CellCoordinates>& Path, 
	//	int maxPathLength = 30);

	const MapContainer&	getMapContainerConstRef() const {return m_Map;}
	MapContainer		getMapContainerTempObj() const {return m_Map;}
	MapContainer&		getMapContainer() {return m_Map;}

	void	setMapContainer(const MapContainer& NewMap);

//	void copyMapToArray();
	bool passable(CellCoordinates);
	bool passable(const char& x, const char& y);
	bool moveUnit(CellCoordinates from, CellCoordinates to);
	char calculate_distance(CellCoordinates from, CellCoordinates to) const;

//	char findClientsTurn(int ClientID);
	inline bool hasUnits(int player_id);

	char operator() (int,int) const;
	char& operator() (int,int);
	char operator() (CellCoordinates c) const {return m_Map(c);}
	char& operator() (CellCoordinates c) {return m_Map(c);}
	AbstractUnit* getUnit(CellCoordinates c) const {return m_UnitMap(c);}
	//AbstractUnit& getUnit(CellCoordinates c) {return m_UnitMap(c);}
	int getHeight() const {return m_Map.getHeight();}
	int getWidth() const {return m_Map.getWidth();}

	~GameMap(void);

public:
	bool remove_player(int player_id);
	void remove_players();
	bool has_players(int except_id = -1) const;
	bool has_alive_players_except(int player_id) const;
	int team_count(bool dead_players_count = false) const;
	//bool game_started() const { return m_bGameStarted; }
	int getActivePlayerId();
	inline bool players_are_all_dead() const;
	bool there_are_units_with_not_null_ap(int player_id);
//	void restore_active_points(int player_id);
	//void setActivePlayer(int playerID) {m_ActivePlayer = playerID;}

	bool next_turn(bool post_message = true);

	//bool add_hero(std::string Name, CellCoordinates where, char type_id, char Class, char Weapon, char Armor, int player_id,
	//	char HP, char AP, CellCoordinates looks_to);
	//bool add_unit(CellCoordinates where, char type_id, char Class, char Weapon, char Armor, int player_id,
	//	char HP, char AP, CellCoordinates looks_to);
	bool add_hero(std::string Name, UnitDef ud, CellCoordinates where, CellCoordinates looks_to);
	bool add_unit(UnitDef ud, CellCoordinates where, CellCoordinates looks_to);
	bool move_unit(std::vector<CellCoordinates>& Path, bool& unblock_after);
	bool shoot(CellCoordinates from, CellCoordinates to, bool& unblock_after);
	//bool add_union(std::list<int> new_union);
	bool force_next_turn(unsigned int client_id, bool post_message = true);

	void start_game();
	void post_map_create_msg();
	std::vector<char> get_map_create_msg();
	std::vector<char> get_move_string(std::vector<CellCoordinates>& Path);
	//char get_team(const std::string& username);
	char get_team(int player_id);

	void handle_turn_timer(const boost::system::error_code&);

	void post(std::vector<char>&& msg, int except_player = 0);
	void post(const std::vector<char>& msg, int except_player = 0);

};

std::ostream& operator << (std::ostream&, const GameMap&);
//template <class T> OgreConsole<T>& operator << (OgreConsole<T>& o, const MapContainer& gm) {
//	for (int i = 0; i < gm.getHeight() - 1; i++) {
//		for (int j = 0; j < gm.getWidth() - 1; j++) {
//			std::string sym = std::stringConverter::toString((int)gm(i,j));
//			while (sym.length() < 4) {
//				sym += ' ';
//			}
//			o << sym;
//		}
//		o << (int)gm(i, gm.getWidth() - 1) << '\n';
//	}
//	for (int j = 0; j < gm.getHeight() - 1; j++) {
//		std::string sym = std::stringConverter::toString((int)gm(gm.getHeight() - 1, j));
//		while (sym.length() < 4) {
//			sym += ' ';
//		}
//		o << sym;
//	}
//	o << (int)gm(gm.getHeight() - 1,gm.getWidth() - 1);
//
//	return o;
//}
//template <class T> OgreConsole<T>& operator << (OgreConsole<T>& o, const GameMap& gm) {
//	//for (int i = 0; i < gm.getDimension() - 1; i++) {
//	//	for (int j = 0; j < gm.getDimension() - 1; j++) {
//	//		std::string sym = std::stringConverter::toString((int)gm(i,j));
//	//		while (sym.length() < 4) {
//	//			sym += ' ';
//	//		}
//	//		o << sym;
//	//	}
//	//	o << (int)gm(i, gm.getDimension() - 1) << '\n';
//	//}
//	//for (int j = 0; j < gm.getDimension() - 1; j++) {
//	//	std::string sym = std::stringConverter::toString((int)gm(gm.getDimension() - 1, j));
//	//	while (sym.length() < 4) {
//	//		sym += ' ';
//	//	}
//	//	o << sym;
//	//}
//	//o << (int)gm(gm.getDimension() - 1,gm.getDimension() - 1);
//
//	return o << gm.getMapContainer();
//}
//template <class T> OgreConsole<T>& operator << (OgreConsole<T>& o, const GameMap* _gm) {
//
//	//for (int i = 0; i < gm.getDimension() - 1; i++) {
//	//	for (int j = 0; j < gm.getDimension() - 1; j++) 
//	//		o << (int)gm(i,j) << "  ";
//	//	o << (int)gm(i,gm.getDimension() - 1) << '\n';
//	//}
//	//for (int j = 0; j < gm.getDimension() - 1; j++) 
//	//	o << (int)gm(gm.getDimension() - 1,gm.getDimension() - 1) << "  ";
//	//o << (int)gm(gm.getDimension() - 1,gm.getDimension() - 1);
//
//	//return o;
//	return o << _gm->getMapContainerConstRef();
//}

#endif