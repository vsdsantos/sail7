/****************************************************************************

    ThreeDWidget Class
    Copyright (C) 2012 Andre Deperrois 
    (C) All rights reserved

*****************************************************************************/


#include <QtOpenGL>
#include <math.h>
#include <QtDebug>


#include "threedwidget.h"
#include "../mainframe.h"
#include "../sail7/sail7.h"
#include "../sail7/sailviewwt.h"
#include "../sail7/gl3dbodydlg.h"
#include "../globals.h"
#include "../sail7/gl3dscales.h"

Sail7 *ThreeDWidget::s_pSail7 = nullptr;
MainFrame *ThreeDWidget::s_pMainFrame = nullptr;


ThreeDWidget::ThreeDWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m_iView = GLSAIL7VIEW;
    setAutoFillBackground(false);

    setMouseTracking(true);
    setCursor(Qt::CrossCursor);

    //create a default pix from a random image - couldn't find a better way to do this
    m_PixText = QPixmap(":/images/splash.png");
    m_PixText.fill(Qt::transparent);

    m_pctrlBtData = new QLabel(this);
    m_pctrlBtData->setTextFormat(Qt::PlainText);
    m_pctrlBtOppData = new QLabel(this);
    m_pctrlBtOppData->setTextFormat(Qt::PlainText);
    m_pctrlBtOppData->setWordWrap(false);
    m_pctrlBtOppData->setAlignment(Qt::AlignLeft);
}


void ThreeDWidget::initializeGL()
{
    glClearColor(float(s_pMainFrame->m_BackgroundColor.redF()), float(s_pMainFrame->m_BackgroundColor.greenF()), float(s_pMainFrame->m_BackgroundColor.blueF()), 0.0f);
}



void ThreeDWidget::paintGL()
{
    int pixelRatio = 1;
#if QT_VERSION >= 0x050000
    pixelRatio = devicePixelRatio();
#endif
    setupViewPort(width() * pixelRatio, height() * pixelRatio);

    glClearColor(float(s_pMainFrame->m_BackgroundColor.redF()), float(s_pMainFrame->m_BackgroundColor.greenF()), float(s_pMainFrame->m_BackgroundColor.blueF()), 0.0f);
}


void ThreeDWidget::resizeGL(int width, int height)
{
    double w = double(width);
    double h = double(height);
    double s = 1.0;

    setupViewPort(width, height);

    glLoadIdentity();
    if(w>h) m_GLViewRect.SetRect(-s, s*h/w, s, -s*h/w);
    else    m_GLViewRect.SetRect(-s*w/h, s, s*w/h, -s*h/w);

    if(!m_PixText.isNull())
    {
        m_PixText= m_PixText.scaled(rect().size()*devicePixelRatio());
        m_PixText.fill(Qt::transparent);
    }

    showEvent(nullptr);
}


/**
* Overrides the virtual paintEvent method of the base class.
* Calls the paintGL() method, then overlays the text using the widget's QPainter
* @param event
*/
void ThreeDWidget::paintEvent(QPaintEvent *)
{
    paintGL();
    QPainter painter(this);

    if(m_iView==GLSAIL7VIEW)
    {
//        painter.setBackgroundMode(Qt::TransparentMode);
//        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
//        painter.setOpacity(1);
    }

    if(!m_PixText.isNull())
    {
        painter.drawPixmap(0,0, m_PixText);
/*        if(s_pSail7->m_b3DCp && s_pSail7->m_pCurBoatOpp)
        {
            QFile file("/home/techwinder/tmp/pixmap.png");
            file.open(QIODevice::WriteOnly);
            m_PixText.save(&file, "PNG");
        }*/
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


void ThreeDWidget::GLSetupLight(GLLightDlg *pglLightParams, double Offset_y, float LightFactor)
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

    if(LightFactor>1.0f) LightFactor = 1.0f;

    LightFactor = .1f;

    fLightDiffuse0[0] = float(LightFactor*pglLightParams->s_Diffuse * pglLightParams->s_Red); // red component
    fLightDiffuse0[1] = float(LightFactor*pglLightParams->s_Diffuse * pglLightParams->s_Green); // green component
    fLightDiffuse0[2] = float(LightFactor*pglLightParams->s_Diffuse * pglLightParams->s_Blue); // blue component
    fLightDiffuse0[3] = 1.0f; // alpha

    // the ambient light conditions.
    fLightAmbient0[0] = float(LightFactor*pglLightParams->s_Ambient * pglLightParams->s_Red); // red component
    fLightAmbient0[1] = float(LightFactor*pglLightParams->s_Ambient * pglLightParams->s_Green); // green component
    fLightAmbient0[2] = float(LightFactor*pglLightParams->s_Ambient * pglLightParams->s_Blue); // blue component
    fLightAmbient0[3] = 1.0f; // alpha


    fLightSpecular0[0] = float(LightFactor*pglLightParams->s_Specular * pglLightParams->s_Red); // red component
    fLightSpecular0[1] = float(LightFactor*pglLightParams->s_Specular * pglLightParams->s_Green); // green component
    fLightSpecular0[2] = float(LightFactor*pglLightParams->s_Specular * pglLightParams->s_Blue); // blue component
    fLightSpecular0[3] = 1.0f; // alpha

    // And finally, its position

    fLightPosition0[0] = pglLightParams->s_XLight;
    fLightPosition0[1] = pglLightParams->s_YLight + GLfloat(Offset_y);
    fLightPosition0[2] = pglLightParams->s_ZLight;
    fLightPosition0[3] = 1.0f; // W (positional light)


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
    else                             glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}


void ThreeDWidget::GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth)
{
    //
    // Draws the axis in the OpenGL view
    //

    double l = .6*length;

    glPolygonMode(GL_FRONT,GL_LINE);
    glLineWidth(GLfloat(AxisWidth));

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
//    glRenderText(l, 0.0, 0.0, "X");


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
//    glRenderText(0.0, l, 0.0, "Y");


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
//    glRenderText( 0.0, 0.0, l, "Z");

    glDisable (GL_LINE_STIPPLE);
}


void ThreeDWidget::CreateArcballList(ArcBall &ArcBall, double GLScale)
{
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

        int NumAngles  = 50;
        int NumCircles =  6;
        lat_incr =  90.0 / NumAngles;
        lon_incr = 360.0 / NumCircles;

        for (int col = 0; col < NumCircles; col++)
        {
            glBegin(GL_LINE_STRIP);
            {
                phi = (col * lon_incr) * PI/180.0;

                for (int row = 1; row < NumAngles-1; row++)
                {
                    theta = (row * lat_incr) * PI/180.0;
                    glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
                }
            }
            glEnd();
            glBegin(GL_LINE_STRIP);
            {
                phi = (col * lon_incr ) * PI/180.0;

                for (int row = 1; row < NumAngles-1; row++)
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
            for(int col=1; col<35; col++)
            {
                phi = (0.0 + double(col)*360.0/72.0) * PI/180.0;
                glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
            }
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        {
            theta = 0.;
            for(int col=1; col<35; col++)
            {
                phi = (0.0 + double(col)*360.0/72.0) * PI/180.0;
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

        int NumAngles  = 10;

        lat_incr = 30.0 / NumAngles;
        lon_incr = 30.0 / NumAngles;

        glBegin(GL_LINE_STRIP);
        {
            phi = 0.0;//longitude

            for (int row = -NumAngles; row < NumAngles; row++)
            {
                theta = (row * lat_incr) * PI/180.0;
                glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
            }
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        {
            theta = 0.;
            for(int col=-NumAngles; col<NumAngles; col++)
            {
                phi = (0.0 + double(col)*30.0/NumAngles) * PI/180.0;
                glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
            }
        }
        glEnd();
    }
    glEndList();
}


void ThreeDWidget::GLCreateCylinderList(GLuint GLList, QColor cr, double rbase, double rtop, double hbase, double htop, int NumArcs, int NumHeight)
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
    double tau=0, theta=0, radius=1, height=1;
    double phi=0, phi1=0, cosphi=0, sinphi=0, cosphi1=0, sinphi1=0;

    theta = atan2(height, rbase-rtop);


    glColor3d(cr.redF(), cr.greenF(), cr.blueF());

    //sides
    for(int ia=0; ia<NumArcs; ia++)
    {
        phi  = double(ia)  /double(NumArcs) *2*PI;
        phi1 = double(ia+1)/double(NumArcs) *2*PI;
        cosphi = cos(phi);
        sinphi = sin(phi);
        cosphi1 = cos(phi1);
        sinphi1 = sin(phi1);

        glBegin(GL_QUAD_STRIP);
        {
            for(int ih=0; ih<=NumHeight; ih++)
            {
                tau =double(ih)   /double(NumHeight);
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
    double start_lat, start_lon,lat_incr, lon_incr, R;
    double phi1, phi2, theta1, theta2;
    GLdouble u[3], v[3], w[3], n[3];
    int row, col;

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

/*    glBegin(GL_LINES);
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




void ThreeDWidget::GLCreateCubeList(GLuint GLList, double side)
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






void ThreeDWidget::GLCreateSphereList(GLuint GLList, double radius, int NumLongitudes, int NumLatitudes)
{
    //
    // Render the sphere representing the light or point masses
    //
    double start_lat, start_lon,lat_incr, lon_incr, R;
    double phi1, phi2, theta1, theta2;
    GLdouble u[3], v[3], w[3], n[3];
    int row, col;

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
    if (d < 0.00000001)
    {
        d = 100000000.0;
    }
    else
    {
        d = 1.0 / sqrt(d);
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
    double h2, w2;
    h2 = double(geometry().height()) /2.0;
    w2 = double(geometry().width())  /2.0;

    if(w2>h2)
    {
        real.x =  (double(point.x()) - w2) / w2;
        real.y = -(double(point.y()) - h2) / w2;
    }
    else
    {
        real.x =  (double(point.x()) - w2) / h2;
        real.y = -(double(point.y()) - h2) / h2;
    }
}


void ThreeDWidget::GL2dToClient(Vector3d const &real, QPoint &point)
{
    //
    //converts an opengl 2D vector to screen client coordinates
    //

    double h2 = m_GLViewRect.height() /2.0;
    double w2 = m_GLViewRect.width()  /2.0;

    double dx = ( real.x + w2)/2.0;
    double dy = (-real.y + h2)/2.0;

    if(w2>h2)
    {
        point.setX(int(dx * geometry().width()));
        point.setY(int(dy * geometry().width()));
    }
    else
    {
        point.setX(int(dx * geometry().height()));
        point.setY(int(dy * geometry().height()));
    }
}


void ThreeDWidget::GLInverseMatrix(double MatIn[][4], double MatOut[][4])
{
    //
    // inverses the rotation matrix by transposition
    //
    for(int i=0 ; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            MatOut[j][i] = MatIn[i][j];
        }
    }
}


QPoint ThreeDWidget::worldToScreen(Vector3d v)
{
    QVector4D v4(v.xf(), v.yf(), v.zf(), 1.0);

    QMatrix4x4 mvMatrix, pMatrix, pvmMatrix;

    QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());

    glFuncs.glGetFloatv(GL_MODELVIEW_MATRIX, mvMatrix.data());
    glFuncs.glGetFloatv(GL_PROJECTION_MATRIX, pMatrix.data());

    pvmMatrix = pMatrix * mvMatrix;

    QVector4D vS = pvmMatrix * v4;
    QPoint pt(int((vS.x()+1.0f)*width()/2), int((1.0f-vS.y())*height()/2));
    return pt;
}


void ThreeDWidget::glRenderText(double x, double y, double z, const QString & str, QColor textColor)
{
    QPoint point = worldToScreen(Vector3d(x,y,z));
    point *= devicePixelRatio();
    if(!m_PixText.isNull())
    {
        QPainter paint(&m_PixText);
        paint.save();
        QPen textPen(textColor);
        paint.setPen(textPen);
        QFont font(paint.font());
        font.setPointSize(paint.font().pointSize()*devicePixelRatio());
        paint.setFont(font);
        paint.drawText(point, str);
        paint.restore();
    }
}


void ThreeDWidget::glRenderText(int x, int y, const QString & str, QColor textColor)
{
    if(!m_PixText.isNull())
    {
        QPainter paint(&m_PixText);
        paint.save();
        QPen textPen(textColor);
        paint.setPen(textPen);
        QFont font(paint.font());
        font.setPointSize(paint.font().pointSize()*devicePixelRatio());
        paint.setFont(font);
        paint.drawText(x*devicePixelRatio(),y*devicePixelRatio(), str);
        paint.restore();
    }
}


void ThreeDWidget::renderText(int x, int y, QString const &text)
{
    QPainter painter(&m_PixText);
    painter.drawText(x,y,text);
}


void ThreeDWidget::setBoatData(QString const &data)
{
    //reset style, just in case settings have changed
    setLabelFont();
    m_pctrlBtData->setText(data);
    showEvent(nullptr);
}


void ThreeDWidget::setBoatOppData(QString const &data)
{
    //reset style, just in case settings have changed
    setLabelFont();

    m_pctrlBtOppData->setText(data);
    showEvent(nullptr);
}


void ThreeDWidget::showEvent(QShowEvent *)
{
    m_pctrlBtData->adjustSize();
    QPoint pos1(5, height()-m_pctrlBtData->height()-5);
    m_pctrlBtData->move(pos1);

    m_pctrlBtOppData->adjustSize();
    QPoint pos2(width()-5-m_pctrlBtOppData->width(), height()-m_pctrlBtOppData->height()-5);
    m_pctrlBtOppData->move(pos2);
}


void ThreeDWidget::setLabelFont()
{
    QPalette palette;
    palette.setColor(QPalette::WindowText, s_pMainFrame->m_TextColor);

    m_pctrlBtData->setFont(s_pMainFrame->m_TextFont);
    m_pctrlBtData->setPalette(palette);

    m_pctrlBtOppData->setFont(s_pMainFrame->m_TextFont);
    m_pctrlBtOppData->setPalette(palette);
}



/**
 * Paints and overlays the labels associated to the Cp color scale in the 3D view
 * @param painter the painter associated to the 3D widget
 */
void ThreeDWidget::PaintCpLegendText()
{
    if (!s_pSail7->m_b3DCp || !s_pSail7->m_pCurBoatOpp ) return;

    QString strong;

    if(m_PixText.isNull())
        return;

    m_PixText.fill(Qt::transparent);

    QRect pixrect = m_PixText.rect();

    QPainter painter(&m_PixText);
    painter.save();

    painter.setFont(s_pMainFrame->m_TextFont);
    painter.setRenderHint(QPainter::Antialiasing);

    QFontMetrics fm(s_pMainFrame->m_TextFont);
    int back = fm.averageCharWidth() * 5;

    double h = pixrect.height();
    double y0 = 2.*h/5.0;

    int ixPos  = pixrect.width()-back;

    int dy     = int(h/40.0);
    int iyPos  = int(y0 - 12.0*dy);


    double range = GL3DScales::s_LegendMax - GL3DScales::s_LegendMin;
    double delta = range / 20;


    QPen textPen(s_pMainFrame->m_TextColor);
    painter.setPen(textPen);
    painter.setRenderHint(QPainter::Antialiasing);

    strong = "Cp";
    int labellength = fm.width(strong)+5;
    painter.drawText(ixPos-labellength, iyPos-dy, strong);

    for (int i=0; i<=20; i ++)
    {
        double f = GL3DScales::s_LegendMax - double(i) * delta;
        strong = QString("%1").arg(f, 5,'f',2);
        labellength = (fm.width(strong)+5);
        painter.drawText(ixPos-labellength, iyPos+i*dy, strong);
    }

    painter.restore();
}

/**
 * Paints the labels associated to the Panel forces color scale in the 3D view
 * @param painter the painter to write on
 */
void ThreeDWidget::PaintPanelForceLegendText(double rmin, double rmax)
{
    if(!s_pSail7->m_pCurBoatPolar || !s_pSail7->m_pCurBoatOpp) return;
    if(!s_pSail7->m_bPanelForce) return;

    if(m_PixText.isNull())
        return;

    QString strForce, strong;
    m_PixText.fill(Qt::transparent);

    QRect pixrect = m_PixText.rect();

    int labellength;
    double f=0;
    double range=0, delta=0;

    GetForceUnit(strForce, s_pMainFrame->m_ForceUnit);

    QPainter painter(&m_PixText);

    painter.save();
    painter.setFont(s_pMainFrame->m_TextFont);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen textPen(s_pMainFrame->m_TextColor);
    painter.setPen(textPen);


    //define the range of values to set the colors in accordance
    range = rmax - rmin;


    QFontMetrics fm(s_pMainFrame->m_TextFont);
    int back = fm.averageCharWidth() * 5;

    double h = double(pixrect.height());
    double y0 = 2.*h/5.0;


    int ixPos, iyPos, dy;

    ixPos  = pixrect.width()-back;

    dy     = int(h/40.0);
    iyPos  = int(y0 - 12.0*dy);

    delta = range / 20.0;


    labellength = fm.width(strForce)+5;
    painter.drawText(ixPos-labellength, iyPos-dy, strForce);


    for (int i=0; i<=20; i++)
    {
        f = rmin + double(i) * delta;
        strong = QString("%1").arg(f, 5,'f',2);
        labellength = (fm.width(strong)+5);
        painter.drawText(ixPos-labellength, iyPos+i*dy, strong);
    }

    painter.restore();
}



