/****************************************************************************

    BoatOpp Class
         Copyright (C) 2011-2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/



#ifndef BOATOPP_H
#define BOATOPP_H

#include "../params.h"
#include "../objects/vector3d.h"

#include <QString>
#include <QColor>



class BoatOpp
{
    friend class MainFrame;
    friend class Sail7;
    friend class BoatPolar;

    public:
        BoatOpp();
        bool SerializeBoatOpp(QDataStream &ar, bool bIsStoring);
        void GetBoatOppProperties(QString &BOppProperties);

        void GetLiftDrag(double &Lift, double &Drag, Vector3d &WindDirection, Vector3d &WindNormal, Vector3d &WindSide);

        double Lift();
        double Drag();

        static void *s_pMainFrame;
        static void *s_pSail7;


    private:
        QString m_BoatName;	// the boat name to which the BoatOpp belongs
        QString m_BoatPolarName; 	// the polar name to which the WOpp belongs

        bool m_bIsVisible;	// true if the curves should be displayed in the WOpp graphs
        bool m_bShowPoints;	// true if the curve' points should be displayed in the WOpp graphs
        bool m_bVLM1;		// true if the WOpp is the result to a classic VLM calculation

        int m_Style;		//WOpp display style
        int m_Width;
        QColor m_Color;

        int m_nWakeNodes;	// number of wake nodes
        int m_NXWakePanels;	//
        double m_FirstWakePanel;
        double m_WakeFactor;

        double m_Weight;


    private:

        int m_NVLMPanels;	// number of VLMpanels
        int m_Type;		// Polar Analysis Type
        int m_NStation;		// number of stations along wing span

        double m_QInf;
        double m_Cp[VLMMAXMATSIZE];		// lift coeffs for each panel
        double m_G[VLMMAXMATSIZE];			// vortice or doublet strengths
        double m_Sigma[VLMMAXMATSIZE];		// source strengths

        double m_Beta;//heading angle, degerees
        double m_Phi;//bank angle, degrees
        double m_Ctrl;		//control variable - converged value
        double m_SailAngle[MAXSAILS];// Sail rotation around the mast, degrees


        Vector3d F;  // resulting force from panel force Summation
        Vector3d M;  // resulting moment from panel force Summation w.r.t. CoG

        Vector3d ForceTrefftz; // resulting force in the far field plane

};

#endif // BOATOPP_H
