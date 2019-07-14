/****************************************************************************

    ThreeDWidget Class
    Copyright (C) 2012 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/


#ifndef THREEDWIDGET_H
#define THREEDWIDGET_H

#include <QOpenGLWidget>
#include <QLabel>

#include "params.h"
#include "objects/arcball.h"
#include "objects/rectangle.h"
#include "misc/gllightdlg.h"

class Sail7;
class MainFrame;

class ThreeDWidget : public QOpenGLWidget
{
    Q_OBJECT

    friend class Sail7;
    friend class BoatDlg;
    friend class MainFrame;
    friend class ArcBall;
    friend class GL3dBodyDlg;
    friend class SailViewWidget;

public:
    ThreeDWidget(QWidget *parent = nullptr);

    void renderText(int x, int y, QString const &text);


    void paintEvent(QPaintEvent *event);
    void initializeGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resizeGL(int width, int height);
    void paintGL();//virtual override
    void contextMenuEvent (QContextMenuEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent (QMouseEvent *event);
    void wheelEvent (QWheelEvent *event );
    void showEvent(QShowEvent *pEvent);


    void ClientToGL(QPoint const &point, Vector3d &real);
    void GL2dToClient(Vector3d const &real, QPoint &point);
    void GLInverseMatrix(double MatIn[][4], double MatOut[][4]);

    void GLDrawArrow(Vector3d O, Vector3d u, double length);
    void GLDrawCube(Vector3d Pt, double side);
    void GLDrawCylinder(QColor cr, double rbase, double rtop, double hbase, double htop, int NumArcs, int NumHeight);
    void GLCreateCylinderList(GLuint GLList, QColor cr, double rbase, double rtop, double hbase, double htop, int NumArcs, int NumHeight);
    void GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes);
    void GLCreateSphereList(GLuint GLList, double radius, int NumLongitudes, int NumLatitudes);
    void GLCreateCubeList(GLuint GLList, double side);
    void GLSetupLight(GLLightDlg *pglLightParams, double Offset_y, float LightFactor=1.0f);
    void GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth);
    void CreateArcballList(ArcBall &ArcBall, double GLScale);
    void NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3]);
    void setupViewPort(int width, int height);

    void glRenderText(int x, int y, const QString & str, QColor textColor = QColor(Qt::white));
    void glRenderText(double x, double y, double z, const QString & str, QColor textColor = QColor(Qt::white));
    QPoint worldToScreen(Vector3d v);

    void setLabelFont();
    void setBoatData(QString const &data);
    void setBoatOppData(QString const &data);

private:
    void PaintCpLegendText();
    void PaintPanelForceLegendText(double rmin, double rmax);

private slots:

private:
    void *m_pParent;
    static Sail7 *s_pSail7;
    static MainFrame *s_pMainFrame;

    //    QRect m_rCltRect;        // The client window rectangle
    CRectangle m_GLViewRect; // The OpenGl Viewport

    QLabel *m_pctrlBtData, *m_pctrlBtOppData;
    QPixmap m_PixText;

    int m_iView;
};

#endif //THREEDWIDGET_H
