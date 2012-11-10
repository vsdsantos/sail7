/****************************************************************************

		 NURBSSail Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#ifndef NURBSSAIL_H
#define NURBSSAIL_H

#include "Sail.h"
#include "NURBSSurface.h"

class NURBSSail : public CSail
{
public:
	NURBSSail();

	bool SerializeSail(QDataStream &ar, bool bIsStoring);

	CVector GetPoint(double xrel, double zrel);
	CVector GetSectionPoint(int iSection, double xrel);
	CVector GetSectionPoint(SailSection *pSailSection, double xrel);

	void CreateSection(int iSection);
	void DeleteSection(int iSection);

	void Duplicate(CSail *pSail);
	bool Import(QFile *pFile);
	void Export();
	void ScaleSail(double XFactor, double YFactor, double ZFactor);

	void SplineSurface();

	double LuffLength();
	double LeechLength();
	double FootLength();
	double Chord(double zrel);


	void DrawFrame(int iSection, QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset, bool bSelected=false);

	NURBSSurface m_SplineSurface;

	bool m_bAlignedLE;
};

#endif // NURBSSAIL_H
