//#ifndef LINEAR_PROGRAMMING_H
//#define LINEAR_PROGRAMMING_H

//#endif // LINEAR_PROGRAMMING_H

#include <stdio.h>
#include <vector>
#include "Graph.h"
using namespace::std;

#define  MMAX  10000
#define  NMAX  10000
#define  R_type  float

typedef R_type Matrix[MMAX][NMAX];


void danchunxing1(Matrix,int,int *,int,int,int *,R_type *);
void danchunxing2(Matrix,int,int,int *,int,int *,int,R_type *);
void danchunxing3(Matrix,int,int,int,int);


void danchunxing(Matrix a,int m,int n,int m1,int m2,int m3,int *icase,int *izrov, int *iposv);
vector<float>   linear_programming(vector<vector<float> > &INPUT);



