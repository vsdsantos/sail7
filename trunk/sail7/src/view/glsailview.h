/****************************************************************************

    glSailView Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/

#ifndef GLSAILVIEW_H
#define GLSAILVIEW_H

#include "threedwidget.h"

class SailViewWt;

class glSailView : public ThreeDWidget
{
    friend class SailViewWt;

    public:
        glSailView(QWidget *pParent);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseDoubleClickEvent (QMouseEvent *event);
        void wheelEvent (QWheelEvent *event );
        void keyPressEvent(QKeyEvent *event);

        void paintGL();//virtual override};

    private:
        SailViewWt *m_pSailViewWt;
};
#endif // GLSAILVIEW_H
