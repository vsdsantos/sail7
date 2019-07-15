/****************************************************************************

    glHullView Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/

#ifndef GLHULLVIEW_H
#define GLHULLVIEW_H

#include "threedwidget.h"

class GL3dBodyDlg;

class glHullView : public ThreeDWidget
{
    public:
        glHullView(QWidget *pParent);

        void contextMenuEvent (QContextMenuEvent * event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseDoubleClickEvent (QMouseEvent *event);
        void wheelEvent (QWheelEvent *event );
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);

        void resizeGL(int width, int height);
        void paintGL();//virtual override

    private:
        GL3dBodyDlg *m_pglHullDlg;
};

#endif // GLHULLVIEW_H
