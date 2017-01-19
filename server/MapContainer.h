#pragma once
#ifndef _MAP_CONTAINER_H_
#define _MAP_CONTAINER_H_

#include "MapObject.h"

class MapContainer
{
	char**	m_pArray;
	char	m_Height;
	char	m_Width;

	std::vector<MapObjectDef> m_Objects;

	bool m_bCleared;

public:
	MapContainer();
	MapContainer(int Height, int Width);
	MapContainer(int Height, int Width, int ObjectCount);
	MapContainer(int Height, int Width, std::vector<MapObjectDef> Objects);
	MapContainer(const MapContainer&);
	MapContainer(MapContainer&&);
	~MapContainer(void);

	char getHeight() const {return m_Height;}
	char getWidth() const {return m_Width;}
//	void setHeight(char Height);
//	void seWidth(char Width);

	std::vector<MapObjectDef>& getObjects() {return m_Objects;} 
	std::vector<MapObjectDef> getObjectsTempObj() const {return m_Objects;}
	std::vector<MapObjectDef>* getObjectsPtr() {return &m_Objects;}
	const std::vector<MapObjectDef>& getObjectsConstRef() const {return m_Objects;} 

	void clear();

	void operator = (const MapContainer&);
	void operator = (MapContainer&&);
	char*& operator[] (int);
	char* operator[] (int) const;
	char& operator() (int,int);
	char operator() (int,int) const;
	char& operator() (CellCoordinates);
	char operator() (CellCoordinates) const;
};

#endif