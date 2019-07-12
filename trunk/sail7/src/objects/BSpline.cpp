/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
	Copyright (C) 2003 Andre Deperrois 

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

#include "../globals.h"
#include "BSpline.h"
#include "math.h"


#include <QtDebug>

BSpline::BSpline()
{
	m_SplineType  = BSPLINE;

	m_Style = 0;
	m_Width = 1;
	m_SplineColor = QColor(15,255,90);
	m_iKnots      =  0;
	m_iDegree     =  2;

	m_iMinPoints = 3;
	m_iMaxPoints = 50;

	memset(m_knots, 0, sizeof(m_knots));
	memset(m_Output, 0, sizeof(m_Output));

	m_CtrlPoint.clear();
	InsertPoint(0.0, 0.0);
	InsertPoint(0.2, 0.09);
	InsertPoint(0.7, 0.09);
	InsertPoint(1.0, 0.0);
	SplineCurve();
}


void BSpline::Duplicate(void *pSpline)
{
	if(!pSpline) return;
	BSpline *pBSpline = (BSpline*)pSpline;
	if(!pBSpline->IsSplineType(BSPLINE)) return;


	m_SplineType  = pBSpline->m_SplineType;
	m_SplineColor = pBSpline->m_SplineColor;

	m_CtrlPoint.clear();
	for(int ic=0; ic<pBSpline->m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint.append(pBSpline->m_CtrlPoint.at(ic));
	}

	m_iDegree     = pBSpline->m_iDegree;
	m_iKnots      = pBSpline->m_iKnots;

	memcpy(m_Output, &pBSpline->m_Output, sizeof(m_Output));
	memcpy(m_knots, &pBSpline->m_knots, sizeof(m_knots));
}


void BSpline::CopySymetric(BSpline *pSpline)
{
	m_SplineType  = pSpline->m_SplineType;
	m_SplineColor = pSpline->m_SplineColor;

	m_iDegree     = pSpline->m_iDegree;
	m_iKnots      = pSpline->m_iKnots;

	for(int ic=0; ic<pSpline->m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint.append(pSpline->m_CtrlPoint.at(ic));
		m_CtrlPoint[ic].y =  -m_CtrlPoint.at(ic).y;
	}

	for(int i=0; i<m_iKnots; i++)
	{
		m_knots[i] = pSpline->m_knots[i];
	}
}




void BSpline::DrawOutputPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	static QPoint pt;
	static int i, width;
	static QPen OutPen;

	width = 2;

	OutPen.setColor(m_SplineColor);
	OutPen.setStyle(Qt::SolidLine);
	OutPen.setWidth(1);
	painter.setPen(OutPen);

	for (i=0; i<SPLINEOUTPUTRES;i++)
	{
		pt.rx() = (int)( m_Output[i].x*scalex + Offset.x());
		pt.ry() = (int)(-m_Output[i].y*scaley + Offset.y());

		painter.drawRect(pt.x()-width, pt.y()-width, 2*width, 2*width);
	}

	painter.restore();
}


void BSpline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	static QPoint From, To;
	static int k;

	if(m_CtrlPoint.size()>0)
	{ 
		From.rx() = (int)( m_Output[0].x * scalex + Offset.x());
		From.ry() = (int)(-m_Output[0].y * scaley + Offset.y());

		for(k=1; k<SPLINEOUTPUTRES;k++)
		{
			To.rx() = (int)( m_Output[k].x * scalex + Offset.x());
			To.ry() = (int)(-m_Output[k].y * scaley + Offset.y());

			painter.drawLine(From, To);

			From = To;
		}
	}
	painter.restore();
}


void BSpline::Export(QTextStream &out, bool bExtrados)
{
	int k;
	QString strOut;

	if(bExtrados)
	{
		for (k=SPLINEOUTPUTRES-1;k>=0; k--)
		{
			strOut= QString(" %1  %2\n").arg(m_Output[k].x,7,'f',4).arg( m_Output[k].y,7,'f',4);
			out << strOut;
		}
	}
	else
	{
		for (k=1;k<SPLINEOUTPUTRES; k++)
		{
			strOut=QString(" %1  %2\n").arg(m_Output[k].x,7,'f',4).arg( m_Output[k].y,7,'f',4);
			out << strOut;
		}
	}
}


double BSpline::GetY(double const &x)
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



Vector3d BSpline::GetNormal(double const &x)
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



void BSpline::GetCamber(double &Camber, double &xc)
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


double BSpline::SplineBlend(int const &i,  int const &p, double const &t)
{
/*	Calculate the blending value, this is done recursively.
	If the numerator and denominator are 0 the expression is 0.
	If the denominator is 0 the expression is 0 */
//
//	   i   is the control point's index
//	   p   is the spline's degree 	
//	   t   is the spline parameter
//

	static double pres = 1.e-10; //same for all the recursive calls...

	if (p == 0) 
	{
		if ((m_knots[i] <= t) && (t < m_knots[i+1]) )  return  1.0;
//		else if (fabs(m_knots[i]-m_knots[i+1])<pres)   return  0.0;
		else                                           return  0.0;
	} 
	else
	{
                if (fabs(m_knots[i+p] - m_knots[i])<pres && fabs(m_knots[i+p+1] - m_knots[i+1])<pres)
			return  0.0;
                else if (fabs(m_knots[i+p] - m_knots[i])<pres)
			return  (m_knots[i+p+1]-t) / (m_knots[i+p+1]-m_knots[i+1]) * SplineBlend(i+1, p-1, t);
                else if (fabs(m_knots[i+p+1]-m_knots[i+1])<pres)
			return  (t - m_knots[i])   / (m_knots[i+p] - m_knots[i])   * SplineBlend(i,   p-1, t);
		else 
			return  (t - m_knots[i])   / (m_knots[i+p]-m_knots[i])	   * SplineBlend(i,   p-1, t) + 
			        (m_knots[i+p+1]-t) / (m_knots[i+p+1]-m_knots[i+1]) * SplineBlend(i+1 ,p-1, t);
	}
}



bool BSpline::SplineCurve()
{
	static double t, increment, b;
	static int i,j;

	if (m_CtrlPoint.size()>0)
	{
		t = 0;
		increment = 1.0/(double)(SPLINEOUTPUTRES - 1);
		
		for (j=0;j<SPLINEOUTPUTRES;j++)
		{
			m_Output[j].x = 0;
			m_Output[j].y = 0;

			for (i=0; i<m_CtrlPoint.size(); i++)
			{
				b = SplineBlend(i, m_iDegree, t);
				m_Output[j].x += m_CtrlPoint[i].x * b;
				m_Output[j].y += m_CtrlPoint[i].y * b;
			}
			t += increment;
		}
		m_Output[SPLINEOUTPUTRES-1] = m_CtrlPoint[m_CtrlPoint.size()-1];
	}
	return true;
}


void BSpline::SplineKnots()
{
	static double a,b;
	static int j, iDegree;

	iDegree = qMin(m_iDegree, m_CtrlPoint.size());

	m_iKnots  = iDegree + m_CtrlPoint.size() + 1;
	for (j=0; j<m_iKnots; j++) 
	{
		if (j<iDegree+1)  m_knots[j] = 0.0;
		else 
		{
			if (j<m_CtrlPoint.size())
			{
				a = (double)(j-iDegree);
				b = (double)(m_iKnots-2*iDegree-1);
                                if(fabs(b)>0.0) m_knots[j] = a/b;
				else            m_knots[j] = 1.0; // added arcds
			}
			else m_knots[j] = 1.0;	
		}
	}
}



bool BSpline::Serialize(QDataStream &ar, bool bIsStoring)
{
	if(bIsStoring)
	{
		ar << m_iDegree;
		ar << m_CtrlPoint.size();
		for(int ic=0; ic<m_CtrlPoint.size();ic++)
		{
			ar << m_CtrlPoint.at(ic).x << m_CtrlPoint.at(ic).y;
		}
	}
	else
	{
		ar >> m_iDegree;
		int n;
		double x,y;
		m_CtrlPoint.clear();
		ar >> n;
		for(int ic=0; ic<n;ic++)
		{
			ar >> x >> y;
			m_CtrlPoint.append(Vector3d(x,y,0.0));
		}
		SplineKnots();
		SplineCurve();
	}
	return true;
}



void BSpline::GetSlopes(double &s0, double &s1)
{
	s0 = atan2(m_CtrlPoint[1].y, m_CtrlPoint[1].x)      * 180/PI;
	int i=m_CtrlPoint.size()-2;
	s1 = atan2(m_CtrlPoint[i].y, (1.-m_CtrlPoint[i].x)) * 180/PI;
}


