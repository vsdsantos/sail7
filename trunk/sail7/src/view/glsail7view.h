/****************************************************************************

    glSail7View Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/


#ifndef GLSAIL7VIEW_H
#define GLSAIL7VIEW_H

#include "threedwidget.h"

class glSail7View : public ThreeDWidget
{
    public:
        glSail7View(QWidget *parent = nullptr);
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
};

#endif // GLSAILVIEW_H
