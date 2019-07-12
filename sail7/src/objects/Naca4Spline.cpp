/****************************************************************************

		 Naca4 Spline Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/

//Spline based on the mid line of the NACA 4 digit airfoil series

#include "Naca4Spline.h"
#include "../globals.h"
#include "../params.h"


Naca4Spline::Naca4Spline()
{
	m_SplineType  = NACA4SPLINE;

	m_Style = 0;
	m_Width = 1;
	m_SplineColor = QColor(187, 100, 34);
	p=.3;
	m = .1;
	m_iMinPoints = 3;
	m_iMaxPoints = 3;
	m_CtrlPoint.clear();
	InsertPoint(0.0,0.0);
	InsertPoint(0.3,0.1);
	InsertPoint(1.0,0.0);
	SplineCurve();
}


void Naca4Spline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();
	static QPoint From, To;

	double x, y;

	if(m_CtrlPoint.size()<3) return;
	p = m_CtrlPoint.at(1).x;
	m = m_CtrlPoint.at(1).y;

	From = QPoint((int)(0.0*scalex) + Offset.x(), (int)(-0.0*scaley) + Offset.y());

	for(int j=0; j<=100; j++)
	{
		x = (double)j/100;

		if(x<p) y = m/p/p* (2.0*p*x - x*x);
		else    y = m/(1.0-p)/(1.0-p) * (1.0-2.0*p + 2.0*p*x-x*x);

		To   = QPoint((int)(x*scalex) + Offset.x(), (int)(-y*scaley) + Offset.y());
		painter.drawLine(From, To);
		From = To;
	}
	painter.restore();
}


void Naca4Spline::GetSlopes(double &s0, double &s1)
{
	s0 = atan(m/p/p * 2.0*p)*180/PI;
	s1 = atan(m/(1.0-p)/(1.0-p)  * (2.0*p-2))*180/PI;
}


void Naca4Spline::Duplicate(void *pSpline)
{
	Naca4Spline * pN4Spline =(Naca4Spline*)pSpline;
	if(!pN4Spline->IsSplineType(NACA4SPLINE)) return;

	m_SplineType  = pN4Spline->m_SplineType;
	m_SplineColor = pN4Spline->m_SplineColor;


	m_CtrlPoint.clear();
	for(int ic=0; ic<pN4Spline->m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint.append(pN4Spline->m_CtrlPoint.at(ic));
	}

	m = pN4Spline->m;
	p = pN4Spline->p;
}


double Naca4Spline::GetY(double const &x)
{
	double y;

	if(x<p) y = m/p/p* (2.0*p*x - x*x);
	else    y = m/(1.0-p)/(1.0-p) * (1.0-2.0*p + 2.0*p*x-x*x);
	return y;
}



void Naca4Spline::GetCamber(double &Camber, double &xc)
{
	Camber = m;
	xc = p;
}


bool Naca4Spline::Serialize(QDataStream &ar, bool bIsStoring)
{
	if(bIsStoring)
	{
		ar << m << p;
	}
	else
	{
		ar >> m >> p;
	}
	SplineCurve();
	return true;
}




Vector3d Naca4Spline::GetNormal(double const &x)
{
	return Vector3d(0.0, 1.0, 0.0);
}

