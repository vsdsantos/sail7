/****************************************************************************

		 BoatPolarDlg Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#include "../mainframe.h"
#include "../globals.h"
#include "../objects/BoatPolar.h"
#include "BoatPolarDlg.h"
#include "sail7.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QtDebug>
#include <math.h>


void* BoatPolarDlg::s_pMainFrame;
void* BoatPolarDlg::s_pSail7;
CBoatPolar BoatPolarDlg::s_BoatPolar;

int BoatPolarDlg::s_UnitType= 1;//1= International, 2= English



BoatPolarDlg::BoatPolarDlg()
{
	setWindowTitle(tr("Analysis Definition"));

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strLength;
	GetLengthUnit(strLength, pMainFrame->m_LengthUnit);

	m_pBoat     = NULL;

	m_BoatPolarName = "SailPolar Name";

	m_NXWakePanels    = 1;
	m_TotalWakeLength = 100.0;//x mac
	m_WakePanelFactor = 1.1;


	s_BoatPolar.m_WindGradient[0][0] =    0.0;
	s_BoatPolar.m_WindGradient[0][1] =    1.0;
	s_BoatPolar.m_WindGradient[1][0] =   25.0;
	s_BoatPolar.m_WindGradient[1][1] =    1.0;

//	m_SymbolFont.CreatePointFont(100, "Symbol");

	SetupLayout();

	//Create the model associated to the table of variables
	m_pVariableModel = new QStandardItemModel(this);
	m_pVariableModel->setRowCount(5);//temporary
	m_pVariableModel->setColumnCount(4);
	m_pVariableModel->setHeaderData(0, Qt::Horizontal, tr("Design Variable"));
	m_pVariableModel->setHeaderData(1, Qt::Horizontal, "min");
	m_pVariableModel->setHeaderData(2, Qt::Horizontal, "max");
	m_pVariableModel->setHeaderData(3, Qt::Horizontal, "Unit");
	m_pctrlVariableTable->setModel(m_pVariableModel);

	QHeaderView *HorizontalHeader = m_pctrlVariableTable->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);

	m_pVariableDelegate = new FloatEditDelegate;
	m_pctrlVariableTable->setItemDelegate(m_pVariableDelegate);
	int  *precision = new int[4];
	precision[0] = -1;// Not a number, will be detected as such by FloatEditDelegate
	precision[1] = 1;
	precision[2] = 1;
	precision[3] = -1;
	m_pVariableDelegate->SetPrecision(precision);

	QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pVariableModel);
	m_pctrlVariableTable->setSelectionModel(selectionModel);


	//Create the model associtated to the wind gradient
	m_pWindGradientModel = new QStandardItemModel(this);
	m_pWindGradientModel->setRowCount(2);
	m_pWindGradientModel->setColumnCount(2);
	QString strHeight =  "Height ("+strLength+")";
	m_pWindGradientModel->setHeaderData(0, Qt::Horizontal, strHeight);
	m_pWindGradientModel->setHeaderData(1, Qt::Horizontal, "Wind speed factor");
	m_pctrlWindGradientTable->setModel(m_pWindGradientModel);

	QHeaderView *HorizontalGradientHeader = m_pctrlWindGradientTable->horizontalHeader();

	HorizontalGradientHeader->setStretchLastSection(true);

	m_pWindGradientDelegate = new FloatEditDelegate;
	m_pctrlWindGradientTable->setItemDelegate(m_pWindGradientDelegate);
	int  *precisionGradient = new int[2];
	precisionGradient[0] = 2;// Not a number, will be detected as such by FloatEditDelegate
	precisionGradient[1] = 2;
	m_pWindGradientDelegate->SetPrecision(precisionGradient);

	QItemSelectionModel *selectionGradientModel = new QItemSelectionModel(m_pWindGradientModel);
	m_pctrlWindGradientTable->setSelectionModel(selectionGradientModel);


	Connect();
}



void BoatPolarDlg::Connect()
{
	connect(m_pctrlUnit1, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void BoatPolarDlg::FillVariableList()
{
	//fill the sail list
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	GetSpeedUnit(strong, pMainFrame->m_SpeedUnit);
	m_pVariableModel->setRowCount(3+m_pBoat->m_poaSail.size());


	// first line is speed range	ind = m_pVariableModel->index(0, 0, QModelIndex());
	ind = m_pVariableModel->index(0, 0, QModelIndex());
	m_pVariableModel->setData(ind, "Wind Speed");
	ind = m_pVariableModel->index(0, 1, QModelIndex());
	m_pVariableModel->setData(ind, s_BoatPolar.m_QInfMin*pMainFrame->m_mstoUnit);
	ind = m_pVariableModel->index(0, 2, QModelIndex());
	m_pVariableModel->setData(ind, s_BoatPolar.m_QInfMax*pMainFrame->m_mstoUnit);
	ind = m_pVariableModel->index(0, 3, QModelIndex());
	m_pVariableModel->setData(ind, strong);


	// second line is wind angle range	ind = m_pVariableModel->index(1, 0, QModelIndex());
	ind = m_pVariableModel->index(1, 0, QModelIndex());
	m_pVariableModel->setData(ind, "Wind Direction");
	ind = m_pVariableModel->index(1, 1, QModelIndex());
	m_pVariableModel->setData(ind, s_BoatPolar.m_BetaMin);
	ind = m_pVariableModel->index(1, 2, QModelIndex());
	m_pVariableModel->setData(ind, s_BoatPolar.m_BetaMax);
	ind = m_pVariableModel->index(1, 3, QModelIndex());
	m_pVariableModel->setData(ind, QString::fromUtf8("°"));

	// third line is bank angle range
	ind = m_pVariableModel->index(2, 0, QModelIndex());
	m_pVariableModel->setData(ind, "Bank Angle");
	ind = m_pVariableModel->index(2, 1, QModelIndex());
	m_pVariableModel->setData(ind, -s_BoatPolar.m_PhiMin);
	ind = m_pVariableModel->index(2, 2, QModelIndex());
	m_pVariableModel->setData(ind, -s_BoatPolar.m_PhiMax);
	ind = m_pVariableModel->index(2, 3, QModelIndex());
	m_pVariableModel->setData(ind, QString::fromUtf8("°"));


	// the next set of line are the sail angles
	for(int is=0; is<m_pBoat->m_poaSail.size(); is++)
	{
		CSail *pSail = (CSail*)m_pBoat->m_poaSail.at(is);
		ind = m_pVariableModel->index(is+3, 0, QModelIndex());
		m_pVariableModel->setData(ind, pSail->m_SailName+" Angle");
		ind = m_pVariableModel->index(is+3, 1, QModelIndex());
		m_pVariableModel->setData(ind, -s_BoatPolar.m_SailAngleMin[is]);
		ind = m_pVariableModel->index(is+3, 2, QModelIndex());
		m_pVariableModel->setData(ind, -s_BoatPolar.m_SailAngleMax[is]);
		ind = m_pVariableModel->index(is+3, 3, QModelIndex());
		m_pVariableModel->setData(ind, QString::fromUtf8("°"));
	}
}



void BoatPolarDlg::ReadData()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_BoatPolarName = m_pctrlBoatPolarName->text();

	s_BoatPolar.m_CoG.x     = m_pctrlXCmRef->Value() / pMainFrame->m_mtoUnit;
	s_BoatPolar.m_CoG.z     = m_pctrlZCmRef->Value() / pMainFrame->m_mtoUnit;

	if(m_pctrlUnit1->isChecked())
	{
		s_BoatPolar.m_Viscosity = m_pctrlViscosity->Value();
		s_BoatPolar.m_Density   = m_pctrlDensity->Value();
	}
	else
	{
		s_BoatPolar.m_Density   = m_pctrlDensity->Value() / 0.00194122;
		s_BoatPolar.m_Viscosity = m_pctrlViscosity->Value() / 10.7182881;
	}

	s_BoatPolar.m_bGround = m_pctrlGroundEffect->isChecked();


	SetDensity();

	QModelIndex index;

	//Read variables
	// first line is speed range
	index = m_pVariableModel->index(0,1, QModelIndex());
	s_BoatPolar.m_QInfMin = index.data().toDouble()/pMainFrame->m_mstoUnit;
	index = m_pVariableModel->index(0,2, QModelIndex());
	s_BoatPolar.m_QInfMax = index.data().toDouble()/pMainFrame->m_mstoUnit;

	// second line is wind angle range
	index = m_pVariableModel->index(1,1, QModelIndex());
	s_BoatPolar.m_BetaMin = index.data().toDouble();
	index = m_pVariableModel->index(1,2, QModelIndex());
	s_BoatPolar.m_BetaMax = index.data().toDouble();

	// third line is bank angle range
	index = m_pVariableModel->index(2,1, QModelIndex());
	s_BoatPolar.m_PhiMin = index.data().toDouble();
	index = m_pVariableModel->index(2,2, QModelIndex());
	s_BoatPolar.m_PhiMax = index.data().toDouble();

	// the next set of line are the sail angles
	for(int is=0; is<m_pBoat->m_poaSail.size(); is++)
	{
		index = m_pVariableModel->index(is+3,1, QModelIndex());
		s_BoatPolar.m_SailAngleMin[is] = index.data().toDouble();
		index = m_pVariableModel->index(is+3,2, QModelIndex());
		s_BoatPolar.m_SailAngleMax[is] = index.data().toDouble();
	}

	//Read Wind Gradient
	index = m_pWindGradientModel->index(0,0, QModelIndex());
	s_BoatPolar.m_WindGradient[0][0] = index.data().toDouble()/pMainFrame->m_mtoUnit;
	index = m_pWindGradientModel->index(0,1, QModelIndex());
	s_BoatPolar.m_WindGradient[0][1] = index.data().toDouble();
	index = m_pWindGradientModel->index(1,0, QModelIndex());
	s_BoatPolar.m_WindGradient[1][0] = index.data().toDouble()/pMainFrame->m_mtoUnit;
	index = m_pWindGradientModel->index(1,1, QModelIndex());
	s_BoatPolar.m_WindGradient[1][1] = index.data().toDouble();


}




void BoatPolarDlg::InitDialog(CBoat *pBoat, CBoatPolar *pBoatPolar)
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QString str;

	if(!pBoat)return;
	m_pBoat=pBoat;


	if(s_UnitType==1) m_pctrlUnit1->setChecked(true);
	else              m_pctrlUnit2->setChecked(true);
	OnUnit();

	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit1->setText(str);
	m_pctrlLengthUnit3->setText(str);


	m_pctrlBoatName->setText(m_pBoat->m_BoatName);
	if(pBoatPolar)
	{
		m_pctrlBoatPolarName->setEnabled(false);
		m_pctrlBoatPolarName->setText(pBoatPolar->m_BoatPolarName);
	}
	else
	{
		int size=0;
		for(int i=0; i<pMainFrame->m_oaBoatPolar.size();i++)
		{
			CBoatPolar* pBoatPolar = (CBoatPolar*)pMainFrame->m_oaBoatPolar.at(i);
			if(pBoatPolar->m_BoatName==pBoat->m_BoatName) size++;
		}
		m_pctrlBoatPolarName->setText(QString("Polar %1").arg(size+1));
	}

	if(pBoatPolar) s_BoatPolar.DuplicateSpec(pBoatPolar);

	m_pctrlGroundEffect->setChecked(s_BoatPolar.m_bGround);
	m_pctrlXCmRef->SetValue(s_BoatPolar.m_CoG.x*pMainFrame->m_mtoUnit);
	m_pctrlZCmRef->SetValue(s_BoatPolar.m_CoG.z*pMainFrame->m_mtoUnit);


	//fill the wind gradient table
	QModelIndex ind;
	QString strong;
	GetSpeedUnit(strong, pMainFrame->m_SpeedUnit);
	ind = m_pWindGradientModel->index(0, 0, QModelIndex());
	m_pWindGradientModel->setData(ind, s_BoatPolar.m_WindGradient[0][0]*pMainFrame->m_mtoUnit);
	ind = m_pWindGradientModel->index(0, 1, QModelIndex());
	m_pWindGradientModel->setData(ind, s_BoatPolar.m_WindGradient[0][1]);
	ind = m_pWindGradientModel->index(1, 0, QModelIndex());
	m_pWindGradientModel->setData(ind, s_BoatPolar.m_WindGradient[1][0]*pMainFrame->m_mtoUnit);
	ind = m_pWindGradientModel->index(1, 1, QModelIndex());
	m_pWindGradientModel->setData(ind, s_BoatPolar.m_WindGradient[1][1]);

	FillVariableList();
	m_pctrlVariableTable->setFocus();


	if(m_pBoat->m_poaHull.size())
	{
		m_pctrlGroundEffect->setChecked(false);
		m_pctrlGroundEffect->setEnabled(false);
	}
}



void BoatPolarDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				ReadData();
				OKButton->setFocus();
				return;
			}
			else
			{
				OnOK();
				return;
			}
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
		}
		default:
			event->ignore();
	}
}


void BoatPolarDlg::OnOK()
{
	ReadData();
	m_BoatPolarName = m_pctrlBoatPolarName->text();

	int LineLength = m_BoatPolarName.length();
	if(!LineLength)
	{
		QMessageBox::warning(this, tr("Warning"),tr("Must enter a name for the polar"));
		m_pctrlBoatPolarName->setFocus();
		return;
	}

	s_BoatPolar.m_PhiMin = - s_BoatPolar.m_PhiMin;
	s_BoatPolar.m_PhiMax = - s_BoatPolar.m_PhiMax;
	for(int is=0; is<m_pBoat->m_poaSail.size(); is++)
	{
		s_BoatPolar.m_SailAngleMin[is] = -s_BoatPolar.m_SailAngleMin[is];
		s_BoatPolar.m_SailAngleMax[is] = -s_BoatPolar.m_SailAngleMax[is];
	}

	accept();
}


void BoatPolarDlg::OnUnit()
{
	if(m_pctrlUnit1->isChecked())
	{
		s_UnitType   = 1;
		m_pctrlViscosity->SetValue(s_BoatPolar.m_Viscosity);
		m_pctrlDensityUnit->setText("kg/m3");
		m_pctrlViscosityUnit->setText("m"+QString::fromUtf8("²")+"/s");
	}
	else
	{
		s_UnitType   = 2;
		m_pctrlViscosity->SetValue(s_BoatPolar.m_Viscosity* 10.7182881);
		m_pctrlDensityUnit->setText("slugs/ft3");
		m_pctrlViscosityUnit->setText("ft"+QString::fromUtf8("²")+"/s");
	}
	SetDensity();
}



void BoatPolarDlg::SetDensity()
{
	int exp, precision;
	if(m_pctrlUnit1->isChecked())
	{
		exp = (int)log(s_BoatPolar.m_Density);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(s_BoatPolar.m_Density);
	}
	else
	{
		exp = (int)log(s_BoatPolar.m_Density* 0.00194122);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(s_BoatPolar.m_Density* 0.00194122);
	}
}


void BoatPolarDlg::SetupLayout()
{
	QFont SymbolFont("Symbol");

	QGroupBox *NameGroup = new QGroupBox(tr("Polar Name"));
	{
		QVBoxLayout *NameLayout = new QVBoxLayout;
		m_pctrlBoatName = new QLabel(tr("Wing Name"));
		m_pctrlBoatPolarName = new QLineEdit(tr("Polar Name"));
		NameLayout->addWidget(m_pctrlBoatName);
		NameLayout->addWidget(m_pctrlBoatPolarName);
		NameGroup->setLayout(NameLayout);
	}


	m_pctrlVariableTable = new QTableView(this);
	m_pctrlVariableTable->installEventFilter(this);
//	m_pctrlVariableTable->setMinimumHeight(250);
//	m_pctrlVariableTable->setMinimumWidth(300);
	m_pctrlVariableTable->setWindowTitle(QObject::tr("Sail definition"));
	m_pctrlVariableTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlVariableTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlVariableTable->setEditTriggers(QAbstractItemView::CurrentChanged |
								QAbstractItemView::DoubleClicked |
								QAbstractItemView::SelectedClicked |
								QAbstractItemView::EditKeyPressed |
								QAbstractItemView::AnyKeyPressed);



//_________________________Right Layout
	m_pctrlGroundEffect = new QCheckBox(tr("Ground Effect"));

	QGroupBox *InertiaBox = new QGroupBox(tr("Inertia properties"));
	{
		QGridLayout *InertiaDataLayout = new QGridLayout;
		QLabel *lab2 = new QLabel(tr("Plane Mass ="));
		QLabel *lab3 = new QLabel(tr("X_CoG ="));
		QLabel *lab4 = new QLabel(tr("Z_CoG ="));
		lab2->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		lab3->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		lab4->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		InertiaDataLayout->addWidget(lab3,1,1);
		InertiaDataLayout->addWidget(lab4,2,1);
		m_pctrlXCmRef  = new FloatEdit(100.00,3);
		m_pctrlZCmRef  = new FloatEdit(100.00,3);
		InertiaDataLayout->addWidget(m_pctrlXCmRef,1,2);
		InertiaDataLayout->addWidget(m_pctrlZCmRef,2,2);
		m_pctrlLengthUnit1 = new QLabel("mm");
		m_pctrlLengthUnit3 = new QLabel("mm");
		InertiaDataLayout->addWidget(m_pctrlLengthUnit1 ,1,3);
		InertiaDataLayout->addWidget(m_pctrlLengthUnit3 ,2,3);
		QVBoxLayout *InertiaLayout = new QVBoxLayout;
		InertiaLayout->addLayout(InertiaDataLayout);
		InertiaBox->setLayout(InertiaLayout);
	}

	QGroupBox *AeroDataGroupBox = new QGroupBox(tr("Aerodynamic Data"));
	{
		QGridLayout *AeroDataLayout = new QGridLayout;
		QLabel *lab9 = new QLabel(tr("Unit"));
		m_pctrlUnit1 = new QRadioButton(tr("International"));
		m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
		m_pctrlRho = new QLabel("r =");
		m_pctrlDensity = new FloatEdit(1.225,3);
		m_pctrlDensityUnit = new QLabel("kg/m3");
		m_pctrlNu = new QLabel("n =");
		m_pctrlRho->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		m_pctrlNu->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		m_pctrlViscosity = new FloatEdit(1.500e-5,3);
		m_pctrlViscosityUnit = new QLabel("m2/s");
		m_pctrlRho->setFont(SymbolFont);
		m_pctrlNu->setFont(SymbolFont);
		m_pctrlDensity->SetPrecision(5);
		m_pctrlViscosity->SetPrecision(3);
		m_pctrlDensity->SetMin(0.0);
		m_pctrlViscosity->SetMin(0.0);
		AeroDataLayout->addWidget(lab9,1,1);
		AeroDataLayout->addWidget(m_pctrlUnit1,1,2);
		AeroDataLayout->addWidget(m_pctrlUnit2,1,3);
		AeroDataLayout->addWidget(m_pctrlRho,2,1);
		AeroDataLayout->addWidget(m_pctrlDensity,2,2);
		AeroDataLayout->addWidget(m_pctrlDensityUnit,2,3);
		AeroDataLayout->addWidget(m_pctrlNu,3,1);
		AeroDataLayout->addWidget(m_pctrlViscosity,3,2);
		AeroDataLayout->addWidget(m_pctrlViscosityUnit,3,3);
		AeroDataGroupBox->setLayout(AeroDataLayout);
	}

	QGroupBox *WindGradientBox = new QGroupBox("Wind gradient parabola");
	{
		QVBoxLayout *WindGradientLayout = new QVBoxLayout;
		m_pctrlWindGradientTable = new QTableView(this);
		m_pctrlWindGradientTable->installEventFilter(this);
//		m_pctrlWindGradientTable->setMinimumHeight(150);
//		m_pctrlWindGradientTable->setMinimumWidth(200);
		m_pctrlWindGradientTable->setWindowTitle(QObject::tr("Wind Gradient"));
		m_pctrlWindGradientTable->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pctrlWindGradientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pctrlWindGradientTable->setEditTriggers(QAbstractItemView::CurrentChanged |
									QAbstractItemView::DoubleClicked |
									QAbstractItemView::SelectedClicked |
									QAbstractItemView::EditKeyPressed |
									QAbstractItemView::AnyKeyPressed);
		WindGradientLayout->addWidget(m_pctrlWindGradientTable);
		WindGradientBox->setLayout(WindGradientLayout);
	}

	QHBoxLayout *DataLayout = new QHBoxLayout;
	{
		QVBoxLayout *LeftSideLayout = new QVBoxLayout;
		{
			LeftSideLayout->addWidget(NameGroup);
			LeftSideLayout->addWidget(InertiaBox);
			LeftSideLayout->addWidget(AeroDataGroupBox);
			LeftSideLayout->addStretch(1);
			LeftSideLayout->addWidget(WindGradientBox);
			LeftSideLayout->addWidget(m_pctrlGroundEffect);
		}
		QVBoxLayout *RightSideLayout = new QVBoxLayout;
		{
			RightSideLayout->addWidget(m_pctrlVariableTable);
		}
		DataLayout->addLayout(LeftSideLayout);
		DataLayout->addLayout(RightSideLayout);
	}


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("OK"));
		OKButton->setDefault(true);
		CancelButton = new QPushButton(tr("Cancel"));
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(CancelButton);
		CommandButtons->addStretch(1);
	}

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	{
		MainLayout->addLayout(DataLayout);
		MainLayout->addStretch(1);
		MainLayout->addLayout(CommandButtons);
	}
	setLayout(MainLayout);
}




bool BoatPolarDlg::eventFilter(QObject* o, QEvent* e)
{
	if(o==m_pctrlVariableTable && e->type() == QEvent::Resize)
	{
		// get size from event, resize columns here
		double wc	= (double)m_pctrlVariableTable->width() *0.85;
		m_pctrlVariableTable->setColumnWidth(0, (int)(wc/2.0));
		m_pctrlVariableTable->setColumnWidth(1,(int)(wc/5.0));
		m_pctrlVariableTable->setColumnWidth(2,(int)(wc/5.0));
	}
	else if(o==	m_pctrlWindGradientTable)
	{
		double wc	= (double)m_pctrlWindGradientTable->width() *0.9;
		m_pctrlWindGradientTable->setColumnWidth(0,(int)(wc/2));
	}
	else return QDialog::eventFilter( o, e );
}



