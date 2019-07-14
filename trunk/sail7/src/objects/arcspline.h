/****************************************************************************

         Arc Spline Class
         Copyright (C) 2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/


#ifndef ARCSPLINE_H
#define ARCSPLINE_H

#include "bspline.h"



class ArcSpline : public Spline
{
public:
    ArcSpline();

    void DrawSpline(QPainter & painter, double scalex, double scaley, QPoint const &Offset);
    void Duplicate(Spline *pSpline);
    void GetCamber(double &Camber, double &xc);
    void GetSlopes(double &s0, double &s1);

    double GetY(double const &x);
    bool Serialize(QDataStream &ar, bool bIsStoring);

    Vector3d GetNormal(double x);


    bool SplineCurve();
    BSpline m_Spline1, m_Spline2;

};

#endif // ARCSPLINE_H
