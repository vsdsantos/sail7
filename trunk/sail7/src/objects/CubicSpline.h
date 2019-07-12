/****************************************************************************

		 CubicSpline Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#ifndef CUBICSPLINE_H
#define CUBICSPLINE_H

#define MAXCTRLPOINTS 30

#include <QPainter>
#include <QPoint>
#include "Spline.h"
#include "./vector3d.h"

// This class defines sections of a sail
// A section is deined by a cubic spline
// The spline projected n the x-axis has unit lenght
// The spline is fully defined by the intermediate points and the boundary conditions
// In a first step we use the standard f"(x)=0 BC at end points


class CubicSpline : public Spline
{
public:
	CubicSpline();

	void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	bool SplineCurve();
	void GetCamber(double &Camber, double &xc);
	double GetY(double const &x);
	void GetSlopes(double &s0, double &s1);
	void Duplicate(void *pSpline);
	bool Serialize(QDataStream &ar, bool bIsStoring);
	Vector3d GetNormal(double const &x);

	//variables
	double a[MAXCTRLPOINTS], b[MAXCTRLPOINTS], c[MAXCTRLPOINTS], d[MAXCTRLPOINTS];
	double M[16*MAXCTRLPOINTS*MAXCTRLPOINTS];// size is 4 coefs x maxctrlpoints
	double RHS[4*MAXCTRLPOINTS*MAXCTRLPOINTS];

	double s0, s1; //slopes at each end point s0>0 and s1<0
};

#endif // SECTION_H
