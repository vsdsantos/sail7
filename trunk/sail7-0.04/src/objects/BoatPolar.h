/****************************************************************************

		 SailPolar Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#ifndef QBOATPOLAR_H
#define QBOATPOLAR_H

#include <QString>
#include <QColor>
#include <QTextStream>

#include "./BoatOpp.h"
#include "../objects/CVector.h"

class BoatPolar
{
	friend class MainFrame;
	friend class QSail;
	friend class QSail7;
	friend class BoatPolarDlg;
	friend class BoatAnalysisDlg;

public:
	BoatPolar();
	void AddPoint(BoatOpp *pBOpp);
	bool SerializeBoatPlr(QDataStream &ar, bool bIsStoring);
	void *GetBoatPlrVariable(enumPolarVar iVar);
	void GetPolarProperties(QString &PolarProperties, bool bData=false);
	void Export(QTextStream &out, int FileType, bool bDataOnly=false);
	void ResetBoatPlr();
	void DuplicateSpec(BoatPolar *pBoatPolar);

	static QString GetPolarVariableName(int iVar);

private:
	double WindFactor(double z);

	static void *s_pMainFrame;
	static void* s_pSail7;

	QString m_BoatPolarName;
	QString m_BoatName;

//	int m_UnitType;//1= International, 2= English
	int m_AnalysisMethod;

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
	QList <double>  m_Lift;    // resulting lift force in the far field
	QList <double>  m_Drag;    // resulting drag force in the far field
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

#endif // QBOATPOLAR_H
