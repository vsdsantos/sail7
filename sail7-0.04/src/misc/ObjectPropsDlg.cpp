/****************************************************************************

	ObjectPropsDlg Class
	Copyright (C) 2010 Andre Deperrois sail7@xflr5.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include "ObjectPropsDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>

#include "../mainframe.h"
#include "../globals.h"

void * ObjectPropsDlg::s_pMainFrame;

ObjectPropsDlg::ObjectPropsDlg()
{
	m_pBoatPolar  = NULL;
	m_pBoatOpp    = NULL;

	SetupLayout();
}


void ObjectPropsDlg::SetupLayout()
{
	setMinimumHeight(400);
	setMinimumWidth(500);

	m_pctrlDescription = new QTextEdit;
	m_pctrlDescription->setFontFamily("Courier");
	m_pctrlDescription->setReadOnly(true);
	m_pctrlDescription->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlDescription->setWordWrapMode(QTextOption::NoWrap);

//	QVBoxLayout *PropertiesLayout = new QVBoxLayout;
//	PropertiesLayout->addWidget(lab);
//	PropertiesLayout->addWidget(m_pctrlDescription);

	QPushButton *OKButton = new QPushButton(tr("OK"));
	QHBoxLayout *CommandButtons = new QHBoxLayout;
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	MainLayout->addWidget(m_pctrlDescription);
//	MainLayout->addStretch(1);
	MainLayout->addSpacing(20);
	MainLayout->addLayout(CommandButtons);
//	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void ObjectPropsDlg::InitDialog()
{
	QString strange;

	if(m_pBoatPolar)
	{
		m_pBoatPolar->GetPolarProperties(strange, 1, true);
		setWindowTitle(tr("Polar Properties"));
	}
	else if(m_pBoatOpp)
	{
		m_pBoatOpp->GetBoatOppProperties(strange);
		setWindowTitle(tr("Operating Point Properties"));
	}
	m_pctrlDescription->setText(strange);
}








