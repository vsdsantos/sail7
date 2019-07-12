/****************************************************************************

		 ScaleSailDlg Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/





#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QLabel>

#include "../mainframe.h"
#include "../globals.h"
#include "../misc/LinePickerDlg.h"
#include "./ScaleSailDlg.h"
#include "./SailDlg.h"
#include "./GL3dBodyDlg.h"


void *ScaleSailDlg::s_pMainFrame = nullptr;

ScaleSailDlg::ScaleSailDlg(void *pParent)
{
	setWindowTitle(("Sail Scale Dialog"));
	m_pSailDlg = pParent;
	m_XFactor = 1.0;
	m_YFactor = 1.0;
	m_ZFactor = 1.0;
	m_pSail = nullptr;
	SetupLayout();
}



void ScaleSailDlg::InitDialog(Sail *pSail)
{
	m_pSail = pSail;
	m_pctrlXScaleFactor->SetValue(m_XFactor);
	m_pctrlYScaleFactor->SetValue(m_YFactor);
	m_pctrlZScaleFactor->SetValue(m_ZFactor);

	m_pctrlXScaleFactor->setFocus();

	EnableControls();
}



void ScaleSailDlg::SetupLayout()
{
	setWindowTitle("Scale Body");
//	QDesktopWidget desktop;
//	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/3);
//	move(r.width()/3, r.height()/6);


	QGridLayout *ScaleLayout = new QGridLayout;
	m_pctrlXScaleFactor = new FloatEdit(1.0);
	m_pctrlYScaleFactor = new FloatEdit(2.000);
	m_pctrlZScaleFactor = new FloatEdit(3.);
	m_pctrlXScaleFactor->SetPrecision(3);
	m_pctrlYScaleFactor->SetPrecision(3);
	m_pctrlZScaleFactor->SetPrecision(3);
	QLabel *lab0 = new QLabel(("Scale Factor"));
	QLabel *lab1 = new QLabel(("X Scale"));
	QLabel *lab2 = new QLabel(("Y Scale"));
	QLabel *lab3 = new QLabel(("Z Scale"));
	ScaleLayout->addWidget(lab0,1,2);
	ScaleLayout->addWidget(lab1,2,1);
	ScaleLayout->addWidget(lab2,3,1);
	ScaleLayout->addWidget(lab3,4,1);
	ScaleLayout->addWidget(m_pctrlXScaleFactor,2,2);
	ScaleLayout->addWidget(m_pctrlYScaleFactor,3,2);
	ScaleLayout->addWidget(m_pctrlZScaleFactor,4,2);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton      = new QPushButton(("OK"));
	CancelButton  = new QPushButton(("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addStretch(1);
	MainLayout->addLayout(ScaleLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);
	setLayout(MainLayout);

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_pctrlXScaleFactor, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlYScaleFactor, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlZScaleFactor, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
}


void ScaleSailDlg::EnableControls()
{

}



void ScaleSailDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
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
			break;
		}
		default:
			event->ignore();
	}
}


void ScaleSailDlg::OnOK()
{

	m_XFactor = m_pctrlXScaleFactor->Value();
	m_YFactor = m_pctrlYScaleFactor->Value();
	m_ZFactor = m_pctrlZScaleFactor->Value();

	QDialog::accept();
}


void ScaleSailDlg::OnEditingFinished()
{
	SailDlg *pSailDlg = (SailDlg*)m_pSailDlg;

//	pSailDlg->UpdateSailView();
	pSailDlg->UpdateView();
}





