/****************************************************************************

		 BoatAnalysisDlg Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include <QtDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QTimer>
#include <QDir>
#include <math.h>

#include "BoatAnalysisDlg.h"
#include "../mainframe.h"
#include "../globals.h"
#include "../objects/CVector.h"
#include "sail7.h"


void *BoatAnalysisDlg::s_pSail7 = NULL;
void *BoatAnalysisDlg::s_pMainFrame = NULL;

CPanel *BoatAnalysisDlg::s_pPanel = NULL; //the original array of panels
CPanel *BoatAnalysisDlg::s_pWakePanel = NULL;// the current working wake panel array
CPanel *BoatAnalysisDlg::s_pRefWakePanel = NULL;// a copy of the reference wake node array if wake needs to be reset
CPanel *BoatAnalysisDlg::s_pMemPanel = NULL;// a copy of the reference panel array for tilted calc

CVector *BoatAnalysisDlg::s_pNode = NULL;	// the working array of Nodes
CVector *BoatAnalysisDlg::s_pMemNode = NULL;	// a copy of the reference node array for tilted calc
CVector *BoatAnalysisDlg::s_pWakeNode = NULL;	// the current working wake node array
CVector *BoatAnalysisDlg::s_pRefWakeNode = NULL; // a copy of the reference wake node array if wake needs to be reset

double *BoatAnalysisDlg::s_aij = NULL;
double *BoatAnalysisDlg::s_aijWake = NULL;
double *BoatAnalysisDlg::s_RHS = NULL;
double *BoatAnalysisDlg::s_RHSRef = NULL;



BoatAnalysisDlg::BoatAnalysisDlg()
{
	setWindowTitle(tr("3D Panel Analysis"));

	SetupLayout();

	m_bSequence      = false;
	m_bIsFinished    = false;
	m_bSequence      = false;
	m_bWarning       = false;
	m_bXFile         = false;
	m_bCancel        = false;
	m_bTrefftz       = false;

	m_MatSize = m_nNodes = 0;

	m_Ctrl = 0.0;
	m_ControlMin  = m_ControlMax = m_ControlDelta  = 0.0;

	m_MatSize        = 0;
	m_nNodes         = 0;

	m_NWakeColumn    = 0;
	m_WakeInterNodes = 1;
	m_MaxWakeIter    = 0;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;
	m_strOut = "";

	for(int iw=0; iw<MAXSAILS; iw++) m_pSailList[iw] = NULL;

	m_pBoatPolar = NULL;
	m_pBoat  = NULL;

	memset(m_Sigma,  0, sizeof(m_Sigma));
	memset(m_Mu,     0, sizeof(m_Mu));
	memset(m_Cp,     0, sizeof(m_Cp));
	memset(m_Speed,  0, sizeof(m_Speed));

	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_RHS, 0, sizeof(m_RHS));

	memset(m_VMuDerivative, 0, sizeof(m_VMuDerivative));
}



void BoatAnalysisDlg::AddString(QString strong)
{
	m_pctrlTextOutput->insertPlainText(strong);
	m_pctrlTextOutput->ensureCursorVisible();
	WriteString(strong);
}



void BoatAnalysisDlg::BuildInfluenceMatrix()
{
	CVector C, V;
	int m, mm, p, pp;
	double phi;

	AddString("      Creating the influence matrix...\n");

	m=0;
	for(p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;

		//for each Boundary Condition point
		if(s_pPanel[p].m_Pos!=MIDSURFACE)
		{
			//Thick surfaces, 3D-panel type BC, use collocation point
			C = s_pPanel[p].CollPt;
		}
		else
		{
			//Thin surface, VLM type BC, use control point
			C = s_pPanel[p].CtrlPt;
		}

		mm = 0;
		for(pp=0; pp<m_MatSize; pp++)
		{
			if(m_bCancel) return;

			//for each panel, get the unit doublet or vortex influence at the boundary condition pt
			GetDoubletInfluence(C, s_pPanel+pp, V, phi);
			if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE) s_aij[m*m_MatSize+mm] = V.dot(s_pPanel[p].Normal);
			else if(m_pBoatPolar->m_bDirichlet)                               s_aij[m*m_MatSize+mm] = phi;

			mm++;
		}
		m++;

		m_Progress += 10.0*(double)m_MatSize/400./(double)m_MatSize;
		qApp->processEvents();
	}
}


void BoatAnalysisDlg::CreateSourceStrength()
{
	// Creates the RHS of the linear problem, using boundary conditions
	// BC may be of the Neumann or Dirichlet type depending on the analysis type and on the geometry
	//
	// if 3d-panel, use NASA 4023 equation (20) & (22)
	// 
	// Compute with a unit speed
	// The results are scaled to speed later depending on the polar type
	//
	
	AddString(tr("      Creating source strengths...")+"\n");

	for (int p=0; p< m_MatSize; p++)
	{
		if(m_bCancel) return;

		if(s_pPanel[p].m_Pos!=MIDSURFACE) m_Sigma[p] = -1.0/4.0/PI* m_VInf.dot(s_pPanel[p].Normal) * m_pBoatPolar->WindFactor(s_pPanel[p].CollPt.z);
		else                              m_Sigma[p] =  0.0;
	}
}



void BoatAnalysisDlg::CreateRHS(double *RHS)
{
	//Creates the RHS vector for
	//  free stream velocity VInf,
	//	rotation Angle around vector Omega
	int m, p, pp;
	double  phi, sigmapp;
	CVector V, C, VPanel;

	m = 0;

	for (p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;

		if(s_pPanel[p].m_Pos!=MIDSURFACE) C = s_pPanel[p].CollPt;
		else                              C = s_pPanel[p].CtrlPt;

		VPanel = m_VInf* m_pBoatPolar->WindFactor(C.z);

		if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
		{
			// first term of RHS is -V.n
			RHS[m] = -s_pPanel[p].Normal.dot(VPanel) ;
		}
		else if(m_pBoatPolar->m_bDirichlet) RHS[m] = 0.0;

		for (pp=0; pp<m_MatSize; pp++)
		{
			// Consider only the panels positioned on thick surfaces,
			// since the source strength is zero on thin surfaces
			if(s_pPanel[pp].m_Pos!=MIDSURFACE)
			{
				// Define the source strength on panel pp
				sigmapp = -1.0/4.0/PI * s_pPanel[pp].Normal.dot(VPanel);

				// Add to RHS the source influence of panel pp on panel p
				GetSourceInfluence(C, s_pPanel+pp, V, phi);

				if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
				{
					// Apply Neumann B.C.
					// NASA4023 eq. (22) and (23)
					// The RHS term is sigma[pp]*DJK = nj.Vjk
					RHS[m] -= V.dot(s_pPanel[p].Normal) * sigmapp;
				}
				else if(m_pBoatPolar->m_bDirichlet)
				{
					//NASA4023 eq. (20)
					RHS[m] -= (phi * sigmapp);
				}
			}
		}
		m++;

		m_Progress += 10.0/(double)m_MatSize;
		qApp->processEvents();
	}
}



void BoatAnalysisDlg::CreateWakeContribution()
{
	//______________________________________________________________________________________
	// Method :
	// 	- follow the method described in NASA 4023 eq. (44)
	//	- add the wake's doublet contribution to the matrix
	//	- add the difference in potential at the trailing edge panels to the RHS
	//______________________________________________________________________________________
	//
	int kw, lw, pw, p, pp;

	CVector V, C, TrPt;
	double phi;
	double PHC[MAXSAILSTATIONS];
	CVector VHC[MAXSAILSTATIONS];

	AddString(tr("      Adding the wake's contribution...")+"\n");

	int m, mm;
	m = mm = 0;

	for(p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;
		m_uWake[m] = m_wWake[m] = 0.0;
		C    = s_pPanel[p].CollPt;

		//____________________________________________________________________________
		//build the contributions of each wake column at point C
		//we have m_NWakeColum to consider
		pw=0;
		for (kw=0; kw<m_NWakeColumn; kw++)
		{
			PHC[kw] = 0.0;
			VHC[kw].Set(0.0,0.0,0.0);
			//each wake column has m_NXWakePanels
			for(lw=0; lw<m_pBoatPolar->m_NXWakePanels; lw++)
			{
				GetDoubletInfluence(C, s_pWakePanel+pw, V, phi, true, true);

				PHC[kw] += phi;
				VHC[kw] += V;

				pw++;
			}
		}
		//____________________________________________________________________________
		//Add the contributions of trailing panels to the matrix coefficients and to the RHS
		mm = 0;
		for(pp=0; pp<m_MatSize; pp++) //for each matrix column
		{
			if(m_bCancel) return;
			s_aijWake[m*m_MatSize+mm] = 0.0;
			// Is the panel pp shedding a wake ?
			if(s_pPanel[pp].m_bIsTrailing)
			{
				// If so, we need to add the contributions of the wake column
				// shedded by this panel to the RHS and to the Matrix
				// Get trailing point where the jup in potential is evaluated v6.02
				TrPt = (s_pNode[s_pPanel[pp].m_iTA] + s_pNode[s_pPanel[pp].m_iTB])/2.0;

				if(s_pPanel[pp].m_Pos==MIDSURFACE)
				{
					//The panel shedding a wake is on a thin surface
					if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
					{
						//then add the velocity contribution of the wake column to the matrix coefficient
						s_aijWake[m*m_MatSize+mm] += VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
						//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
					}
					else if(m_pBoatPolar->m_bDirichlet)
					{
						//then add the potential contribution of the wake column to the matrix coefficient
						s_aijWake[m*m_MatSize+mm] += PHC[s_pPanel[pp].m_iWakeColumn];
						//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
					}
				}
				else if(s_pPanel[pp].m_Pos==BOTSURFACE)
				{
					//the panel sedding a wake is on the bottom side, substract
					if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
					{
						//use Neumann B.C.
						s_aijWake[m*m_MatSize+mm] -= VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
						//corrected in v6.02;
						m_uWake[m] -= TrPt.x  * VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
						m_wWake[m] -= TrPt.z  * VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
					}
					else if(m_pBoatPolar->m_bDirichlet)
					{
						s_aijWake[m*m_MatSize+mm] -= PHC[s_pPanel[pp].m_iWakeColumn];
						m_uWake[m] +=  TrPt.x * PHC[s_pPanel[pp].m_iWakeColumn];
						m_wWake[m] +=  TrPt.z * PHC[s_pPanel[pp].m_iWakeColumn];
					}
				}
				else if(s_pPanel[pp].m_Pos==TOPSURFACE)
				{
					//the panel sedding a wake is on the top side, add
					if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
					{
						//use Neumann B.C.
						s_aijWake[m*m_MatSize+mm] += VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
						//corrected in v6.02;
						m_uWake[m] += TrPt.x * VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
						m_wWake[m] += TrPt.z * VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
					}
					else if(m_pBoatPolar->m_bDirichlet)
					{
						s_aijWake[m*m_MatSize+mm] += PHC[s_pPanel[pp].m_iWakeColumn];
						m_uWake[m] -= TrPt.x * PHC[s_pPanel[pp].m_iWakeColumn];
						m_wWake[m] -= TrPt.z * PHC[s_pPanel[pp].m_iWakeColumn];
					}
				}
			}
			mm++;

		}
		m++;
		m_Progress += 1.0/(double)m_MatSize;
		qApp->processEvents();
	}
}


void BoatAnalysisDlg::CreateWakeContribution(double *pWakeContrib)
{
	//______________________________________________________________________________________
	// Method :
	// 	- follow the method described in NASA 4023 eq. (44)
	//	- add the wake's doublet contribution to the matrix
	//	- add the potential difference at the trailing edge panels to the RHS ; the potential's origin
	//    is set arbitrarily to the geometrical orgin so that phi = V.dot(WindDirectio) x point_position
	//______________________________________________________________________________________
	//
	int kw, lw, pw, p, pp;

	static CVector V, C, TrPt;
	double phi;
	double PHC[MAXSAILSTATIONS];
	CVector VHC[MAXSAILSTATIONS];

	AddString(tr("      Adding the wake's contribution...")+"\n");

	int m, mm;
	m = mm = 0;

	for(p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;

		pWakeContrib[m] = 0.0;
		C    = s_pPanel[p].CollPt;


		//____________________________________________________________________________
		//build the contributions of each wake column at point C
		//we have m_NWakeColum to consider
		pw=0;
		for (kw=0; kw<m_NWakeColumn; kw++)
		{
			PHC[kw] = 0.0;
			VHC[kw].Set(0.0,0.0,0.0);
			//each wake column has m_NXWakePanels
			for(lw=0; lw<m_pBoatPolar->m_NXWakePanels; lw++)
			{
				GetDoubletInfluence(C, s_pWakePanel+pw, V, phi, true, true);

				PHC[kw] += phi;
				VHC[kw] += V;

				pw++;
			}
		}
		//____________________________________________________________________________
		//Add the contributions to the matrix coefficients and to the RHS
		mm = 0;
		for(pp=0; pp<m_MatSize; pp++) //for each matrix column
		{
			if(m_bCancel) return;

			// Is the panel pp shedding a wake ?
			if(s_pPanel[pp].m_bIsTrailing)
			{
				// Get trailing point where the jup in potential is evaluated
				TrPt = (s_pNode[s_pPanel[pp].m_iTA] + s_pNode[s_pPanel[pp].m_iTB])/2.0;
				// If so, we need to add the contributions of the wake column
				// shedded by this panel to the RHS and to the Matrix
				if(s_pPanel[pp].m_Pos==MIDSURFACE)
				{
					//The panel shedding a wake is on a thin surface
					if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
					{
						//then add the velocity contribution of the wake column to the matrix coefficient
						//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
					}
					else if(m_pBoatPolar->m_bDirichlet)
					{
						//then add the potential contribution of the wake column to the matrix coefficient
						//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
					}
				}
				else if(s_pPanel[pp].m_Pos==BOTSURFACE)//bottom side, substract
				{
					//evaluate the potential on the bottom side panel pp which is shedding a wake
					if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
					{
						//use Neumann B.C.
						pWakeContrib[m] -= TrPt.dot(m_WindDirection) * VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
					}
					else if(m_pBoatPolar->m_bDirichlet)
					{
						pWakeContrib[m] += TrPt.dot(m_WindDirection) * PHC[s_pPanel[pp].m_iWakeColumn];
					}
				}
				else if(s_pPanel[pp].m_Pos==TOPSURFACE)  //top side, add
				{
					//evaluate the potential on the top side panel pp which is shedding a wake
					if(!m_pBoatPolar->m_bDirichlet || s_pPanel[p].m_Pos==MIDSURFACE)
					{
						//use Neumann B.C.
						pWakeContrib[m] += TrPt.dot(m_WindDirection) * VHC[s_pPanel[pp].m_iWakeColumn].dot(s_pPanel[p].Normal);
					}
					else if(m_pBoatPolar->m_bDirichlet)
					{
						pWakeContrib[m] -= TrPt.dot(m_WindDirection) * PHC[s_pPanel[pp].m_iWakeColumn];
					}
				}
			}
			mm++;
		}
		m++;

		m_Progress += 1.0/(double)m_MatSize;
		qApp->processEvents();
	}
}


void BoatAnalysisDlg::ComputeFarField()
{
	int pos;
	CVector SailForce;

	AddString(tr("      Calculating aerodynamic coefficients in the far field plane")+"\n");

	pos = 0;


	for(int is=0; is<MAXSAILS; is++)
	{
		if(m_pSailList[is])
		{
			m_pSailList[is]->PanelTrefftz(m_VInf, m_Mu, m_Sigma, pos, SailForce, m_pBoatPolar);

			//save the results... will save another FF calculation when computing operating point
			m_SailForce[is] = SailForce;

			memcpy(m_Vd  + is*MAXSAILSTATIONS, m_pSailList[is]->m_Vd,  m_pSailList[is]->m_NStation*sizeof(CVector));

			pos += m_pSailList[is]->m_NElements;

			m_Progress += 1.0/400.;
			qApp->processEvents();
			if(m_bCancel)return;
		}
	}
}



void BoatAnalysisDlg::ComputeBoat()
{
	// calculates the viscous and inviscid wing coefficients 
	QSail7 *pSail7 = (QSail7*)s_pSail7;
	int pos;

	CVector ForceTrefftz;
	CVector SailForce, SailMoment, F,M;

	QString strong;
	strong = QString("       Calculating point %1\n").arg(m_Ctrl,7,'f',2);
	AddString(strong);


	AddString(tr("         Calculating aerodynamic coefficients...")+"\n");
	ForceTrefftz.Set(0.0, 0.0, 0.0);

	pos = 0;


	for(int iw=0; iw<m_pBoat->m_poaSail.size(); iw++)
	{
		if(m_pSailList[iw])
		{
			AddString(tr("         Calculating sail: ") + m_pSailList[iw]->m_SailName+"\n");

			//restore the saved FF results
			ForceTrefftz += m_SailForce[iw];
			memcpy(m_pSailList[iw]->m_Vd,  m_Vd  + iw*MAXSAILSTATIONS, m_pSailList[iw]->m_NStation*sizeof(CVector));

			//Compute moment coefficients
			SailForce.Set(0.0,0.0,0.0);
			SailMoment.Set(0.0,0.0,0.0);
			m_pSailList[iw]->PanelComputeOnBody(m_VInf, m_Cp+pos, m_Mu+pos,
												SailForce, SailMoment,
												m_pBoatPolar, m_pBoatPolar->m_CoG);

			F += SailForce;
			M += SailMoment;

			pos += m_pSailList[iw]->m_NElements;
		}
	}

	if(m_pBoat) pSail7->AddBoatOpp(m_Cp, m_Mu, m_Sigma, F, M, ForceTrefftz);
	AddString("\n");

	if(pSail7->m_iView==SAILPOLARVIEW)
	{
		pSail7->CreateBoatPolarCurves();
		pSail7->UpdateView();
	}

	qApp->processEvents();
}


void BoatAnalysisDlg::GetDoubletDerivative(const int &p, double *Mu, double &Cp, CVector &VLocal, double const &QInf, double Vx, double Vy, double Vz)
{
	static int PL,PR, PU, PD;
	static double DELQ, DELP, mu0,mu1,mu2, x0,x1,x2, Speed2;
	static CVector VTot;//total local panel speed
	static CVector S2, Sl2;

	PL = s_pPanel[p].m_iPL;
	PR = s_pPanel[p].m_iPR;
	PU = s_pPanel[p].m_iPU;
	PD = s_pPanel[p].m_iPD;

	if(PL>=0 && PR>=0)
	{
		//we have two side neighbours
		x1  = 0.0;
		x0  = x1 - s_pPanel[p].SMQ - s_pPanel[PL].SMQ;
		x2  = x1 + s_pPanel[p].SMQ + s_pPanel[PR].SMQ;
		mu0 = Mu[PL];
		mu1 = Mu[p];
		mu2 = Mu[PR];
		DELQ =	  mu0 *(x1-x2)       /(x0-x1)/(x0-x2)
				+ mu1 *(2.0*x1-x0-x2)/(x1-x0)/(x1-x2)
				+ mu2 *(x1-x0)       /(x2-x0)/(x2-x1);
	}
	else if(PL>=0 && PR<0)
	{
		// no right neighbour
		// do we have two left neighbours ?
		if(s_pPanel[PL].m_iPL>=0){
			x2  = 0.0;
			x1  = x2 - s_pPanel[p].SMQ  - s_pPanel[PL].SMQ;
			x0  = x1 - s_pPanel[PL].SMQ - s_pPanel[s_pPanel[PL].m_iPL].SMQ;

			mu0 = Mu[s_pPanel[PL].m_iPL];
			mu1 = Mu[PL];
			mu2 = Mu[p];
			DELQ =	  mu0 *(x2-x1)       /(x0-x1)/(x0-x2)
					+ mu1 *(x2-x0)       /(x1-x0)/(x1-x2)
					+ mu2 *(2.0*x2-x0-x1)/(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELQ = -(Mu[PL]-Mu[p])/(s_pPanel[p].SMQ  + s_pPanel[PL].SMQ);
		}
	}
	else if(PL<0 && PR>=0)
	{
		// no left neighbour
		// do we have two right neighbours ?
		if(s_pPanel[PR].m_iPR>=0){
			x0  = 0.0;
			x1  = x0 + s_pPanel[p].SMQ  + s_pPanel[PR].SMQ;
			x2  = x1 + s_pPanel[PR].SMQ + s_pPanel[s_pPanel[PR].m_iPR].SMQ;
			mu0 = Mu[p];
			mu1 = Mu[PR];
			mu2 = Mu[s_pPanel[PR].m_iPR];
			DELQ =	  mu0 *(2.0*x0-x1-x2)/(x0-x1)/(x0-x2)
					+ mu1 *(x0-x2)       /(x1-x0)/(x1-x2)
					+ mu2 *(x0-x1)       /(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELQ = (Mu[PR]-Mu[p])/(s_pPanel[p].SMQ  + s_pPanel[PR].SMQ);
		}
	}
	else
	{
		DELQ = 0.0;
		//Cannot calculate a derivative on one panel only
	}

	if(PU>=0 && PD>=0)
	{
		//we have one upstream and one downstream neighbour
		x1  = 0.0;
		x0  = x1 - s_pPanel[p].SMP - s_pPanel[PU].SMP;
		x2  = x1 + s_pPanel[p].SMP + s_pPanel[PD].SMP;
		mu0 = Mu[PU];
		mu1 = Mu[p];
		mu2 = Mu[PD];
		DELP =	  mu0 *(x1-x2)       /(x0-x1)/(x0-x2)
				+ mu1 *(2.0*x1-x0-x2)/(x1-x0)/(x1-x2)
				+ mu2 *(x1-x0)       /(x2-x0)/(x2-x1);
	}
	else if(PU>=0 && PD<0)
	{
		// no downstream neighbour
		// do we have two upstream neighbours ?
		if(s_pPanel[PU].m_iPU>=0)
		{
			x2  = 0.0;
			x1  = x2 - s_pPanel[p ].SMP  - s_pPanel[PU].SMP;
			x0  = x1 - s_pPanel[PU].SMP  - s_pPanel[s_pPanel[PU].m_iPU].SMP;
			mu0 = Mu[s_pPanel[PU].m_iPU];
			mu1 = Mu[PU];
			mu2 = Mu[p];
			DELP =	  mu0 *(x2-x1)       /(x0-x1)/(x0-x2)
					+ mu1 *(x2-x0)       /(x1-x0)/(x1-x2)
					+ mu2 *(2.0*x2-x0-x1)/(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELP = -(Mu[PU]-Mu[p])/(s_pPanel[p].SMP  + s_pPanel[PU].SMP);
		}
	}
	else if(PU<0 && PD>=0)
	{
		// no upstream neighbour
		// do we have two downstream neighbours ?
		if(s_pPanel[PD].m_iPD>=0)
		{
			x0  = 0.0;
			x1  = x0 + s_pPanel[p].SMP  + s_pPanel[PD].SMP;
			x2  = x1 + s_pPanel[PD].SMP + s_pPanel[s_pPanel[PD].m_iPD].SMP;
			mu0 = Mu[p];
			mu1 = Mu[PD];
			mu2 = Mu[s_pPanel[PD].m_iPD];
			DELP =	  mu0 *(2.0*x0-x1-x2)/(x0-x1)/(x0-x2)
					+ mu1 *(x0-x2)       /(x1-x0)/(x1-x2)
					+ mu2 *(x0-x1)       /(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELP = (Mu[PD]-Mu[p])/(s_pPanel[p].SMP  + s_pPanel[PD].SMP);
		}
	}
	else
	{
		DELP = 0.0;
	}

	//find middle of side 2
	S2 = (s_pNode[s_pPanel[p].m_iTA] + s_pNode[s_pPanel[p].m_iTB])/2.0 - s_pPanel[p].CollPt;
	//convert to local coordinates
	Sl2   = s_pPanel[p].GlobalToLocal(S2);
	VTot  = s_pPanel[p].GlobalToLocal(Vx, Vy, Vz);

	//in panel referential
	VLocal.x = -4.0*PI*(s_pPanel[p].SMP*DELP - Sl2.y*DELQ)/Sl2.x;
	VLocal.y = -4.0*PI*DELQ;
//	Vl.z =  4.0*PI*Sigma[p];

	VTot +=VLocal;
	VTot.z = 0.0;

	Speed2 = VTot.x*VTot.x + VTot.y*VTot.y;

	Cp  = 1.0-Speed2/QInf/QInf;
}



void BoatAnalysisDlg::ComputeOnBody()
{
	//following VSAERO theory manual
	//the on-body tangential perturbation speed is the derivative of the doublet strength
	static CVector VLocal;
	static CVector PanelForce, Force;
	double Speed2;

	//______________________________________________________________________________________
	AddString(tr("      Computing On-Body...")+"\n");

	for(int p=0; p<m_MatSize; p++)
	{
		if(s_pPanel[p].m_Pos!=MIDSURFACE)
		{
			double factor = m_pBoatPolar->WindFactor(s_pPanel[p].CollPt.z);
			VLocal  = s_pPanel[p].GlobalToLocal(m_VInf * factor);
			VLocal += m_VMuDerivative[p];
			Speed2 = VLocal.x*VLocal.x + VLocal.y*VLocal.y;
			m_Cp[p]  = 1.0-Speed2/m_QInf/m_QInf/factor/factor;                  //We normalize Cp by QInf, without wind factor
		}
		else
		{
			// for each panel along the chord, add the lift coef

			PanelForce  = m_VInf * s_pPanel[p].Vortex;
			PanelForce *= m_Mu[p] * m_pBoatPolar->m_Density;                 //Newtons

			if(!m_pBoatPolar->m_bVLM1 && !s_pPanel[p].m_bIsLeading)
			{
				Force       = m_VInf* s_pPanel[p].Vortex;
				Force      *= m_Mu[p+1] * m_pBoatPolar->m_Density;       //Newtons
				PanelForce -= Force;
			}

			PanelForce *= m_pBoatPolar->WindFactor(s_pPanel[p].VortexPos.z);

			m_Cp[p]  = -2.0 * PanelForce.dot(s_pPanel[p].Normal) /s_pPanel[p].Area/m_pBoatPolar->m_Density;
		}

		if(m_bCancel) return;
	}
	if(m_bCancel) return;
	m_Progress += 1.0;
	qApp->processEvents();
}



void BoatAnalysisDlg::ComputeSurfSpeeds(double *Mu, double *Sigma)
{
	int p;
	CVector C;

	for (p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;
		C = s_pPanel[p].CollPt;//+ s_pPanel[p].Normal*s_pPanel[p].Size/100.0;
		C += s_pPanel[p].Normal*0.001;

		GetSpeedVector(C, Mu, Sigma, m_Speed[p]);
		m_Speed[p] += m_VInf * m_pBoatPolar->WindFactor(C.z);
	}
}





void BoatAnalysisDlg::GetDoubletInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake, bool bAll)
{
	// returns the influence of the panel pPanel at point C
	// if the panel pPanel is located on a thin surface, then its the influence of a vortex
	// if it is on a thick surface, then its a doublet


	if(pPanel->m_Pos!=MIDSURFACE || pPanel->m_bIsWakePanel)
	{
		pPanel->DoubletNASA4023(C, V, phi, bWake);
	}
	else
	{
		VLMGetVortexInfluence(pPanel, C, V, bAll);
		phi = 0.0;
	}

	if(m_pBoatPolar->m_bGround)
	{
		CG.Set(C.x, C.y, -C.z-2.0*m_pBoatPolar->m_Height);
		if(pPanel->m_Pos!=MIDSURFACE || pPanel->m_bIsWakePanel)
		{
			pPanel->DoubletNASA4023(CG, VG, phiG, bWake);
		}
		else
		{
			VLMGetVortexInfluence(pPanel, CG, VG, bAll);
			phiG = 0.0;
		}
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}


void BoatAnalysisDlg::GetSourceInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi)
{
	// returns the influence of a uniform source distribution on the panel pPanel at point C
	// The panel is necessarily located on a thick surface, else the source strength is zero


	pPanel->SourceNASA4023(C, V, phi);

	if(m_pBoatPolar->m_bGround)
	{
		CG.Set(C.x, C.y, -C.z-2.0*m_pBoatPolar->m_Height);
		pPanel->SourceNASA4023(CG, VG, phiG);
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}


void BoatAnalysisDlg::GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT, bool bAll, bool bTrace)
{
	CVector V;
	int pp, pw, lw;
	double phi, sign;

	VT.Set(0.0,0.0,0.0);

	for (pp=0; pp<m_MatSize;pp++)
	{
		if(m_bCancel) return;

		if(s_pPanel[pp].m_Pos!=MIDSURFACE) //otherwise Sigma[pp] =0.0, so contribution is zero also
		{
			GetSourceInfluence(C, s_pPanel+pp, V, phi);
			VT += V * Sigma[pp] ;
		}
		GetDoubletInfluence(C, s_pPanel+pp, V, phi, false, bAll);

		VT += V * Mu[pp];

		// Is the panel pp shedding a wake ?
		if(s_pPanel[pp].m_bIsTrailing && s_pPanel[pp].m_Pos!=MIDSURFACE)
		{
			//If so, we need to add the contribution of the wake column shedded by this panel
			if(s_pPanel[pp].m_Pos==BOTSURFACE) sign=-1.0; else sign=1.0;
			pw = s_pPanel[pp].m_iWake;
			for(lw=0; lw<m_pBoatPolar->m_NXWakePanels; lw++)
			{
				GetDoubletInfluence(C, s_pWakePanel+pw+lw, V, phi, true, bAll);
				VT += V * Mu[pp]*sign;
			}
		}
	}
}


void BoatAnalysisDlg::InitDialog()
{
	m_Progress = 0.0;
	m_pctrlProgress->setValue(m_Progress);
	QString FileName = QDir::tempPath() + "/sail7.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

/*	if(m_pBoatPolar && (m_pBoatPolar->m_bTiltedGeom || m_pBoatPolar->m_bWakeRollUp))
	{
		//back-up the current geometry if the analysis is to be performed on the tilted geometry
		memcpy(m_pMemPanel, s_pPanel, m_MatSize * sizeof(CPanel));
		memcpy(s_pMemNode,  s_pNode,  m_nNodes * sizeof(CVector));
		memcpy(m_pRefWakePanel, s_pWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(s_pRefWakeNode,  s_pWakeNode,  m_nWakeNodes * sizeof(CVector));
	}*/

	m_pctrlTextOutput->clear();

	m_bCancel   = false;
	m_bWarning  = false;
	m_bWakeRollUp    = false;

	SetFileHeader();

	QString str = "";


	str = QString(tr("Counted %1 panel elements")+"\n").arg(m_MatSize,4);
	AddString(str);

	AddString("\n");

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(100);
}


void BoatAnalysisDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			OnCancelAnalysis();
			event->accept();
			return;
		}
		default:
			event->ignore();
	}
}


void BoatAnalysisDlg::OnCancelAnalysis()
{
	if(m_pXFile->isOpen()) m_pXFile->close();
//	CWing::s_bCancel        = true;
	m_bCancel = true;
	m_bSkip = true;
	m_bExit = true;
	if(m_bIsFinished) done(1);
}


void BoatAnalysisDlg::SetFileHeader()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << pMainFrame->m_VersionName;
	out << "\n";
	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	out << str<<"\n";

	if(m_pBoat)     out << m_pBoat->m_BoatName;
	out << "\n";

	out << m_pBoatPolar->m_BoatPolarName;
	out << "\n___________________________________\n";
}


void BoatAnalysisDlg::OnProgress()
{
	m_pctrlProgress->setValue((int)m_Progress);
}


void BoatAnalysisDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()/2);
	setMinimumWidth(r.width()/2);

	m_pctrlTextOutput = new QTextEdit;
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);
	m_pctrlTextOutput->setFontFamily("Courier");

	m_pctrlProgress = new QProgressBar;
	m_pctrlProgress->setOrientation(Qt::Horizontal);
	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(100);
	m_pctrlProgress->setValue(0);

	m_pctrlCancel = new QPushButton(tr("Cancel"));
	connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));

	QHBoxLayout *ButtonLayout = new QHBoxLayout;
	ButtonLayout->addStretch(1);
	ButtonLayout->addWidget(m_pctrlCancel);
	ButtonLayout->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pctrlTextOutput);
	mainLayout->addWidget(m_pctrlProgress);
	mainLayout->addLayout(ButtonLayout);
	setLayout(mainLayout);
}



bool BoatAnalysisDlg::Solve()
{
	//______________________________________________________________________________________
	// Method :
	// 	- If the polar is of type 1 or 2, solve the linear system
	//	- for cosine and sine parts, for a unit speed
	//	- If the polar is of type 4, solve only for unit speed and for the specified Alpha
	//	- Reconstruct right side results if calculation was symetric
	//	- Sort results i.a.w. panel numbering
	//______________________________________________________________________________________

//	memcpy(s_RHS,           m_RHS, m_MatSize * sizeof(double));

	AddString("      Performing LU Matrix decomposition...\n");

	if(!Crout_LU_Decomposition_with_Pivoting(s_aij, m_Index, m_MatSize, &m_bCancel, 30.0*(double)m_MatSize/400.0, m_Progress))
	{
		AddString(tr("      Singular Matrix.... Aborting calculation...\n"));
		return false;
	}

	AddString("      Solving LU system...\n");
	Crout_LU_with_Pivoting_Solve(s_aij, m_RHS, m_Index, s_RHS, m_MatSize, &m_bCancel);

	memcpy(m_Mu, s_RHS, m_MatSize*sizeof(double));


	//   Define unit local velocity vector, necessary for moment calculations in stability analysis of 3D panels
	CVector u(1.0, 0.0, 0.0);
	double Cp;
	for (int p=0; p<m_MatSize; p++)
	{
		if(s_pPanel[p].m_Pos!=MIDSURFACE)
		{
			GetDoubletDerivative(p, m_RHS, Cp, m_VMuDerivative[p], 1.0, u.x, u.y, u.z);
		}
		if(m_bCancel) return false;
	}

	return true;
}



void BoatAnalysisDlg::StartAnalysis()
{
	//
	// Method applied from v6.00 onwards
	// 
	// First case :
	// If the analysis is for a wing and not a plane, the full 3D panel method is applied
	// and the wing is modelled as a thick surface
	// The method is strictly the one described in NASA TN 4023
	// The boundary condition is of the Dirichlet type, which has proved more convincing 
	// than the Neumann BC for full 3D panel methods
	//
	// Second case :
	// If the analysis is for a plane, the full 3D method is not applicable since the 
	// junctions between wing and body, or between fin and elevator, cannot be adequately 
	// represented as closed surfaces. This would require a 3D CAD programe. 
	// Therefore, in this case, the wings are modelled as thin surfaces.
	// Trial tests using the method of NASA TN 4023 have not been conclusive. With a uniform doublet
	// distribution and a boundary condition applied at the panel's centroid, the results 
	// are less convincing than with VLM.
	// Therefore in this case, the VLM1 method is applied to the thin surfaces, and the 3D-panel method 
	// is applied to the body.
	// Last consideration : since the potential of a straight vortex line requires a lot of computations, 
	// the Neumann type BC is applied to the body panels, rather than the Dirichlet type BC
	//
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	qApp->processEvents();

	if(!m_pBoatPolar) return;

	QString strong;
	m_pctrlCancel->setText(tr("Cancel"));
	m_bIsFinished = false;


	if(m_pBoatPolar->m_bVLM1) strong = tr("Launching VLM1 Analysis....")+"\n";
	else                      strong = tr("Launching VLM2 Analysis....")+"\n";

	AddString(strong);


	if(m_pBoat->m_poaHull.size())
	{
		if(m_pBoatPolar->m_bDirichlet) strong = tr("Using Dirichlet boundary conditions for thick bodies")+"\n";
		else                           strong = tr("Using Neumann boundary conditions for thick bodies")+"\n";
		AddString(strong);
		AddString("\n");
	}


	strong = tr("Type 1 - Fixed speed polar");
	AddString(strong);
	m_bCancel = false;

	QTimer *pTimer = new QTimer;
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnProgress()));
	pTimer->setInterval(100);
	pTimer->start();

	qApp->processEvents();

	UnitLoop();


	if (!m_bCancel && !m_bWarning) strong = "\n"+tr("Panel Analysis completed successfully")+"\n";
	else if (m_bWarning)           strong = "\n"+tr("Panel Analysis completed ... Errors encountered")+"\n";
	AddString(strong);
	pTimer->stop();

//	if(m_pBoatPolar && (m_pBoatPolar->m_Type==STABILITYPOLAR || m_pBoatPolar->m_bTiltedGeom || m_pBoatPolar->m_bWakeRollUp))
	{
		//restore the panels and nodes;
		memcpy(s_pPanel, s_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(s_pNode,  s_pMemNode,  m_nNodes  * sizeof(CVector));
		memcpy(s_pWakePanel, s_pRefWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(s_pWakeNode,  s_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));
	}

	m_bIsFinished = true;
	m_pctrlCancel->setText(tr("Close"));
}




void BoatAnalysisDlg::SetWindAxis(double const Beta)
{
	double cosb, sinb;

	cosb = cos(Beta*PI/180.0);
	sinb = sin(Beta*PI/180.0);

	//   Define wind (stability) axis
	m_WindDirection.Set(cosb, sinb, 0.0);
	m_WindNormal.Set(-sinb,cosb,0);
	m_WindSide      = m_WindNormal * m_WindDirection;

	m_VInf.Set(m_QInf*cosb, m_QInf*sinb, 0.0);
}


void BoatAnalysisDlg::SetAngles(BoatPolar *pBoatPolar, double Ctrl, bool bBCOnly)
{
	// Rotate the panels by the bank angle
	// (and translate the wake to the new T.E. position - thin surfaces, no wake)
	CVector O(0.0,0.0,0.0);
	QSail7 *pSail7 = (QSail7*)s_pSail7;

	//reset the initial geometry before a new set of angles is processed
	memcpy(s_pPanel, s_pMemPanel, m_MatSize * sizeof(CPanel));
	memcpy(s_pNode,  s_pMemNode,  m_nNodes  * sizeof(CVector));
//	memcpy(s_pWakePanel, s_pRefWakePanel, m_WakeSize * sizeof(CPanel));
//	memcpy(s_pWakeNode,  s_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));

	m_QInf = (1.0-Ctrl) * pBoatPolar->m_QInfMin + Ctrl * pBoatPolar->m_QInfMax;
	m_Phi  = (1.0-Ctrl) * pBoatPolar->m_PhiMin  + Ctrl * pBoatPolar->m_PhiMax;
	m_Beta = (1.0-Ctrl) * pBoatPolar->m_BetaMin + Ctrl * pBoatPolar->m_BetaMax;
	SetWindAxis(m_Beta);


	//rotate the sails around the masts
	Quaternion qt;
	CVector Mast;
	for(int is=0; is<m_pBoat->m_poaSail.size(); is++)
	{
		QSail *pSail = (QSail*)m_pBoat->m_poaSail.at(is);
		Mast.Set(sin(pSail->m_LuffAngle*PI/180.0), 0.0, cos(pSail->m_LuffAngle*PI/180.0));
//		Mast.RotateX(pSail->m_LEPosition, m_Phi);

		double Angle = (1.0-Ctrl) * pBoatPolar->m_SailAngleMin[is] + Ctrl * pBoatPolar->m_SailAngleMax[is];
		qt.Set(Angle, Mast);

		if(bBCOnly)
		{
			for(int ip=0; ip<pSail->m_NElements; ip++)
			{
				s_pPanel[ip+pSail->m_FirstPanel].RotateBC(pSail->m_LEPosition, qt);
			}
		}
		else
		{
			for(int in=0; in<m_nNodes; in++)
			{
				if(pSail->IsSailNode(in))
				{
					s_pNode[in].Rotate(pSail->m_LEPosition, Mast, -Angle);
				}
			}
/*			for(int ip=0; ip<pSail->m_NElements; ip++)
			{
				pSail->m_pPanel[ip].SetFrame();
			}*/
		}
	}
	pSail7->RotatePanelsX(m_Phi, O);
}



bool BoatAnalysisDlg::UnitLoop()
{
	QString str;
	int n, nrhs;

	if (m_ControlMax<m_ControlMin) m_ControlDelta = -fabs(m_ControlDelta);
	nrhs  = (int)fabs((m_ControlMax-m_ControlMin)*1.0001/m_ControlDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}

	//ESTIMATED UNIT TIMES FOR OPERATIONS

	double TotalTime = 10.0*(double)m_MatSize/400. //BuildInfluenceMatrix :     10 x MatSize/400
					 + 10.                         //CreateRHS :                10
					 + 30.*(double)m_MatSize/400.  //SolveUnitRHS :             30 x MatSize/400
					 + 1./400.   * (double)m_pBoat->m_poaSail.size()  //ComputeFarField : 1 x MatSize/400x nsails
					 + 1.   ;                      //ComputeOnBodyCp :           1

	TotalTime *= (double)nrhs;

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum((int)TotalTime);
	m_Progress = 0.0;
	qApp->processEvents();

	str = QString(tr("   Solving the problem... ")+"\n");
	AddString(str);

	for (n=0; n<nrhs; n++)
	{
		m_Ctrl = m_ControlMin +(double)n * m_ControlDelta;
		str = QString("      \n    "+tr("Processing parameter= %1")+"\n").arg(m_Ctrl,8,'f',3);
		AddString(str);

		SetAngles(m_pBoatPolar, m_Ctrl, false);
		if (m_bCancel) return true;


		BuildInfluenceMatrix();
		if (m_bCancel) return true;


		CreateRHS(m_RHS);
		if (m_bCancel) return true;


		CreateSourceStrength();
		if (m_bCancel) return true;

		//compute wake contribution
//		CreateWakeContribution();
		if (m_bCancel) return true;

		//add wake contribution to matrix and RHS
//		for(int p=0; p<m_MatSize; p++)
//		{
//			m_uRHS[p]+= m_uWake[p];
//			m_vRHS[p]+= m_wWake[p];
/*			for(int pp=0; pp<m_MatSize; pp++)
			{
				s_aij[p*m_MatSize+pp] += s_aijWake[p*m_MatSize+pp];
			}*/
//		}

		if (m_bCancel) return true;


		if (!Solve())
		{
			m_bWarning = true;
			return true;
		}
		if (m_bCancel) return true;


		ComputeFarField();
		if (m_bCancel) return true;

		ComputeOnBody();
		if (m_bCancel) return true;

		ComputeBoat();
		if (m_bCancel) return true;
	}

	return true;
}


void BoatAnalysisDlg::WriteString(QString strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}


void BoatAnalysisDlg::VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	// calculates the the panel p's vortex influence at point C
	// V is the resulting velocity
	static int lw, pw, p;
	static CVector AA1, BB1, VT;
	p = pPanel->m_iElement;

	V.x = V.y = V.z = 0.0;

	if(m_pBoatPolar->m_bVLM1)
	{
		//just get the horseshoe vortex's influence
		VLMCmn(pPanel->VA, pPanel->VB, m_WindDirection, C, V, bAll, CPanel::s_pCoreSize);
	}
	else
	{
		// we have quad vortices
		// so we follow Katz and Plotkin's lead
		if(!pPanel->m_bIsTrailing)
		{
			if(bAll)
			{
				VLMQmn(pPanel->VA, pPanel->VB, s_pPanel[p-1].VA, s_pPanel[p-1].VB, C, V, CPanel::s_pCoreSize);
			}
		}
		else
		{
			// then panel p is trailing and shedding a wake
			if(!m_bWakeRollUp)
			{
				// since Panel p+1 does not exist...
				// we define the points AA=A+1 and BB=B+1
				AA1.x = s_pNode[pPanel->m_iTA].x + (s_pNode[pPanel->m_iTA].x-pPanel->VA.x)/3.0;
				AA1.y = s_pNode[pPanel->m_iTA].y;
				AA1.z = s_pNode[pPanel->m_iTA].z;
				BB1.x = s_pNode[pPanel->m_iTB].x + (s_pNode[pPanel->m_iTB].x-pPanel->VB.x)/3.0;
				BB1.y = s_pNode[pPanel->m_iTB].y;
				BB1.z = s_pNode[pPanel->m_iTB].z;
				// first we get the quad vortex's influence
				if (bAll)
				{
					VLMQmn(pPanel->VA, pPanel->VB, AA1, BB1, C, V, CPanel::s_pCoreSize);
				}

				//we just add a trailing horseshoe vortex's influence to simulate the wake
				VLMCmn(AA1,BB1,m_WindDirection, C,VT,bAll, CPanel::s_pCoreSize);

				V.x += VT.x;
				V.y += VT.y;
				V.z += VT.z;
			}
			else
			{
				// if there is a wake roll-up required
				pw = pPanel->m_iWake;
				// first close the wing's last vortex ring at T.E.
				if (bAll)
				{
					VLMQmn(pPanel->VA, pPanel->VB, s_pWakePanel[pw].VA, s_pWakePanel[pw].VB, C, V, CPanel::s_pCoreSize);
				}

				//each wake panel has the same vortex strength than the T.E. panel
				//so we just cumulate their unit influences
				if(bAll)
				{
					for (lw=0; lw<m_pBoatPolar->m_NXWakePanels-1; lw++)
					{
						VLMQmn(s_pWakePanel[pw  ].VA, s_pWakePanel[pw  ].VB,
							   s_pWakePanel[pw+1].VA, s_pWakePanel[pw+1].VB, C, VT, CPanel::s_pCoreSize);
						V += VT;
						pw++;
					}
				}
//
// For the very last wake panel downstream, just add a horseshoe vortex influence
//
// TODO : check influence on results
//
//				VLMCmn(s_pWakePanel[pw].A, s_pWakePanel[pw].B,C,VT,bAll);
//				V += VT;
//				if(m_pBoatPolar->m_bGround)
//				{
//					VLMCmn(AAG, BBG, C, VG);
//					V.x += VG.x;
//					V.y += VG.y;
//					V.z -= VG.z;
//				}
//
// simple really !
//
			}
		}
	}
}



void BoatAnalysisDlg::Forces(double *Mu, double *Sigma, double alpha, double *VInf, CVector &Force, CVector &Moment, bool bTilted, bool bTrace)
{
	// Calculates the forces using a farfield method
	// Calculates the moments by a near field method, i.e. direct summation on the panels
	// Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)
	//
	if(!s_pPanel||!m_pBoatPolar) return;
	static int j, k, l, p, pp, m, nw, iTA, iTB;
	static double StripArea;
	static double GammaStrip;
	static CVector  C, PanelLeverArm, Wg;
	static CVector Velocity, StripForce, dF, PanelForce, PanelForcep1;
//	QSail7 *pSail7= (QSail7*)s_pSail7;

	int coef = 1;

	p=m=0;

	Force.Set( 0.0, 0.0, 0.0);
	Moment.Set(0.0, 0.0, 0.0);

	for(j=0; j<m_pBoat->m_poaSail.size(); j++)
	{
		QSail *pSail = (QSail*)m_pBoat->m_poaSail.at(j);
		for(k=0; k<pSail->m_NZPanels; k++)
		{
			//Get the strip area
			pp=p;
			StripArea = 0.0;
			for (l=0; l<coef*pSail->m_NXPanels; l++)
			{
				StripArea  += s_pPanel[pp].Area;
				pp++;
			}

			//Get the strip's lifting force
			if(s_pPanel[p].m_Pos!=MIDSURFACE)
			{
				StripArea /=2.0;
				//FF force
				nw  = s_pPanel[p].m_iWake;
				iTA = s_pWakePanel[nw].m_iTA;
				iTB = s_pWakePanel[nw].m_iTB;
				C = (s_pWakeNode[iTA] + s_pWakeNode[iTB])/2.0;
				GetSpeedVector(C, Mu, Sigma, Wg, false);
				Wg.x += VInf[p            ];
				Wg.y += VInf[p+m_MatSize  ];
				Wg.z += VInf[p+2*m_MatSize];

				GammaStrip = (-Mu[p+coef*pSail->m_NXPanels-1] + Mu[p]) *4.0*PI;

				StripForce  = s_pPanel[p].Vortex * Wg;
				StripForce *= GammaStrip;                            //Newtons/rho
				Force += StripForce;

				Velocity.x = *(VInf               +p);
				Velocity.y = *(VInf +   m_MatSize +p);
				Velocity.z = *(VInf + 2*m_MatSize +p);

				p += pSail->m_NXPanels*coef;
			}
			else
			{
				//iPos=0, VLM type panel
				StripForce.Set(0.0,0.0,0.0);
				for(l=0; l<pSail->m_NXPanels; l++)
				{
					Velocity.x = *(VInf               +p);
					Velocity.y = *(VInf +   m_MatSize +p);
					Velocity.z = *(VInf + 2*m_MatSize +p);

					//FF force
					if(m_pBoatPolar->m_bVLM1 || s_pPanel[p].m_bIsTrailing)
					{
						C = s_pPanel[p].CtrlPt;
						C.x = m_pBoatPolar->m_Span * 100.0;

						GetSpeedVector(C, Mu, Sigma, Wg, false);
						Wg += Velocity; //total speed vector

						//induced force
						dF  = Wg * s_pPanel[p].Vortex;
						dF *=  Mu[p];  // N/rho

						Force += dF;        // N/rho
						StripForce += dF;
					}
					//On-Body moment
					PanelForce  = Velocity * s_pPanel[p].Vortex;
					PanelForce *= Mu[p];                                 //Newtons/rho

					if(!m_pBoatPolar->m_bVLM1 && !s_pPanel[p].m_bIsLeading)
					{
						PanelForcep1       = Velocity * s_pPanel[p].Vortex;
						PanelForcep1      *= Mu[p+1];                          //Newtons/rho

						PanelForce -= PanelForcep1;
					}

					PanelLeverArm = s_pPanel[p].VortexPos - m_pBoatPolar->m_CoG;
					Moment += PanelLeverArm * PanelForce;                     // N.m/rho
					p++;
				}
			}
			m++;
		}
	}


	Force -= m_WindDirection*Force.dot(m_WindDirection)/2.0;

	Force  *= m_pBoatPolar->m_Density;                          // N
	Moment *= m_pBoatPolar->m_Density;                          // N.m
}





