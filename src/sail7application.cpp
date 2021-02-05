/****************************************************************************

    Sail7Application  Class
    Copyright (C) 2012 Andre Deperrois

    This program is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation;
    either version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this program; if not, write to the
    Free Software Foundation, Inc.,
    59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include <QFileOpenEvent>
#include <QApplication>
#include <QSplashScreen>
#include <QMessageBox>
#include <QDateTime>
#include <QtDebug>

#include "sail7application.h"
#include "mainframe.h"
#include "sail7/gl3dscales.h"

sail7Application::sail7Application(int &argc, char** argv) : QApplication(argc, argv)
{
    QPixmap pixmap;
    pixmap.load(":/images/splash.png");
    QSplashScreen splash(pixmap);
    splash.setWindowFlags(Qt::SplashScreen);
    splash.show();

    QString StyleName;
    QString LanguagePath ="";

    QCoreApplication::setOrganizationName("VLMApps");
    QCoreApplication::setOrganizationDomain("xflr5.com");
    QCoreApplication::setApplicationName("Sail7");

    QString str;
    int a,b,c,d,k;
    a=150;
    b=50;
    c=800;
    d=700;


#ifdef Q_WS_MAC
    QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Sail7");
#else
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Sail7");
#endif

    bool bMaximized = true;
    bool bOK;

    qsrand(QDateTime::currentDateTime().toTime_t());

    settings.beginGroup("MainFrame");
    {
        k = settings.value("FrameGeometryx").toInt(&bOK);
        if(bOK) a = k;
        k = settings.value("FrameGeometryy").toInt(&bOK);
        if(bOK) b = k;
        k = settings.value("SizeWidth").toInt(&bOK);
        if(bOK) c = k;
        k = settings.value("SizeHeight").toInt(&bOK);
        if(bOK) d = k;

        bMaximized = settings.value("SizeMaximized").toBool();

        str = settings.value("LanguageFilePath").toString();
        if(str.length()) LanguagePath = str;

        str = settings.value("StyleName").toString();
        if(str.length()) StyleName = str;
    }
    settings.endGroup();

    QTranslator xflr5Translator;

    if(LanguagePath.length())
    {
        if(xflr5Translator.load(LanguagePath)) installTranslator(&xflr5Translator);
    }

    QPoint pt(a,b);
    QSize sz(c,d);

    if(StyleName.length())    qApp->setStyle(StyleName);

    MainFrame *w = MainFrame::self();
    MainFrame::self()->resize(sz);
    MainFrame::self()->move(pt);
    MainFrame::self()->OnSail7();

    if(bMaximized)    MainFrame::self()->showMaximized();
    else MainFrame::self()->show();

#ifndef Q_WS_MAC
    QString PathName, Extension;
    PathName=argv[1];


    Extension = PathName.right(4);
    if(Extension.compare(".sl7",Qt::CaseInsensitive)==0)
    {
        w->LoadFile(PathName);
        //        if (iApp == SAIL7)             w->OnSail7();
    }
#endif

    splash.finish(MainFrame::self());
}


bool sail7Application::event(QEvent *event)
{
    int iApp;
    switch (event->type())
    {
        case QEvent::FileOpen:
        {
            iApp = MainFrame::self()->LoadFile(static_cast<QFileOpenEvent *>(event)->file());
            if (iApp == SAIL7) MainFrame::self()->OnSail7();

            return true;
        }
        default:
            return QApplication::event(event);
    }
}


