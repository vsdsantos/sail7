/****************************************************************************

		 BoatAnalysisDlg Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/




#ifndef BOATANALYSISDLG_H
#define BOATANALYSISDLG_H

#include <QDialog>
#include <QProgressBar>
#include <QGLWidget>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include "../objects/BoatPolar.h"
#include "../objects/Boat.h"
#include "../objects/Panel.h"
#include "../objects/CVector.h"



class BoatAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class QSail7;
	friend class MainFrame;
	friend class CBoatDef;
	friend class CSail;

public:
	BoatAnalysisDlg();

	void InitDialog();

private slots:
	void OnCancelAnalysis();
	void OnProgress();


private:

	QTextEdit *m_pctrlTextOutput;
	QPushButton *m_pctrlCancel;
	QProgressBar *m_pctrlProgress;

	void keyPressEvent(QKeyEvent *event);

	bool Solve();
	bool UnitLoop();

	void AddString(QString strong);
	void BuildInfluenceMatrix();

	void ComputeOnBodyCp();
	void ComputeBoat();
	void ComputeSurfSpeeds(double *Mu, double *Sigma);
	void ComputeFarField();
	void CreateSourceStrength();
	void CreateRHS(double *RHS);
	void CreateWakeContribution();
	void CreateWakeContribution(double *pWakeContrib);

	void GetDoubletInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake=false, bool bAll=true);
	void GetSourceInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi);
	void GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT, bool bAll=true, bool bTrace=false);
	void SetFileHeader();
	void SourceNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi);
	void SetAngles(CBoatPolar *pBoatPolar, double Ctrl, bool bBCOnly=true);
	void SetupLayout();
	void SetWindAxis(double const Beta);
	void StartAnalysis();
	void SumPanelForces(double *Cp, CVector Force);
	void UpdateView();
	void WriteString(QString strong);
	void VLMGetVortexInfluence(CPanel *pPanel, const CVector &C, CVector &V, bool bAll);

	void GetDoubletDerivative(const int &p, double *Mu, double &Cp, CVector &VTotl, double const &QInf, double Vx, double Vy, double Vz);
	void GetVortexCp(const int &p, double *Gamma, double *Cp, CVector &VInf);

	void ComputeResults();
	void Forces(double *Mu, double *Sigma, double alpha, double *VInf, CVector &Force, CVector &Moment, bool bTilted, bool bTrace=false);


	static void *s_pSail7;
	static void *s_pMainFrame;
	static CPanel *s_pPanel; //the original array of panels
	static CPanel *s_pWakePanel;// the current working wake panel array
	static CPanel *s_pRefWakePanel;// a copy of the reference wake node array if wake needs to be reset
	static CPanel *s_pMemPanel;// a copy of the reference panel array for tilted calc

	static CVector *s_pNode;	// the working array of Nodes
	static CVector *s_pMemNode;	// a copy of the reference node array for tilted calc
	static CVector *s_pWakeNode;	// the current working wake node array
	static CVector *s_pRefWakeNode; // a copy of the reference wake node array if wake needs to be reset

	static double *s_aij, *s_aijWake;
	static double *s_RHS, *s_RHSRef;

	QFile *m_pXFile;

	bool m_bIsFinished;
	bool m_bXFile;
//	bool m_bDirichlet;// true if Dirichlet boundary conditions, false if Neumann
	bool m_bTrefftz;
	bool m_bSequence;
	bool m_bSkip, m_bExit, m_bCancel, m_bWarning;
	bool m_bWakeRollUp;

	int m_State;
	int m_nNodes;
	int m_MatSize;
	int m_nWakeNodes;
	int m_WakeSize;	
	int m_NWakeColumn;
	int m_WakeInterNodes;
	int m_MaxWakeIter;

	double m_Progress;

	CVector m_VInf;
	CVector m_WindDirection, m_WindNormal, m_WindSide;
	double m_Ctrl, m_QInf, m_Beta, m_Phi; // the parameters for  the current iteration
	double m_ControlMin, m_ControlMax, m_ControlDelta;

	double eps;

	double *m_pCoreSize;

	double m_Sigma[VLMMAXMATSIZE*VLMMAXRHS];		// Source strengths
	double m_Mu[VLMMAXMATSIZE*VLMMAXRHS];			// Doublet strengths, or vortex circulations if panel is located on a thin surface
	double m_Cp[VLMMAXMATSIZE*VLMMAXRHS];			// lift coef per panel

	CVector m_VMuDerivative[VLMMAXMATSIZE];
	double m_RHS[VLMMAXMATSIZE];
	double m_uWake[VLMMAXMATSIZE], m_wWake[VLMMAXMATSIZE];

	int m_Index[VLMMAXMATSIZE];

	CVector m_Speed[VLMMAXMATSIZE];

	CVector *m_pR[5];

	QString m_strOut;
	QString m_VersionName;
	
	
	CBoatPolar *m_pBoatPolar;
	CBoat *m_pBoat;

	//temp data
	CVector VG, CG;
	double phiG;
	CPanel m_SymPanel;
	CVector R[5];
	CVector r0, r1, r2, Psi, t, Far;
	double r1v,r2v,ftmp, Omega;
	
public:
	CVector m_Vd[ 4*VLMMAXRHS * MAXSAILSTATIONS];
	CVector m_SailForce[4*VLMMAXRHS];
//	CSail *m_pSail; //pointer to the geometry class of the Sail
	CSail* m_pSailList[MAXSAILS]; //pointer to the boat's sails

};

#endif // BOATANALYSISDLG_H














