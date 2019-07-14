/****************************************************************************

    CBody Class
    Copyright (C) 2007-2012 Andre Deperrois adeperrois@xflr5.com

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

#ifndef CBODY_H
#define CBODY_H

#include "panel.h"
#include "nurbssurface.h"
#include <QTextStream>
#include <QColor>


class Body
{
public:
    Body();

    bool Gauss(double *A, int n, double *B, int m);
    bool IsInNURBSBody(Vector3d Pt);
    bool Intersect(Vector3d A, Vector3d B, Vector3d &I, bool bRight);
    bool IntersectPanels(Vector3d A, Vector3d B, Vector3d &I);
    bool IntersectNURBS(Vector3d A, Vector3d B, Vector3d &I, bool bRight);
    bool SerializeBody(QDataStream &ar, bool bIsStoring, int ProjectFormat=5);
    bool ImportDefinition(QTextStream &inStream, double mtoUnit);
    bool ExportDefinition() ;


    int InsertFrame(Vector3d Real);
    int InsertPoint(Vector3d Real);
    int IsFramePos(Vector3d Real, double ZoomFactor);
    int RemoveFrame(int n);
    int ReadFrame(QTextStream &in, int &Line, CFrame *pFrame, double const &Unit);

    double Length();
    double Getu(double x);
    double Getv(double u, Vector3d r, bool bRight);
    double GetSectionArcLength(double x);

    Vector3d LeadingPoint();

    void ComputeAero(double *Cp, double &XCP, double &YCP, double &ZCP,
                     double &GCm, double &GRm, double &GYm, double &Alpha, Vector3d &CoG);
//    void ComputeCenterLine();
    void Duplicate(Body *pBody);
    void ExportGeometry(QTextStream &outStream, int type, double mtoUnit, int nx, int nh);
    void GetPoint(double u, double v, bool bRight, Vector3d &Pt);
    void InsertSideLine(int SideLine);
    void InterpolateCurve(Vector3d *D, Vector3d *P, double *v, double *knots, int degree, int Size);
    void InterpolateSurface();
    void RemoveActiveFrame();
    void RemoveSideLine(int SideLine);
    void Scale(double XFactor, double YFactor, double ZFactor, bool bFrameOnly=false, int FrameID=0);
    void Translate(double XTrans, double YTrans, double ZTrans, bool bFrameOnly=false, int FrameID=0);
    void Translate(Vector3d T, bool bFrameOnly=false, int FrameID=0);
    void SetKnots();
    void SetPanelPos();
    void SetEdgeWeight(double uw, double vw);

    CFrame *Frame(int k);
    CFrame *ActiveFrame();
    void SetActiveFrame(CFrame *pFrame);
    double FramePosition(int iFrame);
    int FrameSize()       {return m_SplineSurface.FrameSize();};
    int FramePointCount() {return m_SplineSurface.FramePointCount();};
    int SideLineCount()   {return m_SplineSurface.FramePointCount();};// same as FramePointCount();

    void ComputeBodyAxisInertia();
    void ComputeVolumeInertia(Vector3d &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz);
    double TotalMass();


    //____________________VARIABLES_____________________________________________
    static void* s_pMainFrame;        //pointer to the Frame window

    NURBSSurface m_SplineSurface;

//    int m_NSideLines;
//    int m_NStations;            // the number of stations along x-axis where frames are defined
    int m_iActiveFrame;        // the currently selected frame for display
    int m_iHighlight;        // the currently selected frame for display
    int m_LineType; //1=lines  2=B-Splines
    int m_iRes; //for splines
    int m_NElements;// = m_nxPanels * m_nhPanels *2
    int m_nxPanels, m_nhPanels;
    int m_BodyStyle, m_BodyWidth;

    double m_Bunch;

    Vector3d m_CoG;
    double m_VolumeMass, m_TotalMass;        //for inertia calculations
//    int m_NMass; //number of point mass values
    QList<double> m_MassValue;
    QList<Vector3d> m_MassPosition;
    QStringList m_MassTag;
    double m_CoGIxx,m_CoGIyy,m_CoGIzz,m_CoGIxz;

    QString m_BodyName;
    QString m_BodyDescription;

    QColor m_BodyColor;


    int m_xPanels[MAXBODYFRAMES];
    int m_hPanels[MAXSIDELINES];

    Vector3d m_LEPosition;

    CPanel *m_pPanel;

    //allocate temporary variables to
    //avoid lengthy memory allocation times on the stack
    double value, eps, bs, cs;
    Vector3d t_R, t_Prod, t_Q, t_r, t_N;
//    CVector P0, P1, P2, PI;
    static double s_XPanelPos[300];

};
#endif

