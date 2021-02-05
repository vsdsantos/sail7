/****************************************************************************

         PointSpline Class
         Copyright (C) 2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/


#ifndef POINTSPLINE_H
#define POINTSPLINE_H

#include "spline.h"

class PointSpline : public Spline
{
public:
    PointSpline();

    void DrawSpline(QPainter & painter, double scalex, double scaley, QPoint const &Offset);
    void GetCamber(double &Camber, double &xc);
    double GetY(double const &x);
    Vector3d GetNormal(double x);

    void GetSlopes(double &s0, double &s1);
    void Duplicate(Spline *pSpline);
    bool Serialize(QDataStream &ar, bool bIsStoring);
};

#endif // POINTSPLINE_H
