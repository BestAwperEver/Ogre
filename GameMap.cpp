#include "GameMap.h"
#include "AbstractUnit.h"
#include "LightMap.h"
#include "Player.h"
//#include "myapp.h"

//GameMap::GameMap(int N, Ogre::String Path):
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

//bool GameMap::loadFromFile(Ogre::String Path) {
//	std::ifstream f(Path, std::ios::binary);
//
//	if (!f.is_open()) 
//		return false;
//
//	char n;
//
//	f.read(&n, 1);
//
//	if (n != m_Dimension) {
//		f.close();
//		return false;
//	}
//
//	for (int i = 0; i < m_Dimension; i++) {
//		f.read(m_Map[i], m_Dimension);
//		if (f.bad()) {
//			f.close();
//			return false;
//		}
//	}
//
//	f.close();
//
//	copyMapToArray();
//
//	return true;
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
//bool GameMap::saveToFile(Ogre::String Path) {
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

//std::ostream& operator << (std::ostream& o, const GameMap& gm) {
//	for (int i = 0; i < gm.getDimension() - 1; i++) {
//		for (int j = 0; j < gm.getDimension() - 1; j++) 
//			o << gm(i,j) << ' ';
//		o << gm(i,gm.getDimension() - 1) << std::endl;
//	}
//	for (int j = 0; j < gm.getDimension() - 1; j++) 
//		o << gm(gm.getDimension() - 1,gm.getDimension() - 1) << ' ';
//	o << gm(gm.getDimension() - 1,gm.getDimension() - 1);
//
//	return o;
//}

GameMap::GameMap(int M, int N):
	m_OriginalMap(M, N),
	m_Map(M, N),
	m_UserMap(M, N),
	m_Array(M, N), 
	//	m_ServerMap(M,N).
	m_Units(M, N, 0),
	m_LightMap(this, M, N),
	m_pLightMap(nullptr),
	m_bInitialized(true),
	m_UnitInTeamCount(5, 0)
{}

GameMap::GameMap(): m_bInitialized(true), m_LightMap(this) {}

GameMap::GameMap(const MapContainer& _Map)
	: m_Map(_Map)
	, m_OriginalMap(_Map)
	//, m_Array(_Map)
	//	, m_ServerMap(_Map)
	, m_Array(_Map.getHeight(), _Map.getWidth())
	, m_UserMap(_Map)
	, m_LightMap(this, _Map.getHeight(), _Map.getWidth())
	, m_pLightMap(nullptr)
	, m_Units(_Map.getHeight(), _Map.getWidth())
	, m_bInitialized(true)
	, m_UnitInTeamCount(5, 0)
{}

GameMap::~GameMap(void)
{
	//for (int i = 0; i < m_Dimension; i++) {
	//	delete[] m_Map[i];
	//}
	//delete[] m_Map;

	//for (int i = 0; i < m_Dimension; i++) {
	//	delete[] m_Array[i];
	//}
	//delete[] m_Array;

	//m_Map = 0;
	//m_Array = 0;
	//m_Dimension = 0;

	if (m_pLightMap) {
		delete m_pLightMap;
		m_pLightMap = 0;
	}
}

char GameMap::operator() (int i,int j) const {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	return m_Map[i][j];
}
char& GameMap::operator() (int i,int j) {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	return m_Map[i][j];
}

//bool GameMap::getPath(CellCoordinates from, CellCoordinates to, std::vector<CellCoordinates>& path,
//	int maxPathLength) {
//#ifdef _USE_ASSERTS_
//		assert( m_bInitialized );
//		assert( path.empty() );
//		assert( path.capacity() > maxPathLength );
//		assert( from.x < getHeight() && from.y < getWidth() );
//		assert( to.x < getHeight() && to.y < getWidth() );
//#endif
//
//		if (m_Map(to) == WALL || m_Map(to) == UNIT) {
//			return false;
//		}
//
//		const int W      = getWidth();         // ширина рабочего поля
//		const int H      = getHeight();         // высота рабочего поля
//
//		//const int ax	= from.x;
//		//const int ay	= from.y;
//
//		//const int bx	= to.x;
//		//const int by	= to.y;
//
//		const int n = 8;
//
//		int len;                       // длина пути
//
//		int dx[n] = {1, 0, -1, 0, 1, 1, -1, -1};   // смещения, соответствующие соседям ячейки
//		int dy[n] = {0, 1, 0, -1, -1, 1, 1, -1};   // справа, снизу, слева и сверху
//		//CellCoordinates s[n] = {CellCoordinates(1,0),CellCoordinates(1,1),CellCoordinates(0,1),CellCoordinates(-1,1),
//		//	CellCoordinates(-1,0),CellCoordinates(-1,-1),CellCoordinates(0,-1),CellCoordinates(1,-1)};
//		int d, x, y, k;
//		bool stop;
//
//		// распространение волны
//		d = 0;
//		m_Array(from) = 0;            // стартовая ячейка помечена 0
//		do {
//			stop = true;               // предполагаем, что все свободные клетки уже помечены
//			for ( x = 0; x < H; ++x )
//				for ( y = 0; y < W; ++y )
//					if ( m_Array[x][y] == d )                         // ячейка (x, y) помечена числом d
//					{
//						for ( k = 0; k < n; ++k )                    // проходим по всем непомеченным соседям
//							if ( y + dy[k] >= 0 && y + dy[k] < W &&
//								x + dx[k] >= 0 && x + dx[k] < H &&
//								m_Array[x + dx[k]][y + dy[k]] == BLANK )
//							{	
//								if (k < 4) {
//									stop = false;							// найдены непомеченные клетки
//									m_Array[x + dx[k]][y + dy[k]] = d + 1;      // распространяем волну
//								}
//								else {
//									// если пытаемся пойти по диагонали, то нужно убедиться, что клетки,
//									// соседствующие с той, в которую хотим пойти, тоже свободны
//									if (m_Map[x + dx[k]][y] == BLANK
//										&& m_Map[x][y + dy[k]] == BLANK) {
//											stop = false;							
//											m_Array[x + dx[k]][y + dy[k]] = d + 1;     
//									}
//								}
//							}
//					}
//					d++;
//					if (d > maxPathLength && !stop) {
//						return false;
//					}
//		} while ( !stop && m_Array(to) == BLANK );
//
//		if (m_Array(to) == BLANK) {
//			m_Array = m_Map;
//			return false;  // путь не найден
//		}
//
//		// восстановление пути
//		len = m_Array(to);            // длина кратчайшего пути из (ax, ay) в (bx, by)
//		x = to.x;
//		y = to.y;
//		d = len;
//		//path.push_back(CellCoordinates(-1,-1));
//		while ( d > 0 )
//		{
//			//px[d] = x;
//			//py[d] = y;                   // записываем ячейку (x, y) в путь
//			path.push_back(CellCoordinates(x,y));
//			--d;
//			for (k = 0; k < n; ++k)
//				if (y + dy[k] >= 0 && y + dy[k] < W &&
//					x + dx[k] >= 0 && x + dx[k] < H &&
//					m_Array[x + dx[k]][y + dy[k]] == d) {
//						if (dx[k] != 0 && dy[k] != 0) {
//							if (passable(x, y + dy[k]) && passable(x + dx[k], y)) {
//								x = x + dx[k];
//								y = y + dy[k];	// переходим в ячейку, которая на 1 ближе к старту
//								break;
//							}
//						} else {
//							x = x + dx[k];
//							y = y + dy[k];	// переходим в ячейку, которая на 1 ближе к старту
//							break;
//						}
//				}
//		}
//		//px[0] = ax;
//		//py[0] = ay;                    // теперь px[0..len] и py[0..len] - координаты ячеек пути
//
//		m_Array = m_Map;
//		return true;
//}

bool GameMap::getPath(CellCoordinates from, CellCoordinates to, std::vector<CellCoordinates>& path,
	int maxPathLength) {
#ifdef _USE_ASSERTS_
		assert( m_bInitialized );
		assert( path.empty() );
//		assert( path.capacity() > maxPathLength );
		assert( from.x < getHeight() && from.y < getWidth() );
		assert( to.x < getHeight() && to.y < getWidth() );
#endif

		if (path.capacity() < maxPathLength) {
			path.reserve(maxPathLength);
		}

		if (!passable(to, true)) {
			return false;
		}

		m_Array = m_UserMap;

		const int W      = getWidth();         // ширина рабочего поля
		const int H      = getHeight();         // высота рабочего поля

		const int n = 8;

		int len;                       // длина пути

		int dx[n] = {1, 0, -1, 0, 1, 1, -1, -1};   // смещения, соответствующие соседям ячейки
		int dy[n] = {0, 1, 0, -1, -1, 1, 1, -1};   // справа, снизу, слева и сверху

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
						for ( k = 0; k < n; ++k )                    // прохОдим по всем непомеченным соседям
							if ( y + dy[k] >= 0 && y + dy[k] < W &&
								x + dx[k] >= 0 && x + dx[k] < H &&
								(m_Array[x + dx[k]][y + dy[k]] == static_cast<char>(MAP_SYMBOL::BLANK) ||
								m_Array[x + dx[k]][y + dy[k]] == static_cast<char>(MAP_SYMBOL::OPENED_DOOR)))
							{	
								if (k < 4) {
									stop = false;							// найдены непомеченные клетки
									m_Array[x + dx[k]][y + dy[k]] = d + 1;      // распространяем волну
								}
								else {
									// если пытаемся пойти по диагонали, то нужно убедиться, что клетки,
									// соседствующие с той, в которую хотим пойти, тоже свободны
									if (passable(x + dx[k], y, true)
										&& passable(x, y + dy[k], true)) {
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
		} while ( !stop && (m_Array(to) == static_cast<char>(MAP_SYMBOL::BLANK) || m_Array(to) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR)) );

		if (m_Array(to) == static_cast<char>(MAP_SYMBOL::BLANK) || m_Array(to) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR)) {
			//m_Array = m_UserMap;
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
					m_Array[x + dx[k]][y + dy[k]] == d) {
						if (dx[k] != 0 && dy[k] != 0) {
							if (passable(x, y + dy[k], true) && passable(x + dx[k], y, true)) {
								x = x + dx[k];
								y = y + dy[k];	// переходим в ячейку, которая на 1 ближе к старту
								break;
							}
						} else {
							x = x + dx[k];
							y = y + dy[k];	// переходим в ячейку, которая на 1 ближе к старту
							break;
						}
				}
		}
		//px[0] = ax;
		//py[0] = ay;                    // теперь px[0..len] и py[0..len] - координаты ячеек пути

		return true;
}
void GameMap::getRealPath(std::vector<CellCoordinates>& path, int range) {
#ifdef _USE_ASSERTS_
	assert( !path.empty() );
#endif
	if (path.size() == 1) return;

	FieldOfView lm(m_LightMap);

	for (int i = 0; i < path.size(); ++i) {
		auto LM = getLightMap(path[i], range);
		if (new_enemy_or_closed_door(path[i], lm, LM)) {
			path.resize(i + 1);
			return;
		}
		lm.decrement(path[i], LM, false);
	}
}
bool GameMap::new_enemy_or_closed_door(CellCoordinates where, FieldOfView& FoV, const array2<bool>& lm)
{
	bool res = false;
	for(int i = 0; i < lm.getM(); ++i) {
		for (int j = 0; j < lm.getN(); ++j) {
			if ( lm( i, j) ) {
				CellCoordinates cur_cell(where.x + i - FoV.getRange(lm),
										 where.y + j - FoV.getRange(lm));
				if (!FoV.inRange(cur_cell)) continue;
				if (FoV(cur_cell.x, cur_cell.y) == -1) {
					FoV(cur_cell.x, cur_cell.y) = 1;
					if (res == false &&
						(getUnit(cur_cell) && getUnit(cur_cell)->getPlayerID() != getPlayerID() ||
						m_Map(cur_cell) == static_cast<char>(MAP_SYMBOL::CLOSED_DOOR) &&
						m_UserMap(cur_cell) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR)))
					{
						//m_pGameMap->getUnit(cur_cell)->setVisible(true);
						res = true;
					}
				} else if (FoV(cur_cell.x, cur_cell.y)++ == 0) {
					if (res == false &&
						(getUnit(cur_cell) && getUnit(cur_cell)->getPlayerID() != getPlayerID() ||
						m_Map(cur_cell) == static_cast<char>(MAP_SYMBOL::CLOSED_DOOR) &&
						m_UserMap(cur_cell) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR)))
					{
						//m_pGameMap->getUnit(cur_cell)->setVisible(true);
						res = true;
					}
				}
			}
		}
	}
	return res;
}
//bool GameMap::inUnion(int t1, int t2) {
//	if (t1 == t2) return true;
//	m_teamForFinding = t1;
//	auto u = m_Unions.end();
//	for (auto it = m_Unions.begin(); it != m_Unions.end(); ++it) {
//		if (std::find(it->begin(), it->end(), t1) != it->end()) {
//			u = it;
//			break;
//		}
//	}
//	if (u == m_Unions.end()) {
//		return false;
//	}
//	return std::find(u->begin(), u->end(), t2) != u->end();
//}
bool GameMap::inUnion(AbstractUnit* unit1, AbstractUnit* unit2) {
	return unit1->getTeam() == unit2->getTeam();
}

bool GameMap::in_map(CellCoordinates cc) const {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	return cc.x < getHeight() && cc.y < getWidth() && cc.x >= 0 &&  cc.y >= 0;
}
bool GameMap::in_map(char x, char y) const {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	// x - width, y - height?
	return y < getHeight() && x < getWidth() && x >= 0 &&  y >= 0;
}
bool GameMap::passable(CellCoordinates cc, bool as_user_sees) const {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	if (!in_map(cc)) return false;

	if (as_user_sees)
		return m_UserMap(cc) == static_cast<char>(MAP_SYMBOL::BLANK) || m_UserMap(cc) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR);

	return m_Map(cc) == static_cast<char>(MAP_SYMBOL::BLANK) || m_Map(cc) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR);
	//	|| (!units_counted && m_Map(cc) == static_cast<char>(MAP_SYMBOL::UNIT));
}
bool GameMap::passable(char x, char y, bool as_user_sees) const {
	return passable(CellCoordinates(x, y), as_user_sees);
}
bool GameMap::visible(char x, char y) const {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	if (!in_map(x, y)) return false;
	return m_Map(x, y) == static_cast<char>(MAP_SYMBOL::BLANK) || m_Map(x, y) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR)
		|| m_Map(x, y) == static_cast<char>(MAP_SYMBOL::UNIT);
}
bool GameMap::visible(CellCoordinates cc) const {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	if (!in_map(cc)) return false;
	return m_Map(cc) == static_cast<char>(MAP_SYMBOL::BLANK) || m_Map(cc) == static_cast<char>(MAP_SYMBOL::OPENED_DOOR)
		|| m_Map(cc) == static_cast<char>(MAP_SYMBOL::UNIT);
}
//bool GameMap::visible(CellCoordinates cc) {
//	if (!in_map(cc)) return false;
//	return m_LightMap(cc) > 0;
//	//	return obj == LIGHT_static_cast<char>(MAP_SYMBOL::BLANK) || obj == LIGHT_static_cast<char>(MAP_SYMBOL::OPENED_DOOR);
//}
bool GameMap::direct_los_exists(CellCoordinates from, CellCoordinates to) const {
	char x = from.x;
	char y = from.y;
	char dir_x = to.x - from.x;
	char dir_y = to.y - from.y;
	char dx = std::abs(dir_x);
	char dy = std::abs(dir_y);
	char sx = dir_x > 0 ? 1 : -1;
	char sy = dir_y > 0 ? 1 : -1;
	//char range = sqrtf(dx*dx+dy*dy);
	int err = dx - dy;
	char x_sum = 0;
	char y_sum = 0;

	bool res1 = true;//, res2 = true;

	while (true)
	{

		if (x == to.x && y == to.y)
			break;

		if (!(x == from.x && y == from.y) && (!in_map(x, y) || !visible(x, y))) {
			res1 = false;
			break;
		}

		int e2 = (2 * err);

		if (e2 > -dy)
		{
			++x_sum;
			err -= dy;
			x += sx;
		}
		if (e2 < dx)
		{
			++y_sum;
			err += dx;
			y += sy;
		}

	}

	//while (true)
	//{

	//	if (x == to.x && y == to.y)
	//		break;
	//			
	//	if (!(x == from.x && y == from.y) && (!in_map(x, y) || !passable(x, y))) {
	//		res2 = false;
	//		break;
	//	}
	//			
	//	int e2 = (2 * err);

	//	if (e2 > -dy && std::abs(e2 + dy) > std::abs(e2 - dx))
	//	{
	//		++x_sum;
	//		err -= dy;
	//		x += sx;
	//	}
	//	if (e2 < dx)
	//	{
	//		++y_sum;
	//		err += dx;
	//		y += sy;
	//	}

	//}

	//return res1 && res2;

	return res1;

}
bool GameMap::line_of_sight_exists(CellCoordinates from, CellCoordinates to) const {
	//return (from.x - to.x)*(from.x - to.x) + (from.y - to.y)*(from.y - to.y) > 121 ?
	//	direct_los_exists(from, to) && direct_los_exists(to, from):
	//	direct_los_exists(from, to) || direct_los_exists(to, from);
	return direct_los_exists(from, to) || direct_los_exists(to, from);
}
bool GameMap::already_seen(CellCoordinates cc) {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	if (!in_map(cc)) return false;
	return m_LightMap(cc) != -1;
	//LIGHT_static_cast<char>(MAP_SYMBOL::INVISIBLE);//== LIGHT_static_cast<char>(MAP_SYMBOL::ALREADY_SEEN);
	//	return obj == LIGHT_static_cast<char>(MAP_SYMBOL::BLANK) || obj == LIGHT_static_cast<char>(MAP_SYMBOL::OPENED_DOOR);
}
void GameMap::addUnit(CellCoordinates where, AbstractUnit* unit) {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
	assert( in_map(where) );
#endif
	m_Map(where) = static_cast<char>(MAP_SYMBOL::UNIT);
	//m_ServerMap(where) = UNIT;
	//m_Array(where) = UNIT;
	m_Units(where) = unit;
	if (unit->getTeam() == m_Team) {
		unit->setVisible(true);
		//m_LightMap.increment(where, m_pLightMap->getLightMapTempObj(where, unit->getViewRange()));
		m_LightMap.increment(where, getLightMap(where, unit->getViewRange()));
	} else {
		//if (m_LightMap(where) > 0)
		//	unit->setVisible(true);
		//else 
		//	unit->setVisible(false);
		unit->setVisible(m_LightMap(where) > 0);
	}
	if (unit->getPlayerID() == getPlayerID()) {
		m_MyUnits.push_back(MyUnit(unit));
		//unit->setActive(false);
	}
	if (m_LightMap(where) > 0) {
		m_UserMap(where) = static_cast<char>(MAP_SYMBOL::UNIT);
	}

	++m_UnitInTeamCount[unit->getTeam()];

}
void GameMap::removeUnit(CellCoordinates where) {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	if (m_Map(where) != static_cast<char>(MAP_SYMBOL::UNIT)) {
		return;
		// на случай, если он уже был удален с карты,
		// но остался в списке всех юнитов (т.е., например, умер, но моделька еще жива)
	}
	--m_UnitInTeamCount[m_Units(where)->getTeam()];
	m_Map(where) = m_OriginalMap(where);
	m_UserMap(where) = m_OriginalMap(where);
	//m_Array(where) = BLANK;
	if (m_Units(where)->getTeam() == getTeam()) {
		//m_LightMap.decrement(where, m_pLightMap->getLightMapTempObj(where, m_Units(where)->getViewRange()));
		m_LightMap.decrement(where, getLightMap(where, m_Units(where)->getViewRange()));
	}
	if (m_Units(where)->getPlayerID() == getPlayerID()) {
		for (auto it = m_MyUnits.begin(); it != m_MyUnits.end(); ++it) {
			if (it->ref == m_Units(where)) {
				it = m_MyUnits.erase(it);
				break;
			}
		}
		//m_MyUnits.remove(MyUnit(m_Units(where)));
	}
	m_Units(where) = 0;
}
void GameMap::moveUnit(CellCoordinates from, CellCoordinates to) {
#ifdef _USE_ASSERTS_
	assert( m_bInitialized );
#endif
	if (from == to) 
		return;
	addUnit(to, m_Units(from));
	removeUnit(from);
}
char GameMap::calculate_distance(CellCoordinates from, CellCoordinates to) {
	return std::sqrtf( (to.x - from.x)*(to.x - from.x) + (to.y - from.y)*(to.y - from.y) );
}
void GameMap::clear_units() {
	for (int i = 0; i < m_Map.getHeight(); ++i) 
		for (int j = 0; j < m_Map.getWidth(); ++i) {
			if (m_Map[i][j] == static_cast<char>(MAP_SYMBOL::UNIT)) {
				removeUnit(CellCoordinates(i,j));
			}
		}
}

void GameMap::setActiveUnit(AbstractUnit* unit) {
	bool finded = false; // нашел ли такого юнита (должен найти)
	for (auto it = m_MyUnits.begin(); it != m_MyUnits.end(); ++it) {
		if (it->ref == unit) {
			finded = true;
			it->isActive = true;
		}
		else it->isActive = false;
	}
#ifdef _USE_ASSERTS_
	assert(finded && "Can't find unit to be set active");
#endif
}
void GameMap::deselectActiveUnit() {
	for (auto it = m_MyUnits.begin(); it != m_MyUnits.end(); ++it) {
		it->isActive = false;
	}
}
AbstractUnit* GameMap::getMyFirstUnit() {
	if (m_MyUnits.empty()) return 0;
	return m_MyUnits.front().ref;
}

void GameMap::setMapContainer(const MapContainer& NewMap) {
	m_OriginalMap = NewMap;
	m_UserMap = NewMap;
	m_Map = NewMap;
	//m_Array = m_Map;
	m_LightMap.clear();
	m_LightMap.initialize(m_Map.getHeight(), m_Map.getWidth(), -1);
	//	m_ServerMap = m_Map;
	m_Units.clear();
	m_Units.initialize(m_Map.getHeight(), m_Map.getWidth());
}
void GameMap::setMapContainer(MapContainer&& NewMap) {
	m_Map = NewMap;
	m_OriginalMap = NewMap;
	m_UserMap = NewMap;
	//m_Array = m_Map;
	m_LightMap.clear();
	m_LightMap.initialize(m_Map.getHeight(), m_Map.getWidth(), -1);
	//	m_ServerMap = m_Map;
	m_Units.clear();
	m_Units.initialize(m_Map.getHeight(), m_Map.getWidth());
}

void GameMap::recalculateLightMaps(char min_range, char max_range) {

	return;

	m_bInitialized = true;
	if (m_pLightMap) {
		delete m_pLightMap;
		m_pLightMap = 0;
	}
	m_pLightMap = new LightMap(*this, min_range, max_range);
}

//#include <boost/lexical_cast.hpp>
//#include <string>
//#include <fstream>

array2<bool> GameMap::getLightMap(CellCoordinates cc, int range)
{
//	return m_pLightMap->getLightMapTempObj(cc, range);

	//static bool flag = false;
	//if (flag == false) {
	//	flag = true;
	//	std::ofstream of;
	//	for (int i = 0; i < 32; ++i) {
	//		for (int j = 0; j < 32; ++j) {
	//			auto lm = getLightMap(CellCoordinates(i, j), 8);
	//#ifdef _DEBUG
	//			of.open("maps/map" + boost::lexical_cast<std::string>(i) + "_" +  boost::lexical_cast<std::string>(j) + ".txt");
	//#else
	//			of.open("maps/map" + boost::lexical_cast<std::string>(i) + "_" +  boost::lexical_cast<std::string>(j) + ".txt");
	//#endif
	//			for (int i1 = 0; i1 < lm.getM(); ++i1) {
	//				for (int j1 = 0; j1 < lm.getN(); ++j1) {
	//					of << lm(i1, j1);
	//				}
	//				of << std::endl;
	//			}
	//			of.close();
	//		}
	//	}
	//}

	array2<bool> lm(2*range + 1, 2*range + 1, false);
	int from_x = cc.x, from_y = cc.y;
	int i = -range, j = -range;
	CellCoordinates to;
	const int range2 = range * range;

	for (i = -range; i <= range; ++i) {
		to.x = cc.x + i;
		for (j = -range; j <= range; ++j) {
			if (i * i + j * j <= range2) {
				if (in_map(from_x + i, from_y + j)) {
					to.y = cc.y + j;
					lm(i + range, j + range) = line_of_sight_exists(cc, to);
				}
			}
		}
	}

	//for (i = -range; i <= range; ++i) {
	//	to.x = cc.x + i;
	//	for (j = -range; j <= range; ++j) {
	//		if (i * i + j * j <= range2) {
	//			if (in_map(from_x + i, from_y + j)) {
	//				//bool res1 = true, res2 = true;
	//				//if (lm(i + range, j + range) == false) {
	//				//	int x = from_x;
	//				//	int y = from_y;
	//				//	int dx = std::abs(i);
	//				//	int dy = std::abs(j);
	//				//	int sx = i > 0 ? 1 : -1;
	//				//	int sy = j > 0 ? 1 : -1;
	//				//	int err = dx - dy;
	//				//	//int x_sum = 0;
	//				//	//int y_sum = 0;

	//				//	while (true)
	//				//	{

	//				//		if (x == from_x + i && y == from_y + j)
	//				//			break;

	//				//		if (!(x == from_x && y == from_y) && (!in_map(x, y) || !passable(x, y))) {
	//				//			res1 = false;
	//				//			break;
	//				//		} else {
	//				//			lm(x - from_x + range, y - from_y + range) = true;
	//				//		}

	//				//		int e2 = (2 * err);

	//				//		if (e2 > -dy) {
	//				//			//++x_sum;
	//				//			err -= dy;
	//				//			x += sx;
	//				//		}
	//				//		if (e2 < dx) {
	//				//			//++y_sum;
	//				//			err += dx;
	//				//			y += sy;
	//				//		}

	//				//	}
	//				//} else {
	//				//	continue;
	//				//}
	//				//if (!res1) {
	//				//	int x = from_x + i;
	//				//	int y = from_y + j;
	//				//	int dx = std::abs(-i);
	//				//	int dy = std::abs(-j);
	//				//	int sx = -i > 0 ? 1 : -1;
	//				//	int sy = -j > 0 ? 1 : -1;
	//				//	int err = dx - dy;

	//				//	while (true)
	//				//	{

	//				//		if (x == from_x && y == from_y)
	//				//			break;

	//				//		if (!(x == from_x + i && y == from_y + j) && (!in_map(x, y) || !passable(x, y))) {
	//				//			res2 = false;
	//				//			break;
	//				//		}

	//				//		int e2 = (2 * err);

	//				//		if (e2 > -dy) {
	//				//			err -= dy;
	//				//			x += sx;
	//				//		}
	//				//		if (e2 < dx) {
	//				//			err += dx;
	//				//			y += sy;
	//				//		}
	//				//	}
	//				//} else {
	//				//	lm(i + range, j + range) = true;
	//				//	continue;
	//				//}
	//				//if (res2) {
	//				//	lm(i + range, j + range) = true;
	//				//}
	//				to.y = cc.y + j;
	//				lm(i + range, j + range) = line_of_sight_exists(cc, to);
	//			}
	//		}
	//	}
	//}

	return lm;
}

void GameMap::set_default_state() {
	clear_units();
	m_Array.clear();
	m_UserMap.clear();
	m_Map.clear();
	m_OriginalMap.clear();
	m_LightMap.clear();
	//if (m_pLightMap) {
	//	delete m_pLightMap;
	//	m_pLightMap = 0;
	//}
	m_Units.clear();
	m_bInitialized = false;
}

int GameMap::team_count() const {
	//if (m_Players.empty()) {
	//	return 0;
	//}
	//bool teams[5] = {false};
	//for (auto it = m_Players.begin(); it != m_Players.end(); ++it) {
	//	teams[it->second->get_team())] = true;
	//}
	int k = 0;
	for (int i = 0; i < 5; ++i) {
		if (m_UnitInTeamCount[i] > 0) ++k;
	}
	return k;
}