/****************************************************************************

		 CSail Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#include <QDomDocument>
#include <QMessageBox>
#include "../mainframe.h"
#include <QtDebug>
#include "../sail7/BoatAnalysisDlg.h"
#include "../objects/SailcutSpline.h"
#include "../globals.h"
#include "Sail.h"


void * QSail::s_pBoatAnalysisDlg;
void * QSail::s_pMainFrame;

QSail::QSail()
{
	m_pPanel     = NULL;

	m_SailName = "Sail Name";
	m_SailColor.setHsv((int)(((double)qrand()/(double)RAND_MAX)*255), 140, 170, 140);
	m_SailType = SAILCUTSAIL;

	m_LuffAngle = 0.0;

	m_LEPosition.Set(0.0,0.0,0.0);

	m_NXPanels   = 17; 		// VLM Panels along horizontal direction
	m_NZPanels   = 29;      // will be set by adding panels in each section
//	m_XPanelDist = COSINE;

	m_FirstPanel = 0;
	m_NElements  = 0;
	m_NStation = 0;
}


void QSail::PanelTrefftz(Vector3d VInf, double *Mu, double *Sigma, int pos, Vector3d &FFForce, BoatPolar *pBoatPolar)
{
	// calculates the induced lift and drag from the vortices or wake panels strength
	// using a farfield method
	// Downwash is evaluated at a distance 1km downstream (i.e. infinite)

	int l, p;
	Vector3d C, Wg, dF, StripForce, WindDirection, WindNormal;
	BoatAnalysisDlg *pPanelDlg = (BoatAnalysisDlg*)s_pBoatAnalysisDlg;


	//lift and drag are calculated in wind axis
	WindDirection = VInf;
	WindDirection.Normalize();
	WindNormal.Set(-VInf.y, VInf.x, VInf.z);
	WindNormal.Normalize();

	p=0;

	FFForce.Set(0.0,0.0,0.0);


	for (int m=0; m<m_NZPanels; m++)
	{
		StripForce.Set(0.0,0.0,0.0);

		for(l=0; l<m_NXPanels; l++)
		{
			if(pBoatPolar->m_bVLM1 || m_pPanel[p].m_bIsTrailing)
			{
				C = m_pPanel[p].CtrlPt;
				C += WindDirection *1000.0;

				pPanelDlg->GetSpeedVector(C, Mu, Sigma, Wg, false);
//				qDebug()<<Wg.x<<Wg.y<<Wg.z;

				if(m_pPanel[p].m_bIsTrailing) m_Vd[m] = Wg;
				Wg += VInf * pBoatPolar->WindFactor(C.z); //total speed vector

				//induced force
				dF  = Wg * m_pPanel[p].Vortex;
				dF *= Mu[p+pos] * pBoatPolar->m_Density;       // N
				StripForce += dF;                              // N    //Katz & Plotkin eq. 8.147

				// get wind-aligned component
				double fw = StripForce.dot(WindDirection);
				// substract half : Katz & Plotkin eq. 8.146
				StripForce -= WindDirection * fw/2.0;
			}
			p++;
		}

		//____________________________
		// Calculate resulting vector force
		FFForce     += StripForce;                              // N

	}
}



void QSail::PanelComputeOnBody(Vector3d VInf, double *Cp, double *Gamma,
							   Vector3d &SailForce, Vector3d &SailMoment,
							   BoatPolar *pBoatPolar, Vector3d CoG)
{
	Vector3d ForcePt, LeverArmPanelCoG;
	Vector3d Force, PanelForce;

	//initialize
	SailMoment.Set(0.0,0.0,0.0);
	SailForce.Set( 0.0,0.0,0.0);
	double QInf = VInf.VAbs();

	for(int p=0; p<m_NElements; p++)
	{
		// Get the force acting on the panel
		ForcePt = m_pPanel[p].VortexPos;

		PanelForce  = VInf * m_pPanel[p].Vortex ;
		PanelForce *= Gamma[p];                 // N/rho

		if(!pBoatPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
		{
			Force       = VInf * m_pPanel[p].Vortex;
			Force      *= Gamma[p+1];            // N/rho
			PanelForce -= Force;
		}

		PanelForce *= pBoatPolar->WindFactor(ForcePt.z);                //N/rho

		Cp[p] = PanelForce.dot(m_pPanel[p].Normal) * 2.0/m_pPanel[p].Area/QInf/QInf;

		LeverArmPanelCoG   = ForcePt - CoG;                             // m
		SailForce  += PanelForce;                                       // Newtons/rho
		SailMoment += LeverArmPanelCoG * PanelForce;                    // N.m/rho
	}
	SailForce  *= pBoatPolar->m_Density;   //Newton
	SailMoment *= pBoatPolar->m_Density;   //N.m
}




bool QSail::IsSailPanel(int nPanel)
{
	for(int p=0; p<m_NElements; p++)
	{
		if(nPanel==m_pPanel[p].m_iElement) return true;
	}
	return false;
}


bool QSail::IsSailNode(int nNode)
{
	for(int p=0; p<m_NElements; p++)
	{
		if(nNode==m_pPanel[p].m_iLA) return true;
		if(nNode==m_pPanel[p].m_iLB) return true;
		if(nNode==m_pPanel[p].m_iTA) return true;
		if(nNode==m_pPanel[p].m_iTB) return true;
	}
	return false;
}



void QSail::SetLuffAngle()
{
	SetLuffAngle(m_LuffAngle);
}



void QSail::SetLuffAngle(double Angle)
{
	m_LuffAngle = Angle;
	SplineSurface();
}


void QSail::SortSections()
{
	// bubble sort
	// derived from http://www.concentric.net/~ttwang/sort/sort.htm
	//

	int indx, indx2;
	int flipped;
	SailSection Section(this);

	indx = 1;
	do
	{
		flipped = 0;
		for (indx2 = m_oaSection.size() - 1; indx2 >= indx; --indx2)
		{
			if (m_oaSection[indx2 - 1]->m_Position.z> m_oaSection[indx2]->m_Position.z)
			{
				Section.Copy(m_oaSection[indx2]);
				m_oaSection[indx2]->Copy(m_oaSection[indx2-1]);
				m_oaSection.at(indx2-1)->Copy(&Section);
				flipped = 1;
			}
		}
	} while ((++indx < m_oaSection.size()) && flipped);
}




void QSail::InsertSection(SailSection *pSection)
{
	for(int is=0; is<m_oaSection.size(); is++)
	{
		if(pSection->m_Position.z <= m_oaSection.at(is)->m_Position.z)
		{
			m_oaSection.insert(is, pSection);
			return;
		}
	}
	m_oaSection.append(pSection);
}



void QSail::CreateSection(int iSection)
{
	SailSection *pNewSection = new SailSection(this);

	if(iSection>=m_oaSection.size())
	{
		pNewSection->Copy(m_oaSection.last());
		m_oaSection.append(pNewSection);
		pNewSection->SetwPosition(pNewSection->m_Position.z-1.0);
	}
	else
	{
		SailSection *pCurSection = m_oaSection.at(iSection);
		SailSection *pPrevSection=NULL;
		if(iSection>0) pPrevSection = m_oaSection.at(iSection-1);

		if(pPrevSection)
		{
//			pNewSection->m_Chord = (pCurSection->m_Chord+pPrevSection->m_Chord)/2.0;
//			pNewSection->m_Twist = (pCurSection->m_Twist+pPrevSection->m_Twist)/2.0;
			pNewSection->m_Angle = (pCurSection->m_Angle+pPrevSection->m_Angle)/2.0;

			pNewSection->m_CtrlPoint.clear();
			for(int ic=0; ic<pCurSection->m_CtrlPoint.count(); ic++) pNewSection->m_CtrlPoint.append(pCurSection->m_CtrlPoint.at(ic));
			pNewSection->SetwPosition((pCurSection->m_Position.z+pPrevSection->m_Position.z) /2.0);
		}
		else
		{
			pNewSection->Copy(pCurSection);
			pNewSection->SetwPosition(pNewSection->m_Position.z-1.0);
		}

		m_oaSection.insert(iSection, pNewSection);
	}
	SplineSurface();
}


void QSail::DeleteSection(int iSection)
{
	m_oaSection.removeAt(iSection);
	SplineSurface();
}












