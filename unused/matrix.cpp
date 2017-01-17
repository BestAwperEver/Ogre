#include "matrix.h"
//#include "vector.h"
/*
16.11.12 21:51 странная проблема: во всех случаях умножения, сложения etc. 
статическая матрица не даёт делать create на себя, т.е. ее значения нифига не меняются
и это не связано с private типом полей матриц
поэтому приходится создавать tmp матрицу, а потом уже ее присваивать статической, так получается

16.11.12 22:44 проблема исчезла О_о

28.01.2013 21:20 исправил ошибку в деструкторе - перепутал M и N

14.03.2013 02:44 сделал дз по векторам, теперь можно на них умножать 
и еще можно считать определитель матрицы по гауссу, причем нигде не будет 
деления на ноль B-)

15.03.2013 11:56 сделал констатный вариант операции индексации, изменил в операции плюса
B.p[i][j] на B[i][j], ошибок не выдало, наверное работает правильно
*/

ostream& operator << (ostream& stream, const matrix &A) {
//	stream<<endl;
	for (int i=0;i<A.M;i++) {
		for (int j=0;j<A.N;j++)
			stream<<A.p[i][j]<<' ';
		if (i<A.M-1) stream<<endl;
	}
	return stream;
}
istream& operator >> (istream& stream, matrix &A) {
	for (int i=0;i<A.M;i++)
		for (int j=0;j<A.N;j++) {
			cin>>A.p[i][j];
		}
	return stream;
}

matrix operator * (double a, matrix &A) {
	if (A.p==NULL) {
		cout<<"something goes wrong: "<<a<<'*'<<A.Name<<endl;
		throw;
	}
	static matrix P;
	P.~matrix();
	P.create(A.M,A.N,"ProductOnDouble");
	for (int i=0;i<A.M;i++)
		for (int j=0;j<A.N;j++) {
			P.p[i][j]=A.p[i][j]*a;
		}
	return P; 
}
matrix operator * (float a, matrix &A) {
	if (A.p==NULL) {
		cout<<"something goes wrong: "<<a<<'*'<<A.Name<<endl;
		throw;
	}
	static matrix P;
	P.~matrix();
	P.create(A.M,A.N,"ProductOnDouble");
	for (int i=0;i<A.M;i++)
		for (int j=0;j<A.N;j++) {
			P.p[i][j]=A.p[i][j]*a;
		}
	return P; 
}
matrix operator * (int a, matrix &A) {
	if (A.p==NULL) {
		cout<<"something goes wrong: "<<a<<'*'<<A.Name<<endl;
		throw;
	}
	static matrix P;
	P.~matrix();
	P.create(A.M,A.N,"ProductOnDouble");
	for (int i=0;i<A.M;i++)
		for (int j=0;j<A.N;j++) {
			P.p[i][j]=A.p[i][j]*a;
		}
	return P; 
}

matrix::matrix() { 
	M=0;
	N=0;
	p=NULL;
	Name="unnamed";
}
matrix::matrix(int m,int n,string name):
	Name(name)
{
	M=m;
	N=n;
	//if (strlen(name)>0) {
	//	Name = new char[strlen(name)+1];
	//	strcpy(Name,name);
	//}
//	else Name=NULL;
	if (M>0) {
		p = new double* [M];
		if (N>0) for (int i=0;i<M;i++) {
			p[i] = new double [N];
		}
		else {
			delete []p;
			p=NULL;
		}
	}
	else p=NULL;
}
matrix::matrix(int n,string name):
	Name(name)
{
	M=n;
	N=n;
	//if (strlen(name)>0) {
	//	Name = new char[strlen(name)+1];
	//	strcpy(Name,name);
	//}
//	else Name=NULL;
	if (M>0) {
		p = new double* [M];
		if (N>0) for (int i=0;i<M;i++) {
			p[i] = new double [N];
		}
		else {
			delete []p;
			p=NULL;
		}
	}
	else p=NULL;
}
matrix::matrix(const matrix& B) {
	//if (*this==B) return;
	//matrix::~matrix();
	//if (!B.Name) {
	//	throw "triyng to create unnamed matrix";
	//}
	Name = B.Name;
	p = new double*[B.M];
	for (int i=0;i<B.M;i++) {
		p[i] = new double [B.N];
		for (int j=0;j<B.N;j++) 
			p[i][j] = B.p[i][j];
	}
	M=B.M;
	N=B.N;
	//Name = new char[strlen(B.Name)+1];
	//strcpy(Name,B.Name);
}
matrix::~matrix() {
	for (int i=0;i<M;i++)
		if (p[i])  {
				delete []p[i];
				p[i]=NULL;
		}
	if (p) {
			delete []p;
			p=NULL;
	}
	M=NULL;
	N=NULL;
//	if (Name) delete []Name;
//	Name=NULL;
}

bool matrix::operator == (const matrix &B) {
	if (M!=B.M || N!=B.N) return false;
	for (int i=0;i<M;i++)
		for (int j=0;j<N;j++)
			if ( p[i][j] != B.p[i][j] ) return false;
	return true;
}
//double*& matrix::operator[](int i) {
//	return p[i];
//}
double* matrix::operator[] (int i) const {
	return p[i];
}
double& matrix::operator() (int i, int j) {
	return p[i][j];
}

matrix& matrix::operator = (const matrix &B) {
	if (this==&B) return *this;
	for (int i=0;i<M;i++)
		if (p[i])  {
				delete []p[i];
				p[i]=NULL;
		}
	if (p) {
			delete []p;
			p=NULL;
	}
	try {
		p = new double*[B.M];
		for (int i=0;i<B.M;i++) {
			p[i] = new double [B.N];
			for (int j=0;j<B.N;j++) 
				p[i][j] = B.p[i][j];
		}
		M=B.M;
		N=B.N;
		return *this;
	}
	catch (...) {
		cout<<"something goes wrong: "<<Name<<'='<<B.Name<<endl;
//		return false;
	}
}

matrix matrix::operator + (const matrix &B) {
	//if (Name==NULL || B.Name==NULL) {
	//	char emsg[] = "trying to sum unnamed matrixes";
	//	throw emsg;
	//}
	if (N!=B.N || M!=B.M || p==NULL || B.p==NULL) {
		//cout<<"something goes wrong: "<<Name<<'+'<<B.Name<<endl;
		//throw;
		//char emsg[] = "something goes wrong: ";
		//strcat(emsg,Name);
		//char * emsg = new char[strlen(Name)+1];
		//strcpy(emsg,Name);
		//strcat(emsg,"+");
		//strcat(emsg,B.Name);
		//strcat(emsg," - invalid dimensionality");
		string emsg = Name + '+' + B.Name + " - invalid dimensionality";
		throw emsg;
	}
	//char* nm = new char[strlen(Name)+strlen(B.Name)+2];
	//strcpy(nm,Name);
	//strcpy(nm,"+");
	//strcpy(nm,B.Name);
	string nm = Name + '+' + B.Name;
	matrix S(M,N,nm);
//	delete []nm;
//	S.~matrix();
//	S.create(M,N,"Sum");
	for (int i=0;i<M;i++)
		for (int j=0;j<N;j++) {
			S.p[i][j] = p[i][j] + B[i][j];
		}
	return S;
}
matrix matrix::operator - (const matrix &B) {
	//if (Name==NULL || B.Name==NULL) {
	//	char emsg[] = "trying to subtract unnamed matrixes";
	//	throw emsg;
	//}
	if (N!=B.N || M!=B.M || p==NULL || B.p==NULL) {
		//cout<<"something goes wrong: "<<Name<<'-'<<B.Name<<endl;
		//throw;
		//char emsg[] = "something goes wrong: ";
		//strcat(emsg,Name);
		//char * emsg = new char[strlen(Name)+1];
		//strcpy(emsg,Name);
		//strcat(emsg,"-");
		//strcat(emsg,B.Name);
		//strcat(emsg," - invalid dimensionality");
		string emsg = Name + '-' + B.Name + " - invalid dimensionality";
		throw emsg;
	}
	//char* nm = new char[strlen(Name)+strlen(B.Name)+2];
	//strcpy(nm,Name);
	//strcpy(nm,"-");
	//strcpy(nm,B.Name);
	string nm = Name + '-' + B.Name;
	matrix S(M,N,nm);
//	delete []nm;
	//matrix tmp(M,N,"TmpDifference");
//	S.~matrix();
	//S=tmp;
//	S.create(M,N,"Difference");
	//S.term("Difference");
	for (int i=0;i<M;i++)
		for (int j=0;j<N;j++) {
			S.p[i][j] = p[i][j] - B.p[i][j];
		}
	return S;
}
matrix matrix::operator * (const matrix &B) {
	//if (Name==NULL || B.Name==NULL) {
	//	char emsg[] = "trying to multiplicate unnamed matrixes";
	//	throw emsg;
	//}
	if (N!=B.M || p==NULL || B.p==NULL) {
//		cout<<"something goes wrong: "<<Name<<'*'<<B.Name<<endl;
//		char * emsg;
//		emsg = new char[24+strlen(Name)+strlen(B.Name)];
		//char emsg[] = "something goes wrong: ";
		//strcat(emsg,Name);
		//char * emsg = new char[strlen(Name)+1];
		//strcpy(emsg,Name);
		//strcat(emsg,"*");
		//strcat(emsg,B.Name);
		//strcat(emsg," - invalid dimensionality");
		string emsg = Name + '*' + B.Name + " - invalid dimensionality";
		throw emsg;
	}
	//char* nm = new char[strlen(Name)+strlen(B.Name)+2];
	//strcpy(nm,Name);
	//strcat(nm,"*");
	//strcat(nm,B.Name);
	string nm = Name + '*' + B.Name;
	matrix P(M,B.N,nm);
//	delete []nm;
//	P.~matrix();
//	matrix tmp(N,B.M,"TmpProductionOnMatrix");
//	P.create(N,B.M,"ProductionOnMatrix");
//	P=tmp;
//	P.term("ProductionOnMatrix");
//	P.term("product");
//	P.create(N,B.M,"ProductionOnMatrix");
	for (int i=0;i<M;i++)
		for (int j=0;j<B.N;j++) {
			P.p[i][j]=0;
			for (int k=0;k<N;k++) 
				P.p[i][j]+=p[i][k]*B.p[k][j];
		}
//	cout<<P; //wtf
	return P; 
}

matrix& matrix::operator+=(const matrix &B) {
	*this = *this + B;
	return *this;
}
matrix& matrix::operator-=(const matrix &B) {
	*this = *this - B;
	return *this;
}
matrix& matrix::operator*=(const matrix &B) {
	*this = *this * B;
	return *this;
}

matrix matrix::operator * (double a) {
	if (p==NULL) {
		cout<<"something goes wrong: "<<Name<<'*'<<a<<endl;
		throw;
	}
	matrix P(M,N,"ProductionOnDouble");
//	matrix tmp(M,N,"TmpProductionOnDouble");
//	P.~matrix();
//	P=tmp;
	//P.~matrix();
//	P.create(M,N,"ProductionOnDouble");
	for (int i=0;i<M;i++)
		for (int j=0;j<N;j++) {
			P.p[i][j]=p[i][j]*a;
		}
	return P; 
}
matrix matrix::operator * (float a) {
	if (p==NULL) {
		cout<<"something goes wrong: "<<Name<<'*'<<a<<endl;
		throw;
	}
	matrix P(M,N,"ProductionOnFloat");
//	matrix tmp(M,N,"TmpProductionOnFloat");
//	P.~matrix();
	//P=tmp;
	//P.term("ProductionOnFloat");
	//P.~matrix();
//	P.create(M,N,"ProductionOnFloat");
	for (int i=0;i<M;i++)
		for (int j=0;j<N;j++) {
			P.p[i][j]=p[i][j]*a;
		}
	return P; 
}
matrix matrix::operator * (int a) {
	if (p==NULL) {
		cout<<"something goes wrong: "<<Name<<'*'<<a<<endl;
		throw;
	}
	matrix P(M,N,"ProductionOnInt");
//	matrix tmp(M,N,"TmpProductionOnInt");
//	P.~matrix();
	//P=tmp;
	//P.term("ProductionOnInt");
	//P.~matrix();
//	P.create(M,N,"ProductionOnInt");
	for (int i=0;i<M;i++)
		for (int j=0;j<N;j++) {
			P.p[i][j]=p[i][j]*a;
		}
	return P; 
}

bool matrix::ruchzap() {
	if (!M || !N) {
		cout<<"something goes wrong: "<<Name<<".ruchzap"<<endl;
		return false;
	}
	cout<<"Заполните матрицу "<<Name<<" ("<<M<<'x'<<N<<')'<<endl;
	for (int i=0;i<M;i++) {
		cout<<"Строка "<<i+1<<": ";
		for (int j=0;j<N;j++)
			cin>>p[i][j];
	}
	return true;
}
bool matrix::is_named() {
	return !(Name == "unnamed");
}
void matrix::term(string s) {
	/*if (Name) delete []Name;
	Name = new char[strlen(s)+1];
	strcpy(Name,s);*/
	Name = s;
}
void matrix::create(int m, int n, string name) {
	if (!(Name == "unnamed")) {
		//cout<<"something goes wrong: "<<this->Name<<".create"<<endl;
		//return;
		//char emsg[] = "something goes wrong: ";
		//strcat(emsg,Name);
		string emsg = Name + ".create - trying to create matrix in already created";
		//strcpy(emsg,Name);
		//strcat(emsg,".create - trying to create matrix in already created");
		throw emsg;
	}
	M=m;
	N=n;
	//Name = new char[strlen(name)+1];
	//strcpy(Name,name);
	Name = name;
	p = new double* [M];
	for (int i=0;i<M;i++) {
		p[i] = new double [N];
	}
}
void matrix::initialyze(int from_a,int to_b,bool integ=true) {
	if (Name == "unnamed") {
		string emsg = "trying to initialyze unnamed matrix";
		throw emsg;
	}
	if (M==0 || N==0) {
		//cout<<"something goes wrong: initialyzing null matrix"<<endl;
		//throw "something goes wrong: initialyzing null matrix";
		//char emsg[] = "something goes wrong: initializating null matrix";
		//strcat(emsg,Name);
		string emsg = Name + ".initialyze - trying to initialize 0x0 matrix";
		//char * emsg = new char[strlen(Name)+1];
		//strcpy(emsg,Name);
		//strcat(emsg,".initialyze - trying to initialize 0x0 matrix");
		throw emsg;
	}
	if (!integ)
		for (int i=0;i<M;i++)
			for (int j=0;j<N;j++) 
				p[i][j] = ( (double)rand()/RAND_MAX ) * (to_b-from_a) + from_a;
	else
		for (int i=0;i<M;i++)
			for (int j=0;j<N;j++) 
				p[i][j] = (int)( ( (double)rand()/RAND_MAX ) * (to_b-from_a) + from_a );
}

bool findNZ(double** p,int n,int i,int k,int &i1,int &i2) {
	for (int l=i;l<n;l++)
		for (int h=k;h<n;h++)
			if (p[l][h]) {
				i1 = l;
				i2 = h;
				return true;
			}
	return false;
}
bool peremestit(double** p,int n,int i,int k,int i1,int i2) {
	bool menyau_stroki = (i!=i1);
	if (menyau_stroki) {
		double* pd = p[i];
		p[i]=p[i1];
		p[i1]=pd;
	}
	bool menyau_stolbci = (k!=i2);
	if (menyau_stolbci)
		for (int l=0;l<n;l++) {
			double d = p[l][k];
			p[l][k] = p[l][i2];
			p[l][i2] = d;
		}
	return menyau_stroki!=menyau_stolbci; //true если надо менять знак
}
void pm(double **A,int N,int M) {
	for (int i=0;i<N;i++) {
		for (int j=0;j<M;j++) cout<<A[i][j]<<' ';
	cout<<endl;
	}
}
bool gauss(double **p,int n) {
	int i,j,k,m=n;
	double pik;
	int smena_znaka = 0;
	for (k=0;k<m-1;k++) {
		if (p[k][k]==0) {
			int i1,i2;
			if (!findNZ(p,n,k,k,i1,i2)) return smena_znaka%2;
			if (peremestit(p,n,k,k,i1,i2)) smena_znaka++;
			//cout<<endl;
			//pm(p,n,n);
		}
		for (i=k+1;i<m;i++) {
			//cout<<endl;
			//pm(p,n,n);
			pik=p[i][k];
			for (j=k;j<n;j++) 
				p[i][j]-=p[k][j]*(pik/p[k][k]);
		}
	}

	for (i=0;i<m;i++) 
		for (j=0;j<n;j++) 
			if (abs(p[i][j])<1e-10) p[i][j]=0;

	return smena_znaka%2; //true, если надо будет сменить знак у определителя
}
double power(double a, int b) {
	int i;
	double res=1;
	for (i=1;i<=b;i++) res*=a;
	return res;
}
double stepen(double x,int n) {
	return power(x,n);
}
double opr(double **A,int N) {
	double res=0;
	if (N==1) res=A[0][0];
	else {
		double **B=new double*[N-1];
//		for (int i=0;i<N-1;i++) B[i] = new int[N-1];

		for (int i=0;i<N;i++) {
			for (int j=0;j<i;j++) 
				B[j]=&(A[j][1]);
			for (int j=i+1;j<N;j++) 
				B[j-1]=&(A[j][1]);

			res+=A[i][0]*opr(B,N-1)*stepen(-1,i%2);
		}

//		for (int i=0;i<N-1;i++) delete []B[i];
		delete []B;
	}
	return res;
}

double matrix::determinant() {
	if (M!=N) throw "Error! Trying to calculate determinant of non-square matrix";
	if (M==0) return 1; //будем считать, что определитель нуль-мерной матрицы равен единице
	matrix temp(*this);
	double res;
	if (gauss(temp.p,temp.N)) res=-1; else res=1;
	//cout<<endl<<temp<<endl;
	for (int i=0;i<N;i++)
		res*=temp[i][i];
	return res;
//	return opr(p,N);
}

void matrix::transp() {
	if (M!=N) throw "Error! Trying to transpose non-square matrix";
	for (int i=0;i<M;i++) 
		for (int j=i+1;j<N;j++)
			swap(p[i][j],p[j][i]);
}
bool matrix::is_square() const {
	return M==N;
}

//vector matrix::operator * (const vector& v) {
//	if (N!=v.N) throw "Error: vector*matrix: invalid dimensionalities";
//	vector S(M);
//	for (int i=0;i<M;i++) {
//		S[i]=0;
//		for (int j=0;j<N;j++)
//			S[i] += v.p[j]*p[i][j];
//	}
//	return S;
//}