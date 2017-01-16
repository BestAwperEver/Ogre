#include "FieldOfView.h"
#include "GameMap.h"
#include "AbstractUnit.h"

FieldOfView::FieldOfView(GameMap* pGameMap, char DefaultEl):
	array2<char>(pGameMap->getHeight(), pGameMap->getWidth(), DefaultEl),
	m_pGameMap(pGameMap)
{}
FieldOfView::FieldOfView(GameMap* pGameMap, char Height, char Width, char DefaultEl):
	array2<char>(Height, Width, DefaultEl),
	m_pGameMap(pGameMap)
{}
FieldOfView::FieldOfView(FieldOfView&& FOV):
	array2<char>(std::move( FOV )),
	m_pGameMap(FOV.m_pGameMap)
{}
void FieldOfView::operator=(FieldOfView&& FOV) {
	array2<char>::operator=(std::move( FOV ));
	m_pGameMap = FOV.m_pGameMap;
}

void FieldOfView::decrement(CellCoordinates where, const array2<bool>& lm, bool affect_gamemap) {
	for(int i = 0; i < lm.getM(); ++i)
		for (int j = 0; j < lm.getN(); ++j)
			if ( lm( i, j) ) {
				CellCoordinates cur_cell(where.x + i - getRange(lm),
										where.y + j - getRange(lm));
				if (!inRange(cur_cell)) continue;
#ifdef _USE_ASSERTS_
				assert( m_Array[cur_cell.x][cur_cell.y] && "FieldOfView::decrement");
#endif
				if (--m_Array[cur_cell.x][cur_cell.y] == 0) {
					if (affect_gamemap && m_pGameMap->getUnit(cur_cell) 
						&& m_pGameMap->getUnit(cur_cell)->getPlayerID() != m_pGameMap->getPlayerID()) 
					{
						m_pGameMap->getUnit(cur_cell)->setVisible(false);
					}
				}
			}
}
void FieldOfView::increment(CellCoordinates where, const array2<bool>& lm, bool affect_gamemap) {
	for(int i = 0; i < lm.getM(); ++i)
		for (int j = 0; j < lm.getN(); ++j)
			if ( lm( i, j) ) {
				CellCoordinates cur_cell(where.x + i - getRange(lm),
										where.y + j - getRange(lm));
				if (!inRange(cur_cell)) continue;
				if (m_Array[cur_cell.x][cur_cell.y] == -1) {
					m_Array[cur_cell.x][cur_cell.y] = 1;
					if (affect_gamemap && m_pGameMap->getUnit(cur_cell) 
						&& m_pGameMap->getUnit(cur_cell)->getPlayerID() != m_pGameMap->getPlayerID()) 
					{
						m_pGameMap->getUnit(cur_cell)->setVisible(true);
					}
				} else if (m_Array[cur_cell.x][cur_cell.y]++ == 0) {
					if (affect_gamemap && m_pGameMap->getUnit(cur_cell) 
						&& m_pGameMap->getUnit(cur_cell)->getPlayerID() != m_pGameMap->getPlayerID()) 
					{
						m_pGameMap->getUnit(cur_cell)->setVisible(true);
					}
				}
			}
}