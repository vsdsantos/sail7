/****************************************************************************

         Sail7
         Copyright (C) 2012 Andre Deperrois
         All rights reserved

*****************************************************************************/


#ifndef QSAIL7_H
#define QSAIL7_H

#include <QWidget>
#include <QPixmap>
#include <QCheckBox>
#include <QSlider>
#include <QStackedWidget>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QLabel>
#include <QList>
#include <QDialog>
#include <QDataStream>
#include <QSettings>
#include <QTimer>

#include "../objects/boatopp.h"
#include "../objects/boatpolar.h"
#include "../objects/sail.h"
#include "../objects/arcball.h"
#include "../objects/panel.h"
#include "../misc/w3dprefsdlg.h"
#include "../misc/floatedit.h"
#include "../misc/linebutton.h"
#include "../misc/linecbbox.h"
#include "../misc/linedelegate.h"
#include "../misc/gllightdlg.h"
#include "../graph/qgraph.h"
#include "boatanalysisDlg.h"


class MainFrame;
class TwoDWidget;
class glSail7View;

class Sail7 : public QWidget
{
    friend class MainFrame;
    friend class TwoDWidget;
    friend class ThreeDWidget;
    friend class glSail7View;
    friend class GL3DScales;
    friend class GLLightDlg;
    friend class SailPolarDlg;
    friend class BoatDlg;
    friend class BoatAnalysisDlg;
    friend class Sail;
    friend class Boat;
    friend class DisplaySettingsDlg;
    friend class SailDlg;
    friend class SailViewWt;
    friend class GL3dBodyDlg;

    Q_OBJECT


    public:
        Sail7(QWidget *parent = nullptr);
        ~Sail7();

    private slots:

        void OnSail3DView();
        void OnSailPolarView();

        void OnEditCurBoat();
        void OnRenameCurBoat();
        void OnDeleteCurBoat();
        void OnDuplicateCurBoat();
        void OnNewBoat();

        void OnDefineBoatPolar();
        void OnEditBoatPolar();
        void OnResetCurBoatPolar();
        void OnExportCurBoatOpp();
        void OnExportCurBoatPolar();

        void OnHideCurBoatPolars();
        void OnShowCurBoatPolars();
        void OnDeleteCurBoatPolars();
        void OnHideAllBoatPolars();
        void OnShowAllBoatPolars();


        void On3DIso();
        void On3DTop();
        void On3DLeft();
        void On3DFront();
        void On3DWindFront();
        void On3DWindRear();
        void On3DReset();

        void OnSetupLight();

        void OnStoreOpp();
        void OnSequence();

        void OnAxes();
        void OnPanels();
        void OnSurfaces();
        void OnOutline();
        void On3DPickCenter();

        void OnAnalyze();
        void OnReadAnalysisData();

        void OnCurveColor();
        void OnCurveStyle(int index);
        void OnCurveWidth(int index);
        void OnGraphSettings();
        void OnShowCurve();
        void OnPanelForce();
        void OnShowLift();
        void OnShowBodyForces();
        void OnShowWind();
        void OnShowWater();
        void On3DCp();
        void OnMoment();
        void OnStreamlines();
        void OnSurfaceSpeeds();
        void OnAnimateBoatOpp();
        void OnAnimateBoatOppSpeed(int pos);
        void OnVortices();
        void OnClipPlane(int pos);
        void OnLight();
        void OnOpenSail7File();
        void OnGL3DScale();
        void OnDeleteCurBoatOpp();
        void OnDeleteCurBoatPolar();
        void OnRenameCurBoatPolar();
        void OnDeleteAllBoatPolarOpps();
        void OnDeleteAllBoatOpps();
        void On3DPrefs();
        void OnResetBoatPlrLegend();
        void OnBoatOppProps();
        void OnBoatPolarProps();
        void OnAnimateBoatOppSingle();
        void OnSingleGraph1();
        void OnSingleGraph2();
        void OnSingleGraph3();
        void OnSingleGraph4();
        void OnTwoGraphs();
        void OnFourGraphs();
        void OnAllBoatPolarGraphSettings();
        void OnAllBoatPolarGraphScales();
        void OnResetGraphLegend();

    public:

        static MainFrame *s_pMainFrame;
        static TwoDWidget *s_p2DWidget;
        static glSail7View *s_pglSail7View;

        //____________________Methods________________________________________
        void contextMenuEvent (QContextMenuEvent * event);
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
        void wheelEvent(QWheelEvent *event);
        void mousePressEvent(QMouseEvent *pEvent);
        void mouseReleaseEvent(QMouseEvent *pEvent);
        void mouseDoubleClickEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void showEvent(QShowEvent *event);

        void GLDraw3D();
        void GLRenderView();
        void GLCreateVortices();
        void GLCreateCp(BoatOpp *pBoatOpp);
        void GLCreatePanelForces(BoatOpp *pBoatOpp);
        void GLCreateSurfSpeeds();
        void GLCreateCpLegendClr(QRect cltRect);

        void SetScale();
        void SetBoat(QString BoatName="");
        void SetBoatPolar(BoatPolar *pBoatPolar=nullptr, QString BoatPlrName = "");
        bool SetBoatOpp(bool bCurrent=true, double x=0);

        void GetDistrib(int const &NPanels, const int &DistType, const int &k, double &tau1, double &tau2);
        void PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence);
        void PaintView(QPainter &painter);
        void setupLayout();
        void SetControls();
        void SetViewControls();
        void SetBoatPolarGraphTitles(Graph* pGraph);
        void SnapClient(QString const &FileName);
        void StopAnimate();
        void UpdateView();
        void UpdateUnits();

        void connectSignals();

        void FillComboBoxes(bool bEnable = true);
        void UpdateCurve();

        BoatOpp* GetBoatOpp(double x);


        void LoadSettings(QSettings *pSettings);
        void SaveSettings(QSettings *pSettings);
        void Set3DRotationCenter();
        void Set3DRotationCenter(QPoint point);
        void Set2DScale();
        void Set3DScale();

        int CreateSailElements(Sail *pSail);
        int CreateBodyElements(Body *pBody);
        int IsNode(Vector3d &Pt);

        bool SetModBoat(Boat *pModBoat);
        bool SetModBoatPolar(BoatPolar *pModBoatPolar);


        QGraph* GetGraph(QPoint &pt);

        void GLCreateSailLists();
        void GLCreateBodyLists();
        void GLCallViewLists();
        void GLCreateSailMesh(Vector3d *pNode, CPanel *pPanel);
        void GLCreateSailGeom(GLuint GLList, Sail *pSail, Vector3d Position);
        void GLCreateStreamLines();

        /** Creates the OpenGl list for lift and drag arrows.  Uses the force calculed in the Trefftz plane.*/
        void GLDrawForces();
        void GLCreateBodyMesh(Vector3d* pNode, CPanel *pPanel);
        void GLCreatePanelNormals();

        QString makeBoatLegend();
        QString makeBoatOppLegend();

        void DrawBoatPolarLegend(QPainter &painter, QPoint place, int bottom);

        void RotatePanelsX(double const &Angle, Vector3d const &P);
        void RotatePanelsZ(double const &Angle, Vector3d const &P);
        void CreateBoatPolarCurves();
        void FillBoatPlrCurve(CCurve *pCurve, BoatPolar *pBoatPolar, enumPolarVar XVar, enumPolarVar YVar);
        void SetAnalysisParams();
        void SetCurveParams();
        void SetBoatPlrLegendPos();

        Boat* AddBoat(Boat *pNewBoat);
        Boat* GetBoat(QString BoatName);
        BoatPolar* AddBoatPolar(BoatPolar *pBoatPolar);
        BoatPolar* GetBoatPolar(QString BoatPolarName);

        void AddBoatOpp(double *Cp, double *Gamma, double *Sigma, Vector3d const &F, Vector3d const &M, Vector3d const& ForceTrefftz);


        //____________________Variables______________________________________
        //
    private:
        ArcBall m_ArcBall;

        QList<void *> *m_poaSail;            // a pointer to the array of sail objects
        QList<void *> *m_poaHull;            // a pointer to the array of hull objects
        QList<void *> *m_poaRig;            // a pointer to the array of rig objects
        QList<Boat *> *m_poaBoat;            // a pointer to the array of boat objects
        QList<BoatPolar *> *m_poaBoatPolar;        // a pointer to the boat polar array
        QList<BoatOpp *> *m_poaBoatOpp;        // a pointer to the boat OpPoint array

        Boat *m_pCurBoat;
        BoatPolar *m_pCurBoatPolar;
        BoatOpp *m_pCurBoatOpp;

        BoatAnalysisDlg m_PanelDlg;              // the dialog class which manages the Panel calculations

        QPoint m_BoatPlrLegendOffset;        // client offset position for wing polar legend


        bool m_bResetTextLegend;

        int m_iView;
        int m_iBoatPlrView;
        int m_GLList;
        bool m_bArcball, m_bCrossPoint, m_bPickCenter;
        bool m_b3DCp;                      /** true if the Cp Colors are to be displayed on the 3D openGl view */
        bool m_bDownwash;                  /** true if the arrows represeting downwash are to be displayed on the 3D openGl view */
        bool m_bMoments;                   /** true if the arrows representing moments are to be displayed on the 3D openGl view */
        bool m_bVortices;
        bool m_bLift, m_bBodyForces;
        bool m_bPanelForce;
        bool m_bICd, m_bStream, m_bSpeeds;
        bool m_bWindDirection, m_bWater;
        bool m_bVLM1;
        bool m_bSequence;
        bool m_bTrefftz;
        bool m_bLogFile;
        bool m_bTransGraph;
        bool m_bAnimateBoatOpp;
        bool m_bAnimateBoatOppPlus;

        bool m_bSurfaces, m_bOutline, m_bAxes, m_bVLMPanels; /** true if the surfaces, outline, axes or panels are to be displayed */
        bool m_bTrans;
        bool m_bResetglBoat;
        bool m_bResetglBody;
        bool m_bResetglSailGeom, m_bResetglMesh;
        bool m_bResetglOpp, m_bResetglLegend, m_bResetglStream,m_bResetglSpeeds;
        bool m_bResetglWake, m_bResetglFlow;
        bool m_bResetglLift, m_bResetglDownwash, m_bResetglDrag;
        bool m_bResetglCPForces;
        bool m_bStoreOpp;
        bool m_bIs2DScaleSet;
        bool m_bIs3DScaleSet;
        //    bool m_bShowLight; // true if the virtual light is to be displayed
        bool m_bglLight;   // true if lighting is to be activate
        bool m_bXPressed, m_bYPressed;
        bool m_bWindFront, m_bWindRear;

        double m_ForceMin, m_ForceMax;
        double m_ClipPlanePos;
        double m_glScaled;
        float m_GLScale;
        double MatIn[4][4], MatOut[4][4];

        double m_ControlMin, m_ControlMax, m_ControlDelta;

        double m_LastBoatOpp;            // last BoatOPP selected, try to set the same if it exists, for the new polar
        W3dPrefsDlg m_3DPrefsDlg;

        QTimer *m_pTimerBoatOpp;
        int m_posAnimateBOpp;            // the current ctrl index for BOpp animation

        QPoint m_PointDown, m_LastPoint;
        QRect m_r3DCltRect;

        Vector3d m_ObjectOffset;
        Vector3d m_glViewportTrans;// the translation vector in gl viewport coordinates
        Vector3d m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector

        int m_3DAxisStyle, m_3DAxisWidth;
        QColor m_3DAxisColor;

        QGraph *m_pCurGraph;
        QGraph m_BoatGraph[4];

        GLLightDlg m_GLLightDlg;

        QCheckBox *m_pctrlMoment, *m_pctrlCp,*m_pctrlSurfVel, *m_pctrlStream;
        QCheckBox *m_pctrlWindDirection, *m_pctrlWater;
        QCheckBox *m_pctrlPanelForce, *m_pctrlLift, *m_pctrlBodyForces;
        QCheckBox *m_pctrlBOppAnimate;
        QSlider *m_pctrlAnimateBOppSpeed;

        QAction *m_pXView, *m_pYView, *m_pZView, *m_pIsoView;
        QToolButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso;

        QPushButton *m_pctrlWindFront, *m_pctrlWindRear, *m_pctrlReset, *m_pctrlPickCenter;
        QCheckBox *m_pctrlAxes, *m_pctrlLight, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels;
        QCheckBox *m_pctrlSequence;
        FloatEdit *m_pctrlControlMin;
        FloatEdit *m_pctrlControlMax;
        FloatEdit *m_pctrlControlDelta;
        QCheckBox *m_pctrlStoreOpp;
        QPushButton *m_pctrlAnalyze;

        QCheckBox *m_pctrlShowCurve;
        QCheckBox *m_pctrlShowPoints;
        LineCbBox *m_pctrlCurveStyle;
        LineCbBox *m_pctrlCurveWidth;
        LineButton *m_pctrlCurveColor;
        LineDelegate *m_pStyleDelegate, *m_pWidthDelegate;

        QCheckBox *m_pctrlVortices;
        QSlider *m_pctrlClipPlanePos;

        QTextEdit *m_pctrlPolarProps1;
        QStackedWidget *m_pctrBottomControls, *m_pctrlMiddleControls;

        int m_NSurfaces, m_MatSize, m_nNodes, m_WakeSize;

        int m_CurveStyle, m_CurveWidth;
        QColor m_CurveColor;
        bool m_bCurveVisible, m_bCurvePoints;

        double *m_paij;    // coefficient matrix
        double *m_paijRef; // coefficient matrix
        double *m_pRHS;            // RHS vector
        double *m_pRHSRef;        // RHS vector

        static CPanel* s_pPanel;        // the panel array for the currently loaded UFO
        static Vector3d* s_pNode;        // the node array for the currently loaded UFO
        static Vector3d* s_pMemNode;         // used if the analysis should be performed on the tilted geometry
        static Vector3d* s_pWakeNode;        // the reference current wake node array
        static Vector3d* s_pRefWakeNode;     // the reference wake node array if wake needs to be reset
        static CPanel* s_pMemPanel;           // used if the analysis should be performed on the tilted geometry
        static CPanel* s_pWakePanel;          // the reference current wake panel array
        static CPanel* s_pRefWakePanel;       // the reference wake panel array if wake needs to be reset
};

#endif // QSAIL7_H
