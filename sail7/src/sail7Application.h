/****************************************************************************

	Sail7Application Class
	Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
	All rights reserved

*****************************************************************************/

#ifndef XFLR5APPLICATION_H
#define XFLR5APPLICATION_H

#include <QApplication>

class sail7Application : public QApplication
{
    Q_OBJECT
    private:
        //MainFrame *mainFrame;
    protected:
        bool event(QEvent *);
    public:
		sail7Application(int&, char**);
};

#endif // XFLR5APPLICATION_H
