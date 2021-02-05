/****************************************************************************

         CSail Class
         Copyright (C) 2011-2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/



#ifndef SAIL_H
#define SAIL_H

#include "sailsection.h"
#include "./boatpolar.h"
#include "./panel.h"
#include <QList>
#include <QFile>


typedef enum {NURBSSAIL, SAILCUTSAIL} enumSailType;// the type of sail definition

class MainFrame;
class BoatAnalysisDlg;

class Sail
{
    friend class MainFrame;
    friend class Sail7;
    friend class Boat;
    friend class SailDlg;
    friend class SailViewWt;
    friend class BoatDlg;
    friend class BoatPolarDlg;
    friend class BoatAnalysisDlg;
    friend class BoatPolar;
    friend class BoatOpp;
    friend class SectionViewWidget;
    friend class NURBSSail;
    friend class SailcutSail;
    friend class SailDomDoc;
    friend class glSailView;

public:
    Sail();
    virtual ~Sail() {}

    bool IsSailPanel(int nPanel) const;
    bool IsSailNode(int nNode) const;
    bool IsSailcutSail() const {return m_SailType==SAILCUTSAIL;}
    bool IsNURBSSail() const {return m_SailType==NURBSSAIL;}

    void PanelTrefftz(Vector3d VInf, double *Mu, double *Sigma, int pos, Vector3d &FFForce, BoatPolar *pBoatPolar);
    void PanelComputeOnBody(Vector3d VInf, double *Cp, double *Gamma, Vector3d &F, Vector3d &M, BoatPolar *pBoatPolar, Vector3d CoG);
    void SortSections();
    void SetLuffAngle(double Angle);
    void SetLuffAngle();
    virtual void ScaleSail(double XFactor, double YFactor, double ZFactor)=0;

    virtual void Duplicate(Sail *pSail)=0;
    virtual bool Import(QFile *pFile)=0;
    virtual void Export()=0;

    virtual double LuffLength()=0;
    virtual double LeechLength()=0;
    virtual double FootLength()=0;
    virtual double Chord(double zrel)=0;

    virtual Vector3d GetPoint(double xrel, double zrel)=0;
    virtual Vector3d GetSectionPoint(int iSection, double xrel)=0;
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
    Vector3d m_LEPosition;

    //relative position of corner points in the sail's referential
    Vector3d tack, head, clew, peak;

    //Global geometry
    double m_LuffAngle;

    //sections
    QList <SailSection*> m_oaSection;


    /** Variables required for VLM analysis*/
    int m_NElements, m_FirstPanel, m_NStation;
    int m_NXPanels;         // VLM Panels along horizontal direction
    int m_NZPanels;         // VLM Panels along vertical direction
    //    enumPanelDistrib m_XPanelDist;        // VLM Panel distribution type, along horizontal direction

    //    CVector m_SailNormal;

    Vector3d m_Vd[MAXSAILSTATIONS];        // downwash vector at span stations

    CPanel *m_pPanel;            //pointer to the VLM Panel array
    static BoatAnalysisDlg *s_pBoatAnalysisDlg;
    static MainFrame *s_pMainFrame;
};

#endif // SAIL_H
