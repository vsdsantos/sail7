#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include "../objects/Sail.h"
#include "../threedwidget.h"
#include <QCheckBox>
#include <QAction>
#include <QToolButton>
#include <QWidget>


#define SAILCTRLPOINTS 3322

class SailViewWidget : public QWidget
{
	Q_OBJECT

public:
	SailViewWidget();

private slots:
	void OnAxes();
	void OnLight();
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DReset();
	void On3DPickCenter();
	void OnPanels();
	void OnSurfaces();
	void OnOutline();
	void OnCtrlPoints();

public:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *event);
	void hideEvent(QHideEvent *event);
	void showEvent(QShowEvent *event);

	void Connect();
	void GLDraw3D();
	void GLInverseMatrix();
	void GLRenderSail();
	void GLCreateSailGeom();
	void GLCreateSectionHighlight();
	void GLCreateSailMesh();
	void GLCreateCtrlPoints();
	void InitDialog(QSail *pSail);
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void Set3DScale();
	void SetupLayout();
	void UncheckPresetViews();
	void UpdateView();

	static void *s_pMainFrame;
	static void *s_pSail7;
	static void *s_pSailDlg;

	static QPoint s_WindowPos;
	static QSize  s_WindowSize;
	static bool s_bWindowMaximized;

	ThreeDWidget *m_p3DWidget;


	QCheckBox *m_pctrlAxes, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPoints, *m_pctrlPanels, *m_pctrlLight;
	QAction *m_pXView, *m_pYView, *m_pZView, *m_pIsoView;
	QToolButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso;
	QPushButton *m_pctrlReset, *m_pctrlPickCenter;


	ArcBall m_ArcBall;
	CVector m_MousePos;
	QPoint m_ViewportTrans;
	double m_Scale, m_RefScale, m_ScaleY;

	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector

	static bool s_bAxes, s_bSurfaces, s_bOutline, s_bCtrlPoints, s_bVLMPanels, s_bglLight;

	double m_glScaled;
	double MatIn[4][4], MatOut[4][4];
	CVector m_SailOffset;


	QPoint m_ptPopUp;
	QPoint m_LastPoint, m_PointDown;
	CVector m_RealPopUp;


	QSail *m_pSail;

	int m_GLList;


	bool m_bTrans;

	bool m_bCrossPoint, m_bArcBall;
	bool m_bIs2DScaleSet, m_bIs3DScaleSet;
	bool m_bPickCenter;
	bool m_bResetglArcball, m_bResetglGeom, m_bResetglSailMesh;
	bool m_bResetglSectionHighlight;
	bool m_bResetglCtrlPoints;

};

#endif // VIEWWINDOW_H
