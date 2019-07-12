#ifndef NACA4SPLINE_H
#define NACA4SPLINE_H
#include "Spline.h"


class Naca4Spline : public Spline
{
public:
	Naca4Spline();

	void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void Duplicate(void *pSpline);
	void GetCamber(double &Camber, double &xc);
	double GetY(double const &x);
	void GetSlopes(double &s0, double &s1);
	bool Serialize(QDataStream &ar, bool bIsStoring);

	Vector3d GetNormal(double const &x);

	double p, m;
};

#endif // NACA4SPLINE_H
