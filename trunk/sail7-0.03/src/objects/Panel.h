/****************************************************************************

    CPanel Class
	Copyright (C) 2006 Andre Deperrois sail7@xflr5.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/



#ifndef CPANEL_H
#define CPANEL_H


#include "Quaternion.h"
#include "CVector.h"

typedef enum {UNIFORM, SINE, MSINE, COSINE} enumPanelDistrib;// the type of panel spacing along chord or span
typedef enum {BOTSURFACE, MIDSURFACE, TOPSURFACE, SIDESURFACE, BODYSURFACE} enumPanelPosition;

class CPanel
{
	friend class MainFrame;
	friend class CSurface;
	friend class QSail7;
	friend class BoatAnalysisDlg;
	friend class CSail;
	friend class CBody;

public:
	CPanel();

	void RotateBC(CVector const &HA, Quaternion & Qt);
	void Reset();
	void SetFrame();
	void SetFrame(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB);

	bool Intersect(CVector const &A, CVector const &U, CVector &I, double &dist);
	bool Invert33(double *l);

	bool IsBotSurface() {return m_Pos==BOTSURFACE;}
	bool IsMidSurface() {return m_Pos==MIDSURFACE;}
	bool IsTopSurface() {return m_Pos==TOPSURFACE;}
	bool IsSideSurface(){return m_Pos==SIDESURFACE;}
	bool IsBodySurface(){return m_Pos==BODYSURFACE;}

	CVector GlobalToLocal(CVector const &V);
	CVector GlobalToLocal(double const &Vx, double const &Vy, double const &Vz);
	CVector LocalToGlobal(CVector const &V);

	double Width();
	double GetArea();

	void DoubletNASA4023(CVector const &C, CVector &V, double &phi, bool bWake);
	void SourceNASA4023(CVector const &C, CVector &V, double &phi);
protected:	
	bool m_bIsInSymPlane;
	bool m_bIsLeftPanel;
	bool m_bIsWakePanel;

	int m_iElement; // panel identification number ; used when panel array is re-arranged in non sequential order
	int m_iSym; //reference of the symetric panel, or -1 if none
	int m_iPL, m_iPR, m_iPU, m_iPD;//Panels left, rigth; upstream, downstream
	int m_iWake;//-1 if not followed by a wake panel, else equal to wake panel number
	int m_iWakeColumn;

	//Local frame of refernce
	CVector VortexPos;
	CVector Vortex;
	CVector P1, P2, P3, P4;//point's local coordinates
	CVector m, l;

	double dl;
	double Area;
	double Size, SMP, SMQ;
	double lij[9];

	static CVector *s_pNode;
	static CVector smp, smq, MidA, MidB;
	static CVector ILA, ILB, ITA, ITB, Tt, V, W, P, LATB, TALB;

	static double s_pCoreSize;
	static double s_VortexPos;//between 0 and 1
	static double s_CtrlPos;//between 0 and 1
	static double det;
	static double mat[9];

public:
	enumPanelPosition m_Pos;

	bool m_bIsLeading, m_bIsTrailing;
	int m_iLA, m_iLB, m_iTA, m_iTB;//Corner Node numbers
	CVector Normal;
	CVector CtrlPt;
	CVector CollPt;
	CVector VA, VB;

	static CVector m_R[5];
	static double RFF, eps;
	static double side, sign, dist, S, GL;
	static double RNUM, DNOM, PN, DA, DB, PA, PB, SM, SL, AM, AL, Al, pjk, CJKi;
	static CVector PJK, a, b, s, T, T1, T2, h;

};

#endif
