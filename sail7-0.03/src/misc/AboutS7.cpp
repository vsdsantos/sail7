/****************************************************************************

	AboutS7 Class
	Copyright (C) 2011 Andre Deperrois sail7@xflr5.com

	All rights reserved

*****************************************************************************/

#include <QtGui>
#include "AboutS7.h"
#include "../mainframe.h"
#include <QBitmap>


AboutS7::AboutS7(void *parent)
{
	setWindowTitle(tr("About Sail7"));
	m_pMainFrame = parent;
	SetupLayout();
}


void AboutS7::SetupLayout()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QGridLayout *LogoLayout = new QGridLayout;
	QLabel *LabIconQ5 = new QLabel;
//	LabIconQ5->setGeometry(QRect(20, 0, 61, 71));

	LabIconQ5->setPixmap(QPixmap(QString::fromUtf8(":/images/Sailboat.png")));
	QLabel *lab1  = new QLabel(pMainFrame->m_VersionName);
	lab1->setAlignment(Qt::AlignVCenter| Qt::AlignLeft);
	QFont vFont;
	vFont.setPointSize(29);
	vFont.setItalic(true);
	vFont.setBold(true);
	lab1->setFont(vFont);

	LogoLayout->setColumnStretch(1,1);
	LogoLayout->setColumnStretch(2,2);
	LogoLayout->addWidget(LabIconQ5,1,1,2,1);
	LogoLayout->addWidget(lab1,1,2);

	QLabel *lab3  = new QLabel(tr("Copyright (C) Andre Deperrois 2003-2012"));
	QLabel *lab4  = new QLabel(tr("Bug reports, comments and suggestions to sail7@xflr5.com"));
	QLabel *lab5  = new QLabel(tr("This program is distributed in the hope that it will be useful,"));
	QLabel *lab6  = new QLabel(tr("but WITHOUT ANY WARRANTY; without even the implied warranty of"));
	QLabel *lab7  = new QLabel(tr("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."));


/*	QLabel *sail7Link = new QLabel;
	sail7Link->setText("Comments and suggestions to : sail7@xflr5.com");
	sail7Link->setOpenExternalLinks(true);
	sail7Link->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);
	sail7Link->setAlignment(Qt::AlignVCenter| Qt::AlignLeft);*/

	QPushButton *OKButton = new QPushButton(tr("OK"));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	QHBoxLayout *OKLayout = new QHBoxLayout;
	OKLayout->addStretch(1);
	OKLayout->addWidget(OKButton);
	OKLayout->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(LogoLayout);
	MainLayout->addWidget(lab3);
	MainLayout->addWidget(lab4);
	MainLayout->addSpacing(30);

	MainLayout->addWidget(lab5);
	MainLayout->addWidget(lab6);
	MainLayout->addWidget(lab7);


	MainLayout->addLayout(OKLayout);
	setLayout(MainLayout);
	setMinimumHeight(400);
}


