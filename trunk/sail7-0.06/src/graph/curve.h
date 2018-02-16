/****************************************************************************

	Curve Class
	Copyright (C) 2003-2009 Andre Deperrois sail7@xflr5.com

	All rights reserved


*****************************************************************************/


#ifndef CURVE_H
#define CURVE_H

#include "../params.h"
#include "../objects/Vector3d.h"
#include <QColor>

class CCurve
{
	friend class Graph;

public:
	Vector3d GetPoint(int ref);

	bool PointsVisible();
	bool IsVisible();

	void Copy(CCurve *pCurve);
	void GetTitle(QString &string);
 	void ShowPoints(bool bShow);
	void SetVisible(bool bVisible);
	void SetColor(QColor clr);
	void SetStyle(int nStyle);
	void SetSelected(int n);
	void SetWidth(int nWidth);
	void SetTitle(QString Title);
	void GetBWStyle(QColor &color, int &style, int &width);
	void GetBWStyle();
	void ResetCurve();

	QString GetTitle();
	
	double GetxMin();
	double GetxMax();
	double GetyMin();
	double GetyMax();

	int AddPoint(double xn, double yn);
	int GetStyle();
	int GetWidth();
	int GetSelected();
	int GetCount();

	int GetClosestPoint(double xs, double ys, double &dist);
	void GetClosestPoint(double xs, double ys, double &dist, int &n);
	void GetClosestPoint(double const &xs, double const &ys, double &xSel, double &ySel, double &dist, int &nSel);

	QColor  GetColor();

	//	Curve Data
	int n;
	double x[1000];
	double y[1000];
	bool m_bShowPoints;

	CCurve();

private:	
	bool m_bIsVisible;
	QColor CurveColor;
	QString m_CurveName;
	int CurveStyle;
	int CurveWidth;
	int m_iSelected;
	void *m_pParentGraph;
};



#endif
