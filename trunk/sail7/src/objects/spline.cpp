/****************************************************************************

         CSpline Class
         Copyright (C) 2012 Andre Deperrois
         All rights reserved

*****************************************************************************/


#include "spline.h"
#include "../params.h"

bool Spline::s_bChanged = false;

Spline::Spline()
{
    m_SplineType = BSPLINE;
    m_Style = 0;//=Solid Line
    m_Width = 1;

    m_bShowSlope = false;

    m_SlopePoint[0].Set(0.1,0.1, 0.0);
    m_SlopePoint[1].Set(0.8,0.1,0.0);
}

Spline::~Spline()
{
}

bool Spline::InsertPoint(double const &x, double const &y)
{
    int k;

    if(m_CtrlPoint.size()>=SPLINECONTROLSIZE) return false;
    if(m_CtrlPoint.size()>=m_iMaxPoints)      return false;

    s_bChanged = true;

    if (x>=0.0 && x<=1.0)
    {
        //No points yet
        if(m_CtrlPoint.size()<=0)
        {
            m_CtrlPoint.append(Vector3d(x,y,0.0));
            m_CtrlPoint[0].x = x;
            m_CtrlPoint[0].y = y;
        }
        else
        {
            if(x<m_CtrlPoint[0].x)
            {
                // we're the new minimum point
                m_CtrlPoint.prepend(Vector3d(x,y,0.0));
            }
            else
            {
                if(x>=m_CtrlPoint[m_CtrlPoint.size()-1].x)
                {
                    // we're the new maximum point
                    m_CtrlPoint.append(Vector3d(x,y,0.0));
                }
                else
                {
                    // we're in between
                    for (k=0; k<m_CtrlPoint.size(); k++)
                    {
                        if (x>m_CtrlPoint[k].x && x<m_CtrlPoint[k+1].x)
                        {
                            m_CtrlPoint.insert(k+1, Vector3d(x,y,0.0));
                            break;
                        }
                    }
                }
            }
        }
    }
    SplineKnots();
    return true;
}



int Spline::IsControlPoint(Vector3d const &Real, double const &ZoomFactor)
{
    int k;
    for (k=0; k<m_CtrlPoint.size(); k++)
    {
        if (fabs(Real.x-m_CtrlPoint[k].x)<0.006/ZoomFactor &&
                fabs(Real.y-m_CtrlPoint[k].y)<0.006/ZoomFactor) return k;
    }
    return -10;
}


int Spline::IsSlopePoint(Vector3d const &Real, double const &ZoomFactor)
{
    int k;
    for (k=0; k<2; k++)
    {
        if (fabs(Real.x-m_SlopePoint[k].x)<0.006/ZoomFactor &&
                fabs(Real.y-m_SlopePoint[k].y)<0.006/ZoomFactor) return k;
    }
    return -10;
}



bool Spline::RemovePoint(int const &k)
{
    if(m_CtrlPoint.size()<=m_iMinPoints) return false;

    s_bChanged = true;

    if (k>0 && k<m_CtrlPoint.size()-1)
    {
        m_CtrlPoint.removeAt(k);
        SplineKnots();
        SplineCurve();
    }
    return true;
}


void Spline::DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint const &Offset)
{
    QPoint pt;
    int i, width;

    if(IsSplineType(SAILCUTSPLINE)) return;//Ctrl points are not used

    painter.save();

    width  = 3;

    QPen PointPen;
    QBrush NoBrush(Qt::NoBrush);
    PointPen.setWidth(1);

    painter.setPen(PointPen);
    painter.setBrush(NoBrush);

    for (i=0; i<m_CtrlPoint.size(); i++)
    {
        pt.rx() = int( m_CtrlPoint[i].x*scalex + Offset.x());
        pt.ry() = int(-m_CtrlPoint[i].y*scaley + Offset.y());

        /*        if (m_iSelect==i)
        {
            PointPen.setWidth(2);
            PointPen.setColor(QColor(0,0,150));
        }
        else if(m_iHighlight==i)
        {
            PointPen.setWidth(2);
            PointPen.setColor(QColor(255,0,0));
        }
        else*/
        {
            PointPen.setWidth(1);
            PointPen.setColor(m_SplineColor);
        }
        painter.setPen(PointPen);
        painter.drawEllipse(pt.x()-width, pt.y()-width, 2*width, 2*width);
    }

    if(IsSplineType(BEZIERSPLINE) || IsSplineType(BSPLINE))
    {
        //link first control points to materialize tangent
        PointPen.setColor(QColor(0,255,255));
        PointPen.setStyle(Qt::DashLine);
        PointPen.setWidth(1);
        painter.setPen(PointPen);

        pt.rx() = int( m_CtrlPoint[1].x*scalex + Offset.x());
        pt.ry() = int(-m_CtrlPoint[1].y*scaley + Offset.y());
        painter.drawLine(int(0.0*scalex+Offset.x()), int(0.0*scaley+Offset.y()), pt.x(), pt.y());

        pt.rx() = int( m_CtrlPoint[m_CtrlPoint.size()-2].x*scalex + Offset.x());
        pt.ry() = int(-m_CtrlPoint[m_CtrlPoint.size()-2].y*scaley + Offset.y());
        painter.drawLine(int(1.0*scalex+Offset.x()), int(0.0*scaley+Offset.y()), pt.x(), pt.y());
    }

    painter.restore();
}



void Spline::DrawSlopePoints(QPainter &painter, double scalex, double scaley, QPoint const &Offset)
{
    painter.save();

    QPoint pt;
    int width;

    width  = 3;

    QPen PointPen;
    QBrush NoBrush(Qt::NoBrush);
    PointPen.setWidth(1);

    painter.setPen(PointPen);
    painter.setBrush(NoBrush);

    for(int i=0; i<2; i++)
    {
        pt.rx() = int( m_SlopePoint[i].x*scalex + Offset.x());
        pt.ry() = int(-m_SlopePoint[i].y*scaley + Offset.y());

        /*        if (m_iSlopeSelect==i)
        {
            PointPen.setWidth(2);
            PointPen.setColor(QColor(0,0,255));
        }
        else if(m_iSlopeHighlight==i)
        {
            PointPen.setWidth(2);
            PointPen.setColor(QColor(0,255,0));
        }
        else*/
        {
            PointPen.setWidth(1);
            PointPen.setColor(QColor(255,0,0));
        }
        painter.setPen(PointPen);
        painter.drawEllipse(pt.x()-width, pt.y()-width, 2*width, 2*width);

        PointPen.setColor(QColor(0,255,255));
        PointPen.setStyle(Qt::DashLine);
        PointPen.setWidth(1);
        painter.setPen(PointPen);
        if(i==0) painter.drawLine(int(0.0*scalex+Offset.x()), int(0.0*scaley+Offset.y()), pt.x(), pt.y());
        else     painter.drawLine(int(1.0*scalex+Offset.x()), int(0.0*scaley+Offset.y()), pt.x(), pt.y());
    }
    painter.restore();
}


QString Spline::SplineType()
{
    switch(m_SplineType)
    {
        case BSPLINE:
        {
            return"B-Spline";
        }
        case BEZIERSPLINE:
        {
            return"Bezier Spline";
        }
        case ARCSPLINE:
        {
            return"Arc Spline";
        }
        case CUBICSPLINE:
        {
            return"Cubic Spline";
        }
        case NACA4SPLINE:
        {
            return"Naca4 Spline";
        }
        case SAILCUTSPLINE:
        {
            return"Sailcut Spline";
        }
        case POINTSPLINE:
        {
            return"Point Spline";
        }
    }
    return QString();
}


QString Spline::SplineType(int iType)
{
    switch(iType)
    {
        case BSPLINE:
        {
            return"B-Spline";
        }
        case BEZIERSPLINE:
        {
            return"Bezier Spline";
        }
        case ARCSPLINE:
        {
            return"Arc Spline";
        }
        case CUBICSPLINE:
        {
            return"Cubic Spline";
        }
        case NACA4SPLINE:
        {
            return"Naca4 Spline";
        }
        case SAILCUTSPLINE:
        {
            return"Sailcut Spline";
        }
        case POINTSPLINE:
        {
            return"Point Spline";
        }
        default:
        {
            return "B-Spline";
        }
    }
}

void Spline::CopyPoints(QList<Vector3d> *m_pPointList)
{
    m_CtrlPoint.clear();
    if(!m_pPointList->size()) return;
    for(int ip=0; ip<m_pPointList->size(); ip++)
    {
        m_CtrlPoint.append(m_pPointList->at(ip));
    }
}









