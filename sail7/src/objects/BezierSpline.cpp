/****************************************************************************

		 CubicSpline Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/


#include "BezierSpline.h"
#include "../globals.h"
#include <QtDebug>


BezierSpline::BezierSpline()
{
	m_SplineType = BEZIERSPLINE;

	m_Style = 0;
	m_Width = 1;
	m_SplineColor = QColor(75,65,170);
	m_iMinPoints = 3;
	m_iMaxPoints = 50;

	m_CtrlPoint.clear();
	InsertPoint(0.0,0.0);
	InsertPoint(0.2,0.15);
	InsertPoint(0.8,0.11);
	InsertPoint(1.0,0.0);
	SplineCurve();
}


void BezierSpline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();
	static QPoint From, To;

	From = QPoint((int)(m_Output[0].x*scalex) + Offset.x(), (int)(-m_Output[0].y*scalex) + Offset.y());

	for(int j=0; j<SPLINEOUTPUTRES; j++)
	{
		To   = QPoint((int)(m_Output[j].x*scalex) + Offset.x(), (int)(-m_Output[j].y*scalex) + Offset.y());
		painter.drawLine(From, To);
		From = To;
	}
	painter.restore();
}


bool BezierSpline::SplineCurve()
{
	double b, t;
	for(int j=0; j<SPLINEOUTPUTRES; j++)
	{
		t = (double)j/(double)(SPLINEOUTPUTRES-1);
		m_Output[j].Set(0.0,0.0,0.0);
		for(int ic=0; ic<m_CtrlPoint.size(); ic++)
		{
			b = Bernstein(ic, m_CtrlPoint.size()-1, t);
			m_Output[j].x +=  b *m_CtrlPoint.at(ic).x;
			m_Output[j].y +=  b *m_CtrlPoint.at(ic).y;
		}
	}
    return true;
}


double BezierSpline::Bernstein(int const &i, int const &n, double const &u)
{
    double pui, pu1i1;

    int fi  = 1;
    int fni = 1;

    for(int k=2;     k<=i; k++) fi  *=k;
    for(int k=n-i+1; k<=n; k++) fni *=k;

	pui   = 1.0;
    for(int k=0; k<i;   k++) pui*=u;

	pu1i1 = 1.0;
    for(int k=0; k<n-i; k++) pu1i1*=1.0-u;

	return pui * pu1i1 * (double)fni/(double)fi;
}


double BezierSpline::BezierBlend(int const &k, int const &n, double const&u)
{
	static int nn,kn,nkn;
	double blend=1.0;

	nn = n;
	kn = k;
	nkn = n - k;

	while (nn >= 1)
	{
		blend *= nn;
		nn--;
		if (kn > 1)
		{
			blend /= (double)kn;
			kn--;
		}
		if (nkn > 1)
		{
			blend /= (double)nkn;
			nkn--;
		}
	}

	if (k > 0)   for(kn=0; kn<k;   kn++) blend *= u;
	if (n-k > 0) for(kn=0; kn<n-k; kn++) blend *= 1.0-u;

	return(blend);
}


void BezierSpline::Duplicate(void *pSpline)
{
	BezierSpline *pBezierSpline = (BezierSpline*)pSpline;
	if(!pBezierSpline->IsSplineType(BEZIERSPLINE)) return;

	m_SplineType  = pBezierSpline->m_SplineType;
	m_SplineColor = pBezierSpline->m_SplineColor;

	m_CtrlPoint.clear();
	for(int ic=0; ic<pBezierSpline->m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint.append(pBezierSpline->m_CtrlPoint.at(ic));
	}
	memcpy(m_Output, pBezierSpline->m_Output, SPLINEOUTPUTRES * sizeof(Vector3d));
}


double BezierSpline::GetY(double const &x)
{
	static int i;
	static double y;

	if(x<=0.0 || x>=1.0) return 0.0;

	for (i=0; i<SPLINEOUTPUTRES-1; i++)
	{
		if (m_Output[i].x <m_Output[i+1].x  &&   m_Output[i].x <= x && x<=m_Output[i+1].x )
		{
			y = (m_Output[i].y 	+ (m_Output[i+1].y-m_Output[i].y)
								 /(m_Output[i+1].x-m_Output[i].x)*(x-m_Output[i].x));
			return y;
		}
	}
	return 0.0; //we'll have to do better...
}


Vector3d BezierSpline::GetNormal(double const &x)
{
	double dx, dy;
	Vector3d Normal;

	if(x<=0.0 || x>=1.0) return Vector3d(0.0, 1.0, 0.0);

	for (int i=0; i<SPLINEOUTPUTRES-1; i++)
	{
		if (m_Output[i].x <m_Output[i+1].x  &&   m_Output[i].x <= x && x<=m_Output[i+1].x )
		{
			dx = m_Output[i+1].x - m_Output[i].x;
			dy = m_Output[i+1].y - m_Output[i].y;
			Normal.Set(-dy, dx, 0.0);
			Normal.Normalize();
			return Normal;
		}
	}
	return Vector3d(0.0, 1.0, 0.0);
}


void BezierSpline::GetCamber(double &Camber, double &xc)
{
	Camber = 0.0;
	xc = 0.0;
	for(int i=0; i<SPLINEOUTPUTRES; i++)
	{
		if(fabs(m_Output[i].y)>fabs(Camber))
		{
			xc = m_Output[i].x;
			Camber = m_Output[i].y;
		}
	}
}


bool BezierSpline::Serialize(QDataStream &ar, bool bIsStoring)
{
	if(bIsStoring)
	{
		ar << m_CtrlPoint.size();
		for(int ic=0; ic<m_CtrlPoint.size();ic++)
		{
			ar << m_CtrlPoint.at(ic).x << m_CtrlPoint.at(ic).y;
		}
	}
	else
	{
		int n;
		double x,y;
		m_CtrlPoint.clear();
		ar >> n;
		for(int ic=0; ic<n;ic++)
		{
			ar >> x >> y;
			m_CtrlPoint.append(Vector3d(x,y,0.0));
		}
		SplineCurve();
	}
	return true;
}



void BezierSpline::GetSlopes(double &s0, double &s1)
{
	s0 = atan2(m_CtrlPoint[1].y, m_CtrlPoint[1].x)      * 180/PI;
	int i=m_CtrlPoint.size()-2;
	s1 = atan2(m_CtrlPoint[i].y, (1.-m_CtrlPoint[i].x)) * 180/PI;
}






