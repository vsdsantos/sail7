/****************************************************************************

		 Arc Spline Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

//Spline based on the two BSpline Arcs

#include "ArcSpline.h"
#include <QtDebug>


ArcSpline::ArcSpline()
{
	m_SplineType = ARCSPLINE;

	m_Style = 0;
	m_Width = 1;
	m_SplineColor = QColor(130, 110, 186);

	m_Spline1.SetColor(m_SplineColor);
	m_Spline2.SetColor(m_SplineColor);

	m_CtrlPoint.clear();
	m_CtrlPoint.append(Vector3d(0.0, 0.0,  0.0));
	m_CtrlPoint.append(Vector3d(0.3, 0.10, 0.0));
	m_CtrlPoint.append(Vector3d(1.0, 0.0,  0.0));

	m_Spline1.InsertPoint(m_CtrlPoint.at(0).x, m_CtrlPoint.at(0).y);
	m_Spline1.InsertPoint(m_SlopePoint[0].x, m_SlopePoint[0].x);
	m_Spline1.InsertPoint(m_CtrlPoint.at(1).x, m_CtrlPoint.at(1).y);
	m_Spline1.m_iDegree = 2;
	m_Spline1.SplineKnots();
	m_Spline1.SplineCurve();

	m_Spline2.InsertPoint(m_CtrlPoint.at(1).x, m_CtrlPoint.at(1).y);
	m_Spline2.InsertPoint(m_SlopePoint[1].x, m_SlopePoint[1].y);
	m_Spline2.InsertPoint(m_CtrlPoint.at(2).x, m_CtrlPoint.at(2).y);
	m_Spline2.m_iDegree = 2;
	m_Spline2.SplineKnots();
	m_Spline2.SplineCurve();

	m_Spline1.SetColor(m_SplineColor);
	m_Spline2.SetColor(m_SplineColor);

	m_bShowSlope = true;
}



void ArcSpline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	m_Spline1.DrawSpline(painter, scalex, scaley, Offset);
	m_Spline2.DrawSpline(painter, scalex, scaley, Offset);

	painter.restore();
}


bool ArcSpline::SplineCurve()
{
	m_Spline1.m_CtrlPoint[0] = m_CtrlPoint[0];
	m_Spline1.m_CtrlPoint[1] = m_SlopePoint[0];
	m_Spline1.m_CtrlPoint[2] = m_CtrlPoint[1];

	m_Spline2.m_CtrlPoint[0] = m_CtrlPoint[1];
	m_Spline2.m_CtrlPoint[1] = m_SlopePoint[1];
	m_Spline2.m_CtrlPoint[2] = m_CtrlPoint[2];

	m_Spline1.SplineCurve();
	m_Spline2.SplineCurve();
    return true;
}


void ArcSpline::Duplicate(void *pSpline)
{
	ArcSpline *pArcSpline = (ArcSpline*)pSpline;
	if(!pArcSpline->IsSplineType(ARCSPLINE)) return;

	m_SplineType  = pArcSpline->m_SplineType;
	m_SplineColor = pArcSpline->m_SplineColor;

	m_CtrlPoint.clear();
	for(int ic=0; ic<pArcSpline->m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint.append(pArcSpline->m_CtrlPoint.at(ic));
	}

	pArcSpline->m_Spline1.Duplicate(&pArcSpline->m_Spline1);
	pArcSpline->m_Spline2.Duplicate(&pArcSpline->m_Spline2);
}


double ArcSpline::GetY(double const &x)
{
	if(x<=m_CtrlPoint.at(1).x) return m_Spline1.GetY(x);
	else                       return m_Spline2.GetY(x);

}



void ArcSpline::GetCamber(double &Camber, double &xc)
{
	xc     = m_CtrlPoint[1].x;
	Camber = m_CtrlPoint[1].y;
}



bool ArcSpline::Serialize(QDataStream &ar, bool bIsStoring)
{
	return true;
}


void ArcSpline::GetSlopes(double &s0, double &s1)
{
	s0 = atan2(m_SlopePoint[0].y, m_SlopePoint[0].x)      * 180/PI;
	s1 = atan2(m_SlopePoint[1].y, (1.-m_SlopePoint[1].x)) * 180/PI;
}



Vector3d ArcSpline::GetNormal(double const &x)
{
	return Vector3d(0.0, 1.0, 0.0);
}
