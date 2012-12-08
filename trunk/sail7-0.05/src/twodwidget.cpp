/****************************************************************************

	TwoDWidget Class
	Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
	All rights reserved

*****************************************************************************/



//#include <QtGui>
#include <QMenu>
#include <math.h>
#include "mainframe.h"
#include "graph/qgraph.h"
#include "graph/curve.h"
#include "sail7/sail7.h"
#include "sail7/SailDlg.h"
#include "twodwidget.h"

void *TwoDWidget::s_pMainFrame=NULL;
void *TwoDWidget::s_pSail7=NULL;

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
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->keyPressEvent(event);
		}
	}
}


void TwoDWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(s_pSail7)
	{
		if(m_iView==SAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->keyReleaseEvent(event);
		}
	}
}


void TwoDWidget::mousePressEvent(QMouseEvent *event)
{
	if(s_pSail7)
	{
		if(m_iView==SAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->mousePressEvent(event);
		}
	}
}


void TwoDWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(s_pSail7)
	{
		if(m_iView==SAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->mouseReleaseEvent(event);
		}
	}
}


void TwoDWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(s_pSail7)
	{
		if(m_iView==SAIL7VIEW)
		{
			QSail7* pSail7 = (QSail7*)s_pSail7;
			pSail7->mouseMoveEvent(event);
		}
	}
}

void TwoDWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
	if(s_pSail7)
	{
		if(m_iView==SAIL7VIEW)
		{
			QSail7    *pSail7    = (QSail7*)s_pSail7;
			pSail7->mouseDoubleClickEvent(event );
		}
	}
}


void TwoDWidget::wheelEvent(QWheelEvent *event)
{
	if(s_pSail7)
	{
		if(m_iView==SAIL7VIEW)
		{
			QSail7    *pSail7    = (QSail7*)s_pSail7;
			pSail7->wheelEvent(event);
		}
	}
}



void TwoDWidget::contextMenuEvent (QContextMenuEvent * event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QPoint ScreenPt = event->globalPos();
	QPoint CltPt = event->pos();

	if(m_iView==SAIL7VIEW)
	{
		QSail7 *pSail7= (QSail7*)s_pSail7;
		if(event->reason()==QContextMenuEvent::Keyboard)
		{
			ScreenPt.rx() = pSail7->m_LastPoint.x()+pMainFrame->pos().x()+geometry().x();
			ScreenPt.ry() = pSail7->m_LastPoint.y()+pMainFrame->pos().y()+geometry().y();
		}

		pSail7->m_pCurGraph = pSail7->GetGraph(CltPt);
		if (pSail7->m_iView==SAILPOLARVIEW) pMainFrame->SailPlrCtxMenu->exec(ScreenPt);
	}
}



void TwoDWidget::resizeEvent(QResizeEvent *event)
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QSail7    *pSail7    = (QSail7*)s_pSail7;
	if(m_iView==SAIL7VIEW && pSail7)
	{
		pSail7->m_bIs2DScaleSet = false;
		pSail7->Set2DScale();
//		if(pSail7->m_iView==SAILOPPVIEW)   pSail7->SetWingLegendPos();
		if(pSail7->m_iView==SAILPOLARVIEW) pSail7->SetBoatPlrLegendPos();
	}
	else
	{
	}
}


void TwoDWidget::paintEvent(QPaintEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView == SAIL7VIEW && s_pSail7)
	{
		QSail7* pSail7 = (QSail7*)s_pSail7;
		QPainter painter(this);
		pSail7->PaintView(painter);
	}
	else
	{
		QPainter painter(this);
		painter.fillRect(rect(), pMainFrame->m_BackgroundColor);
	}
}
