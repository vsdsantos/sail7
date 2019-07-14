#ifndef NACA4SPLINE_H
#define NACA4SPLINE_H
#include "spline.h"


class Naca4Spline : public Spline
{
public:
    Naca4Spline();

    void DrawSpline(QPainter & painter, double scalex, double scaley, QPoint const &Offset);
    void Duplicate(Spline *pSpline);
    void GetCamber(double &Camber, double &xc);
    double GetY(double const &x);
    void GetSlopes(double &s0, double &s1);
    bool Serialize(QDataStream &ar, bool bIsStoring);

    Vector3d GetNormal(double x);

    double p, m;
};

#endif // NACA4SPLINE_H
