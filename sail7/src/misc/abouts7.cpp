/****************************************************************************

    AboutS7 Class
    Copyright (C) 2011 Andre Deperrois 

    All rights reserved

*****************************************************************************/

#include <QtGui>
#include "abouts7.h"
#include "../mainframe.h"
#include <QBitmap>
#include <QGridLayout>

AboutS7::AboutS7(MainFrame *parent)
{
    setWindowTitle(tr("About Sail7"));
    m_pMainFrame = parent;
    SetupLayout();
}


void AboutS7::SetupLayout()
{
    QGridLayout *LogoLayout = new QGridLayout;
    QLabel *LabIconQ5 = new QLabel;
//    LabIconQ5->setGeometry(QRect(20, 0, 61, 71));

    LabIconQ5->setPixmap(QPixmap(QString::fromUtf8(":/images/Sailboat.png")));
    QLabel *lab1  = new QLabel(m_pMainFrame->m_VersionName);
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

    QLabel *lab3  = new QLabel(tr("Copyright (C) Andre Deperrois 2003-2019"));
    QLabel *lab5  = new QLabel(tr("This program is distributed in the hope that it will be useful,"));
    QLabel *lab6  = new QLabel(tr("but WITHOUT ANY WARRANTY; without even the implied warranty of"));
    QLabel *lab7  = new QLabel(tr("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."));


    QPushButton *OKButton = new QPushButton(tr("OK"));
    connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
    QHBoxLayout *OKLayout = new QHBoxLayout;
    OKLayout->addStretch(1);
    OKLayout->addWidget(OKButton);
    OKLayout->addStretch(1);

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    {
        pMainLayout->addLayout(LogoLayout);
        pMainLayout->addWidget(lab3);
        pMainLayout->addSpacing(30);

        pMainLayout->addWidget(lab5);
        pMainLayout->addWidget(lab6);
        pMainLayout->addWidget(lab7);


        pMainLayout->addLayout(OKLayout);
    }
    setLayout(pMainLayout);
    setMinimumHeight(400);
}


