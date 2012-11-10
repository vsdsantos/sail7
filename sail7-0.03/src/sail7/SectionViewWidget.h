/****************************************************************************

		 SectionWidget Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#ifndef SECTIONWIDGET_H
#define SECTIONWIDGET_H

#include <QPushButton>
#include <QRadioButton>
#include <QMouseEvent>
#include <QVector>

#include "../globals.h"
#include "../twodwidget.h"
#include "../misc/FloatEdit.h"
#include "../objects/Sail.h"
#include "../objects/SailSection.h"
#include "../objects/BSpline.h"

class SectionViewWidget : public QWidget
{
	Q_OBJECT

	friend class MainFrame;
	friend class SailDlg;

public:
	SectionViewWidget(QWidget *parent = NULL);
	
	
public slots:
	void OnInsertCtrlPt();
	void OnRemoveCtrlPt();
	void OnResetScale();
	void OnResetXScale();
	void OnResetYScale();
	void OnZoomOut();
	void OnZoomIn();
	void OnZoomYScale();
	void OnCurrentSectionOnly();
	void OnUndo();
	void OnRedo();

private:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event );
	void mouseDoubleClickEvent ( QMouseEvent * event );
	void contextMenuEvent (QContextMenuEvent * event);

	void Connect();
	void Initialize(CSail *pSail, SailSection *pSection);
	void SetSection(SailSection *pSection);
	void PaintLegend(QPainter &painter);
	void ReleaseZoom();
	void UpdateView();

	void TakePicture();
	void SetPicture();
	void ClearStack();

	CVector MousetoReal(QPoint &point);



	static void *s_pMainFrame;
	static bool s_bCurrentOnly;
	void *m_pSailDlg;

	QColor m_NeutralColor;
	int m_NeutralStyle, m_NeutralWidth;

	QPoint m_ptOffset;
	CVector m_MousePos;
	QPoint m_ViewportTrans;
	QPoint m_ptPopUp;
	QPoint m_PointDown;
	QRect m_ZoomRect;

	QCursor m_hcMove;
	QCursor m_hcCross;

	double m_Scale, m_RefScale, m_ScaleY;
	bool m_bNeutralLine;
	bool m_bXDown, m_bYDown, m_bZoomIn, m_bZoomYOnly;
	bool m_bTrans;

	int m_StackPos;// undo : stack position and stack size
	QVector<BSpline> m_PicStack;//we use BSpline instances to store the ctrl points


	QAction *m_pInsertPoint, *m_pRemovePoint, *m_pCurrentOnly;


	CSail *m_pSail;
	SailSection *m_pSailSection;
};

#endif // SectionWidget_H
