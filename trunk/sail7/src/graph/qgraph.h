/****************************************************************************

	QGraph Classes
	Copyright (C) 2008-2010 Andre Deperrois 

	All rights reserved

*****************************************************************************/

#ifndef QGraph_H
#define QGraph_H

#include "graph.h"
#include "curve.h"
#include "../objects/CRectangle.h"
#include <QRect>
#include <QSettings>
#include <QPoint>

class QGraph : public Graph
{
public: 
	QGraph();
	virtual ~QGraph();

	void DrawGraph(QRect const & rect, QPainter &painter);
	void DrawGraph(QPainter &painter);
	void DrawAxes(QPainter &painter);
	void DrawCurve(int nIndex, QPainter &painter);
	void DrawLegend(QPainter &painter, QPoint &Place, QFont &LegendFont, QColor &LegendColor);
	void DrawTitles(QPainter &painter);
	void DrawXMinGrid(QPainter &painter);
	void DrawYMinGrid(QPainter &painter);
	void DrawXMajGrid(QPainter &painter);
	void DrawYMajGrid(QPainter &painter);
	void DrawXTicks(QPainter &painter);
	void DrawYTicks(QPainter &painter);
	void ExpFormat(double &f, int &exp);
	void ExportToFile(QFile &XFile, int FileType);
	void Highlight(QPainter &painter, CCurve *pCurve, int ref);

	void LoadSettings(QSettings *pSettings);
	void SaveSettings(QSettings *pSettings);
	QPoint GetOffset();


public:
	void *m_pParent;
	bool m_bHighlightPoint;
};

#endif
