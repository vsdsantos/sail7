/****************************************************************************

		 BoatOpp Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#include "../globals.h"
#include "../mainframe.h"
#include "../sail7/sail7.h"
#include "BoatOpp.h"
#include <QtDebug>

void *BoatOpp::s_pMainFrame = NULL;
void *BoatOpp::s_pSail7 = NULL;


BoatOpp::BoatOpp()
{
	m_bVLM1        = true;

	m_bIsVisible  = true;
	m_bShowPoints = false;

	m_Color = QColor(255,0,0);
	m_Style = 0;
	m_Width = 1;

	m_nWakeNodes     = 0;
	m_NXWakePanels   = 0;
	m_FirstWakePanel = 0.0;
	m_WakeFactor     = 1.0;

	m_NStation     = 0;
	m_NVLMPanels   = 0;

	m_Weight = 0.0;

	m_QInf        = 0.0;
	m_Beta        = 0.0;
	m_Phi         = 0.0;
	m_Ctrl        = 0.0;

	for(int is=0;is<MAXSAILS; is++) m_SailAngle[is]=0.0;

	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_G,0,sizeof(m_G));
	memset(m_Sigma,0,sizeof(m_Sigma));
}



bool BoatOpp::SerializeBoatOpp(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int a,p,j, k,l;
	float f;
	QSail7 *pSail7 = (QSail7*)s_pSail7;
	CBoat *pBoat = pSail7->GetBoat(m_BoatName);

	if(bIsStoring)
	{
		ar << 100001;
		//100001 : first file format

		WriteCString(ar, m_BoatName);
		WriteCString(ar, m_BoatPolarName);

		if(m_bIsVisible)   ar << 1; else ar<<0;
		if(m_bShowPoints)  ar << 1; else ar<<0;

		ar << m_Style<<m_Width<<m_Color.red()<<m_Color.green()<<m_Color.blue();

		ar << 1;
		ar << m_nWakeNodes << m_NXWakePanels;
		ar << m_FirstWakePanel << m_WakeFactor;
		ar << m_Weight;
		ar << m_QInf << m_Beta << m_Phi << m_Ctrl;

		ar<<pBoat->m_poaSail.size();
		for(int is=0;is<pBoat->m_poaSail.size(); is++)
		{
			ar<< m_SailAngle[is];
		}

		ar << ForceTrefftz.x << ForceTrefftz.y << ForceTrefftz.z;
		ar << F.x << F.y << F.z;
		ar << M.x << M.y << M.z;

		ar << m_NVLMPanels;
		for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_Cp[p] ;
		for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_G[p] ;
		for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_Sigma[p] ;

		//provision
		{
			for(int i=1; i<20; i++) ar<<(float)0.0f;
			for(int i=0; i<20; i++) ar<<0;
		}
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat<100000 || ArchiveFormat>120000) return false;
		//read variables
		ReadCString(ar, m_BoatName);
		ReadCString(ar, m_BoatPolarName);

		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a) m_bIsVisible = true; else m_bIsVisible = false;

		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a) m_bShowPoints = true; else m_bShowPoints = false;

		ar >> m_Style>>m_Width;
		ar >> j>>k>>l;
		m_Color = QColor(j,k,l);

		ar >> j;//m_AnalysisMethod;
		ar >> m_nWakeNodes >> m_NXWakePanels;
		ar >> m_FirstWakePanel >> m_WakeFactor;
		ar >> m_Weight;
		ar >> m_QInf >> m_Beta >> m_Phi >> m_Ctrl;

		ar>>a;
		for(int is=0;is<a; is++)
		{
			ar>> m_SailAngle[is];
		}
		ar >> ForceTrefftz.x >> ForceTrefftz.y >> ForceTrefftz.z;
		ar >> F.x >> F.y >> F.z;
		ar >> M.x >> M.y >> M.z;


		ar>> m_NVLMPanels;
		for (p=0; p<m_NVLMPanels;p++)
		{
			ar >> f; m_Cp[p] =f;

		}

		for (p=0; p<m_NVLMPanels;p++)
		{
			ar >> f; m_G[p] =f;
		}

		for (p=0; p<m_NVLMPanels;p++)
		{
			ar >> f; m_Sigma[p] = f;
		}

		//provision
		for(int i=1; i<20; i++) ar>>f;
		for(int i=0; i<20; i++) ar>>k;

	}
	return true;
}



void BoatOpp::GetBoatOppProperties(QString &BOppProperties)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, lenunit, forceunit, momentunit, speedunit;

	QSail7 *pSail7 = (QSail7*)s_pSail7;
	CBoat *pBoat = pSail7->GetBoat(m_BoatName);

	GetLengthUnit(lenunit, pMainFrame->m_LengthUnit);
	GetSpeedUnit(speedunit, pMainFrame->m_SpeedUnit);
	GetForceUnit(forceunit, pMainFrame->m_ForceUnit);
	GetMomentUnit(momentunit, pMainFrame->m_ForceUnit);

	BOppProperties.clear();


	strong  = QString("VInf =%1 ").arg(m_QInf,7,'f',3);
	BOppProperties += "\n"+strong + speedunit+"\n";

	strong  = QString("Beta = %1").arg(m_Beta,5,'f',1);
	BOppProperties += strong +QString::fromUtf8("°")+"\n\n";

	strong  = QString("Phi = %1").arg(m_Phi,5,'f',1);
	BOppProperties += strong +QString::fromUtf8("°")+"\n\n";

	for(int is=0; is<pBoat->m_poaSail.size(); is++)
	{
		strong  = QString("Sail Angle[%1] = %2").arg(is).arg(m_SailAngle[is], 5,'f',1);
		BOppProperties += strong +QString::fromUtf8("°")+"\n\n";
	}

	strong  = QString("Far Field Fx = %1 ").arg(ForceTrefftz.x*pMainFrame->m_NtoUnit,7,'f',3);
	BOppProperties += strong +forceunit+"\n";
	strong  = QString("Far Field Fy = %1 ").arg(ForceTrefftz.y*pMainFrame->m_NtoUnit,7,'f',3);
	BOppProperties += strong +forceunit+"\n";
	strong  = QString("Far Field Fz = %1 ").arg(ForceTrefftz.z*pMainFrame->m_NtoUnit,7,'f',3);
	BOppProperties += strong +forceunit+"\n\n";

	strong  = QString("Fx = %1 ").arg(F.x*pMainFrame->m_NtoUnit,7,'f',3);
	BOppProperties += strong +forceunit+"\n";
	strong  = QString("Fy = %1 ").arg(F.y*pMainFrame->m_NtoUnit,7,'f',3);
	BOppProperties += strong +forceunit+"\n";
	strong  = QString("Fz = %1 ").arg(F.z*pMainFrame->m_NtoUnit,7,'f',3);
	BOppProperties += strong +forceunit+"\n\n";


	strong  = QString("Mx = %1 ").arg(M.x*pMainFrame->m_NmtoUnit,7,'f',3);
	BOppProperties += strong +momentunit+"\n";
	strong  = QString("My = %1 ").arg(M.y*pMainFrame->m_NmtoUnit,7,'f',3);
	BOppProperties += strong +momentunit+"\n";
	strong  = QString("Mz = %1 ").arg(M.z*pMainFrame->m_NmtoUnit,7,'f',3);
	BOppProperties += strong +momentunit+"\n\n";


	BOppProperties +="\n";
}



CVector BoatOpp::GetWindDirection()
{
	return CVector(cos(m_Beta*PI/180.0), sin(m_Beta*PI/180.0), 0.0);
}







