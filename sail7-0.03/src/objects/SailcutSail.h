/****************************************************************************

		 SailcutSail Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#ifndef SAILCUTSAIL_H
#define SAILCUTSAIL_H

#include <QDomNode>
#include "Sail.h"

class SailcutSail : public CSail
{
public:
	SailcutSail();

	bool SerializeSail(QDataStream &ar, bool bIsStoring);

	CVector GetPoint(double xrel, double zrel);
	CVector GetSectionPoint(int iSection, double xrel);

	CVector SectionLE(int iSection);
	CVector SectionTE(int iSection);

	void SplineSurface();

	bool Import(QFile *pFile);
	void Export();
	void Duplicate(CSail *pSail);
	void ScaleSail(double XFactor, double YFactor, double ZFactor);

	void SetLeechRoundEquation();

	void ReadNodes(QDomNode node);

	double Chord(double zrel);
	double Chord(SailSection *pSection);

	double Twist(double zrel);
	double Twist(SailSection *pSection);

	double Angle(double zrel);
	double Angle(SailSection *pSection);

	double zrel(double z);
	double LuffLength();
	double LeechLength();
	double FootLength();

	double m_LeechRound, m_LeechRoundPos;

	double m_Twist; //gaff twist w.r.t. foot

	double arl,brl,crl;//coefficients of the parabola describing the lower part of the leech round
	double aru,bru,cru;//coefficients of the parabola describing the upper part of the leech round

};

#endif // SAILCUTSAIL_H
