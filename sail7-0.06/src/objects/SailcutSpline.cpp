/****************************************************************************

		 SailcutSpline Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include "../params.h"
#include "SailcutSpline.h"
#include "../globals.h"
#include "./Spline.h"
#include <QtDebug>


/*
	The sailcut spline is based on the curve proposed by Robert Laine for Sailcut :

		The following equation is used to describe the second derivative of the profile function of X:
		Z''= K*[-A*(1-X)^AV - AR*X]

		After a first integration it gives the slope of the profile:
		Z'= K*[A*(1 - X)^(AV + 1) / (AV+1) - AR/2*X^2 + C]

		Finally after a second integration the equation giving the depth at any point is:
		Z = K*[-A*(1-X)^(AV+2) / (( AV+2)*(AV+1)) - AR/6*X^3 + C*X + B]

		To meet the profile end conditions (X=0, Z=0) and (X=1, Z=0) the constants B and C are:
		B = A / ((AV + 2) * (AV + 1))
		C = AR / 6 - B

		The maximum depth is obtained when the slope Z' is equal to zero, this allow to
		calculate K such that the depth at that point is the one desired.

		The factors AV and AR give a measure of the camber at the leading edge (AV) and
		trailing edge (AR). Together with the maximum depth value these factors are sufficient
		to describe the profile of the sail at any height.

		The factor A defines different families of profiles with a different distribution
		of fullness fore/aft. In practice A = 1 give good profiles for sails used in light
		conditions. I prefer to use sail profiles with more fullness forward and a flatter
		leech as obtained with the factor A = 1 + AV / 4. This is the factor used in Sailcut
		and it give a good range of utilisation of the sails.

		The following table give an example of profile data obtained with the above equations.
		AV = 5.00
		AR = 0.02
		K = 2.94
		A = 2.250
		B = 0.054
		C =-0.050
		curvature = z" / (1+ z'*z')^3/2
*/


SailcutSpline::SailcutSpline()
{
	m_Style = 0;
	m_Width = 1;
	m_SplineColor = QColor(10, 245, 171);

	AV = 5.00;
	AR = 0.02;
	K = 2.94;
	A = 1.0 + AV/4.0;

}

#define SPLINEPOINTS 50

void SailcutSpline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset, double Chord, double Twist)
{
	painter.save();

	static double cost, sint, sx, sy;
	static double B,C;
	static double x, y, xr, yr;
	static QPoint From, To;

	cost = cos(Twist*PI/180.0);
	sint = sin(Twist*PI/180.0);

	sx = scalex*Chord;
	sy = scaley*Chord;

	B = A/((AV+2) * (AV+1));
	C = AR/6 - B;

	x = 0.0;
	y = K*(-A*pow((1-x),(AV+2)) / (( AV+2)*(AV +1)) - AR/6*x*x*x + C*x + B);
	xr = x*cost - y*sint;
	yr = x*sint + y*cost;
	From = QPoint((xr*sx) + Offset.x(), (-yr*sy) + Offset.y());
	for(int j=0; j<=SPLINEPOINTS; j++)
	{
		x = (double)j/(double)SPLINEPOINTS;
		y = K*(-A*pow((1-x),(AV+2)) / (( AV+2)*(AV +1)) - AR/6*x*x*x + C*x + B);
		xr = x*cost - y*sint;
		yr = x*sint + y*cost;
		To   = QPoint((xr*sx) + Offset.x(), (-yr*sy) + Offset.y());
		painter.drawLine(From, To);
		From = To;
	}

	painter.restore();
}


double SailcutSpline::Getzmax(double const &k, double const &av, double const &ar)
{
	int iter=0;
	double x, x0, x1, s;
	double a = 1+av/4.0;
	double b = a/((av+2) * (av+1));
	double c = ar/6 - b;

	x0 = 0.0;
	x1 = 1.0;

	while(iter<100 && fabs(x1-x0)>.01)
	{
		x = (x0+x1)/2.0;
		s = k*(a*pow((1-x),(av+1)) / (av+1) - ar/2*x*x + c);
		if(s>0) x0=x;
		else    x1=x;
		iter++;
	}

	return k*(-a*pow((1-x),(av+2)) / ((av+2)*(av+1)) - ar/6*x*x*x + c*x + b);
}


double SailcutSpline::Depth()
{
	int iter=0;
	double x, x0, x1, s;
	double a = 1+AV/4.0;
	double b = a/((AV+2) * (AV+1));
	double c = AR/6 - b;

	x0 = 0.0;
	x1 = 1.0;

	while(iter<100 && fabs(x1-x0)>.01)
	{
		x = (x0+x1)/2.0;
		s = K*(a*pow((1-x),(AV+1)) / (AV+1) - AR/2*x*x + c);
		if(s>0) x0=x;
		else    x1=x;
		iter++;
	}

	return K*(-a*pow((1-x),(AV+2)) / ((AV+2)*(AV+1)) - AR/6*x*x*x + c*x + b);
}


void SailcutSpline::GetCamber(double &Camber, double &xc)
{
	double x, c;
	double B = A/((AV+2) * (AV+1));
	double C = AR/6 - B;

	xc = x = Camber = c = 0.0;

	for(int j=0; j<SPLINEOUTPUTRES; j++)
	{
		x = (double)j/(double)SPLINEOUTPUTRES;
		c = K*(-A*pow((1-x),(AV+2)) / (( AV+2)*(AV +1)) - AR/6*x*x*x + C*x + B);

		if(fabs(c)>fabs(Camber))
		{
			xc=x;
			Camber = c;
		}
//		else return;
	}
}


void SailcutSpline::Duplicate(SailcutSpline *pSCSpline)
{
	m_SplineColor = pSCSpline->m_SplineColor;

	m_pCtrlPoint->clear();
	for(int ic=0; ic<pSCSpline->m_pCtrlPoint->size(); ic++)
	{
		m_pCtrlPoint->append(pSCSpline->m_pCtrlPoint->at(ic));
	}
	AV = pSCSpline->AV;
	AR = pSCSpline->AR;
	K  = pSCSpline->K;
	A  = pSCSpline->A;
}


Vector3d SailcutSpline::GetNormal(double const &x)
{
	double yp = K * ( A *  pow((1 - x),(AV + 1)) / (AV +1) - AR/2 * x*x + AR / 6 - ( A / ((AV + 2) * (AV + 1))));
	Vector3d N(-yp, 1.0, 0.0);
	N.Normalize();
	return N;
}


bool SailcutSpline::Serialize(QDataStream &ar, bool bIsStoring)
{
	if(bIsStoring)
	{
		ar << A << AV << AR << K;
	}
	else
	{
		ar >> A >> AV >> AR >> K;
	}
	return true;
}


void SailcutSpline::GetSlopes(double &s0, double &s1)
{
	//slope at L.E.
	s0 = atan2(K*(A/(AV+1) + AR/6.0 - A/(AV+1)/(AV+2)), 1) * 180.0/PI;

	//slope at T.E.
	s1 = atan2(K*(-AR/2 + AR/6 - A/(AV+1)/(AV+2)),      1) * 180.0/PI;
}


void SailcutSpline::jacobian(double k, double e, double v, double r, double cg[])
{
//	double pow1mevp1 = pow((1.0-e), (v+1.0));
//	double pow1mevp2 = pow((1.0-e), (v+2.0));
//	double log1me = log(1.0-e);

	cg[0] = (1.0 + v / 4.0) / (v + 1.0) + r / 6.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0);
	cg[1] = k * (1.0 / (v + 1.0) / 4.0 - (1.0 + v / 4.0) * pow(v + 1.0, -2.0) - 1.0 / (v + 2.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0));
	cg[2] = k / 6.0;
	cg[3] = 0;
	cg[4] = -r / 3.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0);
	cg[5] = k * (-1.0 / (v + 2.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0));
	cg[6] = -k / 3.0;
	cg[7] = 0;
	cg[8] = (1.0 + v / 4.0) * pow( (1 - e), v + 1.0) / (v + 1.0) - r *e*e / 2.0 + r / 6.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0);
	cg[9] = k * (pow( (1 - e), v + 1.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow( (1 - e), v + 1.0) * log( (1 - e)) / (v + 1.0) - (1.0 + v / 4.0) * pow( (1 - e), v + 1.0) * pow(v + 1.0, -2.0) - 1.0 / (v + 2.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0));
	cg[10] = k * (- (e * e) / 2.0 + 1.0 / 6.0);
	cg[11] = k * (-(1.0 + v / 4.0) * pow( (1 - e), v + 1.0) /  (1 - e) - r*e);
	cg[12] = -(1.0 + v / 4.0) * pow( (1 - e), v + 2.0) / (v + 2.0) / (v + 1.0) - r*e*e*e/ 6.0 + (r / 6.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0)) *  e + (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0);
	cg[13] = k * (-pow( (1 - e), v + 2.0) / (v + 2.0) / (v + 1.0) / 4.0 - (1.0 + v / 4.0) * pow( (1 - e), v + 2.0) * log( (1 - e)) / (v + 2.0) / (v + 1.0) + (1.0 + v / 4.0) * pow( (1 - e), v + 2.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) * pow( (1 - e), v + 2.0) / (v + 2.0) * pow(v + 1.0, -2.0) + (-1.0 / (v + 2.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0)) *  e + 1.0 / (v + 2.0) / (v + 1.0) / 4.0 - (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) - (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0));
	cg[14] = k * (- e*e*e / 6.0 +  e / 6.0);
	cg[15] = k * ((1.0 + v / 4.0) * pow( (1 - e), v + 2.0) /  (1 - e) / (v + 1.0) - r *  (e * e) / 2.0 + r / 6.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0));
}

/*
bool SailcutSpline::SplineCurve()
{
	//The sailcut type spline has 5 control points
	//
	//the first point is the L.E., at (0.0)
	//the second point defines the leading edge slope
	s0 = m_SlopePoint[0].y / m_SlopePoint[0].x;

	//the third point is the point of max camber
	e = m_CtrlPoint[1].x;
	t = m_CtrlPoint[1].y;

	//the fourth and fifth point define the trailing edge slope
	//the fift point is the T.E, at (1, 0)
	s1 = (m_CtrlPoint[2].y-m_SlopePoint[1].y) / (m_CtrlPoint[2].x-m_SlopePoint[1].x);

	//Compute K, A, AV, AK from iput data
//	ComputeAKVR2(s0, s1, t);

	return true;
}*/


#define MAXITER 5

void SailcutSpline::ComputeAKVR4(double slope0, double slope1, double c)
{
	//takes in input :
	//  s0 = the slope at the leading edge  (s1>0)
	//  s1 = the slope at the trailing edge (s1<0)
	//  c  = the max camber
	//  xc = the chord position of the max camber
	//
	// a is set at a= 1 + av/4
	//
	// in output, the parameters for the sailcut spline :  k, e, (a)v, (a)r
	//
	// uses 4 dimensional Newton-Raphson method to solve the problem
	//
	// assumes an initial state has been set and that the current input parameters
	// correspond to the sailcut spline parameters.

	static double e, v, r, k;
	static double e1,v1,r1,k1;
	static double ff0, ff1, ff2, ff3;

	static double cg[16], cgin[16];
	double dmax = 1000.0;
	double dd;
	int iter = 0;

	//use the current spline parameters as the starting point
	GetCamber(t,e);
	k1 = K;
	v1 = AV;
	r1 = AR;
	e1 = e;
	a = 1+AV/4.0;

	//set new input data
	s0 = slope0;
	s1 = slope1;
	t = c;
//	qDebug()<<"starting"<<s0<<s1<<e<<t;

	while (iter<MAXITER && dmax>1.e-3)
	{
		jacobian(k1,e1,v1,r1, cgin);
		Invert44(cgin, cg);

		ff0 = f0(k1,v1,r1,e1);
		ff1 = f1(k1,v1,r1,e1);
		ff2 = f2(k1,v1,r1,e1);
		ff3 = f3(k1,v1,r1,e1);
//		qDebug("%3d   %13.7g    %13.7g    %13.7g    %13.7g    ",iter, ff0,ff1,ff2,ff3);

		k = k1 - cg[0] *ff0 - cg[1] *ff1 - cg[2] *ff2 - cg[3] *ff3;
		e = e1 - cg[4] *ff0 - cg[5] *ff1 - cg[6] *ff2 - cg[7] *ff3;
		v = v1 - cg[8] *ff0 - cg[9] *ff1 - cg[10]*ff2 - cg[11]*ff3;
		r = r1 - cg[12]*ff0 + cg[13]*ff1 - cg[14]*ff2 - cg[15]*ff3;

		dd = qMax(fabs(k1-k), fabs(e-e1));
		dd = qMax(dd, fabs(v1-v));
		dd = qMax(dd, fabs(r1-r));
//		qDebug("%3d   %13.7g    %13.7g    %13.7g    %13.7g    %13.7g",iter, k,e,v,r,dd);

		k1=k;
		e1=e;
		v1=v;
		r1=r;

		if(dd<dmax) dmax = dd;
		iter++;
	}
//	E  = e;
	A  = 1.0 +v/4.0;//unnecessary
	K  = k;
	AV = v;
	AR = r;
//	qDebug()<<"________________";
}




void SailcutSpline::jacobian2(double e, double v, double *cg)
{
	cg[0] = -(1.0 + v / 4.0) * pow( (1 - e), v + 1.0) /  (1 - e) + 0.3e1 / 2.0 * (s0 * v + s0 + s1) * (4.0 + v) *  e / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1);

	cg[1] = pow( (1 - e), v + 1.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow( (1 - e), v + 1.0) * log( (1 - e)) / (v + 1.0) - (1.0 + v / 4.0) * pow( (1 - e), v + 1.0) * pow(v + 1.0, -2.0) + 0.3e1 / 4.0 * s0 * (4.0 + v) *  (e * e) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) + 0.3e1 / 4.0 * (s0 * v + s0 + s1) *  (e * e) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) - 0.3e1 / 4.0 * (s0 * v + s0 + s1) * (4.0 + v) *  (e * e) * pow(v + 2.0, -2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) - 0.3e1 / 4.0 * (s0 * v + s0 + s1) * (4.0 + v) *  (e * e) / (v + 2.0) * pow(s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1, -2.0) * (s0 + 2.0 * s1) - s0 * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 - (s0 * v + s0 + s1) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 + (s0 * v + s0 + s1) * (4.0 + v) * pow(v + 2.0, -2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 + (s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) * pow(s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1, -2.0) * (s0 + 2.0 * s1) / 4.0 - 1.0 / (v + 2.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0);
	cg[2] = (8.0 * s1 * v * v + 0.24e2 * s1 * v + 0.16e2 * s1 + 4.0 * s0 * v * v + 0.12e2 * s0 * v + 8.0 * s0) * ((1.0 + v / 4.0) * pow( (1 - e), v + 2.0) /  (1 - e) / (v + 1.0) + 0.3e1 / 4.0 * (s0 * v + s0 + s1) * (4.0 + v) *  (e * e) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) - (s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0)) / (2.0 * v * v + 0.9e1 * v + 4.0);

	cg[3] = (0.16e2 * s1 * v + 0.24e2 * s1 + 8.0 * s0 * v + 0.12e2 * s0) * (-(1.0 + v / 4.0) * pow( (1 - e), v + 2.0) / (v + 2.0) / (v + 1.0) + (s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) *  e*e*e / 4.0 + (-(s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0)) *  e + (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0)) / (2.0 * v * v + 0.9e1 * v + 4.0) + (8.0 * s1 * v * v + 0.24e2 * s1 * v + 0.16e2 * s1 + 4.0 * s0 * v * v + 0.12e2 * s0 * v + 8.0 * s0) * (-pow( (1 - e), v + 2.0) / (v + 2.0) / (v + 1.0) / 4.0 - (1.0 + v / 4.0) * pow( (1 - e), v + 2.0) * log( (1 - e)) / (v + 2.0) / (v + 1.0) + (1.0 + v / 4.0) * pow( (1 - e), v + 2.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) * pow( (1 - e), v + 2.0) / (v + 2.0) * pow(v + 1.0, -2.0) + s0 * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) *  e*e*e / 4.0 + (s0 * v + s0 + s1) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) *  e*e*e / 4.0 - (s0 * v + s0 + s1) * (4.0 + v) * pow(v + 2.0, -2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) *  e*e*e / 4.0 - (s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) * pow(s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1, -2.0) *  e*e*e * (s0 + 2.0 * s1) / 4.0 + (-s0 * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 - (s0 * v + s0 + s1) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 + (s0 * v + s0 + s1) * (4.0 + v) * pow(v + 2.0, -2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 + (s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) * pow(s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1, -2.0) * (s0 + 2.0 * s1) / 4.0 - 1.0 / (v + 2.0) / (v + 1.0) / 4.0 + (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) + (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0)) *  e + 1.0 / (v + 2.0) / (v + 1.0) / 4.0 - (1.0 + v / 4.0) * pow(v + 2.0, -2.0) / (v + 1.0) - (1.0 + v / 4.0) / (v + 2.0) * pow(v + 1.0, -2.0)) / (2.0 * v * v + 0.9e1 * v + 4.0) - (8.0 * s1 * v * v + 0.24e2 * s1 * v + 0.16e2 * s1 + 4.0 * s0 * v * v + 0.12e2 * s0 * v + 8.0 * s0) * (-(1.0 + v / 4.0) * pow( (1 - e), v + 2.0) / (v + 2.0) / (v + 1.0) + (s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) *  e*e*e / 4.0 + (-(s0 * v + s0 + s1) * (4.0 + v) / (v + 2.0) / (s0 * v + 2.0 * s1 * v + s0 + 2.0 * s1) / 4.0 - (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0)) *  e + (1.0 + v / 4.0) / (v + 2.0) / (v + 1.0)) * pow(2.0 * v * v + 0.9e1 * v + 4.0, -2.0) * (4.0 * v + 0.9e1);
}



void SailcutSpline::ComputeAKVR2(double slope0, double slope1, double c)
{
	//takes in input :
	//  s0 = the slope at the leading edge  (s1>0)
	//  s1 = the slope at the trailing edge (s1<0)
	//  c  = the max camber
	//
	// a is set at a= 1 + av/4, which defines implicitely the position e of max camber
	//
	// in output, the parameters for the sailcut spline :  k, (a)v, (a)r,
		//            and e = the position of max camber
	//
	// uses 2 dimensional Newton-Raphson method to solve the problem for e and (a)v
	// then calculate k and (a)r from e and (a)v
	//
	// assumes an initial state has been set and that the current input parameters
	// correspond to the sailcut spline parameters.

	double e, v, r, k;
	double e1,v1,r1,k1;
	double gg0, gg1;

	double cg[4], cgin[4];
	double dmax = 1000.0;
	double dd;
	int iter = 0;

	//Set profile family
	a = 1+AV/4.0;

	//use the current spline parameters as the starting point
	k1 = K;
	v1 = AV;
	r1 = AR;

	//Get the current position of max camber to initialize the iterations
	GetCamber(t,e);
	e1 = e;

	//set new input data
	s0 = slope0;
	s1 = slope1;
	t = c;
//qDebug()<<"starting"<<s0<<s1<<e<<t;
double relax = 10.0;
	while (iter<10 && dmax>1.e-3)
	{
		jacobian2(e1,v1,cgin);
		Invert22(cgin, cg);

		gg0 = g0(v1,e1);
		gg1 = g1(v1,e1);
//qDebug("%3d   %13.7g    %13.7g",iter, gg0,gg1);

		e = e1 - cg[0] *gg0/relax - cg[1] *gg1/relax;
		v = v1 - cg[2] *gg0/relax - cg[3] *gg1/relax;

		dd = qMax(fabs(v1-v), fabs(e-e1));
//qDebug("%3d   %13.7g    %13.7g    %13.7g    %13.7g    %13.7g",iter, t,e,v,a,dd);

		e1=e;
		v1=v;

		if(dd<dmax) dmax = dd;
		iter++;
	}

	//deduce other coefficients directly
	r = -3.0/2.0 *(s1*v + s1 + s0)*(v+4.0)                                    / (v+2.0) /(s1*v + 2.0*s0*v + s1 + 2.0*s0);
	k =  4.0     *(2.0*s0*v*v + 6*s0*v + 4.0*s0 + s1*v*v + 3.0*s1*v + 2.0*s1) / (2.0*v*v + 9.0*v + 4);

	K  = k;
	AV = v;
	AR = r;
//qDebug()<<"________________";
}


void SailcutSpline::DepthtoK(double d)
{
	//sets the parameter k to achieve depth d
	if(fabs(Depth())>0)	K *= d/Depth();
	else
	{
		K=1;
		K *= d/Depth();
	}
}

