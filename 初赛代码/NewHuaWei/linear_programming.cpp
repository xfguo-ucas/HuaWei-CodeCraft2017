
#include <stdio.h>
#include <math.h>
#include<iostream>
#include<vector>
#include"linear_programming.h"
#include "Graph.h"
using namespace std;



Matrix  A;
int  IPOSV[MMAX], IZROV[NMAX];
int  i,j,ICASE,N,M,M1,M2,M3;
R_type R;


void danchunxing1(Matrix,int,int *,int,int,int *,R_type *);
void danchunxing2(Matrix,int,int,int *,int,int *,int,R_type *);
void danchunxing3(Matrix,int,int,int,int);


void danchunxing(Matrix a,int m,int n,int m1,int m2,int m3,int *icase,int *izrov, int *iposv)  {

    int i,ip,ir,is,k,kh,kp,m12,nl1,nl2,l1[NMAX],l2[MMAX],l3[MMAX];
    R_type bmax,q1,EPS=1e-6;
    if(m != m1+m2+m3) {
        //printf(" Bad input constraint counts in danchunxing.\n");
        return;
    }
    nl1=n;
    for (k=1; k<=n; k++) {
        l1[k]=k;     //Initialize index list of columns admissible for exchange.
        izrov[k]=k;  //Initially make all variables right-hand.
    }
    nl2=m;
    for (i=1; i<=m; i++) {
        if (a[i+1][1] < 0.0) {
            //printf(" Bad input tableau in danchunxing, Constants bi must be nonnegative.\n");
            return;
        }
        l2[i]=i;
        iposv[i]=n+i;

    }
    for (i=1; i<=m2; i++) l3[i]=1;
    ir=0;
    if(m2+m3 == 0) goto e30;
    ir=1;
    for (k=1; k<=n+1; k++) {
        q1=0.0;
        for (i=m1+1; i<=m; i++) q1 += a[i+1][k];
        a[m+2][k]=-q1;
    }
e10: danchunxing1(a,m+1,l1,nl1,0,&kp,&bmax);
    if(bmax <= EPS && a[m+2][1] < -EPS) {
        *icase=-1;
        return;
    }
    else if (bmax <= EPS && a[m+2][1] <= EPS) {

        m12=m1+m2+1;
        if (m12 <= m)
            for (ip=m12; ip<=m; ip++)
                if(iposv[ip] == ip+n) {
                    danchunxing1(a,ip,l1,nl1,1,&kp,&bmax);
                    if(bmax > EPS) goto e1;
                }
        ir=0;
        m12=m12-1;
        if (m1+1 > m12) goto e30;
        for (i=m1+1; i<=m1+m2; i++)
            if(l3[i-m1] == 1)
                for (k=1; k<=n+1; k++)
                    a[i+1][k] *= -1.0;
        goto e30;
    }
    
    danchunxing2(a,m,n,l2,nl2,&ip,kp,&q1);
    
    if(ip == 0) {
        *icase=-1;
        return;
    }
e1: danchunxing3(a,m+1,n,ip,kp);

    if(iposv[ip] >= n+m1+m2+1) {

        for (k=1; k<=nl1; k++)
            if(l1[k] == kp) goto e2;
    e2: nl1=nl1-1;
        for (is=k; is<=nl1; is++)  l1[is]=l1[is+1];
    }
    else {
        if(iposv[ip] < n+m1+1) goto e20;
        kh=iposv[ip]-m1-n;
        if(l3[kh] == 0) goto e20;
        l3[kh]=0;

    }
    a[m+2][kp+1] += 1.0;
    for (i=1; i<=m+2; i++)  a[i][kp+1] *= -1.0;
e20: is=izrov[kp];
    izrov[kp]=iposv[ip];
    iposv[ip]=is;
    if (ir != 0) goto e10;

e30: danchunxing1(a,0,l1,nl1,0,&kp,&bmax);
    if(bmax <= EPS) {
        *icase=0;
        return;
    }
    danchunxing2(a,m,n,l2,nl2,&ip,kp,&q1);
    if(ip == 0) {
        *icase=1;
        return;
    }
    danchunxing3(a,m,n,ip,kp);
    goto e20;
}



void danchunxing1(Matrix a,int mm,int *ll,int nll,int iabf,int *kp,R_type *bmax) {

    int k;
    R_type test;
    *kp=ll[1];
    *bmax=a[mm+1][*kp+1];
    if (nll < 2) return;
    for (k=2; k<=nll; k++) {
        if(iabf == 0)
            test=a[mm+1][ll[k]+1]-(*bmax);
        else
            test=fabs(a[mm+1][ll[k]+1])-fabs(*bmax);
        if(test > 0.0) {
            *bmax=a[mm+1][ll[k]+1];
            *kp=ll[k];
        }
    }
    return;
}

void danchunxing2(Matrix a, int m,int n,int *l2,int nl2,int *ip,int kp,R_type *q1) {
    R_type EPS=1e-6;

    int i,ii,k;
    R_type q,q0,qp;
    *ip=0;
    if(nl2 < 1) return;
    for (i=1; i<=nl2; i++)
        if (a[i+1][kp+1] < -EPS) goto e2;
    return;
e2: *q1=-a[l2[i]+1][1]/a[l2[i]+1][kp+1];
    *ip=l2[i];
    if (i+1 > nl2) return;
    for (i=i+1; i<=nl2; i++) {
        ii=l2[i];
        if(a[ii+1][kp+1] < -EPS) {
            q=-a[ii+1][1]/a[ii+1][kp+1];
            if (q <  *q1) {
                *ip=ii;
                *q1=q;
            }
            else if (q == *q1) {
                for (k=1; k<=n; k++) {
                    qp=-a[*ip+1][k+1]/a[*ip+1][kp+1];
                    q0=-a[ii+1][k+1]/a[ii+1][kp+1];
                    if (q0 != qp) goto e6;
                }
            e6:     if (q0 < qp) *ip=ii;
            }
        }
    }
    return;
}

void danchunxing3(Matrix a,int i1,int k1,int ip,int kp) {

    int ii,kk;
    R_type piv;
    piv=1.0/a[ip+1][kp+1];
    if (i1 >= 0)
        for (ii=1; ii<=i1+1; ii++)
            if (ii-1 != ip) {
                a[ii][kp+1] *= piv;
                for (kk=1; kk<=k1+1; kk++)
                    if (kk-1 != kp)
                        a[ii][kk] -= a[ip+1][kk]*a[ii][kp+1];
            }
    for (kk=1; kk<=k1+1; kk++)
        if(kk-1 !=  kp) a[ip+1][kk] =-a[ip+1][kk]*piv;
    a[ip+1][kp+1]=piv;
    return;
}


vector<float>   linear_programming(vector<vector<float> > &INPUT){
    // cout << "Call Me!" << endl;
    //0代表<=,1代表=; 2代表 >=
    //INPUT 前面试等式，后面是不等式；
    
    vector<float> result;
    int i,m=0;
    N=INPUT[0].size()-2;    // 决策变量个数
    M=INPUT.size()-1;   // 约束条件个数
    
//    for(i=1;i<=M;i++)
//        if(INPUT[i][N]==0)
//            m++;
    
    M1=N;//<=约束个数
    M2=0;
    M3=M-M1;//=约束个数
    
    // A 是单纯形表
    //----------------表的排布形式-------------
    //      0   1   2   3 ------------  N   N+1             表的第2列至第N+1列表示决策变量对应的系数
    //                                                      第0列空出来，第1列是符号位（求最大或最小, 小于或大于或等于）
    //  0                                                   第0行表示目标函数，
    
    //  1                                                   第1行至第M1行表示小于等于的约束
    
    //  2                                                   第M1行至最后表示小于等于的约束

    
    //  3
    //  |
    //  |
    //  |
    //  |
    //  |
    //  |

    for (i=2; i<=N+1; i++)
        A[1][i]=-INPUT[0][i-2];


    A[1][1]=0;
    for (i=1; i<=M; i++) {

        for (j=2; j<=N+1; j++) {
            A[i+1][j]=-INPUT[M-i+1][j-2];   // 把约束放到单纯形表中，符号是什么意思？

        }
        A[i+1][1]=INPUT[M-i+1][N+1];    // 设置符号位

    }

    danchunxing(A,M,N,M1,M2,M3,&ICASE,IZROV,IPOSV);

    if (ICASE==0) {
        result.push_back(1);


        for (i=1; i<=N; i++) {
            for (j=1; j<=M; j++)
                if (IPOSV[j] == i)  {

                    result.push_back(A[j+1][1]);
                    goto e3;
                }

            result.push_back(0);
            e3:;}
        result.push_back(-A[1][1]);
    }
    else{

        result.push_back(0);
    }


    return result;
    
}


