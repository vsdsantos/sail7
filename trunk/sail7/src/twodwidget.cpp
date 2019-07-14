/****************************************************************************

    TwoDWidget Class
    Copyright (C) 2012 Andre Deperrois 
    All rights reserved

*****************************************************************************/



//#include <QtGui>
#include <QMenu>
#include <math.h>
#include "mainframe.h"
#include "graph/qgraph.h"
#include "graph/curve.h"
#include "sail7/sail7.h"
#include "sail7/saildlg.h"
#include "twodwidget.h"

MainFrame *TwoDWidget::s_pMainFrame=nullptr;
Sail7 *TwoDWidget::s_pSail7=nullptr;

TwoDWidget::TwoDWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pParent = parent;
    m_iView = SAIL7VIEW;

    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
}


void TwoDWidget::keyPressEvent(QKeyEvent *event)
{
    if(s_pSail7)
    {
        if(m_iView==SAIL7VIEW)
        {
            s_pSail7->keyPressEvent(event);
        }
    }
}


void TwoDWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(s_pSail7)
    {
        if(m_iView==SAIL7VIEW)
        {
            s_pSail7->keyReleaseEvent(event);
        }
    }
}


void TwoDWidget::mousePressEvent(QMouseEvent *event)
{
    if(s_pSail7)
    {
        if(m_iView==SAIL7VIEW)
        {
            s_pSail7->mousePressEvent(event);
        }
    }
}


void TwoDWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(s_pSail7)
    {
        if(m_iView==SAIL7VIEW)
        {
            s_pSail7->mouseReleaseEvent(event);
        }
    }
}


void TwoDWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(s_pSail7)
    {
        if(m_iView==SAIL7VIEW)
        {
            s_pSail7->mouseMoveEvent(event);
        }
    }
}

void TwoDWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
    if(s_pSail7)
    {
        if(m_iView==SAIL7VIEW)
        {
            s_pSail7->mouseDoubleClickEvent(event );
        }
    }
}


void TwoDWidget::wheelEvent(QWheelEvent *event)
{
    if(s_pSail7)
    {
        if(m_iView==SAIL7VIEW)
        {
            s_pSail7->wheelEvent(event);
        }
    }
}



void TwoDWidget::contextMenuEvent (QContextMenuEvent * pEvent)
{
    QPoint ScreenPt = pEvent->globalPos();
    QPoint CltPt = pEvent->pos();

    if(m_iView==SAIL7VIEW)
    {
        if(pEvent->reason()==QContextMenuEvent::Keyboard)
        {
            ScreenPt.rx() = s_pSail7->m_LastPoint.x()+s_pMainFrame->pos().x()+geometry().x();
            ScreenPt.ry() = s_pSail7->m_LastPoint.y()+s_pMainFrame->pos().y()+geometry().y();
        }

        s_pSail7->m_pCurGraph = s_pSail7->GetGraph(CltPt);
        if (s_pSail7->m_iView==SAILPOLARVIEW) s_pMainFrame->SailPlrCtxMenu->exec(ScreenPt);
    }
}



void TwoDWidget::resizeEvent(QResizeEvent *)
{
    //    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

    if(m_iView==SAIL7VIEW && s_pSail7)
    {
        s_pSail7->m_bIs2DScaleSet = false;
        s_pSail7->Set2DScale();
        //        if(pSail7->m_iView==SAILOPPVIEW)   pSail7->SetWingLegendPos();
        if(s_pSail7->m_iView==SAILPOLARVIEW) s_pSail7->SetBoatPlrLegendPos();
    }
    else
    {
    }
}


void TwoDWidget::paintEvent(QPaintEvent *)
{
    if(m_iView == SAIL7VIEW && s_pSail7)
    {
        QPainter painter(this);
        s_pSail7->PaintView(painter);
    }
    else
    {
        QPainter painter(this);
        painter.fillRect(rect(), s_pMainFrame->m_BackgroundColor);
    }
}
