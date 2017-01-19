#pragma once
#ifndef _CELL_COORDINATES_H_
#define _CELL_COORDINATES_H_

#include <string>

class CellCoordinates {
public:

	char x;
	char y;

	CellCoordinates(int M = -1, int N = -1): x(M), y(N) {}

	char first() const {return x;}
	char second() const {return y;}

	CellCoordinates operator + (const CellCoordinates& c);
	CellCoordinates operator - (const CellCoordinates& c);

	bool operator != (const CellCoordinates& c);
	bool operator == (const CellCoordinates& c);

	std::string to_string() const;
	std::string to_readable_string() const;
};

#endif