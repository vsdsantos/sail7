/****************************************************************************

		 SelectObjectDlg Class
		 Copyright (C) 2012 Andre Deperrois 
		 All rights reserved

*****************************************************************************/

#include "SelectObjectDlg.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QVBoxLayout>



SelectObjectDlg::SelectObjectDlg(void *pParent)
{
	setWindowTitle(tr("Import"));
	SetupLayout();

	connect(m_pctrlNameList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(OnOK()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void SelectObjectDlg::SetupLayout()
{
	QVBoxLayout *MainLayout = new QVBoxLayout;

	m_pctrlMessage = new QLabel("A Message here");


	m_pctrlNameList = new QListWidget;

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);


	MainLayout->setStretchFactor(m_pctrlMessage, 1);
	MainLayout->setStretchFactor(m_pctrlNameList, 5);
	MainLayout->setStretchFactor(CommandButtons, 1);

	MainLayout->addWidget(m_pctrlMessage);
	MainLayout->addWidget(m_pctrlNameList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);

	MainLayout->setStretchFactor(m_pctrlMessage, 1);
	MainLayout->setStretchFactor(m_pctrlNameList, 5);
	MainLayout->setStretchFactor(CommandButtons, 1);

	setLayout(MainLayout);
}


void SelectObjectDlg::InitDialog(QString Question, QStringList *pstrList)
{
	m_pctrlNameList->clear();

	m_pctrlMessage->setText(Question);

	m_pctrlNameList->setFocus();

	if(pstrList && pstrList->size())
	{
		for (int i=0; i<pstrList->size(); i++)
		{
			m_pctrlNameList->addItem(pstrList->at(i));
		}
	}
	else
	{
		m_pctrlNameList->setEnabled(false);
	}
}



void SelectObjectDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
			}
			else
			{
				OnOK();
			}
			return;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		default:
			event->ignore();
	}

}





void SelectObjectDlg::OnOK()
{
	QListWidgetItem *pItem =  m_pctrlNameList->currentItem();

	if(pItem) m_SelectedName = pItem->text();

	if (!m_SelectedName.length())
	{
		reject();
		return;
	}

	QDialog::accept();
}







