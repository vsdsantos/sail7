/****************************************************************************

    glHullView Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/

#include "glhullview.h"
#include "../mainframe.h"
#include "../sail7/gl3dbodydlg.h"

glHullView::glHullView(QWidget *pParent) : ThreeDWidget(pParent)
{
    m_pglHullDlg = dynamic_cast<GL3dBodyDlg*>(pParent);
}

void glHullView::contextMenuEvent (QContextMenuEvent * event)
{
    m_pglHullDlg->ShowContextMenu(event);
}


void glHullView::mousePressEvent(QMouseEvent *event)
{
    m_pglHullDlg->mousePressEvent(event);
}


void glHullView::mouseReleaseEvent(QMouseEvent *event)
{
    m_pglHullDlg->mouseReleaseEvent(event);
}


void glHullView::mouseMoveEvent(QMouseEvent *event)
{
    m_pglHullDlg->mouseMoveEvent(event);
}


void glHullView::mouseDoubleClickEvent ( QMouseEvent * event )
{
    m_pglHullDlg->mouseDoubleClickEvent(event);
}


void glHullView::wheelEvent(QWheelEvent *event)
{
    m_pglHullDlg->wheelEvent(event);
}



void glHullView::keyPressEvent(QKeyEvent *event)
{
    m_pglHullDlg->keyPressEvent(event);
}


void glHullView::keyReleaseEvent(QKeyEvent *event)
{
    m_pglHullDlg->keyReleaseEvent(event);
}


void glHullView::paintGL()
{
    ThreeDWidget::paintGL();

    m_pglHullDlg->GLDraw3D();
    m_pglHullDlg->GLRenderBody();

}


void glHullView::resizeGL(int width, int height)
{
    ThreeDWidget::resizeGL(width, height);

    m_pglHullDlg->m_bIs3DScaleSet = false;
    m_pglHullDlg->SetRectangles();
    m_pglHullDlg->SetBodyLineScale();
    m_pglHullDlg->SetFrameScale();
    m_pglHullDlg->SetBodyScale();
}
