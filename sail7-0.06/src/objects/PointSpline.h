/****************************************************************************

		 PointSpline Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#ifndef POINTSPLINE_H
#define POINTSPLINE_H

#include "Spline.h"

class PointSpline : public Spline
{
public:
	PointSpline();


	void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void GetCamber(double &Camber, double &xc);
	double GetY(double const &x);
	Vector3d GetNormal(double const &x);

	void GetSlopes(double &s0, double &s1);
	void Duplicate(void *pSpline);
	bool Serialize(QDataStream &ar, bool bIsStoring);
};

#endif // POINTSPLINE_H
