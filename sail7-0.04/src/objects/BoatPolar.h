/****************************************************************************

		 SailPolar Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#ifndef CBOATPOLAR_H
#define CBOATPOLAR_H

#include <QString>
#include <QColor>
#include <QTextStream>

#include "./BoatOpp.h"
#include "../objects/CVector.h"

class CBoatPolar
{
public:
	CBoatPolar();
	void AddPoint(CBoatOpp *pBOpp);
	bool SerializeBoatPlr(QDataStream &ar, bool bIsStoring);
	void *GetBoatPlrVariable(int iVar);
	void GetPolarProperties(QString &PolarProperties, bool bData=false);
	void Export(QTextStream &out, int FileType, bool bDataOnly=false);
	void ResetBoatPlr();
	void DuplicateSpec(CBoatPolar *pBoatPolar);

	double WindFactor(double z);

	static void *s_pMainFrame;
	static void* s_pSail7;

	QString m_BoatPolarName;
	QString m_BoatName;

//	int m_UnitType;//1= International, 2= English
	int m_AnalysisMethod;

//	bool m_bTiltedGeom;//true if calculation is performed on the tilted geometry, at alpha=0.0
	bool m_bGround;
	bool m_bVLM1;
	bool m_bDirichlet;
	bool m_bIsVisible,m_bShowPoints;
	bool m_bWakeRollUp;

	double m_AMem;

	double m_Density, m_Viscosity;
	double m_Height;

	double m_Area, m_Span;

	CVector m_CoG;

	QColor m_Color;
	int m_Style, m_Width;

	int m_NXWakePanels;
	double m_WakePanelFactor, m_TotalWakeLength;

	double m_WindGradient[2][2];
	double aw, bw, cw;

	//Variables
	double m_QInfMin, m_QInfMax; //wind speed (m/s)
	double m_BetaMin, m_BetaMax; //sideslip = wind angle (°)
	double m_PhiMin, m_PhiMax;   //bank angle (°)
	double m_SailAngleMin[MAXSAILS];
	double m_SailAngleMax[MAXSAILS];

	QList <double>  m_Ctrl; // The main parameter for the polar
	QList <double>  m_FFFx;    // resulting force in the far field
	QList <double>  m_FFFy;
	QList <double>  m_FFFz;
	QList <double>  m_Fx;    // resulting force from panel force Summation
	QList <double>  m_Fy;
	QList <double>  m_Fz;
	QList <double>  m_Mx;    // resulting moment from panel force Summation w.r.t. CoG
	QList <double>  m_My;
	QList <double>  m_Mz;
};

#endif // CBOATPOLAR_H
