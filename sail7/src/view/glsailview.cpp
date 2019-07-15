/****************************************************************************

    glSailView Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/

#include "glsailview.h"
#include "../mainframe.h"
#include "../sail7/sail7.h"
#include "../sail7/sailviewwt.h"

glSailView::glSailView(QWidget *pParent) : ThreeDWidget (pParent)
{
    m_pSailViewWt = dynamic_cast<SailViewWt*>(pParent);
}


void glSailView::mousePressEvent(QMouseEvent *event)
{
    m_pSailViewWt->mousePressEvent(event);
}


void glSailView::mouseReleaseEvent(QMouseEvent *event)
{
    m_pSailViewWt->mouseReleaseEvent(event);
}


void glSailView::mouseMoveEvent(QMouseEvent *event)
{
     m_pSailViewWt->mouseMoveEvent(event);
}


void glSailView::mouseDoubleClickEvent ( QMouseEvent * event )
{
    m_pSailViewWt->mouseDoubleClickEvent(event);
}


void glSailView::wheelEvent(QWheelEvent *event)
{
     m_pSailViewWt->wheelEvent(event);
}


void glSailView::keyPressEvent(QKeyEvent *event)
{
    m_pSailViewWt->keyPressEvent(event);
}


void glSailView::paintGL()
{
    ThreeDWidget::paintGL();

    m_pSailViewWt->GLDraw3D();
    m_pSailViewWt->GLRenderSail();

}


