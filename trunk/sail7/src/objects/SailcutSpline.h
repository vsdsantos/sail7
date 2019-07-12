/****************************************************************************

		 SailcutSpline Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/


#pragma once
#include "vector3d.h"

#include <QPainter>
#include <QPoint>

class SailcutSpline
{
    friend class SailSection;
    friend class SailDlg;
    friend class SailcutSail;
    friend class SectionViewWidget;
    friend class SailDomDoc;

public:
	SailcutSpline();

    void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, const QPointF &Offset, double Chord, double Twist);
//	bool SplineCurve();
	void ComputeAKVR2(double slope0, double slope1, double c);
	void ComputeAKVR4(double slope0, double slope1, double c);
	void GetCamber(double &Camber, double &xc);
	void GetSlopes(double &s0, double &s1);
	void Duplicate(SailcutSpline *pSCSpline);
	bool Serialize(QDataStream &ar, bool bIsStoring);

	void DepthtoK(double d);
	double Depth();


	void jacobian2(double e, double v, double cg[]);
	void jacobian(double k, double e, double v, double r, double cg[]);

	Vector3d GetNormal(double const &x);

	void SetStyle(int style){m_Style = style;}
	void SetWidth(int width){m_Width = width;}
	void SetColor(QColor color){m_SplineColor = color;}

	void SetSplineParams(double k, double av, double ar)
	{
		K  = k;
		AV = av;
		AR = ar;
		A = 1.0 + av/4.0;
    }

	double GetY(double const &x)
	{
		double B = A/((AV+2)*(AV+1));
		double C = AR/6 - B;
		return K*(-A*pow((1-x),(AV+2)) / (( AV+2)*(AV +1)) - AR/6*x*x*x + C*x + B);
	}



private:

    double f0(double k, double v, double r, double )
	{
		// slope at (0,0) is s0
		return k*(a/(v+1) + r/6.0 - a/(v+1)/(v+2)) -s0;
	}

    double f1(double k, double v, double r, double )
	{
		// slope at (1,0) is s1
		return k*(-r/2 + r/6 - a/(v+1)/(v+2)) -s1;
	}

    double f2(double , double v, double r, double e)
	{
		//slope is 0 at x=e
		return a*pow((1.0-e),(v+1.0)) /(v+1.0) -r/2.0*e*e +r/6.0 -a/(v+1.0)/(v+2.0);
	}

	double f3(double k, double v, double r, double e)
	{
		// y=t at x=e
		return k*( -a*pow((1.0-e),(v+2.0)) /(v+1)/(v+2) -r/6*e*e*e + (r/6-a/(v+1)/(v+2))*e + a/(v+1)/(v+2) ) -t;
	}


	double g0(double v, double e)
	{
		return( (8 * s0 * v*v + 24 * s0 * v + 16 * s0 + 4 * s1 * v*v + 12 * s1 * v + 8 * s1) * ((1.0+  v / 4.0) *   pow( (1 - e),  (v+1)) /  (v+1)+ 3.0 / 4.0*  (s1 * v + s1 + s0) *  (v+4) *  (e*e) /  (v+2)/  (s1 * v + 2 * s0 * v + s1 + 2 * s0) -  ((s1 * v + s1 + s0) * (v+4) / (v+2)/ (s1 * v + 2 * s0 * v + s1 + 2 * s0)) / 4.0- (1.0+  v / 4.0) /  (v+2)/  (v+1)) /  (2 * v*v + 9 * v + 4));
	}

	double g1(double v, double e)
	{
		return( (8 * s0 * v*v + 24 * s0 * v + 16 * s0 + 4 * s1 * v*v + 12 * s1 * v + 8 * s1) * (-(1.0+  v / 4.0) *   pow( (1 - e),  (v+2)) /  (v+2)/  (v+1)+  ((s1 * v + s1 + s0) * (v+4) *  e*e*e / (v+2)/ (s1 * v + 2 * s0 * v + s1 + 2 * s0)) / 4.0+ (- ((s1 * v + s1 + s0) * (v+4) / (v+2)/ (s1 * v + 2 * s0 * v + s1 + 2 * s0)) / 4.0- (1.0+  v / 4.0) /  (v+2)/  (v+1)) *  e + (1.0+  v / 4.0) /  (v+2)/  (v+1)) /  (2 * v*v + 9 * v + 4) - t);
	}
	double Getzmax(double const &k, double const &av, double const &ar);

private:

    QList <Vector3d> *m_pCtrlPoint;

//	double m_Chord;

    QColor m_SplineColor;
    int m_Style, m_Width;



    double AV, AR, K, A;

    double s0, s1; //slopes at the leading and the trailing edges;
    double e; //position of max camber 0<e<1;
    double t; // max camber
    double a;

};

