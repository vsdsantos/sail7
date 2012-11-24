/****************************************************************************

	GraphDlg  Classes
	Copyright (C) 2008 Andre Deperrois sail7@xflr5.com

	All rights reserved


*****************************************************************************/

#ifndef GRAPHDLG_H
#define GRAPHDLG_H

#include <QDialog>
#include <QTabWidget> 
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>

#include "../graph/graph.h"
#include "../misc/FloatEdit.h"
#include "../misc/ColorButton.h"
#include "../misc/LineButton.h"

class GraphDlg : public QDialog
{
	Q_OBJECT
	friend class Graph;


public:
	GraphDlg();
	void SetParams();

private slots:
	void OnOK();
	void OnApply();
	void OnTitleFont();
	void OnLabelFont();
	void OnTitleColor();
	void OnLabelColor();
	void OnRestoreParams();
	void OnAutoX();
	void OnAutoY();
	void OnAutoMinGrid();
	void OnAxisStyle();
	void OnXMajGridStyle();
	void OnXMinGridStyle();
	void OnYMajGridStyle();
	void OnYMinGridStyle();
	void OnXMajGridShow(int state);
	void OnYMajGridShow(int state);
	void OnXMinGridShow(int state);
	void OnYMinGridShow(int state);
	void OnGraphBorder(int state);
	void OnGraphBackColor();
	void OnBorderStyle();
	void OnYInverted();
	void OnActivePage(int index);
	void OnVariableChanged();

private:
	void keyPressEvent(QKeyEvent *event);
	void SetupLayout();
	void SetButtonColors();
	void SetApplied(bool bApplied);
	void Connect();

	void FillVariableList();

	QTabWidget *m_pTabWidget;
	QWidget *ScalePage, *FontPage, *GridPage;
	QWidget *VariablePage;

	QListWidget *m_pctrlXSel, *m_pctrlYSel;

	QPushButton *OKButton,*CancelButton, *RestoreButton, *ApplyButton;
	QPushButton *m_pctrlTitleButton, *m_pctrlLabelButton;
	QPushButton*m_pctrlTitleClr, *m_pctrlLabelClr;
	QLabel *m_pctrlTitleLabel, *m_pctrlLabelLabel;

	QCheckBox *m_pctrlXAuto, *m_pctrlYAuto, *m_pctrlYInverted;
	FloatEdit *m_pctrlXMin, *m_pctrlXMax, *m_pctrlXOrigin,*m_pctrlXUnit;
	FloatEdit *m_pctrlYMin, *m_pctrlYMax, *m_pctrlYOrigin,*m_pctrlYUnit;

	QCheckBox *m_pctrlXMajGridShow, *m_pctrlYMajGridShow, *m_pctrlXMinGridShow, *m_pctrlYMinGridShow;
	QCheckBox *m_pctrlAutoXMinUnit, *m_pctrlAutoYMinUnit;
	LineButton *m_pctrlAxisStyle, *m_pctrlXMajGridStyle, *m_pctrlYMajGridStyle, *m_pctrlXMinGridStyle, *m_pctrlYMinGridStyle;
	FloatEdit *m_pctrlXMinorUnit, *m_pctrlYMinorUnit;

	QCheckBox *m_pctrlGraphBorder;
	ColorButton *m_pctrlGraphBack;
	LineButton *m_pctrlBorderStyle;

	QFont *m_pTitleFont, *m_pLabelFont;

	bool m_bApplied;

public:
	Graph *m_pGraph, *m_pMemGraph;
	Graph *m_GraphArray[30];
	int m_NGraph;
	int m_XSel, m_YSel;
	int m_iGraphType;
	bool m_bVariableChanged;

	static int s_ActivePage;
	static void *s_pMainFrame;
};

#endif // GRAPHDLG_H
