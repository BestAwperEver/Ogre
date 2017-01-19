#include "LightMap.h"
#include "GameMap.h"
#include <cassert>
#include <cmath>

LightMap::LightMap(const GameMap& _map, char min_range, char max_range, LM_CAST_TYPE ct)
	: m_LightMaps(_map.getHeight(), _map.getWidth()), m_CastType(ct), map(_map)
{
#ifdef _USE_ASSERTS_
	assert( map.getHeight() > 0 && map.getHeight() < 128 && map.getWidth() > 0 && map.getWidth() < 128 );
	assert( min_range > 0 && min_range <= max_range );
#endif
	for (int i = 0; i < map.getHeight(); ++i) {
		for (int j = 0; j < map.getWidth(); ++j) {
			for (int k = min_range; k <= max_range; ++k) {
#ifdef FORCE_MAKE_VISIBLE
				m_LightMaps(i,j)[k].initialize(k*2+1, k*2+1, false);
#else
				m_LightMaps(i,j)[k].initialize(k*2+1, k*2+1, true);
#endif
			}
		}
	}
	for (int i = 0; i < map.getHeight(); ++i) {
		for (int j = 0; j < map.getWidth(); ++j) {
			for (int k = min_range; k <= max_range; ++k) {
				array2<uchar> lm(2*k+1, 2*k+1, 0);
				if (m_CastType == LM_CAST_TYPE::LM_RAYCAST) {
					make_light_map(i, j, k, true, 1, lm);
					make_light_map(i, j, k, false, 2, lm);
				} else {
					make_light_map(i, j, k, 3, lm);
				}
				execute_light_map(i, j, k, 3, lm);
			}
		}
	}
}

void LightMap::make_light_map(char cell_x, char cell_y, char range, bool allow_diagonal_steps,
	uchar touch_mask, array2<uchar>& light_map)
{
	if (!map.in_map(cell_x, cell_y) || !map.passable(cell_x, cell_y)) return;

	char dx = -range;
	char dy = -range;

	while (dx <= range)
		cast_ray(cell_x, cell_y, range, dx++, dy, allow_diagonal_steps, touch_mask, light_map);
	dx--;
	while (dy <= range)
		cast_ray(cell_x, cell_y, range, dx, dy++, allow_diagonal_steps, touch_mask, light_map);
	dy--;
	while (dx >= -range)
		cast_ray(cell_x, cell_y, range, dx--, dy, allow_diagonal_steps, touch_mask, light_map);
	dx++;
	while (dy >= -range)
		cast_ray(cell_x, cell_y, range, dx, dy--, allow_diagonal_steps, touch_mask, light_map);
	dy++;
}

void LightMap::execute_light_map(char cell_x, char cell_y, char range,
		uchar touch_mask, array2<uchar>& light_map)
{
	array2<bool>& cur_map = m_LightMaps(cell_x, cell_y)[range];
	for (int i = -range; i <= range; ++i) {
		for (int j = -range; j <= range; ++j) {
#ifdef FORCE_MAKE_VISIBLE
#ifdef TOUCH_MASK_BOTH
			if ( (light_map(range + i, range + j) & touch_mask) == touch_mask) {
#else
			if ( (light_map(range + i, range + j) & touch_mask) ) {
#endif
				// set that cell visible from this
				cur_map(range + i, range + j) = true;
				// force set this cell visible from cell which is now visible from this
				// for the great justice
				m_LightMaps(cell_x + i, cell_y + j)[range](range - i, range - j) = true;
#else
#ifdef TOUCH_MASK_BOTH
			if ( (light_map(range + i, range + j) & touch_mask) != touch_mask) {
#else
			char lm = light_map(range + i, range + j);
			if ( (lm & touch_mask) == 0 ) {
			//if ( lm != touch_mask ) {
#endif
				cur_map(range + i, range + j) = false;
				if (m_LightMaps.inRange(cell_x + i, cell_y + j))
					m_LightMaps(cell_x + i, cell_y + j)[range](range - i, range - j) = false;
#endif
			} 
			//if (cell_x == 9 && cell_y == 10 && i == 2 && j == -4) {
			//	int a = 5;
			//}
			//if (cell_x == 10 && cell_y == 9 && i == -4 && j == 2) {
			//	int a = 5;
			//}
		}
	}
	cur_map(range, range) = true; // to see itself
}

bool LightMap::cast_ray(char cell_x, char cell_y, char range, char dir_x, char dir_y, bool allow_diagonal_steps,
		uchar touch_mask, array2<uchar>& light_map)
{
	char x = cell_x;
	char y = cell_y;
	char dx = std::abs(dir_x);
	char dy = std::abs(dir_y);
	char sx = dir_x > 0 ? 1 : -1;
	char sy = dir_y > 0 ? 1 : -1;
	int err = dx - dy;
	char x_sum = 0;
	char y_sum = 0;
	//move startX, startY to targetX, targetY
	while (true)
	{
		//max range reached?
		if (x_sum * x_sum + y_sum * y_sum > range * range)
			break;
				
		//check for collsion
		//var ld:LightGridData = fetch(x, y); //TODO: optimize - avoid function call
		if (!map.in_map(x, y) || !map.passable(x, y))
			return false;
		//if (ld == null || ld.opaque)
		//	return false;
				
		//touch! set flags as specified in the touchMask
		//ld.touched |= touchMask;
		light_map(range + (x - cell_x), range + (y - cell_y)) |= touch_mask;
				
		int e2 = (2 * err);
		if(allow_diagonal_steps == false)
		{
			//either step vertically or horizontal, based on 
			//wether deviaton from the ideal line is currently bigger
			//in x- or y-direction.
			if (e2 > -dy && std::abs(e2 + dy) > std::abs(e2 - dx))
			{
				++x_sum;
				err -= dy;
				x += sx;
			}
			else if (e2 < dx)
			{
				++y_sum;
				err += dx;
				y += sy;
			}					
		}
		else
		{
			//diagonal movements are allowed too. Just step, when error big enough!
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
	}
	return true;

}

bool LightMap::visible(char from_x, char from_y, char range, char dir_x, char dir_y, bool allow_diagonal_steps)
{
	char x = from_x;
	char y = from_y;
	char dx = std::abs(dir_x);
	char dy = std::abs(dir_y);
	char sx = dir_x > 0 ? 1 : -1;
	char sy = dir_y > 0 ? 1 : -1;
	int err = dx - dy;
	char x_sum = 0;
	char y_sum = 0;

	while (true)
	{
		if (!map.in_map(x, y) || !map.passable(x, y))
			return false;
				
		int e2 = (2 * err);
		if(allow_diagonal_steps == false)
		{
			//either step vertically or horizontal, based on 
			//wether deviaton from the ideal line is currently bigger
			//in x- or y-direction.
			if (e2 > -dy && std::abs(e2 + dy) > std::abs(e2 - dx))
			{
				++x_sum;
				err -= dy;
				x += sx;
			}
			else if (e2 < dx)
			{
				++y_sum;
				err += dx;
				y += sy;
			}					
		}
		else
		{
			//diagonal movements are allowed too. Just step, when error big enough!
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
	}
	return true;

}

/////////// SHADOWCASTING

void LightMap::make_light_map(char cell_x, char cell_y, char range, 
		uchar touch_mask, array2<uchar>& light_map)
{
    for (int i = 1; i < 9; ++i)
        ScanOctant(cell_x, cell_y, range, touch_mask, light_map, 1, i, 1.0, 0.0);
}

int LightMap::GetVisDistance(char x1, char y1, char x2, char y2) {
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

double LightMap::GetSlope(double pX1, double pY1, double pX2, double pY2, bool pInvert)
{
    if (pInvert)
        return (pY1 - pY2) / (pX1 - pX2);
    else
        return (pX1 - pX2) / (pY1 - pY2);
}

void LightMap::ScanOctant(char cell_x, char cell_y, char range, 
		uchar touch_mask, array2<uchar>& light_map,
		int pDepth, int pOctant, double pStartSlope, double pEndSlope)
{

    int visrange2 = range * range;
    int x = 0;
    int y = 0;

    switch (pOctant)
    {

        case 1: //nnw
            y = cell_y - pDepth;
            if (y < 0) return;

            x = cell_x - pStartSlope * pDepth;
            if (x < 0) x = 0;

            while (GetSlope(x, y, cell_x, cell_y, false) >= pEndSlope)
            {
                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {
					if (!map.passable(x, y)) //current cell blocked
                    {
						if (x - 1 >= 0 && map(x - 1, y) == static_cast<char>(MAP_SYMBOL::BLANK)) //prior cell within range AND open...
                            //...incremenet the depth, adjust the endslope and recurse
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x - 0.5, y + 0.5, cell_x, cell_y, false));
                    }
                    else
                    {

                        if (x - 1 >= 0 && map(x - 1, y) == 1) //prior cell within range AND open...
                            //..adjust the startslope
                            pStartSlope = GetSlope(x - 0.5, y - 0.5, cell_x, cell_y, false);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                x++;
            }
            x--;
            break;

        case 2: //nne

            y = cell_y - pDepth;
            if (y < 0) return;

            x = cell_x + pStartSlope * pDepth;
            if (x >= map.getHeight()) x = map.getHeight() - 1;

            while (GetSlope(x, y, cell_x, cell_y, false) <= pEndSlope)
            {
                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {
                    if (!map.passable(x, y))
                    {
                        if (x + 1 < map.getHeight() && map(x + 1, y) == static_cast<char>(MAP_SYMBOL::BLANK))
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x + 0.5, y + 0.5, cell_x, cell_y, false));
                    }
                    else
                    {
                        if (x + 1 < map.getHeight() && map(x + 1, y) == 1)
                            pStartSlope = -GetSlope(x + 0.5, y - 0.5, cell_x, cell_y, false);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                x--;
            }
            x++;
            break;

        case 3:

            x = cell_x + pDepth;
            if (x >= map.getHeight()) return;

            y = cell_y - pStartSlope * pDepth;
            if (y < 0) y = 0;

            while (GetSlope(x, y, cell_x, cell_y, true) <= pEndSlope)
            {

                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {

                    if (!map.passable(x, y))
                    {
                        if (y - 1 >= 0 && map(x, y - 1) == static_cast<char>(MAP_SYMBOL::BLANK))
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x - 0.5, y - 0.5, cell_x, cell_y, true));
                    }
                    else
                    {
                        if (y - 1 >= 0 && map(x, y - 1) == 1)
                            pStartSlope = -GetSlope(x + 0.5, y - 0.5, cell_x, cell_y, true);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                y++;
            }
            y--;
            break;

        case 4:

            x = cell_x + pDepth;
            if (x >= map.getHeight()) return;

            y = cell_y + pStartSlope * pDepth;
            if (y >= map.getWidth()) y = map.getWidth() - 1;

            while (GetSlope(x, y, cell_x, cell_y, false) >= pEndSlope)
            {

                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {

                    if (!map.passable(x, y))
                    {
                        if (y + 1 < map.getWidth() && map(x, y + 1) == static_cast<char>(MAP_SYMBOL::BLANK))
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x - 0.5, y + 0.5, cell_x, cell_y, true));
                    }
                    else
                    {
                        if (y + 1 < map.getWidth() && map(x, y + 1) == 1)
                            pStartSlope = GetSlope(x + 0.5, y + 0.5, cell_x, cell_y, true);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                y--;
            }
            y++;
            break;

        case 5:

            y = cell_y + pDepth;
            if (y >= map.getWidth()) return;

            x = cell_x + pStartSlope * pDepth;
            if (x >= map.getHeight()) x = map.getHeight() - 1;

            while (GetSlope(x, y, cell_x, cell_y, false) >= pEndSlope)
            {
                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {

                    if (!map.passable(x, y))
                    {
                        if (x + 1 < map.getWidth() && map(x + 1, y) == static_cast<char>(MAP_SYMBOL::BLANK))
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x + 0.5, y - 0.5, cell_x, cell_y, false));
                    }
                    else
                    {
                        if (x + 1 < map.getWidth()
                                && map(x + 1, y) == 1)
                            pStartSlope = GetSlope(x + 0.5, y + 0.5, cell_x, cell_y, false);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                x--;
            }
            x++;
            break;

        case 6:

            y = cell_y + pDepth;
            if (y >= map.getWidth()) return;

            x = cell_x - pStartSlope * pDepth;
            if (x < 0) x = 0;

            while (GetSlope(x, y, cell_x, cell_y, false) <= pEndSlope)
            {
                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {

                    if (!map.passable(x, y))
                    {
                        if (x - 1 >= 0 && map(x - 1, y) == static_cast<char>(MAP_SYMBOL::BLANK))
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x - 0.5, y - 0.5, cell_x, cell_y, false));
                    }
                    else
                    {
                        if (x - 1 >= 0
                                && map(x - 1, y) == 1)
                            pStartSlope = -GetSlope(x - 0.5, y + 0.5, cell_x, cell_y, false);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                x++;
            }
            x--;
            break;

        case 7:

            x = cell_x - pDepth;
            if (x < 0) return;

            y = cell_y + pStartSlope * pDepth;
            if (y >= map.getWidth()) y = map.getWidth() - 1;

            while (GetSlope(x, y, cell_x, cell_y, true) <= pEndSlope)
            {

                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {

                    if (!map.passable(x, y))
                    {
                        if (y + 1 < map.getWidth() && map(x, y + 1) == static_cast<char>(MAP_SYMBOL::BLANK))
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x + 0.5, y + 0.5, cell_x, cell_y, true));
                    }
                    else
                    {
                        if (y + 1 < map.getWidth() && map(x, y + 1) == 1)
                            pStartSlope = -GetSlope(x - 0.5, y + 0.5, cell_x, cell_y, true);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                y--;
            }
            y++;
            break;

        case 8: //wnw

            x = cell_x - pDepth;
            if (x < 0) return;

            y = cell_y - pStartSlope * pDepth;
            if (y < 0) y = 0;

            while (GetSlope(x, y, cell_x, cell_y, true) >= pEndSlope)
            {

                if (GetVisDistance(x, y, cell_x, cell_y) <= visrange2)
                {

                    if (!map.passable(x, y))
                    {
                        if (y - 1 >= 0 && map(x, y - 1) == static_cast<char>(MAP_SYMBOL::BLANK))
                            ScanOctant(cell_x, cell_y, range,
								touch_mask, light_map,
								pDepth + 1, pOctant, pStartSlope,
								GetSlope(x + 0.5, y - 0.5, cell_x, cell_y, true));

                    }
                    else
                    {
                        if (y - 1 >= 0 && map(x, y - 1) == 1)
                            pStartSlope = GetSlope(x - 0.5, y - 0.5, cell_x, cell_y, true);

                        light_map(x - cell_x + range, y - cell_y + range) |= touch_mask;
                    }
                }
                y++;
            }
            y--;
            break;
    }


    if (x < 0)
        x = 0;
	else if (x >= map.getHeight())
        x = map.getHeight() - 1;

    if (y < 0)
        y = 0;
    else if (y >= map.getWidth())
        y = map.getWidth() - 1;

    if (pDepth < range && map(x, y) == static_cast<char>(MAP_SYMBOL::BLANK)) // поменял & на &&
        ScanOctant(cell_x, cell_y, range,
			touch_mask, light_map,
			pDepth + 1, pOctant, pStartSlope, pEndSlope);

}