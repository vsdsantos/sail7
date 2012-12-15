/****************************************************************************

		 BoatDlg Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#include <QtDebug>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include "BoatDlg.h"
#include "./GL3dBodyDlg.h"
#include "SailDlg.h"
#include "sail7.h"
#include "../mainframe.h"
#include "../globals.h"
#include "../misc/SelectObjectDlg.h"
#include "../objects/NURBSSail.h"
#include "../objects/SailcutSail.h"

void * BoatDlg::s_pMainFrame;
void * BoatDlg::s_pSail7;



BoatDlg::BoatDlg()
{
	setWindowTitle("Boat definition dialog");
	m_pBoat = NULL;
	SetupLayout();
	Connect();
}


void BoatDlg::SetupLayout()
{
	QHBoxLayout *BoatNameLayout = new QHBoxLayout;
	{
		QLabel *labBoatName = new QLabel(tr("Boat Name"));
		m_pctrlBoatName = new QLineEdit;
		m_pctrlBoatName->setText("First Boat");
		BoatNameLayout->addWidget(labBoatName);
		BoatNameLayout->addWidget(m_pctrlBoatName);
	}

	QTabWidget *pTabWidget = new QTabWidget(this);
	QWidget *SailPage = new QWidget(this);
	QWidget *HullPage = new QWidget(this);
	pTabWidget->addTab(SailPage, tr("Sails"));
	pTabWidget->addTab(HullPage, tr("Hulls"));

	pTabWidget->setCurrentIndex(0);

	QHBoxLayout *SailPageLayout = new QHBoxLayout;
	{
		m_pctrlSailTable = new QTableView;
		m_pctrlSailTable->setMinimumHeight(300);
		m_pctrlSailTable->setMinimumWidth(200);
		m_pctrlSailTable->setWindowTitle(QObject::tr("Sail definition"));
		m_pctrlSailTable->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pctrlSailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pctrlSailTable->setEditTriggers(QAbstractItemView::CurrentChanged |
										  QAbstractItemView::DoubleClicked |
										  QAbstractItemView::SelectedClicked |
										  QAbstractItemView::EditKeyPressed |
										  QAbstractItemView::AnyKeyPressed);
		connect(m_pctrlSailTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnEditSail()));

		QVBoxLayout *SailCommandLayout = new QVBoxLayout;
		{
			m_pctrlAddSail    = new QPushButton(tr("Add Sail"));
			m_pAddNURBSSail   = new QAction("NURBS Type", this);
			m_pAddSailcutSail = new QAction("Sailcut Type", this);
			QMenu *pSailSelMenu = new QMenu(tr("Actions..."),this);
			pSailSelMenu->addAction(m_pAddNURBSSail);
			pSailSelMenu->addAction(m_pAddSailcutSail);
			m_pctrlAddSail->setMenu(pSailSelMenu);

			m_pctrlImportSail = new QPushButton(tr("Get Sail"));
			m_pctrlEditSail   = new QPushButton(tr("Edit Sail"));
			m_pctrlDuplicateSail   = new QPushButton(tr("Duplicate Sail"));
			m_pctrlRemoveSail = new QPushButton(tr("Delete Sail"));
			m_pctrlImportXMLSail = new QPushButton(tr("Import Sail from XML file"));
			m_pImportNURBSSail   = new QAction("NURBS Type", this);
			m_pImportSailcutSail = new QAction("Sailcut Type", this);
			QMenu *pSailImportMenu = new QMenu(tr("Import..."),this);
			pSailImportMenu->addAction(m_pImportNURBSSail);
			pSailImportMenu->addAction(m_pImportSailcutSail);
			m_pctrlImportXMLSail->setMenu(pSailImportMenu);
			m_pctrlExportXMLSail = new QPushButton(tr("Export Sail to XML file"));
			SailCommandLayout->addWidget(m_pctrlAddSail);
			SailCommandLayout->addWidget(m_pctrlImportSail);
			SailCommandLayout->addWidget(m_pctrlEditSail);
			SailCommandLayout->addWidget(m_pctrlDuplicateSail);
			SailCommandLayout->addWidget(m_pctrlRemoveSail);
			SailCommandLayout->addWidget(m_pctrlImportXMLSail);
			SailCommandLayout->addWidget(m_pctrlExportXMLSail);
			SailCommandLayout->addStretch(1);
		}
		SailPageLayout->addWidget(m_pctrlSailTable);
		SailPageLayout->addLayout(SailCommandLayout);

		SailPage->setLayout(SailPageLayout);
	}

	QHBoxLayout *HullPageLayout = new QHBoxLayout;
	{
		m_pctrlHullTable = new QTableView;
		m_pctrlHullTable->setMinimumHeight(300);
		m_pctrlHullTable->setMinimumWidth(200);
		m_pctrlHullTable->setWindowTitle(QObject::tr("Hull definition"));
		m_pctrlHullTable->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pctrlHullTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pctrlHullTable->setEditTriggers(QAbstractItemView::CurrentChanged |
									QAbstractItemView::DoubleClicked |
									QAbstractItemView::SelectedClicked |
									QAbstractItemView::EditKeyPressed |
									QAbstractItemView::AnyKeyPressed);

		QVBoxLayout *HullCommandLayout = new QVBoxLayout;
		{
			m_pctrlAddHull    = new QPushButton(tr("Add Hull"));
			m_pctrlGetHull    = new QPushButton(tr("Get Hull"));
			m_pctrlGetHull->setToolTip(tr("Select an existing hull in another boat"));
			m_pctrlImportHull = new QPushButton(tr("Import Hull"));
			m_pctrlImportHull->setToolTip(tr("Import a hull definition from a text file"));
			m_pctrlEditHull   = new QPushButton(tr("Edit Hull"));
			m_pctrlDuplicateHull = new QPushButton(tr("Duplicate Hull"));
			m_pctrlRemoveHull = new QPushButton(tr("Delete Hull"));
			HullCommandLayout->addWidget(m_pctrlAddHull);
			HullCommandLayout->addWidget(m_pctrlGetHull);
			HullCommandLayout->addWidget(m_pctrlEditHull);
			HullCommandLayout->addWidget(m_pctrlDuplicateHull);
			HullCommandLayout->addWidget(m_pctrlRemoveHull);
			HullCommandLayout->addWidget(m_pctrlImportHull);
			HullCommandLayout->addStretch(1);
		}
		HullPageLayout->addWidget(m_pctrlHullTable);
		HullPageLayout->addLayout(HullCommandLayout);

		HullPage->setLayout(HullPageLayout);
	}

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("OK"));
		CancelButton = new QPushButton(tr("Cancel"));
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(CancelButton);
		CommandButtons->addStretch(1);
	}


	QVBoxLayout *MainLayout = new QVBoxLayout;
	{
		MainLayout->addLayout(BoatNameLayout);
		MainLayout->addWidget(pTabWidget);
		MainLayout->addStretch(1);
		MainLayout->addLayout(CommandButtons);
	}


	setLayout(MainLayout);

}


void BoatDlg::Connect()
{
	connect(OKButton,    SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(OnCancel()));
	connect(m_pctrlEditSail,   SIGNAL(clicked()), this, SLOT(OnEditSail()));
	connect(m_pctrlDuplicateSail,   SIGNAL(clicked()), this, SLOT(OnDuplicateSail()));
	connect(m_pctrlRemoveSail, SIGNAL(clicked()), this, SLOT(OnDeleteSail()));
	connect(m_pctrlImportSail, SIGNAL(clicked()), this, SLOT(OnImportSail()));
	connect(m_pctrlExportXMLSail, SIGNAL(clicked()), this, SLOT(OnExportSail()));
	connect(m_pctrlAddHull,    SIGNAL(clicked()), this, SLOT(OnAddHull()));
	connect(m_pctrlEditHull,   SIGNAL(clicked()), this, SLOT(OnEditHull()));
	connect(m_pctrlDuplicateHull,   SIGNAL(clicked()), this, SLOT(OnDuplicateHull()));
	connect(m_pctrlRemoveHull, SIGNAL(clicked()), this, SLOT(OnDeleteHull()));
	connect(m_pctrlGetHull, SIGNAL(clicked()), this, SLOT(OnGetHull()));
	connect(m_pctrlImportHull, SIGNAL(clicked()), this, SLOT(OnImportHull()));
	connect(m_pAddNURBSSail, SIGNAL(triggered()), this, SLOT(OnAddNURBSSail()));
	connect(m_pAddSailcutSail, SIGNAL(triggered()), this, SLOT(OnAddSailcutSail()));
	connect(m_pImportNURBSSail, SIGNAL(triggered()), this, SLOT(OnImportNURBSSail()));
	connect(m_pImportSailcutSail, SIGNAL(triggered()), this, SLOT(OnImportSailcutSail()));
}



void BoatDlg::InitDialog(CBoat *pBoat)
{
	//Load the controls with the boat definition data
	if(!pBoat) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strLen;

	m_pBoat = pBoat;
	m_pctrlBoatName->setText(m_pBoat->m_BoatName);

	GetLengthUnit(strLen, pMainFrame->m_LengthUnit);

	m_pSailModel = new QStandardItemModel(this);
	m_pSailModel->setRowCount(10);//temporary
	m_pSailModel->setColumnCount(3);
	m_pSailModel->setHeaderData(0, Qt::Horizontal, tr("Sail Name"));
	m_pSailModel->setHeaderData(1, Qt::Horizontal, "x ("+strLen+")");
	m_pSailModel->setHeaderData(2, Qt::Horizontal, "z ("+strLen+")");
	m_pctrlSailTable->setModel(m_pSailModel);

	QHeaderView *HorizontalHeader = m_pctrlSailTable->horizontalHeader();
	m_pctrlSailTable->setColumnWidth(0,80);
	m_pctrlSailTable->setColumnWidth(1,50);
	m_pctrlSailTable->setColumnWidth(2,50);
	HorizontalHeader->setStretchLastSection(true);

	m_pSailDelegate = new FloatEditDelegate;
	m_pctrlSailTable->setItemDelegate(m_pSailDelegate);
	int  *precision = new int[3];
	precision[0] = -1;// Not a number, will be detected as such by FloatEditDelegate
	precision[1] = 3;
	precision[2] = 3;
	m_pSailDelegate->SetPrecision(precision);
	connect(m_pSailDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnSailCellChanged(QWidget *)));

	QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pSailModel);
	m_pctrlSailTable->setSelectionModel(selectionModel);
	connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnSailItemClicked(QModelIndex)));

	FillSailList();

	m_pHullModel = new QStandardItemModel(this);
	m_pHullModel->setRowCount(10);//temporary
	m_pHullModel->setColumnCount(4);
	m_pHullModel->setHeaderData(0, Qt::Horizontal, tr("Hull Name"));
	m_pHullModel->setHeaderData(1, Qt::Horizontal, "x ("+strLen+")");
	m_pHullModel->setHeaderData(2, Qt::Horizontal, "y ("+strLen+")");
	m_pHullModel->setHeaderData(3, Qt::Horizontal, "z ("+strLen+")");
	m_pctrlHullTable->setModel(m_pHullModel);

	HorizontalHeader = m_pctrlHullTable->horizontalHeader();
	m_pctrlHullTable->setColumnWidth(0,80);
	m_pctrlHullTable->setColumnWidth(1,40);
	m_pctrlHullTable->setColumnWidth(2,40);
	m_pctrlHullTable->setColumnWidth(3,40);
	HorizontalHeader->setStretchLastSection(true);

	m_pHullDelegate = new FloatEditDelegate;
	m_pctrlHullTable->setItemDelegate(m_pHullDelegate);
	precision = new int[4];
	precision[0] = -1;// Not a number, will be detected as such by FloatEditDelegate
	precision[1] = 3;
	precision[2] = 3;
	precision[3] = 3;
	m_pHullDelegate->SetPrecision(precision);
	connect(m_pHullDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnHullCellChanged(QWidget *)));

	selectionModel = new QItemSelectionModel(m_pHullModel);
	m_pctrlHullTable->setSelectionModel(selectionModel);
	connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnHullItemClicked(QModelIndex)));


	SetControls();
	FillHullList();
}



void BoatDlg::keyPressEvent(QKeyEvent *event)
{
	bool bShift = false;
	bool bCtrl  = false;
	if(event->modifiers() & Qt::ShiftModifier)   bShift =true;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus()) OKButton->setFocus();
			else                      accept();

			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}

//		default:
//			event->ignore();
	}
}



void BoatDlg::OnAddHull()
{
	CBody *pNewBody = new CBody;
	pNewBody->m_BodyName = QString("Hull %1").arg(m_pBoat->m_poaHull.size()+1);
	m_pBoat->m_poaHull.append(pNewBody);
	FillHullList();
	QModelIndex index = m_pHullModel->index(m_pBoat->m_poaHull.size()-1, 0, QModelIndex());
	m_pctrlHullTable->setCurrentIndex(index);
	SetControls();
	m_bChanged = true;
}


void BoatDlg::OnDeleteHull()
{
	int iSelect = m_pctrlHullTable->currentIndex().row();
	if(iSelect<0 || iSelect>=m_pBoat->m_poaHull.size()) return;
	CBody *pCurHull = (CBody*)m_pBoat->m_poaHull.at(iSelect);
	if(!pCurHull) return;

	QString strange = tr("Are you sure you want to delete the Hull")+" "+pCurHull->m_BodyName+"?";

	int resp = QMessageBox::question(this, tr("Delete Hull"), strange, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if(resp != QMessageBox::Yes) return;
	for(int is=m_pBoat->m_poaHull.size()-1;is>=0;is--)
	{
		CBody* pHull = (CBody*)m_pBoat->m_poaHull.at(is);
		if(is==iSelect)
		{
			//delete
			m_pBoat->m_poaHull.removeAt(is);
			delete pHull;
			break;
		}
	}
	FillHullList();
	if(m_pBoat->m_poaHull.size())
	{
		QModelIndex index = m_pHullModel->index(0, 0, QModelIndex());
		m_pctrlHullTable->setCurrentIndex(index);
	}
	SetControls();
	m_bChanged = true;
}


void BoatDlg::OnImportHull()
{
	CBody *pNewHull = new CBody();
	if(!pNewHull) return;


	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	double mtoUnit,xo,yo,zo;
	xo = yo = zo = 0.0;

//	FrameSize() = 0;

	UnitsDlg Dlg;

	Dlg.m_bLengthOnly = true;
	Dlg.m_Length    = pMainFrame->m_LengthUnit;
	Dlg.m_Area      = pMainFrame->m_AreaUnit;
	Dlg.m_Speed     = pMainFrame->m_SpeedUnit;
	Dlg.m_Weight    = pMainFrame->m_WeightUnit;
	Dlg.m_Force     = pMainFrame->m_ForceUnit;
	Dlg.m_Moment    = pMainFrame->m_MomentUnit;
	Dlg.m_Question = QObject::tr("Choose the length unit to read this file :");
	Dlg.InitDialog();

	if(Dlg.exec() == QDialog::Accepted)
	{
		switch(Dlg.m_Length)
		{
			case 0:{//mdm
				mtoUnit  = 1000.0;
				break;
			}
			case 1:{//cm
				mtoUnit  = 100.0;
				break;
			}
			case 2:{//dm
				mtoUnit  = 10.0;
				break;
			}
			case 3:{//m
				mtoUnit  = 1.0;
				break;
			}
			case 4:{//in
				mtoUnit  = 1000.0/25.4;
				break;
			}
			case 5:{///ft
				mtoUnit  = 1000.0/25.4/12.0;
				break;
			}
			default:{//m
				mtoUnit  = 1.0;
				break;
			}
		}
	}
	else return;

	QString PathName = QFileDialog::getOpenFileName(pMainFrame, QObject::tr("Open File"),
											pMainFrame->m_LastDirName,
											QObject::tr("All files (*.*)"));
	if(!PathName.length()) return;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = PathName.left(pos);

	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = QObject::tr("Could not read the file\n")+PathName;
		QMessageBox::warning(pMainFrame, QObject::tr("Warning"), strange);
		return;
	}

	QTextStream in(&XFile);

	if(!pNewHull->ImportDefinition(in, pMainFrame->m_mtoUnit))
	{
		delete pNewHull;
		return;
	}
	XFile.close();

	m_pBoat->m_poaHull.append(pNewHull);
	FillHullList();
	QModelIndex index = m_pHullModel->index(m_pBoat->m_poaHull.size()-1, 0, QModelIndex());
	m_pctrlHullTable->setCurrentIndex(index);
	SetControls();
}


void BoatDlg::OnGetHull()
{
	SelectObjectDlg dlg;
	QStringList NameList;
	QString BoatName, HullName;
	QSail7 *pSail7= (QSail7*)s_pSail7;

	NameList.clear();
	for(int k=0; k<pSail7->m_poaBoat->size(); k++)
	{
		CBoat *pBoat = (CBoat*)pSail7->m_poaBoat->at(k);
		for(int j=0; j<pBoat->m_poaHull.size(); j++)
		{
			CBody*pHull = (CBody*)pBoat->m_poaHull.at(j);
			NameList.append(pBoat->m_BoatName + " / " + pHull->m_BodyName);
		}
	}

	dlg.InitDialog("Select Hull to import", &NameList);
	if(dlg.exec()==QDialog::Accepted)
	{
		int pos = dlg.m_SelectedName.indexOf(" / ");
		if(pos>0)
		{
			m_bChanged = true;
			BoatName = dlg.m_SelectedName.left(pos);
			HullName = dlg.m_SelectedName.right(dlg.m_SelectedName.length()-pos-3);

			CBody *pNewHull = new CBody;
			pNewHull->m_BodyName = HullName;

			CBoat *pBoat = pSail7->GetBoat(BoatName);
			if(!pBoat) return;

			CBody *pHull = pBoat->GetBody(HullName);
			if(!pHull) return;

			pNewHull->Duplicate(pHull);

			m_pBoat->m_poaHull.append(pNewHull);
			FillHullList();
			QModelIndex index = m_pHullModel->index(m_pBoat->m_poaHull.size()-1, 0, QModelIndex());
			m_pctrlHullTable->setCurrentIndex(index);
			SetControls();
		}
	}
}



void BoatDlg::OnDuplicateHull()
{
	int iSelect = m_pctrlHullTable->currentIndex().row();
	if(iSelect<0 || iSelect>=m_pBoat->m_poaHull.size()) return;
	CBody *pCurHull = (CBody*)m_pBoat->m_poaHull.at(iSelect);
	if(!pCurHull) return;

	CBody *pNewBody = new CBody;
	pNewBody->Duplicate(pCurHull);
	m_pBoat->m_poaHull.append(pNewBody);
	FillHullList();
	QModelIndex index = m_pHullModel->index(m_pBoat->m_poaHull.size()-1, 0, QModelIndex());
	m_pctrlHullTable->setCurrentIndex(index);
	SetControls();
	m_bChanged = true;
}


void BoatDlg::OnEditHull()
{
	//Get a pointer to the currently selected Hull
	int iSelect = m_pctrlHullTable->currentIndex().row();
	if(iSelect<0 || iSelect>=m_pBoat->m_poaHull.size()) return;
	CBody *pCurHull = (CBody*)m_pBoat->m_poaHull.at(iSelect);

	CBody memBody;
	memBody.Duplicate(pCurHull);

	GL3dBodyDlg dlg(this);
	dlg.m_bEnableName = false;
	if(!dlg.InitDialog(pCurHull)) return;

	dlg.move(GL3dBodyDlg::s_WindowPos);
	dlg.resize(GL3dBodyDlg::s_WindowSize);
	if(GL3dBodyDlg::s_bWindowMaximized) dlg.setWindowState(Qt::WindowMaximized);

	if(dlg.exec() == QDialog::Accepted)
	{
		// we returned from the dialog with an 'OK',
		FillHullList();
		if(dlg.m_bChanged) m_bChanged = true;
	}
	else
	{
		pCurHull->Duplicate(&memBody);
	}
}



void BoatDlg::OnAddNURBSSail()
{
	NURBSSail *pNewSail = new NURBSSail;
	pNewSail->m_SailName = QString("NURBS Sail %1").arg(m_pBoat->m_poaSail.size()+1);
	m_pBoat->m_poaSail.append(pNewSail);
	m_bChanged = true;

	FillSailList();
	QModelIndex index = m_pSailModel->index(m_pBoat->m_poaSail.size()-1, 0, QModelIndex());
	m_pctrlSailTable->setCurrentIndex(index);

	SetControls();
}


void BoatDlg::OnAddSailcutSail()
{
	SailcutSail *pNewSail = new SailcutSail;
	pNewSail->m_SailName = QString("Sailcut Sail %1").arg(m_pBoat->m_poaSail.size()+1);
	m_pBoat->m_poaSail.append(pNewSail);
	m_bChanged = true;

	FillSailList();
	QModelIndex index = m_pSailModel->index(m_pBoat->m_poaSail.size()-1, 0, QModelIndex());
	m_pctrlSailTable->setCurrentIndex(index);

	SetControls();
}



void BoatDlg::OnImportNURBSSail()
{
}


void BoatDlg::OnImportSailcutSail()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),pMainFrame->m_XMLPath,
													tr("Sailcut files (*.saildef *.xml)"));
	if (!filePath.isEmpty())
	{
		SailcutSail *pSCSail = new SailcutSail;
		pMainFrame->m_XMLPath = filePath;
		QFile file(filePath);
		if(pSCSail->Import(&file))
			m_pBoat->m_poaSail.append(pSCSail);
	}

	FillSailList();
	SetControls();
}


void BoatDlg::OnDeleteSail()
{
	int iSelect = m_pctrlSailTable->currentIndex().row();
	if(iSelect<0 || iSelect>=m_pBoat->m_poaSail.size()) return;
	QSail *pCurSail = (QSail*)m_pBoat->m_poaSail.at(iSelect);
	if(!pCurSail) return;

	QString strange = tr("Are you sure you want to delete the sail")+" "+pCurSail->m_SailName+"?";

	int resp = QMessageBox::question(this, tr("Delete Sail"), strange, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if(resp != QMessageBox::Yes) return;
	for(int is=m_pBoat->m_poaSail.size()-1;is>=0;is--)
	{
		QSail* pSail = (QSail*)m_pBoat->m_poaSail.at(is);
		if(is==iSelect)
		{
			//delete
			m_pBoat->m_poaSail.removeAt(is);
			delete pSail;
			SetControls();
			break;
		}
	}
	FillSailList();

	m_bChanged = true;
}


void BoatDlg::OnDuplicateSail()
{
	int iSelect = m_pctrlSailTable->currentIndex().row();
	if(iSelect<0 || iSelect>=m_pBoat->m_poaSail.size()) return;
	QSail *pCurSail = m_pBoat->m_poaSail.at(iSelect);
	if(!pCurSail) return;

	QSail *pNewSail = NULL;
	if(pCurSail->IsNURBSSail())        pNewSail = new NURBSSail;
	else if(pCurSail->IsSailcutSail()) pNewSail = new SailcutSail;
	if(pNewSail)
	{
		pNewSail->m_SailName = QString("Sail %1").arg(m_pBoat->m_poaSail.size()+1);
		pNewSail->Duplicate(pCurSail);
		m_pBoat->m_poaSail.append(pNewSail);
		QModelIndex index = m_pSailModel->index(m_pBoat->m_poaSail.size()-1, 0, QModelIndex());
		m_pctrlSailTable->setCurrentIndex(index);
	}
	FillSailList();
	SetControls();

	m_bChanged = true;
}


void BoatDlg::OnEditSail()
{
	int iSelect = m_pctrlSailTable->currentIndex().row();
	if(iSelect<0 || iSelect>=m_pBoat->m_poaSail.size()) return;

	QSail *pMemSail = NULL;
	QSail *pSail = m_pBoat->m_poaSail.at(iSelect);
	if(pSail->IsNURBSSail())        pMemSail = new NURBSSail;
	else if(pSail->IsSailcutSail()) pMemSail = new SailcutSail;

	if(!pMemSail) return;//unrecognized sail type

	pMemSail->Duplicate(pSail);

	SailDlg dlg;
	if(dlg.InitDialog(pMemSail))
	{
		if(dlg.exec())
		{
			pSail->Duplicate(pMemSail);
			FillSailList();
		}
	}
	SailDlg::s_WindowPos = dlg.pos();
	SailDlg::s_WindowSize = dlg.size();
}



void BoatDlg::OnExportSail()
{
	int iSelect = m_pctrlSailTable->currentIndex().row();
	if(iSelect<0 || iSelect>=m_pBoat->m_poaSail.size()) return;
	QSail *pCurSail = (QSail*)m_pBoat->m_poaSail.at(iSelect);
	if(!pCurSail) return;
	pCurSail->Export();
}


void BoatDlg::OnImportSail()
{
	SelectObjectDlg dlg;
	QStringList NameList;
	QString BoatName, SailName;
	QSail7 *pSail7 = (QSail7*)s_pSail7;

	NameList.clear();
	for(int k=0; k<pSail7->m_poaBoat->size(); k++)
	{
		CBoat *pBoat = (CBoat*)pSail7->m_poaBoat->at(k);
		for(int j=0; j<pBoat->m_poaSail.size(); j++)
		{
			QSail *pSail = (QSail*)pBoat->m_poaSail.at(j);
			NameList.append(pBoat->m_BoatName + " / " + pSail->m_SailName);
		}
	}

	dlg.InitDialog("Select sail to import", &NameList);
	if(dlg.exec()==QDialog::Accepted)
	{
		int pos = dlg.m_SelectedName.indexOf(" / ");
		if(pos>0)
		{
			m_bChanged = true;
			BoatName = dlg.m_SelectedName.left(pos);
			SailName = dlg.m_SelectedName.right(dlg.m_SelectedName.length()-pos-3);

			CBoat *pBoat = pSail7->GetBoat(BoatName);
			if(!pBoat) return;

			QSail *pSail = pBoat->GetSail(SailName);
			if(!pSail) return;

			QSail *pNewSail = NULL;
			if(pSail->IsNURBSSail())        pNewSail = new NURBSSail;
			else if(pSail->IsSailcutSail()) pNewSail = new SailcutSail;
			if(pNewSail)
			{
				pNewSail->Duplicate(pSail);
				pNewSail->m_SailName = SailName;

				m_pBoat->m_poaSail.append(pNewSail);
				QModelIndex index = m_pSailModel->index(m_pBoat->m_poaSail.size()-1, 0, QModelIndex());
				m_pctrlSailTable->setCurrentIndex(index);
				FillSailList();
				SetControls();
			}
		}
	}
}


void BoatDlg::OnOK()
{
	m_pBoat->m_BoatName = m_pctrlBoatName->text();
	ReadData();
	accept();
}




void BoatDlg::OnCancel()
{
	if(m_bChanged)
	{
		QString strange = tr("Discard the changes ?");
		int resp = QMessageBox::question(this, tr("Cancel"), strange, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if(resp != QMessageBox::Yes) return;
	}
	reject();
}


void BoatDlg::OnSailItemClicked(const QModelIndex &index)
{
//	m_iSailSelection = index.row();
	SetControls();
}


void BoatDlg::OnSailCellChanged(QWidget *pWidget)
{
	ReadData();
	m_bChanged = true;
}


void BoatDlg::OnHullItemClicked(const QModelIndex &index)
{
//	m_iHullSelection = index.row();
	SetControls();
}


void BoatDlg::OnHullCellChanged(QWidget *pWidget)
{
	ReadData();
	m_bChanged = true;
}



void BoatDlg::ReadData()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	for(int is=0; is<m_pSailModel->rowCount(); is++)
	{
		QSail *pSail = (QSail*)m_pBoat->m_poaSail.at(is);
		if(pSail)
		{
			QModelIndex index = m_pSailModel->index(is, 0, QModelIndex());
			pSail->m_SailName = index.data().toString();

			index = m_pSailModel->index(is,1, QModelIndex());
			pSail->m_LEPosition.x = index.data().toDouble()/pMainFrame->m_mtoUnit;

			index = m_pSailModel->index(is,2, QModelIndex());
			pSail->m_LEPosition.z = index.data().toDouble()/pMainFrame->m_mtoUnit;
		}
	}
	for(int ib=0; ib<m_pHullModel->rowCount(); ib++)
	{
		CBody *pHull = (CBody*)m_pBoat->m_poaHull.at(ib);
		if(pHull)
		{
			QModelIndex index = m_pHullModel->index(ib, 0, QModelIndex());
			pHull->m_BodyName= index.data().toString();

			index = m_pHullModel->index(ib,1, QModelIndex());
			pHull->m_LEPosition.x = index.data().toDouble()/pMainFrame->m_mtoUnit;

			index = m_pHullModel->index(ib,2, QModelIndex());
			pHull->m_LEPosition.y = index.data().toDouble()/pMainFrame->m_mtoUnit;

			index = m_pHullModel->index(ib,3, QModelIndex());
			pHull->m_LEPosition.z = index.data().toDouble()/pMainFrame->m_mtoUnit;
		}
	}
}


void BoatDlg::FillHullList()
{
	//fill the Hull list
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_pHullModel->setRowCount(m_pBoat->m_poaHull.size());
	for(int is=0; is<m_pBoat->m_poaHull.size(); is++)
	{
		CBody *pHull = (CBody*)m_pBoat->m_poaHull.at(is);
		if(pHull)
		{
			ind = m_pHullModel->index(is, 0, QModelIndex());
			m_pHullModel->setData(ind, pHull->m_BodyName);

			ind = m_pHullModel->index(is, 1, QModelIndex());
			m_pHullModel->setData(ind, pHull->m_LEPosition.x * pMainFrame->m_mtoUnit);

			ind = m_pHullModel->index(is, 2, QModelIndex());
			m_pHullModel->setData(ind, pHull->m_LEPosition.y * pMainFrame->m_mtoUnit);

			ind = m_pHullModel->index(is, 3, QModelIndex());
			m_pHullModel->setData(ind, pHull->m_LEPosition.z * pMainFrame->m_mtoUnit);
		}
	}
}


void BoatDlg::FillSailList()
{
	//fill the sail list
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_pSailModel->setRowCount(m_pBoat->m_poaSail.size());
	for(int is=0; is<m_pBoat->m_poaSail.size(); is++)
	{
		QSail *pSail = (QSail*)m_pBoat->m_poaSail.at(is);
		if(pSail)
		{
			ind = m_pSailModel->index(is, 0, QModelIndex());
			m_pSailModel->setData(ind, pSail->m_SailName);

			ind = m_pSailModel->index(is, 1, QModelIndex());
			m_pSailModel->setData(ind, pSail->m_LEPosition.x * pMainFrame->m_mtoUnit);

			ind = m_pSailModel->index(is, 2, QModelIndex());
			m_pSailModel->setData(ind, pSail->m_LEPosition.z * pMainFrame->m_mtoUnit);
		}
	}
}


void BoatDlg::SetControls()
{
	int iSelect = m_pctrlSailTable->currentIndex().row();
	m_pctrlRemoveSail->setEnabled(iSelect>=0);
	m_pctrlEditSail->setEnabled(iSelect>=0);
	m_pctrlDuplicateSail->setEnabled(iSelect>=0);
	m_pctrlExportXMLSail->setEnabled(iSelect>=0);

	iSelect = m_pctrlHullTable->currentIndex().row();
	m_pctrlRemoveHull->setEnabled(iSelect>=0);
	m_pctrlEditHull->setEnabled(iSelect>=0);
	m_pctrlDuplicateHull->setEnabled(iSelect>=0);
}



