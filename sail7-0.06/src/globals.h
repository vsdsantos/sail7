/****************************************************************************

	Global Functions 
	Copyright (C) 2008-2010 Andre Deperrois sail7@xflr5.com

	All rights reserved

*****************************************************************************/


#ifndef GLOBALS_H
#define GLOBALS_H

//#include <Qt> 
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QColor>
#include <QList>
#include <complex>

#include "objects/CVector.h"

using namespace std;

bool IsEven(int n);
bool IsBetween(int f, int f1, int f2);
bool IsBetween(int f, double f1, double f2);
bool ReadAVLString(QTextStream &in, int &Line, QString &strong);
bool Rewind1Line(QTextStream &in, int &Line, QString &strong);

void ReadValues(QString line, int &res, double &x, double &y, double &z);

void ExpFormat(double &f, int &exp);
void ReynoldsFormat(QString &str, double f);

bool Gauss(double *A, int n, double *B, int m, bool *pbCancel);

void GetSpeedUnit(QString &str, int unit);
void GetWeightUnit(QString &str, int unit);
void GetAreaUnit(QString &str, int unit);
void GetLengthUnit(QString &str, int unit);
void GetMomentUnit(QString &str, int unit);
void GetForceUnit(QString &str, int unit);

double GLGetRed(double tau);
double GLGetGreen(double tau);
double GLGetBlue(double tau);

double IntegralC2(double y1, double y2, double c1, double c2);
double IntegralCy(double y1, double y2, double c1, double c2);

double round(double d, int n);

void ReadCString(QDataStream &ar, QString &strong);
void WriteCString(QDataStream &ar, QString const &strong);
void ReadCOLORREF(QDataStream &ar, QColor &color);
void WriteCOLORREF(QDataStream &ar, QColor const &color);

void Trace(int n);
void Trace(QString msg);
void Trace(QString msg, int n);
void Trace(QString msg, double f);

void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit,
			  double &mtoUnit, double &m2toUnit, double &mstoUnit,  double &kgtoUnit,
			  double &NtoUnit, double &NmtoUnit);

Qt::PenStyle GetStyle(int s);


double Det33(double *aij);
double Det44(double *aij);
double Cofactor44(double *aij, int &i, int &j);
bool Invert44(double *ain, double *aout);
bool Invert22(double*ain, double *aout);

void TestEigen();
void CharacteristicPol(double m[][4], double p[5]);
bool LinBairstow(double *p, complex<double> *root, int n);
bool Eigenvector(double a[][4], complex<double> lambda, complex<double> *V);


bool Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n, bool *pbCancel, double TaskSize, double &Progress);
bool Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[], double x[], int n, bool *pbCancel);


bool SplineInterpolation(int n, double *x, double *y,  double *a, double *b, double *c, double *d);
double GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d);

void VLMCmn(CVector const &A, CVector const &B, CVector const &WindDirection, CVector const &C, CVector &V, bool const &bAll=true, double const &CoreSize = 0.0001);
void VLMQmn(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &C, CVector &V, double const &CoreSize = 0.0001);

bool Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal, CVector const &A,  CVector const &U,  CVector &I, double &dist);

double SplineBlend(int const &index, int const &degree, double const &t, double *knots);

void SetWindAxis(double const Beta, CVector &WindDirection, CVector & WindNormal, CVector &WindSide);


#endif // FUNCTIONS_H
