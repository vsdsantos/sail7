/****************************************************************************

		 SailSection Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/


#ifndef SAILSECTION_H
#define SAILSECTION_H

#include "SailcutSpline.h"
#include "Frame.h"


class SailSection : public CFrame
{
public:
	SailSection(void *pParent);

	void DrawSpline(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset, bool bSelected=true);
	void DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint const &Offset);

	void MoveTo(Vector3d NewPosition);
	void Copy(SailSection *pSection);
	bool SerializeSection(QDataStream &ar, bool bIsStoring);
	void SetSection(double const &span, double const& chord, double const&twist, double const&angle);
	void SetSailcutSpline(double k, double av, double ar);

//	CVector GetPoint(double xrel);

	void GetSlopes(double &s0, double &s1);
	void GetCamber(double &c, double &x);

	void SetSpanPosition(double z);

	bool IsTipSection();

	double Chord();
	double Twist();

	bool IsNURBSSection();
	bool IsSailcutSection();

	void *m_pSail;

	SailcutSpline m_SCSpline;

	//all lengths in meters
	//all angles in degrees
	bool m_bIsTipSection;

	double m_Chord;
	double m_Angle; //tilt of the section around the y-axis
	QColor m_SectionColor;

	double m_c, m_x;//camber value and position for a NURBS type section
};

#endif // SAILSECTION_H
