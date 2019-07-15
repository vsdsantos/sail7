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

