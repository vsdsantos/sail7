/****************************************************************************

		 CubicSpline Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include "../params.h"
#include "../globals.h"
#include "CubicSpline.h"
#include <QtDebug>


CubicSpline::CubicSpline()
{
	m_SplineType  = CUBICSPLINE;
	m_SplineColor = QColor(155,200,50);

	m_bShowSlope = true;
	m_iMinPoints = 2;
	m_iMaxPoints = 50;

	s0 =  1.0;
	s1 = -1.0/2.0;

	m_CtrlPoint.clear();
	m_CtrlPoint.insert(0,CVector(0.0,0.0,0.0));
	m_CtrlPoint.append(CVector(0.3, 0.15, 0.0));
	m_CtrlPoint.append(CVector(1.0,0.0,0.0));
	SplineCurve();

}



bool CubicSpline::SplineCurve()
{
//
// Given an array of n+1 pairs (x[i], y[i]), with i ranging from 0 to n,
// this function calculates the 3rd order cubic spline which interpolate the pairs.
//
// The spline is defined for each interval [x[j], x[j+1]) by n third order polynomial functions
//              p_j(x) = ax3 + bx2 + cx + d
//
// The equations to determine the coefficients a,b,c,d are
//
// Interpolation : 2n conditions
//    p_j(x[j])   = y[j];
//    p_j(x[j+1]) = y[j+1];
//
// Continuity of 1st and 2nd order derivatives at internal points: 2(n-1) conditions
//    p_j'(x[j]) = p_j+1'(x[j])
//    p_j"(x[j]) = p_j+1"(x[j])
//
// Second order derivative is zero at the end points : 2 conditions
//    p_j"(x[0]) =  p_j"(x[n]) =0
//    or set the slope at eact end point : 2 conditions
//    p_j'(x[0])=s         p_j'(x[n])=t
//
//
// This sets a linear system of size 4n which is solved by the Gauss algorithm for coefs a,b,c and d
// The RHS vector is
//	  a[0]
//	  b[0]
//	  c[0]
//	  d[0]
//	  a[1]
//    ...
//	  d[n-1]
//

	int i,size;
	int n = m_CtrlPoint.size()-1;

	s0 = m_SlopePoint[0].y/m_SlopePoint[0].x;
	s1 = (m_SlopePoint[1].y-m_CtrlPoint.last().y)/(m_SlopePoint[1].x-m_CtrlPoint.last().x);

	memset(M,   0, 16*n*n*sizeof(double));
	memset(RHS, 0,  4*n*sizeof(double));

	size = 4*(m_CtrlPoint.size()-1);

//	Interpolation conditions
	for (i=0; i<n; i++)
	{
		//pj(x[i]) = y[i]
		M[2*i*size +4*i]     = m_CtrlPoint[i].x*m_CtrlPoint[i].x*m_CtrlPoint[i].x;
		M[2*i*size +4*i + 1] = m_CtrlPoint[i].x*m_CtrlPoint[i].x;
		M[2*i*size +4*i + 2] = m_CtrlPoint[i].x;
		M[2*i*size +4*i + 3] = 1.0;

		//pj(x[i+1]) = y[i+1]
		M[(2*i+1)*size +4*i]     = m_CtrlPoint[i+1].x*m_CtrlPoint[i+1].x*m_CtrlPoint[i+1].x;
		M[(2*i+1)*size +4*i + 1] = m_CtrlPoint[i+1].x*m_CtrlPoint[i+1].x;
		M[(2*i+1)*size +4*i + 2] = m_CtrlPoint[i+1].x;
		M[(2*i+1)*size +4*i + 3] = 1.0;

		RHS[2*i]   = m_CtrlPoint[i].y;
		RHS[2*i+1] = m_CtrlPoint[i+1].y;
	}

//  Derivation conditions
	for (i=1; i<n; i++)
	{
		//continuity of 1st order derivatives

		M[(2*n+i)*size + 4*(i-1)]     =  3.0*m_CtrlPoint[i].x*m_CtrlPoint[i].x;
		M[(2*n+i)*size + 4*(i-1)+1]   =  2.0*m_CtrlPoint[i].x;
		M[(2*n+i)*size + 4*(i-1)+2]   =  1.0;

		M[(2*n+i)*size + 4*i]   = -3.0*m_CtrlPoint[i].x*m_CtrlPoint[i].x;
		M[(2*n+i)*size + 4*i+1] = -2.0*m_CtrlPoint[i].x;
		M[(2*n+i)*size + 4*i+2] = -1.0;

		RHS[2*n+i]   = 0.0;

		//continuity of 2nd order derivatives
		M[(3*n+i)*size + 4*(i-1)]     =  6.0*m_CtrlPoint[i].x;
		M[(3*n+i)*size + 4*(i-1)+1]   =  2.0 ;

		M[(3*n+i)*size + 4*i]   = -6.0*m_CtrlPoint[i].x;
		M[(3*n+i)*size + 4*i+1] = -2.0;

		RHS[3*n+i]   = 0.0;

/*		//mid derivative is zero
		M[(2*n+i)*size + 4*(i-1)]     =  3.0*m_CtrlPoint[i].x*m_CtrlPoint[i].x;
		M[(2*n+i)*size + 4*(i-1)+1]   =  2.0*m_CtrlPoint[i].x;
		M[(2*n+i)*size + 4*(i-1)+2]   =  1.0;
		RHS[2*n+i]   = 0.0;

		M[(3*n+i)*size + 4*i]   = -3.0*m_CtrlPoint[i].x*m_CtrlPoint[i].x;
		M[(3*n+i)*size + 4*i+1] = -2.0*m_CtrlPoint[i].x;
		M[(3*n+i)*size + 4*i+2] = -1.0;*/

		RHS[3*n+i]   = 0.0;

	}

//	second order derivative is zero at end points = "natural spline"
/*	M[2*n*size]     = 6.0*m_CtrlPoint[0].x;
	M[2*n*size+1]   = 2.0;
	RHS[2*n]        = 0.0;

	M[3*n*size + size-4]   = 6.0*m_CtrlPoint[n].x;
	M[3*n*size + size-3]   = 2.0;
	RHS[3*n+1]             = 0.0;*/
//	first order derivative is set to predefined slope value at end points;
	M[2*n*size]     = 3.0*m_CtrlPoint[0].x*m_CtrlPoint[0].x;
	M[2*n*size+1]   = 2.0*m_CtrlPoint[0].x;
	M[2*n*size+2]   = 1.0;
	RHS[2*n]        = s0;

	M[3*n*size + size-4]   = 3.0*m_CtrlPoint[n].x*m_CtrlPoint[n].x;
	M[3*n*size + size-3]   = 2.0*m_CtrlPoint[n].x;
	M[3*n*size + size-2]   = 1.0;
	RHS[3*n]        = s1;



	bool bCancel = false;
	if(!Gauss(M, 4*n, RHS, 1, &bCancel)) return false;

	for(i=0; i<n; i++)
	{
		a[i] = RHS[4*i];
		b[i] = RHS[4*i+1];
		c[i] = RHS[4*i+2];
		d[i] = RHS[4*i+3];
	}

	return true;
}


void CubicSpline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	double x, y;
	static QPoint From, To;

	for(int i=0; i<m_CtrlPoint.size()-1; i++)
	{
		x = m_CtrlPoint[i].x;
		y = d[i] + c[i]*x + b[i]*x*x + a[i]*x*x*x;
		From = QPoint((int)(x*scalex) + Offset.x(), (int)(-y*scaley) + Offset.y());
		for(int j=0; j<=20; j++)
		{
			x = m_CtrlPoint[i].x + (double)j/20.0*(m_CtrlPoint[i+1].x-m_CtrlPoint[i].x);
			y = d[i] + c[i]*x + b[i]*x*x + a[i]*x*x*x;
			To   = QPoint((int)(x*scalex) + Offset.x(), (int)(-y*scaley) + Offset.y());
			painter.drawLine(From, To);
			From = To;
		}
	}

	painter.restore();
}



void CubicSpline::GetSlopes(double &s0, double &s1)
{
/*	s0 = atan(c[0])*180/PI;
	int i=m_CtrlPoint.size()-2;
	s1 = atan(c[i]+b[i]+a[i])*180.0/PI;*/
	s0 = atan2(m_SlopePoint[0].y, m_SlopePoint[0].x)      * 180/PI;
	s1 = atan2(m_SlopePoint[1].y, (1.-m_SlopePoint[1].x)) * 180/PI;

}



double CubicSpline::GetY(double const &x)
{
	if(x<m_CtrlPoint.at(0).x) return 0.0;
	for(int i=0; i<m_CtrlPoint.size()-1; i++)
	{
		if(m_CtrlPoint.at(i).x<=x && x<m_CtrlPoint.at(i+1).x)
		{
			//interpolate
			return d[i] + c[i]*x + b[i]*x*x + a[i]*x*x*x;
		}
	}
	return 0.0;
}



void CubicSpline::GetCamber(double &Camber, double &xc)
{
	double x, camb;
	xc = x = Camber = camb = 0.0;



	for(int i=0; i<m_CtrlPoint.size()-1; i++)
	{
		for(int j=0; j<=20; j++)
		{
			x = m_CtrlPoint[i].x + (double)j/20.0*(m_CtrlPoint[i+1].x-m_CtrlPoint[i].x);
			camb = d[i] + c[i]*x + b[i]*x*x + a[i]*x*x*x;
			if(fabs(camb)>fabs(Camber))
			{
				xc=x;
				Camber = camb;
			}
			//		else return;

		}
	}

}


void CubicSpline::Duplicate(void *pSpline)
{
	if(!pSpline) return;
	CubicSpline *pCubicSpline = (CubicSpline*)pSpline;
	if(!pCubicSpline->IsSplineType(CUBICSPLINE)) return;

	m_SplineType  = pCubicSpline->m_SplineType;
	m_SplineColor = pCubicSpline->m_SplineColor;

	m_CtrlPoint.clear();
	for(int i=0; i<pCubicSpline->m_CtrlPoint.size(); i++)
	{
		m_CtrlPoint.append(pCubicSpline->m_CtrlPoint.at(i));
	}

	memcpy(a, pCubicSpline->a, MAXCTRLPOINTS*sizeof(double));
	memcpy(b, pCubicSpline->b, MAXCTRLPOINTS*sizeof(double));
	memcpy(c, pCubicSpline->c, MAXCTRLPOINTS*sizeof(double));
	memcpy(d, pCubicSpline->d, MAXCTRLPOINTS*sizeof(double));

	s0 = pCubicSpline->s0;
	s1 = pCubicSpline->s1;
}



bool CubicSpline::Serialize(QDataStream &ar, bool bIsStoring)
{
	if(bIsStoring)
	{
		ar << m_CtrlPoint.size();
		for(int ic=0; ic<m_CtrlPoint.size();ic++)
		{
			ar << m_CtrlPoint.at(ic).x << m_CtrlPoint.at(ic).y;
		}
		ar << s0 << s1;
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
			m_CtrlPoint.append(CVector(x,y,0.0));
		}

		ar >> s0 >> s1;
		SplineCurve();
	}
	return true;
}




CVector CubicSpline::GetNormal(double const &x)
{
	return CVector(0.0, 1.0, 0.0);
}






