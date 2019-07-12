/****************************************************************************

		 BoatDlg Class
		 Copyright (C) 2011-2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/


#ifndef BOATDLG_H
#define BOATDLG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QTabWidget>
#include <QTableView>
#include <QStandardItemModel>

#include "../objects/Boat.h"
#include "../objects/ArcBall.h"
#include "../misc/FloatEdit.h"
#include "../misc/FloatEditDelegate.h"


class BoatDlg : public QDialog
{
    Q_OBJECT
	friend class MainFrame;
	friend class Sail7;
	friend class CBoatDef;
	friend class Sail;


public:
	BoatDlg();


private slots:
	void OnAddNURBSSail();
	void OnAddSailcutSail();
	void OnImportNURBSSail();
	void OnImportSailcutSail();
	void OnDeleteSail();
	void OnEditSail();
	void OnImportSail();
	void OnExportSail();
	void OnAddHull();
	void OnDeleteHull();
	void OnEditHull();
	void OnDuplicateSail();
	void OnDuplicateHull();
	void OnGetHull();
	void OnImportHull();
	void OnOK();
	void OnCancel();

	void OnSailItemClicked(const QModelIndex &index);
	void OnSailCellChanged(QWidget *pWidget);
	void OnHullItemClicked(const QModelIndex &index);
	void OnHullCellChanged(QWidget *pWidget);

public:
	void InitDialog(Boat *pBoatDef);

	void keyPressEvent(QKeyEvent *event);

private:
	void SetupLayout();
	void SetControls();
	void Connect();
	void FillSailList();
	void FillHullList();
	void ReadData();
	void SetSail(int iSelect){if(iSelect>=0) m_pctrlSailTable->selectRow(iSelect);};
	void SetHull(int iSelect){if(iSelect>=0) m_pctrlHullTable->selectRow(iSelect);};


	//private variables:
private:
	static void *s_pMainFrame;
	static void *s_pSail7;

	Boat *m_pBoat;

//	QTabWidget *m_pTabWidget;
//	QWidget *SailPage, *HullPage;

	QLineEdit *m_pctrlBoatName;

	QTableView *m_pctrlSailTable, *m_pctrlHullTable;
	QStandardItemModel *m_pSailModel, *m_pHullModel;
	FloatEditDelegate *m_pSailDelegate, *m_pHullDelegate;

	QPushButton *m_pctrlAddSail, *m_pctrlImportXMLSail;
	QAction *m_pAddNURBSSail, *m_pAddSailcutSail;
	QAction *m_pImportNURBSSail, *m_pImportSailcutSail;

	QPushButton *m_pctrlRemoveSail, *m_pctrlImportSail, *m_pctrlExportXMLSail, *m_pctrlEditSail, *m_pctrlDuplicateSail;
	QPushButton *m_pctrlAddHull, *m_pctrlRemoveHull, *m_pctrlGetHull, *m_pctrlImportHull, *m_pctrlEditHull, *m_pctrlDuplicateHull;

	QLabel *m_pctrlLen1, *m_pctrlLen2, *m_pctrlLen3, *m_pctrlLen4, *m_pctrlLen5, *m_pctrlLen6;
	QPushButton *OKButton, *CancelButton;

	bool m_bChanged;

//	int m_iSailSelection, m_iHullSelection;
};

#endif // CBoatDlg_H





