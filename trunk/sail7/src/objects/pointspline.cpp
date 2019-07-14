/****************************************************************************

         PointSpline Class
         Copyright (C) 2012 Andre Deperrois
         All rights reserved

*****************************************************************************/

#include "../params.h"
#include "pointspline.h"
#include <QtDebug>


PointSpline::PointSpline()
{
    m_SplineType = POINTSPLINE;

    m_Style = 0;
    m_Width = 1;
    m_SplineColor = QColor(31, 77, 255);

    m_CtrlPoint.clear();
    m_iMinPoints = 2;
    m_iMaxPoints = 300;

    m_CtrlPoint.clear();
    InsertPoint(0.0,0.0);
    InsertPoint(0.1,0.12);
    InsertPoint(0.2,0.15);
    InsertPoint(0.35,0.11);
    InsertPoint(0.5,0.1);
    InsertPoint(0.7,0.07);
    InsertPoint(1.0,0.0);

}


void PointSpline::DrawSpline(QPainter & painter, double scalex, double , QPoint const &Offset)
{
    painter.save();
    static QPoint From, To;

    From = QPoint(int(m_CtrlPoint[0].x*scalex) + Offset.x(), int(-m_CtrlPoint[0].y*scalex) + Offset.y());

    for(int i=0; i<m_CtrlPoint.size(); i++)
    {
        To   = QPoint(int(m_CtrlPoint[i].x*scalex) + Offset.x(), int(-m_CtrlPoint[i].y*scalex) + Offset.y());
        painter.drawLine(From, To);
        From = To;
    }

    painter.restore();
}


double PointSpline::GetY(double const &x)
{
    static int i;
    static double y;

    if(x<=0.0 || x>=1.0) return 0.0;

    for (i=0; i<m_CtrlPoint.size()-1; i++)
    {
        if (m_CtrlPoint[i].x <m_CtrlPoint[i+1].x  &&   m_CtrlPoint[i].x <= x && x<=m_CtrlPoint[i+1].x )
        {
            y = (m_CtrlPoint[i].y     + (m_CtrlPoint[i+1].y-m_CtrlPoint[i].y)
                /(m_CtrlPoint[i+1].x-m_CtrlPoint[i].x)*(x-m_CtrlPoint[i].x));
            return y;
        }
    }
    return 0.0; //we'll have to do better...
}



Vector3d PointSpline::GetNormal(double )
{
    return Vector3d(0.0, 1.0, 0.0);
}




void PointSpline::GetCamber(double &Camber, double &xc)
{
    static int i;
    Camber = xc =0.0;

    for (i=0; i<m_CtrlPoint.size()-1; i++)
    {
        if(fabs(m_CtrlPoint.at(i).y)>fabs(Camber))
        {
            xc = m_CtrlPoint.at(i).x;
            Camber = m_CtrlPoint.at(i).y;
        }
    }
}


void PointSpline::Duplicate(Spline *pSpline)
{
    if(!pSpline) return;
    PointSpline *pPointSpline = dynamic_cast<PointSpline*>(pSpline);
    if(!pPointSpline->IsSplineType(POINTSPLINE)) return;

    m_SplineType  = pPointSpline->m_SplineType;
    m_SplineColor = pPointSpline->m_SplineColor;

    m_CtrlPoint.clear();
    for(int i=0; i<pPointSpline->m_CtrlPoint.size(); i++)
    {
        m_CtrlPoint.append(pPointSpline->m_CtrlPoint.at(i));
    }
}




bool PointSpline::Serialize(QDataStream &ar, bool bIsStoring)
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
    }
    return true;
}


void PointSpline::GetSlopes(double &s0, double &s1)
{
    s0 = atan2(m_CtrlPoint[1].y, m_CtrlPoint[1].x)      * 180/PI;
    int i=m_CtrlPoint.size()-2;
    s1 = atan2(m_CtrlPoint[i].y, (1.-m_CtrlPoint[i].x)) * 180/PI;
}






