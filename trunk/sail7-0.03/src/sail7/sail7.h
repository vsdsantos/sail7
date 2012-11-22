/****************************************************************************

		 Sail7
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
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

#include "../objects/BoatOpp.h"
#include "../objects/BoatPolar.h"
#include "../objects/Sail.h"
#include "../objects/ArcBall.h"
#include "../objects/Panel.h"
#include "../misc/W3dPrefsDlg.h"
#include "../misc/FloatEdit.h"
#include "../misc/LineButton.h"
#include "../misc/LineCbBox.h"
#include "../misc/LineDelegate.h"
#include "../misc/GLLightDlg.h"
#include "../graph/qgraph.h"
#include "BoatAnalysisDlg.h"



class QSail7 : public QWidget
{
	friend class MainFrame;
	friend class TwoDWidget;
	friend class ThreeDWidget;
	friend class GL3DScales;
	friend class GLLightDlg;
	friend class SailPolarDlg;
	friend class BoatDlg;
	friend class BoatAnalysisDlg;
	friend class CSailPolar;
	friend class CSail;
	friend class CBoat;
	friend class DisplaySettingsDlg;
	friend class SailDlg;
	friend class SailViewWidget;
	friend class GL3dBodyDlg;

	Q_OBJECT


public:
	QSail7(QWidget *parent = NULL);


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

	static void *s_pMainFrame, *s_p2DWidget, *s_p3DWidget;

//____________________Methods________________________________________
	void contextMenuEvent (QContextMenuEvent * event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void showEvent(QShowEvent *event);

	void GLDraw3D();
	void GLRenderView();
	void GLCreateVortices();
	void GLCreateCp(CBoatOpp *pBoatOpp);
	void GLCreatePanelForces(CBoatOpp *pBoatOpp);
	void GLDrawPanelForceLegend(void *pMainFrame, double rmin, double rmax);
	void GLCreateCpLegendClr(QRect rect, double LegendMin, double LegendMax);
	void GLDrawCpLegend(void *pGLWidget, QRect rect, double LegendMin, double LegendMax, QFont &TextFont, QColor TextColor);
	void GLCreateSurfSpeeds();

	void SetScale();
	void SetBoat(QString BoatName="");
	void SetBoatPolar(CBoatPolar *pBoatPolar=NULL, QString BoatPlrName = "");
	bool SetBoatOpp(bool bCurrent=true, double x=0);

	void GetDistrib(int const &NPanels, const int &DistType, const int &k, double &tau1, double &tau2);
	void PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence);
	void PaintView(QPainter &painter);
	void SetupLayout();
	void SetControls();
	void SetViewControls();
	void SetBoatPolarGraphTitles(Graph* pGraph);
	void SnapClient(QString const &FileName);
	void StopAnimate();
	void UpdateView();
	void UpdateUnits();

	void Connect();

	void FillComboBoxes(bool bEnable = true);
	void UpdateCurve();

	CBoatOpp* GetBoatOpp(double x);


	void LoadSettings(QSettings *pSettings);
	void SaveSettings(QSettings *pSettings);
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void Set2DScale();
	void Set3DScale();

	int CreateSailElements(CSail *pSail);
	int CreateBodyElements(CBody *pBody);
	int IsNode(CVector &Pt);

	bool SetModBoat(CBoat *pModBoat);
	bool SetModBoatPolar(CBoatPolar *pModBoatPolar);


	QGraph* GetGraph(QPoint &pt);

	void GLCreateSailLists();
	void GLCreateBodyLists();
	void GLCallViewLists();
	void GLCreateSailMesh(CVector *pNode, CPanel *pPanel);
	void GLCreateSailGeom(int GLList, CSail *pSail, CVector Position);
	void GLCreateWaterList();
	void GLCreateWindList();
	void GLCreateStreamLines();
	void GLCreateForce();
	void GLDrawBoatLegend();
	void GLDrawBoatOppLegend();
	void GLCreateBodyMesh(CVector* pNode, CPanel *pPanel);
	void GLCreatePanelNormals();

	void DrawBoatPolarLegend(QPainter &painter, QPoint place, int bottom);

	void RotatePanelsX(double const &Angle, CVector const &P);
	void RotatePanelsZ(double const &Angle, CVector const &P);
	void CreateBoatPolarCurves();
	void FillBoatPlrCurve(CCurve *pCurve, CBoatPolar *pBoatPolar, int XVar, int YVar);
	void SetAnalysisParams();
	void SetCurveParams();
	void SetBoatPlrLegendPos();

	CBoat* AddBoat(CBoat *pNewBoat);
	CBoat* GetBoat(QString BoatName);
	CBoatPolar* AddBoatPolar(CBoatPolar *pBoatPolar);
	CBoatPolar* GetBoatPolar(QString BoatPolarName);

	void AddBoatOpp(double *Cp, double *Gamma, double *Sigma, CVector const &F, CVector const &M, CVector const& ForceTrefftz);


//____________________Variables______________________________________
//
private:
	ArcBall m_ArcBall;

	QList<void *> *m_poaSail;			// a pointer to the array of sail objects
	QList<void *> *m_poaHull;			// a pointer to the array of hull objects
	QList<void *> *m_poaRig;			// a pointer to the array of rig objects
	QList<void *> *m_poaBoat;			// a pointer to the array of boat objects
	QList<void *> *m_poaBoatPolar;		// a pointer to the boat polar array
	QList<void *> *m_poaBoatOpp;		// a pointer to the boat OpPoint array

	CBoat *m_pCurBoat;
	CBoatPolar *m_pCurBoatPolar;
	CBoatOpp *m_pCurBoatOpp;

	BoatAnalysisDlg m_PanelDlg;              // the dialog class which manages the Panel calculations
//	CSurface *m_pSurface[MAXSAILS];	// An array with the pointers to the sails surfaces

	QPoint m_BoatPlrLegendOffset;		// client offset position for wing polar legend

	int m_iView;
	int m_iBoatPlrView;
	int m_GLList;
	bool m_bArcball, m_bCrossPoint, m_bPickCenter;
	bool m_b3DCp;                      // true if the Cp Colors are to be displayed on the 3D openGl view
	bool m_bDownwash;                  // true if the arrows represeting downwash are to be displayed on the 3D openGl view
	bool m_bMoments;                   // true if the arrows representing moments are to be displayed on the 3D openGl view
	bool m_bVortices;
	bool m_bForce;
	bool m_bPanelForce;
	bool m_bICd, m_bStream, m_bSpeeds;
	bool m_bWindDirection, m_bWater;
	bool m_bVLM1;
	bool m_bSequence;
	bool m_bTrefftz;
	bool m_bLogFile;
	bool m_bAutoCpScale;
	bool m_bTransGraph;
	bool m_bAnimateBoatOpp;
	bool m_bAnimateBoatOppPlus;

	bool m_bSurfaces, m_bOutline, m_bAxes, m_bVLMPanels; // true if the surfaces, outline, axes or panels are to be displayed
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
//	bool m_bShowLight; // true if the virtual light is to be displayed
	bool m_bglLight;   // true if lighting is to be activate
	bool m_bXPressed, m_bYPressed;
	bool m_bWindFront, m_bWindRear;

	double m_LiftScale, m_DragScale, m_VelocityScale;  //maximum value of hte Cp scale in 3D view
	double m_LegendMin, m_LegendMax;
	double m_ForceMin, m_ForceMax;
	double m_ClipPlanePos;
	double m_glScaled;
	double m_GLScale;
	double MatIn[4][4], MatOut[4][4];

	double m_ControlMin, m_ControlMax, m_ControlDelta;

	double m_LastBoatOpp;			// last BoatOPP selected, try to set the same if it exists, for the new polar
	W3dPrefsDlg m_3DPrefsDlg;

	QTimer *m_pTimerBoatOpp;
	int m_posAnimateBOpp;			// the current ctrl index for BOpp animation

	QPoint m_PointDown, m_LastPoint;
	QRect m_r3DCltRect;

	CVector m_ObjectOffset;
	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector

	int m_3DAxisStyle, m_3DAxisWidth;
	QColor m_3DAxisColor;

	QGraph *m_pCurGraph;
	QGraph m_BoatGraph[4];

	GLLightDlg m_GLLightDlg;

	QCheckBox *m_pctrlPanelForce, *m_pctrlLift, *m_pctrlBOppAnimate;
	QSlider *m_pctrlAnimateBOppSpeed;
	QCheckBox *m_pctrlMoment, *m_pctrlCp,*m_pctrlSurfVel, *m_pctrlStream;
	QCheckBox *m_pctrlWindDirection, *m_pctrlWater;

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
	double *m_pRHS;			// RHS vector
	double *m_pRHSRef;		// RHS vector

	static CPanel* s_pPanel;		// the panel array for the currently loaded UFO
	static CVector* s_pNode;		// the node array for the currently loaded UFO
	static CVector* s_pMemNode;         // used if the analysis should be performed on the tilted geometry
	static CVector* s_pWakeNode;        // the reference current wake node array
	static CVector* s_pRefWakeNode;     // the reference wake node array if wake needs to be reset
	static CPanel* s_pMemPanel;           // used if the analysis should be performed on the tilted geometry
	static CPanel* s_pWakePanel;          // the reference current wake panel array
	static CPanel* s_pRefWakePanel;       // the reference wake panel array if wake needs to be reset
};

#endif // QSAIL7_H
