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
    QGridLayout *pLogoLayout = new QGridLayout;
    {
        QLabel *pLabIconQ5 = new QLabel;
    //    LabIconQ5->setGeometry(QRect(20, 0, 61, 71));

        pLabIconQ5->setPixmap(QPixmap(QString::fromUtf8(":/images/sailboat.png")));
        QLabel *lab1  = new QLabel(m_pMainFrame->m_VersionName);
        lab1->setAlignment(Qt::AlignVCenter| Qt::AlignLeft);
        QFont vFont;
        vFont.setPointSize(29);
        vFont.setItalic(true);
        vFont.setBold(true);
        lab1->setFont(vFont);

        pLogoLayout->setColumnStretch(1,1);
        pLogoLayout->setColumnStretch(2,2);
        pLogoLayout->addWidget(pLabIconQ5,1,1,2,1);
        pLogoLayout->addWidget(lab1,1,2);

    }

    QHBoxLayout *pOKLayout = new QHBoxLayout;
    {
        QPushButton *pOKButton = new QPushButton(tr("OK"));
        connect(pOKButton, SIGNAL(clicked()), SLOT(accept()));
        pOKLayout->addStretch(1);
        pOKLayout->addWidget(pOKButton);
        pOKLayout->addStretch(1);
     }

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    {
        QLabel *lab3  = new QLabel(tr("Copyright (C) Andre Deperrois 2003-2019"));
        QLabel *lab5  = new QLabel(tr("This program is distributed in the hope that it will be useful,"));
        QLabel *lab6  = new QLabel(tr("but WITHOUT ANY WARRANTY; without even the implied warranty of"));
        QLabel *lab7  = new QLabel(tr("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."));
        pMainLayout->addLayout(pLogoLayout);
        pMainLayout->addWidget(lab3);
        pMainLayout->addSpacing(30);

        pMainLayout->addWidget(lab5);
        pMainLayout->addWidget(lab6);
        pMainLayout->addWidget(lab7);


        pMainLayout->addLayout(pOKLayout);
    }
    setLayout(pMainLayout);
    setMinimumHeight(400);
}


