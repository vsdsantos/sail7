/****************************************************************************

	ThreeDWidget Class
	Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
	(C) All rights reserved

*****************************************************************************/


#ifndef THREEDWIDGET_H
#define THREEDWIDGET_H

#include <QGLWidget>
#include "params.h"
#include "objects/ArcBall.h"
#include "objects/CRectangle.h"
#include "misc/GLLightDlg.h"

class ThreeDWidget : public QGLWidget
{
    Q_OBJECT

	friend class Sail7;
	friend class BoatDlg;
	friend class MainFrame;
	friend class ArcBall;
	friend class GL3dBodyDlg;
	friend class SailViewWidget;

public:
	ThreeDWidget(QWidget *parent = 0);


private:
	void paintEvent(QPaintEvent *event);
	void initializeGL();
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeGL(int width, int height);
	void paintGL();//virtual override
	void contextMenuEvent (QContextMenuEvent * event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);
	void wheelEvent (QWheelEvent *event );

	void ClientToGL(QPoint const &point, Vector3d &real);
	void GLToClient(Vector3d const &real, QPoint &point);
	void GLInverseMatrix(double MatIn[][4], double MatOut[][4]);

	void GLDrawArrow(Vector3d O, Vector3d u, double length);
	void GLDrawCube(Vector3d Pt, double side);
	void GLDrawCylinder(QColor cr, double rbase, double rtop, double hbase, double htop, int NumArcs, int NumHeight);
	void GLCreateCylinderList(int GLList, QColor cr, double rbase, double rtop, double hbase, double htop, int NumArcs, int NumHeight);
	void GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes);
	void GLCreateSphereList(int GLList, double radius, int NumLongitudes, int NumLatitudes);
	void GLCreateCubeList(int GLList, double side);
	void GLSetupLight(GLLightDlg *pglLightParams, double Offset_y, double LightFactor=1.0f);
	void GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth);
	void CreateArcballList(ArcBall &ArcBall, double GLScale);
	void NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3]);
	void setupViewPort(int width, int height);

private slots:

private:
	void *m_pParent;
	static void *s_pSail7;
	static void *s_pMainFrame;

//	QRect m_rCltRect;        // The client window rectangle
	CRectangle m_GLViewRect; // The OpenGl Viewport

	int m_iView;
};

#endif //THREEDWIDGET_H
