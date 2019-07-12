/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
	Copyright (C) 2003 Andre Deperrois sail7@xflr5.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#ifndef BSPLINE_H
#define BSPLINE_H
#include <QPainter>
#include <QRect>
#include <QFile>
#include <QTextStream>
#include "./Spline.h"
#include "./vector3d.h"
#include "../params.h"

class BSpline : public Spline
{
public:

	BSpline();

	void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void Duplicate(void *pSpline);
	void SplineKnots();
	bool SplineCurve();
	void GetCamber(double &Camber, double &xc);
	double GetY(double const &x);
	void GetSlopes(double &s0, double &s1);
	bool Serialize(QDataStream &ar, bool bIsStoring);
	Vector3d GetNormal(double const &x);

	void DrawOutputPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);


	double SplineBlend(int const &i, int const &p, double const &t);

	void CopySymetric(BSpline *pSpline);
	void Export(QTextStream &out, bool bExtrados);
	

	int m_iKnots;
	int m_iDegree;
	double m_knots[SPLINECONTROLSIZE];
	Vector3d m_Output[SPLINEOUTPUTRES*2];


};


#endif
