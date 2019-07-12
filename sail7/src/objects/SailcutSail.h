/****************************************************************************

		 SailcutSail Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/



#ifndef SAILCUTSAIL_H
#define SAILCUTSAIL_H

#include <QDomNode>
#include "Sail.h"

class SailcutSail : public Sail
{
public:
	SailcutSail();

	bool SerializeSail(QDataStream &ar, bool bIsStoring);

	Vector3d GetPoint(double xrel, double zrel);
	Vector3d GetSectionPoint(int iSection, double xrel);

	Vector3d SectionLE(int iSection);
	Vector3d SectionTE(int iSection);

	void SplineSurface();

	bool Import(QFile *pFile);
	void Export();
	void Duplicate(Sail *pSail);
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
