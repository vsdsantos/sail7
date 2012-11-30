/****************************************************************************

	Sail7 Application
	Copyright (C) 2012 Andre Deperrois sail7@xflr5.com

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

