/****************************************************************************

		 SailSection Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include "../globals.h"
#include "Sail.h"
#include "SailSection.h"
#include "NURBSSail.h"
#include "SailcutSail.h"
#include "BSpline.h"
#include "BezierSpline.h"
#include "CubicSpline.h"
#include "SailcutSpline.h"
#include "PointSpline.h"
#include "Naca4Spline.h"
#include "ArcSpline.h"

#include<QtDebug>


SailSection::SailSection(void *pParent)
{
	m_pSail=pParent;
	m_bIsTipSection = false;
	m_Position.Set(0.0,0.0,0.0);
	m_Chord = 1.0;
	m_c = m_x = 0.0;
	m_Angle = 0.0;
	m_iSelect = m_iHighlight = -1;
	m_SectionColor = QColor(255,100, 0);
	m_CtrlPoint.clear();
	m_SCSpline.m_pCtrlPoint = &m_CtrlPoint;
}


void SailSection::SetSection(double const &span, double const& chord, double const&twist, double const&angle)
{
	m_Position.z = span;
	if(IsSailcutSection())
	{
		m_Chord = chord;

		if(m_pSail)
		{
//			SailcutSail *pSCSail = (SailcutSail*)m_pSail;
//			m_SCSpline.m_Twist = pSCSail->Twist(this);
		}
	}
	m_Angle = angle;
}

/*
CVector SailSection::GetPoint(double xrel)
{
	if(IsSailcutSection())	return CVector(xrel, m_SCSpline.GetY(xrel), 0.0);
	else
	{
		//TODO
		qDebug()<<"Calling NURBS SECTION point";
		return CVector(0,0,0);
	}
}*/




void SailSection::Copy(SailSection *pSection)
{
	m_CtrlPoint.clear();
	for(int ic=0; ic<m_CtrlPoint.count(); ic++) m_CtrlPoint.append(pSection->m_CtrlPoint.at(ic));

	m_Chord     = pSection->m_Chord;
	m_Position  = pSection->m_Position;
	m_Angle     = pSection->m_Angle;

	m_SCSpline.Duplicate(&pSection->m_SCSpline);
}


bool SailSection::SerializeSection(QDataStream &ar, bool bIsStoring)
{
	if(bIsStoring)
	{	// storing code
		ar << 234000; //234000 : initial format
		ar << m_Position.x << m_Position.y << m_Position.z;
		ar <<  m_Angle << m_Chord;

		if(IsNURBSSection())
		{
			ar << 1;
			ar <<m_CtrlPoint.size();
			for(int ic=0; ic<m_CtrlPoint.size(); ic++)
			{
				ar << m_CtrlPoint.at(ic).x << m_CtrlPoint.at(ic).y << m_CtrlPoint.at(ic).z;
			}
		}
		else if(IsSailcutSection())
		{
			ar << 2;
			m_SCSpline.Serialize(ar, bIsStoring);
		}

		return true;
	}
	else
	{
		// loading code
		int ArchiveFormat, k, nc;
		double x,y,z;
		ar >> ArchiveFormat;
		if (ArchiveFormat!=234000)  return false;

		ar >> m_Position.x >> m_Position.y >> m_Position.z;
		ar >>  m_Angle >> m_Chord;
		ar >> k;
		if(k==1)
		{
			//NURBS type section
			m_CtrlPoint.clear();
			ar>>nc;
			for(int ic=0; ic<nc; ic++)
			{
				ar >> x >> y >> z;
				m_CtrlPoint.append(CVector(x,y,z));
			}
		}
		else if (k==2)
		{
			//Sailcut type section
			m_SCSpline.Serialize(ar, bIsStoring);
		}
		return true;
	}
}

/*
void SailSection::DrawSpline(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset, bool bSelected)
{
	static QPoint From, To;
	static int k;
	QPen SplinePen;

	painter.save();

	if(bSelected)
	{
		SplinePen.setStyle(GetStyle(m_SCSpline.m_Style));
		SplinePen.setWidth(m_SCSpline.m_Width);
		SplinePen.setColor(m_SCSpline.m_SplineColor);
		painter.setPen(SplinePen);
	}
	else
	{
		SplinePen.setWidth(1);
		SplinePen.setColor(QColor(100,100,100));
		SplinePen.setStyle(Qt::DashLine);
		painter.setPen(SplinePen);
	}

	if(IsNURBSSection())
	{
		NURBSSail *pNSail = (NURBSSail*)m_pSail;
		double w = pNSail->m_SplineSurface.Getu(m_uPosition);
		double v = 0.0;
		CVector Point;

		int nv = 17;

		Point = pNSail->GetPoint(v, w);
		From.rx() = (int)( Point.x * scalex + Offset.x());
		From.ry() = (int)(-Point.y * scaley + Offset.y());
		m_c = Point.y;
		m_x = Point.x;

		for(k=1; k<=nv;k++)
		{
			v = (double)k/(double)nv;
			Point = pNSail->GetPoint(v, w);
			if(fabs(Point.y)>m_c)
			{
				m_c = Point.y;
				m_x = Point.x;
			}
			To.rx() = (int)( Point.x * scalex + Offset.x());
			To.ry() = (int)(-Point.y * scaley + Offset.y());

			painter.drawLine(From, To);

			From = To;
		}
	}
	else
	{
		SailcutSail *pSCSail = (SailcutSail*)m_pSail;
		m_SCSpline.DrawSpline(painter, scalex, scaley, Offset, pSCSail->Chord(this), pSCSail->Twist(this));
	}
	painter.restore();
}
*/

void SailSection::DrawSpline(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset, bool bSelected)
{
	static QPoint From, To;
	static int k;
	QPen SplinePen;

	painter.save();

	if(bSelected)
	{
		SplinePen.setStyle(GetStyle(m_SCSpline.m_Style));
		SplinePen.setWidth(m_SCSpline.m_Width);
		SplinePen.setColor(m_SCSpline.m_SplineColor);
		painter.setPen(SplinePen);
	}
	else
	{
		SplinePen.setWidth(1);
		SplinePen.setColor(QColor(100,100,100));
		SplinePen.setStyle(Qt::DashLine);
		painter.setPen(SplinePen);
	}

	if(IsSailcutSection())
	{
		SailcutSail *pSCSail = (SailcutSail*)m_pSail;
		m_SCSpline.DrawSpline(painter, scalex, scaley, Offset, pSCSail->Chord(this), pSCSail->Twist(this));
	}
	painter.restore();
}



bool SailSection::IsNURBSSection()
{
	CSail *pSail = (CSail*)m_pSail;
	return (pSail->IsNURBSSail());
}


bool SailSection::IsSailcutSection()
{
	CSail *pSail = (CSail*)m_pSail;
	return (pSail->IsSailcutSail());
}



void SailSection::DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint const &Offset)
{
	static QPoint pt;
	static int i, width;

	if(!IsSailcutSection() && m_CtrlPoint.size()>0)
	{
		painter.save();

		QColor SlopeClr;
		SlopeClr.setHsv(m_SectionColor.hue(), m_SectionColor.saturation(), m_SectionColor.value()*.29);
		QPen PointPen;
		QBrush NoBrush(Qt::NoBrush);
		PointPen.setStyle(Qt::DashLine);
		PointPen.setWidth(1);

		width  = 3;

		double cosa = cos(m_Angle*PI/180.0);
		double sina = sin(m_Angle*PI/180.0);

		PointPen.setWidth(1);

		painter.setPen(PointPen);
		painter.setBrush(NoBrush);

		CVector Point, LastPoint;
		for (i=0; i<m_CtrlPoint.size(); i++)
		{
			// rotate the control point in the direction of the section
			Point.x = m_Position.x + (m_CtrlPoint[i].x-m_Position.x) * cosa - (m_CtrlPoint[i].z-m_Position.z) * sina;
			Point.y = m_CtrlPoint[i].y;
			Point.z = m_Position.z + (m_CtrlPoint[i].x-m_Position.x) * sina + (m_CtrlPoint[i].z-m_Position.z) * cosa;

			pt.rx() = (int)( Point.x*scalex + Offset.x());
			pt.ry() = (int)(-Point.y*scaley + Offset.y());

			if (m_iSelect==i)
			{
				PointPen.setWidth(2);
				PointPen.setColor(QColor(100,100,250));
			}
			else if(m_iHighlight==i)
			{
				PointPen.setWidth(2);
				PointPen.setColor(QColor(255,0,0));
			}
			else
			{
				PointPen.setWidth(1);
				PointPen.setColor(m_SectionColor);
			}
			painter.setPen(PointPen);
			painter.drawEllipse(pt.x()-width, pt.y()-width, 2*width, 2*width);

			// if either the first of last section is selected, then
			// link the first and last control points to materialize the tangents
			if(IsTipSection()  && (i==1 || i==m_CtrlPoint.size()-1))
			{
				PointPen.setWidth(1);
				PointPen.setColor(SlopeClr);
				painter.setPen(PointPen);

				painter.drawLine(LastPoint.x*scalex+Offset.x(), -LastPoint.y*scaley+Offset.y(), pt.x(), pt.y());
			}

			LastPoint = Point;
		}

		painter.restore();

	}
}


bool SailSection::IsTipSection()
{
	return m_bIsTipSection;
}


void SailSection::GetSlopes(double &s0, double &s1)
{
	if(IsNURBSSection())
	{
		if(m_CtrlPoint.size()<=2)
		{
			s0=s1=0.0;
		}
		else
		{
			s0 = atan2(m_CtrlPoint[1].y    - m_CtrlPoint.first().y                , m_CtrlPoint[1].x     - m_CtrlPoint.first().x);
			s1 = atan2(m_CtrlPoint.last().y - m_CtrlPoint[m_CtrlPoint.size()-2].y , m_CtrlPoint.last().x - m_CtrlPoint[m_CtrlPoint.size()-2].x);
			s0 *= 180.0/PI;
			s1 *= 180.0/PI;
		}
	}
	else
	{
		m_SCSpline.GetSlopes(s0, s1);
		//add section twist;
		SailcutSail *pSCSail = (SailcutSail*)m_pSail;
		double twist = pSCSail->Twist(this);
		s0 += twist;
		s1 += twist;
	}
}


void SailSection::GetCamber(double &c, double &x)
{
	if(IsNURBSSection())
	{
		c = m_c;
		x = m_x;
	}
	else
	{
		m_SCSpline.GetCamber(c, x);
	}
}


double SailSection::Chord()
{
	if(IsNURBSSection())
	{
		if(IsTipSection())
		{
			if(m_CtrlPoint.size()<2) return 1.0;
			else                     return fabs(m_CtrlPoint.last().x-m_CtrlPoint.first().x);
		}
		else
		{
			return 1.0;//TODO... unused though
		}
	}
	else
	{
		return m_Chord;
	}
}


double SailSection::Twist()
{
	//TODO : should be a sail method
	static double w;
	static CVector PtA, PtB;

	if(IsNURBSSection())
	{
		NURBSSail *pNSail = (NURBSSail*)m_pSail;
		w = pNSail->m_SplineSurface.Getu(m_Position.z, 0.0);
		PtA =pNSail->GetPoint(0.0, w);
		PtB =pNSail->GetPoint(.9999, w);
		return atan2(PtB.y-PtA.y, PtB.x-PtA.x)*180.0/PI;
	}
	else
	{
		return 0.0;
	}
}


void SailSection::SetSpanPosition(double z)
{
	double trans = z - m_Position.z;

	if(IsNURBSSection())
	{
		for(int ic=0; ic<m_CtrlPoint.size(); ic++)
			m_CtrlPoint[ic].z += trans;
	}

	m_Position.z = z;
}


void SailSection::SetSailcutSpline(double k, double av, double ar)
{
	m_SCSpline.SetSplineParams(k, av, ar);
}


void SailSection::MoveTo(CVector NewPosition)
{
	if(!m_CtrlPoint.size()) return;
	CVector trans = NewPosition - m_CtrlPoint.first();

	for(int ic=0; ic<m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint[ic].x += trans.x;
		m_CtrlPoint[ic].y += trans.y;
	}
	SetwPosition(NewPosition.z);
}















