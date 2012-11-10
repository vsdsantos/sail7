/****************************************************************************

	Sail7 Application
	Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
	All rights reserved

*****************************************************************************/


#include <QtGui/QApplication>
#include "sail7Application.h"
#include "mainframe.h"
#include <QDate>
#include <QMessageBox>


int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x040600
//	QGL::setPreferredPaintEngine (QPaintEngine::OpenGL);
#endif
	QDate dt = QDate::currentDate();
	if(dt.year()<2013 && dt.month()<10) return -1;

	sail7Application app(argc, argv);
	return app.exec();

}

