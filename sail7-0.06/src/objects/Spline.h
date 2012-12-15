/****************************************************************************

		 CSpline Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

//Base class for Cubic Splines and B-Splines

#ifndef CSPLINE_H
#define CSPLINE_H

#include <QList>
#include <QColor>
#include <QPainter>
#include <QPoint>
#include "CVector.h"

#define SPLINEOUTPUTRES  50
#define SPLINECONTROLSIZE 30

typedef enum { BSPLINE, BEZIERSPLINE, CUBICSPLINE, ARCSPLINE, SAILCUTSPLINE, POINTSPLINE, NACA4SPLINE } enumSplineType;


class Spline
{
public:
	Spline();

	virtual void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset) = 0;
	virtual void Duplicate(void *pSpline) = 0;
	virtual void GetCamber(double &Camber, double &xc) = 0;
	virtual double GetY(double const &x) = 0;
	virtual CVector GetNormal(double const &x) = 0;
	virtual void GetSlopes(double &s0, double &s1) = 0;
	virtual void SplineKnots(){};//re-implemented in some subclasses
	virtual bool SplineCurve(){return true;};//re-implemented in some subclasses
	virtual bool Serialize(QDataStream &ar, bool bIsStoring) = 0;

	void CopyPoints(QList<CVector> *m_pPointList);
	void DrawCtrlPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void DrawSlopePoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);

	bool InsertPoint(double const &x, double const &y);
	bool RemovePoint(int const &k);

	int IsControlPoint(CVector const &Real, double const &ZoomFactor);
	int IsSlopePoint(CVector const &Real, double const &ZoomFactor);

	bool IsSplineType(enumSplineType type){return m_SplineType==type;};
	QString SplineType();
	QString SplineType(int iType);


	void SetStyle(int style){m_Style = style;}
	void SetWidth(int width){m_Width = width;}
	void SetColor(QColor color){m_SplineColor = color;}
	void SetSplineStyle(int style, int width, QColor color)
	{
		m_Width = width;
		m_Style = style;
		m_SplineColor = color;
	}

	enumSplineType m_SplineType;

	QList <CVector> m_CtrlPoint;
	CVector m_SlopePoint[2];
	QColor m_SplineColor;
	int m_Style, m_Width;
	int m_iMinPoints, m_iMaxPoints;

	bool m_bShowSlope;
	static bool s_bChanged;
};

#endif // CSPLINE_H
