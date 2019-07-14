/****************************************************************************

    TwoDWidget Class
    Copyright (C) 2012 Andre Deperrois 
    All rights reserved

*****************************************************************************/



#ifndef TwoDWidget_H
#define TwoDWidget_H

#include <QWidget>
#include "params.h"

class MainFrame;
class Sail7;

class TwoDWidget : public QWidget
{
    Q_OBJECT

    friend class MainFrame;
    friend class Sail7;
    friend class SectionWidget;
    friend class SailDlg;

public:
    TwoDWidget(QWidget *parent = nullptr);
    //    void UpdateView();


protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent (QWheelEvent *event );
    void resizeEvent ( QResizeEvent * event );
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void contextMenuEvent (QContextMenuEvent * event);

private:

    static MainFrame *s_pMainFrame;
    static Sail7 *s_pSail7;

    void *m_pParent;
    int m_iView;

    double m_Scale, m_RefScale, m_ScaleY;
};

#endif
