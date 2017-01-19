#include "CellCoordinates.h"

CellCoordinates CellCoordinates::operator + (const CellCoordinates& c) {
	return CellCoordinates(x + c.x, y + c.y);
}
CellCoordinates CellCoordinates::operator - (const CellCoordinates& c) {
	return CellCoordinates(x - c.x, y - c.y);
}
bool CellCoordinates::operator != (const CellCoordinates& c) {
	return (x != c.x || y != c.y);
}
bool CellCoordinates::operator == (const CellCoordinates& c) {
	return (x == c.x && y == c.y);
}
std::string CellCoordinates::to_string() const {
	char str[3];
	str[0] = x;
	str[1] = y;
	str[2] = 0;
	return str;
}
std::string CellCoordinates::to_readable_string() const {
	return std::to_string((long long)x) + " " + std::to_string((long long)y);
}