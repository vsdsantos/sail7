/****************************************************************************

    glSail7View Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/

#include <QMenu>

#include "glsail7view.h"
#include "../mainframe.h"
#include "../sail7/sail7.h"

glSail7View::glSail7View(QWidget *parent) : ThreeDWidget (parent)
{

}


glSail7View::~glSail7View()
{
    if(glIsList(WATERLIST))           glDeleteLists(WATERLIST,  1);
    if(glIsList(WINDLIST))            glDeleteLists(WINDLIST,   1);
    if(glIsList(NORMALLIST))          glDeleteLists(NORMALLIST, 1);
}

void glSail7View::contextMenuEvent (QContextMenuEvent * event)
{
    s_pMainFrame->Sail3DCtxMenu->exec(event->pos());
}


void glSail7View::mousePressEvent(QMouseEvent *event)
{
    s_pSail7->mousePressEvent(event);
}


void glSail7View::mouseReleaseEvent(QMouseEvent *event)
{
    s_pSail7->mouseReleaseEvent(event);
}


void glSail7View::mouseMoveEvent(QMouseEvent *event)
{
    s_pSail7->mouseMoveEvent(event);
}


void glSail7View::mouseDoubleClickEvent ( QMouseEvent * event )
{
    //    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

    s_pSail7->mouseDoubleClickEvent(event);
}


void glSail7View::wheelEvent(QWheelEvent *event)
{
    s_pSail7->wheelEvent(event);
}


void glSail7View::keyPressEvent(QKeyEvent *event)
{
    s_pSail7->keyPressEvent(event);
}


void glSail7View::keyReleaseEvent(QKeyEvent *event)
{
    s_pSail7->keyReleaseEvent(event);
}


void glSail7View::paintGL()
{
    ThreeDWidget::paintGL();
    makeCurrent();
    s_pSail7->GLDraw3D();
    s_pSail7->GLRenderView();
}


void glSail7View::resizeGL(int width, int height)
{
    ThreeDWidget::resizeGL(width, height);

    s_pSail7->m_ArcBall.GetMatrix();
    s_pSail7->m_bIs3DScaleSet = false;
    s_pSail7->Set3DScale();

}


void glSail7View::GLCreateWaterList()
{
    if(glIsList(WATERLIST)) return;

    QColor wColor(10, 70, 190);
    glNewList(WATERLIST, GL_COMPILE);
    {
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPolygonOffset(1.0, 1.0);

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


void glSail7View::GLCreateWindList(Boat*pBoat, BoatPolar *pBtPolar)
{
    if(glIsList(WINDLIST)) return;

    double s=0, h=0, height=0;
    Vector3d w(1.0,0.0,0.0);
    s=1.0;

    if(!pBoat || !pBtPolar) return;

    height = pBoat->Height() * 2.0;

    glNewList(WINDLIST, GL_COMPILE);
    {
        glLineWidth(2.0);
        glColor3d(0.7,1.0,0.7);

        for(int iw=0; iw<=10; iw++)
        {
            h = double(iw)/10.0 * height;

            s = pBtPolar->WindFactor(h);
            //            p3DWidget->GLDrawCylinder(QColor(150,150,150), 0.13*s, 0.13*s, 0.0*s, 1.5*s, 31, 11);
            //            p3DWidget->GLDrawCylinder(QColor(150,150,150), 0.31*s, 0.00*s, 1.5*s, 2.5*s, 31, 11);
            GLDrawArrow(Vector3d(0.0, 0.0, iw*height/10.0), w, s*2.0);

            //            glTranslated(0.0, 0.0, height/10.0);
        }

        /*        glBegin(GL_LINE_STRIP);
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
