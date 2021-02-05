/****************************************************************************

    glSail7View Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/


#ifndef GLSAIL7VIEW_H
#define GLSAIL7VIEW_H

#define WATERLIST 7778
#define WINDLIST  7779
#define NORMALLIST 7777

#include "threedwidget.h"
class Boat;
class BoatPolar;
class glSail7View : public ThreeDWidget
{
    public:
        glSail7View(QWidget *parent = nullptr);
        ~glSail7View();

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
        void GLCreateWaterList();
        void GLCreateWindList(Boat *pBoat, BoatPolar *pBtPolar);
};

#endif // GLSAILVIEW_H

