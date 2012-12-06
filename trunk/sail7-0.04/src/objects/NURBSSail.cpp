/****************************************************************************

		 NURBSSail Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#include "../globals.h"
#include "../mainframe.h"
#include "NURBSSail.h"
#include <QFileDialog>
#include <QtDebug>




NURBSSail::NURBSSail()
{
	m_SailType = NURBSSAIL;
	m_bAlignedLE = true;

	m_oaSection.clear();

	SailSection *pSection = new SailSection(this);
	pSection->SetSection(0, 5, 0.0, 0.0);
	pSection->AppendPoint(CVector(0.0, 0.0, 0.0));
	pSection->AppendPoint(CVector(1.0, 0.41, 0.0));
	pSection->AppendPoint(CVector(5.0, 0.39, 0.0));
	InsertSection(pSection);

	pSection = new SailSection(this);
	pSection->SetSection(3.5, 3, 0.0, 0);
	pSection->AppendPoint(CVector(0.5, 0.0, 3.5));
	pSection->AppendPoint(CVector(0.9, 0.3, 3.5));
	pSection->AppendPoint(CVector(4.1, 0.39, 3.5));
	InsertSection(pSection);

	pSection = new SailSection(this);
	pSection->SetSection(7.0, 1, 5.0, 0);
	pSection->AppendPoint(CVector(1.5, 0.0, 7.0));
	pSection->AppendPoint(CVector(2.3, 0.1, 7.0));
	pSection->AppendPoint(CVector(3.0, 0.1,  7.0));
	InsertSection(pSection);

	m_SplineSurface.m_uAxis=2;
	m_SplineSurface.m_iuDegree = 2;
	m_SplineSurface.m_ivDegree = 2;

	SplineSurface();
}



CVector NURBSSail::GetPoint(double xrel, double zrel)
{
	CVector Pt;
	m_SplineSurface.GetPoint(zrel, xrel, Pt);
	return Pt;
}

/*

CVector NURBSSail::GetSectionPoint(int iSection, double xrel)
{
	if(iSection>=m_oaSection.size()) return CVector(0.0,0.0,0.0);
	CVector Pt;
	SailSection *pSection = m_oaSection.at(iSection);
	double u0  = m_SplineSurface.Getu(pSection->m_CtrlPoint.first().z, 0.0);
	double u1  = m_SplineSurface.Getu(pSection->m_CtrlPoint.last().z,  .99999);
	m_SplineSurface.GetPoint(u0 * (1.0-xrel) + u1*xrel, xrel, Pt);
	return Pt;
}*/


CVector NURBSSail::GetSectionPoint(SailSection *pSailSection, double xrel)
{
	int iSection = m_oaSection.indexOf(pSailSection);
	if(iSection>0) return GetSectionPoint(iSection, xrel);
	else           return CVector(0.0,0.0,0.0);
}



CVector NURBSSail::GetSectionPoint(int iSection, double xrel)
{
	if(iSection>=m_oaSection.size()) return CVector(0.0,0.0,0.0);

	if(iSection==0)	                         return GetPoint(xrel, 0.0);
	else if(iSection==m_oaSection.size()-1)  return GetPoint(xrel, 1.0);

	//middle section, needs interpolation
	SailSection *pSection = m_oaSection.at(iSection);

	double z = pSection->m_Position.z + xrel*pSection->Chord() *sin(-pSection->m_Angle*PI/180.0);
	double u=0.5, u0=0.0, u1=1.0, ulast=0.0;
	double dmax=1000.0;
	int iter=0;
	CVector Pt, Pt0, Pt1;
	Pt0 = GetPoint(xrel, u0);
	Pt1 = GetPoint(xrel, u1);
	//todo : use u from previous calculation to speed up
	u = u0 + (u1-u0)/(Pt1.z -Pt0.z) * (z-u0);

	while (dmax>.01  && iter<10)
	{
		Pt = GetPoint(xrel, u);
		if(Pt.z<=z)
		{
			u0 = u;
			Pt0 = Pt;
		}
		else
		{
			u1 = u;
			Pt1 = Pt;
		}
		ulast = u;
		u = u0 + (u1-u0) / (Pt1.z -Pt0.z) * (z-Pt0.z);
		dmax = fabs(u-ulast);
		iter++;
//		qDebug("%3d    %13.6f    %13.6f    %13.6f", iter , u0 , u1 , u);
	}
//	qDebug()<<"_____________";
	return Pt;
}


/*
CVector NURBSSail::GetSectionPoint(int iSection, double xrel)
{
	if(iSection>=m_oaSection.size()) return CVector(0.0,0.0,0.0);

	if(iSection==0)	                         return GetPoint(xrel, 0.0);
	else if(iSection==m_oaSection.size()-1)  return GetPoint(xrel, 1.0);

	//middle section, needs interpolation
	SailSection *pSection = m_oaSection.at(iSection);


	double z = pSection->m_Position.z + xrel*pSection->Chord() *sin(-pSection->m_Angle*PI/180.0);
	double u=0.5, u0=0.0, u1=1.0;
	double dmax=1000.0;
	int iter=0;
	CVector Pt, Pt0, Pt1;
	Pt0 = GetPoint(xrel, u0);
	Pt1 = GetPoint(xrel, u1);

	while (dmax>.01  && iter<20)
	{
		u = (u0+u1)/2.0;
		Pt = GetPoint(xrel, u);
		if(Pt.z<=z)  u0 = u;	else u1 = u;
		dmax = fabs(u0-u1);
		iter++;
		qDebug("%3d    %13.6f    %13.6f    %13.6f", iter , u0 , u1 , u);
	}
	qDebug()<<"_____________";
	return Pt;
}*/


void NURBSSail::CreateSection(int iSection)
{
	SailSection *pNewSection = new SailSection(this);

	if(iSection>=m_oaSection.size())
	{
		pNewSection->Copy(m_oaSection.last());
		pNewSection->m_Position.z += 1.0;
		m_oaSection.append(pNewSection);
	}
	else
	{
		SailSection *pCurSection = m_oaSection.at(iSection);
		SailSection *pPrevSection=NULL;
		if(iSection>0) pPrevSection = m_oaSection.at(iSection-1);

		if(pPrevSection)
		{
			pNewSection->m_Position.z  = (pCurSection->m_Position.z+pPrevSection->m_Position.z)  /2.0;
//			pNewSection->m_Chord = (pCurSection->m_Chord+pPrevSection->m_Chord)/2.0;
//			pNewSection->m_Twist = (pCurSection->m_Twist+pPrevSection->m_Twist)/2.0;
			pNewSection->m_Angle = (pCurSection->m_Angle+pPrevSection->m_Angle)/2.0;

			pNewSection->m_CtrlPoint.clear();
			for(int ic=0; ic<pCurSection->m_CtrlPoint.count(); ic++)
			{
				pNewSection->m_CtrlPoint.append(pCurSection->m_CtrlPoint.at(ic));
			}
		}
		else
		{
			pNewSection->Copy(pCurSection);
			pNewSection->m_Position.z -=1.0;
		}
		m_oaSection.insert(iSection, pNewSection);
	}

	for(int ic=0; ic<pNewSection->m_CtrlPoint.count(); ic++)
	{
		pNewSection->m_CtrlPoint[ic].z = pNewSection->m_Position.z;
	}

	SplineSurface();
}


void NURBSSail::DeleteSection(int iSection)
{
	m_oaSection.removeAt(iSection);
	SplineSurface();
}


void NURBSSail::SplineSurface()
{
	if(!m_oaSection.size()) return;
	if(! m_oaSection.first()->m_CtrlPoint.size()) return;

	// set unused corner points
	tack = m_oaSection.first()->m_CtrlPoint.first();
	clew = m_oaSection.first()->m_CtrlPoint.last();
	clew.RotateY(m_oaSection.first()->m_Position, m_oaSection.first()->m_Angle);
	head = m_oaSection.last()->m_CtrlPoint.first();
	peak = m_oaSection.last()->m_CtrlPoint.last();
	peak.RotateY(m_oaSection.last()->m_Position, m_oaSection.last()->m_Angle);

	//Set the number of control points in each direction
//	m_SplineSurface.m_nvLines = m_oaSection.first()->m_CtrlPoint.size();//assumes all sections have the same number of control points--> TODO force

	m_SplineSurface.m_pFrame.clear();
	for(int is=0; is<m_oaSection.size(); is++)
	{
		m_SplineSurface.m_pFrame.append(m_oaSection.at(is));
	}

	for(int is=0; is<m_oaSection.size(); is++)
	{
		SailSection *pSection = m_oaSection.at(is);
		pSection->m_bIsTipSection = (is==0)||(is==m_oaSection.size()-1);
	}
	//set the knots and we're done
	m_SplineSurface.SetKnots();
}



bool NURBSSail::SerializeSail(QDataStream &ar, bool bIsStoring)
{
	//saves or loads the wing data to the archive ar

//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int ArchiveFormat;// identifies the format of the file

	if(bIsStoring)
	{	// storing code
		ar << 133304;
		//133304 : no change, but does not require luff angle calc for NURBS sails
		//133303 : first operational release- requires luff angle calculation for NURBS sails
		//133302 : twist data
		//133301 : initial format

		WriteCString(ar, m_SailName);
		WriteCString(ar, m_SailDescription);
		ar << m_SailColor.red()<< m_SailColor.green()<< m_SailColor.blue()<< m_SailColor.alpha();
		ar << m_NXPanels;
		ar << 0.0;
		ar << m_LEPosition.x << m_LEPosition.z;

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
		double d;

		ar >> ArchiveFormat;

		if (ArchiveFormat<133303)  return false;

		ReadCString(ar, m_SailName);
		ReadCString(ar, m_SailDescription);
		ar>>i>>j>>k>>l;
		m_SailColor = QColor(i,j,k,l);

		ar >> m_NXPanels;
		ar >> d;
		ar >> m_LEPosition.x >> m_LEPosition.z;

		ar >> l;
		m_oaSection.clear();
		for(int is=0; is<l; is++)
		{
			SailSection *pSailSection =  new SailSection(this);
			pSailSection->SerializeSection(ar, bIsStoring);
			m_oaSection.append(pSailSection);
		}

		SplineSurface();

		if (ArchiveFormat<133304 && IsNURBSSail())
		{
			// compute luff angle
			CVector LE = m_SplineSurface.LeadingEdgeAxis();
			m_LuffAngle = atan2(LE.x, LE.z) * 180./PI;
		}
		return true;
	}
 }



void NURBSSail::Duplicate(QSail *pSail)
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
	SplineSurface();

}


bool NURBSSail::Import(QFile *pFile)
{
	return false;
}


void NURBSSail::Export()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	QString fileName = "/home/windsoarer/sail7.saildef";
	QString fileName = QFileDialog::getSaveFileName(pMainFrame, QObject::tr("Save Sailcut File"),
								 QDir::currentPath(),
								 QObject::tr("saildef (*.x3d *.xml)"));
	if (fileName.isEmpty()) return;

	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		return;
	}


	file.close();

	pMainFrame->statusBar()->showMessage("File saved", 5000);
}


double NURBSSail::Chord(double zrel)
{
	return (GetPoint(1.0, zrel) - GetPoint(0.0, zrel)).VAbs();
}


double NURBSSail::LuffLength()
{
	if(!m_oaSection.size()) return 0.0;
	return (m_oaSection.last()->m_CtrlPoint.first() - m_oaSection.first()->m_CtrlPoint.first()).VAbs();
}


double NURBSSail::LeechLength()
{
	if(!m_oaSection.size()) return 0.0;
	if(!m_oaSection.first()->m_CtrlPoint.size()) return 0.0;

	if(!m_oaSection.last()->m_CtrlPoint.size() || !m_oaSection.first()->m_CtrlPoint.size()) return 0.0;

	return (m_oaSection.last()->m_CtrlPoint.last()-m_oaSection.first()->m_CtrlPoint.last()).VAbs();
}


double NURBSSail::FootLength()
{
	if(!m_oaSection.size() || !m_oaSection.first()->m_CtrlPoint.size()) return 0.0;
	return (m_oaSection.first()->m_CtrlPoint.last()-m_oaSection.first()->m_CtrlPoint.first()).VAbs();
}



void NURBSSail::ScaleSail(double XFactor, double YFactor, double ZFactor)
{
	for(int is=0; is<m_oaSection.size(); is++)
	{
		SailSection *pSection = m_oaSection.at(is);
		for(int ic=0; ic<pSection->m_CtrlPoint.size(); ic++)
		{
			pSection->m_CtrlPoint[ic].x *= XFactor;
			pSection->m_CtrlPoint[ic].y *= YFactor;
			pSection->m_CtrlPoint[ic].z *= ZFactor;
		}

		pSection->m_Position.x *=XFactor;
		pSection->m_Position.y *=YFactor;
		pSection->m_Position.z *=ZFactor;
	}
	SplineSurface();
}


void NURBSSail::DrawFrame(int iSection, QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset, bool bSelected)
{
	// cuts the spline surface along the section with index iSection
	// finds the u0 nd u1 parameters at each end of the section
	// projects the points on the section's xy plane
	if(iSection<0 || iSection>=m_SplineSurface.FrameSize()) return;
	CFrame *pFrame = m_SplineSurface.m_pFrame.at(iSection);
	if(!pFrame->m_CtrlPoint.size()) return;
	painter.save();

	QPoint From, To;
	CVector Pt, is;
	double px;

	is.x = pFrame->m_CtrlPoint.last().x - pFrame->m_CtrlPoint.first().x;
	is.y = 0.0;
	is.z = pFrame->m_CtrlPoint.last().z - pFrame->m_CtrlPoint.first().z;
	is.Normalize();

	Pt = GetSectionPoint(iSection, 0.0);
	px = Pt.x*is.x + (Pt.z-pFrame->m_Position.z)*is.z;

	//use the drawing opportunity to get the section's camber
	m_oaSection[iSection]->m_c = Pt.y;
	m_oaSection[iSection]->m_x = px;

	From.rx() = (int)(   px * scalex + Offset.x());
	From.ry() = (int)(-Pt.y * scaley + Offset.y());

	double x0 = px;

	int N=29;
	for(int iv=1; iv<=N; iv++)
	{
		double v= (double)iv /(double)N;

		Pt = GetSectionPoint(iSection, v);


		//project Pt on section transverse plane.
		px = Pt.x*is.x + (Pt.z-pFrame->m_Position.z)*is.z;
		To.rx() = (int)(   px * scalex + Offset.x());
		To.ry() = (int)(-Pt.y * scaley + Offset.y());

		if(fabs(Pt.y)>m_oaSection[iSection]->m_c)
		{
			m_oaSection[iSection]->m_c = Pt.y;
			m_oaSection[iSection]->m_x = px;
		}

		painter.drawLine(From, To);
		From = To;
	}

	m_oaSection[iSection]->m_c *=1.0/(px-x0);
	m_oaSection[iSection]->m_x *=1.0/(px-x0);

	painter.restore();
}














