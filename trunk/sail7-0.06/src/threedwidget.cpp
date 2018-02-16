/****************************************************************************

	ThreeDWidget Class
	Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
	(C) All rights reserved

*****************************************************************************/


#include <QtOpenGL>
#include <math.h>
#include <QtDebug>
#include "mainframe.h"
#include "sail7/sail7.h"
#include "sail7/SailViewWidget.h"
#include "sail7/GL3dBodyDlg.h"
#include "threedwidget.h"

void *ThreeDWidget::s_pSail7 = NULL;
void *ThreeDWidget::s_pMainFrame = NULL;


ThreeDWidget::ThreeDWidget(QWidget *parent)
	: QGLWidget(parent)
{
	m_pParent = parent;
	m_iView = GLSAIL7VIEW;
	setAutoFillBackground(false);

	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}


void ThreeDWidget::contextMenuEvent (QContextMenuEvent * event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QPoint ScreenPt = event->globalPos();
	if(s_pSail7)
	{
		if(m_iView == GLSAIL7VIEW)
		{
			pMainFrame->Sail3DCtxMenu->exec(ScreenPt);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->ShowContextMenu(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
//			ViewWindow *pDlg = (ViewWindow*)m_pParent;
//			pDlg->ShowContextMenu(event);
		}
	}
}


void ThreeDWidget::mousePressEvent(QMouseEvent *event)
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(s_pSail7)
	{
		if(m_iView == GLSAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->mousePressEvent(event);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->mousePressEvent(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
			SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
			pDlg->mousePressEvent(event);
		}
	}
}


void ThreeDWidget::mouseReleaseEvent(QMouseEvent *event)
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(s_pSail7)
	{
		if(m_iView == GLSAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->mouseReleaseEvent(event);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->mouseReleaseEvent(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
			SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
			pDlg->mouseReleaseEvent(event);
		}
	}
}


void ThreeDWidget::mouseMoveEvent(QMouseEvent *event)
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(s_pSail7)
	{
		if(m_iView==GLSAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->mouseMoveEvent(event);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->mouseMoveEvent(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
			SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
			pDlg->mouseMoveEvent(event);
		}
	}
}



void ThreeDWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(s_pSail7)
	{
		if(m_iView == GLSAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->mouseDoubleClickEvent(event);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->mouseDoubleClickEvent(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
			SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
			pDlg->mouseDoubleClickEvent(event);
		}
	}
}


void ThreeDWidget::wheelEvent(QWheelEvent *event)
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(s_pSail7)
	{
		if(m_iView == GLSAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->wheelEvent(event);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->wheelEvent(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
			SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
			pDlg->wheelEvent(event);
		}
	}
}


void ThreeDWidget::initializeGL()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	glClearColor(pMainFrame->m_BackgroundColor.redF(), pMainFrame->m_BackgroundColor.greenF(), pMainFrame->m_BackgroundColor.blueF(),0.0);
}


void ThreeDWidget::keyPressEvent(QKeyEvent *event)
{
	if(s_pSail7)
	{
		if(m_iView == GLSAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->keyPressEvent(event);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->keyPressEvent(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
			SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
			pDlg->keyPressEvent(event);
		}
	}
}


void ThreeDWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(s_pSail7)
	{
		if(m_iView == GLSAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->keyReleaseEvent(event);
		}
		else  if(m_iView ==GLBODYVIEW)
		{
			GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
			pDlg->keyReleaseEvent(event);
		}
		else if (m_iView==GLSAILVIEW)
		{
/*			ViewWindow *pDlg = (ViewWindow*)m_pParent;
			pDlg->keyReleaseEvent(event);*/
		}
	}
}


/**
* Overrides the virtual paintEvent method of the base class.
* Calls the paintGL() method, then overlays the text using the widget's QPainter
* @param event
*/
void ThreeDWidget::paintEvent(QPaintEvent *event)
{
	paintGL();
	QPainter painter(this);

	if(m_iView==GLSAIL7VIEW)
	{
		QSail7* pSail7 = (QSail7*)s_pSail7;

		if(pSail7->m_bResetTextLegend)
		{
			pSail7->DrawTextLegend();
		}

//		painter.setBackgroundMode(Qt::TransparentMode);
//		painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
//		painter.setOpacity(1);
		painter.drawPixmap(0,0, pSail7->m_PixText);
	}

	event->accept();
}



void ThreeDWidget::paintGL()
{
	qreal pixelRatio = 1;
#if QT_VERSION >= 0x050000
	pixelRatio = devicePixelRatio();
#endif
	setupViewPort(width() * pixelRatio, height() * pixelRatio);

	if(m_iView==GLSAIL7VIEW)
	{
		QSail7* pSail7 = (QSail7*)s_pSail7;
		pSail7->GLDraw3D();
		if(pSail7->m_iView==SAIL3DVIEW) pSail7->GLRenderView();
	}
	else  if(m_iView ==GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->GLDraw3D();
		pDlg->GLRenderBody();
	}
	else if (m_iView==GLBOATVIEW)
	{
	}
	else if (m_iView==GLSAILVIEW)
	{
		SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
		pDlg->GLDraw3D();
		pDlg->GLRenderSail();
	}
}




void ThreeDWidget::setupViewPort(int width, int height)
{
	makeCurrent();
	int side = qMax(width, height);
#ifdef Q_WS_MAC
	glViewport(0,0, width, height);
#else
	glViewport((width - side) / 2, (height - side) / 2, side, side);
#endif

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double s = 1.0;
#ifdef Q_WS_MAC
	glOrtho(-s,s,-(height*s)/width,(height*s)/width,-100.0*s,100.0*s);
#else
	glOrtho(-s,s,-s,s,-100.0*s,100.0*s);
#endif
	glMatrixMode(GL_MODELVIEW);
}


void ThreeDWidget::resizeGL(int width, int height)
{	
	/*
	double w, h;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int side = qMax(width, height);
	w = (double)width;
	h = (double)height;

	glViewport((width - side) / 2, (height - side) / 2, side, side);
//	d = qMax(w,h);
//	glViewport(0,0, d, d);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double s = 1.0;
	glOrtho(-s,s,-s,s,-100.0*s,100.0*s);
//	glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(w>h)	m_GLViewRect.SetRect(-s, s*h/w, s, -s*h/w);
	else    m_GLViewRect.SetRect(-s*w/h, s, s*w/h, -s*h/w);*/

	double w, h, s;
	w = (double)width;
	h = (double)height;
	s = 1.0;


	setupViewPort(width, height);

	glLoadIdentity();
	if(w>h)	m_GLViewRect.SetRect(-s, s*h/w, s, -s*h/w);
	else    m_GLViewRect.SetRect(-s*w/h, s, s*w/h, -s*h/w);


	if(s_pSail7 && m_iView == GLSAIL7VIEW)
	{
		QSail7* pSail7 = (QSail7*)s_pSail7;
		pSail7->m_ArcBall.GetMatrix();
//		pSail7->SetScale(m_rCltRect);
		pSail7->m_bIs3DScaleSet = false;
		pSail7->Set3DScale();
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->m_bIs3DScaleSet = false;
		pDlg->SetRectangles();
		pDlg->SetBodyLineScale();
		pDlg->SetFrameScale();
		pDlg->SetBodyScale();
	}
	else if(m_iView==GLSAILVIEW)
	{
/*		SailViewWidget *pDlg = (SailViewWidget*)m_pParent;
		pDlg->m_bIs3DScaleSet = false;
		pDlg->Set3DScale();*/
	}
}



void ThreeDWidget::GLSetupLight(GLLightDlg *pglLightParams, double Offset_y, double LightFactor)
{
	//
	// Sets the light parameters for the OpenGl display
	//

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // the ambient light
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

	float fLightAmbient0[4];
	float fLightDiffuse0[4];
	float fLightSpecular0[4];
	float fLightPosition0[4];

	if(LightFactor>1.0) LightFactor = 1.0f;

	LightFactor = .1f;

	fLightDiffuse0[0] = LightFactor*pglLightParams->s_Diffuse * pglLightParams->s_Red; // red component
	fLightDiffuse0[1] = LightFactor*pglLightParams->s_Diffuse * pglLightParams->s_Green; // green component
	fLightDiffuse0[2] = LightFactor*pglLightParams->s_Diffuse * pglLightParams->s_Blue; // blue component
	fLightDiffuse0[3] = 1.0; // alpha

	// the ambient light conditions.
	fLightAmbient0[0] = LightFactor*pglLightParams->s_Ambient * pglLightParams->s_Red; // red component
	fLightAmbient0[1] = LightFactor*pglLightParams->s_Ambient * pglLightParams->s_Green; // green component
	fLightAmbient0[2] = LightFactor*pglLightParams->s_Ambient * pglLightParams->s_Blue; // blue component
	fLightAmbient0[3] = 1.0; // alpha


	fLightSpecular0[0] = LightFactor*pglLightParams->s_Specular * pglLightParams->s_Red; // red component
	fLightSpecular0[1] = LightFactor*pglLightParams->s_Specular * pglLightParams->s_Green; // green component
	fLightSpecular0[2] = LightFactor*pglLightParams->s_Specular * pglLightParams->s_Blue; // blue component
	fLightSpecular0[3] = 1.0; // alpha

	// And finally, its position

	fLightPosition0[0] = (GLfloat)((pglLightParams->s_XLight));
	fLightPosition0[1] = (GLfloat)((pglLightParams->s_YLight + Offset_y));
	fLightPosition0[2] = (GLfloat)((pglLightParams->s_ZLight));
	fLightPosition0[3] = 1.0; // W (positional light)



	// Enable the basic light
	glLightfv(GL_LIGHT0, GL_AMBIENT,  fLightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  fLightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecular0);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition0);

	float fMatAmbient[4]   = {pglLightParams->s_MatAmbient,  pglLightParams->s_MatAmbient,   pglLightParams->s_MatAmbient,  1.0f};
	float fMatSpecular[4]  = {pglLightParams->s_MatSpecular, pglLightParams->s_MatSpecular,  pglLightParams->s_MatSpecular, 1.0f};
	float fMatDiffuse[4]   = {pglLightParams->s_MatDiffuse,  pglLightParams->s_MatDiffuse,   pglLightParams->s_MatDiffuse,  1.0f};
	float fMatEmission[4]  = {pglLightParams->s_MatEmission, pglLightParams->s_MatEmission,  pglLightParams->s_MatEmission, 1.0f};


	if(pglLightParams->s_bColorMaterial)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//		glColorMaterial(GL_FRONT, GL_AMBIENT);
//		glColorMaterial(GL_FRONT, GL_DIFFUSE);
//		glColorMaterial(GL_FRONT, GL_SPECULAR);

	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);

	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  fMatSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   fMatAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   fMatDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  fMatEmission);
	glMateriali( GL_FRONT_AND_BACK, GL_SHININESS, pglLightParams->s_iMatShininess);

	if(pglLightParams->s_bDepthTest)  glEnable(GL_DEPTH_TEST);     else glDisable(GL_DEPTH_TEST);
	if(pglLightParams->s_bCullFaces)  glEnable(GL_CULL_FACE);      else glDisable(GL_CULL_FACE);
	if(pglLightParams->s_bSmooth)     glEnable(GL_POLYGON_SMOOTH); else glDisable(GL_POLYGON_SMOOTH);
	if(pglLightParams->s_bShade)      glShadeModel(GL_SMOOTH);     else glShadeModel(GL_FLAT);

	if(pglLightParams->s_bLocalView) glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	else                           glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}




void ThreeDWidget::GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth)
{
	//
	// Draws the axis in the OpenGL view
	//

	MainFrame * pMainFrame =(MainFrame*)s_pMainFrame;
	double l = .6*length;

	glPolygonMode(GL_FRONT,GL_LINE);
	glLineWidth((GLfloat)(AxisWidth));

	glColor3d(AxisColor.redF(),AxisColor.greenF(),AxisColor.blueF());

	glEnable (GL_LINE_STIPPLE);

	if     (AxisStyle == Qt::DashLine)       glLineStipple(1, 0xCFCF);
	else if(AxisStyle == Qt::DotLine)        glLineStipple(1, 0x6666);
	else if(AxisStyle == Qt::DashDotLine)    glLineStipple(1, 0xFF18);
	else if(AxisStyle == Qt::DashDotDotLine) glLineStipple(1, 0x7E66);
	else                                     glLineStipple(1, 0xFFFF);//Solid


	// X axis____________
	glBegin(GL_LINES);
		glVertex3d(-l, 0.0, 0.0);
		glVertex3d( l, 0.0, 0.0);
	glEnd();
	//Arrow
	glBegin(GL_LINES);
		glVertex3d( 1.0*l,   0.0,   0.0);
		glVertex3d( 0.98*l,  0.015*l, 0.015*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 1.0*l,  0.0,    0.0);
		glVertex3d( 0.98*l,-0.015*l,-0.015*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//XLabel
	renderText( l, 0.0, 0.0, "X", pMainFrame->m_TextFont);


	// Y axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, -l, 0.0);
		glVertex3d(0.0,  l, 0.0);
	glEnd();

	//Arrow
	glBegin(GL_LINES);
		glVertex3d( 0.0,     1.0*l,  0.0);
		glVertex3d( 0.015*l, 0.98*l, 0.015*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 0.0,     1.0*l,  0.0);
		glVertex3d(-0.015*l, 0.98*l,-0.015*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//Y Label
	renderText( 0.0, l, 0.0, "Y", pMainFrame->m_TextFont);


	// Z axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, -l);
		glVertex3d(0.0, 0.0,  l);
	glEnd();

	//Arrow
	glBegin(GL_LINES);
		glVertex3d(  0.0,   0.0, 1.0*l);
		glVertex3d( 0.015*l,  0.015*l,  0.98*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 0.0,    0.0, 1.0*l);
		glVertex3d(-0.015*l, -0.015*l,  0.98*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//ZLabel
	renderText( 0.0, 0.0, l, "Z", pMainFrame->m_TextFont);

	glDisable (GL_LINE_STIPPLE);
}


void ThreeDWidget::CreateArcballList(ArcBall &ArcBall, double GLScale)
{
	int row, col, NumAngles, NumCircles;
	double R, lat_incr, lon_incr, phi, theta;
	ArcBall.GetMatrix();
	Vector3d eye(0.0,0.0,1.0);
	Vector3d up(0.0,1.0,0.0);
	ArcBall.SetZoom(0.45,eye,up);

	glNewList(ARCBALL,GL_COMPILE);
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		glColor3d(0.3,0.3,.5);
		glLineWidth(1.0);

		R = ArcBall.ab_sphere;

		NumAngles  = 50;
		NumCircles =  6;
		lat_incr =  90.0 / NumAngles;
		lon_incr = 360.0 / NumCircles;

		for (col = 0; col < NumCircles; col++)
		{
			glBegin(GL_LINE_STRIP);
			{
				phi = (col * lon_incr) * PI/180.0;

				for (row = 1; row < NumAngles-1; row++)
				{
					theta = (row * lat_incr) * PI/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				phi = (col * lon_incr ) * PI/180.0;

				for (row = 1; row < NumAngles-1; row++)
				{
					theta = -(row * lat_incr) * PI/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
				}
			}
			glEnd();
		}


		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=1; col<35; col++)
			{
				phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
				glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=1; col<35; col++)
			{
				phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
				glVertex3d(R * cos(-phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(-phi) * cos(theta)*GLScale);
			}
		}
		glEnd();
	}
	glEndList();

	glNewList(ARCPOINT,GL_COMPILE);
	{
		glPolygonMode(GL_FRONT,GL_LINE);

		glColor3d(0.3,0.1,.2);
		glLineWidth(2.0);

		NumAngles  = 10;

		lat_incr = 30.0 / NumAngles;
		lon_incr = 30.0 / NumAngles;

		glBegin(GL_LINE_STRIP);
		{
			phi = 0.0;//longitude

			for (row = -NumAngles; row < NumAngles; row++)
			{
				theta = (row * lat_incr) * PI/180.0;
				glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=-NumAngles; col<NumAngles; col++)
			{
				phi = (0.0 + (double)col*30.0/NumAngles) * PI/180.0;
				glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
			}
		}
		glEnd();
	}
	glEndList();
}


void ThreeDWidget::GLCreateCylinderList(int GLList, QColor cr, double rbase, double rtop, double hbase, double htop, int NumArcs, int NumHeight)
{
	//
	// Create a list for a vertical cone
	//

	glNewList(GLList, GL_COMPILE);
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		GLDrawCylinder(cr, rbase, rtop, hbase, htop, NumArcs, NumHeight);
	}
	glEndList();
}



void ThreeDWidget::GLDrawCylinder(QColor cr, double rbase, double rtop, double hbase, double htop, int NumArcs, int NumHeight)
{
	//
	// Draws a horizontal cone
	// TODO check/improve the normal to the quads
	//
	static double tau, theta, radius, height;
	static double phi, phi1, cosphi, sinphi, cosphi1, sinphi1;

	theta = atan2(height, rbase-rtop);


	glColor3d(cr.redF(), cr.greenF(), cr.blueF());

	//sides
	for(int ia=0; ia<NumArcs; ia++)
	{
		phi  = (double) ia   /(double)NumArcs *2*PI;
		phi1 = (double)(ia+1)/(double)NumArcs *2*PI;
		cosphi = cos(phi);
		sinphi = sin(phi);
		cosphi1 = cos(phi1);
		sinphi1 = sin(phi1);

		glBegin(GL_QUAD_STRIP);
		{
			for(int ih=0; ih<=NumHeight; ih++)
			{
				tau =(double) ih   /double(NumHeight);
				radius = (1.-tau) *rbase + tau *rtop;
				height = (1.-tau) *hbase + tau *htop;

				glNormal3d(sin(theta), (cosphi+cosphi1)/2.0*cos(theta), (sinphi+sinphi1)/2*cos(theta));
				glVertex3d(height, radius*cosphi,  radius*sinphi);
				glVertex3d(height, radius*cosphi1, radius*sinphi1);
			}
		}
		glEnd();
	}
/*
	// base flange
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3d(0.0,0.0,hbase);
		for(int ia=0; ia<NumArcs; ia++)
		{
			phi  = (double) ia   /(double)NumArcs *2*PI;
			glVertex3d(hbase, rbase*cos(phi), rbase*sin(phi));
		}
	}
	glEnd();

	// top flange
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3d(0.0,0.0, htop);
		for(int ia=0; ia<NumArcs; ia++)
		{
			phi  = (double) ia   /(double)NumArcs *2*PI;
			glVertex3d(htop, rtop*cos(phi), rbase*sin(phi));
		}
	}
	glEnd();*/
}



void ThreeDWidget::GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes)
{
	//
	// Render the sphere representing the light or point masses
	//
	static double start_lat, start_lon,lat_incr, lon_incr, R;
	static double phi1, phi2, theta1, theta2;
	static GLdouble u[3], v[3], w[3], n[3];
	static int row, col;

	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT,GL_FILL);
	glColor3d(cr.redF(),cr.greenF(),cr.blueF());

	glBegin(GL_TRIANGLES);
	{
		start_lat = -90;
		start_lon = 0.0;
		R = radius;

		lat_incr = 180.0 / NumLatitudes;
		lon_incr = 360.0 / NumLongitudes;

		for (col = 0; col < NumLongitudes; col++)
		{
			phi1 = (start_lon + col * lon_incr) * PI/180.0;
			phi2 = (start_lon + (col + 1) * lon_incr) * PI/180.0;

			for (row = 0; row < NumLatitudes; row++)
			{
				theta1 = (start_lat + row * lat_incr) * PI/180.0;
				theta2 = (start_lat + (row + 1) * lat_incr) * PI/180.0;

				u[0] = R * cos(phi1) * cos(theta1);//x
				u[1] = R * sin(theta1);//y
				u[2] = R * sin(phi1) * cos(theta1);//z

				v[0] = R * cos(phi1) * cos(theta2);//x
				v[1] = R * sin(theta2);//y
				v[2] = R * sin(phi1) * cos(theta2);//z

				w[0] = R * cos(phi2) * cos(theta2);//x
				w[1] = R * sin(theta2);//y
				w[2] = R * sin(phi2) * cos(theta2);//z

				NormalVector(u,v,w,n);

				glNormal3dv(n);
				glVertex3dv(u);
				glVertex3dv(v);
				glVertex3dv(w);

				v[0] = R * cos(phi2) * cos(theta1);//x
				v[1] = R * sin(theta1);//y
				v[2] = R * sin(phi2) * cos(theta1);//z

				NormalVector(u,w,v,n);
				glNormal3dv(n);
				glVertex3dv(u);
				glVertex3dv(w);
				glVertex3dv(v);
			}
		}
	}
	glEnd();
}


void ThreeDWidget::GLDrawArrow(Vector3d O, Vector3d u, double length)
{
	// draws an arrow at point P
	// in the direction u
	// with length length

	Vector3d P, P1, P2;

	//reference arrow
	Vector3d R(0.0,0.0,1.0);
	Vector3d R1( 0.07, 0.0, -0.15);
	Vector3d R2(-0.07, 0.0, -0.15);

	// Rotate the reference arrow to align it with u
	Quaternion Qt;
	double cosa   = R.dot(u);
	double sina2  = sqrt((1.0 - cosa)*0.5);
	double cosa2  = sqrt((1.0 + cosa)*0.5);

	Vector3d Omega = R * u;

	Omega.Normalize();
	Omega *=sina2;
	Qt.Set(cosa2, Omega.x, Omega.y, Omega.z);

	Qt.Conjugate(R,  P);
	Qt.Conjugate(R1, P1);
	Qt.Conjugate(R2, P2);

	P  *= length;
	P1 *= length;
	P2 *= length;

	glBegin(GL_LINES);
	{
		glVertex3d(O.x,     O.y,     O.z);
		glVertex3d(O.x+P.x, O.y+P.y, O.z+P.z);
	}
	glEnd();

	glBegin(GL_LINES);
	{
		glVertex3d(O.x+P.x,      O.y+P.y,      O.z+P.z);
		glVertex3d(O.x+P.x+P1.x, O.y+P.y+P1.y, O.z+P.z+P1.z);
	}
	glEnd();

	glBegin(GL_LINES);
	{
		glVertex3d(O.x+P.x,      O.y+P.y,      O.z+P.z);
		glVertex3d(O.x+P.x+P2.x, O.y+P.y+P2.y, O.z+P.z+P2.z);
	}
	glEnd();

/*	glBegin(GL_LINES);
	{
		glVertex3d(O.x,              O.y,              O.z);
		glVertex3d(O.x + u.x*length, O.y + u.y*length, O.z + u.z*length);
	}
	glEnd();

	glBegin(GL_LINES);
	{
		glVertex3d(O.x+u.x*length,      O.y+ u.y*length,       O.z + u.z*length);
		glVertex3d(O.x+u.x*length+P1.x, O.y + u.y*length+P1.y, O.z + u.z*length+P1.z);
	}
	glEnd();

	glBegin(GL_LINES);
	{
		glVertex3d(O.x+u.x*length,      O.y + u.y*length,      O.z + u.z*length);
		glVertex3d(O.x+u.x*length+P2.x, O.y + u.y*length+P2.y, O.z + u.z*length +P2.z);
	}
	glEnd();*/
}


void ThreeDWidget::GLDrawCube(Vector3d Pt, double side)
{
	//
	// Render the sphere representing the light or point masses
	//
	side *= 0.5;

	glBegin(GL_QUAD_STRIP);
	{
		glVertex3d(Pt.x-side, Pt.y-side, Pt.z-side);
		glVertex3d(Pt.x+side, Pt.y-side, Pt.z-side);

		glVertex3d(Pt.x-side, Pt.y-side, Pt.z+side);
		glVertex3d(Pt.x+side, Pt.y-side, Pt.z+side);

		glVertex3d(Pt.x-side, Pt.y+side, Pt.z+side);
		glVertex3d(Pt.x+side, Pt.y+side, Pt.z+side);

		glVertex3d(Pt.x-side, Pt.y+side, Pt.z-side);
		glVertex3d(Pt.x+side, Pt.y+side, Pt.z-side);

		glVertex3d(Pt.x-side, Pt.y-side, Pt.z-side);
		glVertex3d(Pt.x+side, Pt.y-side, Pt.z-side);
	}
	glEnd();
	glBegin(GL_QUADS);
	{
		glVertex3d(Pt.x-side, Pt.y-side, Pt.z-side);
		glVertex3d(Pt.x-side, Pt.y+side, Pt.z-side);
		glVertex3d(Pt.x-side, Pt.y+side, Pt.z+side);
		glVertex3d(Pt.x-side, Pt.y-side, Pt.z+side);
	}
	glEnd();
	glBegin(GL_QUADS);
	{
		glVertex3d(Pt.x+ side, Pt.y-side, Pt.z-side);
		glVertex3d(Pt.x+ side, Pt.y+side, Pt.z-side);
		glVertex3d(Pt.x+ side, Pt.y+side, Pt.z+side);
		glVertex3d(Pt.x+ side, Pt.y-side, Pt.z+side);
	}
	glEnd();
}




void ThreeDWidget::GLCreateCubeList(int GLList, double side)
{
	//
	// Render the sphere representing the light or point masses
	//
	glNewList(GLList, GL_COMPILE);
	{
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT,GL_FILL);

		//six sides to construct 4+2 (or 3+3...)
		Vector3d O;
		GLDrawCube(O, side);
	}
	glEndList();
}






void ThreeDWidget::GLCreateSphereList(int GLList, double radius, int NumLongitudes, int NumLatitudes)
{
	//
	// Render the sphere representing the light or point masses
	//
	static double start_lat, start_lon,lat_incr, lon_incr, R;
	static double phi1, phi2, theta1, theta2;
	static GLdouble u[3], v[3], w[3], n[3];
	static int row, col;

	glNewList(GLList, GL_COMPILE);
	{
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT,GL_FILL);

		glBegin(GL_TRIANGLES);
		{
			start_lat = -90;
			start_lon = 0.0;
			R = radius;

			lat_incr = 180.0 / NumLatitudes;
			lon_incr = 360.0 / NumLongitudes;

			for (col = 0; col < NumLongitudes; col++)
			{
				phi1 = (start_lon + col * lon_incr) * PI/180.0;
				phi2 = (start_lon + (col + 1) * lon_incr) * PI/180.0;

				for (row = 0; row < NumLatitudes; row++)
				{
					theta1 = (start_lat + row * lat_incr) * PI/180.0;
					theta2 = (start_lat + (row + 1) * lat_incr) * PI/180.0;

					u[0] = R * cos(phi1) * cos(theta1);//x
					u[1] = R * sin(theta1);//y
					u[2] = R * sin(phi1) * cos(theta1);//z

					v[0] = R * cos(phi1) * cos(theta2);//x
					v[1] = R * sin(theta2);//y
					v[2] = R * sin(phi1) * cos(theta2);//z

					w[0] = R * cos(phi2) * cos(theta2);//x
					w[1] = R * sin(theta2);//y
					w[2] = R * sin(phi2) * cos(theta2);//z

					NormalVector(u,v,w,n);

					glNormal3dv(n);
					glVertex3dv(u);
					glVertex3dv(v);
					glVertex3dv(w);

					v[0] = R * cos(phi2) * cos(theta1);//x
					v[1] = R * sin(theta1);//y
					v[2] = R * sin(phi2) * cos(theta1);//z

					NormalVector(u,w,v,n);
					glNormal3dv(n);
					glVertex3dv(u);
					glVertex3dv(w);
					glVertex3dv(v);
				}
			}
		}
		glEnd();
	}
	glEndList();
}


void ThreeDWidget::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
{
	//
	// calculate two vectors, using the middle point as the common origin
	//

	GLdouble v1[3], v2[3], d;
	v1[0] = p3[0] - p1[0];
	v1[1] = p3[1] - p1[1];
	v1[2] = p3[2] - p1[2];
	v2[0] = p3[0] - p2[0];
	v2[1] = p3[1] - p2[1];
	v2[2] = p3[2] - p2[2];

	// calculate the cross product of the two vectors
	n[0] = v1[1] * v2[2] - v2[1] * v1[2];
	n[1] = v1[2] * v2[0] - v2[2] * v1[0];
	n[2] = v1[0] * v2[1] - v2[0] * v1[1];

	// normalize the vector
	d = ( n[0] * n[0] + n[1] * n[1] + n[2] * n[2] );
	// try to catch very small vectors
	if (d < (GLdouble)0.00000001)
	{
		d = (GLdouble)100000000.0;
	}
	else
	{
		d = (GLdouble)1.0 / sqrt(d);
	}

	n[0] *= d;
	n[1] *= d;
	n[2] *= d;
}



void ThreeDWidget::ClientToGL(QPoint const &point, Vector3d &real)
{
	//
	// In input, takes the 2D point in screen client area coordinates
	// In output, returns the 2D OpenGL point
	//
	static double h2, w2;
	h2 = (double)geometry().height() /2.0;
	w2 = (double)geometry().width()  /2.0;

	if(w2>h2)
	{
		real.x =  ((double)point.x() - w2) / w2;
		real.y = -((double)point.y() - h2) / w2;
	}
	else
	{
		real.x =  ((double)point.x() - w2) / h2;
		real.y = -((double)point.y() - h2) / h2;
	}
}



void ThreeDWidget::GLToClient(Vector3d const &real, QPoint &point)
{
	//
	//converts an opengl 2D vector to screen client coordinates
	//
	static double dx, dy, h2, w2;

	h2 = m_GLViewRect.height() /2.0;
	w2 = m_GLViewRect.width()  /2.0;

	dx = ( real.x + w2)/2.0;
	dy = (-real.y + h2)/2.0;

	if(w2>h2)
	{
		point.setX((int)(dx * (double)geometry().width()));
		point.setY((int)(dy * (double)geometry().width()));
	}
	else
	{
		point.setX((int)(dx * (double)geometry().height()));
		point.setY((int)(dy * (double)geometry().height()));
	}
}



void ThreeDWidget::GLInverseMatrix(double MatIn[][4], double MatOut[][4])
{
	//
	// inverses the rotation matrix by transposition
	//
	int i,j;

	for(i=0 ; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			MatOut[j][i] = MatIn[i][j];
		}
	}
}
