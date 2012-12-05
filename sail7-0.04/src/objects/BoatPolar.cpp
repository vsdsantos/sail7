/****************************************************************************

		 SailPolar Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include "../params.h"
#include "../mainframe.h"
#include "../globals.h"
#include "../sail7/sail7.h"
#include "BoatPolar.h"


void *BoatPolar::s_pMainFrame;
void *BoatPolar::s_pSail7;


BoatPolar::BoatPolar()
{
	m_BoatPolarName.clear();
	m_BoatName.clear();


	m_bVLM1 = false;
	m_bGround     = false;
	m_bDirichlet  = true;
	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_bWakeRollUp = false;

	m_NXWakePanels = 1;
	m_WakePanelFactor = 1.1;
	m_TotalWakeLength = 5.0;

	m_QInfMin = m_QInfMax = 10.0; //wind speed
	m_BetaMin = m_BetaMax = 0.0;  //sideslip = wind angle
	m_PhiMin  = m_PhiMax  = 0.0;  //bank angle
	memset(m_SailAngleMin, 0, MAXSAILS*sizeof(double));
	memset(m_SailAngleMax, 0, MAXSAILS*sizeof(double));


	m_Density   = 1.225;
	m_Viscosity = 1.5e-5; //m2/s
	m_Height = 0.0;
	m_Area = 0.0;
	m_Span = 0.0;

	m_WindGradient[0][0] =    0.0;
	m_WindGradient[0][1] =    1.0;
	m_WindGradient[1][0] =   25.0;
	m_WindGradient[1][1] =    1.0;

	m_CoG.Set(0.0,0.0,0.0);

	m_Color = QColor(255,0,0);
	m_Style = 0;
	m_Width = 1;

	m_AMem = 0.0;

	ResetBoatPlr();
}



void BoatPolar::DuplicateSpec(BoatPolar *pBoatPolar)
{

	m_bVLM1           = pBoatPolar->m_bVLM1;
	m_bGround         = pBoatPolar->m_bGround;
	m_bDirichlet      = pBoatPolar->m_bDirichlet;
	m_bWakeRollUp     = pBoatPolar->m_bWakeRollUp;

	m_NXWakePanels    = pBoatPolar->m_NXWakePanels;
	m_WakePanelFactor = pBoatPolar->m_WakePanelFactor;
	m_TotalWakeLength = pBoatPolar->m_TotalWakeLength;

	m_QInfMin = pBoatPolar->m_QInfMin;
	m_QInfMax = pBoatPolar->m_QInfMax;
	m_BetaMin = pBoatPolar->m_BetaMin;
	m_BetaMax = pBoatPolar->m_BetaMax;
	m_PhiMin  = pBoatPolar->m_PhiMin;
	m_PhiMax  = pBoatPolar->m_PhiMax;

	memcpy(m_SailAngleMin, pBoatPolar->m_SailAngleMin, MAXSAILS*sizeof(double));
	memcpy(m_SailAngleMax, pBoatPolar->m_SailAngleMax, MAXSAILS*sizeof(double));


	m_Density = pBoatPolar->m_Density;
	m_Viscosity = pBoatPolar->m_Viscosity;

	m_Area = pBoatPolar->m_Area;
	m_Span = pBoatPolar->m_Span;
	m_Height = pBoatPolar->m_Height;

	memcpy(m_WindGradient, pBoatPolar->m_WindGradient, 4*sizeof(double));

	m_CoG.Set(pBoatPolar->m_CoG);

	m_AMem = pBoatPolar->m_AMem;
}


void BoatPolar::AddPoint(BoatOpp *pBOpp)
{
	bool bInserted = false;
	int i=0;
	int size = m_Ctrl.size();
	if(size)
	{
		for (i=0; i<size; i++)
		{
			if (fabs(pBOpp->m_Ctrl - m_Ctrl[i]) < 0.001)
			{
				// then erase former result
				m_Ctrl[i]    =  pBOpp->m_Ctrl;
				m_Lift[i]    =  pBOpp->m_Lift;
				m_Drag[i]    =  pBOpp->m_Drag;
				m_FFFx[i]    =  pBOpp->ForceTrefftz.x;
				m_FFFy[i]    =  pBOpp->ForceTrefftz.y;
				m_FFFz[i]    =  pBOpp->ForceTrefftz.z;
				m_Fx[i]      =  pBOpp->F.x;
				m_Fy[i]      =  pBOpp->F.y;
				m_Fz[i]      =  pBOpp->F.z;
				m_Mx[i]      =  pBOpp->M.x;
				m_My[i]      =  pBOpp->M.y;
				m_Mz[i]      =  pBOpp->M.z;
				bInserted = true;
				break;
			}
			else if (pBOpp->m_Ctrl < m_Ctrl[i])
			{
				// sort by crescending control values
				m_Ctrl.insert(i, pBOpp->m_Ctrl);
				m_Lift.insert(i, pBOpp->m_Lift);
				m_Drag.insert(i, pBOpp->m_Drag);
				m_FFFx.insert(i, pBOpp->ForceTrefftz.x);
				m_FFFy.insert(i, pBOpp->ForceTrefftz.y);
				m_FFFz.insert(i, pBOpp->ForceTrefftz.z);
				m_Fx.insert(i, pBOpp->F.x);
				m_Fy.insert(i, pBOpp->F.y);
				m_Fz.insert(i, pBOpp->F.z);
				m_Mx.insert(i, pBOpp->M.x);
				m_My.insert(i, pBOpp->M.y);
				m_Mz.insert(i, pBOpp->M.z);
				bInserted = true;
				break;
			}
		}
	}
	if(!bInserted)
	{
		// data is appended at the end
		m_Ctrl.append(pBOpp->m_Ctrl);
		m_Lift.append(pBOpp->m_Lift);
		m_Drag.append(pBOpp->m_Drag);
		m_FFFx.append( pBOpp->ForceTrefftz.x);
		m_FFFy.append( pBOpp->ForceTrefftz.y);
		m_FFFz.append( pBOpp->ForceTrefftz.z);
		m_Fx.append( pBOpp->F.x);
		m_Fy.append( pBOpp->F.y);
		m_Fz.append( pBOpp->F.z);
		m_Mx.append( pBOpp->M.x);
		m_My.append( pBOpp->M.y);
		m_Mz.append( pBOpp->M.z);
	}
}



bool BoatPolar::SerializeBoatPlr(QDataStream &ar, bool bIsStoring)
{
	int n;
	float f,g,h;
	double d;
	int i;

	QSail7 *pSail7 = (QSail7*)s_pSail7;
	CBoat *pBoat = pSail7->GetBoat(m_BoatName);

	int PolarFormat = 100391;
	// 100391 : added Lift and Drag
	// 100390 : added wind gradient
	// 100389 : v0.00

	if(bIsStoring)
	{
		//write variables
		ar << PolarFormat; // identifies the format of the file
		WriteCString(ar,m_BoatName);
		WriteCString(ar,m_BoatPolarName);

		ar << m_Style  << m_Width;
		WriteCOLORREF(ar, m_Color);

		if (m_bIsVisible)  ar << 1; else ar << 0;
		if (m_bShowPoints) ar << 1; else ar << 0;
		if (m_bVLM1)         ar << 1; else ar << 0;
		ar<<0;//m_bTiltedGeom
		if (m_bDirichlet)    ar << 1; else ar << 0;
		ar<<0;//m_bViscous
		if (m_bGround)       ar << 1; else ar << 0;
		ar << 0.0;//formerly height

		ar << m_WindGradient[0][0] << m_WindGradient[0][1] << m_WindGradient[1][0] << m_WindGradient[1][1];

		ar <<  m_CoG.x << m_CoG.y << m_CoG.z;
		ar << m_Density << m_Viscosity;

		ar << m_QInfMin<<m_QInfMax<<m_BetaMin<<m_BetaMax<<m_PhiMin<<m_PhiMax;

		ar << pBoat->m_poaSail.size();
		for(int is=0; is<pBoat->m_poaSail.size();is++)
		{
			ar << m_SailAngleMin[is] << m_SailAngleMax[is];
		}

		ar <<m_Ctrl.size();
		for (i=0; i<m_Ctrl.size(); i++)
		{
			ar << (float)m_Ctrl[i];
			ar << (float)m_Lift[i] << (float)m_Drag[i];
			ar << (float)m_FFFx[i] << (float)m_FFFy[i] << (float)m_FFFz[i];
			ar << (float)m_Fx[i]   << (float)m_Fy[i]   << (float)m_Fz[i];
			ar << (float)m_Mx[i]   << (float)m_My[i]   << (float)m_Mz[i];
		}

		return true;
	}
	else
	{
		//read variables
		ar >> PolarFormat;
		if (PolarFormat <=100000 || PolarFormat>200000) return false;

		ReadCString(ar, m_BoatName);
		ReadCString(ar, m_BoatPolarName);

		ar >> m_Style  >> m_Width;
		if (m_Style<0 || m_Style> 10) return false;

		if (m_Width<0 || m_Width> 10) return false;

		ReadCOLORREF(ar, m_Color);

		ar >> n;
		if (n!=0 && n!=1) return false;
		if(n) m_bIsVisible =true; else m_bIsVisible = false;

		ar >> n;
		if (n!=0 && n!=1) return false;
		if(n) m_bShowPoints =true; else m_bShowPoints = false;

		ar >> n;
		if (n!=0 && n!=1) return false;
		if(n) m_bVLM1 =true; else m_bVLM1 = false;

		ar >> n;
		if (n!=0 && n!=1) return false;
//		if(n) m_bTiltedGeom =true; else m_bTiltedGeom = false;

		ar >> n;
		if (n!=0 && n!=1) return false;
		if(n) m_bDirichlet =true; else m_bDirichlet = false;

		ar >> n;
		if (n!=0 && n!=1) return false;
//		if(n) m_bViscous =true; else m_bViscous = false;

		ar >> n;
		if (n!=0 && n!=1) return false;
		if(n) m_bGround =true; else m_bGround = false;

		ar >> d;//formerly height

		if(PolarFormat>=100390)
		{
			ar >> m_WindGradient[0][0] >> m_WindGradient[0][1] >> m_WindGradient[1][0] >> m_WindGradient[1][1];
		}

		ar >> m_CoG.x >> m_CoG.y >> m_CoG.z;

		ar >> m_Density >> m_Viscosity;

		ar >> m_QInfMin>>m_QInfMax>>m_BetaMin>>m_BetaMax>>m_PhiMin>>m_PhiMax;

		ar >> n;
		for (int is=0; is<n; is++)
		{
			ar >> m_SailAngleMin[is] >> m_SailAngleMax[is];
		}

		ar >> n;
		for (i=0; i<n; i++)
		{
			ar >> f;                m_Ctrl.append(f);
			if(PolarFormat>=100391)
			{
				ar >> f >> g;       m_Lift.append(f); m_Drag.append(g);
			}
			else
			{
				m_Lift.append(0.0); m_Drag.append(0.0);
			}
			ar >>f>>g>>h; 			m_FFFx.append(f);  m_FFFy.append(g);  m_FFFz.append(h);
			ar >>f>>g>>h; 			m_Fx.append(f);  m_Fy.append(g);  m_Fz.append(h);
			ar >>f>>g>>h; 			m_Mx.append(f);  m_My.append(g);  m_Mz.append(h);
		}
	}
	return true;
}




void * BoatPolar::GetBoatPlrVariable(enumPolarVar iVar)
{
	// returns a pointer to the variable array defined by its index iVar
	void * pVar;
	switch (iVar)
	{
		case CTRL:
			pVar = &m_Ctrl;
			break;
		case VINF:
			pVar = NULL;//QInf
			break;
		case BETA:
			pVar = NULL;//Beta
			break;
		case PHI:
			pVar = NULL;//PHi
			break;
		case LIFT:
			pVar = &m_Lift;
			break;
		case DRAG:
			pVar = &m_Drag;
			break;
		case FFFX:
			pVar = &m_FFFx;
			break;
		case FFFY:
			pVar = &m_FFFy;
			break;
		case FFFZ:
			pVar = &m_FFFz;
			break;
		case FX:
			pVar = &m_Fx;
			break;
		case FY:
			pVar = &m_Fy;
			break;
		case FZ:
			pVar = &m_Fz;
			break;
		case MX:
			pVar = &m_Mx;
			break;
		case MY:
			pVar = &m_My;
			break;
		case MZ:
			pVar = &m_Mz;
			break;
		default:
			pVar = &m_Ctrl;
			break;
	}
	return pVar;
}



QString BoatPolar::GetPolarVariableName(int iVar)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString StrForce;
	QString StrSpeed;
	QString StrMoment;
	GetForceUnit(StrForce, pMainFrame->m_ForceUnit);
	GetSpeedUnit(StrSpeed, pMainFrame->m_SpeedUnit);
	GetMomentUnit(StrMoment, pMainFrame->m_MomentUnit);

	switch (iVar)
	{
		case 0:
			return QObject::tr("Ctrl");
			break;
		case 1:
			return QObject::tr("VInf") + " ("+StrSpeed+")";
			break;
		case 2:
			return QObject::tr("Beta") + QString::fromUtf8(" (°)");
			break;
		case 3:
			return QObject::tr("Phi") + QString::fromUtf8(" (°)");
			break;
		case 4:
			return QObject::tr("Lift") + " ("+StrForce+")";
			break;
		case 5:
			return QObject::tr("Drag") + " ("+StrForce+")";
			break;
		case 6:
			return QObject::tr("FF_Fx") + " ("+StrForce+")";
			break;
		case 7:
			return QObject::tr("FF_Fy") + " ("+StrForce+")";
			break;
		case 8:
			return QObject::tr("FF_Fz") + " ("+StrForce+")";
			break;
		case 9:
			return QObject::tr("Fx") + " ("+StrForce+")";
			break;
		case 10:
			return QObject::tr("Fy") + " ("+StrForce+")";
			break;
		case 11:
			return QObject::tr("Fz") + " ("+StrForce+")";
			break;
		case 12:
			return QObject::tr("Mx") + " ("+StrMoment+")";
			break;
		case 13:
			return QObject::tr("My") + " ("+StrMoment+")";
			break;
		case 14:
			return QObject::tr("Mz") + "("+StrMoment+")";
			break;
	}
}



void BoatPolar::GetPolarProperties(QString &PolarProperties, int FileType, bool bData)
{
	QSail7 *pSail7 = (QSail7*)s_pSail7;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, lenunit, massunit, speedunit;
	GetLengthUnit(lenunit, pMainFrame->m_LengthUnit);
	GetWeightUnit(massunit, pMainFrame->m_WeightUnit);
	GetSpeedUnit(speedunit, pMainFrame->m_SpeedUnit);

	CBoat *pBoat = pSail7->GetBoat(m_BoatName);
	if(!pBoat) return;
	PolarProperties.clear();


	strong = QObject::tr("Analysis Variables:");
	PolarProperties += strong + "\n";

	strong = QString::fromUtf8("  V: %1m/s / %2m/s").arg(m_QInfMin, 5,'f',1).arg(m_QInfMax, 5,'f',1);
	PolarProperties += strong + "\n";

	strong = QString::fromUtf8("  Beta: %1° / %2°").arg(m_BetaMin, 5,'f',1).arg(m_BetaMax, 5,'f',1);
	PolarProperties += strong + "\n";

	strong = QString::fromUtf8("  Phi: %1° / %2°").arg(-m_PhiMin, 5,'f',1).arg(-m_PhiMax, 5,'f',1);
	PolarProperties += strong + "\n";

	for(int is=0; is<pBoat->m_poaSail.size();is++)
	{
		QSail*pSail =(QSail*)pBoat->m_poaSail.at(is);
		strong  = "  "+QString(pSail->m_SailName + QString::fromUtf8(" = %1° / %2°"))
							   .arg(-m_SailAngleMin[is],5,'f',1)
							   .arg(-m_SailAngleMax[is],5,'f',1);
		PolarProperties += strong + "\n";

	}

	PolarProperties +="\n";

	strong  = QString(QObject::tr("CoG")+" = (%1 ").arg(m_CoG.x*pMainFrame->m_mtoUnit,6,'f',2);
	PolarProperties += strong + lenunit;

	strong  = QString(", %1 ").arg(m_CoG.z*pMainFrame->m_mtoUnit,6,'f',2);
	PolarProperties += strong + lenunit + ")\n";
	if(m_bGround)
	{
		strong = QObject::tr("Ground effect");
		PolarProperties += strong +"\n";
	}
	PolarProperties += "\n";

	PolarProperties += QObject::tr("Wind gradient")+":\n";

	strong  = QString("  pt 1 = (%1 "+lenunit+", %2) \n")
						.arg(m_WindGradient[0][0]*pMainFrame->m_mtoUnit,5,'f',1)
						.arg(m_WindGradient[0][1],5,'f',2);
	PolarProperties += strong;

	strong  = QString("  pt 2 = (%1 "+lenunit+", %2) \n")
						.arg(m_WindGradient[1][0]*pMainFrame->m_mtoUnit,5,'f',1)
						.arg(m_WindGradient[1][1],5,'f',2);
	PolarProperties += strong;
	PolarProperties += "\n";


	strong  = QString(QObject::tr("Density =")+"%1 kg/m3\n").arg(m_Density,12,'g',4);
	PolarProperties += strong;

	strong  = QString(QObject::tr("Viscosity =")+"%1").arg(m_Viscosity,12,'g',4);
	strong +=  "m"+QString::fromUtf8("²")+"/s\n";
	PolarProperties += strong;


	strong = QString(QObject::tr("Data points") +" = %1\n").arg(m_Ctrl.size());
	PolarProperties += "\n"+strong;

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
	Export(out, FileType, true);
	PolarProperties += "\n"+strong;
}


void BoatPolar::Export(QTextStream &out, int FileType, bool bDataOnly)
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int j;
	QString Header, strong, str;

	if (FileType==1)
	{
		if(!bDataOnly)
		{
			strong =pMainFrame->m_VersionName + "\n\n";
			out << strong;

			out << m_BoatName<<"\n";
			out << m_BoatPolarName<<"\n";

			GetSpeedUnit(str, pMainFrame->m_SpeedUnit);

			out << "\n";
		}


		Header = "     Ctrl      Lift       Drag       FFFx         FFFy      FFFz       Fx         Fy         Fz         Mx         My         Mz    \n";
		out << Header;
		for (j=0; j<m_Ctrl.size(); j++)
		{
			strong = QString(" %1  %2  %3  %4  %5  %6  %7  %8  %9  %10  %11  %12\n")
					 .arg(m_Ctrl[j],8,'f',3)
					 .arg(m_Lift[j], 9,'f',3)
					 .arg(m_Drag[j], 9,'f',3)
					 .arg(m_FFFx[j], 9,'f',3)
					 .arg(m_FFFy[j],9,'f',3)
					 .arg(m_FFFz[j],9,'f',3)
					 .arg(m_Fx[j],9,'f',3)
					 .arg(m_Fy[j] ,9,'f',3)
					 .arg(m_Fz[j],9,'f',3)
					 .arg(m_Mx[j],9,'f',3)
					 .arg(m_My[j],9,'f',3)
					 .arg(m_Mz[j],9,'f',3);

			out << strong;
		}
	}
	else if(FileType==2)
	{
		if(!bDataOnly)
		{
			strong =pMainFrame->m_VersionName + "\n\n";
			out << strong;
			out << m_BoatName<<"\n";
			out << m_BoatPolarName<<"\n";
		}

		Header = "Ctrl, FFFx, FFFy, FFFz, Fx, Fy, Fz, Mx, My, Mz\n";
		out << Header;
		for (j=0; j<m_Ctrl.size(); j++)
		{
//			strong.Format(" %8.3f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %8.4f,  %9.4f\n",
			strong = QString(" %1,  %2,  %3,  %4,  %5,  %6,  %7,  %8,  %9,  %10,  %11,  %12\n")
					   .arg(m_Ctrl[j],8,'f',3)
					   .arg(m_Lift[j], 9,'f',3)
					   .arg(m_Drag[j], 9,'f',3)
					   .arg(m_FFFx[j], 9,'f',3)
					   .arg(m_FFFy[j],9,'f',3)
					   .arg(m_FFFz[j],9,'f',3)
					   .arg(m_Fx[j],9,'f',3)
					   .arg(m_Fy[j] ,9,'f',3)
					   .arg(m_Fz[j],9,'f',3)
					   .arg(m_Mx[j],9,'f',3)
					   .arg(m_My[j],9,'f',3)
					   .arg(m_Mz[j],9,'f',3);

			out << strong;

		}
	}
	out << "\n\n";
}


void BoatPolar::ResetBoatPlr()
{
	m_Ctrl.clear(); // The main parameter for the polar
	m_Lift.clear();
	m_Drag.clear();
	m_FFFx.clear();    // resulting force in the far field
	m_FFFy.clear();
	m_FFFz.clear();
	m_Fx.clear();    // resulting force from panel force Summation
	m_Fy.clear();
	m_Fz.clear();
	m_Mx.clear();    // resulting moment from panel force Summation w.r.t. CoG
	m_My.clear();
	m_Mz.clear();
}



double BoatPolar::WindFactor(double z)
{
	if(z>m_WindGradient[1][0]) return 1.0;
	else
	{
		double xa, xb, ya, yb;

		xa = m_WindGradient[0][0];    ya = m_WindGradient[0][1];
		xb = m_WindGradient[1][0];    yb = m_WindGradient[1][1];

		aw = (ya - yb)                          / (-2*xb*xa +  xb*xb +  xa*xa);
		bw = -2*xb* (ya - yb)                   / (-2*xb*xa +  xb*xb +  xa*xa);
		cw = (xb*xb*ya - 2*yb*xb*xa + yb*xa*xa) / (-2*xb*xa +  xb*xb +  xa*xa);

		return aw*z*z+bw*z+cw;
	}
	return 1.0;
}
