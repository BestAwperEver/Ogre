#pragma once
#ifndef _MAP_CONTAINER_H_
#define _MAP_CONTAINER_H_

#include "MapObject.h"
#include "array2.h"

class MapContainer
{
	//char**	m_pArray;
	//char	m_Height;
	//char	m_Width;
	array2<char> m_Array;

	std::vector<MapObjectDef> m_Objects;

	//bool m_bCleared;

public:
	MapContainer();
	MapContainer(int Height, int Width);
	MapContainer(int Height, int Width, int ObjectCount);
	MapContainer(int Height, int Width, const std::vector<MapObjectDef>& Objects);
	MapContainer(int Height, int Width, std::vector<MapObjectDef>&& Objects);
	MapContainer(const MapContainer&);
	MapContainer(MapContainer&&);

	char getHeight() const {return m_Array.getM();}
	char getWidth() const {return m_Array.getN();}
//	void setHeight(char Height);
//	void seWidth(char Width);

	std::vector<MapObjectDef>& getObjects() {return m_Objects;} 
	const std::vector<MapObjectDef>& getObjectsConstRef() const {return m_Objects;} 
	std::vector<MapObjectDef> getObjectsTempObj() const {return m_Objects;}
	std::vector<MapObjectDef>* getObjectsPtr() {return &m_Objects;}

	array2<char>& getArray() {return m_Array;} 
	const array2<char>& getArrayConstRef() const {return m_Array;} 
	array2<char> getArrayTempObj() const {return m_Array;}
	array2<char>* getArrayPtr() {return &m_Array;}

	void clear() {m_Array.clear();}

	void operator = (const MapContainer& mc);
	void operator = (MapContainer&& mc);
	char*& operator[] (int i) {return m_Array[i];}
	char* operator[] (int i) const {return m_Array[i];}
	char& operator() (int i,int j) {return m_Array[i][j];}
	char operator() (int i,int j) const {return m_Array[i][j];}
	char& operator() (CellCoordinates cc) {return m_Array(cc);}
	char operator() (CellCoordinates cc) const {return m_Array(cc);}
};

#endif