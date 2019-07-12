/****************************************************************************

		 Arc Spline Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/


#ifndef ARCSPLINE_H
#define ARCSPLINE_H

#include "BSpline.h"



class ArcSpline : public Spline
{
public:
	ArcSpline();

	void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void Duplicate(void *pSpline);
	void GetCamber(double &Camber, double &xc);
	void GetSlopes(double &s0, double &s1);

	double GetY(double const &x);
	bool Serialize(QDataStream &ar, bool bIsStoring);

	Vector3d GetNormal(double const &x);


	bool SplineCurve();
	BSpline m_Spline1, m_Spline2;

};

#endif // ARCSPLINE_H
