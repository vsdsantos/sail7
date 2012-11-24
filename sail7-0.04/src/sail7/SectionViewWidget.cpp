/****************************************************************************

		 SectionWidget Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include "../mainframe.h"
#include "./SailDlg.h"
#include "./SailDlg.h"
#include "../objects/NURBSSail.h"
#include "SectionViewWidget.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDesktopWidget>
#include <QMenu>
#include <QMessageBox>
#include <QtDebug>


void *SectionViewWidget::s_pMainFrame;
bool SectionViewWidget::s_bCurrentOnly=false;



SectionViewWidget::SectionViewWidget(QWidget *parent) :QWidget(parent)
{
	setWindowTitle("Section Edition");
	setMouseTracking(true);
	setCursor(Qt::CrossCursor);

	m_pSailDlg = parent;

	m_pSail = NULL;
	m_pSailSection = NULL;
	m_Scale = m_RefScale = m_ScaleY = 1.0;
	m_ptOffset = QPoint(0,0);

	m_bZoomYOnly   = false;
	m_bZoomIn      = false;
	m_bXDown       = false;
	m_bYDown       = false;
	m_bTrans       = false;
	m_bNeutralLine = true;

	m_NeutralColor= QColor(255, 255, 255);
	m_NeutralStyle = DASHDOTLINE;
	m_NeutralWidth = 1;
	m_StackPos = -1;

	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);

	m_pInsertPoint      = new QAction(tr("Insert Point") + QString("\tShift+Click"), this);
	m_pRemovePoint      = new QAction(tr("Remove Point") + QString("\tCtrl+Click"), this);
	m_pCurrentOnly      = new QAction(tr("Show Current Section Only"), this);
	m_pCurrentOnly->setCheckable(true);
	connect(m_pInsertPoint,    SIGNAL(triggered()), this, SLOT(OnInsertCtrlPt()));
	connect(m_pRemovePoint,    SIGNAL(triggered()), this, SLOT(OnRemoveCtrlPt()));
	connect(m_pCurrentOnly,    SIGNAL(triggered()), this, SLOT(OnCurrentSectionOnly()));
}


void SectionViewWidget::Connect()
{
}


void SectionViewWidget::OnCurrentSectionOnly()
{
	s_bCurrentOnly = !s_bCurrentOnly;
	m_pCurrentOnly->setChecked(s_bCurrentOnly);
	UpdateView();
}




CVector SectionViewWidget::MousetoReal(QPoint &point)
{
	CVector Real;

	Real.x =  (point.x() - m_ptOffset.x())/m_Scale;
	Real.y = -(point.y() - m_ptOffset.y())/m_Scale/m_ScaleY;
	Real.z = m_pSailSection->m_Position.z;

	//Rotate the point
	if(m_pSailSection->m_CtrlPoint.size())
		Real.RotateY(m_pSailSection->m_Position, m_pSailSection->m_Angle);

	int nada=0;
	++nada;
	return Real;
}


void SectionViewWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.save();

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	QPen LinePen;
	LinePen.setColor(QColor(255,0,0));
	LinePen.setWidth(2);
	painter.setPen(LinePen);
	painter.fillRect(rect(), pMainFrame->m_BackgroundColor);

	painter.setFont(pMainFrame->m_TextFont);

	if(m_bZoomIn&& !m_ZoomRect.isEmpty())
	{
		QRect ZRect = m_ZoomRect.normalized();
		QPen ZoomPen(QColor(100,100,100));
		ZoomPen.setStyle(Qt::DashLine);
		painter.setPen(ZoomPen);
		painter.drawRect(ZRect);
	}

	if(m_bNeutralLine)
	{
		QPen NPen(m_NeutralColor);
		NPen.setStyle(GetStyle(m_NeutralStyle));
		NPen.setWidth(m_NeutralWidth);
		painter.setPen(NPen);

		painter.drawLine(rect().right(), m_ptOffset.y(), rect().left(), m_ptOffset.y());
		painter.drawLine(m_ptOffset.x(), rect().bottom(), m_ptOffset.x(), rect().top());
	}

	if(!s_bCurrentOnly && m_pSail /* && m_pSail->IsSailcutSail()*/)
	{
		QColor clr = m_pSailSection->m_SCSpline.m_SplineColor;
		clr.setHsv(clr.hue(), (int)(clr.saturation()), (int)(clr.value()*.29));
		QPen OtherPen(clr);
		OtherPen.setStyle(Qt::DashLine);
		OtherPen.setWidth(1.0);
		painter.setPen(OtherPen);

		for(int is=0; is<m_pSail->m_oaSection.size(); is++)
		{
			SailSection *pSection = (SailSection*)m_pSail->m_oaSection.at(is);
			if(pSection != m_pSailSection)
			{
				if(m_pSail->IsSailcutSail()) pSection->DrawSpline(painter, m_Scale,m_Scale*m_ScaleY, m_ptOffset, false);
				else
				{
					NURBSSail *pNSail = (NURBSSail*)m_pSail;
					int index = m_pSail->m_oaSection.indexOf(m_pSailSection);
					if(is!=index) pNSail->DrawFrame(is, painter, m_Scale, m_Scale*m_ScaleY, m_ptOffset);

				}
			}
		}
	}

	if(m_pSailSection)
	{
		if(m_pSail->IsNURBSSail())
		{
			NURBSSail *pNSail = (NURBSSail*)m_pSail;
			int index = m_pSail->m_oaSection.indexOf(m_pSailSection);
			QPen SplinePen;
			SplinePen.setStyle(GetStyle(m_pSailSection->m_SCSpline.m_Style));
			SplinePen.setWidth(m_pSailSection->m_SCSpline.m_Width);
			SplinePen.setColor(m_pSailSection->m_SCSpline.m_SplineColor);
			painter.setPen(SplinePen);

//			if(index==0 || index==m_pSail->m_oaSection.size()-1)
			{
				pNSail->DrawFrame(index, painter, m_Scale, m_Scale*m_ScaleY, m_ptOffset);
			}

			m_pSailSection->DrawCtrlPoints(painter, m_Scale, m_Scale*m_ScaleY, m_ptOffset);
		}
		else
		{
			m_pSailSection->DrawSpline(painter, m_Scale, m_Scale*m_ScaleY, m_ptOffset);
		}
	}


	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);
	PaintLegend(painter);

	QString str;

	str = QString("X-Scale = %1").arg(m_Scale/m_RefScale,4,'f',1);
	painter.drawText(5,10, str);
	str = QString("Y-Scale = %1").arg(m_ScaleY*m_Scale/m_RefScale,4,'f',1);
	painter.drawText(5,22, str);
	str = QString("x  = %1").arg(m_MousePos.x,7,'f',4);
	painter.drawText(5,34, str);
	str = QString("y  = %1").arg(m_MousePos.y,7,'f',4);
	painter.drawText(5,46, str);

	painter.restore();
}



void SectionViewWidget::PaintLegend(QPainter &painter)
{
	if(!m_pSailSection) return;
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QString strlength;
	GetLengthUnit(strlength, pMainFrame->m_LengthUnit);

	painter.setFont(pMainFrame->m_TextFont);
	QFontMetrics fm(pMainFrame->m_TextFont);
	int dD = fm.height();//pixels

	QPoint Place(5, rect().bottom()-5*dD);

	if(m_pSail->IsNURBSSail()) Place.ry() -=dD;

	painter.setBackgroundMode(Qt::TransparentMode);

	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);

	if(m_pSail->IsNURBSSail() && m_pSailSection->IsTipSection())
	{
		QString strtwist = QString("Section chord = %1").arg(m_pSailSection->Chord()*pMainFrame->m_mtoUnit, 6,'f',2) + strlength;
		painter.drawText(Place.x(), Place.y() , strtwist);
	}

	if(m_pSail->IsSailcutSail() || m_pSailSection->IsTipSection())
	{
		double x, c;
		m_pSailSection->GetCamber(c, x);

		QString strc = QString("Camber        = %1").arg(c*100.0, 6, 'f',2);
		QString strx = QString("Camber pos    = %1").arg(x*100.0, 6, 'f',2);
		strc += QString::fromUtf8("%");
		strx += QString::fromUtf8("%");

		painter.drawText(Place.x(), Place.y() + dD, strx);
		painter.drawText(Place.x(), Place.y() + 2*dD, strc);
	}


	double s0,s1;
	m_pSailSection->GetSlopes(s0,s1);
	QString str0 = QString("Luff Slope    = %1").arg(s0,5,'f',1) + QString::fromUtf8("°");
	QString str1 = QString("Leech Slope   = %1").arg(s1,5,'f',1) + QString::fromUtf8("°");

	painter.drawText(Place.x(), Place.y() + 3*dD, str0);
	painter.drawText(Place.x(), Place.y() + 4*dD, str1);

	if(m_pSail->IsNURBSSail())
	{
		QString strtwist = QString("Section twist = %1").arg(m_pSailSection->Twist(), 5,'f',1) + QString::fromUtf8("°");
		painter.drawText(Place.x(), Place.y() + 5*dD, strtwist);
	}

	painter.restore();
}



void SectionViewWidget::OnResetScale()
{
	if(m_pSail)
	{
		m_RefScale = ((double)rect().width()-150.0)/m_pSail->FootLength();
	}
	else		m_RefScale =  (double)rect().width()-150.0;

	m_Scale = m_RefScale;

	m_ptOffset.rx() = 75;
	m_ptOffset.ry() = (int)(rect().height()/2);

	m_ViewportTrans = QPoint(0,0);
	UpdateView();
}


void SectionViewWidget::OnResetXScale()
{
}


void SectionViewWidget::OnResetYScale()
{
	m_ScaleY = 1.0;

	UpdateView();
}


void SectionViewWidget::OnZoomOut()
{
	ReleaseZoom();

	double ZoomFactor = 0.8;
	double newScale = qMax(ZoomFactor*m_Scale, m_RefScale);

	ZoomFactor = qMax(ZoomFactor, newScale/m_Scale);

	m_Scale = ZoomFactor*m_Scale;
	int a = (int)((rect().right()+rect().left())/2);
	int b = (int)((rect().top()+rect().bottom())/2);

	//scale
	m_ptOffset.rx() = (int)(ZoomFactor*(m_ptOffset.x()-a)+a);
	m_ptOffset.ry() = (int)(ZoomFactor*(m_ptOffset.y()-b)+b);

	UpdateView();
}


void SectionViewWidget::OnZoomIn()
{
	if(!m_bZoomIn)
	{
		if(m_Scale/m_RefScale <32.0)
		{
			m_bZoomIn = true;
			SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
			pSailDlg->m_pZoomIn->setChecked(m_bZoomIn);


		}
		else
		{
			ReleaseZoom();
		}
	}
	else
	{
		ReleaseZoom();
	}
}


void SectionViewWidget::OnZoomYScale()
{
	m_bZoomYOnly = !m_bZoomYOnly;
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	pSailDlg->m_pZoomYOnly->setChecked(m_bZoomYOnly);
}


void SectionViewWidget::Initialize(CSail *pSail, SailSection *pSection)
{
	m_pSail = pSail;
	m_pSailSection = pSection;
	Spline::s_bChanged = false;

	m_StackPos = -1;
	m_PicStack.clear();

	TakePicture();

	OnResetScale();
}


void SectionViewWidget::SetSection(SailSection *pSection)
{
	m_pSailSection = pSection;
	Spline::s_bChanged = false;

	m_StackPos = -1;
	m_PicStack.clear();

	TakePicture();
}


void SectionViewWidget::keyPressEvent(QKeyEvent *event)
{
//	bool bShift = (event->modifiers() & Qt::ShiftModifier);
//	bool bCtrl  = (event->modifiers() & Qt::ControlModifier);

	switch (event->key())
	{
		case Qt::Key_R:
		{
			OnResetScale();
			break;
		}
		case Qt::Key_Escape:
		{
			if(m_bZoomYOnly)
			{
				m_bZoomYOnly = true;
				OnZoomYScale();
			}
			else if(m_bZoomIn)
			{
				ReleaseZoom();
			}
			else event->ignore();
			break;
		}
		default:
		{
			event->ignore();
		}
	}
}



void SectionViewWidget::keyReleaseEvent(QKeyEvent *event)
{

}


void SectionViewWidget::OnInsertCtrlPt()
{
	//Inserts a point in all sections, to keep the same number of points in each
	if(!m_pSail->IsNURBSSail()) return;
	if(!m_pSailSection || !m_pSailSection->IsNURBSSection()) return;

	CVector Real = MousetoReal(m_PointDown);

	int pos = m_pSailSection->InsertPoint(Real, 1);
	for(int ifr=0; ifr<m_pSail->m_oaSection.size(); ifr++)
	{
		SailSection *pSailSection = m_pSail->m_oaSection.at(ifr);
		if(pSailSection != m_pSailSection) pSailSection->InsertPoint(pos);
	}

	m_pSail->SplineSurface();

	TakePicture();

	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	pSailDlg->FillPointTable();
	pSailDlg->UpdateView();
}


void SectionViewWidget::OnRemoveCtrlPt()
{
	//Removes a point in the spline
	if(!m_pSail->IsNURBSSail()) return;
	if(!m_pSailSection || !m_pSailSection->IsNURBSSection()) return;
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;

	CVector Real = MousetoReal(m_PointDown);

	int n =  m_pSailSection->IsPoint(Real, m_Scale/m_RefScale);
	if (n>=0)
	{

		for(int ifr=0; ifr<m_pSail->m_oaSection.size(); ifr++)
		{
			SailSection *pSailSection = m_pSail->m_oaSection.at(ifr);
			pSailSection->RemovePoint(n);
		}
	}

	//Check the degree
	NURBSSail *pNSail = (NURBSSail*)m_pSail;

	if(pNSail->m_SplineSurface.m_ivDegree >= m_pSailSection->m_CtrlPoint.size())
	{
		//reduce the degree
		pNSail->m_SplineSurface.m_ivDegree = m_pSailSection->m_CtrlPoint.size()-1;
		pSailDlg->m_pctrlNXDegree->SetValue(pNSail->m_SplineSurface.m_ivDegree);
		m_pSail->SplineSurface();
		QString strong = tr("The degree has been reduced to be less than the number of points");
		QMessageBox::warning(window(), tr("Warning"), strong);
	}
	else
	{
		m_pSail->SplineSurface();
	}

	TakePicture();
	pSailDlg->FillPointTable();
}


void SectionViewWidget::mousePressEvent(QMouseEvent *event)
{
	if(!m_pSailSection) return;
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	QPoint point = event->pos();
	CVector Real = MousetoReal(point);
//	Real.z = m_pSailSection->m_Position.z;

	// get a reference for mouse movements
	m_PointDown.rx() = point.x();
	m_PointDown.ry() = point.y();

	if(m_bZoomIn && rect().contains(point))
	{
		m_ZoomRect.setTopLeft(point);
		m_ZoomRect.setBottomRight(point);
	}
	else if(!m_bZoomIn && (event->buttons() & Qt::LeftButton))
	{
		if (event->modifiers() & Qt::ShiftModifier)
		{
			//shift --> inserts a point
			OnInsertCtrlPt();
		}
		else if (event->modifiers() & Qt::ControlModifier)
		{
			//Ctrl --> removes the point
			OnRemoveCtrlPt();
		}
		else if(m_pSailSection && m_pSail->IsNURBSSail())
		{
			//Selects the point
			m_pSailSection->m_iSelect = m_pSailSection->IsPoint(Real, m_Scale/m_RefScale);
			pSailDlg->m_pctrlPointTable->selectRow(m_pSailSection->m_iSelect);

			NURBSSail *pNSail = (NURBSSail*)m_pSail;
			if(pNSail->m_bAlignedLE)
			{
				if(m_pSailSection==pNSail->m_oaSection[1] && m_pSailSection->m_iSelect==0)
					m_pSailSection->m_iSelect = -1;
			}

			if (m_pSailSection->m_iSelect>=0)
			{
//					TakePicture();
			}

			if(m_pSailSection->m_iSelect ==-10)
			{
				setCursor(m_hcMove);
				m_bTrans = true;
			}
		}
		else
		{
			setCursor(m_hcMove);
			m_bTrans = true;
		}
	}
	UpdateView();
}


void SectionViewWidget::UpdateView()
{
	update();
}


void SectionViewWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(!m_pSailSection) return;
	if(!hasFocus()) setFocus();
	QPoint point = event->pos();
	m_MousePos = MousetoReal(point);
//	m_MousePos.z = m_pSailSection->m_Position.z;
	QRect CltRect = rect();

//	bool bCtrl = (event->modifiers() & Qt::ControlModifier) ;

	if(m_bZoomIn && (event->buttons() & Qt::LeftButton))
	{
		// we're zooming in using the rectangle method
		m_ZoomRect.setBottomRight(point);
		UpdateView();
		return;
	}
	else if(CltRect.contains(point) && (event->buttons() & Qt::LeftButton) && m_bTrans)
	{
		//translate
		m_ptOffset.rx() += point.x() - m_PointDown.x();
		m_ptOffset.ry() += point.y() - m_PointDown.y();
		m_ViewportTrans.rx() += point.x() - m_PointDown.x();
		m_ViewportTrans.ry() += point.y() - m_PointDown.y();

		m_PointDown.rx() = point.x();
		m_PointDown.ry() = point.y();
//		m_MousePos = Real;

		UpdateView();
		return;
	}

	if (event->buttons() & Qt::LeftButton && !m_bZoomIn)
	{
		// user is dragging the point
		if(CltRect.contains(point)  && m_pSail->IsNURBSSail())
		{
			int n = m_pSailSection->m_iSelect;

			if (n>=0 && n<m_pSailSection->m_CtrlPoint.size())
			{
//				if(n==1) m_MousePos.x = 0.0;// we can't move point 1 for vertical slope
				m_pSailSection->m_CtrlPoint[n].x = m_MousePos.x;
				m_pSailSection->m_CtrlPoint[n].y = qMax(m_MousePos.y, 0.0);
				m_pSailSection->m_CtrlPoint[n].z = m_MousePos.z;

				NURBSSail *pNSail = (NURBSSail*)m_pSail;
				if(pNSail->m_bAlignedLE && n==0)
				{
					// keep middle point aligned
					pNSail->m_oaSection[1]->m_CtrlPoint[0].x  = (pNSail->m_oaSection[0]->m_CtrlPoint[0].x + pNSail->m_oaSection[2]->m_CtrlPoint[0].x)/2.0;
					pNSail->m_oaSection[1]->m_CtrlPoint[0].y  = (pNSail->m_oaSection[0]->m_CtrlPoint[0].y + pNSail->m_oaSection[2]->m_CtrlPoint[0].y)/2.0;
				}

				Spline::s_bChanged = true;
			}
		}
	}
	else if ((event->buttons() & Qt::MidButton))
	{
		// user is zooming with mouse button down rather than with wheel
		if(CltRect.contains(point))
		{
			double scale = m_Scale;

			if(!m_bZoomYOnly)
			{
				if (m_bXDown)
				{
					if(point.y()-m_PointDown.y()>0)
					{
						m_Scale  *= 1.02;
						m_ScaleY /= 1.02;
					}
					else
					{
						m_Scale  /= 1.02;
						m_ScaleY *= 1.02;
					}
				}
				else if (m_bYDown)
				{
					if(point.y()-m_PointDown.y()>0) m_ScaleY *= 1.02;
					else                            m_ScaleY /= 1.02;
				}
				else
				{
					if(point.y()-m_PointDown.y()>0) m_Scale *= 1.02;
					else		                    m_Scale /= 1.02;
				}
			}
			else
			{
				if(point.y()-m_PointDown.y()>0) m_ScaleY *= 1.02;
				else                            m_ScaleY /= 1.02;
			}

			m_PointDown = point;

			int a = (int)((CltRect.right()+CltRect.left())/2);
			m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_Scale/scale);
		}
	}
	else if(!m_bZoomIn)
	{
		//not zooming, check if mouse passes over control point and highlight
		int n = m_pSailSection->IsPoint(m_MousePos, m_Scale/m_RefScale);
		if (n>=0 && n<m_pSailSection->m_CtrlPoint.size())
		{
			m_pSailSection->m_iHighlight = n;
		}
		else if(m_pSailSection->m_iHighlight>=0)
		{
			m_pSailSection->m_iHighlight = -10;
		}
	}
	UpdateView();
}



void SectionViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
	m_bTrans = false;
	QPoint point = event->pos();
	QRect CltRect = rect();
	if(m_bZoomIn && CltRect.contains(point))
	{
		m_ZoomRect.setBottomRight(point);
		QRect ZRect = m_ZoomRect.normalized();

		if(!ZRect.isEmpty())
		{
			m_ZoomRect = ZRect;

			double ZoomFactor = qMin((double)CltRect.width()  / (double)m_ZoomRect.width() ,
									 (double)CltRect.height() / (double)m_ZoomRect.height());

			double newScale = qMin(ZoomFactor*m_Scale, 32.0*m_RefScale);

			ZoomFactor = qMin(ZoomFactor, newScale/m_Scale);

			m_Scale = ZoomFactor*m_Scale;
			int a = (int)((CltRect.right() + CltRect.left())/2);
			int b = (int)((CltRect.top()   + CltRect.bottom())/2);

			int aZoom = (int)((m_ZoomRect.right() + m_ZoomRect.left())/2);
			int bZoom = (int)((m_ZoomRect.top()   + m_ZoomRect.bottom())/2);

			//translate view
			m_ptOffset.rx() += (a - aZoom);
			m_ptOffset.ry() += (b - bZoom);
			//scale view
			m_ptOffset.rx() = (int)(ZoomFactor * (m_ptOffset.x()-a)+a);
			m_ptOffset.ry() = (int)(ZoomFactor * (m_ptOffset.y()-b)+b);

//			m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
			m_ZoomRect.setRight(m_ZoomRect.left()-1);
		}
		else
		{
			m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
			ReleaseZoom();
		}
	}
	else if(m_bZoomIn && !CltRect.contains(point))
	{
		ReleaseZoom();
	}
	else
	{
		//The user has finished dragging something
		if (m_pSailSection->m_iSelect>=0) TakePicture();

		SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
		pSailDlg->FillPointTable();
		m_pSail->SplineSurface();
		pSailDlg->UpdateSailView();
	}

	setCursor(m_hcCross);

	UpdateView();
}


void SectionViewWidget::ReleaseZoom()
{
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	m_bZoomIn = false;
	pSailDlg->m_pZoomIn->setChecked(m_bZoomIn);

	m_ZoomRect.setRight(m_ZoomRect.left()-1);
	m_ZoomRect.setTop(m_ZoomRect.bottom()+1);
	setCursor(m_hcCross);
}



void SectionViewWidget::wheelEvent (QWheelEvent *event )
{
	MainFrame * pMainFrame = (MainFrame*)s_pMainFrame;
	static double ZoomFactor, scale;

	if(! rect().contains(event->pos())) return;

	m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
	ReleaseZoom();

	if(event->delta()>0)
	{
		if(!pMainFrame->m_bReverseZoom) ZoomFactor = 1./1.06;
		else                            ZoomFactor = 1.06;
	}
	else
	{
		if(!pMainFrame->m_bReverseZoom) ZoomFactor = 1.06;
		else                            ZoomFactor = 1./1.06;
	}

	scale = m_Scale;

	if(!m_bZoomYOnly)
	{
		if (m_bXDown)
		{
			m_Scale  *= ZoomFactor;
			m_ScaleY *= 1./ZoomFactor;
		}
		else if (m_bYDown) m_ScaleY *= ZoomFactor;
		else  m_Scale *= ZoomFactor;
	}
	else m_ScaleY *= ZoomFactor;


	int a = (int)((rect().right() + rect().left())/2);
	m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_Scale/scale);
	m_ViewportTrans.rx() = (int)((m_ViewportTrans.x())*m_Scale /scale);
	m_ViewportTrans.ry() = (int)((m_ViewportTrans.y())*m_Scale /scale);

	UpdateView();
}



void SectionViewWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
	QPoint point = event->pos();
	QPoint center;
	//translate
	center.rx() = (int)(rect().width()/2);
	center.ry() = (int)(rect().height()/2);

	m_ptOffset.rx() += -point.x() + center.x();
	m_ptOffset.ry() += -point.y() + center.y();
	m_ViewportTrans.rx() += -point.x() + center.x();
	m_ViewportTrans.ry() += -point.y() + center.y();

	UpdateView();
	return;
}


void SectionViewWidget::contextMenuEvent (QContextMenuEvent * event)
{
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	QMenu *CtxMenu = new QMenu(tr("Context Menu"),this);
	CtxMenu->addAction(m_pInsertPoint);
	CtxMenu->addAction(m_pRemovePoint);
	CtxMenu->addSeparator();
	CtxMenu->addAction(pSailDlg->m_pZoomYOnly);
	CtxMenu->addSeparator();
	CtxMenu->addAction(pSailDlg->m_pResetScales);
	CtxMenu->addAction(pSailDlg->m_pResetYScale);
	CtxMenu->addSeparator();
	CtxMenu->addAction(m_pCurrentOnly);

	QPoint CltPt = event->pos();
	m_ptPopUp.rx() = CltPt.x();
	m_ptPopUp.ry() = CltPt.y();

	CtxMenu->exec(event->globalPos());

	setCursor(Qt::CrossCursor);
}


void SectionViewWidget::resizeEvent(QResizeEvent *event)
{
	OnResetScale();
}

void SectionViewWidget::ClearStack()
{
	m_StackPos = -1;
	m_PicStack.clear();
}


void SectionViewWidget::TakePicture()
{
//	if(m_pSailSection) m_TmpPic.Duplicate(*m_pSailSection);
	while(m_PicStack.size()>m_StackPos+1 && m_PicStack.size()>0)
	{
		m_PicStack.remove(m_PicStack.size()-1);
	}
	BSpline StoreSpline;
	StoreSpline.CopyPoints(&m_pSailSection->m_CtrlPoint);
	m_PicStack.append(StoreSpline);

	m_StackPos++;

	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	pSailDlg->SetControls();
}


void SectionViewWidget::SetPicture()
{
	if(!m_PicStack.isEmpty() && m_StackPos>=0 && m_StackPos<m_PicStack.size())
	{
		BSpline SetSpline = m_PicStack.at(m_StackPos);
		m_pSailSection->CopyPoints(&SetSpline.m_CtrlPoint);
		SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
		pSailDlg->UpdateSailView();
	}
	UpdateView();
}



void SectionViewWidget::OnUndo()
{
	if(m_StackPos>0)
	{
		m_StackPos--;
		SetPicture();
	}
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	pSailDlg->SetControls();
}


void SectionViewWidget::OnRedo()
{
	if(m_StackPos<m_PicStack.size()-1)
	{
		m_StackPos++;
		SetPicture();
	}
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;
	pSailDlg->SetControls();
}

