/****************************************************************************

		 SailcutSail Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include "../mainframe.h"
#include "SailcutSpline.h"
#include "SailcutSail.h"
#include "../globals.h"
#include "../sail7/SailDomDoc.h"
#include <QFileDialog>
#include <QtDebug>



SailcutSail::SailcutSail()
{
	m_SailType = SAILCUTSAIL;
	m_oaSection.clear();

	//bottom section
	SailSection *pSection = new SailSection(this);
	pSection->SetSection(0, 5, 0.0, 11.0);
	pSection->m_SCSpline.SetSplineParams(1.0, .7, .0005);
	InsertSection(pSection);

	//mid section
	pSection = new SailSection(this);
	pSection->SetSection(2.5, 2.7, 0.0, 0.0);
	pSection->m_SCSpline.SetSplineParams(1.0, .5, .001);
	InsertSection(pSection);

	//top section
	pSection = new SailSection(this);
	pSection->SetSection(7.0, 1, 5.0, 0);
	pSection->m_SCSpline.SetSplineParams(1.0, .3, .003);
	InsertSection(pSection);

	m_LuffAngle = 17.0;

	SplineSurface();

	m_LeechRound = -0.5;
	m_LeechRoundPos = .4197;

	m_Twist = 11.0;    // reference position, 0°

	aru = bru = cru = 0.0;
	arl = brl = crl = 0.0;
	SetLeechRoundEquation();
}


void SailcutSail::Duplicate(QSail *pSail)
{
	m_SailName        = pSail->m_SailName;
	m_SailDescription = pSail->m_SailDescription;
	m_NXPanels        = pSail->m_NXPanels;
	m_NZPanels        = pSail->m_NZPanels;
	m_SailColor       = pSail->m_SailColor;
	m_LEPosition      = pSail->m_LEPosition;
	m_LuffAngle       = pSail->m_LuffAngle;


	m_oaSection.clear();
	for(int is=0; is<pSail->m_oaSection.size(); is++)
	{
		SailSection *pNewSection = new SailSection(this);
		pNewSection->Copy(pSail->m_oaSection.at(is));
		m_oaSection.append(pNewSection);
	}
	m_SailType = pSail->m_SailType;

	SailcutSail *pSCSail = (SailcutSail*)pSail;

	m_LeechRound      = pSCSail->m_LeechRound;
	m_LeechRoundPos   = pSCSail->m_LeechRoundPos;
	SetLeechRoundEquation();

	m_Twist = pSCSail->m_Twist;

	SplineSurface();
}




CVector SailcutSail::GetSectionPoint(int iSection, double xrel)
{
	double k, av, ar, a, b, c;
	SailSection *pSection;
	CVector Point;

	pSection = m_oaSection.at(iSection);

	k  = pSection->m_SCSpline.K;
	av = pSection->m_SCSpline.AV;
	ar = pSection->m_SCSpline.AR;
	a = 1+ av/4;
	b = a/((av+2) * (av+1));
	c = ar/6 - b;

	Point.Set(xrel,
			  k*(-a*pow((1-xrel),(av+2)) / (( av+2)*(av +1)) - ar/6*xrel*xrel*xrel + c*xrel + b),
			  0.0);
	Point *= Chord(pSection);
	Point.RotateY(Angle(pSection));
	Point.RotateZ(Twist(zrel(pSection->m_Position.z))); // TODO : or rotate around the mast ??
	Point.Translate(pSection->m_Position.z*tan(m_LuffAngle*PI/180.0), 0.0, pSection->m_Position.z);
	return Point;
}




bool SailcutSail::SerializeSail(QDataStream &ar, bool bIsStoring)
{
	//saves or loads the wing data to the archive ar

//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int ArchiveFormat;// identifies the format of the file

	if(bIsStoring)
	{	// storing code
		ar << 133303;
		//133302 : twist data
		//133301 : initial format

		WriteCString(ar, m_SailName);
		WriteCString(ar, m_SailDescription);
		ar << m_SailColor.red()<< m_SailColor.green()<< m_SailColor.blue()<< m_SailColor.alpha();
		ar << m_NXPanels;
		ar << m_LuffAngle;
		ar << m_LEPosition.x << m_LEPosition.z;

		ar << m_LeechRound << m_LeechRoundPos;

		ar << m_Twist;

		ar << m_oaSection.size();
		for(int is=0; is<m_oaSection.size(); is++)
		{
			SailSection *pSailSection =  (SailSection*)m_oaSection.at(is);
			pSailSection->SerializeSection(ar, bIsStoring);
		}
		return true;
	}
	else
	{
		// loading code
		int i,j,k,l;
		ar >> ArchiveFormat;

		if (ArchiveFormat<133301)  return false;

		ReadCString(ar, m_SailName);
		ReadCString(ar, m_SailDescription);
		ar>>i>>j>>k>>l;
		m_SailColor = QColor(i,j,k,l);

		ar >> m_NXPanels;
		ar >> m_LuffAngle;
		ar >> m_LEPosition.x >> m_LEPosition.z;

		ar >> m_LeechRound >> m_LeechRoundPos;

		ar >> m_Twist;

		ar >> l;
		m_oaSection.clear();
		for(int is=0; is<l; is++)
		{
			SailSection *pSailSection =  new SailSection(this);
			pSailSection->SerializeSection(ar, bIsStoring);
			m_oaSection.append(pSailSection);
		}

		SetLuffAngle();
		SetLeechRoundEquation();
		SplineSurface();
		return true;
	}
}




void SailcutSail::Export()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	QString fileName ="/home/windsoarer/sail7.saildef";
	QString fileName = QFileDialog::getSaveFileName(pMainFrame, "Save Sailcut File",
								 QDir::currentPath(),
								 "saildef (*.saildef *.xml)");
	if (fileName.isEmpty()) return;

	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		return;
	}

	SailDomDoc SailcutSailDomDoc(this);
	SailcutSailDomDoc.Export(&file);
	pMainFrame->statusBar()->showMessage("The file has been exported", 5000);
}


double SailcutSail::Twist(double zrel)
{
	//Returns the twist in degrees at the position defined by zrel of the luff
	return zrel*m_Twist;
}


double SailcutSail::Twist(SailSection *pSection)
{
	//Returns the twist in degrees at the position defined by percentHeight of the leech
	return zrel(pSection->m_Position.z) * m_Twist;
}



double SailcutSail::Angle(double zrel)
{
	//Returns the chord angle in degrees at the position defined by percentHeight of the leech
	return m_oaSection.first()->m_Angle *(1.0-zrel) + m_oaSection.last()->m_Angle *zrel;
}

double SailcutSail::Angle(SailSection *pSection)
{
	//Returns the chord angle in degrees at the position defined by percentHeight of the leech
	if(pSection==m_oaSection.first() || pSection==m_oaSection.last()) return pSection->m_Angle;
	else
	{
		double zp = zrel(pSection->m_Position.z);
		return m_oaSection.first()->m_Angle *(1.0-zp) + m_oaSection.last()->m_Angle *zp;
	}
}


void SailcutSail::SetLeechRoundEquation()
{
	// The equation describing the T.E twist is a parabola equation
	// defined by three points : clew, peak, and max round position
	//
	//     y = a*x*x + b*x + c
	//

	double xa, xb, xc, ya, yb, yc;

	xa = 0.0;                ya = 0.0;
	xb = m_LeechRoundPos;    yb = m_LeechRound;
	xc = 1.0;                yc = 0.0;

	arl = (ya - yb)                          / (-2*xb*xa +  xb*xb +  xa*xa);
	brl = -2*xb* (ya - yb)                   / (-2*xb*xa +  xb*xb +  xa*xa);
	crl = (xb*xb*ya - 2*yb*xb*xa + yb*xa*xa) / (-2*xb*xa +  xb*xb +  xa*xa);

	aru = (yc - yb)                          / (-2*xb*xc +  xb*xb +  xc*xc);
	bru = -2*xb* (yc - yb)                   / (-2*xb*xc +  xb*xb +  xc*xc);
	cru = (xb*xb*yc - 2*yb*xb*xc + yb*xc*xc) / (-2*xb*xc +  xb*xb +  xc*xc);
}



double SailcutSail::zrel(double z)
{
	if(!m_oaSection.size()) return 0.0;
	if(fabs(m_oaSection.last()->m_Position.z - m_oaSection.first()->m_Position.z)<PRECISION) return 0.0;
	else return z/fabs(m_oaSection.last()->m_Position.z - m_oaSection.first()->m_Position.z);
}

double SailcutSail::LuffLength()
{
	if(!m_oaSection.size()) return 0.0;
	return fabs(m_oaSection.last()->m_Position.z - m_oaSection.first()->m_Position.z)/cos(m_LuffAngle*PI/180.0);
}


double SailcutSail::LeechLength()
{
	if(!m_oaSection.size()) return 0.0;

	return(GetPoint(1.0, 1.0)-GetPoint(1.0,0.0)).VAbs();
}


double SailcutSail::FootLength()
{
	if(!m_oaSection.size()) return 0.0;
	return m_oaSection.first()->Chord();
}


bool SailcutSail::Import(QFile *pFile)
{
	if (!pFile->open(QIODevice::ReadOnly)) return false;

	SailDomDoc SailDocument(this);
	return SailDocument.Import(pFile);

}



void SailcutSail::ScaleSail(double XFactor, double YFactor, double ZFactor)
{
	for(int is=0; is<m_oaSection.size(); is++)
	{
		SailSection *pSection = m_oaSection.at(is);
		pSection->m_Chord *= XFactor;
		pSection->m_SCSpline.K *=YFactor;
		pSection->SetwPosition(pSection->m_Position.z*ZFactor);
	}
	SplineSurface();
}



double SailcutSail::Chord(SailSection *pSection)
{
	if(pSection==m_oaSection.first())     return m_oaSection.first()->Chord();
	else if(pSection==m_oaSection.last()) return m_oaSection.last()->Chord();

	return Chord(zrel(pSection->m_Position.z));
}



CVector SailcutSail::GetPoint(double xrel, double zrel)
{
	double tau, k, av, ar, a, b, c, yrel;
	SailSection *pSection0, *pSection1;
	SailcutSpline *pSpline0, *pSpline1;
	CVector Point;

	double z = zrel*fabs(m_oaSection.last()->m_Position.z - m_oaSection.first()->m_Position.z);

	// get the point on the straight line between tack and head
	CVector LEPoint = tack * (1.0-zrel) + head *zrel;

	// get the point on the straight line between clew and peak
	CVector TEPoint = clew * (1.0-zrel) + peak *zrel;

	//construct the outward normal to the straight line
	CVector Normal(peak.z-clew.z, 0.0, -peak.x+clew.x);
	Normal.Normalize();

	// add the sail round
	double r;
	if(zrel<m_LeechRoundPos) r = arl *zrel*zrel + brl * zrel + crl;
	else                     r = aru *zrel*zrel + bru * zrel + cru;
	TEPoint  += Normal *r;

	//interpolate between sections

	for(int is=0; is<m_oaSection.size()-1; is++)
	{
		pSection0 = m_oaSection[is];
		pSpline0 = &pSection0->m_SCSpline;

		pSection1 = m_oaSection.at(is+1);
		pSpline1 = &pSection1->m_SCSpline;

		if(pSection0->m_Position.z<=z && z<=pSection1->m_Position.z)
		{
			tau = (z-pSection0->m_Position.z)/(pSection1->m_Position.z - pSection0->m_Position.z);
			k  = (1.0-tau)*pSpline0->K  + tau*pSpline1->K;
			av = (1.0-tau)*pSpline0->AV + tau*pSpline1->AV;
			ar = (1.0-tau)*pSpline0->AR + tau*pSpline1->AR;
			a = 1+ av/4;
			b = a/((av+2) * (av+1));
			c = ar/6 - b;

			yrel = k*(-a*pow((1-xrel),(av+2)) / (( av+2)*(av +1)) - ar/6*xrel*xrel*xrel + c*xrel + b);
			Point.x = LEPoint.x * (1.-xrel) + TEPoint.x *xrel;
			Point.y = yrel * (LEPoint-TEPoint).VAbs();
			Point.z = LEPoint.z * (1.-xrel) + TEPoint.z *xrel;

			Point.RotateZ(LEPoint, -Twist(zrel)); // TODO : or rotate around the mast ??

			return Point;
		}
	}
	return CVector(0.0,0.0,0.0);
}



double SailcutSail::Chord(double zrel)
{
	if(zrel<=PRECISION) return m_oaSection.first()->Chord();
	if(zrel>=1.0)       return m_oaSection.last()->Chord();


	// get the point on the straight line between tack and head
	CVector LEPoint = tack * (1.0-zrel) + head *zrel;

	// get the point on the straight line between clew and peak
	CVector TEPoint = clew * (1.0-zrel) + peak *zrel;

	//construct the outward normal to the straight line
	CVector Normal(peak.z-clew.z, 0.0, -peak.x+clew.x);
	Normal.Normalize();

	// add the sail round
	double r;
	if(zrel<m_LeechRoundPos) r = arl *zrel*zrel + brl * zrel + crl;
	else                     r = aru *zrel*zrel + bru * zrel + cru;
	TEPoint  += Normal *r;

	return (LEPoint-TEPoint).VAbs();
}


CVector SailcutSail::SectionLE(int iSection)
{
	double zp = zrel(m_oaSection[iSection]->m_Position.z);
	return tack * (1.0-zp) + head *zp;
}

CVector SailcutSail::SectionTE(int iSection)
{
	double zp = zrel(m_oaSection[iSection]->m_Position.z);
	return clew * (1.0-zp) + peak *zp;
}

void SailcutSail::SplineSurface()
{
	// construct the end points
	tack.Set(0.0,0.0,0.0);

	double FootAngle = -m_oaSection.first()->m_Angle * PI/180.0;
	clew = tack + CVector(cos(FootAngle), 0.0, sin(FootAngle)) * m_oaSection.first()->m_Chord;

	double LuffAngle = m_LuffAngle *PI/180.0;
	head = tack + CVector(sin(LuffAngle), 0.0, cos(LuffAngle)) * LuffLength();

	double GaffAngle = -m_oaSection.last()->m_Angle * PI/180.0;
	peak = head + CVector(cos(GaffAngle), 0.0, sin(GaffAngle)) * m_oaSection.last()->m_Chord;
}















