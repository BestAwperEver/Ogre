#pragma once
#ifndef _ARRAY2BOOL.H_
#define _ARRAY2BOOL.H_

class array2bool {
	unsigned int* int_Array;
	unsigned int int_Array_Length;
	unsigned int m_Bool;
	unsigned int n_Bool;
public:
	array2bool();
	array2bool(bool**, unsigned int, unsigned int);
	array2bool(const array2bool&);
	~array2bool();
	bool** convert_To_Bool2();
	unsigned int getM();
	unsigned int getN();
	void change_Element(unsigned int, unsigned int, bool);
	bool operator () (unsigned int, unsigned int) const;
	void operator = (const array2bool&);
};

array2bool::array2bool() {
	int_Array = new unsigned int[0];
	int_Array_Length = 0;
	m_Bool = 0;
	n_Bool = 0;
}

array2bool::array2bool(bool** array, unsigned int m, unsigned int n) {
	m_Bool = m;
	n_Bool = n;
	if (m_Bool * n_Bool) {
		int_Array_Length = (m_Bool * n_Bool - 1) / 32 + 1;
		int_Array = new unsigned int[int_Array_Length];
		for (unsigned int i = 0; i < int_Array_Length; i++)
			int_Array[i] = 0;
		for (unsigned int i = 0; i < m_Bool * n_Bool; i++) {
			int count = i/32;
			int_Array[count] = (array[i / n_Bool][i % n_Bool]) ? int_Array[count] | (1 << i)
				 : int_Array[count];
		}
	}
}

array2bool::array2bool(const array2bool& array) {
	m_Bool = array.m_Bool;
	n_Bool = array.n_Bool;
	int_Array_Length = array.int_Array_Length;
	int_Array = new unsigned int[int_Array_Length];
	for (unsigned int i = 0; i < int_Array_Length; i++) {
		int_Array[i] = array.int_Array[i];
	}
}

array2bool::~array2bool() {
	if (int_Array)
		delete [] int_Array;
}

bool** array2bool::convert_To_Bool2() {
	bool** tmp = new bool*[m_Bool];
	for (unsigned int i = 0; i < m_Bool; i++) {
		tmp[i] = new bool[n_Bool];
		for (unsigned int j = 0; j < n_Bool; j++) {
			tmp[i][j] = int_Array[(i * n_Bool + j) / 32] & (1 << ((i * n_Bool + j) % 32));
		}
	}
	return tmp;
}

unsigned int array2bool::getM() {
	return m_Bool;
}

unsigned int array2bool::getN() {
    return n_Bool;
}

void array2bool::change_Element(unsigned int i, unsigned int j, bool value) {
	unsigned int count = (i * n_Bool + j) / 32;
	unsigned int pos = (i * n_Bool + j) % 32;
	unsigned int tmp = 0;
	for (unsigned int k = 0; k < 31 - pos; k++) {
		tmp += 1;
		tmp = tmp << 1;
	}
	for (unsigned int k = 0; k < pos; k++) {
		tmp = tmp << 1;
		tmp += 1;
	}
	int_Array[count] = (value) ? (int_Array[count] | (1 << ((i * n_Bool + j) % 32)))
								: (int_Array[count] & tmp);

}

bool array2bool::operator () (unsigned int i, unsigned int j) const {
	unsigned int count = (i * n_Bool + j) / 32;
	return (int_Array[count] & (1 << ((i * n_Bool + j) % 32)));
}

void array2bool::operator = (const array2bool& array) {
	m_Bool = array.m_Bool;
	n_Bool = array.n_Bool;
	int_Array_Length = array.int_Array_Length;
	int_Array = new unsigned int[int_Array_Length];
	for (unsigned int i = 0; i < int_Array_Length; i++) {
		int_Array[i] = array.int_Array[i];
	}
}
#endif
