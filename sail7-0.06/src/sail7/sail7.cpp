/****************************************************************************

			Sail7
			Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
			All rights reserved

*****************************************************************************/

//
// This class is associated to the MMI of 3D Sail design and analysis
// It dispatches user commands towards object definition, analysis and post-processing
//


#include <QApplication>
#include <QDockWidget>
#include <QtDebug>
#include <QAction>
#include <QMessageBox>
#include <QColorDialog>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QDomDocument>
#include <math.h>

#include "./sail7.h"
#include "./BoatPolarDlg.h"
#include "./BoatDlg.h"
#include "./GL3dBodyDlg.h"
#include "./SailViewWidget.h"
#include "./SailDlg.h"
#include "./GLCreateBodyLists.h"
#include "./GL3DScales.h"
#include "../globals.h"
#include "../mainframe.h"
#include "../twodwidget.h"
#include "../objects/NURBSSail.h"
#include "../objects/SailcutSail.h"
#include "../misc/ProgressDlg.h"
#include "../misc/ModDlg.h"
#include "../misc/ObjectPropsDlg.h"
#include "../misc/RenameDlg.h"
#include "../misc/SelectObjectDlg.h"
#include "../graph/graphdlg.h"


void *QSail7::s_pMainFrame;
void *QSail7::s_p2DWidget;
void *QSail7::s_p3DWidget;


CPanel* QSail7::s_pPanel;		// the panel array for the currently loaded UFO
CVector* QSail7::s_pNode;		// the node array for the currently loaded UFO
CVector* QSail7::s_pMemNode;         // used if the analysis should be performed on the tilted geometry
CVector* QSail7::s_pWakeNode;        // the reference current wake node array
CVector* QSail7::s_pRefWakeNode;     // the reference wake node array if wake needs to be reset
CPanel* QSail7::s_pMemPanel;           // used if the analysis should be performed on the tilted geometry
CPanel* QSail7::s_pWakePanel;          // the reference current wake panel array
CPanel* QSail7::s_pRefWakePanel;       // the reference wake panel array if wake needs to be reset

#define WATERLIST 7778
#define WINDLIST  7779
#define NORMALLIST 7777

#define SPANPOINTS 59
#define SIDEPOINTS 51


QSail7::QSail7(QWidget *parent)
	: QWidget(parent)
{
	m_GLList = 0;

	m_iView = SAIL3DVIEW;
	m_iBoatPlrView = 4;

	m_MatSize = m_WakeSize = 0;

	m_ClipPlanePos = 5.0;

	m_BoatPlrLegendOffset.setX(0);
	m_BoatPlrLegendOffset.setY(0);

	m_bArcball = m_bCrossPoint = m_bPickCenter = m_bTrans = false;
	m_bResetglSailGeom  = m_bResetglMesh = m_bResetglLegend = m_bResetglOpp = m_bResetglWake = m_bResetglFlow = true;
	m_bResetglStream = true;
	m_bResetglSpeeds = true;
	m_bResetglLift   = m_bResetglDownwash = m_bResetglDrag = true;
	m_bResetglCPForces = true;
	m_bResetglBoat   = true;
	m_bResetglBody   = true;

	m_bIs2DScaleSet = m_bIs3DScaleSet = false;

	m_bAxes          = false;
	m_bWater         = false;
	m_bWindDirection = true;
//	m_bShowLight     = false;
	m_bglLight       = true;
	m_bSurfaces      = true;
	m_bOutline       = true;
	m_bAxes          = true;
	m_bVLMPanels     = false;
	m_bVLM1          = true;
	m_bSequence      = false;
	m_bLogFile       = false;
	m_bStoreOpp      = true;

	m_bTransGraph    = false;
	m_bAnimateBoatOpp = false;
	m_bAnimateBoatOppPlus = true;
	m_bWindFront =  m_bWindRear = false;


	m_CurveStyle = 0;
	m_CurveWidth = 1;
	m_CurveColor = QColor(127, 255, 70);
	m_bCurveVisible = true;
	m_bCurvePoints  = false;

	m_bXPressed = m_bYPressed = false;

	m_glScaled = 1.0;
	m_GLScale = 1.0;
	m_ForceMin = m_ForceMax = 0.0;


	m_ControlMin = 0.0;
	m_ControlMax = 1.0;
	m_ControlDelta = 0.2;

	m_pCurGraph = m_BoatGraph;
	m_BoatGraph[0].SetVariables(0, 1);
	m_BoatGraph[1].SetVariables(0, 2);
	m_BoatGraph[2].SetVariables(0, 3);
	m_BoatGraph[3].SetVariables(0,4);
	for(int ig=0; ig<4; ig++)
	{
		m_BoatGraph[ig].SetAutoX(true);
		m_BoatGraph[ig].SetXUnit(2.0);
		m_BoatGraph[ig].SetXMin(-1.0);
		m_BoatGraph[ig].SetXMax( 1.0);
		m_BoatGraph[ig].SetYMin(0.000);
		m_BoatGraph[ig].SetYMax(0.001);
		m_BoatGraph[ig].SetXMajGrid(true, QColor(120,120,120),2,1);
		m_BoatGraph[ig].SetXMinGrid(false, true, QColor(80,80,80),2, 1, 100.0);
		m_BoatGraph[ig].SetYMajGrid(true, QColor(120,120,120),2,1);
		m_BoatGraph[ig].SetYMinGrid(false, true, QColor(80,80,80),2, 1, 0.1);
		m_BoatGraph[ig].SetType(1);
		m_BoatGraph[ig].SetMargin(50);
		m_BoatGraph[ig].SetGraphName(QString("Boat Graph %1").arg(ig));
		SetBoatPolarGraphTitles(m_BoatGraph+ig);
	}

	m_pCurBoat = NULL;

	m_3DAxisStyle    = 3;
	m_3DAxisWidth    = 1;
	m_3DAxisColor    = QColor(150,150,150);

	memset(MatIn, 0, 16*sizeof(double));
	memset(MatOut, 0, 16*sizeof(double));

	m_ArcBall.m_pOffx     = &m_ObjectOffset.x;
	m_ArcBall.m_pOffy     = &m_ObjectOffset.y;
	m_ArcBall.m_pTransx   = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy   = &m_glViewportTrans.y;


	QSail::s_pBoatAnalysisDlg = &m_PanelDlg;


	SailDlg::s_WindowPos = QPoint(20, 30);
	SailDlg::s_bWindowMaximized = false;
	SailDlg::s_WindowSize=QSize(500, 400);


	m_pTimerBoatOpp = new QTimer(this);
	m_posAnimateBOpp         = 0;

	SetupLayout();
	Connect();

}



void QSail7::SetupLayout()
{
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	setSizePolicy(szPolicyMaximum);

//_______________________Analysis____________________________________________
	QGroupBox *AnalysisBox = new QGroupBox(tr("Analysis settings"));
	{
		QVBoxLayout *AnalysisGroupLayout = new QVBoxLayout;
		{
			m_pctrlSequence = new QCheckBox(tr("Sequence"));
			QGridLayout *SequenceGroupLayout = new QGridLayout;
			{
				QLabel *AlphaMinLab   = new QLabel(tr("Start="));
				QLabel *AlphaMaxLab   = new QLabel(tr("End="));
				QLabel *AlphaDeltaLab = new QLabel(tr("D="));
				AlphaDeltaLab->setFont(QFont("Symbol"));
				AlphaDeltaLab->setAlignment(Qt::AlignRight);
				AlphaMinLab->setAlignment(Qt::AlignRight);
				AlphaMaxLab->setAlignment(Qt::AlignRight);
				m_pctrlControlMin     = new FloatEdit(0.0, 3);
				m_pctrlControlMax     = new FloatEdit(1., 3);
				m_pctrlControlDelta   = new FloatEdit(0.5, 3);

			/*	m_pctrlControlMin->setMinimumHeight(20);
				m_pctrlControlMax->setMinimumHeight(20);
				m_pctrlControlDelta->setMinimumHeight(20);*/
				m_pctrlControlMin->setAlignment(Qt::AlignRight);
				m_pctrlControlMax->setAlignment(Qt::AlignRight);
				m_pctrlControlDelta->setAlignment(Qt::AlignRight);
				SequenceGroupLayout->addWidget(AlphaMinLab,1,1);
				SequenceGroupLayout->addWidget(AlphaMaxLab,2,1);
				SequenceGroupLayout->addWidget(AlphaDeltaLab,3,1);
				SequenceGroupLayout->addWidget(m_pctrlControlMin,1,2);
				SequenceGroupLayout->addWidget(m_pctrlControlMax,2,2);
				SequenceGroupLayout->addWidget(m_pctrlControlDelta,3,2);
			}

			m_pctrlStoreOpp    = new QCheckBox(tr("Store OpPoint"));
			m_pctrlAnalyze     = new QPushButton(tr("Analyze"));

			AnalysisGroupLayout->addWidget(m_pctrlSequence);
			AnalysisGroupLayout->addLayout(SequenceGroupLayout);
			AnalysisGroupLayout->addStretch(1);
			AnalysisGroupLayout->addWidget(m_pctrlStoreOpp);
			AnalysisGroupLayout->addWidget(m_pctrlAnalyze);
		}

		AnalysisBox->setLayout(AnalysisGroupLayout);
	}

//_______________________Display____________________________________________
	QGroupBox *DisplayBox = new QGroupBox(tr("Results"));
	{
		QGridLayout *CheckDispLayout = new QGridLayout;
		{
			m_pctrlPanelForce = new QCheckBox(tr("Panel Forces"));
			m_pctrlPanelForce->setToolTip(tr("Display the force 1/2.rho.V2.S.Cp acting on the panel"));
			m_pctrlLift           = new QCheckBox(tr("Lift and Drag"));
			m_pctrlBodyForces     = new QCheckBox(tr("Body Axis Forces"));
			m_pctrlMoment         = new QCheckBox(tr("Moment"));
			m_pctrlCp             = new QCheckBox(tr("Cp"));
			m_pctrlSurfVel        = new QCheckBox(tr("Surf. Vel."));
			m_pctrlStream         = new QCheckBox(tr("Stream"));
			m_pctrlWater          = new QCheckBox(tr("Water"));
			m_pctrlWindDirection  = new QCheckBox(tr("Wind"));
			m_pctrlBOppAnimate    = new QCheckBox(tr("Animate"));
		//	m_pctrlHighlightOpp   = new QCheckBox(tr("Highlight OpPoint"));


			m_pctrlAnimateBOppSpeed= new QSlider(Qt::Horizontal);
			m_pctrlAnimateBOppSpeed->setMinimum(0);
			m_pctrlAnimateBOppSpeed->setMaximum(500);
			m_pctrlAnimateBOppSpeed->setSliderPosition(250);
			m_pctrlAnimateBOppSpeed->setTickInterval(50);
			m_pctrlAnimateBOppSpeed->setTickPosition(QSlider::TicksBelow);
			CheckDispLayout->addWidget(m_pctrlCp,       1,1);
			CheckDispLayout->addWidget(m_pctrlPanelForce, 1, 2);
			CheckDispLayout->addWidget(m_pctrlLift,     2, 1);
			CheckDispLayout->addWidget(m_pctrlBodyForces,   2, 2);
			CheckDispLayout->addWidget(m_pctrlSurfVel,  4, 1);
			CheckDispLayout->addWidget(m_pctrlStream,   4, 2);
			CheckDispLayout->addWidget(m_pctrlWater,  5, 1);
			CheckDispLayout->addWidget(m_pctrlWindDirection,   5, 2);
			CheckDispLayout->addWidget(m_pctrlBOppAnimate,  6, 1);
			CheckDispLayout->addWidget(m_pctrlAnimateBOppSpeed,6,2);

		}
		DisplayBox->setLayout(CheckDispLayout);
	}

	QGroupBox *PolarPropsBox = new QGroupBox(tr("Polar properties"));
	{
		m_pctrlPolarProps1 = new QTextEdit;
		m_pctrlPolarProps1->setReadOnly(true);
	//	m_pctrlPolarProps1->setWordWrapMode(QTextOption::NoWrap);
		QHBoxLayout *PolarPropsLayout = new QHBoxLayout;
		{
			PolarPropsLayout->addWidget(m_pctrlPolarProps1);
			PolarPropsLayout->addStretch(1);
		}
		PolarPropsBox->setLayout(PolarPropsLayout);
	}

//_______________________Curve params____________________________________________
	QGroupBox *CurveBox = new QGroupBox(tr("Curve settings"));
	{
		QVBoxLayout *CurveGroupLayout = new QVBoxLayout;
		{
			m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
			m_pctrlShowPoints = new QCheckBox(tr("Points"));
		//	m_pctrlShowCurve->setMinimumHeight(10);
		//	m_pctrlShowPoints->setMinimumHeight(10);
			m_pctrlCurveStyle = new LineCbBox();
			m_pctrlCurveWidth = new LineCbBox();
			m_pctrlCurveColor = new LineButton;
			for (int i=0; i<5; i++)
			{
				m_pctrlCurveStyle->addItem(tr("item"));
				m_pctrlCurveWidth->addItem(tr("item"));
			}
			m_pStyleDelegate = new LineDelegate;
			m_pWidthDelegate = new LineDelegate;
			m_pctrlCurveStyle->setItemDelegate(m_pStyleDelegate);
			m_pctrlCurveWidth->setItemDelegate(m_pWidthDelegate);

			QHBoxLayout *ShowCurve = new QHBoxLayout;
			{
				ShowCurve->addWidget(m_pctrlShowCurve);
				ShowCurve->addWidget(m_pctrlShowPoints);
			}

			QGridLayout *CurveStyleLayout = new QGridLayout;
			{
				QLabel *lab200 = new QLabel(tr("Style"));
				QLabel *lab201 = new QLabel(tr("Width"));
				QLabel *lab202 = new QLabel(tr("Color"));
				lab200->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
				lab201->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
				lab202->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
				CurveStyleLayout->addWidget(lab200,1,1);
				CurveStyleLayout->addWidget(lab201,2,1);
				CurveStyleLayout->addWidget(lab202,3,1);
				CurveStyleLayout->addWidget(m_pctrlCurveStyle,1,2);
				CurveStyleLayout->addWidget(m_pctrlCurveWidth,2,2);
				CurveStyleLayout->addWidget(m_pctrlCurveColor,3,2);
				CurveStyleLayout->setColumnStretch(2,5);
			}

			CurveGroupLayout->addLayout(ShowCurve);
			CurveGroupLayout->addLayout(CurveStyleLayout);
			CurveGroupLayout->addStretch(1);
		}
		CurveBox->setLayout(CurveGroupLayout);
	}



//_______________________3D view controls____________________________________________
	QGroupBox *ThreeDViewBox = new QGroupBox(tr("Display"));
	{
		QVBoxLayout *ThreeDViewControls = new QVBoxLayout;
		{
			QGridLayout *ThreeDParams = new QGridLayout;
			{
				m_pctrlAxes       = new QCheckBox(tr("Axes"));
				m_pctrlLight      = new QCheckBox(tr("Light"));
				m_pctrlSurfaces   = new QCheckBox(tr("Surfaces"));
				m_pctrlOutline    = new QCheckBox(tr("Outline"));
				m_pctrlPanels     = new QCheckBox(tr("Panels"));
				m_pctrlVortices   = new QCheckBox(tr("Vortices"));

				ThreeDParams->addWidget(m_pctrlAxes, 1,1);
			//	ThreeDParams->addWidget(m_pctrlLight, 1,2);
				ThreeDParams->addWidget(m_pctrlPanels, 1,2);
				ThreeDParams->addWidget(m_pctrlSurfaces, 2,1);
				ThreeDParams->addWidget(m_pctrlOutline, 2,2);
			}

			QVBoxLayout *ThreeDView = new QVBoxLayout;
			{
				QHBoxLayout *AxisViewLayout = new QHBoxLayout;
				{
					m_pctrlX          = new QToolButton;
					m_pctrlY          = new QToolButton;
					m_pctrlZ          = new QToolButton;
					m_pctrlIso        = new QToolButton;
					if(m_pctrlX->iconSize().height()<=48)
					{
						m_pctrlX->setIconSize(QSize(32,32));
						m_pctrlY->setIconSize(QSize(32,32));
						m_pctrlZ->setIconSize(QSize(32,32));
						m_pctrlIso->setIconSize(QSize(32,32));
					}
					m_pXView = new QAction(QIcon(":/images/OnXView.png"), tr("X View"), this);
					m_pYView = new QAction(QIcon(":/images/OnYView.png"), tr("Y View"), this);
					m_pZView = new QAction(QIcon(":/images/OnZView.png"), tr("Z View"), this);
					m_pIsoView = new QAction(QIcon(":/images/OnIsoView.png"), tr("Iso View"), this);
					m_pXView->setCheckable(true);
					m_pYView->setCheckable(true);
					m_pZView->setCheckable(true);
					m_pIsoView->setCheckable(true);

					m_pctrlX->setDefaultAction(m_pXView);
					m_pctrlY->setDefaultAction(m_pYView);
					m_pctrlZ->setDefaultAction(m_pZView);
					m_pctrlIso->setDefaultAction(m_pIsoView);
					AxisViewLayout->addWidget(m_pctrlX);
					AxisViewLayout->addWidget(m_pctrlY);
					AxisViewLayout->addWidget(m_pctrlZ);
					AxisViewLayout->addWidget(m_pctrlIso);
				}


				QHBoxLayout *WindViewLayout  = new QHBoxLayout;
				{
					m_pctrlWindFront = new QPushButton("Wind Front");
					m_pctrlWindRear  = new QPushButton("Wind Rear");
					m_pctrlWindFront->setCheckable(true);
					m_pctrlWindRear->setCheckable(true);
					WindViewLayout->addWidget(m_pctrlWindFront);
					WindViewLayout->addWidget(m_pctrlWindRear);
				}

				QHBoxLayout *ViewResetLayout = new QHBoxLayout;
				{
					m_pctrlPickCenter     = new QPushButton(tr("Pick Center"));
					m_pctrlPickCenter->setToolTip(tr("Activate the button, then click on the object to center it in the viewport; alternatively, double click on the object"));
					m_pctrlReset          = new QPushButton(tr("Reset"));
					m_pctrlPickCenter->setCheckable(true);

					ViewResetLayout->addWidget(m_pctrlReset);
					ViewResetLayout->addWidget(m_pctrlPickCenter);
				}
				ThreeDView->addLayout(AxisViewLayout);
				ThreeDView->addLayout(WindViewLayout);
				ThreeDView->addLayout(ViewResetLayout);
			}

			QHBoxLayout *ClipLayout = new QHBoxLayout;
			{
				QLabel *ClipLabel = new QLabel(tr("Clip:"));
				m_pctrlClipPlanePos = new QSlider(Qt::Horizontal);
				m_pctrlClipPlanePos->setMinimum(-300);
				m_pctrlClipPlanePos->setMaximum(300);
				m_pctrlClipPlanePos->setSliderPosition(0);
				m_pctrlClipPlanePos->setTickInterval(30);
				m_pctrlClipPlanePos->setTickPosition(QSlider::TicksBelow);
				ClipLayout->addWidget(ClipLabel);
				ClipLayout->addWidget(m_pctrlClipPlanePos,1);
			}
			ThreeDViewControls->addLayout(ThreeDParams);
			ThreeDViewControls->addLayout(ThreeDView);
			ThreeDViewControls->addLayout(ClipLayout);
			ThreeDViewControls->addStretch(1);

		}
		ThreeDViewBox->setLayout(ThreeDViewControls);
	}

//_________________________Main Layout____________________________________________
	QVBoxLayout *mainLayout = new QVBoxLayout;
	{
		m_pctrlMiddleControls = new QStackedWidget;
		m_pctrlMiddleControls->addWidget(DisplayBox);
		m_pctrlMiddleControls->addWidget(PolarPropsBox);

		m_pctrBottomControls = new QStackedWidget;
		m_pctrBottomControls->addWidget(CurveBox);
		m_pctrBottomControls->addWidget(ThreeDViewBox);

		mainLayout->addWidget(AnalysisBox);
		mainLayout->addWidget(m_pctrlMiddleControls);
		mainLayout->addWidget(m_pctrBottomControls);
	}
	setLayout(mainLayout);
}



void QSail7::Connect()
{
	//Connect signals to slots
	connect(m_pctrlSequence, SIGNAL(clicked()), this, SLOT(OnSequence()));
	connect(m_pctrlStoreOpp, SIGNAL(clicked()), this, SLOT(OnStoreOpp()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
	connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
	connect(m_pctrlCurveColor, SIGNAL(clicked()), this, SLOT(OnCurveColor()));
	connect(m_pctrlShowPoints, SIGNAL(clicked()), this, SLOT(OnShowCurve()));
	connect(m_pctrlShowCurve, SIGNAL(clicked()), this, SLOT(OnShowCurve()));

	connect(m_pctrlPanelForce, SIGNAL(clicked()), this, SLOT(OnPanelForce()));
	connect(m_pctrlLift, SIGNAL(clicked()), this, SLOT(OnShowLift()));
	connect(m_pctrlBodyForces, SIGNAL(clicked()), this, SLOT(OnShowBodyForces()));
	connect(m_pctrlCp, SIGNAL(clicked()),this, SLOT(On3DCp()));
	connect(m_pctrlMoment, SIGNAL(clicked()), this, SLOT(OnMoment()));
	connect(m_pctrlStream, SIGNAL(clicked()), this, SLOT(OnStreamlines()));
	connect(m_pctrlWater, SIGNAL(clicked()), this, SLOT(OnShowWater()));
	connect(m_pctrlWindDirection, SIGNAL(clicked()), this, SLOT(OnShowWind()));
	connect(m_pctrlSurfVel, SIGNAL(clicked()), this, SLOT(OnSurfaceSpeeds()));

	connect(m_pctrlBOppAnimate, SIGNAL(clicked()), this, SLOT(OnAnimateBoatOpp()));
	connect(m_pctrlAnimateBOppSpeed, SIGNAL(sliderMoved(int)), this, SLOT(OnAnimateBoatOppSpeed(int)));

	connect(m_pctrlSurfaces, SIGNAL(clicked()), SLOT(OnSurfaces()));
	connect(m_pctrlOutline, SIGNAL(clicked()), SLOT(OnOutline()));
	connect(m_pctrlPanels, SIGNAL(clicked()), SLOT(OnPanels()));
	connect(m_pctrlVortices, SIGNAL(clicked()), SLOT(OnVortices()));
	connect(m_pctrlClipPlanePos, SIGNAL(sliderMoved(int)), this, SLOT(OnClipPlane(int)));
	connect(m_pctrlLight, SIGNAL(clicked()), SLOT(OnLight()));

	connect(m_pctrlAxes, SIGNAL(clicked()), SLOT(OnAxes()));
	connect(m_pctrlX, SIGNAL(clicked()), SLOT(On3DFront()));
	connect(m_pctrlY, SIGNAL(clicked()), SLOT(On3DLeft()));
	connect(m_pctrlZ, SIGNAL(clicked()), SLOT(On3DTop()));
	connect(m_pctrlIso, SIGNAL(clicked()), SLOT(On3DIso()));
	connect(m_pctrlWindFront, SIGNAL(clicked()), SLOT(On3DWindFront()));
	connect(m_pctrlWindRear, SIGNAL(clicked()), SLOT(On3DWindRear()));
	connect(m_pctrlReset, SIGNAL(clicked()), SLOT(On3DReset()));
	connect(m_pctrlPickCenter, SIGNAL(clicked()), SLOT(On3DPickCenter()));

	connect(m_pctrlControlMin, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));
	connect(m_pctrlControlMax, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));
	connect(m_pctrlControlDelta, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));

	connect(m_pTimerBoatOpp, SIGNAL(timeout()), this, SLOT(OnAnimateBoatOppSingle()));
}


void QSail7::keyPressEvent(QKeyEvent *event)
{
	//
	// Capture and dispatch user keyboard input
	//
	bool bShift = false;
	bool bCtrl  = false;
	if(event->modifiers() & Qt::ShiftModifier)   bShift =true;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if (event->modifiers().testFlag(Qt::AltModifier))
			{
				OnBoatPolarProps();
				break;
			}
			if(!m_pctrlAnalyze->hasFocus())
			{

				activateWindow();
				m_pctrlAnalyze->setFocus();
			}
			else
			{
				OnAnalyze();
			}
			event->accept();
			break;
		}
		case Qt::Key_Escape:
		{
			if(m_GLLightDlg.isVisible())m_GLLightDlg.hide();
			else if(pMainFrame->m_pctrl3DScalesWidget->isVisible()) pMainFrame->m_pctrl3DScalesWidget->hide();
			else
			{
				StopAnimate();
			}
			break;
		}
		case Qt::Key_1:
		{
			if(m_iView==SAILPOLARVIEW)
			{
				m_iBoatPlrView  = 1;
				m_pCurGraph = m_BoatGraph;
				SetBoatPlrLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_2:
		{
			if(m_iView==SAILPOLARVIEW)
			{
				m_iBoatPlrView  = 1;
				m_pCurGraph = m_BoatGraph+1;
				SetBoatPlrLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_3:
		{
			if(m_iView==SAILPOLARVIEW)
			{
				m_iBoatPlrView  = 1;
				m_pCurGraph = m_BoatGraph+2;
				SetBoatPlrLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_4:
		{
			if(m_iView==SAILPOLARVIEW)
			{
				m_iBoatPlrView  = 1;
				m_pCurGraph = m_BoatGraph+3;
				SetBoatPlrLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_R:
		{
			if(m_iView==SAIL3DVIEW)    On3DReset();
			else if(m_pCurGraph)
			{
				m_pCurGraph->SetAuto(true);
				UpdateView();
			}
//			else if(m_iView==WOPPVIEW)   OnResetWingScale();
			else if(m_iView==SAILPOLARVIEW) OnResetBoatPlrLegend();

			break;
		}
		case Qt::Key_T:
		{
			if(m_iView==SAILPOLARVIEW)
			{
				m_iBoatPlrView = 2;
				SetBoatPlrLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_A:
		{
			if(m_iView==SAILPOLARVIEW)
			{
				m_iBoatPlrView = 4;
				SetBoatPlrLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_G:
		{
			OnGraphSettings();
			break;
		}
		case Qt::Key_L:
		{
			pMainFrame->OnLogFile();
			break;
		}
		case Qt::Key_F1:
		{
/*			if(bShift)
			{
				SailcutSail *pSail = new SailcutSail;
				SailDlg dlg;
				if(dlg.InitDialog(pSail)) dlg.exec();

			}
			else
			{
				NURBSSail *pSail = new NURBSSail;
				SailDlg dlg;
				if(dlg.InitDialog(pSail)) dlg.exec();

			}*/
			break;
		}
		case Qt::Key_F2:
		{
			OnRenameCurBoat();
			break;
		}
		case Qt::Key_F3:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier))        OnEditCurBoat();
			else                                                       OnNewBoat();
			break;
		}
		case Qt::Key_F4:
		{
			OnSail3DView();
			break;
		}
		case Qt::Key_F5:
		{
			GL3dBodyDlg dlg(pMainFrame);
			CBody Hull;
			dlg.m_bEnableName = false;
			if(!dlg.InitDialog(&Hull)) return;
			dlg.move(GL3dBodyDlg::s_WindowPos);
			dlg.resize(GL3dBodyDlg::s_WindowSize);
			if(GL3dBodyDlg::s_bWindowMaximized) dlg.setWindowState(Qt::WindowMaximized);

			dlg.exec();
		}
		case Qt::Key_F6:
		{
			if(bShift) OnEditBoatPolar();
			else       OnDefineBoatPolar();
			break;
		}
		case Qt::Key_F7:
		{
			NURBSSail *pNewSail = new NURBSSail;
			SailDlg dlg;
			dlg.InitDialog(pNewSail);
			dlg.exec();
			break;
		}
		case Qt::Key_F8:
		{
			OnSailPolarView();
			break;
		}
		case Qt::Key_F12:
		{
			SailcutSail *pSCSail = new SailcutSail;
			if(bShift) pSCSail->Export();
			else
			{
				MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
				QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),pMainFrame->m_XMLPath,
																tr("Sailcut files (*.saildef *.xml)"));
				if (!filePath.isEmpty())
				{
					pMainFrame->m_XMLPath = filePath;
					QFile file(filePath);
					pSCSail->Import(&file);
					SailDlg dlg;
					if(dlg.InitDialog(pSCSail)) dlg.exec();
				}
			}
			break;
		}
		case Qt::Key_Control:
		{
			m_bArcball = true;
			UpdateView();
			break;
		}

		default:
			event->ignore();
	}
	event->accept();
}


void QSail7::OnSail3DView()
{
	//

	// The user has requested a switch to the OpenGL 3D view
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ThreeDWidget* p3DWidget = (ThreeDWidget*)s_p3DWidget;
	p3DWidget->GLSetupLight(&m_GLLightDlg, m_ObjectOffset.y, 1.0f);

	m_bArcball = false;
	if(m_iView==SAIL3DVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}

	m_bIs3DScaleSet = false;
	m_iView = SAIL3DVIEW;
	pMainFrame->SetCentralWidget();

	SetControls();
	UpdateView();
	return;
}




void QSail7::OnSailPolarView()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
//	if (m_bAnimateWOpp) StopAnimate();

	if(m_iView==SAILPOLARVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}

	m_iView=SAILPOLARVIEW;
	m_pCurGraph = m_BoatGraph;

	pMainFrame->SetCentralWidget();

	SetBoatPlrLegendPos();

	CreateBoatPolarCurves();
	SetCurveParams();
	SetControls();

	UpdateView();
}


void QSail7::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	ThreeDWidget * p3DWidget = (ThreeDWidget*)s_p3DWidget;
	if(m_iView==SAIL3DVIEW)
	{
		if(p3DWidget) p3DWidget->updateGL();
	}
	else
	{
		if(p2DWidget) p2DWidget->update();
	}
}



void QSail7::PaintView(QPainter &painter)
{
	static int h,w,w2,h2, h23, w3,w23;
	static QRect Rect1, Rect2, Rect3, Rect4, r2DCltRect;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;

	r2DCltRect = p2DWidget->geometry();

	painter.save();

	w   = r2DCltRect.width();
	w2  = (int)(w/2);
	w3  = (int)(0.35*w);
	w23 = 2*w3;
	h   = r2DCltRect.height();
	h2  = (int)(h/2);
	h23 = (int)(2*h/3);
//	h34 = (int)(3*h/4);
//	h38 = (int)(3*h/8);

	//Refresh the active view
	painter.fillRect(r2DCltRect, pMainFrame->m_BackgroundColor);

	if(r2DCltRect.width()<200 || r2DCltRect.height()<200)
	{
		painter.restore();
		return;//too small to paint
	}

	if (m_iView==SAILPOLARVIEW)
	{
		if (m_iBoatPlrView == 1)
		{
			if(!m_pCurGraph) m_pCurGraph     = m_BoatGraph;

			Rect1.setRect(0,0,w23,r2DCltRect.bottom()-00);
			m_pCurGraph->DrawGraph(Rect1, painter);
			DrawBoatPolarLegend(painter, m_BoatPlrLegendOffset, Rect1.bottom());
		}
		else if(m_iBoatPlrView == 2)
		{
			Rect1.setRect(0,0,w2,h23);
			Rect2.setRect(w2,0,w2,h23);

			m_BoatGraph[0].DrawGraph(Rect1, painter);
			m_BoatGraph[1].DrawGraph(Rect2, painter);

			DrawBoatPolarLegend(painter, m_BoatPlrLegendOffset, r2DCltRect.height());
		}
		else if(m_iBoatPlrView == 4)
		{
			Rect1.setRect(0,0,w3,h2);
			Rect2.setRect(w3,0,w3,h2);
			Rect3.setRect(0,h2,w3,h2);
			Rect4.setRect(w3,h2,w3,h2);

			m_BoatGraph[0].DrawGraph(Rect1, painter);
			m_BoatGraph[1].DrawGraph(Rect2, painter);
			m_BoatGraph[2].DrawGraph(Rect3, painter);
			m_BoatGraph[3].DrawGraph(Rect4, painter);

			DrawBoatPolarLegend(painter, m_BoatPlrLegendOffset, r2DCltRect.height());
		}
	}
	painter.restore();
}



void QSail7::GLDraw3D()
{
	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;
	if(!glIsList(ARCBALL))
	{
		p3DWidget->CreateArcballList(m_ArcBall, m_GLScale);
		m_GLList+=2;
	}

	if(!glIsList(WATERLIST))
	{
		GLCreateWaterList();
		m_GLList++;
	}

	if(m_bResetglOpp)
	{
		if(glIsList(WINDLIST))
		{
			glDeleteLists(WINDLIST, 1);
			m_GLList--;
		}
		GLCreateWindList();
		m_GLList++;
	}

	if(!glIsList(LIGHTSPHERE))
	{
		double radius = (20.0+15.0)/100.0*m_GLScale;
		p3DWidget->GLCreateSphereList(LIGHTSPHERE,radius,17,17);
		m_GLList+=1;
	}

	if((m_bResetglStream || m_bResetglOpp) && m_iView==SAIL3DVIEW)
	{
		if(glIsList(STREAMLINES))
		{
			glDeleteLists(STREAMLINES,1);
			m_GLList--;
		}
		if(m_bStream)
		{
			//no need to recalculate if not showing
			if(m_pCurBoat && m_pCurBoatPolar && m_pCurBoatOpp)
			{
				GLCreateStreamLines();
				m_bResetglStream = false;
			}
		}
	}

	if((m_bResetglSpeeds || m_bResetglOpp) && m_iView==SAIL3DVIEW)
	{
		if(glIsList(SURFACESPEEDS))
		{
			glDeleteLists(SURFACESPEEDS,1);
			m_GLList--;
		}
		if(m_bSpeeds)
		{
			//no need to recalculate if not showing
			if(m_pCurBoat && m_pCurBoatPolar && m_pCurBoatOpp)
			{
				GLCreateSurfSpeeds();
				m_bResetglSpeeds = false;
			}
		}
	}

	if(m_bResetglLift)
	{
		if(glIsList(LIFTFORCE))
		{
			glDeleteLists(LIFTFORCE,1);
			m_GLList -=1;
		}
		if(m_pCurBoat && m_pCurBoatPolar && m_pCurBoatOpp)
		{
//			GLCreateForce();
			m_bResetglLift = false;
		}
	}

	if(m_pCurBoat)
	{
		GLCreateSailLists();
		GLCreateBodyLists();
		m_bResetglBoat = false;
	}
	if(m_bResetglMesh)
	{
		GLCreateSailMesh(s_pNode, s_pPanel);
		GLCreateBodyMesh(s_pNode, s_pPanel);
//		GLCreatePanelNormals();
		m_bResetglMesh = false;
	}

	if((m_bResetglLegend || m_bResetglCPForces || m_bResetglSailGeom) && (m_iView==SAIL3DVIEW))
	{
		if(glIsList(PANELCPLEGENDCOLOR))
		{
			glDeleteLists(PANELCPLEGENDCOLOR,1);
			m_GLList -= 1;
		}
		if(m_pCurBoatOpp)
		{
			GLCreateCpLegendClr(m_r3DCltRect, GL3DScales::s_LegendMin, GL3DScales::s_LegendMax);
			m_GLList++;
		}
		m_bResetglLegend = false;
	}


	if(m_bResetglCPForces && m_iView==SAIL3DVIEW && m_pCurBoat && m_pCurBoatOpp)
	{
		for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
		{
			if(glIsList(SAILCPBASE+is))
			{
				glDeleteLists(SAILCPBASE+is,1);
				m_GLList--;
			}

			if(glIsList(SAILFORCELISTBASE+is))
			{
				glDeleteLists(SAILFORCELISTBASE+is,1);
				m_GLList--;
			}
		}

		for(int ib=0; ib<m_pCurBoat->m_poaHull.size(); ib++)
		{
			if(glIsList(BODYCPBASE+ib))
			{
				glDeleteLists(BODYCPBASE+ib,1);
				m_GLList--;
			}
			if(glIsList(BODYFORCELISTBASE+ib))
			{
				glDeleteLists(BODYFORCELISTBASE+ib,1);
				m_GLList--;
			}
		}

		GLCreateCp(m_pCurBoatOpp);
		GLCreatePanelForces(m_pCurBoatOpp);

		m_bResetglCPForces = false;
	}


	m_bResetglOpp = false;
}


void QSail7::GLRenderView()
{
	//
	// Renders the OpenGl 3D view
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;
	static GLdouble pts[4];

	pts[0]= 0.0; pts[1]=0.0; pts[2]=-1.0; pts[3]= m_ClipPlanePos;  //x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE1, pts);
	if(m_ClipPlanePos>4.9999) 	glDisable(GL_CLIP_PLANE1);
	else						glEnable(GL_CLIP_PLANE1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	p3DWidget->GLSetupLight(&m_GLLightDlg, m_ObjectOffset.y, 1.0f);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);


	glPushMatrix();
	{
		if(m_ClipPlanePos>4.9999) 	glDisable(GL_CLIP_PLANE1);
		else						glEnable(GL_CLIP_PLANE1);

		if(m_GLLightDlg.isVisible()) //show light
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glPushMatrix();
			{
				glTranslated(( GLLightDlg::s_XLight+ m_ObjectOffset.x)*m_GLScale,
							 ( GLLightDlg::s_YLight+ m_ObjectOffset.y)*m_GLScale,
							   GLLightDlg::s_ZLight*m_GLScale);

				QColor color;
				color = QColor((int)(GLLightDlg::s_Red  *255),
							   (int)(GLLightDlg::s_Green*255),
							   (int)(GLLightDlg::s_Blue *255));
				glScaled((GLLightDlg::s_ZLight+5.0)/100.0, (GLLightDlg::s_ZLight+5.0)/100.0, (GLLightDlg::s_ZLight+5.0)/100.0);
				glCallList(LIGHTSPHERE);
			}
			glPopMatrix();
		}
		glLoadIdentity();
		if(m_bCrossPoint && m_bArcball)
		{
			glPushMatrix();
			{
				glTranslated(m_ObjectOffset.x, m_ObjectOffset.y,  0.0);
				m_ArcBall.RotateCrossPoint();
				glRotated(m_ArcBall.angle, m_ArcBall.p.x, m_ArcBall.p.y, m_ArcBall.p.z);
				glCallList(ARCPOINT);
			}
			glPopMatrix();
		}
		if(m_bArcball)
		{
			glPushMatrix();
			{
				glTranslated(m_ObjectOffset.x, m_ObjectOffset.y,  0.0);
				m_ArcBall.Rotate();
				glCallList(ARCBALL);
			}
			glPopMatrix();
		}
		glTranslated(m_ObjectOffset.x, m_ObjectOffset.y,  0.0);

		m_ArcBall.Rotate();

		glScaled(m_glScaled, m_glScaled, m_glScaled);
		glTranslated(m_glRotCenter.x, m_glRotCenter.y, m_glRotCenter.z);

		if(m_bAxes)  p3DWidget->GLDrawAxes(1.0/m_glScaled, W3dPrefsDlg::s_3DAxisColor, W3dPrefsDlg::s_3DAxisStyle, W3dPrefsDlg::s_3DAxisWidth);

		if(m_bWater && m_pCurBoatPolar && m_pCurBoatPolar->m_bGround) glCallList(WATERLIST);

		if(m_bWindDirection && m_pCurBoatOpp)
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glPushMatrix();
			{
				glRotated(m_pCurBoatOpp->m_Beta, 0.0, 0.0, 1.0);
				glTranslated(-7, 0.0, 0.0);
				glCallList(WINDLIST);
			}
			glPopMatrix();
		}

		if(m_pCurBoatOpp)
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			if(m_bBodyForces || m_bLift )  GLDrawForces();
			if(m_bStream) glCallList(STREAMLINES);
			if(m_bSpeeds) glCallList(SURFACESPEEDS);
		}

		GLCallViewLists();

		glLoadIdentity();

		GLDrawBoatLegend();
		if(m_pCurBoatOpp) GLDrawBoatOppLegend();

		glDisable(GL_CLIP_PLANE1);
		if (m_b3DCp && m_pCurBoatOpp)
		{
			GLDrawCpLegend(s_p3DWidget, m_r3DCltRect, GL3DScales::s_LegendMin, GL3DScales::s_LegendMax, pMainFrame->m_TextFont, pMainFrame->m_TextColor);
			//glCallList(WOPPCPLEGENDTXT);
			glCallList(PANELCPLEGENDCOLOR);
		}
		if (m_bPanelForce && m_pCurBoatOpp)
		{
			GLDrawPanelForceLegend(s_pMainFrame, m_ForceMin, m_ForceMax);
			glCallList(PANELCPLEGENDCOLOR);
		}
	}
	glPopMatrix();
}


void QSail7::GetDistrib(int const &NPanels, const int &DistType, const int &k, double &tau1, double &tau2)
{
	//leading edge

	double dPanels, dk;
	dPanels = (double)NPanels;
	dk      = (double)k;

	if(DistType==COSINE)
	{
		//cosine case
		tau1  = 1.0/2.0*(1.0-cos( dk   *PI/dPanels));
		tau2  = 1.0/2.0*(1.0-cos((dk+1)*PI/dPanels));
	}
	else if(DistType==SINE)
	{
		//sine case
		tau1  = 1.0*(sin( dk   *PI/2.0/dPanels));
		tau2  = 1.0*(sin((dk+1)*PI/2.0/dPanels));
	}
	else if(DistType==MSINE)
	{
		//-sine case
		tau1  = 1.0*(1.-cos( dk   *PI/2.0/dPanels));
		tau2  = 1.0*(1.-cos((dk+1)*PI/2.0/dPanels));
	}
	else
	{
		//equally spaced case
		tau1 =  dk     /dPanels;
		tau2 = (dk+1.0)/dPanels;
	}
}


int QSail7::CreateSailElements(QSail *pSail)
{
	int k,l;
	int n0, n1, n2, n3;
	double tau, tau1, x, x1;
	CVector LA, LB, TA, TB;

	int InitialSize = m_MatSize;
	pSail->m_FirstPanel = m_MatSize;


	for (k=0; k<pSail->m_NZPanels; k++)
	{
		//add the panels, following a strip from foot to gaff and from luff to leech

		GetDistrib(pSail->m_NZPanels, COSINE, k, tau, tau1);

		for (l=pSail->m_NXPanels-1; l>=0; l--)
		{
			GetDistrib(pSail->m_NXPanels, COSINE, l, x, x1);

			LA = pSail->GetPoint(x, tau)   + pSail->m_LEPosition;
			LB = pSail->GetPoint(x, tau1)  + pSail->m_LEPosition;
			TA = pSail->GetPoint(x1, tau)  + pSail->m_LEPosition;
			TB = pSail->GetPoint(x1, tau1) + pSail->m_LEPosition;

			n0 = IsNode(LA);
			n1 = IsNode(TA);
			n2 = IsNode(LB);
			n3 = IsNode(TB);

			if(l==0)                   s_pPanel[m_MatSize].m_bIsLeading   = true;
			if(l==pSail->m_NXPanels-1) s_pPanel[m_MatSize].m_bIsTrailing  = true;
			s_pPanel[m_MatSize].m_bIsWakePanel   = false;
			s_pPanel[m_MatSize].m_bIsInSymPlane  = false;


			if(n0>=0)
			{
				s_pPanel[m_MatSize].m_iLA = n0;
			}
			else {
				s_pPanel[m_MatSize].m_iLA = m_nNodes;
				s_pNode[m_nNodes].Copy(LA);
				m_nNodes++;
			}

			if(n1>=0)
			{
				s_pPanel[m_MatSize].m_iTA = n1;
			}
			else {
				s_pPanel[m_MatSize].m_iTA = m_nNodes;
				s_pNode[m_nNodes].Copy(TA);
				m_nNodes++;
			}

			if(n2>=0)
			{
				s_pPanel[m_MatSize].m_iLB = n2;
			}
			else {
				s_pPanel[m_MatSize].m_iLB = m_nNodes;
				s_pNode[m_nNodes].Copy(LB);
				m_nNodes++;
			}

			if(n3>=0)
			{
				s_pPanel[m_MatSize].m_iTB = n3;
			}
			else {
				s_pPanel[m_MatSize].m_iTB = m_nNodes;
				s_pNode[m_nNodes].Copy(TB);
				m_nNodes++;
			}

			s_pPanel[m_MatSize].m_Pos = MIDSURFACE;
			s_pPanel[m_MatSize].m_iElement = m_MatSize;
			s_pPanel[m_MatSize].m_iSym  = -1;
			s_pPanel[m_MatSize].m_bIsLeftPanel  = true;//no point for sails
			s_pPanel[m_MatSize].SetFrame(LA, LB, TA, TB);


			// set neighbour panels
			// valid only for Panel 2-sided Analysis
			// we are on the bottom or middle surface
			s_pPanel[m_MatSize].m_iPD = m_MatSize-1;
			s_pPanel[m_MatSize].m_iPU = m_MatSize+1;
			if(l==0)                   s_pPanel[m_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSail->m_NXPanels-1) s_pPanel[m_MatSize].m_iPU = -1;// no panel upstream


			//sails are modelled as thin surfaces
			s_pPanel[m_MatSize].m_iPR = m_MatSize + pSail->m_NXPanels;
			s_pPanel[m_MatSize].m_iPL = m_MatSize - pSail->m_NXPanels;
			if(k==0                  ) s_pPanel[m_MatSize].m_iPL = -1;
			if(k==pSail->m_NZPanels-1) s_pPanel[m_MatSize].m_iPR = -1;

			//do not link to next surfaces... will be done in JoinSurfaces() if surfaces are continuous
			if(k==0)                   s_pPanel[m_MatSize].m_iPR = -1;
			if(k==pSail->m_NZPanels-1) s_pPanel[m_MatSize].m_iPL = -1;

//			if(m_pPanel[m_MatSize].m_bIsTrailing)
			if(true)
			{
				s_pPanel[m_MatSize].m_iWake = 0;          //no need for wake panels in VLM formulation
				s_pPanel[m_MatSize].m_iWakeColumn = 0;    //no need for wake panels in VLM formulation
			}

			s_pPanel[m_MatSize].m_bIsLeftPanel = true;

			m_MatSize++;
		}
	}

	pSail->m_NElements = m_MatSize-InitialSize;
	return pSail->m_NElements;
}


int QSail7::CreateBodyElements(CBody *pBody)
{
	//
	// Creates the panel elements at the body's surface
	//
	if(!pBody) return 0;
	int i,j,k,l;
	double uk, uk1, v, dj, dj1, dl1;
	CVector LATB, TALB;
	CVector LA, LB, TA, TB;
	CVector PLA, PTA, PLB, PTB;

	int n0, n1, n2, n3, lnx, lnh;
	int nx = pBody->m_nxPanels;
	int nh = pBody->m_nhPanels;
	int p = 0;

	int InitialSize = m_MatSize;
	int FullSize =0;

	lnx = 0;


	if(pBody->m_LineType==BODYPANELTYPE)
	{
		nx = 0;
		for(i=0; i<pBody->FrameSize()-1; i++) nx+=pBody->m_xPanels[i];
		nh = 0;
		for(i=0; i<pBody->FramePointCount()-1; i++) nh+=pBody->m_hPanels[i];
		FullSize = nx*nh*2;
		pBody->m_nxPanels = nx;
		pBody->m_nhPanels = nh;

		for (i=0; i<pBody->FrameSize()-1; i++)
		{
			for (j=0; j<pBody->m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(pBody->m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(pBody->m_xPanels[i]);

				//body left side
				lnh = 0;
				for (k=0; k<pBody->FramePointCount()-1; k++)
				{
					//build the four corner points of the strips
					PLB.x =  (1.0- dj) * pBody->Frame(i)->m_Position.x       +  dj * pBody->Frame(i+1)->m_Position.x      +pBody->m_LEPosition.x;
					PLB.y = -(1.0- dj) * pBody->Frame(i)->m_CtrlPoint[k].y   -  dj * pBody->Frame(i+1)->m_CtrlPoint[k].y;
					PLB.z =  (1.0- dj) * pBody->Frame(i)->m_CtrlPoint[k].z   +  dj * pBody->Frame(i+1)->m_CtrlPoint[k].z  +pBody->m_LEPosition.z;

					PTB.x =  (1.0-dj1) * pBody->Frame(i)->m_Position.x       + dj1 * pBody->Frame(i+1)->m_Position.x      +pBody->m_LEPosition.x;
					PTB.y = -(1.0-dj1) * pBody->Frame(i)->m_CtrlPoint[k].y   - dj1 * pBody->Frame(i+1)->m_CtrlPoint[k].y;
					PTB.z =  (1.0-dj1) * pBody->Frame(i)->m_CtrlPoint[k].z   + dj1 * pBody->Frame(i+1)->m_CtrlPoint[k].z  +pBody->m_LEPosition.z;

					PLA.x =  (1.0- dj) * pBody->Frame(i)->m_Position.x       +  dj * pBody->Frame(i+1)->m_Position.x      +pBody->m_LEPosition.x;
					PLA.y = -(1.0- dj) * pBody->Frame(i)->m_CtrlPoint[k+1].y -  dj * pBody->Frame(i+1)->m_CtrlPoint[k+1].y;
					PLA.z =  (1.0- dj) * pBody->Frame(i)->m_CtrlPoint[k+1].z +  dj * pBody->Frame(i+1)->m_CtrlPoint[k+1].z+pBody->m_LEPosition.z;

					PTA.x =  (1.0-dj1) * pBody->Frame(i)->m_Position.x       + dj1 * pBody->Frame(i+1)->m_Position.x      +pBody->m_LEPosition.x;
					PTA.y = -(1.0-dj1) * pBody->Frame(i)->m_CtrlPoint[k+1].y - dj1 * pBody->Frame(i+1)->m_CtrlPoint[k+1].y;
					PTA.z =  (1.0-dj1) * pBody->Frame(i)->m_CtrlPoint[k+1].z + dj1 * pBody->Frame(i+1)->m_CtrlPoint[k+1].z+pBody->m_LEPosition.z;

					LB = PLB;
					TB = PTB;

					for (l=0; l<pBody->m_hPanels[k]; l++)
					{
						dl1  = (double)(l+1) / (double)(pBody->m_hPanels[k]);
						LA = PLB * (1.0- dl1) + PLA * dl1;
						TA = PTB * (1.0- dl1) + PTA * dl1;

						n0 = IsNode(LA);
						n1 = IsNode(TA);
						n2 = IsNode(LB);
						n3 = IsNode(TB);

						if(n0>=0) {
							s_pPanel[m_MatSize].m_iLA = n0;
						}
						else {
							s_pPanel[m_MatSize].m_iLA = m_nNodes;
							s_pNode[m_nNodes].Copy(LA);
							m_nNodes++;
						}

						if(n1>=0) {
							s_pPanel[m_MatSize].m_iTA = n1;
						}
						else {
							s_pPanel[m_MatSize].m_iTA = m_nNodes;
							s_pNode[m_nNodes].Copy(TA);
							m_nNodes++;
						}

						if(n2>=0) {
							s_pPanel[m_MatSize].m_iLB = n2;
						}
						else {
							s_pPanel[m_MatSize].m_iLB = m_nNodes;
							s_pNode[m_nNodes].Copy(LB);
							m_nNodes++;
						}

						if(n3 >=0) {
							s_pPanel[m_MatSize].m_iTB = n3;
						}
						else {
							s_pPanel[m_MatSize].m_iTB = m_nNodes;
							s_pNode[m_nNodes].Copy(TB);
							m_nNodes++;
						}

						LATB = TB - LA;
						TALB = LB - TA;
						s_pPanel[m_MatSize].Normal = LATB * TALB;
						s_pPanel[m_MatSize].Area =  s_pPanel[m_MatSize].Normal.VAbs()/2.0;
						s_pPanel[m_MatSize].Normal.Normalize();

						s_pPanel[m_MatSize].m_bIsInSymPlane  = false;
						s_pPanel[m_MatSize].m_bIsLeading     = false;
						s_pPanel[m_MatSize].m_bIsTrailing    = false;
						s_pPanel[m_MatSize].m_Pos     = BODYSURFACE;
						s_pPanel[m_MatSize].m_iElement = m_MatSize;
						s_pPanel[m_MatSize].m_iSym     = -1;
						s_pPanel[m_MatSize].m_bIsLeftPanel  = true;
						s_pPanel[m_MatSize].SetFrame(LA, LB, TA, TB);

						// set neighbour panels

						s_pPanel[m_MatSize].m_iPD = m_MatSize + nh;
						s_pPanel[m_MatSize].m_iPU = m_MatSize - nh;

						if(lnx==0)      s_pPanel[m_MatSize].m_iPU = -1;// no panel downstream
						if(lnx==nx-1)	s_pPanel[m_MatSize].m_iPD = -1;// no panel upstream

						s_pPanel[m_MatSize].m_iPL = m_MatSize + 1;
						s_pPanel[m_MatSize].m_iPR = m_MatSize - 1;

						if(lnh==0)     s_pPanel[m_MatSize].m_iPR = InitialSize + FullSize - p - 1;
						if(lnh==nh-1)  s_pPanel[m_MatSize].m_iPL = InitialSize + FullSize - p - 1;

						m_MatSize++;
						p++;
						LB = LA;
						TB = TA;
						lnh++;
					}
				}
				lnx++;
			}
		}
	}
	else if(pBody->m_LineType==BODYSPLINETYPE)
	{
		pBody->SetPanelPos();
		FullSize = 2*nx*nh;
		//start with left side... same as for wings
		for (k=0; k<nx; k++)
		{
			uk  = pBody->s_XPanelPos[k];
			uk1 = pBody->s_XPanelPos[k+1];

			pBody->GetPoint(uk,  0, false, LB);
			pBody->GetPoint(uk1, 0, false, TB);

			LB += pBody->m_LEPosition;
			TB += pBody->m_LEPosition;

			for (l=0; l<nh; l++)
			{
				//start with left side... same as for wings
				v = (double)(l+1) / (double)(nh);
				pBody->GetPoint(uk,  v, false, LA);
				pBody->GetPoint(uk1, v, false, TA);

				LA += pBody->m_LEPosition;
				TA += pBody->m_LEPosition;

				n0 = IsNode(LA);
				n1 = IsNode(TA);
				n2 = IsNode(LB);
				n3 = IsNode(TB);

				if(n0>=0) {
					s_pPanel[m_MatSize].m_iLA = n0;
				}
				else {
					s_pPanel[m_MatSize].m_iLA = m_nNodes;
					s_pNode[m_nNodes].Copy(LA);
					m_nNodes++;
				}

				if(n1>=0) {
					s_pPanel[m_MatSize].m_iTA = n1;
				}
				else {
					s_pPanel[m_MatSize].m_iTA = m_nNodes;
					s_pNode[m_nNodes].Copy(TA);
					m_nNodes++;
				}

				if(n2>=0) {
					s_pPanel[m_MatSize].m_iLB = n2;
				}
				else {
					s_pPanel[m_MatSize].m_iLB = m_nNodes;
					s_pNode[m_nNodes].Copy(LB);
					m_nNodes++;
				}

				if(n3 >=0) {
					s_pPanel[m_MatSize].m_iTB = n3;
				}
				else {
					s_pPanel[m_MatSize].m_iTB = m_nNodes;
					s_pNode[m_nNodes].Copy(TB);
					m_nNodes++;
				}

				LATB = TB - LA;
				TALB = LB - TA;
				s_pPanel[m_MatSize].Normal = LATB * TALB;
				s_pPanel[m_MatSize].Area =  s_pPanel[m_MatSize].Normal.VAbs()/2.0;
				s_pPanel[m_MatSize].Normal.Normalize();

				s_pPanel[m_MatSize].m_bIsInSymPlane  = false;
				s_pPanel[m_MatSize].m_bIsLeading     = false;
				s_pPanel[m_MatSize].m_bIsTrailing    = false;
				s_pPanel[m_MatSize].m_Pos     = BODYSURFACE;
				s_pPanel[m_MatSize].m_iElement = m_MatSize;
				s_pPanel[m_MatSize].m_iSym     = -1;
				s_pPanel[m_MatSize].m_bIsLeftPanel  = true;
				s_pPanel[m_MatSize].SetFrame(LA, LB, TA, TB);

				// set neighbour panels

				s_pPanel[m_MatSize].m_iPD = m_MatSize + nh;
				s_pPanel[m_MatSize].m_iPU = m_MatSize - nh;

				if(k==0)    s_pPanel[m_MatSize].m_iPU = -1;// no panel downstream
				if(k==nx-1)	s_pPanel[m_MatSize].m_iPD = -1;// no panel upstream

				s_pPanel[m_MatSize].m_iPL = m_MatSize + 1;
				s_pPanel[m_MatSize].m_iPR = m_MatSize - 1;

				if(l==0)     s_pPanel[m_MatSize].m_iPR = InitialSize + FullSize - p - 1;
				if(l==nh-1)  s_pPanel[m_MatSize].m_iPL = InitialSize + FullSize - p - 1;

				m_MatSize++;
				p++;
				LB = LA;
				TB = TA;
			}
		}
	}


	//right side next
	i = m_MatSize;

	for (k=nx-1; k>=0; k--)
	{
		for (l=nh-1; l>=0; l--)
		{
			i--;
			LA = s_pNode[s_pPanel[i].m_iLB];
			TA = s_pNode[s_pPanel[i].m_iTB];
			LB = s_pNode[s_pPanel[i].m_iLA];
			TB = s_pNode[s_pPanel[i].m_iTA];

			LA.y = -LA.y+2*pBody->m_LEPosition.y;
			LB.y = -LB.y+2*pBody->m_LEPosition.y;
			TA.y = -TA.y+2*pBody->m_LEPosition.y;
			TB.y = -TB.y+2*pBody->m_LEPosition.y;

			n0 = IsNode(LA);
			n1 = IsNode(TA);
			n2 = IsNode(LB);
			n3 = IsNode(TB);

			if(n0>=0) {
				s_pPanel[m_MatSize].m_iLA = n0;
			}
			else {
				s_pPanel[m_MatSize].m_iLA = m_nNodes;
				s_pNode[m_nNodes].Copy(LA);
				m_nNodes++;
			}

			if(n1>=0) {
				s_pPanel[m_MatSize].m_iTA = n1;
			}
			else {
				s_pPanel[m_MatSize].m_iTA = m_nNodes;
				s_pNode[m_nNodes].Copy(TA);
				m_nNodes++;
			}

			if(n2>=0) {
				s_pPanel[m_MatSize].m_iLB = n2;
			}
			else {
				s_pPanel[m_MatSize].m_iLB = m_nNodes;
				s_pNode[m_nNodes].Copy(LB);
				m_nNodes++;
			}

			if(n3 >=0) {
				s_pPanel[m_MatSize].m_iTB = n3;
			}
			else {
				s_pPanel[m_MatSize].m_iTB = m_nNodes;
				s_pNode[m_nNodes].Copy(TB);
				m_nNodes++;
			}

			LATB = TB - LA;
			TALB = LB - TA;
			s_pPanel[m_MatSize].Normal = LATB * TALB;
			s_pPanel[m_MatSize].Area =  s_pPanel[m_MatSize].Normal.VAbs()/2.0;
			s_pPanel[m_MatSize].Normal.Normalize();

			s_pPanel[m_MatSize].m_bIsInSymPlane  = false;
			s_pPanel[m_MatSize].m_bIsLeading     = false;
			s_pPanel[m_MatSize].m_bIsTrailing    = false;
			s_pPanel[m_MatSize].m_Pos = BODYSURFACE;
			s_pPanel[m_MatSize].m_iElement = m_MatSize;
			s_pPanel[m_MatSize].m_iSym = -1;
			s_pPanel[i].m_iSym = m_MatSize;
			s_pPanel[m_MatSize].m_bIsLeftPanel  = false;
			s_pPanel[m_MatSize].SetFrame(LA, LB, TA, TB);

			// set neighbour panels
			// valid only for Panel Analysis

			s_pPanel[m_MatSize].m_iPD = m_MatSize - nh;
			s_pPanel[m_MatSize].m_iPU = m_MatSize + nh;

			if(k==0)	s_pPanel[m_MatSize].m_iPU = -1;// no panel downstream
			if(k==nx-1)	s_pPanel[m_MatSize].m_iPD = -1;// no panel upstream

			s_pPanel[m_MatSize].m_iPL = m_MatSize + 1;
			s_pPanel[m_MatSize].m_iPR = m_MatSize - 1;

			if(l==0)     s_pPanel[m_MatSize].m_iPL = InitialSize + FullSize - p - 1;
			if(l==nh-1)  s_pPanel[m_MatSize].m_iPR = InitialSize + FullSize - p - 1;

			m_MatSize++;
			p++;
			LB = LA;
			TB = TA;
		}
	}
	pBody->m_NElements = m_MatSize-InitialSize;
	return pBody->m_NElements;
}


void QSail7::GLCreateWindList()
{
	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;
	double s, h, height;
	CVector w(1.0,0.0,0.0);
	s=1.0;

	if(!m_pCurBoat || !m_pCurBoatPolar) return;

	height = m_pCurBoat->Height() * 2.0;

	glNewList(WINDLIST, GL_COMPILE);
	{
		glLineWidth(2.0);
		glColor3d(0.7,1.0,0.7);

		for(int iw=0; iw<=10; iw++)
		{
			h = (double)iw/10.0 * height;

			s = m_pCurBoatPolar->WindFactor(h);
//			p3DWidget->GLDrawCylinder(QColor(150,150,150), 0.13*s, 0.13*s, 0.0*s, 1.5*s, 31, 11);
//			p3DWidget->GLDrawCylinder(QColor(150,150,150), 0.31*s, 0.00*s, 1.5*s, 2.5*s, 31, 11);
			p3DWidget->GLDrawArrow(CVector(0.0, 0.0, iw*height/10.0), w, s*2.0);

//			glTranslated(0.0, 0.0, height/10.0);
		}

/*		glBegin(GL_LINE_STRIP);
		{
			for(int iw=0; iw<=100; iw++)
			{
				h = (double)iw/100.0 * height;
				s = m_pCurBoatPolar->WindFactor(h);

				glVertex3d(s*2.0, 0.0, iw*height/100.0);
			}
		}
		glEnd();*/
	}
	glEndList();
}



#define WATERMESH 100
void QSail7::GLCreateWaterList()
{
	QColor wColor(10, 70, 190);
	glNewList(WATERLIST, GL_COMPILE);
	{
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPolygonOffset(1.0, 1.0);

/*		double ti, tip1, tj;
		double lx=1.0, ly=5.;
		for(int iw=0; iw<WATERMESH; iw++)
		{
			ti   = (double) iw   /WATERMESH;
			tip1 = (double)(iw+1)/WATERMESH;

			glBegin(GL_QUAD_STRIP);
			{
				wColor.setGreen((int)(70+23*sin(iw/PI/100)));
				for(int jw=0; jw<WATERMESH; jw++)
				{
					wColor.setBlue((int)(190+31*sin(jw/PI/100)));
					glColor4d(wColor.redF(), wColor.greenF(), wColor.blueF(), .5);


					tj = (double)jw/WATERMESH;

					glNormal3d(-2.*PI* lx* cos(ti*2*PI*lx)*cos(tj*2*PI*ly),
							   +2.*PI* ly* sin(ti*2*PI*lx)*sin(tj*2*PI*ly),
							   1.0/.37);
					glVertex3d(-20.+ti  *40.0, -20.+tj*40, 0.37* sin(ti*2*PI*lx)   * cos(tj*2*PI*ly));
					glVertex3d(-20.+tip1*40.0, -20.+tj*40, 0.37* sin(tip1*2*PI*lx) * cos(tj*2*PI*ly));
				}
			}
			glEnd();
		}*/
		glColor4d(wColor.redF(), wColor.greenF(), wColor.blueF(), .3);
		glBegin(GL_QUADS);
		{
			glNormal3d(0.0,0.0,1.0);
			glVertex3d(-20.0, -20.0, 0.0);
			glVertex3d(-20.0,  20.0, 0.0);
			glVertex3d( 20.0,  20.0, 0.0);
			glVertex3d( 20.0, -20.0, 0.0);
		}
		glEnd();
	}
	glEndList();
}


void QSail7::OnNewBoat()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;


	//Define a Boat from scratch using the default values
	//On exit, check if the Boat's name is already used
	int i;
	CBoat *pOldBoat;
	CBoat *pNewBoat= new CBoat;

	BoatDlg dlg;
	dlg.InitDialog(pNewBoat);
	//	BoatDlg.m_bAcceptName= true;
	dlg.move(pMainFrame->m_DlgPos);

	if(QDialog::Accepted == dlg.exec())
	{
		pMainFrame->SetSaveState(false);
		bool bExists = false;
		for(i=0; i<m_poaBoat->size(); i++)
		{
			pOldBoat = (CBoat*)m_poaBoat->at(i);
			if(pNewBoat->m_BoatName == pOldBoat->m_BoatName)
			{
				bExists = true;
				break;
			}
		}

		if(bExists)
		{
			if(!SetModBoat(pNewBoat))
			{
				delete pNewBoat;
				SetControls();
				return;
			}
		}

		m_pCurBoat = AddBoat(pNewBoat);
		pMainFrame->SetSaveState(false);
		SetBoat();
		pMainFrame->UpdateBoats();
//		m_bIs2DScaleSet = false;
		SetScale();
		UpdateView();
	}
	else
	{
		delete pNewBoat;
	}
	SetControls();
	pMainFrame->m_DlgPos = dlg.pos();
}


void QSail7::OnEditCurBoat()
{
	if(!m_pCurBoat) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	BoatDlg dlg;
	BoatOpp *pBoatOpp;
	bool bHasResults = false;

	for (int i=0; i< m_poaBoatOpp->size(); i++)
	{
		pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(i);
		if(pBoatOpp->m_BoatName == m_pCurBoat->m_BoatName)
		{
			bHasResults = true;
			break;
		}
	}

	CBoat *pModBoat = new CBoat;
	pModBoat->DuplicateBoat(m_pCurBoat);
	dlg.InitDialog(pModBoat);
	dlg.move(pMainFrame->m_DlgPos);
	if (dlg.exec()==QDialog::Accepted)
	{
		// we returned from the dialog with an 'OK',
		pMainFrame->SetSaveState(false);
		//convert form to object
		if(dlg.m_bChanged)
		{
			if(bHasResults)
			{
				ModDlg Mdlg;
				Mdlg.move(pMainFrame->m_DlgPos);
				Mdlg.m_Question = tr("The modification will erase all results associated to this Wing.\nContinue ?");
				Mdlg.InitDialog();
				int Ans = Mdlg.exec();
				if (Ans == QDialog::Rejected)
				{
					delete pModBoat; // clean up
					return;
				}
				else if(Ans==20)
				{
					//user wants to save as a new boat
					if(!SetModBoat(pModBoat))
					{
						delete pModBoat;
						return;
					}
					else
					{
						m_pCurBoat = AddBoat(pModBoat);
					}
					SetBoat();
					pMainFrame->UpdateBoats();
					UpdateView();
					return;
				}
				else
				{
					//user wants to overwrite
					pMainFrame->DeleteBoat(m_pCurBoat,true);
					*m_pCurBoat = *pModBoat;
				}
			}
			else
			{
				//No results, record the changes without prompting the user
				*m_pCurBoat = *pModBoat;
			}

			if(m_iView==SAIL3DVIEW)
			{
			}
			else
			{
				if(m_iView==SAILPOLARVIEW)     CreateBoatPolarCurves();
			}
		}

		m_bResetglStream   = true;
		m_bResetglSpeeds   = true;
		m_bIs2DScaleSet    = false;
		m_bResetglBoat     = true;
		m_bResetglSailGeom = true;
		m_bResetglMesh     = true;
		m_bResetglOpp      = true;

		SetBoat();

		pMainFrame->UpdateBoats();
		SetScale();
		UpdateView();
	}
	else
	{
		delete pModBoat; // clean up
	}
	pMainFrame->m_DlgPos=dlg.pos();
}


void QSail7::OnRenameCurBoat()
{
	//Rename the currently selected Boat
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pCurBoat)	return;

	//Rename the currently selected UFO
	QString OldName;
	int l;

	OldName = m_pCurBoat->m_BoatName;
	SetModBoat(m_pCurBoat);

	BoatOpp   *pBoatOpp;
	BoatPolar *pBoatPolar;

	for (l=m_poaBoatPolar->size()-1;l>=0; l--)
	{
		pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(l);
		if (pBoatPolar->m_BoatName == OldName)
		{
			pBoatPolar->m_BoatName = m_pCurBoat->m_BoatName;
		}
	}
	for (l=m_poaBoatOpp->size()-1;l>=0; l--)
	{
		pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(l);
		if (pBoatOpp->m_BoatName==OldName)
		{
			pBoatOpp->m_BoatName = m_pCurBoat->m_BoatName;
		}
	}

	pMainFrame->UpdateBoats();
	SetBoatPolar();
	UpdateView();
}


void QSail7::OnDuplicateCurBoat()
{
	if(!m_pCurBoat) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	CBoat* pNewBoat= new CBoat;
	pNewBoat->DuplicateBoat(m_pCurBoat);

	if(!SetModBoat(pNewBoat))
	{
		delete pNewBoat;
		UpdateView();
	}
	else
	{
		m_pCurBoat = AddBoat(pNewBoat);
		SetBoat();
		pMainFrame->UpdateBoats();
		OnEditCurBoat();
	}
}


void QSail7::OnDeleteCurBoat()
{
	//
	// The user has requested a deletion of the current wing of plane
	//

	if(!m_pCurBoat) return;
	m_bAnimateBoatOpp = false;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong = tr("Are you sure you want to delete the boat :\n") +  m_pCurBoat->m_BoatName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	pMainFrame->DeleteBoat(m_pCurBoat);

	SetBoat();
	pMainFrame->UpdateBoats();
	SetControls();
	UpdateView();
}

void QSail7::OnDeleteCurBoatOpp()
{
	//
	// The user has requested a deletion of the current operating point
	//

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i;
	if(!m_pCurBoatOpp) return;


	BoatOpp* pBoatOpp;
	for (i = m_poaBoatOpp->size()-1; i>=0; i--)
	{
		pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(i);
		if(pBoatOpp == m_pCurBoatOpp)
		{
			m_poaBoatOpp->removeAt(i);
			delete pBoatOpp;
			m_pCurBoatOpp = NULL;
			break;
		}
	}
	pMainFrame->UpdateBoatOpps();
	SetBoatOpp(true);
	if(pMainFrame->m_pctrlBoatOpp->count())
	{
		QString strong;
		double x;
		pMainFrame->m_pctrlBoatOpp->setCurrentIndex(0);
		strong = pMainFrame->m_pctrlBoatOpp->itemText(0);
		bool bRes;
		x = strong.toDouble(&bRes);
		if(bRes)
		{
			m_pCurBoatOpp = GetBoatOpp(x);
		}
		else m_pCurBoatOpp = NULL;
	}
	else
	{
		m_pCurBoatOpp = NULL;
	}
	pMainFrame->SetSaveState(false);

	UpdateView();

	SetControls();
}

void QSail7::OnDeleteCurBoatPolar()
{
	//
	// The user has requested a deletion of the current BoatPolar object
	//

	if(!m_pCurBoatPolar) return;
	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QString strong = tr("Are you sure you want to delete the polar :\n") +  m_pCurBoatPolar->m_BoatPolarName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	//first remove all BoatOpps associated to the Wing Polar
	BoatOpp * pBoatOpp;
	for (i=m_poaBoatOpp->size()-1; i>=0; i--)
	{
		pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(i);
		if (pBoatOpp->m_BoatPolarName == m_pCurBoatPolar->m_BoatPolarName   &&  pBoatOpp->m_BoatName == m_pCurBoat->m_BoatName)
		{
			m_poaBoatOpp->removeAt(i);
			delete pBoatOpp;
		}
	}


	//next remove the BoatPolar
	BoatPolar* pBoatPolar;
	for (i=m_poaBoatPolar->size()-1;i>=0; i--)
	{
		pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(i);
		if (pBoatPolar == m_pCurBoatPolar)
		{
			m_poaBoatPolar->removeAt(i);
			delete pBoatPolar;
			break;
		}
	}

	m_pCurBoatOpp = NULL;
	m_pCurBoatPolar = NULL;
	pMainFrame->SetSaveState(false);
	SetBoatPolar();

	pMainFrame->UpdateBoatPolars();
	SetControls();
	UpdateView();
}


void QSail7::OnDeleteAllBoatPolarOpps()
{
	//
	// The user has requested a deletion of all the BoatOpps or POpps associated to the current WPolar
	//

	if(!m_pCurBoatPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	BoatOpp* pBoatOpp;
	int i;

	for (i=m_poaBoatOpp->size()-1; i>=0; i--)
	{
		pBoatOpp = (BoatOpp*) m_poaBoatOpp->at(i);
		if(pBoatOpp->m_BoatPolarName == m_pCurBoatPolar->m_BoatPolarName &&
		   pBoatOpp->m_BoatName == m_pCurBoat->m_BoatName)
		{
			m_poaBoatOpp->removeAt(i);
			delete pBoatOpp;
		}
	}


	m_pCurBoatOpp = NULL;
	m_bResetglMesh = true;
	pMainFrame->UpdateBoatOpps();
	SetBoatOpp(true);
	SetControls();
	UpdateView();
}


void QSail7::OnDeleteAllBoatOpps()
{
	//
	// The user has requested a deletion of all the BoatOpps or POpps
	//

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	BoatOpp* pBoatOpp;

	int i;
	for (i=m_poaBoatOpp->size()-1; i>=0; i--)
	{
		pBoatOpp = (BoatOpp*) m_poaBoatOpp->at(i);
		m_poaBoatOpp->removeAt(i);
		delete pBoatOpp;
	}


	m_pCurBoatOpp = NULL;
	pMainFrame->UpdateBoatOpps();
	SetBoatOpp(true);

	SetControls();
	UpdateView();
}




void QSail7::wheelEvent(QWheelEvent *event)
{
	//The mouse button has been wheeled
	//Process the message
	MainFrame * pMainFrame = (MainFrame*)s_pMainFrame;
	QPoint pt(event->x(), event->y()); //client coordinates
	static double ZoomFactor;
	if(event->delta()>0)
	{
		if(!pMainFrame->m_bReverseZoom) ZoomFactor = 1./1.06;
		else                            ZoomFactor = 1.06;
	}
	else
	{
		if(!pMainFrame->m_bReverseZoom) ZoomFactor = 1.06;
		else                            ZoomFactor = 1./1.06;
	}

	if(m_iView==SAIL3DVIEW)
	{
			//zoom sail
			m_glScaled *= ZoomFactor;
			UpdateView();
	}
	else if(m_iView==SAILPOLARVIEW)
	{
		m_pCurGraph = GetGraph(pt);
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt))
		{
			if (m_bXPressed)
			{
				//zoom x scale
				m_pCurGraph->SetAutoX(false);
				m_pCurGraph->Scalex(1./ZoomFactor);
			}
			else if(m_bYPressed)
			{
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				m_pCurGraph->Scaley(1./ZoomFactor);
			}
			else
			{
				//zoom both
				m_pCurGraph->SetAuto(false);
				m_pCurGraph->Scale(1./ZoomFactor);
			}

			m_pCurGraph->SetAutoXUnit();
			m_pCurGraph->SetAutoYUnit();
			UpdateView();

		}
	}
}




void QSail7::mousePressEvent(QMouseEvent *event)
{
	//
	// capture and dispatch user mouse input
	//
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_iView==SAIL3DVIEW && (event->buttons() & Qt::MidButton))
	{
		m_bArcball = true;
		m_ArcBall.Start(event->pos().x(), m_r3DCltRect.height()-event->pos().y());
		m_bCrossPoint = true;

		Set3DRotationCenter();
		UpdateView();
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		QPoint point = event->pos();

		if(m_iView==SAIL3DVIEW)
		{
			ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;
			//	point is in client coordinates

			CVector Real;
			bool bCtrl = false;
			if(event->modifiers() & Qt::ControlModifier)
			{
				m_bArcball = true;
				bCtrl =true;
			}

			p3DWidget->ClientToGL(point, Real);

			if(m_r3DCltRect.contains(point)) p3DWidget->setFocus();

			if(m_bPickCenter)
			{
				Set3DRotationCenter(point);
				m_bPickCenter = false;
				m_pctrlPickCenter->setChecked(false);
			}
			else
			{
				m_ArcBall.Start(point.x(), m_r3DCltRect.height()-point.y());
				m_bCrossPoint = true;

				Set3DRotationCenter();

				if (!bCtrl)
				{
					m_bTrans = true;
					p3DWidget->setCursor(Qt::ClosedHandCursor);
				}
			}

			UpdateView();

			m_bPickCenter = false;
		}
		else
		{
			TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
			m_pCurGraph = GetGraph(point);
			if(p2DWidget->geometry().contains(point)) p2DWidget->setFocus();

			if (m_pCurBoat || (m_pCurGraph && m_pCurGraph->IsInDrawRect(point)))
			{
				m_LastPoint.rx() = point.x();
				m_LastPoint.ry() = point.y();

				m_bTrans = true;
				m_bTransGraph = true;
				if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point)) m_bTransGraph = true;
				else                                                m_bTransGraph = false;
				if(m_bTrans || m_bTransGraph) p2DWidget->setCursor(Qt::ClosedHandCursor);

			}
		}
		m_PointDown = point;
		m_LastPoint = point;
	}
}


void QSail7::mouseReleaseEvent(QMouseEvent *event)
{
	//
	// capture and dispatch user mouse input
	//

	if(m_iView==SAIL3DVIEW)
	{
		ThreeDWidget* p3DWidget = (ThreeDWidget*)s_p3DWidget;
		p3DWidget->setCursor(Qt::CrossCursor);

		m_bArcball = false;
		m_bCrossPoint = false;
		UpdateView();

	//	We need to re-calculate the translation vector
		int i,j;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
				MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

		p3DWidget->GLInverseMatrix(MatIn, MatOut);
		m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
		m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
		m_glViewportTrans.z =  (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);
	}
	else
	{
		TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
		p2DWidget->setCursor(Qt::CrossCursor);
	}
	m_bTrans = false;
}




void QSail7::Set3DRotationCenter()
{
	//adjust the new rotation center after a translation or a rotation
	int i,j;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatOut[i][j] =  m_ArcBall.ab_quat[i*4+j];

	m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
	m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
	m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;
}



void QSail7::Set3DRotationCenter(QPoint point)
{
	//adjusts the new rotation center after the user has picked a point on the screen
	//finds the closest panel under the point,
	//and changes the rotation vector and viewport translation
	int  i, j, p;
	CVector I, A, B, AA, BB, PP, U;
	double dmin, dist;
	ThreeDWidget* p3DWidget = (ThreeDWidget*)s_p3DWidget;

	i=-1;
	dmin = 100000.0;

	p3DWidget->ClientToGL(point, B);

	B.x += -m_ObjectOffset.x - m_glViewportTrans.x*m_glScaled;
	B.y += -m_ObjectOffset.y + m_glViewportTrans.y*m_glScaled;

	B *= 1.0/m_glScaled;

	A.Set(B.x, B.y, +1.0);
	B.z = -1.0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

	//convert screen to model coordinates
	AA.x = MatIn[0][0]*A.x + MatIn[0][1]*A.y + MatIn[0][2]*A.z;
	AA.y = MatIn[1][0]*A.x + MatIn[1][1]*A.y + MatIn[1][2]*A.z;
	AA.z = MatIn[2][0]*A.x + MatIn[2][1]*A.y + MatIn[2][2]*A.z;

	BB.x = MatIn[0][0]*B.x + MatIn[0][1]*B.y + MatIn[0][2]*B.z;
	BB.y = MatIn[1][0]*B.x + MatIn[1][1]*B.y + MatIn[1][2]*B.z;
	BB.z = MatIn[2][0]*B.x + MatIn[2][1]*B.y + MatIn[2][2]*B.z;

	U.Set(BB.x-AA.x, BB.y-AA.y, BB.z-AA.z);
	U.Normalize();

	bool bIntersect = false;

	if(m_iView==SAIL3DVIEW)
	{
		for(p=0; p<m_MatSize; p++)
		{
			if(s_pPanel[p].Intersect(AA, U, I, dist))
			{
				if(dist < dmin)
				{
					dmin = dist;
					PP.Set(I);
					bIntersect = true;
				}
			}
		}
	}

	if(bIntersect)
	{

//		smooth visual transition
		p3DWidget->GLInverseMatrix(MatIn, MatOut);

		U.x = (-PP.x -m_glRotCenter.x)/30.0;
		U.y = (-PP.y -m_glRotCenter.y)/30.0;
		U.z = (-PP.z -m_glRotCenter.z)/30.0;

		for(i=0; i<30; i++)
		{
			m_glRotCenter +=U;
			m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
			m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
			m_glViewportTrans.z=   (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);

			UpdateView();
		}
	}
}




void QSail7::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Control:
		{
			m_bArcball = false;
			UpdateView();
			break;
		}
		default:
			event->ignore();
	}
}



void QSail7::mouseDoubleClickEvent (QMouseEvent * event)
{
	//
	// capture and dispatch user mouse input
	//

	if(m_iView==SAIL3DVIEW)
	{
		QPoint point = event->pos();

		ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;

		CVector Real;
		p3DWidget->ClientToGL(point, Real);
		if(m_r3DCltRect.contains(point)) p3DWidget->setFocus();

		Set3DRotationCenter(point);
		m_bPickCenter = false;
		m_pctrlPickCenter->setChecked(false);
		UpdateView();
	}
	else if (m_pCurGraph)
	{
		OnGraphSettings();
	}

}

void QSail7::Set2DScale()
{
}

void QSail7::Set3DScale()
{
	if(m_iView!=SAIL3DVIEW ) return;

	ThreeDWidget* p3DWidget = (ThreeDWidget*)s_p3DWidget;
	m_r3DCltRect = p3DWidget->geometry();
	if(m_pCurBoat)
	{
		double lmax=5;
		for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
		{
//			CSail*pSail= (CSail*)m_pCurBoat->m_poaSail.at(is);
//			SailSection *pSailSection = (SailSection*)pSail->m_oaSection.at(0);
//			lmax = qMax(lmax, pSailSection->m_Chord);
		}
		m_glScaled = (GLfloat)(3./4.*m_GLScale/lmax);
		m_glViewportTrans.x = 0.0;
		m_glViewportTrans.y = 0.0;
		m_glViewportTrans.z = 0.0;
		m_bIs3DScaleSet = true;
	}
	else
	{
		m_glScaled = (GLfloat)(3./4.*2.0*m_GLScale/10.);
		m_ObjectOffset.x = 0.0;
		m_ObjectOffset.y = 0.0;
	}
}




void QSail7::mouseMoveEvent(QMouseEvent *event)
{
	//
	// capture and process user mouse input
	//

	if(!hasFocus()) setFocus();
	static CVector Real;
	static bool bCtrl;
	static QPoint Delta, point;
	static double xu, yu, x1, y1, xmin, xmax, ymin, ymax;
	ThreeDWidget* p3DWidget = (ThreeDWidget*)s_p3DWidget;

	Delta.setX(event->pos().x() - m_LastPoint.x());
	Delta.setY(event->pos().y() - m_LastPoint.y());
	point = event->pos();

	bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;
	if(m_iView==SAIL3DVIEW)
	{
		p3DWidget->ClientToGL(point, Real);

		if (event->buttons() & Qt::LeftButton)
		{
			if(bCtrl)//rotate
			{
				m_bWindFront =  m_bWindRear = false;
				SetViewControls();
				m_ArcBall.Move(point.x(), m_r3DCltRect.height()-point.y());
				UpdateView();
			}
			else if(m_bTrans)
			{
				//translate
				m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0*m_GLScale/m_glScaled/m_r3DCltRect.width());
				m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0*m_GLScale/m_glScaled/m_r3DCltRect.width());

				m_glRotCenter.x = MatOut[0][0]*m_glViewportTrans.x + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
				m_glRotCenter.y = MatOut[1][0]*m_glViewportTrans.x + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
				m_glRotCenter.z = MatOut[2][0]*m_glViewportTrans.x + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

				UpdateView();
			}
		}
		else if ((event->buttons() & Qt::MidButton) && !bCtrl)
		//rotating
		{
			m_bWindFront =  m_bWindRear = false;
			SetViewControls();
			m_ArcBall.Move(point.x(), m_r3DCltRect.height()-point.y());
			UpdateView();
		}
	}
	else
	{
		m_pCurGraph = GetGraph(point);
		if ((event->buttons() & Qt::LeftButton) && m_bTrans && (m_iView==SAILPOLARVIEW))
		{
			if(m_pCurGraph && m_bTransGraph)
			{
				// we translate the curves inside the graph
				m_pCurGraph->SetAuto(false);
				x1 =  m_pCurGraph->ClientTox(m_LastPoint.x()) ;
				y1 =  m_pCurGraph->ClientToy(m_LastPoint.y()) ;

				xu = m_pCurGraph->ClientTox(point.x());
				yu = m_pCurGraph->ClientToy(point.y());

				xmin = m_pCurGraph->GetXMin() - xu+x1;
				xmax = m_pCurGraph->GetXMax() - xu+x1;
				ymin = m_pCurGraph->GetYMin() - yu+y1;
				ymax = m_pCurGraph->GetYMax() - yu+y1;

				m_pCurGraph->SetWindow(xmin, xmax, ymin, ymax);
				UpdateView();
			}
			else if (m_iView==SAILPOLARVIEW)
			{
				// we translate the legend
				m_BoatPlrLegendOffset.rx() += Delta.x();
				m_BoatPlrLegendOffset.ry() += Delta.y();
				UpdateView();
			}
		}

		else if ((event->buttons() & Qt::MidButton) && !bCtrl)
		//scaling
		{
			// we zoom the graph
			if(m_iView==SAILPOLARVIEW)
			{
				if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
				{
					//zoom graph
					m_pCurGraph->SetAuto(false);
					if(point.y()-m_LastPoint.y()<0) m_pCurGraph->Scale(1.02);
					else                            m_pCurGraph->Scale(1.0/1.02);

					UpdateView();
				}
			}
		}
		else if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
		{
			MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
			pMainFrame->statusBar()->showMessage(QString("X =%1, Y = %2").arg(m_pCurGraph->ClientTox(event->x())).arg(m_pCurGraph->ClientToy(event->y())));
		}
	}
	m_LastPoint = point;
}




void QSail7::On3DIso()
{
	//
	// The user has requested a perspective view in the 3D display
	//
	m_bWindFront =  m_bWindRear = false;
	SetViewControls();
	m_pctrlIso->setChecked(true);

	m_ArcBall.ab_quat[0]	= -0.65987748f;
	m_ArcBall.ab_quat[1]	=  0.38526487f;
	m_ArcBall.ab_quat[2]	= -0.64508355f;
	m_ArcBall.ab_quat[3]	=  0.0f;
	m_ArcBall.ab_quat[4]	= -0.75137258f;
	m_ArcBall.ab_quat[5]	= -0.33720365f;
	m_ArcBall.ab_quat[6]	=  0.56721509f;
	m_ArcBall.ab_quat[7]	=  0.0f;
	m_ArcBall.ab_quat[8]	=  0.000f;
	m_ArcBall.ab_quat[9]	=  0.85899049f;
	m_ArcBall.ab_quat[10]	=  0.51199043f;
	m_ArcBall.ab_quat[11]	=  0.0f;
	m_ArcBall.ab_quat[12]	=  0.0f;
	m_ArcBall.ab_quat[13]	=  0.0f;
	m_ArcBall.ab_quat[14]	=  0.0f;
	m_ArcBall.ab_quat[15]	=  1.0f;

	Set3DRotationCenter();
	UpdateView();
}



void QSail7::On3DTop()
{
	//
	// The user has requested a top view in the 3D display
	//
	m_bWindFront =  m_bWindRear = false;
	SetViewControls();
	m_pctrlZ->setChecked(true);

	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


void QSail7::On3DLeft()
{
	//
	// The user has requested a left view in the 3D display
	//
	m_bWindFront =  m_bWindRear = false;
	SetViewControls();
	m_pctrlY->setChecked(true);

	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90 deg around x
	Set3DRotationCenter();
	UpdateView();
}


void QSail7::On3DFront()
{
	//
	// The user has requested a front view in the 3D display
	//
	m_bWindFront =  m_bWindRear = false;
	SetViewControls();
	m_pctrlX->setChecked(true);

	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,            sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


void QSail7::On3DWindFront()
{
	//
	// The user has requested a front view in the 3D display
	//
	m_bWindFront = true;
	m_bWindRear = false;
	SetViewControls();

	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,            sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	if(!m_pCurBoatOpp)	m_ArcBall.SetQuat(Qt1 * Qt2);
	else
	{
		CVector R(0.0, 0.0, 1.0);
		Quaternion Qt3(-m_pCurBoatOpp->m_Beta, R);// rotate by beta around z
		m_ArcBall.SetQuat((Qt1 * Qt2) * Qt3);
	}
	Set3DRotationCenter();
	UpdateView();
}


void QSail7::On3DWindRear()
{
	//
	// The user has requested a front view in the 3D display
	//
	m_bWindFront = false;
	m_bWindRear = true;
	SetViewControls();

	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,            sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	if(!m_pCurBoatOpp)	m_ArcBall.SetQuat(Qt1 * Qt2);
	else
	{
		CVector R(0.0, 0.0, 1.0);
		Quaternion Qt3(-m_pCurBoatOpp->m_Beta+180.0, R);// rotate by beta around z
		m_ArcBall.SetQuat((Qt1 * Qt2) * Qt3);
	}
	Set3DRotationCenter();
	UpdateView();
}



void QSail7::On3DReset()
{
	//
	// The user has requested a reset of the scales in the 3D view
	//
	m_glViewportTrans.Set(0.0, 0.0, 0.0);
	m_bPickCenter   = false;
	m_bIs3DScaleSet = false;
	Set3DScale();
	Set3DRotationCenter();
	UpdateView();
}



void QSail7::OnSetupLight()
{
	if(m_iView!=SAIL3DVIEW) return;
	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;

	GLLightDlg::s_bLight = m_bglLight;
	m_GLLightDlg.m_p3DWidget = s_p3DWidget;
	m_GLLightDlg.show();

	p3DWidget->GLSetupLight(&m_GLLightDlg, m_ObjectOffset.y, 1.0f);
	UpdateView();
}



void QSail7::SetControls()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView==SAILPOLARVIEW)  m_pctrlMiddleControls->setCurrentIndex(1);
	else                        m_pctrlMiddleControls->setCurrentIndex(0);

	if(m_iView==SAIL3DVIEW)     m_pctrBottomControls->setCurrentIndex(1);
	else                        m_pctrBottomControls->setCurrentIndex(0);

	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlPanels->setChecked(m_bVLMPanels);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlCp->setChecked(m_b3DCp);
	m_pctrlPanelForce->setChecked(m_bPanelForce);
	m_pctrlMoment->setChecked(m_bMoments);
	m_pctrlLift->setChecked(m_bLift);
	m_pctrlBodyForces->setChecked(m_bBodyForces);
	m_pctrlWindDirection->setChecked(m_bWindDirection);
	m_pctrlWater->setChecked(m_bWater);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlLight->setChecked(m_bglLight);
	m_pctrlSurfaces->setChecked(m_bSurfaces);
	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlStream->setChecked(m_bStream);
	m_pctrlSurfVel->setChecked(m_bSpeeds);
	m_pctrlClipPlanePos->setValue((int)(m_ClipPlanePos*100.0));

	m_pctrlCp->setEnabled(m_pCurBoatOpp);
	m_pctrlPanelForce->setEnabled(m_pCurBoatOpp);
	m_pctrlMoment->setEnabled(m_pCurBoatOpp);
	m_pctrlLift->setEnabled(m_pCurBoatOpp);
	m_pctrlBodyForces->setEnabled(m_pCurBoatOpp);
	m_pctrlWindDirection->setEnabled(m_pCurBoatOpp);
	m_pctrlStream->setEnabled(m_pCurBoatOpp);
	m_pctrlSurfVel->setEnabled(m_pCurBoatOpp);
	m_pctrlBOppAnimate->setEnabled(m_pCurBoatOpp);
	m_pctrlWindFront->setEnabled(m_pCurBoatOpp);
	m_pctrlWindRear->setEnabled(m_pCurBoatOpp);

	m_pctrlWater->setEnabled(m_pCurBoatPolar && m_pCurBoatPolar->m_bGround);

	pMainFrame->Sail7EditBoatAct->setEnabled(m_pCurBoat);
	pMainFrame->renameCurBoat->setEnabled(m_pCurBoat);
	pMainFrame->deleteCurBoat->setEnabled(m_pCurBoat);
	pMainFrame->duplicateCurBoat->setEnabled(m_pCurBoat);
	pMainFrame->deleteAllBoatOpps->setEnabled(m_pCurBoat);
	pMainFrame->showCurBoatPlrs->setEnabled(m_pCurBoat);
	pMainFrame->hideCurBoatPlrs->setEnabled(m_pCurBoat);
	pMainFrame->deleteCurBoatPlrs->setEnabled(m_pCurBoat);

	pMainFrame->defineBoatPolar->setEnabled(m_pCurBoat);
	pMainFrame->editBoatPolar->setEnabled(m_pCurBoatPolar);
	pMainFrame->deleteCurBoatPolar->setEnabled(m_pCurBoatPolar);
	pMainFrame->renameCurBoatPolar->setEnabled(m_pCurBoatPolar);
	pMainFrame->deleteAllBoatPolarOpps->setEnabled(m_pCurBoatPolar);
	pMainFrame->resetCurBoatPolar->setEnabled(m_pCurBoatPolar);
	pMainFrame->showBoatPolarProperties->setEnabled(m_pCurBoatPolar);

	pMainFrame->deleteCurBoatOpp->setEnabled(m_pCurBoat);
	pMainFrame->showBoatOppProperties->setEnabled(m_pCurBoat);

	pMainFrame->BoatPolarAct->setChecked(m_iView==SAILPOLARVIEW);
	pMainFrame->Boat3DAct->setChecked(m_iView==SAIL3DVIEW);

	pMainFrame->Sail73DLightAct->setChecked(m_bglLight);

	pMainFrame->Graph1->setChecked(m_iBoatPlrView==1 && (m_pCurGraph == m_BoatGraph));
	pMainFrame->Graph2->setChecked(m_iBoatPlrView==1 && (m_pCurGraph == m_BoatGraph+1));
	pMainFrame->Graph3->setChecked(m_iBoatPlrView==1 && (m_pCurGraph == m_BoatGraph+2));
	pMainFrame->Graph4->setChecked(m_iBoatPlrView==1 && (m_pCurGraph == m_BoatGraph+3));
	pMainFrame->twoGraphs->setChecked(m_iBoatPlrView==2);
	pMainFrame->fourGraphs->setChecked(m_iBoatPlrView==4);

	if(m_pCurBoatPolar)
	{
		QString PolarProps;
		m_pCurBoatPolar->GetPolarProperties(PolarProps);
		m_pctrlPolarProps1->setText(PolarProps);
	}
}


void QSail7::SetViewControls()
{
	m_pctrlWindFront->setEnabled(m_pCurBoatOpp);
	m_pctrlWindRear->setEnabled(m_pCurBoatOpp);
	m_pctrlWindFront->setChecked(m_bWindFront);
	m_pctrlWindRear->setChecked(m_bWindRear);
	m_pctrlX->setChecked(false);
	m_pctrlY->setChecked(false);
	m_pctrlZ->setChecked(false);
	m_pctrlIso->setChecked(false);
}



void QSail7::contextMenuEvent (QContextMenuEvent * event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;

	if(pMainFrame->m_pctrlCentralWidget->currentIndex()==0) p2DWidget->contextMenuEvent(event);
	else                                                    p3DWidget->contextMenuEvent(event);
}


void QSail7::SetScale()
{
	if(m_iView==SAIL3DVIEW) Set3DScale();
	else                    Set2DScale();

}


CBoat* QSail7::AddBoat(CBoat *pNewBoat)
{
	//
	// Adds the boat pointed by pBoat to the array of boat objects
	// places it in alphabetical order
	//
	int i,j;
	bool bExists   = false;
	bool bInserted = false;
	CBoat *pOldBoat;

//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(pNewBoat->m_BoatName.length())
	{
		for (i=0; i<m_poaBoat->size(); i++)
		{
			pOldBoat = (CBoat*)m_poaBoat->at(i);
			if (pOldBoat->m_BoatName == pNewBoat->m_BoatName)
			{
				bExists = true;
				break;
			}
		}
	}
	else bExists = true;

	while(!bInserted)
	{
		if(!bExists)
		{
			for (j=0; j<m_poaBoat->size(); j++)
			{
				pOldBoat = (CBoat*)m_poaBoat->at(j);
				if (pNewBoat->m_BoatName.compare(pOldBoat->m_BoatName, Qt::CaseInsensitive)<0)
				{
					m_poaBoat->insert(j, pNewBoat);
					bInserted = true;
					break;
				}
			}
			if(!bInserted)
			{
				m_poaBoat->append(pNewBoat);
				bInserted = true;
			}
			return pNewBoat;
		}
		else
		{
			//Ask for user intentions

			if(SetModBoat(pNewBoat))
			{
				m_poaBoat->append(pNewBoat);
				return pNewBoat;
			}
			else
			{
				delete pNewBoat;
				return NULL;
			}

			bExists = false;
		}
	}
	return NULL;
}


void QSail7::SetBoat(QString BoatName)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	CBoat* pBoat;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!BoatName.length())
	{
		if(m_pCurBoat) BoatName = m_pCurBoat->m_BoatName;
		else
		{
			if(m_poaBoat->size())
			{
				pBoat = (CBoat*)m_poaBoat->at(0);
				BoatName = pBoat->m_BoatName;
			}
			else BoatName.clear();

			if(!BoatName.size())
			{
//				for(int i=0; i<4; i++) m_pBoatList[i] = NULL;
				m_pCurBoat  = NULL;
//				if(m_iView==BoatPOLARVIEW)		CreateWPolarCurves();
//				else if(m_iView==BoatOPPVIEW)	CreateWOppCurves();

				QApplication::restoreOverrideCursor();

				return;
			}
		}
	}

	m_pCurBoat  = GetBoat(BoatName);

	if(!m_pCurBoat)
	{
		QApplication::restoreOverrideCursor();
		return;
	}


	m_bResetglBoat   = true;
	m_bResetglSailGeom = true;
	m_bResetglMesh   = true;
	m_bResetglBody   = true;
	m_bResetglStream = true;
	m_bResetglSpeeds = true;

	memset(s_pPanel, 0, sizeof(pMainFrame->m_Panel));
	memset(s_pNode,  0, sizeof(pMainFrame->m_Node));

	m_NSurfaces = 0;
	m_MatSize = 0;
	m_nNodes = 0;

	for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
	{
		QSail *pSail = (QSail*)m_pCurBoat->m_poaSail.at(is);
		if(pSail)
		{
			pSail->m_pPanel = s_pPanel+m_MatSize;
			CreateSailElements(pSail);
		}
	}

	for(int ib=0; ib<m_pCurBoat->m_poaHull.size(); ib++)
	{
		CBody *pHull = (CBody*)m_pCurBoat->m_poaHull.at(ib);
		if(pHull)
		{
			pHull->m_pPanel = s_pPanel+m_MatSize;
			CreateBodyElements(pHull);
		}
	}

	memcpy(s_pMemPanel, s_pPanel, m_MatSize* sizeof(CPanel));
	memcpy(s_pMemNode,  s_pNode,  m_nNodes * sizeof(CVector));

	if (m_pCurBoatPolar)
	{
		// try to set the same as the existing polar... Special for Marc
		SetBoatPolar(NULL, m_pCurBoatPolar->m_BoatPolarName);
	}
	else
	{
		SetBoatPolar();
	}

	SetScale();
//	SetWGraphScale();

	SetControls();


	QApplication::restoreOverrideCursor();

}


bool QSail7::SetModBoat(CBoat *pModBoat)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!pModBoat) pModBoat = m_pCurBoat;
	CBoat *pBoat, *pOldBoat;

	bool bExists = true;
	int resp, k, l;

	QStringList NameList;
	for(k=0; k<m_poaBoat->size(); k++)
	{
		pBoat = (CBoat*)m_poaBoat->at(k);
		NameList.append(pBoat->m_BoatName);
	}

	RenameDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_pstrArray = & NameList;
	dlg.m_strQuestion = tr("Enter the new name for the Boat :");
	dlg.m_strName = pModBoat->m_BoatName;
	dlg.InitDialog();

	while (bExists)
	{
		resp = dlg.exec();
		pMainFrame->m_DlgPos = dlg.pos();
		if(resp==QDialog::Accepted)
		{
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaBoat->size(); k++)
			{
				pBoat = (CBoat*)m_poaBoat->at(k);
				if (pBoat->m_BoatName == dlg.m_strName)
				{
					bExists = true;
					break;
				}
			}

			if(!bExists)
			{
				pModBoat->m_BoatName = dlg.m_strName;
				//replace the Boat in alphabetical order in the array
				//remove the current Boat from the array
				bool bInserted = false;
				for (l=0; l<m_poaBoat->size();l++)
				{
					pBoat = (CBoat*)m_poaBoat->at(l);
					if(pBoat == pModBoat)
					{
						m_poaBoat->removeAt(l);
						// but don't delete it !
						//and re-insert it
						for (l=0; l<m_poaBoat->size();l++)
						{
							pBoat = (CBoat*)m_poaBoat->at(l);
							if(pBoat->m_BoatName.compare(pModBoat->m_BoatName, Qt::CaseInsensitive) >0)
							{
								//then insert before
								m_poaBoat->insert(l, pModBoat);
								bInserted = true;
								break;
							}
						}
						if(!bInserted)	m_poaBoat->append(pModBoat);
						break;
					}
				}
				pMainFrame->SetSaveState(false);
				return true;
			}
		}
		else if(resp==10)
		{
			//user wants to overwrite
			pOldBoat  = GetBoat(dlg.m_strName);

			if(pOldBoat)
			{
				//remove all results associated to the old boat
				for (l=m_poaBoatOpp->size()-1;l>=0; l--)
				{
					BoatOpp *pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(l);
					if (pBoatOpp->m_BoatName == pOldBoat->m_BoatName)
					{
						m_poaBoatOpp->removeAt(l);
						delete pBoatOpp;
					}
				}
				for (l=m_poaBoatPolar->size()-1;l>=0; l--)
				{
					BoatPolar *pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(l);
					if (pBoatPolar->m_BoatName == pOldBoat->m_BoatName)
					{
						m_poaBoatPolar->removeAt(l);
						delete pBoatPolar;
					}
				}

				//remove the old boat
				for (l=m_poaBoat->size()-1;l>=0; l--)
				{
					CBoat *pBoat = (CBoat*)m_poaBoat->at(l);
					if (pBoat->m_BoatName == dlg.m_strName)
					{
						m_poaBoat->removeAt(l);
						pModBoat->m_BoatName = pOldBoat->m_BoatName;
						m_poaBoat->insert(l, pModBoat);
						delete pOldBoat;
						break;
					}
				}
			}

			m_pCurBoatPolar = NULL;
			m_pCurBoatOpp = NULL;
			m_pCurBoat = pModBoat;

			pMainFrame->SetSaveState(false);
			return true;
		}
		else
		{
			return false;//cancelled
		}
	}
	return false ;//useless...
}



void QSail7::OnRenameCurBoatPolar()
{
	//Rename the currently selected Wing Polar

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurBoatPolar) return;

	int resp, k;
	BoatPolar* pBoatPolar = NULL;

//	BoatOpp * pBoatOpp = NULL;
	QString OldName = m_pCurBoatPolar->m_BoatPolarName;


	QStringList NameList;
	for(k=0; k<m_poaBoatPolar->size(); k++)
	{
		pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(k);
		if(pBoatPolar->m_BoatName==m_pCurBoat->m_BoatName) NameList.append(pBoatPolar->m_BoatPolarName);
	}

	RenameDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_pstrArray = & NameList;

	dlg.m_strQuestion = tr("Enter the new name for the boat polar :");
	dlg.m_strName     = m_pCurBoatPolar->m_BoatPolarName;
	dlg.InitDialog();

	bool bExists = true;

	while (bExists)
	{
		resp = dlg.exec();
		pMainFrame->m_DlgPos = dlg.pos();
		if(resp==QDialog::Accepted)
		{
			if (OldName == dlg.m_strName) return;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaBoatPolar->size(); k++)
			{
				pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(k);
				if (pBoatPolar->m_BoatPolarName == dlg.m_strName &&	pBoatPolar->m_BoatName == m_pCurBoat->m_BoatName)
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				m_pCurBoatPolar->m_BoatPolarName = dlg.m_strName;
				for (int l=m_poaBoatOpp->size()-1;l>=0; l--)
				{
					BoatOpp* pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(l);
					if (pBoatOpp->m_BoatPolarName == OldName && pBoatOpp->m_BoatName == m_pCurBoat->m_BoatName)
					{
						pBoatOpp->m_BoatPolarName = dlg.m_strName;
					}
				}
			}
			//put the BoatPlr at its new place in alphabetical order
			//so find the current polar's index
			for (k=0; k<m_poaBoatPolar->size(); k++)
				if(m_pCurBoatPolar==m_poaBoatPolar->at(k)) break;

			if(k<m_poaBoatPolar->size())//you never know
			{

				//remove the BoatPolar
				m_poaBoatPolar->removeAt(k);
				//And re-insert it
				bool bInserted = false;
				for (k=0; k<m_poaBoatPolar->size(); k++)
				{
					pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(k);
					if (m_pCurBoatPolar->m_BoatPolarName.compare(pBoatPolar->m_BoatPolarName, Qt::CaseInsensitive)<0 &&
						pBoatPolar->m_BoatName== m_pCurBoat->m_BoatName)
					{
						m_poaBoatPolar->insert(k, m_pCurBoatPolar);
						bInserted = true;
						break;
					}
				}
				if(!bInserted)
				{
					m_poaBoatPolar->append(m_pCurBoatPolar);
				}
			}

			pMainFrame->SetSaveState(false);
		}
		else if(resp ==10)
		{
			//user wants to overwrite
			if (OldName == dlg.m_strName) return;
			for (k=0; k<m_poaBoatPolar->size(); k++)
			{
				pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(k);
				if (pBoatPolar->m_BoatPolarName == dlg.m_strName &&	pBoatPolar->m_BoatName == m_pCurBoat->m_BoatName)
				{
					bExists = true;
					break;
				}
			}
			for (int l=m_poaBoatOpp->size()-1;l>=0; l--)
			{
				BoatOpp *pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(l);
				if (pBoatOpp->m_BoatPolarName == pBoatPolar->m_BoatPolarName && pBoatOpp->m_BoatName == m_pCurBoat->m_BoatName)
				{
					m_poaBoatOpp->removeAt(l);
					if(pBoatOpp==m_pCurBoatOpp)
					{
						m_pCurBoatOpp = NULL;
					}
					delete pBoatOpp;
				}
			}
			m_poaBoatPolar->removeAt(k);
			if(pBoatPolar==m_pCurBoatPolar)
			{
				m_pCurBoatPolar = NULL;
			}
			delete pBoatPolar;

			//and rename everything
			m_pCurBoatPolar->m_BoatPolarName = dlg.m_strName;

			for (int l=m_poaBoatOpp->size()-1;l>=0; l--)
			{
				BoatOpp *pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(l);
				if (pBoatOpp->m_BoatPolarName == OldName &&	pBoatOpp->m_BoatName == m_pCurBoat->m_BoatName)
				{
					pBoatOpp->m_BoatPolarName= dlg.m_strName;
				}
			}

			bExists = false;
			pMainFrame->SetSaveState(false);
		}
		else
		{
			return ;//cancelled
		}
	}
	SetBoatPolar();
	pMainFrame->UpdateBoatPolars();
	UpdateView();
}


bool QSail7::SetModBoatPolar(BoatPolar *pModBoatPolar)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!pModBoatPolar) pModBoatPolar = m_pCurBoatPolar;
	BoatPolar *pBoatPolar, *pOldBoatPolar;

	bool bExists = true;
	int resp, k, l;

	QStringList NameList;
	for(k=0; k<m_poaBoatPolar->size(); k++)
	{
		pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(k);
		if(pBoatPolar->m_BoatName==m_pCurBoat->m_BoatName) NameList.append(pBoatPolar->m_BoatPolarName);
	}

	RenameDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_pstrArray = & NameList;
	dlg.m_strQuestion = tr("Enter the new name for the Boat Polar:");
	dlg.m_strName = pModBoatPolar->m_BoatPolarName;
	dlg.InitDialog();

	while (bExists)
	{
		resp = dlg.exec();
		pMainFrame->m_DlgPos = dlg.pos();
		if(resp==QDialog::Accepted)
		{
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<NameList.count(); k++)
			{
				if(dlg.m_strName==NameList.at(k))
				{
					bExists = true;
					break;
				}
			}

			if(!bExists)
			{
				pModBoatPolar->m_BoatPolarName = dlg.m_strName;
				//replace the BoatPolar in alphabetical order in the array

				bool bInserted = false;
				for (l=0; l<m_poaBoatPolar->size();l++)
				{
					pOldBoatPolar = (BoatPolar*)m_poaBoatPolar->at(l);

					if(pOldBoatPolar->m_BoatPolarName.compare(pModBoatPolar->m_BoatPolarName, Qt::CaseInsensitive) >0)
					{
						//then insert before
						m_poaBoatPolar->insert(l, pModBoatPolar);
						bInserted = true;
						break;
					}
				}
				if(!bInserted)	m_poaBoatPolar->append(pModBoatPolar);
				pMainFrame->SetSaveState(false);
				return true;
			}
		}
		else if(resp==10)
		{
			//user wants to overwrite
			pOldBoatPolar = NULL;
			for(int ipb=0; ipb<m_poaBoatPolar->size(); ipb++)
			{
				pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(ipb);
				if(pBoatPolar->m_BoatPolarName==pModBoatPolar->m_BoatPolarName && pBoatPolar->m_BoatName==m_pCurBoat->m_BoatName)
				{
					pOldBoatPolar = pBoatPolar;
					for (l=m_poaBoatOpp->size()-1;l>=0; l--)
					{
						BoatOpp *pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(l);
						if (pBoatOpp->m_BoatName==pOldBoatPolar->m_BoatName && pBoatOpp->m_BoatPolarName==pOldBoatPolar->m_BoatPolarName)
						{
							m_poaBoatOpp->removeAt(l);
							delete pBoatOpp;
						}
					}
					m_pCurBoatOpp = NULL;

					m_poaBoatPolar->removeAt(ipb);
					delete pOldBoatPolar;
					m_poaBoatPolar->insert(ipb, pModBoatPolar);
					m_pCurBoatPolar = NULL;

					break;
				}
			}

			pModBoatPolar->m_BoatPolarName = dlg.m_strName;
			m_pCurBoatPolar = pModBoatPolar;

			pMainFrame->SetSaveState(false);
			return true;
		}
		else
		{
			return false;//cancelled
		}
	}
	return false ;//useless...
}



CBoat* QSail7::GetBoat(QString BoatName)
{
	//
	// returns a pointer to the sail with the name SailName
	//
	if(!BoatName.length()) return NULL;

	int i;
	CBoat* pBoat;
	for (i=0; i<m_poaBoat->size(); i++)
	{
		pBoat = (CBoat*)m_poaBoat->at(i);
		if (pBoat->m_BoatName == BoatName) return pBoat;
	}
	return NULL;
}


void QSail7::GLCreateBodyLists()
{
	if(m_pCurBoat)
	{
		for(int ib=0; ib<m_pCurBoat->m_poaHull.size(); ib++)
		{
			CBody *pHull = (CBody*)m_pCurBoat->m_poaHull.at(ib);
			if(pHull && m_bResetglBody)
			{
				//create GL lists associated to this hull

				if(glIsList(BODYGEOMBASE+ib))
				{
					glDeleteLists(BODYGEOMBASE+ib,1);
					glDeleteLists(BODYGEOMBASE+ib+MAXBODIES,1);
					m_GLList -=2;
				}
				if(pHull->m_LineType==BODYPANELTYPE)	   GLCreateBody3DFlatPanels(s_pMainFrame, BODYGEOMBASE+ib, pHull);
				else if(pHull->m_LineType==BODYSPLINETYPE) GLCreateBody3DSplines(s_pMainFrame, BODYGEOMBASE+ib, pHull,
																				 GL3dBodyDlg::s_NXPoints,
																				 GL3dBodyDlg::s_NHoopPoints);
			}
		}
	}
	m_bResetglBody = false;
}


void QSail7::GLCreateSailLists()
{
	if(!m_pCurBoat) return;//nothing to draw

	if(m_bResetglSailGeom)
	{
		for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
		{
			QSail *pSail = (QSail*)m_pCurBoat->m_poaSail.at(is);

			//create the list for the sail surface
			if(glIsList(is+SAILGEOMBASE))
			{
				glDeleteLists(is+SAILGEOMBASE,1);
				m_GLList--;
			}
			glNewList(is+SAILGEOMBASE, GL_COMPILE);
			{
				GLCreateSailGeom(SAILGEOMBASE+is, pSail, pSail->m_LEPosition);
			}
			glEndList();

		}
		m_bResetglSailGeom = false;
	}
}



void QSail7::GLCallViewLists()
{
	if(!m_pCurBoat) return;//Nothing to call

	if(m_pCurBoatOpp)
	{
		glRotated(m_pCurBoatOpp->m_Phi, 1.0, 0.0, 00);
	}


	for(int ib=0; ib<m_pCurBoat->m_poaHull.size(); ib++)
	{
		CBody *pHull = (CBody*)m_pCurBoat->m_poaHull.at(ib);
		if(m_pCurBoatOpp)
		{
			glPushMatrix();
			{
				glTranslated(pHull->m_LEPosition.x, pHull->m_LEPosition.y, pHull->m_LEPosition.z);

				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);
				if(m_bOutline)
				{
					if(glIsList(BODYGEOMBASE+MAXBODIES+ib)) glCallList(BODYGEOMBASE+MAXBODIES+ib);
				}

				if(m_bglLight)
				{
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT0);
				}

				if(m_bSurfaces)
				{
					if(glIsList(BODYGEOMBASE+ib)) glCallList(BODYGEOMBASE+ib);
				}
			}
			glPopMatrix();

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			if(m_b3DCp)       glCallList(BODYCPBASE+ib);
			if(m_bPanelForce) glCallList(BODYFORCELISTBASE+ib);
		}
		else
		{
			glPushMatrix();
			{
				glTranslated(pHull->m_LEPosition.x, pHull->m_LEPosition.y, pHull->m_LEPosition.z);
				if(m_bglLight)
				{
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT0);
				}

				if(m_bSurfaces)
				{
					if(glIsList(BODYGEOMBASE+ib)) glCallList(BODYGEOMBASE+ib);
				}

				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);
				if(m_bOutline)
				{
					if(glIsList(BODYGEOMBASE+MAXBODIES+ib)) glCallList(BODYGEOMBASE+MAXBODIES+ib);
				}
			}
			glPopMatrix();
		}
		if(m_bVLMPanels)
		{
			glCallList(BODYMESHBASE+ib);
			if(!m_b3DCp && !m_bSurfaces) glCallList(BODYMESHBASE+MAXBODIES+ib);
		}
	}

	//Call sail lists first
	for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
	{
		if(m_pCurBoatOpp)
		{
			QSail *pSail = (QSail*)m_pCurBoat->m_poaSail.at(is);
			CVector LE = pSail->m_LEPosition;
//			LE.RotateX(m_pCurBoatOpp->m_Phi);

			glPushMatrix();
			{
				glTranslated( LE.x,  LE.y,  LE.z);
				glRotated(-m_pCurBoatOpp->m_SailAngle[is], sin(pSail->m_LuffAngle*PI/180.0),0.0, cos(pSail->m_LuffAngle*PI/180.0));
				glTranslated(-LE.x, -LE.y, -LE.z);

				if(m_bglLight)
				{
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT0);
				}

				if(m_bSurfaces)
				{
					if(glIsList(SAILGEOMBASE+is)) glCallList(SAILGEOMBASE+is);
				}

				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);

				if(m_bOutline)
				{
					if(glIsList(SAILGEOMBASE+MAXSAILS+is)) glCallList(SAILGEOMBASE+MAXSAILS+is);
				}

				if(m_bVLMPanels)
				{
					glCallList(SAILMESHBASE+is);
//					if(!m_b3DCp) glCallList(SAILMESHBASE+MAXSAILS+is);
				}

				if(m_b3DCp) glCallList(SAILCPBASE+is);

				if(m_bPanelForce) glCallList(SAILFORCELISTBASE+is);
			}
			glPopMatrix();
		}
		else
		{
			if(m_bglLight)
			{
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
			}

			if(m_bSurfaces)
			{
				if(glIsList(SAILGEOMBASE+is)) glCallList(SAILGEOMBASE+is);
			}

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			if(m_bOutline)
			{
				if(glIsList(SAILGEOMBASE+MAXSAILS+is)) glCallList(SAILGEOMBASE+MAXSAILS+is);
			}
			if(m_bVLMPanels)
			{
				glCallList(SAILMESHBASE+is);
//				glCallList(SAILMESHBASE+MAXSAILS+is);
			}
		}
	}
}



void QSail7::LoadSettings(QSettings *pSettings)
{
	//
	// Loads the user's saved settings for 3D analysis
	//
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strange;
	pSettings->beginGroup("Sail7");
	{
		m_iView         = pSettings->value("iView", SAIL3DVIEW).toInt();
		m_iBoatPlrView  = pSettings->value("iBoatView", 1).toInt();
		m_bStoreOpp     = pSettings->value("StoreOpp", false).toBool();
		m_bSequence     = pSettings->value("Sequence", false).toBool();
		m_ControlMin    = pSettings->value("ControlMin", 0.0).toDouble();
		m_ControlMax    = pSettings->value("ControlMax", 1.0).toDouble();
		m_ControlDelta  = pSettings->value("ControlDelta", 0.25).toDouble();

		m_bWater         = pSettings->value("bWater", false).toBool();
		m_bWindDirection = pSettings->value("bWindDir", false).toBool();
		m_bLift          = pSettings->value("bLift", false).toBool();
		m_bBodyForces    = pSettings->value("bBodyForces", false).toBool();
		m_bPanelForce    = pSettings->value("bPanelForce", true).toBool();
		m_bICd           = pSettings->value("bICd", true).toBool();
		m_bSurfaces      = pSettings->value("bSurfaces", true).toBool();
		m_bOutline       = pSettings->value("bOutline", true).toBool();
		m_bVLMPanels     = pSettings->value("bVLMPanels", false).toBool();
		m_bAxes          = pSettings->value("bAxes", true).toBool();
		m_b3DCp          = pSettings->value("b3DCp", false).toBool();
		m_bDownwash      = pSettings->value("bDownwash", false).toBool();
		m_bMoments       = pSettings->value("bMoments", false).toBool();
		m_bglLight       = pSettings->value("bLight", true).toBool();

		SailViewWidget::s_bAxes      = m_bAxes;
		SailViewWidget::s_bOutline   = m_bOutline;
		SailViewWidget::s_bSurfaces  = m_bSurfaces;
		SailViewWidget::s_bVLMPanels = m_bVLMPanels;
		GL3dBodyDlg::s_bAxes      = m_bAxes;
		GL3dBodyDlg::s_bOutline   = m_bOutline;
		GL3dBodyDlg::s_bSurfaces  = m_bSurfaces;
		GL3dBodyDlg::s_bVLMPanels = m_bVLMPanels;

		BoatPolarDlg::s_BoatPolar.m_BetaMin     = pSettings->value("BetaMin",  0.0).toDouble();
		BoatPolarDlg::s_BoatPolar.m_BetaMax     = pSettings->value("BetaMax", 10.0).toDouble();
		BoatPolarDlg::s_BoatPolar.m_PhiMin      = pSettings->value("PhiMin", 0.0).toDouble();
		BoatPolarDlg::s_BoatPolar.m_PhiMax      = pSettings->value("PhiMax", 0.0).toDouble();
		BoatPolarDlg::s_BoatPolar.m_QInfMax     = pSettings->value("QInfMin", 10.0).toDouble();
		BoatPolarDlg::s_BoatPolar.m_QInfMax     = pSettings->value("QInfMax", 10.0).toDouble();
		BoatPolarDlg::s_BoatPolar.m_Viscosity   = pSettings->value("Viscosity", 1.5e-5).toDouble();
		BoatPolarDlg::s_BoatPolar.m_Density     = pSettings->value("Density", 1.225).toDouble();
		BoatPolarDlg::s_BoatPolar.m_bGround     = pSettings->value("bGround", false).toBool();
		BoatPolarDlg::s_BoatPolar.m_CoG.x       = pSettings->value("CoGx", 0.0).toDouble();
		BoatPolarDlg::s_BoatPolar.m_CoG.z       = pSettings->value("CoGz", 0.0).toDouble();
		BoatPolarDlg::s_UnitType    = pSettings->value("UnitType", 1).toInt();
		for(int is=0; is<MAXSAILS; is++)
		{
			strange = QString("SailAngleMin%1").arg(is);
			BoatPolarDlg::s_BoatPolar.m_SailAngleMin[is] = pSettings->value(strange, 0.0).toDouble();
			strange = QString("SailAngleMax%1").arg(is);
			BoatPolarDlg::s_BoatPolar.m_SailAngleMax[is] = pSettings->value(strange, 0.0).toDouble();
		}

		for(int ig=0; ig<4; ig++)
		{
			m_BoatGraph[ig].LoadSettings(pSettings);
			SetBoatPolarGraphTitles(m_BoatGraph+ig);
		}

		SailDlg::s_bWindowMaximized = pSettings->value("SailDlgMaximized", false).toBool();
		SailDlg::s_WindowSize = pSettings->value("SailDlgWindowSize", QSize(300, 400)).toSize();
		SailDlg::s_WindowPos = pSettings->value("SailDlgWindowPos",  QPoint(20,30)).toPoint();
	}
	pSettings->endGroup();

	m_3DPrefsDlg.LoadSettings(pSettings);
	m_GLLightDlg.LoadSettings(pSettings);
	SailDlg::LoadSettings(pSettings);
}


void QSail7::SaveSettings(QSettings *pSettings)
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strange;
	OnReadAnalysisData();

	pSettings->beginGroup("Sail7");
	{
		pSettings->setValue("iView", m_iView);
		pSettings->setValue("iBoatView", m_iBoatPlrView);
		pSettings->setValue("StoreOpp", m_bStoreOpp);
		pSettings->setValue("Sequence", m_bSequence );
		pSettings->setValue("ControlMin", m_ControlMin );
		pSettings->setValue("ControlMax", m_ControlMax );
		pSettings->setValue("ControlDelta", m_ControlDelta );
		pSettings->setValue("bWater", m_bWater);
		pSettings->setValue("bWindDir", m_bWindDirection);
		pSettings->setValue("bLift", m_bLift);
		pSettings->setValue("bBodyForces", m_bBodyForces);
		pSettings->setValue("bPanelForce", m_bPanelForce);
		pSettings->setValue("bICd", m_bICd);
		pSettings->setValue("bSurfaces", m_bSurfaces);
		pSettings->setValue("bOutline", m_bOutline);
		pSettings->setValue("bVLMPanels", m_bVLMPanels);
		pSettings->setValue("bAxes", m_bAxes);
		pSettings->setValue("b3DCp", m_b3DCp);
		pSettings->setValue("bDownwash", m_bDownwash);
		pSettings->setValue("bMoments", m_bMoments);
		pSettings->setValue("bLight", m_bglLight);

		pSettings->setValue("BetaMin", BoatPolarDlg::s_BoatPolar.m_BetaMin);
		pSettings->setValue("BetaMax", BoatPolarDlg::s_BoatPolar.m_BetaMax);
		pSettings->setValue("PhiMin", BoatPolarDlg::s_BoatPolar.m_PhiMin);
		pSettings->setValue("PhiMax", BoatPolarDlg::s_BoatPolar.m_PhiMax);
		pSettings->setValue("QInfMin", BoatPolarDlg::s_BoatPolar.m_QInfMax);
		pSettings->setValue("QInfMax", BoatPolarDlg::s_BoatPolar.m_QInfMax);
		pSettings->setValue("Viscosity", BoatPolarDlg::s_BoatPolar.m_Viscosity);
		pSettings->setValue("Density", BoatPolarDlg::s_BoatPolar.m_Density);
		pSettings->setValue("bGround", BoatPolarDlg::s_BoatPolar.m_bGround);
		pSettings->setValue("CoGx", BoatPolarDlg::s_BoatPolar.m_CoG.x);
		pSettings->setValue("CoGz", BoatPolarDlg::s_BoatPolar.m_CoG.z);
		pSettings->setValue("UnitType", BoatPolarDlg::s_UnitType);
		for(int is=0; is<MAXSAILS; is++)
		{
			strange = QString("SailAngleMin%1").arg(is);
			pSettings->setValue(strange, BoatPolarDlg::s_BoatPolar.m_SailAngleMin[is]);
			strange = QString("SailAngleMax%1").arg(is);
			pSettings->setValue(strange, BoatPolarDlg::s_BoatPolar.m_SailAngleMax[is]);
		}

		for(int ig=0; ig<4; ig++)
		{
			m_BoatGraph[ig].SaveSettings(pSettings);
		}

		pSettings->setValue("SailDlgMaximized",  SailDlg::s_bWindowMaximized);
		pSettings->setValue("SailDlgWindowSize", SailDlg::s_WindowSize);
		pSettings->setValue("SailDlgWindowPos",  SailDlg::s_WindowPos);
	}
	pSettings->endGroup();

	m_3DPrefsDlg.SaveSettings(pSettings);
	m_GLLightDlg.SaveSettings(pSettings);
	SailDlg::SaveSettings(pSettings);
}


int QSail7::IsNode(CVector &Pt)
{
	//
	// returns the index of a node if found, else returns NULL
	//
	int in;
//	for (in=0; in<m_nNodes; in++)
	// explore in reverse order, since we have better chance of
	// finding the node close to the last point when creating elements
	//
	for (in=m_nNodes-1; in>=0; in--)
	{
		if(Pt.IsSame(s_pNode[in])) return in;
	}
	return -1;
}



void QSail7::GLCreateSailMesh(CVector *pNode, CPanel *pPanel)
{
	if(!m_pCurBoat) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QColor color;
	int iLA, iLB, iTA, iTB;
	int p;
	CVector  N;
	N.Set(0.0, 0.0, 0.0);

	for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
	{
		// one list for each sail so that we can rotate the sail panels around the mast axis
		QSail *pSail = m_pCurBoat->m_poaSail.at(is);

		if(glIsList(SAILMESHBASE+is)) glDeleteLists(SAILMESHBASE+is, 1);
		glNewList(SAILMESHBASE+is,GL_COMPILE);
		{
			m_GLList++;
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//			glEnable(GL_POLYGON_OFFSET_FILL);
//			glPolygonOffset(1.0, 1.0);

			color = W3dPrefsDlg::s_VLMColor;
//			style = W3dPrefsDlg::s_VLMStyle;
//			width = W3dPrefsDlg::s_VLMWidth;

			glLineWidth(1.0);

			glColor3d(color.redF(),color.greenF(),color.blueF());

			for (p=0; p<pSail->m_NElements; p++)
			{
				glBegin(GL_QUADS);
				{
					iLA = pSail->m_pPanel[p].m_iLA;
					iLB = pSail->m_pPanel[p].m_iLB;
					iTA = pSail->m_pPanel[p].m_iTA;
					iTB = pSail->m_pPanel[p].m_iTB;

					glNormal3d(pPanel[p].Normal.x, pPanel[p].Normal.y, pPanel[p].Normal.z);
					glVertex3d(pNode[iLA].x, pNode[iLA].y, pNode[iLA].z);
					glVertex3d(pNode[iTA].x, pNode[iTA].y, pNode[iTA].z);
					glVertex3d(pNode[iTB].x, pNode[iTB].y, pNode[iTB].z);
					glVertex3d(pNode[iLB].x, pNode[iLB].y, pNode[iLB].z);
				}
				glEnd();
			}
		}
		glEndList();

		if(glIsList(SAILMESHBASE+MAXSAILS+is)) glDeleteLists(SAILMESHBASE+MAXSAILS+is, 1);
		glNewList(SAILMESHBASE+MAXSAILS+is,GL_COMPILE);
		{
			m_GLList++;
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);

			color = pMainFrame->m_BackgroundColor;
//			style = W3dPrefsDlg::s_VLMStyle;
//			width = W3dPrefsDlg::s_VLMWidth;

			glColor3d(color.redF(),color.greenF(),color.blueF());

			glLineWidth(1.0);
			glDisable (GL_LINE_STIPPLE);

			for (p=0; p<pSail->m_NElements; p++)
			{
					glBegin(GL_QUADS);
					{
						iLA = pSail->m_pPanel[p].m_iLA;
						iLB = pSail->m_pPanel[p].m_iLB;
						iTA = pSail->m_pPanel[p].m_iTA;
						iTB = pSail->m_pPanel[p].m_iTB;

						glVertex3d(pNode[iLA].x, pNode[iLA].y, pNode[iLA].z);
						glVertex3d(pNode[iTA].x, pNode[iTA].y, pNode[iTA].z);
						glVertex3d(pNode[iTB].x, pNode[iTB].y, pNode[iTB].z);
						glVertex3d(pNode[iLB].x, pNode[iLB].y, pNode[iLB].z);
						glNormal3d(pPanel[p].Normal.x, pPanel[p].Normal.y, pPanel[p].Normal.z);
					}
					glEnd();
			}

			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		glEndList();
	}
}



void QSail7::GLCreateBodyMesh(CVector *pNode, CPanel *pPanel)
{
	if(!m_pCurBoat) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QColor color;
	int iLA, iLB, iTA, iTB;
	int p;
	CVector  N;
	N.Set(0.0, 0.0, 0.0);

	for(int ib=0; ib<m_pCurBoat->m_poaHull.size(); ib++)
	{
		// one list for each Hull so that we can rotate the Hull panels around the mast axis
		CBody *pHull = m_pCurBoat->m_poaHull.at(ib);

		if(glIsList(BODYMESHBASE+ib)) glDeleteLists(BODYMESHBASE+ib, 1);
		glNewList(BODYMESHBASE+ib,GL_COMPILE);
		{
			m_GLList++;
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//			glEnable(GL_POLYGON_OFFSET_FILL);
//			glPolygonOffset(1.0, 1.0);

			color = W3dPrefsDlg::s_VLMColor;
//			style = W3dPrefsDlg::s_VLMStyle;
//			width = W3dPrefsDlg::s_VLMWidth;

			glLineWidth(1.0);

			glColor3d(color.redF(),color.greenF(),color.blueF());

			for (p=0; p<pHull->m_NElements; p++)
			{
				glBegin(GL_QUADS);
				{
					iLA = pHull->m_pPanel[p].m_iLA;
					iLB = pHull->m_pPanel[p].m_iLB;
					iTA = pHull->m_pPanel[p].m_iTA;
					iTB = pHull->m_pPanel[p].m_iTB;

					glNormal3d(pPanel[p].Normal.x, pPanel[p].Normal.y, pPanel[p].Normal.z);
					glVertex3d(pNode[iLA].x, pNode[iLA].y, pNode[iLA].z);
					glVertex3d(pNode[iTA].x, pNode[iTA].y, pNode[iTA].z);
					glVertex3d(pNode[iTB].x, pNode[iTB].y, pNode[iTB].z);
					glVertex3d(pNode[iLB].x, pNode[iLB].y, pNode[iLB].z);
				}
				glEnd();
			}
		}
		glEndList();

		if(glIsList(BODYMESHBASE+MAXBODIES+ib)) glDeleteLists(BODYMESHBASE+MAXBODIES+ib, 1);
		glNewList(BODYMESHBASE+MAXBODIES+ib,GL_COMPILE);
		{
			m_GLList++;
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);

			color = pMainFrame->m_BackgroundColor;
//			style = W3dPrefsDlg::s_VLMStyle;
//			width = W3dPrefsDlg::s_VLMWidth;

			glColor3d(color.redF(),color.greenF(),color.blueF());

			glLineWidth(1.0);
			glDisable (GL_LINE_STIPPLE);

			for (p=0; p<pHull->m_NElements; p++)
			{
					glBegin(GL_QUADS);
					{
						iLA = pHull->m_pPanel[p].m_iLA;
						iLB = pHull->m_pPanel[p].m_iLB;
						iTA = pHull->m_pPanel[p].m_iTA;
						iTB = pHull->m_pPanel[p].m_iTB;

						glVertex3d(pNode[iLA].x, pNode[iLA].y, pNode[iLA].z);
						glVertex3d(pNode[iTA].x, pNode[iTA].y, pNode[iTA].z);
						glVertex3d(pNode[iTB].x, pNode[iTB].y, pNode[iTB].z);
						glVertex3d(pNode[iLB].x, pNode[iLB].y, pNode[iLB].z);
						glNormal3d(pPanel[p].Normal.x, pPanel[p].Normal.y, pPanel[p].Normal.z);
					}
					glEnd();
			}

			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		glEndList();
	}
}




void QSail7::GLCreateVortices()
{
	int p;
	CVector A, B, C, D, AC, BD;
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0xFFFF);

	CPanel *pPanel = s_pPanel;

	glNewList(VLMVORTICES,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth(1.0);
		glColor3d(0.7,0.0,0.0);
		for (p=0; p<m_MatSize; p++)
		{
			if(!pPanel[p].m_bIsTrailing)
			{
				if(pPanel[p].m_Pos<=MIDSURFACE)
				{
					A = pPanel[p].VA;
					B = pPanel[p].VB;
					C = pPanel[p-1].VB;
					D = pPanel[p-1].VA;
				}
				else
				{
					A = pPanel[p].VA;
					B = pPanel[p].VB;
					C = pPanel[p+1].VB;
					D = pPanel[p+1].VA;
				}
			}
			else
			{
				A = pPanel[p].VA;
				B = pPanel[p].VB;
				// we define point AA=A+1 and BB=B+1
				C.x =  s_pNode[pPanel[p].m_iTB].x
					+ (s_pNode[pPanel[p].m_iTB].x-pPanel[p].VB.x)/3.0;
				C.y =  s_pNode[pPanel[p].m_iTB].y;
				C.z =  s_pNode[pPanel[p].m_iTB].z;
				D.x =  s_pNode[pPanel[p].m_iTA].x
					+ (s_pNode[pPanel[p].m_iTA].x-pPanel[p].VA.x)/3.0;
				D.y =  s_pNode[pPanel[p].m_iTA].y;
				D.z =  s_pNode[pPanel[p].m_iTA].z;
			}

			//next we "shrink" the points to avoid confusion with panels sides
			AC = C-A;
			BD = D-B;

			AC *= 0.03;
			A  += AC;
			C  -= AC;
			BD *= 0.03;
			B  += BD;
			D  -= BD;

			if(m_pCurBoatPolar && m_pCurBoatPolar->m_bVLM1)
			{
				glLineStipple (1, 0xFFFF);
				glBegin(GL_LINES);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
				}
				glEnd();
				glLineStipple (1, 0x0F0F);
				glBegin(GL_LINES);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
				}
				glEnd();
			}
			else if(!m_pCurBoatPolar || (m_pCurBoatPolar && !m_pCurBoatPolar->m_bVLM1))
			{
				glBegin(GL_LINE_STRIP);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
					glVertex3d(D.x, D.y, D.z);
					glVertex3d(A.x, A.y, A.z);
				}
				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}





void QSail7::GLCreatePanelNormals()
{
	int p;
	CVector C;
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0xFFFF);

	CPanel *pPanel = s_pPanel;

	glNewList(NORMALLIST,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth(1.0);
		glColor3d(0.7,0.0,0.0);

		for (p=0; p<m_MatSize; p++)
		{
			if(pPanel->m_Pos==MIDSURFACE) C = pPanel[p].CtrlPt;
			else                          C = pPanel[p].CollPt;

			glBegin(GL_LINES);
			{
				glVertex3d(C.x, C.y, C.z);
				glVertex3d(C.x+pPanel[p].Normal.x, C.y+pPanel[p].Normal.y, C.z+pPanel[p].Normal.z);
			}
			glEnd();

		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void QSail7::OnAxes()
{
	m_bAxes = m_pctrlAxes->isChecked();
	GL3dBodyDlg::s_bAxes = m_bAxes;
	SailViewWidget::s_bAxes= m_bAxes;

	UpdateView();
}


void QSail7::OnSurfaces()
{
	m_bSurfaces = m_pctrlSurfaces->isChecked();
	GL3dBodyDlg::s_bSurfaces = m_bSurfaces;
	SailViewWidget::s_bSurfaces = m_bSurfaces;

	if(m_bSurfaces)
	{
		m_b3DCp = false;
		m_pctrlCp->setChecked(false);
//		m_bVLMPanels=false;
//		m_pctrlPanels->setChecked(false);
	}
	UpdateView();
}


void QSail7::OnOutline()
{
	m_bOutline = m_pctrlOutline->isChecked();
	GL3dBodyDlg::s_bOutline = m_bOutline;
	SailViewWidget::s_bOutline = m_bOutline;
	UpdateView();
}


void QSail7::OnPanels()
{
	m_bVLMPanels = m_pctrlPanels->isChecked();
	GL3dBodyDlg::s_bVLMPanels = m_bVLMPanels;
	SailViewWidget::s_bVLMPanels = m_bVLMPanels;
	if(m_bVLMPanels)
	{
//		m_bSurfaces=false;
//		m_pctrlSurfaces->setChecked(false);
	}
	UpdateView();
}




void QSail7::On3DPickCenter()
{
	//
	// The user has toggled the switch for a pick of the rotation center in 3D view
	//
	m_bPickCenter = m_pctrlPickCenter->isChecked();
}


void QSail7::OnDefineBoatPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	BoatPolarDlg PlrDlg;

	if(!m_pCurBoat) return;

	if(!m_pCurBoat->m_poaSail.size())
	{
		QString strong = tr("Please add at least one sail to the boat definition before defining a polar");

		QMessageBox::warning(pMainFrame, tr("Question"), strong);
		return;
	}

	PlrDlg.InitDialog(m_pCurBoat);
	PlrDlg.move(pMainFrame->m_DlgPos);
	if(PlrDlg.exec()==QDialog::Accepted)
	{
		pMainFrame->SetSaveState(false);
		//Then create and add a BoatPolar to the array
		BoatPolar *pNewBoatPolar = new BoatPolar;

		pNewBoatPolar->m_BoatName   = m_pCurBoat->m_BoatName;
		pNewBoatPolar->m_BoatPolarName   = PlrDlg.m_BoatPolarName;
		pNewBoatPolar->DuplicateSpec(&BoatPolarDlg::s_BoatPolar);
		pNewBoatPolar->m_Color = pMainFrame->GetColor(0);
		pNewBoatPolar->m_bIsVisible = true;

		m_pCurBoatPolar = AddBoatPolar(pNewBoatPolar);
		SetBoatPolar(pNewBoatPolar);
		pMainFrame->UpdateBoatPolars();

		m_pCurBoatOpp = NULL;
		m_bResetglSailGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
		UpdateView();

		SetControls();
		m_pctrlAnalyze->setFocus();
	}

	pMainFrame->m_DlgPos = PlrDlg.pos();
}



void QSail7::OnEditBoatPolar()
{
	if(!m_pCurBoat || !m_pCurBoatPolar) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	BoatPolarDlg PlrDlg;
	PlrDlg.InitDialog(m_pCurBoat, m_pCurBoatPolar);
	PlrDlg.move(pMainFrame->m_DlgPos);

	if(PlrDlg.exec()==QDialog::Accepted)
	{
		pMainFrame->SetSaveState(false);
		for(int iopp=m_poaBoatOpp->size()-1; iopp>=0; iopp--)
		{
			BoatOpp *pBOpp = (BoatOpp*)m_poaBoatOpp->at(iopp);
			if(pBOpp->m_BoatName==m_pCurBoat->m_BoatName && pBOpp->m_BoatPolarName==m_pCurBoatPolar->m_BoatPolarName)
			{
				delete pBOpp;
				m_poaBoatOpp->removeAt(iopp);
			}
		}
		m_pCurBoatOpp = NULL;

		BoatPolar *pNewBoatPolar = new BoatPolar;
		pNewBoatPolar->m_BoatName        = m_pCurBoatPolar->m_BoatName;
		pNewBoatPolar->m_BoatPolarName   = m_pCurBoatPolar->m_BoatPolarName;
		pNewBoatPolar->DuplicateSpec(&BoatPolarDlg::s_BoatPolar);
		pNewBoatPolar->ResetBoatPlr();
		pNewBoatPolar->m_Color = pMainFrame->GetColor(0);
		SetModBoatPolar(pNewBoatPolar);
		SetBoatPolar(pNewBoatPolar);
		pMainFrame->UpdateBoatPolars();


		CreateBoatPolarCurves();
		m_bResetglSailGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
		UpdateView();

		SetControls();
		m_pctrlAnalyze->setFocus();
	}

	pMainFrame->m_DlgPos = PlrDlg.pos();
}


BoatPolar* QSail7::AddBoatPolar(BoatPolar *pBoatPolar)
{
	//
	// Adds the BoatPolar pointed by pBoatPolar to the m_oaBoatPolar array
	// Inserts it in alphabetical order
	//
	int i,j;
	bool bExists   = false;
	bool bInserted = false;
	BoatPolar *pOldBoatPlr;

	for (i=0; i<m_poaBoatPolar->size(); i++)
	{
		pOldBoatPlr = (BoatPolar*)m_poaBoatPolar->at(i);
		if (pOldBoatPlr->m_BoatPolarName == pBoatPolar->m_BoatPolarName && pOldBoatPlr->m_BoatName == pBoatPolar->m_BoatName)
		{
			bExists = true;
			break;
		}
	}

	if(!bExists)
	{
		//if it doesn't exist, find its place in alphabetical order and insert it
		for (j=0; j<m_poaBoatPolar->size(); j++)
		{
			pOldBoatPlr = (BoatPolar*)m_poaBoatPolar->at(j);
			if(pBoatPolar->m_BoatName.compare(pOldBoatPlr->m_BoatName, Qt::CaseInsensitive)<0)
			{
				m_poaBoatPolar->insert(j, pBoatPolar);
				bInserted = true;
				break;
			}
			else if (pBoatPolar->m_BoatName == pOldBoatPlr->m_BoatName)
			{
				//first sort by Type
				if(pBoatPolar->m_BoatPolarName.compare(pOldBoatPlr->m_BoatPolarName, Qt::CaseInsensitive)<0)
				{
					m_poaBoatPolar->insert(j, pBoatPolar);
					bInserted = true;
					break;
				}
			}
		}
		if(!bInserted)
		{
			m_poaBoatPolar->append(pBoatPolar);
			bInserted = true;
		}
		return pBoatPolar;
	}
	else
	{
		SetModBoatPolar(pBoatPolar);
	}

	return NULL;
}


BoatPolar* QSail7::GetBoatPolar(QString BoatPolarName)
{
	//
	// returns a pointer to the WPolar with name WPolarName
	// or returns NULL if non with that name for the current UFO
	//
	BoatPolar *pBoatPolar;
	int i;
	if(!m_pCurBoat) return NULL;
	if(!BoatPolarName.length())

	for (i=0; i<m_poaBoatPolar->size(); i++)
	{
		pBoatPolar = (BoatPolar*) m_poaBoatPolar->at(i);
		if (pBoatPolar->m_BoatName == m_pCurBoat->m_BoatName && pBoatPolar->m_BoatPolarName == BoatPolarName) return pBoatPolar;
	}
	return NULL;
}


void QSail7::SetBoatPolar(BoatPolar *pBoatPolar, QString BoatPolarName)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	BoatPolar *pOldBoatPolar = NULL;
	int i;

	if(!m_pCurBoat) return;

	if(!BoatPolarName.length() && m_pCurBoatPolar)	BoatPolarName = m_pCurBoatPolar->m_BoatPolarName;

	if(!pBoatPolar && BoatPolarName.length())
	{
		for (i=0; i<m_poaBoatPolar->size(); i++)
		{
			pOldBoatPolar = (BoatPolar*) m_poaBoatPolar->at(i);
			if (pOldBoatPolar->m_BoatName == m_pCurBoat->m_BoatName &&	pOldBoatPolar->m_BoatPolarName == BoatPolarName)
			{
				pBoatPolar = pOldBoatPolar;
				break;
			}
		}
	}
	else if(!pBoatPolar) pBoatPolar = m_pCurBoatPolar;

	if(pBoatPolar && pBoatPolar == m_pCurBoatPolar) SetBoatOpp(true);
	else
	{
		m_bResetglMesh = true;
		if(!m_pCurBoat)
		{
			pBoatPolar = NULL;
			m_pCurBoatPolar = NULL;
		}

		if(pBoatPolar) m_pCurBoatPolar = pBoatPolar;
		else if(m_pCurBoat)
		{
			// set the first one in the array, if any
			m_pCurBoatPolar = NULL;
			for(i=0; i< m_poaBoatPolar->size(); i++)
			{
				pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(i);
				if(pBoatPolar && pBoatPolar->m_BoatName==m_pCurBoat->m_BoatName)
				{
					m_pCurBoatPolar = pBoatPolar;
					break;
				}
			}
		}
	}

	if(m_pCurBoatPolar)
	{
		int pos = pMainFrame->m_pctrlBoatPolar->findText(m_pCurBoatPolar->m_BoatPolarName);
		if (pos>=0)		pMainFrame->m_pctrlBoatPolar->setCurrentIndex(pos);
	}


	if(m_pCurBoatPolar && m_pCurBoatPolar->m_BoatName==m_pCurBoat->m_BoatName)
	{
		pMainFrame->UpdateBoatOpps();

		double Ctrl = 0.0;
		if(m_pCurBoatOpp) Ctrl= m_pCurBoatOpp->m_Ctrl;

		SetBoatOpp(false, Ctrl);
	}
	else
	{
		m_pCurBoatPolar = NULL;
		m_pCurBoatOpp = NULL;
		SetBoatOpp(false);
	}

	if(m_pCurBoatPolar)
	{
		QString PolarProps;
		m_bCurveVisible = m_pCurBoatPolar->m_bIsVisible;
		m_bCurvePoints  = m_pCurBoatPolar->m_bShowPoints;
		m_pCurBoatPolar->GetPolarProperties(PolarProps);
		m_pctrlPolarProps1->setText(PolarProps);
	}
	else m_pctrlPolarProps1->clear();


	if(!m_pCurBoatPolar || !m_pCurBoatPolar->m_bGround)
	{
		m_bWater = false;
		m_pctrlWater->setChecked(false);
	}

	if(m_iView==SAILPOLARVIEW)	CreateBoatPolarCurves();


	pMainFrame->UpdateBoatOpps();
	SetAnalysisParams();
	SetCurveParams();
	m_bResetglLegend = true;


	m_PanelDlg.m_pBoatPolar  = m_pCurBoatPolar;
	m_PanelDlg.m_MatSize     = m_MatSize;
	m_PanelDlg.m_WakeSize    = m_WakeSize;
	m_PanelDlg.m_nNodes      = m_nNodes;

	QApplication::restoreOverrideCursor();
}


bool QSail7::SetBoatOpp(bool bCurrent, double x)
{
	m_bResetglMesh   = true;
	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglSpeeds = true;
	m_bResetglFlow   = true;
	m_bResetglLegend = true;
	m_bResetglLift   = true;
	m_bResetglCPForces = true;

	// first restore the panel geometry
	memcpy(s_pPanel, s_pMemPanel, m_MatSize* sizeof(CPanel));
	memcpy(s_pNode,  s_pMemNode,  m_nNodes * sizeof(CVector));

	if(!m_pCurBoat|| !m_pCurBoatPolar)
	{
		m_pCurBoatOpp = NULL;
		SetCurveParams();
		return false;
	}

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	// set set the current BoatOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL

	BoatOpp *pBoatOpp = NULL;
	if(bCurrent)
	{
		pBoatOpp = m_pCurBoatOpp;
		if(pBoatOpp)
		{
			//Check it is consistent with wing and polar
			if(pBoatOpp->m_BoatName != m_pCurBoat->m_BoatName || pBoatOpp->m_BoatPolarName!=m_pCurBoatPolar->m_BoatPolarName) pBoatOpp=NULL;
		}
	}
	else pBoatOpp = GetBoatOpp(x);

	if(!pBoatOpp)
	{
	//first try to set the last alpha which has been selected
		if(m_pCurBoatPolar)
			pBoatOpp = GetBoatOpp(m_pCurBoatPolar->m_AMem);
		else
			pBoatOpp = GetBoatOpp(m_LastBoatOpp);
	}


	if(!pBoatOpp)
	{
		m_pCurBoatOpp = NULL;
		//try to select the first in the ListBox
		for(int ibo=0; ibo<m_poaBoatOpp->size(); ibo++)
		{
			pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(ibo);
			if(pBoatOpp->m_BoatName==m_pCurBoat->m_BoatName && pBoatOpp->m_BoatPolarName==m_pCurBoatPolar->m_BoatPolarName)
			{
				pMainFrame->SelectBoatOpp(pBoatOpp->m_Ctrl);
				m_pCurBoatOpp = pBoatOpp;
				break;
			}
		}
	}
	else m_pCurBoatOpp = pBoatOpp;

	SetCurveParams();
	if(m_pCurBoatOpp)
	{
		m_LastBoatOpp = m_pCurBoatOpp->m_Ctrl;
		m_pCurBoatPolar->m_AMem = m_pCurBoatOpp->m_Ctrl;
//		m_nWakeNodes   = m_pCurBoatOpp->m_nWakeNodes;
//		m_NXWakePanels = m_pCurBoatOpp->m_NXWam_AlphakePanels;
//		m_TotalWakeLength  =		m_pCurBoatOpp->m_FirstWakePanel;
//		m_WakePanelFactor =		m_pCurBoatOpp->m_WakeFactor;

		//select m_pCurBoatOpp in the listbox
		pMainFrame->SelectBoatOpp(m_pCurBoatOpp->m_Ctrl);
	}

	if(!m_pCurBoatOpp) return false;
	else if(m_iView==SAIL3DVIEW)
	{
		if(m_bWindFront)     On3DWindFront();
		else if(m_bWindRear) On3DWindRear();
	}
	return true;
}



void QSail7::CreateBoatPolarCurves()
{
	//
	//resets and creates the WPolar graphs curves
	//
	BoatPolar *pBoatPolar;
	CCurve *pCurve[4];

	for(int ig=0; ig<4; ig++) m_BoatGraph[ig].DeleteCurves();

	for (int k=0; k<m_poaBoatPolar->size(); k++)
	{
		pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(k);
		if (pBoatPolar->m_bIsVisible && pBoatPolar->m_Ctrl.size()>0)
		{
			for(int ig=0; ig<4; ig++)
			{
				pCurve[ig] = m_BoatGraph[ig].AddCurve();
				pCurve[ig]->ShowPoints(pBoatPolar->m_bShowPoints);
				pCurve[ig]->SetStyle(pBoatPolar->m_Style);
				pCurve[ig]->SetColor(pBoatPolar->m_Color);
				pCurve[ig]->SetWidth(pBoatPolar->m_Width);
				FillBoatPlrCurve(pCurve[ig], pBoatPolar, (enumPolarVar)m_BoatGraph[ig].GetXVariable(), (enumPolarVar)m_BoatGraph[ig].GetYVariable());
				pCurve[ig]->SetTitle(pBoatPolar->m_BoatPolarName);
			}
		}
	}
}



void QSail7::FillBoatPlrCurve(CCurve *pCurve, BoatPolar *pBoatPolar, enumPolarVar XVar, enumPolarVar YVar)
{
	//
	//The Boat Polar curve object has been created
	//Fill it with the variable data specified by XVar and YVar
	//
	int i;
	double x,y;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	static QList <double> *pX, *pY;

	pX = (QList <double> *) pBoatPolar->GetBoatPlrVariable(XVar);
	pY = (QList <double> *) pBoatPolar->GetBoatPlrVariable(YVar);

	pCurve->SetSelected(-1);


	for (i=0; i<pBoatPolar->m_Ctrl.size(); i++)
	{
		if(XVar==VINF)
		{
			x=(1.0 - pBoatPolar->m_Ctrl.at(i)) * pBoatPolar->m_QInfMin + pBoatPolar->m_Ctrl.at(i) * pBoatPolar->m_QInfMax;
			x *=pMainFrame->m_mstoUnit;
		}
		else if(XVar==BETA) x=(1.0 - pBoatPolar->m_Ctrl.at(i)) * pBoatPolar->m_BetaMin + pBoatPolar->m_Ctrl.at(i) * pBoatPolar->m_BetaMax;
		else if(XVar==PHI)  x=(1.0 - pBoatPolar->m_Ctrl.at(i)) * pBoatPolar->m_PhiMin + pBoatPolar->m_Ctrl.at(i) * pBoatPolar->m_PhiMax;
		else if(pX)
		{
			x = (*pX)[i];
			if(XVar>=LIFT && XVar<=FZ)  x*= pMainFrame->m_NtoUnit;
			if(XVar>=MX   && XVar==MZ)  x*= pMainFrame->m_NmtoUnit;
		}
		else return;

		if(YVar==VINF)
		{
			y=(1.0 - pBoatPolar->m_Ctrl.at(i)) * pBoatPolar->m_QInfMin + pBoatPolar->m_Ctrl.at(i) * pBoatPolar->m_QInfMax;
			y *= pMainFrame->m_mstoUnit;
		}
		else if(YVar==BETA) y=(1.0 - pBoatPolar->m_Ctrl.at(i)) * pBoatPolar->m_BetaMin + pBoatPolar->m_Ctrl.at(i) * pBoatPolar->m_BetaMax;
		else if(YVar==PHI)  y=(1.0 - pBoatPolar->m_Ctrl.at(i)) * pBoatPolar->m_PhiMin + pBoatPolar->m_Ctrl.at(i) * pBoatPolar->m_PhiMax;
		else if(pY)
		{
			y = (*pY)[i];
			if(YVar>=LIFT && YVar<=FZ)  x*= pMainFrame->m_NtoUnit;
			if(YVar>=MX   && YVar==MZ)  x*= pMainFrame->m_NmtoUnit;
		}
		else return;

		pCurve->AddPoint(x,y);
	}
}


void QSail7::SetBoatPolarGraphTitles(Graph* pGraph)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString StrLength;
	QString StrSpeed;
	QString StrMoment;
	QString StrForce;
	GetLengthUnit(StrLength, pMainFrame->m_LengthUnit);
	GetSpeedUnit(StrSpeed, pMainFrame->m_SpeedUnit);
	GetMomentUnit(StrMoment, pMainFrame->m_MomentUnit);
	GetForceUnit(StrForce, pMainFrame->m_ForceUnit);

	pGraph->SetXTitle(BoatPolar::GetPolarVariableName(pGraph->GetXVariable()));
	pGraph->SetYTitle(BoatPolar::GetPolarVariableName(pGraph->GetYVariable()));
}



void QSail7::AddBoatOpp(double *Cp, double *Gamma, double *Sigma, CVector const &F, CVector const &M, CVector const& ForceTrefftz)
{
	//
	// Creates the wing's operating point
	// Fills it with the input resulting from the LLT, VLM or 3D-panel analysis,
	// and inserts it in the array of wing operating points
	//
	// In input, takes
	//   - the array of Cp distribution
	//   - the array of circulation or doublet strengths Gamma
	//   - the array of source strengths Sigma
	//   - the data stored in the LLTAnalysisDlg or PanelAnalysisDlg current instances
	//   - the data stored in the current wing object
	//
	// In output, fills the pBoatOpp object and returns the pointer
	//

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int i,j;

	pMainFrame->SetSaveState(false);

	BoatOpp *pBoatOpp;
	BoatOpp * pNewPoint;
	pNewPoint = new BoatOpp();
	if(pNewPoint == NULL)
	{
		QMessageBox::warning(pMainFrame,tr("Warning"),tr("Not enough memory to store the OpPoint\n"));
		return;
	}
	else
	{
		//load BoatOpp with data
		pNewPoint->m_Color = pMainFrame->GetColor(5);
		bool bFound;
		for(i=0; i<30;i++)
		{
			bFound = false;
			for (j=0; j<m_poaBoatOpp->size();j++)
			{
				pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(j);
				if(pBoatOpp->m_Color == pMainFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound)
			{
				pNewPoint->m_Color = pMainFrame->m_crColors[i];
				break;
			}
		}

		pNewPoint->m_BoatName =  m_pCurBoat->m_BoatName;

		pNewPoint->m_BoatPolarName       = m_pCurBoatPolar->m_BoatPolarName;
		pNewPoint->m_bVLM1               = m_pCurBoatPolar->m_bVLM1;

		pNewPoint->m_NVLMPanels          = m_PanelDlg.m_MatSize;


		//get the data from the PanelAnalysis dialog, and from the wing object
		double Ctrl = m_PanelDlg.m_Ctrl;
		pNewPoint->m_Beta                = m_pCurBoatPolar->m_BetaMin * (1.0-Ctrl) + m_pCurBoatPolar->m_BetaMax * Ctrl;
		pNewPoint->m_Phi                 = m_pCurBoatPolar->m_PhiMin  * (1.0-Ctrl) + m_pCurBoatPolar->m_PhiMax  * Ctrl;
		pNewPoint->m_QInf                = m_PanelDlg.m_QInf;
		pNewPoint->m_Ctrl                = m_PanelDlg.m_Ctrl;
		for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
		{
			pNewPoint->m_SailAngle[is] = m_pCurBoatPolar->m_SailAngleMin[is] * (1.0-Ctrl) + m_pCurBoatPolar->m_SailAngleMax[is] * Ctrl;
		}


		pNewPoint->F  = F;
		pNewPoint->M  = M;
		pNewPoint->ForceTrefftz = ForceTrefftz;

/*		CVector WindDirection, WindNormal, WindSide;
		SetWindAxis(pNewPoint->m_Beta, WindDirection, WindNormal, WindSide);
		pNewPoint->m_Lift = pNewPoint->ForceTrefftz.dot(WindNormal);
		pNewPoint->m_Drag = pNewPoint->ForceTrefftz.dot(WindDirection);*/


		memcpy(pNewPoint->m_Cp,    Cp,    m_MatSize*sizeof(double));
		memcpy(pNewPoint->m_G,     Gamma, m_MatSize*sizeof(double));
		memcpy(pNewPoint->m_Sigma, Sigma, m_MatSize*sizeof(double));

		pNewPoint->m_nWakeNodes     = 0;
		pNewPoint->m_NXWakePanels   = 1;
//		pNewPoint->m_FirstWakePanel = m_pCurBoatPolar->m_WSpan;
		pNewPoint->m_WakeFactor     = 1.0;

	}

	//add the data to the polar object
	m_pCurBoatPolar->AddPoint(pNewPoint);

	bool bIsInserted = false;

	// add the BoatOppoint to the BoatOppoint Array for the current WingName
	if(m_bStoreOpp)
	{
		for (i=0; i<m_poaBoatOpp->size(); i++)
		{
			pBoatOpp = (BoatOpp*)m_poaBoatOpp->at(i);
			if (pNewPoint->m_BoatName == pBoatOpp->m_BoatName)
			{
				if (pNewPoint->m_BoatPolarName == pBoatOpp->m_BoatPolarName)
				{
					if(fabs(pNewPoint->m_Ctrl- pBoatOpp->m_Ctrl)<0.002)
					{
						//replace existing point
						pNewPoint->m_Color = pBoatOpp->m_Color;
						pNewPoint->m_Style = pBoatOpp->m_Style;
						pNewPoint->m_Width = pBoatOpp->m_Width;
						pNewPoint->m_bIsVisible  = pBoatOpp->m_bIsVisible;
						pNewPoint->m_bShowPoints = pBoatOpp->m_bShowPoints;
						if (m_pCurBoatOpp == pBoatOpp) m_pCurBoatOpp = NULL;
						m_poaBoatOpp->removeAt(i);
						delete pBoatOpp;
						m_poaBoatOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = m_poaBoatOpp->size();// to break
					}
					else if (pNewPoint->m_Ctrl > pBoatOpp->m_Ctrl)
					{
						//insert point
						m_poaBoatOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = m_poaBoatOpp->size();// to break
					}
				}
			}
		}
		if (!bIsInserted) 	m_poaBoatOpp->append(pNewPoint);
		m_pCurBoatOpp = pNewPoint;//TODO remove
		m_bResetglOpp = true;
		m_bResetglLegend = true;
	}
	else
	{
		delete pNewPoint;
		pNewPoint = NULL;
	}
}


void QSail7::RotatePanelsX(double const &Angle, CVector const &P)
{
	//
	// rotates the panels around the x-axis to account for bank
	//
	//
	int n, p, pw, lw;

	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB, Pt, Trans;

	for (n=0; n< m_nNodes; n++)
	{
		s_pNode[n].RotateX(P, Angle);
	}

	for (p=0; p< m_MatSize; p++)
	{
		iLA = s_pPanel[p].m_iLA; iLB = s_pPanel[p].m_iLB;
		iTA = s_pPanel[p].m_iTA; iTB = s_pPanel[p].m_iTB;

		LATB = s_pNode[iLA] - s_pNode[iTB];
		TALB = s_pNode[iTA] - s_pNode[iLB];

		s_pPanel[p].SetFrame(s_pNode[iLA], s_pNode[iLB], s_pNode[iTA], s_pNode[iTB]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge
	pw=0;

	if(!m_pCurBoatPolar) return;
/*	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		//consider the first panel of the column;
		Pt = s_pWakeNode[s_pWakePanel[pw].m_iLA];
		Pt.RotateY(P, Angle);
		//define the translation to be applied to the column's left points
		Trans = Pt - s_pWakeNode[s_pWakePanel[pw].m_iLA];

		//each wake column has m_NXWakePanels ... translate all left A nodes
		for(lw=0; lw<m_pCurBoatPolar->m_NXWakePanels; lw++)
		{
			if(lw==0) m_pWakeNode[s_pWakePanel[pw].m_iLA] += Trans;
			s_pWakeNode[s_pWakePanel[pw].m_iTA] += Trans;
			pw++;
		}
	}*/

	//do the same for the very last right wake node column
	pw -= m_pCurBoatPolar->m_NXWakePanels;
	Pt = s_pWakeNode[s_pWakePanel[pw].m_iLB];
	Pt.RotateX(P, Angle);
	//define the translation to be applied to the column's left points
	Trans = Pt-s_pWakeNode[s_pWakePanel[pw].m_iLB];

	//each wake column has m_NXWakePanels ... translate all right B nodes
	for(lw=0; lw<m_pCurBoatPolar->m_NXWakePanels; lw++)
	{
		if(lw==0) s_pWakeNode[s_pWakePanel[pw].m_iLB] += Trans;
		s_pWakeNode[s_pWakePanel[pw].m_iTB] += Trans;
		pw++;
	}

	//Reset panel frame : CollPt has been translated
	for (pw=0; pw< m_WakeSize; pw++)
	{
		iLA = s_pWakePanel[pw].m_iLA; iLB = s_pWakePanel[pw].m_iLB;
		iTA = s_pWakePanel[pw].m_iTA; iTB = s_pWakePanel[pw].m_iTB;
		s_pWakePanel[pw].SetFrame(s_pWakeNode[iLA], s_pWakeNode[iLB], s_pWakeNode[iTA], s_pWakeNode[iTB]);
	}
}



void QSail7::RotatePanelsZ(double const &Angle, CVector const &P)
{
	//
	// rotates the panels around the z-axis to account for sail angle
	//
	//
	int n, p, pw, lw;

	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB, Pt, Trans;

	for (n=0; n< m_nNodes; n++)
	{
		s_pNode[n].RotateZ(P, Angle);
	}
	for (p=0; p< m_MatSize; p++)
	{
		iLA = s_pPanel[p].m_iLA; iLB = s_pPanel[p].m_iLB;
		iTA = s_pPanel[p].m_iTA; iTB = s_pPanel[p].m_iTB;

		LATB = s_pNode[iLA] - s_pNode[iTB];
		TALB = s_pNode[iTA] - s_pNode[iLB];

		if(s_pPanel[p].m_Pos==MIDSURFACE || s_pPanel[p].m_Pos==TOPSURFACE)	s_pPanel[p].SetFrame(s_pNode[iLA], s_pNode[iLB], s_pNode[iTA], s_pNode[iTB]);
		else if (s_pPanel[p].m_Pos==BOTSURFACE)                            s_pPanel[p].SetFrame(s_pNode[iLB], s_pNode[iLA], s_pNode[iTB], s_pNode[iTA]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge
	pw=0;

	if(!m_pCurBoatPolar) return;
/*	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		//consider the first panel of the column;
		Pt = s_pWakeNode[s_pWakePanel[pw].m_iLA];
		Pt.RotateY(P, Angle);
		//define the translation to be applied to the column's left points
		Trans = Pt - s_pWakeNode[s_pWakePanel[pw].m_iLA];

		//each wake column has m_NXWakePanels ... translate all left A nodes
		for(lw=0; lw<m_pCurBoatPolar->m_NXWakePanels; lw++)
		{
			if(lw==0) m_pWakeNode[s_pWakePanel[pw].m_iLA] += Trans;
			s_pWakeNode[s_pWakePanel[pw].m_iTA] += Trans;
			pw++;
		}
	}*/

	//do the same for the very last right wake node column
	pw -= m_pCurBoatPolar->m_NXWakePanels;
	Pt = s_pWakeNode[s_pWakePanel[pw].m_iLB];
	Pt.RotateZ(P, Angle);
	//define the translation to be applied to the column's left points
	Trans = Pt-s_pWakeNode[s_pWakePanel[pw].m_iLB];

	//each wake column has m_NXWakePanels ... translate all right B nodes
	for(lw=0; lw<m_pCurBoatPolar->m_NXWakePanels; lw++)
	{
		if(lw==0) s_pWakeNode[s_pWakePanel[pw].m_iLB] += Trans;
		s_pWakeNode[s_pWakePanel[pw].m_iTB] += Trans;
		pw++;
	}

	//Reset panel frame : CollPt has been translated
	for (pw=0; pw< m_WakeSize; pw++)
	{
		iLA = s_pWakePanel[pw].m_iLA; iLB = s_pWakePanel[pw].m_iLB;
		iTA = s_pWakePanel[pw].m_iTA; iTB = s_pWakePanel[pw].m_iTB;
		s_pWakePanel[pw].SetFrame(s_pWakeNode[iLA], s_pWakeNode[iLB], s_pWakeNode[iTA], s_pWakeNode[iTB]);
	}
}



void QSail7::OnAnalyze()
{
	//
	// The user has requested a launch of the analysis
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pCurBoat)
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), tr("Please define a boat object before running a calculation"));
		return;
	}
	if(!m_pCurBoatPolar)
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), tr("Please define an analysis/polar before running a calculation"));
		return;
	}

	//prevent an automatic and lengthy redraw of the streamlines after the calculation
	m_bStream = m_bSpeeds = false;
	m_pctrlStream->setChecked(false);
	m_pctrlSurfVel->setChecked(false);


	// make sure that the latest parameters are loaded
	OnReadAnalysisData();

	PanelAnalyze(m_ControlMin, m_ControlMax, m_ControlDelta, m_bSequence);


	//refresh the view
	if(m_iView==SAILPOLARVIEW) CreateBoatPolarCurves();
	UpdateView();
	SetControls();
	pMainFrame->setFocus();
}


void QSail7::OnReadAnalysisData()
{
	m_bSequence = m_pctrlSequence->isChecked();
	m_bStoreOpp = m_pctrlStoreOpp->isChecked();

	m_ControlMin   = m_pctrlControlMin->Value();
	m_ControlMax   = m_pctrlControlMax->Value();
	m_ControlDelta = fabs(m_pctrlControlDelta->Value());

	if(fabs(m_ControlDelta)<0.001)
	{
		m_ControlDelta = 0.001;
		m_pctrlControlDelta->SetValue(0.001);
	}
}




void QSail7::PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence)
{
	if(!m_pCurBoat) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	m_PanelDlg.m_bSequence      = bSequence;
	m_PanelDlg.m_pBoat          = m_pCurBoat;
	m_PanelDlg.m_pBoatPolar     = m_pCurBoatPolar;
	m_PanelDlg.m_MaxWakeIter    = 0;
	m_PanelDlg.m_WakeInterNodes = 0;
	m_PanelDlg.m_nWakeNodes     = 0;
	m_PanelDlg.m_NWakeColumn    = 0;
	m_PanelDlg.m_bSequence      = bSequence;
	m_PanelDlg.m_WakeSize       = m_WakeSize;
	m_PanelDlg.m_bTrefftz       = m_bTrefftz;
	m_PanelDlg.m_nNodes         = m_nNodes;
//	m_PanelDlg.m_NSurfaces      = m_NSurfaces;
//	m_PanelDlg.m_ppSurface      = m_pSurface;
	m_PanelDlg.m_MatSize        = m_MatSize;


	for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
	{
		m_PanelDlg.m_pSailList[is] = m_pCurBoat->m_poaSail.at(is);
	}

	m_PanelDlg.m_ControlMin    = V0;
	m_PanelDlg.m_ControlMax    = VMax;
	m_PanelDlg.m_ControlDelta  = VDelta;


	m_PanelDlg.move(pMainFrame->m_DlgPos);

	m_PanelDlg.InitDialog();
	m_PanelDlg.show();
	m_PanelDlg.StartAnalysis();
	m_bResetglMesh = true; //TODO remove

	if(!m_bLogFile || !m_PanelDlg.m_bWarning) m_PanelDlg.hide();

	pMainFrame->m_DlgPos = m_PanelDlg.pos();

	pMainFrame->UpdateBoatOpps();

	if(m_pCurBoat)     SetBoatOpp(false, m_PanelDlg.m_ControlMin);


	UpdateView();
}



void QSail7::SetAnalysisParams()
{
	m_pctrlSequence->setChecked(m_bSequence);
	m_pctrlStoreOpp->setChecked(m_bStoreOpp);
	m_pctrlControlMin->SetValue(m_ControlMin);
	m_pctrlControlMax->SetValue(m_ControlMax);
	m_pctrlControlDelta->SetValue(m_ControlDelta);

	m_pctrlAnalyze->setEnabled(m_pCurBoatPolar);

	if (!m_pCurBoatPolar)
	{
		m_pctrlSequence->setEnabled(false);
		m_pctrlControlMin->setEnabled(false);
		m_pctrlControlMax->setEnabled(false);
		m_pctrlControlDelta->setEnabled(false);
		m_pctrlStoreOpp->setEnabled(false);
		return;
	}
	else
	{
		m_pctrlSequence->setEnabled(true);
		m_pctrlControlMin->setEnabled(true);
		m_pctrlControlMax->setEnabled(m_bSequence);
		m_pctrlControlDelta->setEnabled(m_bSequence);

		m_pctrlStoreOpp->setEnabled(true);
	}
}




void QSail7::showEvent(QShowEvent *event)
{
	SetAnalysisParams();
	SetCurveParams();
}


void QSail7::OnStoreOpp()
{
	m_bStoreOpp = m_pctrlStoreOpp->isChecked();
}


void QSail7::OnSequence()
{
	m_bSequence = m_pctrlSequence->isChecked();
	m_pctrlControlMax->setEnabled(m_bSequence);
	m_pctrlControlDelta->setEnabled(m_bSequence);
}


void QSail7::OnPanelForce()
{
	m_bPanelForce	 = m_pctrlPanelForce->isChecked();
	if(m_bPanelForce)
	{
		m_b3DCp =false;
		m_pctrlCp->setChecked(false);
	}
	if(m_iView == SAIL3DVIEW)
	{
//		if(!m_bAnimateWOpp) UpdateView();
	}
	UpdateView();
}


void QSail7::OnShowCurve()
{
	m_bCurveVisible = m_pctrlShowCurve->isChecked();
	m_bCurvePoints  = m_pctrlShowPoints->isChecked();
	UpdateCurve();
}


void QSail7::OnShowLift()
{
	m_bLift = m_pctrlLift->isChecked();
	UpdateView();
}


void QSail7::OnShowBodyForces()
{
	m_bBodyForces = m_pctrlBodyForces->isChecked();
	UpdateView();
}



void QSail7::OnShowWater()
{
	m_bWater = m_pctrlWater->isChecked();
	UpdateView();
}


void QSail7::OnShowWind()
{
	m_bWindDirection = m_pctrlWindDirection->isChecked();
	UpdateView();
}


void QSail7::On3DCp()
{
	//
	// The user has toggled the switch for the display of Cp coefficients
	//
	m_b3DCp = m_pctrlCp->isChecked();

	if(m_b3DCp)
	{
		m_bSurfaces = false;
		m_pctrlSurfaces->setChecked(false);
		m_bPanelForce = false;
		m_pctrlPanelForce->setChecked(false);
//		m_bVLMPanels = false;
//		m_pctrlPanels->setChecked(false);
	}
	UpdateView();
}

void QSail7::OnMoment()
{

}


void QSail7::OnStreamlines()
{
	m_bStream = m_pctrlStream->isChecked();
	if(m_pctrlStream->isChecked())
	{
//		m_bResetglStream = true;
	}
	if(m_iView==SAIL3DVIEW) UpdateView();
}


void QSail7::OnSurfaceSpeeds()
{
	m_bSpeeds = m_pctrlSurfVel->isChecked();
	if(m_pctrlSurfVel->isChecked())
	{
//		m_bResetglSpeeds = true;
	}
	if(m_iView==SAIL3DVIEW) UpdateView();
}


void QSail7::OnAnimateBoatOpp()
{
	//
	// Launch the animation of the BoatOpp display
	// Will display all the available BoatOpps for this BoatPolar in sequence
	//
	if(!m_pCurBoat || !m_pCurBoatPolar || m_iView!=SAIL3DVIEW)
	{
		m_bAnimateBoatOpp = false;
		return;
	}
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int l;

	if(m_pctrlBOppAnimate->isChecked())
	{
		for (l=0; l< m_poaBoatOpp->size(); l++)
		{
			BoatOpp *pBOpp = (BoatOpp*)m_poaBoatOpp->at(l);

			if (pBOpp &&
				pBOpp->m_BoatPolarName  == m_pCurBoatPolar->m_BoatPolarName &&
				pBOpp->m_BoatName == m_pCurBoat->m_BoatName)
			{
					if(m_pCurBoatOpp->m_Ctrl- pBOpp->m_Ctrl<0.0001)
						m_posAnimateBOpp = l;
			}
		}

		m_bAnimateBoatOpp= true;
		int speed = m_pctrlAnimateBOppSpeed->value();
		m_pTimerBoatOpp->setInterval(800-speed);
		m_pTimerBoatOpp->start();
	}
	else
	{
		StopAnimate();
	}
	QApplication::restoreOverrideCursor();
}

void QSail7::OnAnimateBoatOppSingle()
{
	//
	// A signal has been received from the timer to update the WOPP display
	// So display the next WOpp in the sequence
	//

	static bool bIsValid, bSkipOne;
	static int size;
	static BoatOpp *pBOpp;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_iView!=SAIL3DVIEW)             return; //nothing to animate
	if(!m_pCurBoat || !m_pCurBoatPolar) return; //nothing to animate

	pBOpp = NULL;

	size = m_poaBoatOpp->size();
	if(size<=1) return;

	bIsValid = false;
	bSkipOne = false;

	while(!bIsValid)
	{
		//Find the current position to display
		pBOpp = (BoatOpp*)m_poaBoatOpp->at(m_posAnimateBOpp);
		if(!pBOpp) return;

		bIsValid =(pBOpp->m_BoatPolarName==m_pCurBoatPolar->m_BoatPolarName  &&  pBOpp->m_BoatName==m_pCurBoat->m_BoatName);

		if (bIsValid && !bSkipOne)
		{
			m_pCurBoatOpp = pBOpp;

			m_bResetglOpp      = true;
			m_bResetglDownwash = true;
			m_bResetglLift     = true;
			m_bResetglDrag     = true;
			m_bResetglWake     = true;
			m_bResetglLegend   = true;
			m_bResetglCPForces = true;

			UpdateView();

			//select current BOpp in Combobox
			pMainFrame->SelectBoatOpp(pBOpp->m_Ctrl);
		}
		else if(bIsValid) bSkipOne = false;

		if(m_bAnimateBoatOppPlus)
		{
			m_posAnimateBOpp++;
			if (m_posAnimateBOpp >= size)
			{
				m_posAnimateBOpp = size-1;
				m_bAnimateBoatOppPlus = false;
				bSkipOne = true;
			}
		}
		else
		{
			m_posAnimateBOpp--;
			if (m_posAnimateBOpp <0)
			{
				m_posAnimateBOpp = 0;
				m_bAnimateBoatOppPlus = true;
				bSkipOne = true;
			}
		}

		if(m_posAnimateBOpp<0 || m_posAnimateBOpp>=size) return;
	}
}


void QSail7::OnAnimateBoatOppSpeed(int pos)
{
	//
	// The user has changed the animation speed for the WOpp display
	//
	if(m_pTimerBoatOpp->isActive())
	{
		m_pTimerBoatOpp->setInterval(800-pos);
	}
}


void QSail7::OnVortices()
{

}

void QSail7::OnClipPlane(int pos)
{
	double planepos =  (double)pos/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}


void QSail7::OnLight()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_bglLight = ! m_bglLight;
	SailViewWidget::s_bglLight= m_bglLight;

	pMainFrame->Sail73DLightAct->setChecked(m_bglLight);
	UpdateView();
}


void QSail7::OnOpenSail7File()
{

}


void QSail7::OnCurveColor()
{
	QColor Color = QColorDialog::getColor(m_CurveColor);
	if(Color.isValid()) m_CurveColor = Color;
	FillComboBoxes();

	UpdateCurve();
}


void QSail7::OnCurveStyle(int index)
{
	m_CurveStyle = index;
	FillComboBoxes();
	UpdateCurve();
}


void QSail7::OnCurveWidth(int index)
{
	m_CurveWidth = index+1;
	FillComboBoxes();
	UpdateCurve();
}


void QSail7::FillComboBoxes(bool bEnable)
{
	if(!bEnable)
	{
		m_pctrlCurveColor->setEnabled(false);
		m_pctrlCurveStyle->setEnabled(false);
		m_pctrlCurveWidth->setEnabled(false);
		m_pctrlShowCurve->setEnabled(false);
		m_pctrlShowPoints->setEnabled(false);
	}
	else
	{
		m_pctrlCurveColor->setEnabled(true);
		m_pctrlCurveStyle->setEnabled(true);
		m_pctrlCurveWidth->setEnabled(true);
		m_pctrlShowCurve->setEnabled(true);
		m_pctrlShowPoints->setEnabled(true);
	}
	int LineWidth[5];
	for (int i=0; i<5;i++) LineWidth[i] = m_CurveWidth;
	m_pStyleDelegate->SetLineWidth(LineWidth); // the same selected width for all styles
	m_pStyleDelegate->SetLineColor(m_CurveColor);

	int LineStyle[5];
	for (int i=0; i<5;i++) LineStyle[i] = m_CurveStyle;
	m_pWidthDelegate->SetLineStyle(LineStyle); //the same selected style for all widths
	m_pWidthDelegate->SetLineColor(m_CurveColor);

	m_pctrlCurveStyle->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);
	m_pctrlCurveWidth->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);

	m_pctrlCurveColor->SetColor(m_CurveColor);
	m_pctrlCurveColor->SetStyle(m_CurveStyle);
	m_pctrlCurveColor->SetWidth(m_CurveWidth);

	m_pctrlCurveStyle->update();
	m_pctrlCurveWidth->update();
	m_pctrlCurveColor->update();

	m_pctrlCurveStyle->setCurrentIndex(m_CurveStyle);
	m_pctrlCurveWidth->setCurrentIndex((int)m_CurveWidth-1);
}


void QSail7::UpdateCurve()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView==SAILPOLARVIEW && m_pCurBoatPolar)
	{
		m_pCurBoatPolar->m_Color = m_CurveColor;
		m_pCurBoatPolar->m_Style = m_CurveStyle;
		m_pCurBoatPolar->m_Width = m_CurveWidth;
		m_pCurBoatPolar->m_bIsVisible  = m_bCurveVisible;
		m_pCurBoatPolar->m_bShowPoints = m_bCurvePoints;
		CreateBoatPolarCurves();
	}
	UpdateView();
	pMainFrame->SetSaveState(false);

}


void QSail7::GLCreatePanelForces(BoatOpp *pBoatOpp)
{
	//
	// Creates an OpenGl list of pressure arrows on the panels
	//

	if(!m_pCurBoat || !pBoatOpp || !m_pCurBoatPolar) return;

	int p;
	double force, cosa, sina2, cosa2, color;
	double range;
	double coef = .1;
	Quaternion Qt; // Quaternion operator to align the reference arrow to the panel's normal
	CVector Omega; //rotation vector to align the reference arrow to the panel's normal
	CVector O;

	//The vectors defining the reference arrow
	CVector R(0.0,0.0,1.0);
	CVector R1( 0.05, 0.0, -0.1);
	CVector R2(-0.05, 0.0, -0.1);

	//The three vectors defining the arrow on the panel
	CVector P, P1, P2;


	//define the range of values to set the colors in accordance
	m_ForceMin = 1.e10;
	m_ForceMax = -m_ForceMin;
	for (int p=0; p<m_MatSize; p++)
	{
		if(pBoatOpp->m_Cp[p]*s_pPanel[p].GetArea()>m_ForceMax) m_ForceMax = pBoatOpp->m_Cp[p]*s_pPanel[p].GetArea();
		if(pBoatOpp->m_Cp[p]*s_pPanel[p].GetArea()<m_ForceMin) m_ForceMin = pBoatOpp->m_Cp[p]*s_pPanel[p].GetArea();
	}
	m_ForceMin *= 0.5*m_pCurBoatPolar->m_Density *pBoatOpp->m_QInf*pBoatOpp->m_QInf;
	m_ForceMax *= 0.5*m_pCurBoatPolar->m_Density *pBoatOpp->m_QInf*pBoatOpp->m_QInf;
	range = m_ForceMax - m_ForceMin;

	for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
	{
		QSail *pSail = (QSail*)m_pCurBoat->m_poaSail.at(is);
		glNewList(SAILFORCELISTBASE+is, GL_COMPILE);
		{
			m_GLList++;
			glLineWidth(1.0);

			for (p=0; p<pSail->m_NElements; p++)
			{
				int pp = pSail->m_pPanel[p].m_iElement;
				CPanel *pPanel = s_pPanel+pp;
				// plot Cp ? f ? f/s=q.Cp ?
				force = 0.5*m_pCurBoatPolar->m_Density *pBoatOpp->m_QInf*pBoatOpp->m_QInf *pBoatOpp->m_Cp[pp]*pPanel->GetArea();
				color = (force-m_ForceMin)/range;
				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				force *= GL3DScales::s_LiftScale *coef;

				O = pPanel->CtrlPt;


				// Rotate the reference arrow to align it with the panel normal

				if(R==P)
				{
					Qt.Set(0.0, 0.0,0.0,1.0); //Null quaternion
				}
				else
				{
					cosa   = R.dot(pPanel->Normal);
					sina2  = sqrt((1.0 - cosa)*0.5);
					cosa2  = sqrt((1.0 + cosa)*0.5);
//					angle = acos(cosa2)*180.0/PI;

					Omega = R * pPanel->Normal;//crossproduct
					Omega.Normalize();
					Omega *=sina2;
					Qt.Set(cosa2, Omega.x, Omega.y, Omega.z);
				}

				Qt.Conjugate(R,  P);
				Qt.Conjugate(R1, P1);
				Qt.Conjugate(R2, P2);

				// Scale the pressure vector
				P  *= force;
				P1 *= force;
				P2 *= force;


				glBegin(GL_LINES);
				{
					glVertex3d(O.x, O.y, O.z);
					glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
				}
				glEnd();

				if(force>0)
				{
					glBegin(GL_LINES);
					{
						glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
						glVertex3d(O.x+P.x+P1.x, O.y+P.y+P1.y, O.z+P.z+P1.z);
					}
					glEnd();
					glBegin(GL_LINES);
					{
						glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
						glVertex3d(O.x+P.x+P2.x, O.y+P.y+P2.y, O.z+P.z+P2.z);
					}
					glEnd();
				}
				else
					{
						glBegin(GL_LINES);
						{
							glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
							glVertex3d(O.x+P.x+P1.x, O.y+P.y+P1.y, O.z+P.z+P1.z);
						}
						glEnd();
						glBegin(GL_LINES);
						{
							glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
							glVertex3d(O.x+P.x+P2.x, O.y+P.y+P2.y, O.z+P.z+P2.z);
						}
						glEnd();
					}

			}
		}
		glEndList();
	}

	for(int ih=0; ih<m_pCurBoat->m_poaHull.size(); ih++)
	{
		CBody *pHull = (CBody*)m_pCurBoat->m_poaHull.at(ih);
		glNewList(BODYFORCELISTBASE+ih, GL_COMPILE);
		{
			m_GLList++;
			glLineWidth(1.0);

			for (p=0; p<pHull->m_NElements; p++)
			{
				int pp = pHull->m_pPanel[p].m_iElement;
				CPanel *pPanel = s_pPanel+pp;
				// plot Cp ? f ? f/s=q.Cp ?
				force = 0.5*m_pCurBoatPolar->m_Density *pBoatOpp->m_QInf*pBoatOpp->m_QInf *pBoatOpp->m_Cp[pp]*pPanel->GetArea();
				color = (force-m_ForceMin)/range;
				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				force *= GL3DScales::s_LiftScale *coef;

				O = pPanel->CollPt;

				// Rotate the reference arrow to align it with the panel normal

				if(R==P)
				{
					Qt.Set(0.0, 0.0,0.0,1.0); //Null quaternion
				}
				else
				{
					cosa   = R.dot(pPanel->Normal);
					sina2  = sqrt((1.0 - cosa)*0.5);
					cosa2  = sqrt((1.0 + cosa)*0.5);
//					angle = acos(cosa2)*180.0/PI;

					Omega = R * pPanel->Normal;//crossproduct
					Omega.Normalize();
					Omega *=sina2;
					Qt.Set(cosa2, Omega.x, Omega.y, Omega.z);
				}

				Qt.Conjugate(R,  P);
				Qt.Conjugate(R1, P1);
				Qt.Conjugate(R2, P2);

				// Scale the pressure vector
				P  *= force;
				P1 *= force;
				P2 *= force;

				// Plot
				if(pBoatOpp->m_Cp[p]>0)
				{
					// compression, point towards the surface
//					P.Set(-P.x, -P.y, -P.z);
					glBegin(GL_LINES);
					{
						glVertex3d(O.x, O.y, O.z);
						glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
					}
					glEnd();
					glBegin(GL_LINES);
					{
						glVertex3d(O.x, O.y, O.z);
						glVertex3d(O.x-P1.x, O.y-P1.y, O.z-P1.z);
					}
					glEnd();
					glBegin(GL_LINES);
					{
						glVertex3d(O.x, O.y, O.z);
						glVertex3d(O.x-P2.x, O.y-P2.y, O.z-P2.z);
					}
					glEnd();
				}
				else
				{
					// depression, point outwards from the surface
					P.Set(-P.x, -P.y, -P.z);
					glBegin(GL_LINES);
					{
						glVertex3d(O.x, O.y, O.z);
						glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
					}
					glEnd();
					glBegin(GL_LINES);
					{
						glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
						glVertex3d(O.x+P.x-P1.x, O.y+P.y-P1.y, O.z+P.z-P1.z);
					}
					glEnd();
					glBegin(GL_LINES);
					{
						glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
						glVertex3d(O.x+P.x-P2.x, O.y+P.y-P2.y, O.z+P.z-P2.z);
					}
					glEnd();
				}
			}
		}
		glEndList();
	}
}




void QSail7::GLCreateSailGeom(int GLList, QSail*pSail, CVector Position)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;

	glNewList(GLList, GL_COMPILE);
	{
		m_GLList++;

		if(pMainFrame->m_bAlphaChannel && pSail->m_SailColor.alpha()<255)
		{
			glColor4d(pSail->m_SailColor.redF(),pSail->m_SailColor.greenF(),pSail->m_SailColor.blueF(), pSail->m_SailColor.alphaF());
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glColor3d(pSail->m_SailColor.redF(),pSail->m_SailColor.greenF(),pSail->m_SailColor.blueF());
			glDisable (GL_BLEND);
		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPolygonOffset(1.0, 1.0);

		static CVector PtA, PtB, PtA0, PtB0, LATB, TALB, PtNormal;
		PtNormal.Set(0.0, 1.0, 0.0);
		for (int k=0; k<SPANPOINTS; k++)
		{
			double zrelA = (double)k     / (double)SPANPOINTS;
			double zrelB = (double)(k+1) / (double)SPANPOINTS;

			glBegin(GL_QUAD_STRIP);
			{
				for (int l=0; l<=SIDEPOINTS; l++)
				{
					double xrel = (double)l/(double)SIDEPOINTS;

					PtA = pSail->GetPoint(xrel, zrelA);
					PtB = pSail->GetPoint(xrel, zrelB);

					if(l==0)
					{
						PtA0  = pSail->GetPoint(0.01, zrelA);
						PtB0  = pSail->GetPoint(0.01, zrelB);
						LATB = PtA0-PtB;
						TALB = PtB0-PtA;
						PtNormal = TALB * LATB;
						PtNormal.Normalize();
					}
					else
					{
						LATB = PtA-PtB0;
						TALB = PtB-PtA0;
						PtNormal = TALB * LATB;
						PtNormal.Normalize();
					}
					glNormal3d(PtNormal.x, PtNormal.y, PtNormal.z);
					glVertex3d(PtA.x+Position.x, PtA.y, PtA.z+Position.z);
					glVertex3d(PtB.x+Position.x, PtB.y, PtB.z+Position.z);
					PtA0 = PtA;
					PtB0 = PtB;
				}
			}
			glEnd();
		}
		glDisable(GL_BLEND);
	}
	glEndList();







	glNewList(GLList+MAXSAILS, GL_COMPILE);
	{
		//remove
/*		{
			CVector PtA;
			glColor3d(1.0, 0.7, .41);
			for (int k=0; k<SPANPOINTS; k++)
			{
				double zrelA = (double)k     / (double)SPANPOINTS;
				glBegin(GL_LINE_STRIP);
				{
					for (int l=0; l<=SIDEPOINTS; l++)
					{
						double xrel = (double)l/(double)SIDEPOINTS;

						PtA = pSail->GetPoint(xrel, zrelA);

						glVertex3d(PtA.x+Position.x, PtA.y, PtA.z+Position.z);

					}
				}
				glEnd();
			}
		}*/

		m_GLList++;
		glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		glColor3d(W3dPrefsDlg::s_OutlineColor.redF(), W3dPrefsDlg::s_OutlineColor.greenF(), W3dPrefsDlg::s_OutlineColor.blueF());
		glLineWidth(W3dPrefsDlg::s_OutlineWidth);
		CVector PtA;

		//Sections
		// only top and bottom - rest is virtual

		//Bot section
		glBegin(GL_LINE_STRIP);
		{
			for (int l=0; l<=SIDEPOINTS; l++)
			{
				PtA = pSail->GetSectionPoint(0, (double)l/(double)SIDEPOINTS);
				glVertex3d(PtA.x+Position.x, PtA.y, PtA.z+Position.z);
			}
		}
		glEnd();

		//top section
		glBegin(GL_LINE_STRIP);
		{
			for (int l=0; l<=SIDEPOINTS; l++)
			{
				PtA = pSail->GetSectionPoint(pSail->m_oaSection.size()-1, (double)l/(double)SIDEPOINTS);
				glVertex3d(PtA.x+Position.x, PtA.y, PtA.z+Position.z);
			}
		}
		glEnd();

		//Luff
		glBegin(GL_LINE_STRIP);
		{
			for (int j=0; j<=SPANPOINTS; j++)
			{
				PtA = pSail->GetPoint(0.0, (double)j/(double)SPANPOINTS);
				glVertex3d(PtA.x+Position.x, PtA.y, PtA.z+Position.z);
			}
		}
		glEnd();

		//Leech
		glBegin(GL_LINE_STRIP);
		{
			for (int j=0; j<=SPANPOINTS; j++)
			{
				PtA = pSail->GetPoint(1.0, (double)j/(double)SPANPOINTS);
				glVertex3d(PtA.x+Position.x, PtA.y, PtA.z+Position.z);
			}
		}
		glEnd();

/*		CVector PtB;
		for (int j=0; j<=SPANPOINTS; j++)
		{
			glBegin(GL_LINES);
			{
				PtA = pSail->GetPoint(0.0, (double)j/(double)SPANPOINTS);
				PtB = pSail->GetPoint(1.0, (double)j/(double)SPANPOINTS);
				glVertex3d(PtA.x+Position.x, PtA.y, PtA.z+Position.z);
				glVertex3d(PtB.x+Position.x, PtB.y, PtB.z+Position.z);
			}
			glEnd();
		}*/
	}
	glEndList();
}



void QSail7::GLCreateCp(BoatOpp *pBoatOpp)
{
	if(!m_pCurBoat || !pBoatOpp)
	{
		glNewList(PANELCP,GL_COMPILE);
		glEndList();
		return;
	}
	int p, pp, n, averageInf, averageSup, average100;
	int nPanels;
	double color;
	double lmin, lmax, range;
	double CpInf[2*VLMMAXMATSIZE], CpSup[2*VLMMAXMATSIZE], Cp100[2*VLMMAXMATSIZE];
	CVector LA,LB,TA,TB;
	nPanels = pBoatOpp->m_NVLMPanels;

	lmin = 10000.0;
	lmax = -10000.0;
	// find min and max Cp for scale set
	for (n=0; n<m_nNodes; n++)
	{
		averageInf = 0; averageSup = 0; average100 = 0;
		CpInf[n] = 0.0; CpSup[n] = 0.0; Cp100[n] = 0.0;
		for (pp=0; pp< nPanels; pp++)
		{
			if (s_pNode[s_pPanel[pp].m_iLA].IsSame(s_pNode[n]) || s_pNode[s_pPanel[pp].m_iTA].IsSame(s_pNode[n]) ||
				s_pNode[s_pPanel[pp].m_iTB].IsSame(s_pNode[n]) || s_pNode[s_pPanel[pp].m_iLB].IsSame(s_pNode[n]))
			{
				if(s_pPanel[pp].m_Pos==TOPSURFACE)
				{
					CpSup[n] +=pBoatOpp->m_Cp[pp];
					averageSup++;
				}
				else if(s_pPanel[pp].m_Pos<=MIDSURFACE)
				{
					CpInf[n] +=pBoatOpp->m_Cp[pp];
					averageInf++;
				}
				else if(s_pPanel[pp].m_Pos>=SIDESURFACE)
				{
					Cp100[n] +=pBoatOpp->m_Cp[pp];
					average100++;
				}
			}
		}
		if(averageSup>0)
		{
			CpSup[n] /= averageSup;
			if(CpSup[n]<lmin) lmin = CpSup[n];
			if(lmax<CpSup[n]) lmax = CpSup[n];
		}
		if(averageInf>0)
		{
			CpInf[n] /= averageInf;
			if(CpInf[n]<lmin) lmin = CpInf[n];
			if(lmax<CpInf[n]) lmax = CpInf[n];
		}
		if(average100>0)
		{
			Cp100[n] /= average100;
			if(Cp100[n]<lmin) lmin = Cp100[n];
			if(lmax<Cp100[n]) lmax = Cp100[n];
		}
	}

	if(GL3DScales::s_bAutoCpScale)
	{
		GL3DScales::s_LegendMin = lmin;
		GL3DScales::s_LegendMax = lmax;
	}
	else
	{
		lmin = GL3DScales::s_LegendMin;
		lmax = GL3DScales::s_LegendMax;
	}
	range = lmax - lmin;

	for(int is=0; is<m_pCurBoat->m_poaSail.size(); is++)
	{
		QSail *pSail = m_pCurBoat->m_poaSail.at(is);
		CPanel *pCpPanel;

		glNewList(SAILCPBASE+is, GL_COMPILE);
		{
			m_GLList++;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);

			glLineWidth(1.0);

			for (p=0; p<pSail->m_NElements; p++)
			{
				int pp = pSail->m_pPanel[p].m_iElement;

				pCpPanel = s_pPanel+pp;
				glBegin(GL_QUADS);
				{
					TA.Copy(s_pNode[pCpPanel->m_iTA]);
					TB.Copy(s_pNode[pCpPanel->m_iTB]);
					LA.Copy(s_pNode[pCpPanel->m_iLA]);
					LB.Copy(s_pNode[pCpPanel->m_iLB]);

					if(pCpPanel->m_Pos==TOPSURFACE)      color = (CpSup[pCpPanel->m_iLA]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE) color = (CpInf[pCpPanel->m_iLA]-lmin)/range;
					else                                 color = (Cp100[pCpPanel->m_iLA]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(LA.x, LA.y, LA.z);

					if(pCpPanel->m_Pos==TOPSURFACE)       color = (CpSup[pCpPanel->m_iTA]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE)  color = (CpInf[pCpPanel->m_iTA]-lmin)/range;
					else                                  color = (Cp100[pCpPanel->m_iTA]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(TA.x, TA.y, TA.z);

					if(pCpPanel->m_Pos==TOPSURFACE)      color = (CpSup[pCpPanel->m_iTB]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE) color = (CpInf[pCpPanel->m_iTB]-lmin)/range;
					else                                 color = (Cp100[pCpPanel->m_iTB]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(TB.x, TB.y, TB.z);

					if(pCpPanel->m_Pos==TOPSURFACE)      color = (CpSup[pCpPanel->m_iLB]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE) color = (CpInf[pCpPanel->m_iLB]-lmin)/range;
					else                                 color = (Cp100[pCpPanel->m_iLB]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(LB.x, LB.y, LB.z);

				}
				glEnd();
			}
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		glEndList();
	}

	for(int ib=0; ib<m_pCurBoat->m_poaHull.size(); ib++)
	{
		CBody* pHull = m_pCurBoat->m_poaHull.at(ib);
		CPanel *pCpPanel;

		glNewList(BODYCPBASE+ib, GL_COMPILE);
		{
			m_GLList++;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);

			glLineWidth(1.0);

			for (p=0; p<pHull->m_NElements; p++)
			{
				int pp = pHull->m_pPanel[p].m_iElement;

				pCpPanel = s_pPanel+pp;
				glBegin(GL_QUADS);
				{
					TA.Copy(s_pNode[pCpPanel->m_iTA]);
					TB.Copy(s_pNode[pCpPanel->m_iTB]);
					LA.Copy(s_pNode[pCpPanel->m_iLA]);
					LB.Copy(s_pNode[pCpPanel->m_iLB]);

					if(pCpPanel->m_Pos==TOPSURFACE)      color = (CpSup[pCpPanel->m_iLA]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE) color = (CpInf[pCpPanel->m_iLA]-lmin)/range;
					else                                 color = (Cp100[pCpPanel->m_iLA]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(LA.x, LA.y, LA.z);

					if(pCpPanel->m_Pos==TOPSURFACE)      color = (CpSup[pCpPanel->m_iTA]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE) color = (CpInf[pCpPanel->m_iTA]-lmin)/range;
					else                                 color = (Cp100[pCpPanel->m_iTA]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(TA.x, TA.y, TA.z);

					if(pCpPanel->m_Pos==TOPSURFACE)      color = (CpSup[pCpPanel->m_iTB]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE) color = (CpInf[pCpPanel->m_iTB]-lmin)/range;
					else                                 color = (Cp100[pCpPanel->m_iTB]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(TB.x, TB.y, TB.z);

					if(pCpPanel->m_Pos==TOPSURFACE)      color = (CpSup[pCpPanel->m_iLB]-lmin)/range;
					else if(pCpPanel->m_Pos<=MIDSURFACE) color = (CpInf[pCpPanel->m_iLB]-lmin)/range;
					else                                 color = (Cp100[pCpPanel->m_iLB]-lmin)/range;
					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(LB.x, LB.y, LB.z);

				}
				glEnd();
			}
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		glEndList();
	}
}


void QSail7::On3DPrefs()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_3DPrefsDlg.move(pMainFrame->m_DlgPos);
	m_3DPrefsDlg.InitDialog();
	m_3DPrefsDlg.exec();
	pMainFrame->m_DlgPos = m_3DPrefsDlg.pos();
	m_bResetglMesh = m_bResetglSailGeom = true;
	m_bResetglBody = true;
	UpdateView();
}


void QSail7::OnGL3DScale()
{
	if(m_iView != SAIL3DVIEW)
	{
//		m_pctrl3DSettings->setChecked(false);
		return;
	}
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(pMainFrame->m_pctrl3DScalesWidget->isVisible()) pMainFrame->m_pctrl3DScalesWidget->hide();
	else                                               pMainFrame->m_pctrl3DScalesWidget->show();

//	pMainFrame->Sail73DScalesAct->setChecked(pMainFrame->m_pctrl3DScalesWidget->isVisible());
//	if(m_pctrl3DSettings->isChecked()) pMainFrame->m_pctrl3DScalesWidget->show();
//	else                               pMainFrame->m_pctrl3DScalesWidget->hide();
}



BoatOpp* QSail7::GetBoatOpp(double x)
{
	//
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current wing and current WPolar
	//
	if(!m_pCurBoat || !m_pCurBoatPolar) return NULL;
	int i;
	BoatOpp* pBOpp;

	for (i=0; i<m_poaBoatOpp->size(); i++)
	{
		pBOpp = (BoatOpp*)m_poaBoatOpp->at(i);
		if ((pBOpp->m_BoatName==m_pCurBoat->m_BoatName) &&(pBOpp->m_BoatPolarName==m_pCurBoatPolar->m_BoatPolarName))
		{
			if(fabs(pBOpp->m_Ctrl - x)<0.002) return pBOpp;
		}
	}
	return NULL;
}




void QSail7::SetBoatPlrLegendPos()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	int h  = p2DWidget->geometry().height();
	int w  = p2DWidget->geometry().width();
	int h23 = (int)(2*h/3);

	int margin = 10;
	if(m_iView==SAILPOLARVIEW)
	{
		if(m_iBoatPlrView == 1)
		{
			m_BoatPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_BoatPlrLegendOffset.ry() = margin;
		}
		else if (m_iBoatPlrView == 2)
		{
			m_BoatPlrLegendOffset.rx() = margin;
			m_BoatPlrLegendOffset.ry() = h23+margin;
		}
		else if	(m_iBoatPlrView == 4)
		{
			m_BoatPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_BoatPlrLegendOffset.ry() = margin;
		}
	}
}




void QSail7::SetCurveParams()
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView==SAILPOLARVIEW)
	{
		if(m_pCurBoatPolar)
		{
			m_pctrlShowCurve->setChecked(m_pCurBoatPolar->m_bIsVisible);
			m_pctrlShowPoints->setChecked(m_pCurBoatPolar->m_bShowPoints);

			m_CurveColor = m_pCurBoatPolar->m_Color;
			m_CurveStyle = m_pCurBoatPolar->m_Style;
			m_CurveWidth = m_pCurBoatPolar->m_Width;
			FillComboBoxes();
		}
		else
		{
			FillComboBoxes(false);
		}
	}
}




void QSail7::DrawBoatPolarLegend(QPainter &painter, QPoint place, int bottom)
{
	//
	// Draws the legend of the polar graphs
	//

	painter.save();

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int LegendSize, LegendWidth, ypos;
	int i,j,k,l, ny, x1;

	LegendSize = 30;
	LegendWidth = 280;

	painter.setFont(pMainFrame->m_TextFont);

	QFontMetrics fm(pMainFrame->m_TextFont);
	ypos = fm.height();

	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);
	TextPen.setWidth(1);

	QStringList str; // we need to make an inventory of wings
	BoatPolar * pBoatPolar;
	CBoat *pBoat;

	for (j=0; j<m_poaBoat->size(); j++)
	{
		pBoat = (CBoat*)m_poaBoat->at(j);
		for (i=0; i<m_poaBoatPolar->size(); i++)
		{
			pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(i);
			if (pBoatPolar->m_BoatName == pBoat->m_BoatName && pBoatPolar->m_bIsVisible && pBoatPolar->m_Ctrl.size())
			{
				str.append(pBoat->m_BoatName);
				break;
			}
		}// finished inventory
	}

	int nBoats= str.size();

	painter.setBackgroundMode(Qt::TransparentMode);
	QBrush LegendBrush(pMainFrame->m_BackgroundColor);
	painter.setBrush(LegendBrush);

	QPen LegendPen;
	LegendPen.setWidth(1);

	ny=0;
	for (k=0; k<nBoats; k++)
	{
		int BoatPlrs = 0;
		for (l=0; l < m_poaBoatPolar->size(); l++)
		{
			pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(l);

			if (pBoatPolar->m_Ctrl.size() && pBoatPolar->m_bIsVisible)
			{
					BoatPlrs++;
			}
		}

		if (BoatPlrs)
		{
			int YPos = place.y() + (ny+BoatPlrs+2) * ypos;// bottom line of this foil's legend
			if(abs(bottom) > abs(YPos))
			{
				ny++;
				painter.drawText(place.x(), place.y() + ypos*ny-(int)(ypos/2), str.at(k));
			}
			else
			{
				// move rigth if outside screen
				place.rx() += LegendWidth;
				ny=1;
				painter.setPen(TextPen);
				painter.drawText(place.x() , place.y() + ypos*ny-(int)(ypos/2), str.at(k));
			}

			for (int nc=0; nc<m_poaBoatPolar->size(); nc++)
			{
				pBoatPolar = (BoatPolar*)m_poaBoatPolar->at(nc);
				if(str.at(k) == pBoatPolar->m_BoatName)
				{
					if (pBoatPolar->m_Ctrl.size() && pBoatPolar->m_bIsVisible)
					{
						LegendPen.setColor(pBoatPolar->m_Color);
						LegendPen.setStyle(GetStyle(pBoatPolar->m_Style));
						LegendPen.setWidth(pBoatPolar->m_Width);
						painter.setPen(LegendPen);

						painter.drawLine(place.x() + (int)(0.5*LegendSize), place.y() + (int)(1.*ypos*ny),
										 place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*ypos*ny));

						if(pBoatPolar->m_bShowPoints)
						{
							x1 = place.x() + (int)(1.0*LegendSize);
							painter.drawRect(x1-2, place.y()-2 + (int)(1.*ypos*ny), 4, 4);
						}
						painter.setPen(TextPen);
						painter.drawText(place.x() + (int)(2.0*LegendSize),
										 place.y() + (int)(1.*ypos*ny)+(int)(ypos/3), pBoatPolar->m_BoatPolarName);
						ny++ ;
					}
				}
			}
			if(BoatPlrs) ny++;
		}
	}
	painter.restore();
}


QGraph* QSail7::GetGraph(QPoint &pt)
{
	//
	// returns a pointer to the graph in which the point pt lies
	//
	if(m_iView==SAILPOLARVIEW)
	{
		if(m_iBoatPlrView==1)
		{
			return m_pCurGraph;
		}
		else if (m_iBoatPlrView==2)
		{
			if(m_BoatGraph[0].IsInDrawRect(pt)){return m_BoatGraph;}
			if(m_BoatGraph[1].IsInDrawRect(pt)){return m_BoatGraph+1;}
			return NULL;
		}
		else //if (m_iBoatPlrView==4)
		{
			for(int ig=0; ig<4; ig++)
				if(m_BoatGraph[ig].IsInDrawRect(pt)) {return m_BoatGraph+ig;}
		}
	}
	return NULL;
}


void QSail7::StopAnimate()
{
	m_bAnimateBoatOpp = false;
	m_pctrlBOppAnimate->setChecked(false);
	m_pTimerBoatOpp->stop();

//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_bAnimateBoatOpp) return;
	SetBoatOpp(true);
}



void QSail7::OnGraphSettings()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QGraph *pGraph = NULL;

	pGraph = m_pCurGraph;
	if(!pGraph) return;

	GraphDlg GDlg;

	if(m_iView==SAILPOLARVIEW)
	{
		GDlg.m_iGraphType = 72;
	}

	QGraph graph;
	graph.CopySettings(pGraph);
	GDlg.move(pMainFrame->m_DlgPos);
	GDlg.m_pMemGraph = &graph;
	GDlg.m_pGraph = pGraph;
	GDlg.SetParams();

	if(GDlg.exec() == QDialog::Accepted)
	{
		if(m_iView==SAILPOLARVIEW)
		{
			SetBoatPolarGraphTitles(pGraph);

			if(GDlg.m_bVariableChanged)
			{
				pGraph->SetAuto(true);
				pGraph->SetAutoYMinUnit(true);
			}
			CreateBoatPolarCurves();
		}
	}
	else
	{
		pGraph->CopySettings(&graph);
	}
	pMainFrame->m_DlgPos = GDlg.pos();
	UpdateView();
}


void QSail7::OnResetBoatPlrLegend()
{
	SetBoatPlrLegendPos();
	UpdateView();
}


void QSail7::GLCreateStreamLines()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurBoatOpp || !m_pCurBoatPolar || !m_pCurBoat) return;

//	GL3DScales *p3DScales = (GL3DScales *)m_pGL3DScales;
	bool bFound;
	int i;
	int m, p, style, width;
	double ds, *Mu, *Sigma;
	QColor color;
	CVector O(0.0,0.0,0.0);
	CVector C, D, D1, VA, VT, VInf, TC;
	D1.Set(987654321.0, 0.0, 0.0);

	QList <int> iStream;
	QList <CVector> VStream;

	if(GL3DScales::s_pos==YLINE)
	{
		for(int iy=0; iy<GL3DScales::s_NStreamLines; iy++)
		{
			VStream.append(CVector(GL3DScales::s_XOffset, GL3DScales::s_YOffset + GL3DScales::s_StreamlineSpacing*(double)iy, GL3DScales::s_ZOffset));
			iStream.append(iy);
		}
	}
	else if(GL3DScales::s_pos==ZLINE)
	{
		for(int iz=0; iz<GL3DScales::s_NStreamLines; iz++)
		{
			VStream.append(CVector(GL3DScales::s_XOffset, GL3DScales::s_YOffset, GL3DScales::s_ZOffset + GL3DScales::s_StreamlineSpacing*(double)iz));
			iStream.append(iz);
		}
	}
	else
	{
		if(GL3DScales::s_pos==LEADINGEDGE)
		{
			for (p=0; p<m_MatSize; p++)
			{
				if(s_pPanel[p].m_bIsLeading && s_pPanel[p].m_Pos<=MIDSURFACE)
				{
					bFound = false;
					for(int i=0; i<iStream.size(); i++)
					{
						if(iStream.at(i)==s_pPanel[p].m_iLA)
						{
							bFound = true;
							break;
						}
					}
					if(!bFound) iStream.append(s_pPanel[p].m_iLA);

					bFound = false;
					for(int i=0; i<iStream.size(); i++)
					{
						if(iStream.at(i)==s_pPanel[p].m_iLB)
						{
							bFound = true;
							break;
						}
					}
					if(!bFound) iStream.append(s_pPanel[p].m_iLB);
				}
			}
		}
		else if(GL3DScales::s_pos==TRAILINGEDGE)
		{
			for (p=0; p<m_MatSize; p++)
			{
				if(s_pPanel[p].m_bIsTrailing && s_pPanel[p].m_Pos<=MIDSURFACE)
				{
					bFound = false;
					for(int i=0; i<iStream.size(); i++)
					{
						if(iStream.at(i)==s_pPanel[p].m_iTA)
						{
							bFound = true;
							break;
						}
					}
					if(!bFound) iStream.append(s_pPanel[p].m_iTA);

					bFound = false;
					for(int i=0; i<iStream.size(); i++)
					{
						if(iStream.at(i)==s_pPanel[p].m_iTB)
						{
							bFound = true;
							break;
						}
					}
					if(!bFound) iStream.append(s_pPanel[p].m_iTB);
				}
			}
		}
	}

	ProgressDlg dlg;
	dlg.setWindowTitle("Streamines calculation");
	dlg.InitDialog(0, iStream.size());
	dlg.setWindowModality(Qt::WindowModal);
	dlg.SetValue(0);
	dlg.move(pMainFrame->m_DlgPos);
	dlg.show();

	Mu    = m_pCurBoatOpp->m_G;
	Sigma = m_pCurBoatOpp->m_Sigma;

	m_PanelDlg.m_MatSize = m_pCurBoatOpp->m_NVLMPanels;
	m_PanelDlg.m_pBoat = m_pCurBoat;

	//Define the freestream wind vector
	double beta = m_pCurBoatPolar->m_BetaMin * (1-m_pCurBoatOpp->m_Ctrl) +m_pCurBoatPolar->m_BetaMax * m_pCurBoatOpp->m_Ctrl ;
	beta *= PI/180.0;
	VInf.Set(m_pCurBoatOpp->m_QInf*cos(beta), m_pCurBoatOpp->m_QInf*sin(beta), 0.0);

	//Apply the currently selected Boat's Opp angles
	m_PanelDlg.SetAngles(m_pCurBoatPolar, m_pCurBoatOpp->m_Ctrl, false);

//________________________________

	glNewList(STREAMLINES,GL_COMPILE);
	{
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);

		color = W3dPrefsDlg::s_StreamLinesColor;
		style = W3dPrefsDlg::s_StreamLinesStyle;
		width = W3dPrefsDlg::s_StreamLinesWidth;

		glLineWidth(width);

		if     (style == Qt::DashLine)       glLineStipple (1, 0xCFCF);
		else if(style == Qt::DotLine)        glLineStipple (1, 0x6666);
		else if(style == Qt::DashDotLine)    glLineStipple (1, 0xFF18);
		else if(style == Qt::DashDotDotLine) glLineStipple (1, 0x7E66);
		else                                 glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		m = 0;

		for (int is=0; is<iStream.size(); is++)
		{
			if(GL3DScales::s_pos==YLINE)      C = VStream.at(is);
			else if(GL3DScales::s_pos==ZLINE) C = VStream.at(is);
			else                              C = s_pNode[iStream.at(is)];

			if(GL3DScales::s_pos==TRAILINGEDGE && fabs(GL3DScales::s_XOffset)<0.001 && fabs(GL3DScales::s_ZOffset)<0.001)
			{
//				VA = m_pCurBoatOpp->GetWindDirection();
//				VA.Normalize();
				//The initial velocity vector is the direction at the T.E., i.e. the bisector angle of the two panels
				bFound =false;
				for(int iSail=0; iSail<m_pCurBoat->m_poaSail.size(); iSail++)
				{
					QSail *pSail = (QSail*)m_pCurBoat->m_poaSail.at(iSail);
					for(int pp=0; pp<pSail->m_NElements; pp++)
					{
						if(pSail->m_pPanel[pp].m_iTA == iStream.at(is))
						{
							VA = s_pNode[pSail->m_pPanel[pp].m_iTA] - s_pNode[pSail->m_pPanel[pp].m_iLA];
							VA.Normalize();
							bFound=true;
							break;
						}
						if(pSail->m_pPanel[pp].m_iTB == iStream.at(is))
						{
							VA = s_pNode[pSail->m_pPanel[pp].m_iTB] - s_pNode[pSail->m_pPanel[pp].m_iLB];
							VA.Normalize();
							bFound=true;
							break;
						}
					}
					if(bFound) break;
				}
			}

			C.x += GL3DScales::s_XOffset;
			C.z += GL3DScales::s_ZOffset;

			ds = GL3DScales::s_DeltaL;

			glBegin(GL_LINE_STRIP);
			{
				glVertex3d(C.x+TC.x, C.y+TC.y, C.z+TC.z);
				C   += VA *ds;
				glVertex3d(C.x+TC.x, C.y+TC.y, C.z+TC.z);
				ds *= GL3DScales::s_XFactor;

				for (i=1; i<GL3DScales::s_NX; i++)
				{
					m_PanelDlg.GetSpeedVector(C, Mu, Sigma, VT);

					VT += VInf;
					VT.Normalize();
					C   += VT* ds;
					glVertex3d(C.x+TC.x, C.y+TC.y, C.z+TC.z);
					ds *= GL3DScales::s_XFactor;
				}
			}
			glEnd();

			dlg.SetValue(m);
			m++;

			if(dlg.IsCanceled()) break;

		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

	//restore panels.
	memcpy(s_pPanel, s_pMemPanel, m_MatSize * sizeof(CPanel));
	memcpy(s_pNode,  s_pMemNode,  m_nNodes  * sizeof(CVector));
	//	memcpy(s_pWakePanel, s_pRefWakePanel, m_WakeSize * sizeof(CPanel));
	//	memcpy(s_pWakeNode,  s_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));
}


void QSail7::GLCreateSurfSpeeds()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pCurBoatOpp || !m_pCurBoatPolar || !m_pCurBoat) return;

	ProgressDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.InitDialog(0, m_MatSize);
	dlg.setModal(true);
	dlg.SetValue(0);
	dlg.show();

	int p, style;
	double factor;
	double length, sinT, cosT, beta;
	double *Mu, *Sigma;
	double x1, x2, y1, y2, z1, z2, xe, ye, ze, dlx, dlz;
	CVector C, V, VT, VInf;

	factor = GL3DScales::s_VelocityScale/100.0;

	Mu    = m_pCurBoatOpp->m_G;
	Sigma = m_pCurBoatOpp->m_Sigma;

	m_PanelDlg.m_pBoat       = m_pCurBoat;
	m_PanelDlg.m_MatSize     = m_pCurBoatOpp->m_NVLMPanels;
	m_PanelDlg.m_pBoatPolar  = m_pCurBoatPolar;
	m_PanelDlg.m_MatSize     = m_MatSize;
	m_PanelDlg.m_WakeSize    = m_WakeSize;
	m_PanelDlg.m_nNodes      = m_nNodes;


	//Define the freestream wind vector
	beta = m_pCurBoatPolar->m_BetaMin * (1-m_pCurBoatOpp->m_Ctrl) +m_pCurBoatPolar->m_BetaMax * m_pCurBoatOpp->m_Ctrl ;
	beta *= PI/180.0;
	VInf.Set(m_pCurBoatOpp->m_QInf*cos(beta), m_pCurBoatOpp->m_QInf*sin(beta), 0.0);

	//Apply the currently selected Boat's Opp angles
	m_PanelDlg.SetAngles(m_pCurBoatPolar, m_pCurBoatOpp->m_Ctrl, false);


	glNewList(SURFACESPEEDS, GL_COMPILE);
	{
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		glLineWidth(W3dPrefsDlg::s_WakeWidth);
		style = W3dPrefsDlg::s_WakeStyle;

		if     (style == Qt::DashLine) 	     glLineStipple (1, 0xCFCF);
		else if(style == Qt::DotLine)        glLineStipple (1, 0x6666);
		else if(style == Qt::DashDotLine)    glLineStipple (1, 0xFF18);
		else if(style == Qt::DashDotDotLine) glLineStipple (1, 0x7E66);
		else                                 glLineStipple (1, 0xFFFF);

		glColor3d(W3dPrefsDlg::s_WakeColor.redF(), W3dPrefsDlg::s_WakeColor.greenF(), W3dPrefsDlg::s_WakeColor.blueF());

		for (p=0; p<m_MatSize; p++)
		{			
			VT = m_PanelDlg.m_VInf;

			if(s_pPanel[p].m_Pos==MIDSURFACE) C.Copy(s_pPanel[p].CtrlPt);
			else                              C.Copy(s_pPanel[p].CollPt);

			m_PanelDlg.GetSpeedVector(C, Mu, Sigma, V, true, false);
			VT += V;

			length = VT.VAbs()*factor;
			xe     = C.x+factor*VT.x;
			ye     = C.y+factor*VT.y;
			ze     = C.z+factor*VT.z;
			if(length>0.0)
			{
				cosT   = (xe-C.x)/length;
				sinT   = (ze-C.z)/length;
				dlx     = 0.15*length;
				dlz     = 0.07*length;
				beta   = atan((ye-C.y)/length)*180.0/PI;
			}
			else
			{
				cosT   = 0.0;
				sinT   = 0.0;
				dlx    = 0.0;
				dlz    = 0.0;
			}

			x1 = xe -dlx*cosT - dlz*sinT;
			y1 = ye;
			z1 = ze -dlx*sinT + dlz*cosT;

			x2 = xe -dlx*cosT + dlz*sinT;
			y2 = ye;
			z2 = ze -dlx*sinT - dlz*cosT;

			glBegin(GL_LINES);
			{
				glVertex3d(C.x, C.y, C.z);
				glVertex3d(xe,ye,ze);
			}
			glEnd();

			glBegin(GL_LINES);
			{
				glVertex3d(xe, ye, ze);
				glVertex3d(x1, y1, z1);
			}
			glEnd();

			glBegin(GL_LINES);
			{
				glVertex3d(xe, ye, ze);
				glVertex3d(x2, y2, z2);
			}
			glEnd();

			dlg.SetValue(p);
			if(dlg.IsCanceled()) break;
		}

		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

	//leave things as they were
	memcpy(s_pPanel, s_pMemPanel, m_MatSize * sizeof(CPanel));
	memcpy(s_pNode,  s_pMemNode,  m_nNodes  * sizeof(CVector));
//	memcpy(s_pWakePanel, s_pRefWakePanel, m_WakeSize * sizeof(CPanel));
//	memcpy(s_pWakeNode,  s_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));

	dlg.hide();
}



void QSail7::OnBoatPolarProps()
{
	if(!m_pCurBoatPolar) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ObjectPropsDlg dlg;
	dlg.m_pBoatPolar = m_pCurBoatPolar;
	dlg.InitDialog();
	dlg.move(pMainFrame->m_DlgPos);
	dlg.exec();
	pMainFrame->m_DlgPos = dlg.pos();
}



void QSail7::OnBoatOppProps()
{
	if(!m_pCurBoatOpp) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ObjectPropsDlg dlg;
	dlg.m_pBoatOpp = m_pCurBoatOpp;
	dlg.InitDialog();
	dlg.move(pMainFrame->m_DlgPos);
	dlg.exec();
	pMainFrame->m_DlgPos = dlg.pos();
}


void QSail7::GLDrawForces()
{
	if(!m_pCurBoatOpp) return;
	CVector WindDirection, WindNormal, WindSide, Pt;
	double Lift, Drag;

	int style=W3dPrefsDlg::s_XCPStyle;

	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strForce, strForceUnit;

	GetForceUnit(strForceUnit, pMainFrame->m_ForceUnit);

	glEnable (GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT,GL_LINE);


	double coef = 1./200.0 *GL3DScales::s_LiftScale;


	if(m_bLift)
	{
		glColor3d(W3dPrefsDlg::s_XCPColor.redF(),W3dPrefsDlg::s_XCPColor.greenF(),W3dPrefsDlg::s_XCPColor.blueF());
		glLineWidth(W3dPrefsDlg::s_XCPWidth);

		if     (style == Qt::DashLine)       glLineStipple (1, 0xCFCF);
		else if(style == Qt::DotLine)        glLineStipple (1, 0x6666);
		else if(style == Qt::DashDotLine)    glLineStipple (1, 0xFF18);
		else if(style == Qt::DashDotDotLine) glLineStipple (1, 0x7E66);
		else                                 glLineStipple (1, 0xFFFF);

		m_pCurBoatOpp->GetLiftDrag(Lift, Drag, WindDirection, WindNormal, WindSide);

		p3DWidget->GLDrawArrow(m_pCurBoatPolar->m_CoG, WindNormal,    Lift * coef);
		p3DWidget->GLDrawArrow(m_pCurBoatPolar->m_CoG, WindDirection, Drag * coef);

		glColor3d(pMainFrame->m_TextColor.redF(), pMainFrame->m_TextColor.greenF(), pMainFrame->m_TextColor.blueF());

		Pt = m_pCurBoatPolar->m_CoG +  WindNormal * m_pCurBoatOpp->ForceTrefftz.dot(WindNormal) * coef;
		strForce = QString("Lift=%1").arg(Lift*pMainFrame->m_NtoUnit,7,'f',1);
		strForce += strForceUnit;
		p3DWidget->renderText(Pt.x, Pt.y, Pt.z+.11, strForce, pMainFrame->m_TextFont);

		Pt = m_pCurBoatPolar->m_CoG +  WindDirection * m_pCurBoatOpp->ForceTrefftz.dot(WindDirection) * coef;
		strForce = QString("Drag=%1").arg(Drag*pMainFrame->m_NtoUnit,7,'f',1);
		strForce += strForceUnit;
		p3DWidget->renderText(Pt.x, Pt.y, Pt.z+.11, strForce, pMainFrame->m_TextFont);
	}

	if(m_bBodyForces)
	{
		glColor3d(W3dPrefsDlg::s_MomentColor.redF(),W3dPrefsDlg::s_MomentColor.greenF(),W3dPrefsDlg::s_MomentColor.blueF());
		glLineWidth(W3dPrefsDlg::s_MomentWidth);

		switch(W3dPrefsDlg::s_MomentStyle)
		{
			case Qt::DashLine:
				 glLineStipple (1, 0xCFCF);
				 break;
			case Qt::DotLine:
				 glLineStipple (1, 0x6666);
				 break;
			case Qt::DashDotLine:
				 glLineStipple (1, 0xFF18);
				 break;
			case Qt::DashDotDotLine:
				 glLineStipple (1, 0x7E66);
				 break;
			default:
				 glLineStipple (1, 0xFFFF);
				 break;
		}

		p3DWidget->GLDrawArrow(m_pCurBoatPolar->m_CoG, CVector(1.0,0.0,0.0), m_pCurBoatOpp->F.x* coef);
		p3DWidget->GLDrawArrow(m_pCurBoatPolar->m_CoG, CVector(0.0,1.0,0.0), m_pCurBoatOpp->F.y * coef);
		p3DWidget->GLDrawArrow(m_pCurBoatPolar->m_CoG, CVector(0.0,0.0,1.0), m_pCurBoatOpp->F.z * coef);

		glColor3d(pMainFrame->m_TextColor.redF(), pMainFrame->m_TextColor.greenF(), pMainFrame->m_TextColor.blueF());

		Pt = m_pCurBoatPolar->m_CoG +  CVector(1.0,0.0,0.0) * m_pCurBoatOpp->ForceTrefftz.x * coef;
		strForce = QString("FF_Fx=%1").arg(m_pCurBoatOpp->F.x*pMainFrame->m_NtoUnit,7,'f',1);
		strForce += strForceUnit;
		p3DWidget->renderText(Pt.x, Pt.y, Pt.z+.11, strForce, pMainFrame->m_TextFont);

		Pt = m_pCurBoatPolar->m_CoG +  CVector(0.0, 1.0, 0.0) * m_pCurBoatOpp->ForceTrefftz.y * coef;
		strForce = QString("FF_Fy=%1").arg(m_pCurBoatOpp->F.y*pMainFrame->m_NtoUnit, 7, 'f', 1);
		strForce += strForceUnit;
		p3DWidget->renderText(Pt.x, Pt.y, Pt.z+.11, strForce, pMainFrame->m_TextFont);

		Pt = m_pCurBoatPolar->m_CoG +  CVector(0.0, 0.0, 1.0) * m_pCurBoatOpp->ForceTrefftz.z * coef;
		strForce = QString("FF_Fz=%1").arg(m_pCurBoatOpp->F.z*pMainFrame->m_NtoUnit, 7, 'f', 1);
		strForce += strForceUnit;
		p3DWidget->renderText(Pt.x, Pt.y, Pt.z+.11, strForce, pMainFrame->m_TextFont);
	}

	glDisable (GL_LINE_STIPPLE);
}


void QSail7::GLDrawBoatLegend()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;
	static int dD, ZPos, LeftPos, total;
	QString  strong, str1;
	QString length, surface;

	if(!m_pCurBoat) return;

	QFontMetrics fm(pMainFrame->m_TextFont);
	dD = fm.height();//pixels

	// Write boat data

	total = 4;
	ZPos = p3DWidget->geometry().bottom()- total * dD ;
	LeftPos = p3DWidget->geometry().left() +15;

	int a,b;
	a=8; b=3;

	QSail *pSail =NULL;
	if(m_pCurBoat->m_poaSail.size())
	{
		pSail = (QSail*)m_pCurBoat->m_poaSail.at(0);
	}
	//glNewList(WINGLEGEND,GL_COMPILE);
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());

		GetLengthUnit(length,pMainFrame->m_LengthUnit);
		GetAreaUnit(surface,pMainFrame->m_AreaUnit);

		p3DWidget->renderText(LeftPos, ZPos, m_pCurBoat->m_BoatName, pMainFrame->m_TextFont);
		ZPos +=dD;

		if(pSail)
		{
			str1 = QString(QObject::tr("Luff Length  = %1 ")).arg(pSail->LuffLength()*pMainFrame->m_mtoUnit, a,'f',b);
			strong = str1 + length;
			p3DWidget->renderText(LeftPos, ZPos, QString(strong), pMainFrame->m_TextFont);
			ZPos +=dD;
			str1 = QString(QObject::tr("Leech Length = %1 ")).arg(pSail->LeechLength()*pMainFrame->m_mtoUnit, a,'f',b);
			strong = str1 + length;
			p3DWidget->renderText(LeftPos, ZPos, QString(strong), pMainFrame->m_TextFont);
			ZPos +=dD;
			str1 = QString(QObject::tr("Foot Length  = %1 ")).arg(pSail->FootLength()*pMainFrame->m_mtoUnit, a,'f',b);
			strong = str1 + length;
			p3DWidget->renderText(LeftPos, ZPos, QString(strong), pMainFrame->m_TextFont);
			ZPos +=dD;
		}

	}
	//glEndList();
}



void QSail7::GLDrawBoatOppLegend()
{
	if(!m_pCurBoat || !m_pCurBoatOpp) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ThreeDWidget *p3DWidget = (ThreeDWidget*)s_p3DWidget;
	int dD, YPos, XPos;
	QString Result, strLength, strSpeed, strForce, strMoment;
	int l;
	GetLengthUnit(strLength, pMainFrame->m_LengthUnit);
	GetSpeedUnit(strSpeed, pMainFrame->m_SpeedUnit);
	GetForceUnit(strForce, pMainFrame->m_ForceUnit);
	GetMomentUnit(strMoment, pMainFrame->m_MomentUnit);

	QFontMetrics fm(pMainFrame->m_TextFont);
	dD = fm.height();//pixels

	int nSails = m_pCurBoat->m_poaSail.size();
	YPos = p3DWidget->geometry().bottom()- 12*dD - nSails*dD;
	XPos = p3DWidget->geometry().right() - 10 ;

	//glNewList(BOATOPPLEGEND,GL_COMPILE);
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());

		Result = QString("Control pos. = %1    ").arg(m_pCurBoatOpp->m_Ctrl, 6,'f',2);
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		l = strSpeed.length();
		if     (l==2) Result = QString(QObject::tr("V = %1 ")).arg(m_pCurBoatOpp->m_QInf*pMainFrame->m_mstoUnit,7,'f',3);
		else if(l==3) Result = QString(QObject::tr("V = %1 ")).arg(m_pCurBoatOpp->m_QInf*pMainFrame->m_mstoUnit,6,'f',2);
		else if(l==4) Result = QString(QObject::tr("V = %1 ")).arg(m_pCurBoatOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',1);
		Result += strSpeed;
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		Result = QString("Beta = %1").arg(m_pCurBoatOpp->m_Beta,6,'f',2);
		Result += QString::fromUtf8("°   ");
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		Result = QString("Phi = %1").arg(m_pCurBoatOpp->m_Phi, 6,'f',2);
		Result += QString::fromUtf8("°   ");
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		for(int is=0; is<nSails; is++)
		{
			QSail *pSail = m_pCurBoat->m_poaSail.at(is);
			Result = pSail->m_SailName+ QString(" angle = %1").arg(m_pCurBoatOpp->m_SailAngle[is], 6,'f',2);
			Result += QString::fromUtf8("°   ");
			YPos += dD;
			p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);
		}

		QSail *pSail = m_pCurBoat->m_poaSail.at(0);
		if(pSail)
		{
			ReynoldsFormat(Result, pSail->FootLength() * m_pCurBoatOpp->m_QInf /m_pCurBoatPolar->m_Viscosity);
			Result = QObject::tr("Foot Re = ") + Result +" ";
			YPos += dD;
			p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);
		}


		Result = QString("Fx = %1 ").arg(m_pCurBoatOpp->F.x, 7,'f',2);
		Result += strForce+" ";
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		Result = QString("Fy = %1 ").arg(m_pCurBoatOpp->F.y, 7,'f',2);
		Result += strForce+" ";
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		Result = QString("Fz = %1 ").arg(m_pCurBoatOpp->F.z, 7,'f',2);
		Result += strForce +" ";
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		Result = QString("Mx = %1 ").arg(m_pCurBoatOpp->M.x*pMainFrame->m_NmtoUnit, 6, 'f', 1);
		Result += strMoment;
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		Result = QString("My = %1 ").arg(m_pCurBoatOpp->M.y*pMainFrame->m_NmtoUnit, 6, 'f', 1);
		Result += strMoment;
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);

		Result = QString("Mz = %1 ").arg(m_pCurBoatOpp->M.z*pMainFrame->m_NmtoUnit, 6, 'f', 1);
		Result += strMoment;
		YPos += dD;
		p3DWidget->renderText(XPos-fm.width(Result), YPos, Result, pMainFrame->m_TextFont);
	}
	//glEndList();
}




void QSail7::SnapClient(QString const &FileName)
{
	int NbBytes, bitsPerPixel;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QSize size(m_r3DCltRect.width(),m_r3DCltRect.height());
	ThreeDWidget * p3DWidget = (ThreeDWidget*)s_p3DWidget;
	QGLFormat GLFormat = p3DWidget->format();

	if(!GLFormat.rgba())
	{
		QMessageBox::warning(pMainFrame,tr("Warning"),tr("OpenGL color format is not recognized... Sorry"));
		return;
	}

	bitsPerPixel = 24;
	int width = size.width();
	switch(bitsPerPixel)
	{
		case 8:
		{
			QMessageBox::warning(pMainFrame,tr("Warning"),tr("Cannot (yet ?) save 8 bit depth opengl screen images... Sorry"));
			return;
		}
		case 16:
		{
			QMessageBox::warning(pMainFrame,tr("Warning"),tr("Cannot (yet ?) save 16 bit depth opengl screen images... Sorry"));
			size.setWidth(width - size.width() % 2);
			return;
		}
		case 24:
		{
			NbBytes = 4 * size.width() * size.height();//24 bits type BMP
//			size.setWidth(width - size.width() % 4);
			break;
		}
		case 32:
		{
			NbBytes = 4 * size.width() * size.height();//32 bits type BMP
			break;
		}
		default:
		{
			QMessageBox::warning(pMainFrame,tr("Warning"),tr("Unidentified bit depth... Sorry"));
			return;
		}
	}
	uchar *pPixelData = new uchar[NbBytes];

	// Copy from OpenGL
	glReadBuffer(GL_FRONT);
	switch(bitsPerPixel)
	{
		case 8: return;
		case 16: return;
		case 24:
		{
#if QT_VERSION >= 0x040400
			  glReadPixels(0,0,size.width(),size.height(),GL_RGB,GL_UNSIGNED_BYTE,pPixelData);
			  QImage Image(pPixelData, size.width(),size.height(), QImage::Format_RGB888);
			  QImage FlippedImaged;
			  FlippedImaged = Image.mirrored();	//flip vertically
			  FlippedImaged.save(FileName);
#else
			  QMessageBox::warning(pMainFrame,tr("Warning"),"The version of Qt used to compile the code is older than 4.4 and does not support 24 bit images... Sorry");
#endif
			  break;
		}
		case 32:
		{
			glReadPixels(0,0,size.width(),size.height(),GL_RGBA,GL_UNSIGNED_BYTE,pPixelData);
			QImage Image(pPixelData, size.width(),size.height(), QImage::Format_ARGB32);
			QImage FlippedImaged;
			FlippedImaged = Image.mirrored();	//flip vertically
			FlippedImaged.save(FileName);
			break;
		}
		default: break;
	}
}


void QSail7::UpdateUnits()
{

}


void QSail7::GLCreateCpLegendClr(QRect rect, double LegendMin, double LegendMax)
{
	int i;

//	double ClientToGL;

	double fi, ZPos,dz,Right1, Right2;
	double color = 0.0;

	double w = (double)rect.width();
	double h = (double)rect.height();
	double XPos;

	if(w>h)
	{
		XPos  = 1.0;
		//		ZPos  = h/w * (-1.0 + 2.0/3.0);
		dz    = h/w*1.0/20.0;
		ZPos  = h/w - 23.0*dz;
//		ClientToGL = 2.0/w;
	}
	else
	{
		XPos = w/h;
		//		ZPos  = (-1.0 + 2.0/3.0);
		dz    = 1.0/20.0;
		ZPos  = 1.0 - 23.0*dz;
//		ClientToGL = 2.0/h;
	}

//	dD      = 12.0/w*2.0;

	Right1  = .94*XPos;
	Right2  = .98*XPos;

//	range = (LegendMax - LegendMin);
//	delta = range / 20;


	glNewList(PANELCPLEGENDCOLOR,GL_COMPILE);
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glPolygonMode(GL_FRONT,GL_FILL);

		glBegin(GL_QUAD_STRIP);
		{
			for (i=0; i<=20; i++)
			{
				fi = (double)i*dz;
				color += 0.05;

				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				glVertex3d(Right1, ZPos+fi, 0.0);
				glVertex3d(Right2, ZPos+fi, 0.0);
			}
		}
		glEnd();
	}
	glEndList();
}


void QSail7::GLDrawPanelForceLegend(void *pMainFrame, double rmin, double rmax)
{
	MainFrame *pMF = (MainFrame*)pMainFrame;
	ThreeDWidget *p3DWidget = (ThreeDWidget*)pMF->Get3DWidget();
	int i;
	double labellength, ClientToGL;
	double f, fi, ZPos,dz,Right1;
	double range, delta;
	double XPos, w, h;
	QString strong, strForce;
	QFontMetrics fm(pMF->m_TextFont);

	w = (double)p3DWidget->geometry().width();
	h = (double)p3DWidget->geometry().height();

	GetForceUnit(strForce, pMF->m_ForceUnit);

	range = rmax - rmin;

	if(w>h)
	{
			XPos  = 1.0;
			dz    = h/w*1.0/20.0;
			ZPos  = h/w - 23.0*dz;
			ClientToGL = 2.0/w;
	}
	else
	{
			XPos = w/h;
			dz    = 1.0/20.0;
			ZPos  = 1.0 - 23.0*dz;
			ClientToGL = 2.0/h;
	}

	Right1  = .94*XPos;
//	Right2  = .98*XPos;

	delta = range / 20.0;

	glColor3d(pMF->m_TextColor.redF(), pMF->m_TextColor.greenF(), pMF->m_TextColor.blueF());

	// Draw the labels
	for (i=0; i<=20; i ++)
	{
		f = rmin + (double)i * delta;
		fi = (double)i*dz ;
		strong = QString("%1").arg(f, 5,'g',3);
		labellength = (fm.width(strong)+5) * ClientToGL;
		p3DWidget->renderText(Right1-labellength, ZPos+fi, 0.0, strong, pMF->m_TextFont);
	}
	labellength = (fm.width(strForce)+5) * ClientToGL;
	p3DWidget->renderText(Right1-labellength, ZPos+21.0*dz,  0.0, strForce, pMF->m_TextFont);
}


void GLDrawCpLegend(void *p3DWidget, QRect rect, double LegendMin, double LegendMax, QFont &TextFont, QColor TextColor)
{
	int i;
	ThreeDWidget *pOGL = (ThreeDWidget*)p3DWidget;
	QString strong;

	double labellength, ClientToGL;

	double f, fi, ZPos,dz,Right1;
	double range, delta;

	QFontMetrics fm(TextFont);

	double w = (double)rect.width();
	double h = (double)rect.height();
	double XPos;

	if(w>h)
	{
		XPos  = 1.0;
//		ZPos  = h/w * (-1.0 + 2.0/3.0);
		dz    = h/w*1.0/20.0;
		ZPos  = h/w - 23.0*dz;
		ClientToGL = 2.0/w;
	}
	else
	{
		XPos = w/h;
//		ZPos  = (-1.0 + 2.0/3.0);
		dz    = 1.0/20.0;
		ZPos  = 1.0 - 23.0*dz;
		ClientToGL = 2.0/h;
	}

//	dD      = 12.0/w*2.0;

	Right1  = .94*XPos;
//	Right2  = .98*XPos;

	range = (LegendMax - LegendMin);
	delta = range / 20;


		//glNewList(WOPPCPLEGENDTXT,GL_COMPILE);
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
//		glBegin(GL_LINES);
//		glVertex3d(-1.0, h/w, 0.0);
//		glVertex3d(0.0, 0.0, 0.0);
//		glEnd();

		glPolygonMode(GL_FRONT,GL_LINE);

		glColor3d(TextColor.redF(), TextColor.greenF(), TextColor.blueF());
		// Draw the labels
		for (i=0; i<=20; i ++)
		{
			f = LegendMin + (double)i * delta;
			fi = (double)i*dz;
			strong = QString("%1").arg(f, 5,'f',2);
			labellength = (fm.width(strong)+5) * ClientToGL;
			pOGL->renderText(Right1-labellength, ZPos+fi, 0.0, strong, TextFont);
		}
		strong = "Cp";
		labellength = (fm.width(strong)+5) * ClientToGL;
		pOGL->renderText(Right1-labellength, ZPos+21.0*dz,  0.0, strong, TextFont);
	}
		//glEndList();
}



void QSail7::GLDrawCpLegend(void *p3DWidget, QRect rect, double LegendMin, double LegendMax, QFont &TextFont, QColor TextColor)
{
	int i;
	ThreeDWidget *pOGL = (ThreeDWidget*)p3DWidget;
	QString strong;

	double labellength, ClientToGL;

	double f, fi, ZPos,dz,Right1;
	double range, delta;

	QFontMetrics fm(TextFont);

	double w = (double)rect.width();
	double h = (double)rect.height();
	double XPos;

	if(w>h)
	{
		XPos  = 1.0;
//		ZPos  = h/w * (-1.0 + 2.0/3.0);
		dz    = h/w*1.0/20.0;
		ZPos  = h/w - 23.0*dz;
		ClientToGL = 2.0/w;
	}
	else
	{
		XPos = w/h;
//		ZPos  = (-1.0 + 2.0/3.0);
		dz    = 1.0/20.0;
		ZPos  = 1.0 - 23.0*dz;
		ClientToGL = 2.0/h;
	}

//	dD      = 12.0/w*2.0;

	Right1  = .94*XPos;
//	Right2  = .98*XPos;

	range = (LegendMax - LegendMin);
	delta = range / 20;


		//glNewList(WOPPCPLEGENDTXT,GL_COMPILE);
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
//		glBegin(GL_LINES);
//		glVertex3d(-1.0, h/w, 0.0);
//		glVertex3d(0.0, 0.0, 0.0);
//		glEnd();

		glPolygonMode(GL_FRONT,GL_LINE);

		glColor3d(TextColor.redF(), TextColor.greenF(), TextColor.blueF());
		// Draw the labels
		for (i=0; i<=20; i ++)
		{
			f = LegendMin + (double)i * delta;
			fi = (double)i*dz;
			strong = QString("%1").arg(f, 5,'f',2);
			labellength = (fm.width(strong)+5) * ClientToGL;
			pOGL->renderText(Right1-labellength, ZPos+fi, 0.0, strong, TextFont);
		}
		strong = "Cp";
		labellength = (fm.width(strong)+5) * ClientToGL;
		pOGL->renderText(Right1-labellength, ZPos+21.0*dz,  0.0, strong, TextFont);
	}
		//glEndList();
}


void QSail7::OnExportCurBoatOpp()
{
	if(!m_pCurBoatOpp) return;

	QString Header, strong, Format;
	int k;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, filter;

	if(pMainFrame->m_ExportFileType==1) filter = "Text File (*.txt)";
	else                                filter = "Comma Separated Values (*.csv)";

	FileName = m_pCurBoatOpp->m_BoatName+"_"+m_pCurBoatOpp->m_BoatPolarName+QString("_Ctrl=%1").arg(m_pCurBoatOpp->m_Ctrl,5,'f',2);
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Boat Opp"),
											pMainFrame->m_LastDirName + "/"+FileName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) pMainFrame->m_ExportFileType = 2;
	else       pMainFrame->m_ExportFileType = 1;


	QFile XFile(FileName);
	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);


	if(pMainFrame->m_ExportFileType==1) Header = "       x           y            z          Cp \n";
	else                                Header = "  x,y,z,Cp\n";
	out << Header;

	if(pMainFrame->m_ExportFileType==1) Format = "%1  %2   %3   %4\n";
	else                                Format = "%1,%2,%3,%4\n";

	for (k=0; k<m_MatSize; k++)
	{
		strong = QString(Format)
			.arg(s_pPanel[k].CtrlPt.x,10,'f',4).arg(s_pPanel[k].CtrlPt.y,10,'f',4).arg(s_pPanel[k].CtrlPt.z,10,'f',4)
			.arg(m_pCurBoatOpp->m_Cp[k],9,'f',4);
		out << strong;
	}
	out << "\n\n";

	XFile.close();
}



void QSail7::OnExportCurBoatPolar()
{
	if(!m_pCurBoatPolar) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, filter;

	if(pMainFrame->m_ExportFileType==1) filter = "Text File (*.txt)";
	else                                filter = "Comma Separated Values (*.csv)";

	FileName = m_pCurBoatPolar->m_BoatPolarName;
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Polar"),
											pMainFrame->m_LastDirName + "/"+FileName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) pMainFrame->m_ExportFileType = 2;
	else       pMainFrame->m_ExportFileType = 1;


	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);
	m_pCurBoatPolar->Export(out, pMainFrame->m_ExportFileType);
	XFile.close();

	UpdateView();
}


void QSail7::OnResetCurBoatPolar()
{
	if (!m_pCurBoatPolar) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong = tr("Are you sure you want to reset the content of the polar :\n")+  m_pCurBoatPolar->m_BoatPolarName+"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
												  QMessageBox::Cancel)) return;

	m_pCurBoatPolar->ResetBoatPlr();

	BoatOpp *pBOpp;
	if(m_pCurBoat)
	{
		for(int i=m_poaBoatOpp->size()-1; i>=0; --i)
		{
			pBOpp = (BoatOpp*) m_poaBoatOpp->at(i);
			if(pBOpp->m_BoatPolarName==m_pCurBoatPolar->m_BoatPolarName && pBOpp->m_BoatName==m_pCurBoat->m_BoatName)
			{
				m_poaBoatOpp->removeAt(i);
				delete pBOpp;
			}
		}
	}
	pMainFrame->UpdateBoatOpps();
	m_pCurBoatOpp = NULL;

	if(m_iView==SAILPOLARVIEW)
	{
		CreateBoatPolarCurves();
		if(m_pCurBoatPolar)
		{
			QString PolarProps;
			m_pCurBoatPolar->GetPolarProperties(PolarProps);
			m_pctrlPolarProps1->setText(PolarProps);
		}
	}

	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QSail7::OnHideCurBoatPolars()
{
	if(!m_pCurBoat) return;
	int i;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	BoatPolar *pBPolar;
	for (i=0; i<m_poaBoatPolar->size(); i++)
	{
		pBPolar = (BoatPolar*)m_poaBoatPolar->at(i);
		if(pBPolar->m_BoatName == m_pCurBoat->m_BoatName)	pBPolar->m_bIsVisible = false;
	}

	if(m_iView==SAILPOLARVIEW)		CreateBoatPolarCurves();
	SetCurveParams();
	pMainFrame->SetSaveState(false);
	UpdateView();
}

void QSail7::OnShowCurBoatPolars()
{
	if(!m_pCurBoat) return;
	int i;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	BoatPolar *pBPolar;
	for (i=0; i<m_poaBoatPolar->size(); i++)
	{
		pBPolar = (BoatPolar*)m_poaBoatPolar->at(i);
		if(pBPolar->m_BoatName == m_pCurBoat->m_BoatName)	pBPolar->m_bIsVisible = true;
	}

	if(m_iView==SAILPOLARVIEW)		CreateBoatPolarCurves();
	SetCurveParams();
	pMainFrame->SetSaveState(false);
	UpdateView();
}

void QSail7::OnDeleteCurBoatPolars()
{
	//
	// The user has requested a deletion of all WPolars associated to the wing or plane
	//

	if(!m_pCurBoat) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	BoatPolar *pBPolar;
	QString strong;

	strong = tr("Are you sure you want to delete the polars associated to :\n") +  m_pCurBoat->m_BoatName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	for(int j=m_poaBoatPolar->size()-1; j>=0; j--)
	{
		pBPolar  = (BoatPolar *)m_poaBoatPolar->at(j);
		if(pBPolar && pBPolar->m_BoatName==m_pCurBoat->m_BoatName)
		{
			//first remove all WOpps and POpps associated to the Wing Polar
/*			CWOpp * pWOpp;
			for (int i=m_poaWOpp->size()-1; i>=0; i--)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(i);
				if (pWOpp->m_PlrName == pWPolar->m_PlrName   &&  pWOpp->m_WingName == UFOName)
				{
					m_poaWOpp->removeAt(i);
					delete pWOpp;
				}
			}*/

			//then remove the polar
			m_poaBoatPolar->removeAt(j);
			delete pBPolar;
		}
	}
	m_pCurBoatPolar = NULL;
	SetBoatPolar();
	pMainFrame->UpdateBoatPolars();
	pMainFrame->SetSaveState(false);
	SetControls();
	UpdateView();
}



void QSail7::OnHideAllBoatPolars()
{
	int i;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	BoatPolar *pBPolar;
	for (i=0; i<m_poaBoatPolar->size(); i++)
	{
		pBPolar = (BoatPolar*)m_poaBoatPolar->at(i);
		pBPolar->m_bIsVisible = false;
	}
	if(m_iView==SAILPOLARVIEW)		CreateBoatPolarCurves();
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}



void QSail7::OnShowAllBoatPolars()
{
	int i;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	BoatPolar *pBPolar;
	for (i=0; i<m_poaBoatPolar->size(); i++)
	{
		pBPolar = (BoatPolar*)m_poaBoatPolar->at(i);
		pBPolar->m_bIsVisible = true;
	}
	if(m_iView==SAILPOLARVIEW)		CreateBoatPolarCurves();
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}



void QSail7::OnSingleGraph1()
{
	m_iBoatPlrView = 1;
	m_pCurGraph = m_BoatGraph;
	SetBoatPlrLegendPos();
	UpdateView();
	SetControls();
}

void QSail7::OnSingleGraph2()
{
	m_iBoatPlrView = 1;
	m_pCurGraph = m_BoatGraph+1;
	SetBoatPlrLegendPos();
	UpdateView();
	SetControls();
}

void QSail7::OnSingleGraph3()
{
	m_iBoatPlrView = 1;
	m_pCurGraph = m_BoatGraph+2;
	SetBoatPlrLegendPos();
	UpdateView();
	SetControls();
}

void QSail7::OnSingleGraph4()
{
	m_iBoatPlrView = 1;
	m_pCurGraph = m_BoatGraph+3;
	SetBoatPlrLegendPos();
	UpdateView();
	SetControls();
}



void QSail7::OnTwoGraphs()
{
	m_iBoatPlrView = 2;
	m_pCurGraph = m_BoatGraph;
	SetBoatPlrLegendPos();
	UpdateView();
	SetControls();
}

void QSail7::OnFourGraphs()
{
	m_iBoatPlrView = 4;
	m_pCurGraph = m_BoatGraph;
	SetBoatPlrLegendPos();
	UpdateView();
	SetControls();
}

void QSail7::OnAllBoatPolarGraphSettings()
{
	//
	// The user has requested an edition of the settings for all WPolar graphs
	//
	QGraph graph;
	graph.CopySettings(m_BoatGraph);
	GraphDlg GDlg;
	GDlg.m_pMemGraph = &graph;
	GDlg.m_pGraph    = m_BoatGraph;
	GDlg.m_GraphArray[0] = m_BoatGraph;
	GDlg.m_GraphArray[1] = m_BoatGraph+1;
	GDlg.m_GraphArray[2] = m_BoatGraph+2;
	GDlg.m_GraphArray[3] = m_BoatGraph+3;
	GDlg.m_NGraph = 4;
	GDlg.SetParams();

	if(GDlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		for(int ig=0; ig<4; ig++)
			m_BoatGraph[ig].CopySettings(&graph);
	}
	UpdateView();
}


void QSail7::OnAllBoatPolarGraphScales()
{
	for(int ig=0; ig<4; ig++)
	{
		m_BoatGraph[ig].SetAuto(true);
		m_BoatGraph[ig].ResetXLimits();
		m_BoatGraph[ig].ResetYLimits();
	}
	UpdateView();
}


void QSail7::OnResetGraphLegend()
{
	SetBoatPlrLegendPos();
	UpdateView();
}










