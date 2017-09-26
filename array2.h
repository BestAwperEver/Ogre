#pragma once
#ifndef _ARRAY2_H_
#define _ARRAY2_H_

#define _ARRAY2_USES_CELLCOORDINATES_

#ifdef _ARRAY2_USES_CELLCOORDINATES_
#include "..\..\DataBase0\DataBase0\CellCoordinates.h"
#endif

template <class T>
class array2
{
protected:
	T** m_Array;
	int m_M;
	int m_N;
	bool m_bCleared;
	
	void allocateMemory(int M, int N);
	void freeMemory();

public:
	array2();
	array2(int M, int N, const T& defElement = T());
	array2(const array2&);
	array2(array2 &&);
	~array2(void);

	void initialize(int M, int N, const T& defElement = T());
	bool clear();
	bool cleared() {return m_bCleared;}

	int getM() const {return m_M;}
	int getN() const {return m_N;}
	bool inRange(char,char) const;
#ifdef _ARRAY2_USES_CELLCOORDINATES_
	bool inRange(CellCoordinates) const;
#endif

	T*& operator[] (int);
	T* operator[] (int) const;
	T& operator() (int,int);
	T operator() (int,int) const;
#ifdef _ARRAY2_USES_CELLCOORDINATES_
	T& operator() (CellCoordinates);
	T operator() (CellCoordinates) const;
#endif

	void operator = (const array2&);
	void operator = (array2 &&);
};

// is not safe!
template <class T> void array2<T>::allocateMemory(int M, int N) {
	m_Array = new T*[M];
	for (int i = 0; i < M; i++) {
		m_Array[i] = new T[N];
	}
}
template <class T> void array2<T>::freeMemory() {
	for (int i = 0; i < m_M; i++) {
		delete[] m_Array[i];
	}
	delete[] m_Array;
	m_Array = 0;
}

template <class T> array2<T>::array2() :
	m_M(0), m_N(0), m_Array(0), m_bCleared(true)
{

}
template <class T> array2<T>::array2(int M, int N, const T& defElement) :
	m_bCleared(false)
{
	assert(M > 0 && N > 0);

	m_M = M;
	m_N = N;
	m_Array = new T*[M];

	for (int i = 0; i < M; ++i) {
		m_Array[i] = new T[N];
		for (int j = 0; j < N; ++j) {
			m_Array[i][j] = defElement;
		}
	}
}
template <class T> array2<T>::array2(const array2& a) :
	m_M(a.m_M), m_N(a.m_N), m_bCleared(a.m_bCleared)
{
	if (m_bCleared) {
		m_Array = 0;
		return;
	}

	m_Array = new T*[m_M];

	for (int i = 0; i < m_M; ++i) {
		m_Array[i] = new T[m_N];
		for (int j = 0; j < m_N; ++j) {
			m_Array[i][j] = a.m_Array[i][j];
		}
	}

}
template <class T> array2<T>::array2(array2 &&a) :
	m_M(a.m_M), m_N(a.m_N), m_Array(a.m_Array), m_bCleared(a.m_bCleared)
{
	a.m_Array = nullptr;
	a.m_M = 0;
	a.m_N = 0;
	a.m_bCleared = true;
}

template <class T> array2<T>::~array2(void)
{
	clear();
}

template <class T> void array2<T>::initialize(int M, int N, const T& defElement) {
	assert(m_bCleared);

	m_M = M;
	m_N = N;
	m_Array = new T*[M];
	m_bCleared = false;

	for (int i = 0; i < M; ++i) {
		m_Array[i] = new T[N];
		for (int j = 0; j < N; ++j) {
			m_Array[i][j] = defElement;
		}
	}

}
template <class T> bool array2<T>::clear() {

	if (m_bCleared) {
		return false;
	}

	if (m_Array) {
		freeMemory();
	}

	m_M = 0;
	m_N = 0;

	m_bCleared = true;

	return true;
}

template <class T> T*& array2<T>::operator[] (int i) {
	assert(i < m_M && i >= 0);
	return m_Array[i];
}
template <class T> T* array2<T>::operator[] (int i) const {
	assert(i < m_M && i >= 0);
	return m_Array[i];
}
template <class T> T& array2<T>::operator() (int i, int j) {
	assert(i < m_M && i >= 0);
	assert(j < m_N && j >= 0);
	return m_Array[i][j];
}
template <class T> T array2<T>::operator() (int i, int j) const {
	assert(i < m_M && i >= 0);
	assert(j < m_N && j >= 0);
	return m_Array[i][j];
}
#ifdef _ARRAY2_USES_CELLCOORDINATES_
template <class T> T& array2<T>::operator() (CellCoordinates c) {
	assert(c.x < m_M && c.x >= 0);
	assert(c.y < m_N && c.y >= 0);
	return m_Array[c.x][c.y];
}
template <class T> T array2<T>::operator() (CellCoordinates c) const {
	assert(c.x < m_M && c.x >= 0);
	assert(c.y < m_N && c.y >= 0);
	return m_Array[c.x][c.y];
}
#endif

template <class T> void array2<T>::operator = (const array2& a) {
	if (a.m_bCleared) {
		clear();
		return;
	}
	if (m_M == a.m_M && m_N == a.m_N) {
		for (int i = 0; i < m_M; ++i) {
			for (int j = 0; j < m_N; ++j) {
				m_Array[i][j] = a.m_Array[i][j];
			}
		}
	}
	else {
		clear();
		allocateMemory(a.m_M, a.m_N);
		m_M = a.m_M;
		m_N = a.m_N;
		m_bCleared = false;

		for (int i = 0; i < m_M; ++i) {
			for (int j = 0; j < m_N; ++j) {
				m_Array[i][j] = a.m_Array[i][j];
			}
		}
	}
}
template <class T> void array2<T>::operator = (array2&& a) {
	freeMemory();
	m_M = a.m_M;
	m_N = a.m_N;
	m_Array = a.m_Array;
	m_bCleared = a.m_bCleared;

	a.m_bCleared = true;
}

template <class T> bool array2<T>::inRange(char m, char n) const {
	if (m >= m_M || n >= m_N || m < 0 || n < 0) {
		return false;
	}
	return true;
}
#ifdef _ARRAY2_USES_CELLCOORDINATES_
template <class T> bool array2<T>::inRange(CellCoordinates cc) const {
	if (cc.x >= m_M || cc.y >= m_N || cc.x < 0 || cc.y < 0) {
		return false;
	}
	return true;
}
#endif

#ifdef array2_bool_separate_implementation

#include <vector>

template <> class array2<bool> {
protected:
	int m_M;
	int m_N;
	std::vector<bool> m_Array; //?
public:
	array2();
	array2(int M, int N, bool defElement = false);
	array2(const array2&);
	array2(array2 &&);
	~array2(void);

	void initialize(int M, int N, bool defElement = false);
	bool clear();
	bool cleared() { return m_bCleared; }

	int getM() const { return m_M; }
	int getN() const { return m_N; }
	bool inRange(char, char);

	bool*& operator[] (int);
	bool* operator[] (int) const;
	bool& operator() (int, int);
	bool operator() (int, int) const;
#ifdef _ARRAY2_USES_CELLCOORDINATES_
	bool& operator() (CellCoordinates);
	bool operator() (CellCoordinates) const;
#endif

	void operator = (const array2&);
	void operator = (array2 &&);
};

#endif

#endif