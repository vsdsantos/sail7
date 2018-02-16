/****************************************************************************

		 CubicSpline Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#ifndef BEZIERSPLINE_H
#define BEZIERSPLINE_H
#include "../params.h"
#include "Spline.h"
#include <QTextStream>


class BezierSpline : public Spline
{
public:
	BezierSpline();

	double BezierBlend(int const &k, int const &n, double const&u);
	double Bernstein(int const &i, int const &n, double const &u);

	void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void Duplicate(void *pSpline);
	void GetCamber(double &Camber, double &xc);
	double GetY(double const &x);
	void GetSlopes(double &s0, double &s1);
	bool SplineCurve();
	bool Serialize(QDataStream &ar, bool bIsStoring);
	Vector3d GetNormal(double const &x);

/*	void SplineKnots();

	void DrawOutputPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);


	double SplineBlend(int const &i, int const &p, double const &t);

	void Export(QTextStream &out, bool bExtrados);
*/
	Vector3d m_Output[SPLINEOUTPUTRES];

};

#endif // BEZIERSPLINE_H
