/****************************************************************************

		 CSail Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#ifndef SAIL_H
#define SAIL_H

#include "SailSection.h"
#include "./BoatPolar.h"
#include "./Panel.h"
#include <QList>
#include <QFile>


typedef enum {NURBSSAIL, SAILCUTSAIL} enumSailType;// the type of sail definition


class QSail
{
	friend class MainFrame;
	friend class QSail7;
	friend class CBoat;
	friend class SailDlg;
	friend class SailViewWidget;
	friend class BoatDlg;
	friend class BoatPolarDlg;
	friend class BoatAnalysisDlg;
	friend class BoatPolar;
	friend class BoatOpp;
	friend class SectionViewWidget;
	friend class NURBSSail;
	friend class SailcutSail;
	friend class SailDomDoc;

public:
	QSail();

	bool IsSailPanel(int nPanel);
	bool IsSailNode(int nNode);
	bool IsSailcutSail() {return m_SailType==SAILCUTSAIL;};
	bool IsNURBSSail()  {return m_SailType==NURBSSAIL;};

	void PanelTrefftz(CVector VInf, double *Mu, double *Sigma, int pos, CVector &FFForce, BoatPolar *pBoatPolar);
	void PanelComputeOnBody(CVector VInf, double *Cp, double *Gamma, CVector &F, CVector &M, BoatPolar *pBoatPolar, CVector CoG);
	void SortSections();
	void SetLuffAngle(double Angle);
	void SetLuffAngle();
	virtual void ScaleSail(double XFactor, double YFactor, double ZFactor)=0;

	virtual void Duplicate(QSail *pSail)=0;
	virtual bool Import(QFile *pFile)=0;
	virtual void Export()=0;

	virtual double LuffLength()=0;
	virtual double LeechLength()=0;
	virtual double FootLength()=0;
	virtual double Chord(double zrel)=0;

	virtual CVector GetPoint(double xrel, double zrel)=0;
	virtual CVector GetSectionPoint(int iSection, double xrel)=0;
	virtual void SplineSurface() = 0;
	virtual bool SerializeSail(QDataStream &ar, bool bIsStoring)=0;

	virtual void CreateSection(int iSection);
	virtual void InsertSection(SailSection *pSection);
	virtual void DeleteSection(int iSection);

// Variables
private:
	QString m_SailName;
	QString m_SailDescription;//non compulsory free description
	QColor m_SailColor;

	enumSailType m_SailType;

	//The variables defining the sail

	//position wrt absolute coordinates
	CVector m_LEPosition;

	//relative position of corner points in the sail's referential
	CVector tack, head, clew, peak;

	//Global geometry
	double m_LuffAngle;

	//sections
	QList <SailSection*> m_oaSection;


	/** Variables required for VLM analysis*/
	int m_NElements, m_FirstPanel, m_NStation;
	int m_NXPanels; 		// VLM Panels along horizontal direction
	int m_NZPanels; 		// VLM Panels along vertical direction
//	enumPanelDistrib m_XPanelDist;		// VLM Panel distribution type, along horizontal direction

//	CVector m_SailNormal;

	CVector m_Vd[MAXSAILSTATIONS];		// downwash vector at span stations

	CPanel *m_pPanel;			//pointer to the VLM Panel array
	static void *s_pBoatAnalysisDlg;
	static void *s_pMainFrame;
};

#endif // SAIL_H
