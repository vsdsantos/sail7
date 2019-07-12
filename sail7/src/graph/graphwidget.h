/****************************************************************************

	GraphWidget Class
        Copyright (C) 2008 Andre Deperrois sail7@xflr5.com

	All rights reserved


*****************************************************************************/


#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include "qgraph.h"

class GraphWidget : public QWidget
{
	friend class XFoilAnalysisDlg;
	friend class LLTAnalysisDlg;
	friend class BatchDlg;

public:
		GraphWidget();

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent (QResizeEvent *event);

	void SetTitle(QString &Title, QPoint &Place);

/*	QWidget *ScalePage;
	QWidget *AxisPage;
	QWidget *FontPage;
	QWidget *BackPage;*/
	QPoint m_TitlePosition;
	QString m_GraphTitle;
	QGraph *m_pGraph;

};

#endif // GRAPHWIDGET_H
