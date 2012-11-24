/****************************************************************************

		 SailDlg Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#ifndef SAILDLG_H
#define SAILDLG_H

#include <QDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QRadioButton>
#include <QToolButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QAction>
#include <QStackedWidget>

#include "./SailViewWidget.h"
#include "./SectionViewWidget.h"
#include "../misc/ColorButton.h"
#include "../misc/FloatEditDelegate.h"
#include "../objects/ArcBall.h"
#include "../objects/CubicSpline.h"
#include "../objects/Sail.h"
#include "../threedwidget.h"


class SailDlg : public QDialog
{
	Q_OBJECT

private slots:
	void OnSectionCellChanged(QWidget *pWidget);
	void OnSectionItemClicked(const QModelIndex &index);
	void OnPointCellChanged(QWidget *pWidget);
	void OnPointItemClicked(const QModelIndex &index);
	void OnTwistChanged();
	void OnAlignedLE();
	void OnInsertBefore();
	void OnInsertAfter();
	void OnDeleteSection();
	void OnUpdate();
	void OnUpdateLeechRound();
	void OnSailColor();
	void OnImportSail();
	void OnExportSail();
	void OnScaleSail();
	void OnOK();
	void OnCancel();


public:
	SailDlg();
	void showEvent(QShowEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);


	void Connect();
	void FillSectionTable();
	void FillPointTable();
	static void LoadSettings(QSettings *pSettings);
	static void SaveSettings(QSettings *pSettings);
	void SetControls();
	void SetupLayout();
//	void PaintSectionView(QPainter &painter);
//	void Set2DScale(QRect rect);
	void ReadData();
	void ReadSectionData();
	void ReadPointData();
	void SetSailData(CSail *pSail);
	void UpdateSailDataOutput();
	void UpdateView();
	void UpdateSailView();

	bool InitDialog(CSail *pSail);
	bool eventFilter( QObject* o, QEvent* e );

	static void *s_pMainFrame;
	static void *s_pSail7;
	static QPoint s_WindowPos;
	static QSize  s_WindowSize;
	static bool s_bWindowMaximized;


	QLineEdit *m_pctrlSailName;
	QLabel *m_pctrlLength1, *m_pctrlLength2, *m_pctrlLength3, *m_pctrlLength4, *m_pctrlLength5, *m_pctrlLength6;
	QLabel *m_pctrlAreaUnit1, *m_pctrlAreaUnit2;

	QTableView *m_pctrlSectionTable;
	QStandardItemModel *m_pSectionModel;
	FloatEditDelegate *m_pSectionDelegate;

	QStackedWidget *m_pctrlStackedSailWidget ;
	QTableView *m_pctrlPointTable;
	QStandardItemModel *m_pPointModel;
	FloatEditDelegate *m_pPointDelegate;

	QWidget *m_pctrlSectionCommands;
	QGroupBox *m_pctrlNURBSBox;

	QPushButton *m_pctrlInsertBefore, *m_pctrlInsertAfter, *m_pctrlDeleteSection;
	QPushButton *m_pctrlScaleSail, *m_pctrlExportSail, *m_pctrlImportSail;
	QPushButton *m_pctrlOK, *m_pctrlCancel;
	FloatEdit *m_pctrlNXDegree, *m_pctrlNZDegree, *m_pctrlEdgeWeightu, *m_pctrlEdgeWeightv;
	QLabel *m_pctrlNXDegMin,  *m_pctrlNZDegMin;
	FloatEdit *m_pctrlNXPanels, *m_pctrlNZPanels;
	FloatEdit *m_pctrlLuffAngle;
	FloatEdit *m_pctrlLeechRound, *m_pctrlLeechRoundPos;
	FloatEdit *m_pctrlGaffTwist;

	QCheckBox *m_pctrlAlignedLE;

	QLabel *m_pctrlLeechLength, *m_pctrlLuffLength, *m_pctrlFootLength;

	QLineEdit *m_pctrlWingName;
	ColorButton *m_pctrlSailColor;

	SailViewWidget *m_pSailWidget;
	SectionViewWidget *m_pSectionWidget;


	QAction *m_pResetScales , *m_pResetYScale;
	QAction *m_pZoomIn, *m_pZoomOut, *m_pZoomYOnly;
	QAction *m_pUndo, *m_pRedo;
	QAction *m_pResetXScale;

	CSail *m_pSail;
	bool m_bChanged;




//	CVector m_LA, m_LB, m_TA, m_TB;
	CVector m_Normal;

	int m_iSection;
};

#endif // SAILDLG_H
