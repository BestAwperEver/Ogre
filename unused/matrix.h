#include <iostream>
#include <string.h>
using namespace std;

#ifndef MATRIX_H
#define MATRIX_H

class matrix {
private:
	int M,N;
	string Name;
	double **p;
public:
//	friend class vector;

	matrix();
	matrix(int m, int n, string name = "unnamed");
	matrix(int n, string name = "unnamed");
	matrix(const matrix&);
	~matrix();

//	double*& operator[] (int);
	double* operator[] (int) const;

	double& operator() (int,int);

	bool operator == (const matrix&);
	matrix& operator = (const matrix&);

	matrix operator + (const matrix&);
	matrix operator - (const matrix&);
	matrix operator * (const matrix&);
	matrix& operator += (const matrix&);
	matrix& operator -= (const matrix&);
	matrix& operator *= (const matrix&);

	matrix operator * (double a);
	matrix operator * (float a);
	matrix operator * (int a);

//	vector operator * (const vector&);

	friend matrix operator * (double, matrix&);
	friend matrix operator * (float, matrix&);
	friend matrix operator * (int, matrix&);

	bool ruchzap();
	bool is_named();
	void term(string);
	void create(int m,int n,string name);
	void initialyze(int from_a,int to_b,bool integ);
	double determinant();
	void transp();
	bool is_square() const;

	friend ostream& operator << (ostream&,const matrix&);
	friend istream& operator >> (istream&, matrix&);
};

#endif