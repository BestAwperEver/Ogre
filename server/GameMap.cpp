#include "GameMap.h"
#include "tcp_server.h"
#include "shared_enums.h"

#define INT(x) *( (int*)(&x) )
#define SHORT(x) *( (short*)(&x) )
#define UD(x) *( (UnitDef*)(&x) )

#define TURN_TIME boost::posix_time::seconds(60)
#define TURN_TIME_FOR_OFFLINE boost::posix_time::seconds(20)

//GameMap::GameMap(int N, std::string Path):
//	m_Dimension(N)
//{
//#ifdef _USE_ASSERTS_
//	assert( N > 0 );
//#endif
//	m_Map = new char*[N];
//	m_Array = new char*[N];
//	for (int i = 0; i < N; i++) {
//		m_Map[i] = new char[N];
//		m_Array[i] = new char[N];
//		for (int j = 0; j < m_Dimension; j++) {
//			m_Map[i][j] = 0;
//			m_Array[i][j] = 0;
//		}
//	}
//	loadFromFile(Path);
//}

//void GameMap::copyMapToArray() {
//	for (int i = 0; i < m_Dimension; i++) {
//		for (int j = 0; j < m_Dimension; j++) {
//			m_Array[i][j] = m_Map[i][j];
//		}
//	}
//}
//int GameMap::getDimension() const {
//	return m_Dimension;
//}
//bool GameMap::saveToFile(std::string Path) {
//	std::ofstream f(Path, std::ios::trunc | std::ios::binary);
//
//	if (!f.is_open()) 
//		return false;
//
//	char n = m_Dimension;
//
//	f.write(&n, 1);
//
//	for (int i = 0; i < m_Dimension; i++) {
//		f.write(m_Map[i], m_Dimension);
//		if (f.bad()) return false;
//	}
//
//	f.close();
//
//	copyMapToArray();
//
//	return true;
//}
//bool GameMap::getPath(CellCoordinates from, CellCoordinates to, std::deque<CellCoordinates>& path,
//	int maxPathLength) {
//#ifdef _USE_ASSERTS_
//	assert( path.empty() );
//	assert( from.first < m_Dimension && from.second < m_Dimension );
//	assert( to.first < m_Dimension && to.second < m_Dimension );
//#endif
//
//	const int W      = m_Dimension;         // ширина рабочего поля
//	const int H      = m_Dimension;         // высота рабочего поля
//
//	const int ax	= from.first;
//	const int ay	= from.second;
//
//	const int bx	= to.first;
//	const int by	= to.second;
//
//	const int n = 8;
//
//	int len;                       // длина пути
//
//	int dx[n] = {1, 1, 0, -1, -1, -1, 0, 1};   // смещения, соответствующие соседям ячейки
//	int dy[n] = {0, 1, 1, 1, 0, -1, -1, -1};   // справа, снизу, слева и сверху
//	//CellCoordinates s[n] = {CellCoordinates(1,0),CellCoordinates(1,1),CellCoordinates(0,1),CellCoordinates(-1,1),
//	//	CellCoordinates(-1,0),CellCoordinates(-1,-1),CellCoordinates(0,-1),CellCoordinates(1,-1)};
//	int d, x, y, k;
//	bool stop;
//
//	// распространение волны
//	d = 0;
//	m_Array[ay][ax] = 0;            // стартовая ячейка помечена 0
//	do {
//		stop = true;               // предполагаем, что все свободные клетки уже помечены
//		for ( y = 0; y < H; ++y )
//			for ( x = 0; x < W; ++x )
//				if ( m_Array[y][x] == d )                         // ячейка (x, y) помечена числом d
//				{
//					for ( k = 0; k < n; ++k )                    // проходим по всем непомеченным соседям
//						if ( y + dy[k] >= 0 && y + dy[k] < H &&
//							x + dx[k] >= 0 && x + dx[k] < W &&
//							m_Array[y + dy[k]][x + dx[k]] == BLANK )
//						{
//							stop = false;							// найдены непомеченные клетки
//							m_Array[y + dy[k]][x + dx[k]] = d + 1;      // распространяем волну
//						}
//				}
//				d++;
//	} while ( !stop && m_Array[by][bx] == BLANK );
//
//	if (m_Array[by][bx] == BLANK) {
//		copyMapToArray();
//		return false;  // путь не найден
//	}
//
//	// восстановление пути
//	len = m_Array[by][bx];            // длина кратчайшего пути из (ax, ay) в (bx, by)
//	x = bx;
//	y = by;
//	d = len;
//	while ( d > 0 )
//	{
//		//px[d] = x;
//		//py[d] = y;                   // записываем ячейку (x, y) в путь
//		path.push_back(CellCoordinates(x,y));
//		d--;
//		for (k = 0; k < n; ++k)
//			if ( y + dy[k] >= 0 && y + dy[k] < H &&
//				x + dx[k] >= 0 && x + dx[k] < W &&
//				m_Array[y + dy[k]][x + dx[k]] == d)
//			{
//				x = x + dx[k];
//				y = y + dy[k];           // переходим в ячейку, которая на 1 ближе к старту
//				break;
//			}
//	}
//	//px[0] = ax;
//	//py[0] = ay;                    // теперь px[0..len] и py[0..len] - координаты ячеек пути
//
//	copyMapToArray();
//	return true;
//}

std::ostream& operator << (std::ostream& o, const GameMap& gm) {
	for (int i = 0; i < gm.getHeight() - 1; i++) {
		for (int j = 0; j < gm.getWidth() - 1; j++) {
			std::string sym = boost::to_string((int)gm(i,j));
			while (sym.length() < 4) {
				sym += ' ';
			}
			o << sym;
		}
		o << (int)gm(i, gm.getWidth() - 1) << std::endl;
	}
	for (int j = 0; j < gm.getHeight() - 1; j++) {
		std::string sym = boost::to_string((int)gm(gm.getHeight() - 1, j));
		while (sym.length() < 4) {
			sym += ' ';
		}
		o << sym;
	}
	o << (int)gm(gm.getHeight() - 1,gm.getWidth() - 1);

	return o;
}

//extern boost::asio::io_service g_Service;

//GameMap::GameMap(int M, int N):
//	m_Map(M,N), 
//	m_Array(M,N), 
//	m_UnitMap(M,N),
//	m_bGameStarted(false)
//{
//	MapObject::StandartObjects[OgreHead] = MapObject(MAP_OBJECT_INDEX::OgreHead, OgreHeadMask, "OgreHead");
//}
//boost::asio::io_service& GameMap::m_Service;

//GameMap::GameMap(const std::string& path, boost::asio::io_service* service, int id)
//	: m_pService(service)
//	, m_pTurnTimer(new boost::asio::deadline_timer(*service))
//	, m_bGameStarted(false)
//	, m_ID(id)
//	//, m_bInitialised(true)
//{
//	MapObject::StandartObjects[MAP_OBJECT_INDEX::OgreHead] = MapObject(MAP_OBJECT_INDEX::OgreHead,
//		OgreHeadMask, "OgreHead");
//	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_Z] = MapObject(MAP_OBJECT_INDEX::Wall_Z,
//		WallMask, "Wall_Z");
//	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_X] = MapObject(MAP_OBJECT_INDEX::Wall_X,
//		WallMask, "Wall_X");
//
//	if (loadFromFile(path)) {
//		std::cout << "Map loaded" << std::endl;
//		//std::cout << *this << std::endl;
//	} else {
//		std::cout << "file not found" << std::endl;
//	}
//}
//GameMap::GameMap(int map_id, boost::asio::io_service* service, int id)
//	: m_pService(service)
//	, m_pTurnTimer(new boost::asio::deadline_timer(*service))
//	, m_bGameStarted(false)
//	, m_ID(id)
//	, m_MapID(map_id)
//	//, m_bInitialised(true)
//{
//	MapObject::StandartObjects[MAP_OBJECT_INDEX::OgreHead] = MapObject(MAP_OBJECT_INDEX::OgreHead,
//		OgreHeadMask, "OgreHead");
//	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_Z] = MapObject(MAP_OBJECT_INDEX::Wall_Z,
//		WallMask, "Wall_Z");
//	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_X] = MapObject(MAP_OBJECT_INDEX::Wall_X,
//		WallMask, "Wall_X");
//
//	if (loadMap(map_id)) {
//		std::cout << "Map loaded" << std::endl;
//		//std::cout << *this << std::endl;
//	} else {
//		std::cout << "file not found" << std::endl;
//	}
//}

GameMap::GameMap(int map_id, int game_id,
	boost::asio::io_service* service, const std::map<int, Player::ptr>& Players)
	: m_pService(service)
	, m_pTurnTimer(new boost::asio::deadline_timer(*service))
	, m_bGameStarted(false)
	, m_ID(game_id)
	, m_Players(Players)
	, m_MapID(map_id)
	//, m_bInitialised(true)
{
	MapObject::StandartObjects[MAP_OBJECT_INDEX::OgreHead] = MapObject(MAP_OBJECT_INDEX::OgreHead,
		OgreHeadMask, "OgreHead");
	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_Z] = MapObject(MAP_OBJECT_INDEX::Wall_Z,
		WallMask, "Wall_Z");
	MapObject::StandartObjects[MAP_OBJECT_INDEX::Wall_X] = MapObject(MAP_OBJECT_INDEX::Wall_X,
		WallMask, "Wall_X");
}
//GameMap::GameMap(): m_pService(nullptr), m_pTurnTimer(nullptr) {}
//GameMap::GameMap(GameMap&& map)
//	: m_pService( map.m_pService )
//	, m_pTurnTimer( map.m_pTurnTimer )
//	, m_bGameStarted( map.m_bGameStarted )
//	, m_Array( std::move(map.m_Array) )
//	, m_Map( std::move(map.m_Map) )
//	, m_UnitMap( std::move(map.m_UnitMap) )
//	//, m_Unions( std::move(map.m_Unions) )
//	, m_ActivePlayer( std::move(map.m_ActivePlayer) )
//	, m_Players( std::move(map.m_Players) )
//	, m_Units( std::move(map.m_Units) )
//	, m_ID( map.m_ID )
//	, m_MapID( map.m_MapID )
//{
//	map.m_pService = nullptr;
//	map.m_pTurnTimer = nullptr;
//	map.m_ID = -1;
//}
//void GameMap::operator= (GameMap&& map) {
//
//	m_pService = map.m_pService;
//	map.m_pService = nullptr;
//
//	m_bGameStarted = map.m_bGameStarted;
//
//	m_pTurnTimer = map.m_pTurnTimer;
//	map.m_pTurnTimer = nullptr;
//
//	m_ID = map.m_ID;
//	map.m_ID = -1;
//
//	m_MapID = map.m_MapID;
//	map.m_MapID = -1;
//
//	m_Array = std::move(map.m_Array);
//	m_Map = std::move(map.m_Map);
//	m_UnitMap = std::move(map.m_UnitMap);
//	//m_Unions = std::move(map.m_Unions);
//	m_ActivePlayer = std::move(map.m_ActivePlayer);
//	m_Players = std::move(map.m_Players);
//	m_Units = std::move(map.m_Units);
//}
//void GameMap::initialise(const std::string& map_file, boost::asio::io_service& service) {
//
//}
//GameMap::GameMap(const MapContainer& _Map)
//	: m_Map(_Map)
//	, m_Array(_Map)
//	, m_UnitMap(_Map.getHeight(), _Map.getWidth())
//	, m_bGameStarted(false)
//{
//	MapObject::StandartObjects[OgreHead] = MapObject(MAP_OBJECT_INDEX::OgreHead, OgreHeadMask, "OgreHead");
//}
GameMap::~GameMap(void) {
	if (m_pTurnTimer) {
		delete m_pTurnTimer;
		m_pTurnTimer = nullptr;
	}
}

GameMap::ptr GameMap::create(int map_id, int game_id,
	boost::asio::io_service* service, const std::map<int, Player::ptr>& Players) 
{
	GameMap::ptr new_map(new GameMap(map_id, game_id, service, Players));

	if (new_map->loadMap(map_id)) {
		std::cout << "Map loaded" << std::endl;
		//std::cout << *this << std::endl;
	} else {
		std::cout << "file not found" << std::endl;
	}
	return new_map;
}

bool GameMap::loadMap(int map_id) {
	if (map_id < 1 || _DATA.getMapName(map_id) == "undefined") {
		// _SERVER.maps()[map_id].length() < 2) {
		return false;
	}
	//return loadFromFile(_SERVER.maps()[map_id] + ".mapdef");
	return loadFromFile(_DATA.getMapName(map_id) + ".mapdef");
}
bool GameMap::loadFromFile(std::string Path) {

	std::ifstream stream;
	stream.open(Path, std::ios_base::binary);
	if (!stream.is_open()) {
		return false;
	}

	char H,W; short OC; // height, width, object count

	stream.read(&H,1);
	stream.read(&W,1);
	stream.read((char*)(&OC),sizeof(short));

	std::vector<MapObjectDef> vec(OC);

	CellCoordinates Oc; // object coordinates
	char OI; // object index

	for (int i = 0; i < OC; ++i) {
		stream.read(&Oc.x,1);
		stream.read(&Oc.y,1);
		stream.read(&OI,1);
		vec[i] = MapObjectDef( Oc, MAP_OBJECT_INDEX( OI ));
	}

	MapContainer mc(H,W,vec);

	for (auto it = mc.getObjects().begin(); it !=  mc.getObjects().end(); ++it) {
		for (int i = 0; i < MapObject::StandartObjects[it->objectIndex].getMask().getM(); ++i) {
			for (int j = 0; j < MapObject::StandartObjects[it->objectIndex].getMask().getN(); ++j) {
				mc[it->objectCoordinates.x + i][it->objectCoordinates.y + j] 
					= MapObject::StandartObjects[it->objectIndex].getMask()[i][j];
			}
		}
	}

	setMapContainer( std::move(mc) );

	

	stream.close();

	return true;
}

char GameMap::operator() (int i, int j) const {
	return m_Map[i][j];
}
char& GameMap::operator() (int i, int j) {
	return m_Map[i][j];
}

bool GameMap::getPath(CellCoordinates from, CellCoordinates to, std::vector<CellCoordinates>& path,
	int maxPathLength) {
#ifdef _USE_ASSERTS_
	assert( path.empty() );
	assert( path.capacity() > maxPathLength );
	assert( from.x < getHeight() && from.y < getWidth() );
	assert( to.x < getHeight() && to.y < getWidth() );
#endif

	if (m_Map(to) == static_cast<char>(MAP_SYMBOL::WALL) || m_Map(to) == static_cast<char>(MAP_SYMBOL::UNIT)) {
		return false;
	}

	const int W      = getWidth();         // ширина рабочего поля
	const int H      = getHeight();         // высота рабочего поля

	//const int ax	= from.x;
	//const int ay	= from.y;

	//const int bx	= to.x;
	//const int by	= to.y;

	const int n = 8;

	int len;                       // длина пути

	int dx[n] = {1, 0, -1, 0, 1, 1, -1, -1};   // смещения, соответствующие соседям ячейки
	int dy[n] = {0, 1, 0, -1, -1, 1, 1, -1};   // справа, снизу, слева и сверху
	//CellCoordinates s[n] = {CellCoordinates(1,0),CellCoordinates(1,1),CellCoordinates(0,1),CellCoordinates(-1,1),
	//	CellCoordinates(-1,0),CellCoordinates(-1,-1),CellCoordinates(0,-1),CellCoordinates(1,-1)};
	int d, x, y, k;
	bool stop;

	// распространение волны
	d = 0;
	m_Array(from) = 0;            // стартовая ячейка помечена 0
	do {
		stop = true;               // предполагаем, что все свободные клетки уже помечены
		for ( x = 0; x < H; ++x )
			for ( y = 0; y < W; ++y )
				if ( m_Array[x][y] == d )                         // ячейка (x, y) помечена числом d
				{
					for ( k = 0; k < n; ++k )                    // проходим по всем непомеченным соседям
						if ( y + dy[k] >= 0 && y + dy[k] < W &&
							x + dx[k] >= 0 && x + dx[k] < H &&
							m_Array[x + dx[k]][y + dy[k]] == static_cast<char>(MAP_SYMBOL::BLANK) )
						{	
							if (k < 4) {
								stop = false;							// найдены непомеченные клетки
								m_Array[x + dx[k]][y + dy[k]] = d + 1;      // распространяем волну
							}
							else {
								// если пытаемся пойти по диагонали, то нужно убедиться, что клетки,
								// соседствующие с той, в которую хотим пойти, тоже свободны
								if (m_Map[x + dx[k]][y] == static_cast<char>(MAP_SYMBOL::BLANK)
								 && m_Map[x][y + dy[k]] == static_cast<char>(MAP_SYMBOL::BLANK)) {
									stop = false;							
									m_Array[x + dx[k]][y + dy[k]] = d + 1;     
								}
							}
						}
				}
				d++;
				if (d > maxPathLength && !stop) {
					return false;
				}
	} while ( !stop && m_Array(to) == static_cast<char>(MAP_SYMBOL::BLANK) );

	if (m_Array(to) == static_cast<char>(MAP_SYMBOL::BLANK)) {
		m_Array = m_Map;
		return false;  // путь не найден
	}

	// восстановление пути
	len = m_Array(to);            // длина кратчайшего пути из (ax, ay) в (bx, by)
	x = to.x;
	y = to.y;
	d = len;
	//path.push_back(CellCoordinates(-1,-1));
	while ( d > 0 )
	{
		//px[d] = x;
		//py[d] = y;                   // записываем ячейку (x, y) в путь
		path.push_back(CellCoordinates(x,y));
		--d;
		for (k = 0; k < n; ++k)
			if (y + dy[k] >= 0 && y + dy[k] < W &&
				x + dx[k] >= 0 && x + dx[k] < H &&
				m_Array[x + dx[k]][y + dy[k]] == d)
			{
				x = x + dx[k];
				y = y + dy[k];           // переходим в ячейку, которая на 1 ближе к старту
				break;
			}
	}
	//px[0] = ax;
	//py[0] = ay;                    // теперь px[0..len] и py[0..len] - координаты ячеек пути

	m_Array = m_Map;
	return true;
}

bool GameMap::passable(CellCoordinates cc) {
	if (cc.x < 0 || cc.x >= m_Map.getHeight() || cc.y < 0 || cc.y >= m_Map.getWidth()) return false;
	char obj = m_Map(cc);
	return obj == static_cast<char>(MAP_SYMBOL::BLANK) || obj == static_cast<char>(MAP_SYMBOL::OPENED_DOOR);
}
bool GameMap::passable(const char& x, const char& y) {
	if (x < 0 || x >= m_Map.getHeight() || y < 0 || y >= m_Map.getWidth()) return false;
	char obj = m_Map(x,y);
	return obj == static_cast<char>(MAP_SYMBOL::BLANK) || obj == static_cast<char>(MAP_SYMBOL::OPENED_DOOR);
}
bool GameMap::addUnit(CellCoordinates where, AbstractUnit::ptr unit, bool add_to_g) {
	if (!passable(where)) return false;

	m_Map(where) = static_cast<char>(MAP_SYMBOL::UNIT);
	m_Array(where) = static_cast<char>(MAP_SYMBOL::UNIT);
	m_UnitMap(where) = unit.get();

	//if (m_Players.empty()) {
	//	m_Players[0] = unit->getPlayerID();
	//} else {
		//bool there_are_no_others = true;
		//for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		//	if (it->second == unit->getPlayerID()) {
		//		there_are_no_others = false;
		//		break;
		//	}
		//}
	//if (!isSuchTeam(unit->getPlayerID())) {
	//m_Players.push_back(unit->getPlayerID());
	//}
	//}

	//if (m_bGameStarted) {
	//	char active_team = *m_ActivePlayer;
	//	m_Players[unit->getPlayerID()] = true;
	//	m_ActivePlayer = m_Players.find(active_team);
	//} else {
	//	m_Players[unit->getPlayerID()] = true;
	//}

	//_SERVER.players()[unit->getPlayerID()].set_has_units(true);
	//_SERVER.players()[unit->getPlayerID()].new_unit();
	m_Players[unit->getPlayerID()]->new_unit();

	if (add_to_g) {
		m_Units.push_back(unit);
	}

	unit->setCoordinates(where);

	return true;
}
bool GameMap::removeUnit(CellCoordinates where) {
	if (passable(where)) return false;

	m_Map(where) = static_cast<char>(MAP_SYMBOL::BLANK);
	m_Array(where) = static_cast<char>(MAP_SYMBOL::BLANK);

	auto unit = m_UnitMap(where);
	m_UnitMap(where) = nullptr;

	unit->setCoordinates(CellCoordinates());

	//auto it = m_Players.find(unit->getPlayerID());
	//if (it->second->get_game_id() == m_ID) {
	//	it->loose_unit();
	//}

	m_Players[unit->getPlayerID()]->loose_unit();

	m_Units.remove(unit->shared_from_this());
	//_SERVER.players()[unit->getPlayerID()].loose_unit();

	//bool there_are_others = false;

	//for (auto it = m_Units.begin(); it != m_Units.end(); ++it) {
	//	if ((*it)->getPlayer() == unit->getPlayerID()) 
	//		there_are_others = true;
	//}

	//if (players_are_all_dead()) {
	//	//m_Players[unit->getPlayerID()] = false;
	//	_SERVER.players()[unit->getPlayerID()].set_has_units(false);
	//	//m_bGameStarted = false;
	//} else if (!isSuchTeam(unit->getPlayerID())) {
	//	//for (auto iter = m_Players.begin(); iter != m_Players.end(); ++iter) {
	//	//	if (iter->first == unit->getPlayerID()) {
	//	//		//iter = m_Players.erase(iter);
	//	//		iter->second = false;
	//	//		break;
	//	//	} //else {
	//	//	//	++iter;
	//	//	//}
	//	_SERVER.players()[unit->getPlayerID()].set_has_units(false);
	////}
	//	//if (*m_ActivePlayer == unit->getPlayerID()) {
	//	//	assert( false ); // умер во время своего хода
	//	//	//m_ActiveUnit = m_Players.erase(m_ActiveUnit);
	//	//} else {
	//	//	m_Players.remove(unit->getPlayerID());
	//	//}
	//}
	
	return true;
}
bool GameMap::moveUnit(CellCoordinates from, CellCoordinates to) {
	//if (!addUnit(to, m_UnitMap(from), false)) return false;

	if (!passable(to)) return false;

	m_Map(to) = static_cast<char>(MAP_SYMBOL::UNIT);
	m_Array(to) = static_cast<char>(MAP_SYMBOL::UNIT);
	m_UnitMap(to) = m_UnitMap(from);

	m_Map(from) = static_cast<char>(MAP_SYMBOL::BLANK);
	m_Array(from) = static_cast<char>(MAP_SYMBOL::BLANK);
	m_UnitMap(from) = 0;

	return true;
}

//bool GameMap::addUnion(std::list<int> new_union) {
//for (auto it = m_Unions.begin(); it != m_Unions.end(); ++it) 
//		for (auto jt = it->begin(); jt != it->end(); ++jt) 
//			for (auto kt = new_union.begin(); kt != new_union.end(); ++kt) 
//				if (*jt == *kt) 
//					return false;
//	m_Unions.push_back( std::move(new_union) );
//
//	return true;
//}

void GameMap::setMapContainer(const MapContainer& NewMap) {
	m_Map=NewMap;
	m_Array=NewMap;
	m_UnitMap.clear();
	m_UnitMap.initialize(NewMap.getHeight(),NewMap.getWidth());
}

int GameMap::nextTurn() {
	if (m_PlayersTurn.empty() || players_are_all_dead()) {
		return -1;
	}
	if (!m_bGameStarted) {
		m_ActivePlayerIt = m_PlayersTurn.begin();
		m_bGameStarted = true;
	} else do {
		if (++m_ActivePlayerIt == m_PlayersTurn.end()) {
			m_ActivePlayerIt = m_PlayersTurn.begin();
		}
	} while (!(*m_ActivePlayerIt)->has_units());
	//if (m_ActivePlayer >= m_Players.size() - 1) {// || m_ActivePlayer == -1) {
	//	m_ActivePlayer = 0;
	//	return m_Players[0];
	//} else {
	//	++m_ActivePlayer;
	//}
	//auto it = std::find(m_Players.begin(), m_Players.end(), m_ActivePlayer);
	//return m_Players[m_ActivePlayer];
	return (*m_ActivePlayerIt)->get_id();
}
bool GameMap::hasUnits(int player_id) {
	for (auto it = m_Units.begin(); it != m_Units.end(); ++it) {
		if ((*it)->getPlayerID() == player_id) 
			return true;
	}
	return false;
}
bool GameMap::players_are_all_dead() const {
	//for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
	//	if (it->second) return false;
	//}
	//return true;
	return m_Units.empty();
}
bool GameMap::there_are_units_with_not_null_ap(int player_id) {
	for (auto it = m_Units.begin(); it != m_Units.end(); ++it) {
		if ((*it)->getPlayerID() == player_id && (*it)->getAP() > 0) 
			return true;
	}
	return false;
}
char GameMap::calculate_distance(CellCoordinates from, CellCoordinates to) const {
	return std::sqrtf( (to.x - from.x)*(to.x - from.x) + (to.y - from.y)*(to.y - from.y) );
}
int GameMap::getActivePlayerId() {
	if (!m_bGameStarted || m_Players.empty() || players_are_all_dead()) {
		return -1;
	}
	//return m_Players[m_ActivePlayer];
	return (*m_ActivePlayerIt)->get_id();
}
//char GameMap::findClientsTurn(int ClientID) {
//
//}

void GameMap::handle_turn_timer(const boost::system::error_code& err) {
	if (err) {
		return;
	}
	next_turn();
}

//bool GameMap::client_reconnected(client_ptr cl) {
//	if (this->isSuchTeam(_CLDATA.getClientID(cl->username()))) {
//		return true;
//	}
//	return false;
//}
std::vector<char> GameMap::create_hero_msg(std::string name, UnitDef ud,
	CellCoordinates where, CellCoordinates looks_to) 
{
	//std::string msg(HERO_SIZE + cl->username().size(), 0);
	//int i = 0;
	//msg[i++] = REQUEST_TYPE::CREATE;
	//msg[i++] = UNIT_TYPE::HERO;
	//msg[i++] = cl->username().size();
	//std::memcpy(&msg[i], cl->username().c_str(), cl->username().size());
	//i += cl->username().size();
	//msg[i++] = h.type_id;
	//msg[i++] = h.Class;
	//msg[i++] = _CLDATA.getClientID(cl->username());
	//*((short*)(&msg[i])) = _DATA.getDefaultHP(h.type_id);
	//i += sizeof(short);
	//msg[i++] = _DATA.getDefaultAP(h.type_id);
	//msg[i++] = h.Weapon;
	//msg[i++] = h.Armor;
	//msg[i++] = where.x;
	//msg[i++] = where.y;
	//msg[i++] = looks_to.x;
	//msg[i++] = looks_to.y;
	//return msg;
	int N = 3*sizeof(char) + name.size() + sizeof(UnitDef) + 4;
	std::vector<char> msg(N, 0);
	int i = 0;
	msg[i++] = static_cast<char>(REQUEST_TYPE::CREATE_UNIT);
	//msg[i++] = UNIT_TYPE::HERO;
	UD(msg[i]) = ud;
	i += sizeof(UnitDef);
	msg[i++] = name.size();
	memcpy_s(&msg[i], name.size(), name.c_str(), name.size());
	i += name.size();
	msg[i++] = where.x;
	msg[i++] = where.y;
	msg[i++] = looks_to.x;
	msg[i++] = looks_to.y;
	return msg;
}
void GameMap::create_start_unit(Player::ptr player) {
//	auto h = _DATA.getClientStartHero(_CLDATA.getClientID(cl->username()));
	int player_id = player->get_id();
	//UnitDef h = _CLDATA.getClientStartHero(id);
	UnitDef h = player->get_merk()->get_ud();

	CellCoordinates start_coord, look_coord;
	int width_div, width_add = 0, height_div, height_add = 0;

	const int N_width = getWidth() / 4, N_height = getHeight() / 4;
	const int n = 1;
	// если две команды, то каждой по половине поля, кроме края в n клеток
	// и центральной полосы в (2N-n) клеток
	// если больше, то по четверти
	height_div = getHeight() / 2 - N_height - n;
	if (team_count() < 3) {
		width_div = getWidth() - 2*n;
		width_add = n;
		height_add = (player->get_team() == 1) ? (getHeight() / 2 + N_height) : n;
	} else {
		width_div = getWidth() / 2 - N_width - n;
		switch (player->get_team()) {
			case 1: {
				width_add = n;
				height_add = n;
			} break;
			case 2: {
				width_add = n;
				height_add = (getHeight() / 2 + N_height);
			} break;
			case 3: {
				width_add = (getWidth() / 2 + N_width);
				height_add = n;
			} break;
			case 4: {
				height_add = (getHeight() / 2 + N_height);
				width_add = (getWidth() / 2 + N_width);
			} break;
			default: {
				width_div = getWidth() - 2*n;
				height_div = getHeight() - 2*n;
				width_add = n;
				height_add = n;
			}
		}
	}

	do {
		start_coord = CellCoordinates(rand() % width_div + width_add,
						rand() % height_div + height_add);
	} while (!this->passable(start_coord));

	do {
		look_coord = start_coord + CellCoordinates(rand()%3-1, rand()%3-1);
	} while (look_coord == start_coord);

	std::string name = player->get_merk()->get_name();

	add_hero(name, h, start_coord, /*h.type_id, h.Class, h.Weapon, h.Armor, id,
		_DATA.getDefaultHP(h.type_id), _DATA.getDefaultAP(h.type_id),*/ look_coord);

	if (m_bGameStarted) {
		post(create_hero_msg(name, h, start_coord, look_coord)); 
		// было всем, кроме клиента
		// не понял почему, удалил
		// вроде всё работает
	}
}
//void GameMap::player_is_dead(Player::ptr player, bool post_message) {
//#ifdef _USE_ASSERTS_
//	assert(!player->has_units() && "Try to set dead player that has alive units");
//#endif
//	bool finded = false;
//	if for (auto it = m_PlayersTurn.begin(), e = m_PlayersTurn.end(); it != e; ++it) {
//		if ((*it)->get_id() == player_id) {
//			m_PlayersTurn.erase(it);
//			finded = true;
//			break;
//		}
//	}
//	// TO DO: post message about dying
//}
// физическое удаление из списка всех юнитов
//void GameMap::remove_unit(AbstractUnit* unit) {
//#ifdef _USE_ASSERTS_
//	assert(false && "Trying to use old function remove_unit");
//#endif
//	//auto h = dynamic_cast<Hero*>(unit);
//	//if (h) {
//	//	//Hero::m_Units.remove(AbstractUnit::ptr(unit));
//	//	Hero::remove(h);
//	//} else {
//	//	//AbstractUnit::m_Units.remove(AbstractUnit::ptr(unit));
//	//	AbstractUnit::remove(unit);
//	//}
//}

char GameMap::get_team(int player_id) {
	//return _CLDATA.getClientID(user); // заглушко
	//return player_id; // херня полная, просто не помню, где это применяется
	return m_Players[player_id]->get_team();
}

bool GameMap::add_hero(std::string Name, UnitDef ud, CellCoordinates where, CellCoordinates looks_to) 
{
	if (!do_add_hero(Name, ud, where, looks_to))
		return false;
	//if (this->getActivePlayer() == -1)
	//	return next_turn();
	return true;
}
bool GameMap::add_unit(UnitDef ud, CellCoordinates where, CellCoordinates looks_to) 
{
	if (!do_add_unit(ud, where, looks_to))
		return false;
	//if (this->getActivePlayer() == -1)
	//	return next_turn();
	return true;
}
bool GameMap::move_unit(std::vector<CellCoordinates>& Path, bool& unblock_after)
{
	auto unit = getUnit(Path[0]);

	if (unit == 0 || (*m_ActivePlayerIt)->get_id() != unit->getPlayerID()) {
		return false;
	}

	if (getUnit(Path[0])->getAP() < Path.size()-1) {
		return false;
	}

	for (int i = 0; i < Path.size() - 1; ++i) {
		if (i > 0 && !this->passable(Path[i])) {
			return false;
		}
		if (Path[i].x != Path[i+1].x && Path[i].y != Path[i+1].y) {
			if (!this->passable(Path[i].x, Path[i+1].y)
				|| !this->passable(Path[i+1].x, Path[i].y)) {
					return false;
			}
		}
	}

	return do_move_unit(Path, unblock_after);
}
bool GameMap::shoot(CellCoordinates from, CellCoordinates to, bool& unblock_after)
{
	if (this->passable(to) || this->passable(from)) {
		return false;
	}

	if (this->getUnit(from)->getAP() < _DATA.getShootCost(this->getUnit(from)->getWeapon())) {
		return false;
	}

	short Damage = calculate_damage(from, to);
/*
	ANS_SHOOT
	Координаты стреляющего
	Координаты того, в кого стреляли
	Урон (0 - промах)
*/
	const int len = 7 + 6;
	char ans[len] = { static_cast<char>(REQUEST_TYPE::ANS_SHOOT), from.x, from.y, to.x, to.y, 0, 0,
		static_cast<char>(REQUEST_TYPE::SET), from.x, from.y, static_cast<char>(PROPERTY::AP), 0, 0};
	std::memcpy(ans+5, &Damage, 2);

	auto who_shooted = this->getUnit(from);
	short new_ap = who_shooted->getAP() - _DATA.getShootCost(who_shooted->getWeapon());
	who_shooted->setActivePoints(new_ap);

	std::memcpy(ans+11, &new_ap, 2);
	post(std::vector<char>(ans, ans+len));

	if (Damage) {
		auto who_get_shooted = this->getUnit(to);
		auto HP = who_get_shooted->getHP();
		if (Damage < HP) {
//			short new_hp = HP - Damage;
//			unit->setHitPoints(new_hp);
//			std::memcpy(ans+11, &new_hp, 2);
			who_get_shooted->setHitPoints(HP - Damage);
		} else {
			if (!this->removeUnit(to)) {
				return false;
			}
			//remove_unit(who_get_shooted); // физически уничтожается в списке
		}
	}

	if (new_ap == 0 && !this->there_are_units_with_not_null_ap(who_shooted->getPlayerID())) {
		unblock_after = false;
		force_next_turn(who_shooted->getPlayerID());
	} else {
		unblock_after = true;
	}

	return true;
}
//bool GameMap::add_union(std::list<int> new_union)
//{
//	return do_add_union(new_union);
//}

bool GameMap::do_add_hero(std::string Name, UnitDef ud, CellCoordinates where, CellCoordinates looks_to) 
{
	//return addUnit(where, Hero::create(Name, ud, looks_to));
	return addUnit(where, AbstractUnit::create(ud, looks_to, Name));
}
bool GameMap::do_add_unit(UnitDef ud, CellCoordinates where, CellCoordinates looks_to) 
{
	return addUnit(where, AbstractUnit::create(ud, looks_to));
}
bool GameMap::do_move_unit(std::vector<CellCoordinates>& Path, bool& unblock_after) {
	if (this->moveUnit(Path.front(), Path.back())) {
		auto unit = this->getUnit(Path.back());
		short new_ap = unit->getAP() + 1 - Path.size();
		unit->setCoordinates(Path.back());
		unit->setLook(Path.back().operator+((Path.back()) - Path[Path.size()-2]));
		unit->setActivePoints(new_ap);
		if (m_bGameStarted) {
		char ans[6] = {static_cast<char>(REQUEST_TYPE::SET), Path.front().x, Path.front().y,
			// старые координаты, так как на клиенте он еще не ходил
				static_cast<char>(PROPERTY::AP), 0, 0};
			std::memcpy(ans+4, &new_ap, 2);
			post(std::vector<char>(ans,ans+6));
			std::vector<char> move_string = get_move_string(Path);
			post(move_string);
		}
		if (new_ap == 0 && !this->there_are_units_with_not_null_ap(unit->getPlayerID())) {
			unblock_after = false;
			force_next_turn(unit->getPlayerID(), m_bGameStarted);
		} else {
			unblock_after = true;
		}
		return true;
	}
	return false;
}
short GameMap::calculate_damage(CellCoordinates from, CellCoordinates to) {
	this->getUnit(from)->setLook(to);

	auto Weapon = this->getUnit(from)->getWeapon();
	auto Distance = this->calculate_distance(from, to);
	bool Hit = rand()%100 < _DATA.getAccuracy(Weapon, Distance);

	if (Hit) {
		return std::rand()%(_DATA.getMaxDamage(Weapon, Distance)
							- _DATA.getMinDamage(Weapon, Distance))
				+ _DATA.getMinDamage(Weapon, Distance);
	} else {
		return 0;
	}
}
//bool GameMap::do_add_union(std::list<int> new_union) {
////	return this->addUnion(new_union);
//	return false;
//}
bool GameMap::next_turn(bool post_message) {
//	boost::mutex::scoped_lock(m_Mutex);
	//do {
	//	++m_ActivePlayer;
	//	if (m_ActivePlayer == online_clients.size())
	//		m_ActivePlayer = 1;
	//} while (!online_clients[m_ActivePlayer]);

	if (has_players() == false) {
		return false; // не должно такого быть
	}

	int PrevPlayerID = getActivePlayerId();
	int NextPlayerID = nextTurn();
	if (NextPlayerID == -1) {
		m_ActivePlayerIt = m_PlayersTurn.end();
		return false;
	}
	if (post_message) {
		if (PrevPlayerID != -1) {
			char msg[] = { static_cast<char>(REQUEST_TYPE::TURN_END), 0, 0, 0, 0,
							static_cast<char>(REQUEST_TYPE::TURN_BEGIN), 0, 0, 0, 0};
			INT(msg[1]) = PrevPlayerID;
			INT(msg[6]) = NextPlayerID;
			post(std::vector<char>(msg, msg+10));
		} else {
			//char msg[] = {TURN_BEGIN, 0, 0, 0, 0};
			std::vector<char> msg(sizeof(char) + sizeof(int), 0);
			msg[0] = static_cast<char>(REQUEST_TYPE::TURN_BEGIN);
			INT(msg[1]) = NextPlayerID;
			post(std::move( msg ));
		}
	}
	restore_active_points(NextPlayerID, post_message);
	if ((*m_ActivePlayerIt)->is_online()) {
		m_pTurnTimer->expires_from_now(TURN_TIME);
	} else {
		m_pTurnTimer->expires_from_now(TURN_TIME_FOR_OFFLINE);
	}
	m_pTurnTimer->async_wait(boost::bind(&GameMap::handle_turn_timer, this, _1));
	return true;
}
bool GameMap::force_next_turn(unsigned int client_id, bool post_message) {
	if (client_id == getActivePlayerId() || client_id == 1) { //_CLDATA.getClientID("Radagast") ) {
		m_pTurnTimer->cancel();
		return next_turn(post_message);
	} else {
		return true;
	}
}
void GameMap::restore_active_points(int player_id, bool post_message) {
	if (post_message) {
		for (auto it = this->m_Units.begin(); it != this->m_Units.end(); ++it) {
			if ((*it)->getPlayerID() == player_id) {
				short new_ap = _DATA.getDefaultAP((*it)->getType());
				(*it)->setActivePoints(new_ap);
				//char ans[6] = {REQUEST_TYPE::SET, (*it)->getCoordinates().x, (*it)->getCoordinates().y,
				//	PROPERTY::AP, 0, 0};
				//std::memcpy(ans+4, &new_ap, 2);
				std::vector<char> ans(sizeof(char)*4 + sizeof(short), 0);
				int i = 0;
				ans[i++] = static_cast<char>(REQUEST_TYPE::SET);
				ans[i++] = (*it)->getCoordinates().x;
				ans[i++] = (*it)->getCoordinates().y;
				ans[i++] = static_cast<char>(PROPERTY::AP);
				SHORT(ans[i]) = new_ap;
				post(std::move( ans ));
			}
		}
	} else {
		for (auto it = this->m_Units.begin(); it != this->m_Units.end(); ++it) {
			if ((*it)->getPlayerID() == player_id) 
				(*it)->setActivePoints(_DATA.getDefaultAP((*it)->getType()));
		}
	}
}

void GameMap::start_game() {
	const int tc = team_count(true); // тут юниты ещё не созданы
	std::vector<std::vector<Player::ptr>> teams(tc);
	for (int i = 0; i < tc; ++i) {
		teams[i].reserve(MAX_PLAYERS_IN_TEAM);
	}
	for (auto it = m_Players.begin(), e = m_Players.end(); it != e; ++it) {
		//++team_counts[it->second->get_team()];
		teams[it->second->get_team() - 1].push_back(it->second);
		create_start_unit(it->second);
		it->second->set_game_id(m_ID);
	}
	for (int i = 0; i < tc; ++i) {
		std::random_shuffle(teams[i].begin(), teams[i].end());
	}
	//auto teams_it = teams.begin();
	//int first_team = rand() % tc;
	//while (first_team-- > 0) ++teams_it;
	int cur_team = rand() % tc; // first turn

	//std::map<int, Player::ptr> Players;
	int players_count = m_Players.size();
	for (int i = 0; i < players_count; ++i) {
		while (teams[cur_team].empty()) {
			++cur_team;
			if (cur_team >= tc) cur_team = 0;
		}

		Player::ptr p = teams[cur_team].back();
		//Players[p->get_id()] = p;
		m_PlayersTurn.push_back(p);
		teams[cur_team].pop_back();

		++cur_team;
		if (cur_team >= tc) cur_team = 0;
	}

	//m_Players = std::move( Players );
	nextTurn();
	post_map_create_msg();
}
std::vector<char> GameMap::get_map_create_msg() {
	int N =	
		sizeof(char)*2					// LOAD_MAP + map_id
		+ m_Units.size() * 
			(sizeof(char)				// CREATE_UNIT
			+ sizeof(UnitDef)
			+ MAX_NICK_LENGTH
			+ 2*sizeof(CellCoordinates) // координаты и направление взгляда
			+ sizeof(short)				// current hp
			+ sizeof(char))				// current ap
		+ m_Players.size() *
			(sizeof(char)				// PLAYER_IN_GAME
			+ sizeof(int)				// id
			+ sizeof(char)				// team
			+ sizeof(char)				// is_online
			+ sizeof(char))				// has_units
		+ sizeof(char) + sizeof(int);	// объявление хода
		

	std::vector<char> req(N, 0);

	int i = 0;
	req[i++] = static_cast<char>(REQUEST_TYPE::LOAD_MAP);
	req[i++] = m_MapID;

	//int len = AbstractUnit::m_Units.size() * (UNIT_SIZE);
	//for (auto it = Hero::m_Units.begin(); it != Hero::m_Units.end(); ++it) {
	//	len += HERO_SIZE + dynamic_cast<Hero*>(it->get())->getName().size();
	//}
	//for (auto it = this->m_Unions.begin(); it != this->m_Unions.end(); ++it) {
	//	len += 2 + it->size();
	//}
	//for (auto it = clients.begin(); it != clients.end(); ++it) {
	//	len += 3 + it->second->username().size();
	//}
	//len += 2;
	//char* req = new char[len];

	//int player_id = msg[i++];
	//m_pConnection->set_team(player_id);
	//m_pGameMap->set_team(player_id);

	//for (auto it = this->m_Players.begin(); it != this->m_Players.end(); ++it) {
	//	req[i++] = NEW_PLAYER;

	//	*((int*)(&req[i])) = *it;
	//	i += sizeof(int);

	//	req[i++] = _SERVER.players()[*it].getTeam();

	//	std::string name = _CLDATA.getClientName(*it);
	//	req[i++] = name.size();
	//	std::memcpy(&req[i], name.c_str(), name.size());
	//	i += name.size();
	//}
	//for (auto it = AbstractUnit::m_Units.begin(); it != AbstractUnit::m_Units.end(); ++it) {
	//	req[i++] = CREATE;
	//	//req[i++] = it->get()->getType();
	//	//req[i++] = it->get()->getClass();
	//	//req[i++] = it->get()->getTeam();
	//	//*((short*)(&req[i])) = it->get()->getHP();
	//	//i += sizeof(short);
	//	//req[i++] = it->get()->getAP();
	//	//req[i++] = it->get()->getWeapon();
	//	//req[i++] = it->get()->getArmor();
	//	*( (UnitDef*)&req[i] ) = it->get()->getUnitDef();
	//	i += sizeof(UnitDef);
	//	req[i++] = it->get()->getCoordinates().x;
	//	req[i++] = it->get()->getCoordinates().y;
	//	req[i++] = it->get()->getLook().x;
	//	req[i++] = it->get()->getLook().y;
	//}

	for (auto it = m_Units.begin(), e = m_Units.end(); it != e; ++it) {
		//Hero* h = dynamic_cast<Hero*>(it->get());
		req[i++] = static_cast<char>(REQUEST_TYPE::CREATE_UNIT);
		//req[i++] = UNIT_TYPE::HERO;
		UD(req[i]) = (*it)->getUnitDef();
		i += sizeof(UnitDef);
		std::string name = (*it)->getName();
		req[i++] = name.size();
		memcpy_s(&req[i], name.size(), name.c_str(), name.size());
		i += name.size();
		//req[i++] = h->getType();
		//req[i++] = h->getClass();
		//req[i++] = h->getTeam();
		//*((short*)(&req[i])) = it->get()->getHP();
		//i += sizeof(short);
		//req[i++] = it->get()->getAP();
		//req[i++] = it->get()->getWeapon();
		//req[i++] = it->get()->getArmor();
		req[i++] = (*it)->getCoordinates().x;
		req[i++] = (*it)->getCoordinates().y;
		req[i++] = (*it)->getLook().x;
		req[i++] = (*it)->getLook().y;
		SHORT(req[i]) = (*it)->getHP();
		i += sizeof(short);
		req[i++] = (*it)->getAP();
	}
	
	for (auto it = m_Players.begin(), e = m_Players.end(); it != e; ++it) {
		req[i++] = static_cast<char>(REQUEST_TYPE::PLAYER_IN_GAME);
		INT(req[i]) = it->first;
		i += sizeof(int);
		req[i++] = it->second->get_team();
		req[i++] = it->second->is_online();
		req[i++] = it->second->has_units();
	}

	req[i++] = static_cast<char>(REQUEST_TYPE::TURN_BEGIN);
	//m_ActivePlayer = m_Players.begin();
	INT(req[i]) = (*m_ActivePlayerIt)->get_id();
	i += sizeof(int);

	req.resize(i);

	return req;
}
void GameMap::post_map_create_msg() {
	std::vector<char> msg = get_map_create_msg();
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		//it->second->set_game_id(m_ID);
		_SERVER.post(it->second, msg);
	}
}
std::vector<char> GameMap::get_move_string(std::vector<CellCoordinates>& Path) {
	std::vector<char> msg(2 + Path.size() * 2, 0);
	//msg.resize(2 + Path.size() * 2);

	int i = 0;

	msg[i++] = static_cast<char>(REQUEST_TYPE::MOVE);
	msg[i++] = Path.size() - 1;

	for (auto it = Path.begin(); it != Path.end(); ++it) {
		msg[i++] = it->x;
		msg[i++] = it->y;
	}

	return msg;
}

void GameMap::post(std::vector<char>&& msg, int except_player) {
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		if (it->first != except_player) {
			//_SERVER.players()[*it].get_client()->post(msg);
			_SERVER.post(it->second, msg);
		}
	}
}
void GameMap::post(const std::vector<char>& msg, int except_player) {
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		if (it->first != except_player) {
			//_SERVER.players()[*it].get_client()->post(msg);
			_SERVER.post(it->second, msg);
		}
	}
}

bool GameMap::has_players(int except_id) const {
	if (m_Players.empty()) {
		return false;
	}
	for (auto it = m_Players.begin(), e = m_Players.end(); it != e; ++it) {
		if (it->first != except_id && it->second->is_online()) {
			return true;
		}
	}
	return false;
}
bool GameMap::has_alive_players_except(int player_id) const {
	for (auto it = m_Players.begin(), e = m_Players.end(); it != e; ++it) {
		if (it->first != player_id && it->second->has_units()) {
			return true;
		}
	}
	return false;
}
bool GameMap::remove_player(int player_id) {
	auto it = m_Players.find(player_id);
	if (it == m_Players.end()) {
		return false;
	}

	Player::ptr player = it->second;

	if (m_ActivePlayerIt != m_PlayersTurn.end()) {
		if ((*m_ActivePlayerIt)->get_id() == player_id) {
			if (has_alive_players_except(player_id)) {
				next_turn();
			} else {
				m_ActivePlayerIt = m_PlayersTurn.end();
			}
		}
	}

	if (has_players(player_id)) {
		if (team_count() > 1 && player->has_units()) {
			return false;
		}
	}

	//it->second->set_is_in_game(false);
	
	m_Players.erase(it);
	m_PlayersTurn.remove(player);

	//for (auto it = m_PlayersTurn.begin(), e = m_PlayersTurn.end(); it != e; ++it) {
	//	if ((*it)->get_id() == player_id) {
	//		m_PlayersTurn.erase(it);
	//		break;
	//	}
	//}
	
	return true;
}
void GameMap::remove_players() {
	for (auto it = m_Players.begin(), e = m_Players.end(); it != e; ++it) {
		it->second->set_is_in_game(false);
	}
	m_Players.clear();
	m_PlayersTurn.clear();
	m_Units.clear();
}

int GameMap::team_count(bool dead_players_count) const {
	if (m_Players.empty()) {
		return 0;
	}
	bool teams[5] = {false};
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
		if (dead_players_count || it->second->has_units()) {
			teams[it->second->get_team()] = true;
		}
	}
	int k = 0;
	for (int i = 1; i < 5; ++i) {
		if (teams[i]) ++k;
	}
	return k;
}