/****************************************************************************

    SailViewWt Class
    Copyright (C) 2019 Andre Deperrois
    (C) All rights reserved

*****************************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>

#include "sailviewwt.h"
#include "sail7.h"
#include "saildlg.h"
#include "../mainframe.h"
#include "../globals.h"
#include "../view/glsailview.h"


MainFrame *SailViewWt::s_pMainFrame;
Sail7 *SailViewWt::s_pSail7;
SailDlg *SailViewWt::s_pSailDlg;

QPoint SailViewWt::s_WindowPos=QPoint(500,100);
QSize  SailViewWt::s_WindowSize = QSize(500,400);
bool SailViewWt::s_bWindowMaximized = false;


bool SailViewWt::s_bAxes = true;
bool SailViewWt::s_bSurfaces = true;
bool SailViewWt::s_bOutline = true;
bool SailViewWt::s_bCtrlPoints = true;
bool SailViewWt::s_bVLMPanels = true;
bool SailViewWt::s_bglLight = true;


#define SECTIONHIGHLIGHT 6779
#define SAILSURFACE      6781
#define SAILMESHPANELS   6783
#define SAILMESHBACK     6784

#define SIDEPOINTS 23


SailViewWt::SailViewWt() : QWidget()
{
    setWindowTitle("Sail view");
    setWindowFlags(Qt::Dialog);
    m_GLList=0;

    m_bPickCenter = false;
    m_bTrans      = false;
    m_bCrossPoint = m_bArcBall = false;

    m_bResetglArcball = m_bResetglGeom = m_bResetglSailMesh=true;
    m_bResetglSectionHighlight = true;
    m_bResetglCtrlPoints = true;

    m_glScaled = 1.0;


    m_ArcBall.m_pOffx    = &m_SailOffset.x;
    m_ArcBall.m_pOffy    = &m_SailOffset.y;
    m_ArcBall.m_pTransx  = &m_glViewportTrans.x;
    m_ArcBall.m_pTransy  = &m_glViewportTrans.y;

    SetupLayout();
    Connect();
}


void SailViewWt::Connect()
{
    connect(m_pctrlIso, SIGNAL(clicked()),this, SLOT(On3DIso()));
    connect(m_pctrlX, SIGNAL(clicked()),this, SLOT(On3DFront()));
    connect(m_pctrlY, SIGNAL(clicked()),this, SLOT(On3DLeft()));
    connect(m_pctrlZ, SIGNAL(clicked()),this, SLOT(On3DTop()));
    connect(m_pctrlReset, SIGNAL(clicked()),this, SLOT(On3DReset()));
    connect(m_pctrlPickCenter, SIGNAL(clicked()),this, SLOT(On3DPickCenter()));

    connect(m_pctrlAxes,       SIGNAL(clicked()), this, SLOT(OnAxes()));
    connect(m_pctrlPanels,     SIGNAL(clicked()), this, SLOT(OnPanels()));
    connect(m_pctrlSurfaces,   SIGNAL(clicked()), this, SLOT(OnSurfaces()));
    connect(m_pctrlOutline,    SIGNAL(clicked()), this, SLOT(OnOutline()));
    connect(m_pctrlPoints,     SIGNAL(clicked()), this, SLOT(OnCtrlPoints()));
    connect(m_pctrlLight,      SIGNAL(clicked()), this, SLOT(OnLight()));
}


void SailViewWt::InitDialog(Sail *pSail)
{
    m_pSail = pSail;
    m_pctrlSurfaces->setChecked(s_bSurfaces);
    m_pctrlOutline->setChecked(s_bOutline);
    m_pctrlPoints->setChecked(s_bCtrlPoints);
    m_pctrlPanels->setChecked(s_bVLMPanels);
    m_pctrlAxes->setChecked(s_bAxes);
    m_pctrlLight->setChecked(s_bglLight);

    m_pctrlPoints->setEnabled(m_pSail->IsNURBSSail());
    if(m_pSail->IsSailcutSail()) m_pctrlPoints->setChecked(false);

}



void SailViewWt::SetupLayout()
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QVBoxLayout *MainLayout = new QVBoxLayout;
    {
        m_pglSailView = new glSailView(this);
        m_pglSailView->m_iView = GLSAILVIEW;
        m_pglSailView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        m_pglSailView->setMinimumWidth(200);
        m_pglSailView->setMinimumHeight(300);
        m_ArcBall.m_p3dWidget = m_pglSailView;

        QHBoxLayout *AxisViewLayout = new QHBoxLayout;
        {
            QVBoxLayout * pViewCtrlsLayout =new QVBoxLayout;
            {
                m_pctrlAxes       = new QCheckBox(tr("Axes"));
                m_pctrlLight      = new QCheckBox(tr("Light"));
                pViewCtrlsLayout->addWidget(m_pctrlAxes);
                pViewCtrlsLayout->addWidget(m_pctrlLight);
            }

            m_pctrlX          = new QToolButton;
            m_pctrlY          = new QToolButton;
            m_pctrlZ          = new QToolButton;
            m_pctrlIso        = new QToolButton;
            if(m_pctrlX->iconSize().height()<=48)
            {
                m_pctrlX->setIconSize(QSize(32,32));
                m_pctrlY->setIconSize(QSize(32,32));
                m_pctrlZ->setIconSize(QSize(32,32));
                m_pctrlIso->setIconSize(QSize(32,32));
            }
            m_pXView = new QAction(QIcon(":/images/OnXView.png"), tr("X View"), this);
            m_pYView = new QAction(QIcon(":/images/OnYView.png"), tr("Y View"), this);
            m_pZView = new QAction(QIcon(":/images/OnZView.png"), tr("Z View"), this);
            m_pIsoView = new QAction(QIcon(":/images/OnIsoView.png"), tr("Iso View"), this);
            m_pXView->setCheckable(true);
            m_pYView->setCheckable(true);
            m_pZView->setCheckable(true);
            m_pIsoView->setCheckable(true);

            m_pctrlX->setDefaultAction(m_pXView);
            m_pctrlY->setDefaultAction(m_pYView);
            m_pctrlZ->setDefaultAction(m_pZView);
            m_pctrlIso->setDefaultAction(m_pIsoView);

            AxisViewLayout->addLayout(pViewCtrlsLayout);
            AxisViewLayout->addStretch();
            AxisViewLayout->addWidget(m_pctrlX);
            AxisViewLayout->addWidget(m_pctrlY);
            AxisViewLayout->addWidget(m_pctrlZ);
            AxisViewLayout->addWidget(m_pctrlIso);
            AxisViewLayout->addStretch();
        }
        MainLayout->addWidget(m_pglSailView);
        MainLayout->addLayout(AxisViewLayout);

        QHBoxLayout *ViewSettingsLayout = new QHBoxLayout;
        {
            m_pctrlSurfaces   = new QCheckBox(tr("Surfaces"));
            m_pctrlOutline    = new QCheckBox(tr("Outline"));
            m_pctrlPoints     = new QCheckBox(tr("Points"));
            m_pctrlPanels     = new QCheckBox(tr("Panels"));
            ViewSettingsLayout->addWidget(m_pctrlSurfaces);
            ViewSettingsLayout->addWidget(m_pctrlOutline);
            ViewSettingsLayout->addWidget(m_pctrlPoints);
            ViewSettingsLayout->addWidget(m_pctrlPanels);
            ViewSettingsLayout->addStretch();
        }

        QHBoxLayout* ThreeDView = new QHBoxLayout;
        {
            m_pctrlPickCenter = new QPushButton(tr("Pick Center"));
            m_pctrlReset      = new QPushButton(tr("Reset Scales"));
            m_pctrlPickCenter->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            m_pctrlPickCenter->setCheckable(true);
            m_pctrlReset->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            ThreeDView->addStretch();
            ThreeDView->addWidget(m_pctrlReset);
            ThreeDView->addWidget(m_pctrlPickCenter);
            ThreeDView->addStretch();
        }


        MainLayout->addLayout(ViewSettingsLayout);
        MainLayout->addLayout(ThreeDView);
        //        MainLayout->addStretch(2);
    }
    setLayout(MainLayout);
}


void SailViewWt::Set3DScale()
{
    m_glViewportTrans.Set(0.0,0.0,0.0);
    m_bIs3DScaleSet = true;

    SailSection *pSec1 = m_pSail->m_oaSection.last();
    if(pSec1->m_Position.z>0) m_glScaled = 1.0/ pSec1->m_Position.z;
    else                      m_glScaled = 1.0;

    m_SailOffset.x = 0.0;
    m_SailOffset.y = 0.0;
    //    m_SailOffset.y = -pSec1->m_Span/2.0 * m_glScaled;

    m_ArcBall.GetMatrix();
    Vector3d eye(0.0,0.0,1.0);
    Vector3d up(0.0,1.0,0.0);
    m_ArcBall.SetZoom(0.3,eye,up);

    Set3DRotationCenter();
}





void SailViewWt::OnAxes()
{
    s_bAxes = m_pctrlAxes->isChecked();
    //    m_bResetglBody2D = true;
    UpdateView();
}


void SailViewWt::OnLight()
{
    s_bglLight = m_pctrlLight->isChecked();
    UpdateView();
}



void SailViewWt::On3DIso()
{
    UncheckPresetViews();
    m_pctrlIso->setChecked(true);

    m_ArcBall.ab_quat[0]    = -0.65987748f;
    m_ArcBall.ab_quat[1]    =  0.38526487f;
    m_ArcBall.ab_quat[2]    = -0.64508355f;
    m_ArcBall.ab_quat[3]    =  0.0f;
    m_ArcBall.ab_quat[4]    = -0.75137258f;
    m_ArcBall.ab_quat[5]    = -0.33720365f;
    m_ArcBall.ab_quat[6]    =  0.56721509f;
    m_ArcBall.ab_quat[7]    =  0.0f;
    m_ArcBall.ab_quat[8]    =  0.000f;
    m_ArcBall.ab_quat[9]    =  0.85899049f;
    m_ArcBall.ab_quat[10]    =  0.51199043f;
    m_ArcBall.ab_quat[11]    =  0.0f;
    m_ArcBall.ab_quat[12]    =  0.0f;
    m_ArcBall.ab_quat[13]    =  0.0f;
    m_ArcBall.ab_quat[14]    =  0.0f;
    m_ArcBall.ab_quat[15]    =  1.0f;

    Set3DRotationCenter();
    UpdateView();
}


void SailViewWt::On3DTop()
{
    UncheckPresetViews();
    m_pctrlZ->setChecked(true);
    m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
    Set3DRotationCenter();
    UpdateView();
}


void SailViewWt::On3DLeft()
{
    UncheckPresetViews();
    m_pctrlY->setChecked(true);
    m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90 deg around x
    Set3DRotationCenter();
    UpdateView();
}


void SailViewWt::On3DFront()
{
    UncheckPresetViews();
    m_pctrlX->setChecked(true);
    Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
    Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

    m_ArcBall.SetQuat(Qt1 * Qt2);
    Set3DRotationCenter();
    UpdateView();
}


void SailViewWt::On3DReset()
{
    m_glViewportTrans.Set(0.0, 0.0, 0.0);
    m_bPickCenter   = false;
    m_pctrlPickCenter->setChecked(false);
    m_bIs3DScaleSet = false;

    Set3DScale();
    m_bResetglCtrlPoints = true;
    UpdateView();
}


void SailViewWt::On3DPickCenter()
{
    m_bPickCenter = true;
    m_pctrlPickCenter->setChecked(true);
}



void SailViewWt::OnPanels()
{
    s_bVLMPanels = m_pctrlPanels->isChecked();
    UpdateView();
}


void SailViewWt::OnSurfaces()
{
    s_bSurfaces = m_pctrlSurfaces->isChecked();
    UpdateView();
}


void SailViewWt::OnOutline()
{
    s_bOutline = m_pctrlOutline->isChecked();
    UpdateView();
}

void SailViewWt::OnCtrlPoints()
{
    s_bCtrlPoints = m_pctrlPoints->isChecked();
    UpdateView();
}



void SailViewWt::UncheckPresetViews()
{
    m_pctrlX->setChecked(false);
    m_pctrlY->setChecked(false);
    m_pctrlZ->setChecked(false);
    m_pctrlIso->setChecked(false);
}


void SailViewWt::Set3DRotationCenter()
{
    //adjust the new rotation center after a translation or a rotation

    int i,j;

    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            MatOut[i][j] =  m_ArcBall.ab_quat[i*4+j];

    m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
    m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
    m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;
}


void SailViewWt::Set3DRotationCenter(QPoint point)
{
    //adjusts the new rotation center after the user has picked a point on the screen
    //finds the closest sail point under the screen point,
    //and changes the rotation vector and viewport translation
    int  i, j;
    Vector3d I, A, B, AA, BB, U;
    double dist;

    i=-1;

    m_pglSailView->ClientToGL(point, B);

    B.x += -m_SailOffset.x - m_glViewportTrans.x*m_glScaled;
    B.y += -m_SailOffset.y + m_glViewportTrans.y*m_glScaled;

    B *= 1.0/m_glScaled;

    A.Set(B.x, B.y, +1.0);
    B.z = -1.0;

    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

    //convert screen to model coordinates
    AA.x = MatIn[0][0]*A.x + MatIn[0][1]*A.y + MatIn[0][2]*A.z;
    AA.y = MatIn[1][0]*A.x + MatIn[1][1]*A.y + MatIn[1][2]*A.z;
    AA.z = MatIn[2][0]*A.x + MatIn[2][1]*A.y + MatIn[2][2]*A.z;

    BB.x = MatIn[0][0]*B.x + MatIn[0][1]*B.y + MatIn[0][2]*B.z;
    BB.y = MatIn[1][0]*B.x + MatIn[1][1]*B.y + MatIn[1][2]*B.z;
    BB.z = MatIn[2][0]*B.x + MatIn[2][1]*B.y + MatIn[2][2]*B.z;

    U.Set(BB.x-AA.x, BB.y-AA.y, BB.z-AA.z);
    U.Normalize();

    Vector3d LA,LB,TA,TB;
    LA = m_pSail->GetPoint(0.0,0.0);
    LB = m_pSail->GetPoint(0.0,1.0);
    TA = m_pSail->GetPoint(1.0,0.0);
    TB = m_pSail->GetPoint(1.0,1.0);

    if(Intersect(LA, LB, TA, TB, Vector3d(0.0,-1.0,0.0), AA, U, I, dist))
    {
        //        smooth visual transition
        m_pglSailView->GLInverseMatrix(MatIn, MatOut);

        U.x = (-I.x -m_glRotCenter.x)/30.0;
        U.y = (-I.y -m_glRotCenter.y)/30.0;
        U.z = (-I.z -m_glRotCenter.z)/30.0;

        for(i=0; i<30; i++)
        {
            m_glRotCenter +=U;
            m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
            m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
            m_glViewportTrans.z=   (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);

            UpdateView();
        }
    }
}



void SailViewWt::GLCreateSailGeom()
{
    if(m_pSail->m_oaSection.size()<2) return;
    s_pSail7->GLCreateSailGeom(SAILSURFACE, m_pSail, Vector3d(0.0,0.0,0.0));
}



void SailViewWt::GLCreateSailMesh()
{
    //    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
    QColor color;
    int width;
    Vector3d N, LATB, TALB;

    int k,l;
    double tau, tau1, x, x1;

    Vector3d LA, LB, TA, TB;

    glNewList(SAILMESHPANELS,GL_COMPILE);
    {
        m_GLList++;
        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        color = W3dPrefsDlg::s_VLMColor;
        //        style = W3dPrefsDlg::s_VLMStyle;
        width = W3dPrefsDlg::s_VLMWidth;

        glLineWidth(width);

        glColor3d(color.redF(),color.greenF(),color.blueF());


        for (k=0; k<m_pSail->m_NZPanels; k++)
        {
            s_pSail7->GetDistrib(m_pSail->m_NZPanels, COSINE, k, tau, tau1);

            for (l=m_pSail->m_NXPanels-1; l>=0; l--)
            {
                s_pSail7->GetDistrib(m_pSail->m_NXPanels, COSINE, l, x1, x);

                LA = m_pSail->GetPoint(x, tau);
                LB = m_pSail->GetPoint(x, tau1);
                TA = m_pSail->GetPoint(x1, tau);
                TB = m_pSail->GetPoint(x1, tau1);


                LATB = TB - LA;
                TALB = LB - TA;
                N = LATB *TALB;
                N.Normalize();
                glBegin(GL_QUADS);
                {
                    glNormal3d(N.x, N.y, N.z);
                    glVertex3d(TA.x, TA.y, TA.z);
                    glVertex3d(TB.x, TB.y, TB.z);
                    glVertex3d(LB.x, LB.y, LB.z);
                    glVertex3d(LA.x, LA.y, LA.z);
                }
                glEnd();
            }
        }
    }
    glEndList();
}



void SailViewWt::GLCreateCtrlPoints()
{
    glNewList(SAILCTRLPOINTS, GL_COMPILE);
    {
        m_GLList++;
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        glColor3d(0.0,0.0,1.0);
        glLineWidth(W3dPrefsDlg::s_OutlineWidth);

        if(m_pSail->IsNURBSSail())
        {
            for (int j=0; j<m_pSail->m_oaSection.size(); j++)
            {
                for(int kc=0;kc<m_pSail->m_oaSection.first()->m_CtrlPoint.size(); kc++)
                {
                    if(j==s_pSailDlg->m_iSection)
                    {
                        if(kc==m_pSail->m_oaSection[j]->m_iSelect) glColor3d(0.4, 0.4, 1.0);
                        else                                       glColor3d(0.9, 0.1, 0.1);
                    }
                    else                     glColor3d(.5,.5,.5);

                    m_pglSailView->GLDrawCube(m_pSail->m_oaSection[j]->m_CtrlPoint[kc], .015/m_glScaled);
                }
            }
        }
        else
        {
            /*            m_p3DWidget->GLCreateCube(m_pSail->tack, .015/m_glScaled);
            m_p3DWidget->GLCreateCube(m_pSail->clew, .015/m_glScaled);
            m_p3DWidget->GLCreateCube(m_pSail->head, .015/m_glScaled);
            m_p3DWidget->GLCreateCube(m_pSail->peak, .015/m_glScaled);*/
        }
    }
    glEndList();
}


void SailViewWt::GLCreateSectionHighlight()
{
    if(m_pSail->m_oaSection.size()<2) return;

    if(s_pSailDlg->m_iSection!=0 && s_pSailDlg->m_iSection!=m_pSail->m_oaSection.size()-1) return;

    glNewList(SECTIONHIGHLIGHT, GL_COMPILE);
    {
        /*        glDisable (GL_BLEND);
        m_GLList++;

        CVector PtA;

        glColor3d(1.0,0.0,0.0);
        glLineWidth(3);

        glBegin(GL_LINE_STRIP);
        {
            for (int l=0; l<=SIDEPOINTS; l++)
            {
                PtA = m_pSail->GetSectionPoint(pSDlg->m_iSection, (double)l/(double)SIDEPOINTS);
                glVertex3d(PtA.x, PtA.y, PtA.z);
            }
        }
        glEnd();*/
    }
    glEndList();
}



void SailViewWt::GLDraw3D()
{
    glClearColor(s_pMainFrame->m_BackgroundColor.redF(), s_pMainFrame->m_BackgroundColor.greenF(), s_pMainFrame->m_BackgroundColor.blueF(),0.0);

    if(m_bResetglArcball)
    {
        m_ArcBall.GetMatrix();
        Vector3d eye(0.0,0.0,1.0);
        Vector3d up(0.0,1.0,0.0);
        m_ArcBall.SetZoom(0.3,eye,up);

        if(glIsList(ARCBALL))
        {
            glDeleteLists(ARCBALL,2);
            m_GLList-=2;
        }
        m_pglSailView->CreateArcballList(m_ArcBall, 1.0);
        m_GLList+=2;
        m_bResetglArcball = false;
    }

    if(m_bResetglGeom && m_pSail)
    {
        if(glIsList(SAILSURFACE))
        {
            glDeleteLists(SAILSURFACE,1);
            glDeleteLists(SAILSURFACE+MAXSAILS,1);
            m_GLList -=2;
        }
        GLCreateSailGeom();
    }

    if(m_pSail)
    {
        if(m_bResetglGeom || m_bResetglCtrlPoints)
        {
            if(glIsList(SAILCTRLPOINTS))
            {
                glDeleteLists(SAILCTRLPOINTS,1);
                m_GLList -=1;
            }
            GLCreateCtrlPoints();
            m_bResetglCtrlPoints = false;
        }
    }

    if(m_pSail)
    {
        if(m_bResetglGeom || m_bResetglSectionHighlight)
        {
            if(glIsList(SECTIONHIGHLIGHT))
            {
                glDeleteLists(SECTIONHIGHLIGHT, 1);
            }
            GLCreateSectionHighlight();
            m_bResetglSectionHighlight = false;
        }
    }

    if((m_bResetglGeom || m_bResetglSailMesh) && m_pSail)
    {
        if(glIsList(SAILMESHPANELS))
        {
            glDeleteLists(SAILMESHPANELS,2);
            m_GLList -=2;
        }
        GLCreateSailMesh();
        m_bResetglSailMesh = false;
        m_bResetglGeom = false;
    }
}


void SailViewWt::GLRenderSail()
{
    //    int width;
    //    width = m_rCltRect.width();

    glPushMatrix();
    {
        glFlush();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Display 2D View
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);


        glPushMatrix();
        {
            m_pglSailView->GLSetupLight(&s_pSail7->m_GLLightDlg, m_SailOffset.y, 1.0);

            glLoadIdentity();

            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);

            if(m_bCrossPoint && m_bArcBall)
            {
                glPushMatrix();
                {
                    glTranslated(m_SailOffset.x, m_SailOffset.y,  0.0);
                    m_ArcBall.RotateCrossPoint();
                    glRotated(m_ArcBall.angle, m_ArcBall.p.x, m_ArcBall.p.y, m_ArcBall.p.z);
                    glCallList(ARCPOINT);
                }
                glPopMatrix();
            }

            if(m_bArcBall)
            {
                glPushMatrix();
                {
                    glTranslated(m_SailOffset.x, m_SailOffset.y,  0.0);
                    m_ArcBall.Rotate();
                    glCallList(ARCBALL);
                }
                glPopMatrix();
            }

            glTranslated(m_SailOffset.x, m_SailOffset.y,  0.0);

            m_ArcBall.Rotate();

            glScaled(m_glScaled, m_glScaled, m_glScaled);
            glTranslated(m_glRotCenter.x, m_glRotCenter.y, m_glRotCenter.z);
            if(s_bAxes) m_pglSailView->GLDrawAxes(1.0/m_glScaled, W3dPrefsDlg::s_3DAxisColor, W3dPrefsDlg::s_3DAxisStyle, W3dPrefsDlg::s_3DAxisWidth);

            if(s_bglLight)
            {
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
            }
            else
            {
                glDisable(GL_LIGHTING);
                glDisable(GL_LIGHT0);
            }

            if(s_bSurfaces && m_pSail && glIsList(SAILSURFACE))
                glCallList(SAILSURFACE);

            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);

            if(s_bOutline && m_pSail    && glIsList(SAILSURFACE+MAXSAILS)) glCallList(SAILSURFACE+MAXSAILS);
            if(s_bOutline && glIsList(SECTIONHIGHLIGHT))                   glCallList(SECTIONHIGHLIGHT);
            if(s_bVLMPanels && m_pSail  && glIsList(SAILMESHPANELS))       glCallList(SAILMESHPANELS);
            if(s_bCtrlPoints && m_pSail && glIsList(SAILCTRLPOINTS))       glCallList(SAILCTRLPOINTS);
        }
        glPopMatrix();
    }
    glPopMatrix();
    glFinish();
}


void SailViewWt::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint point(event->pos().x(), event->pos().y());

    Set3DRotationCenter(point);
    m_bPickCenter = false;
    m_pctrlPickCenter->setChecked(false);
    UpdateView();
}


void SailViewWt::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point(event->pos().x(), event->pos().y());
    QPoint glPoint(event->pos().x() + m_pglSailView->geometry().x(), event->pos().y()+m_pglSailView->geometry().y());
    Vector3d Real;

    if(!hasFocus()) setFocus();

    QPoint Delta(point.x() - m_LastPoint.x(), point.y() - m_LastPoint.y());
    m_pglSailView->ClientToGL(point, Real);

    bool bCtrl = false;

    if (event->modifiers() & Qt::ControlModifier) bCtrl =true;

    if (event->buttons()   & Qt::LeftButton)
    {
        if(bCtrl&& m_pglSailView->geometry().contains(glPoint))
        {
            //rotate
            UncheckPresetViews();
            m_ArcBall.Move(point.x(), m_pglSailView->rect().height()-point.y());
            UpdateView();
        }
        else if(m_bTrans)
        {
            //translate
            if(m_pglSailView->geometry().contains(glPoint))
            {
                m_glViewportTrans.x += GLfloat(Delta.x()*2.0/m_glScaled/m_pglSailView->rect().width());
                m_glViewportTrans.y += GLfloat(Delta.y()*2.0/m_glScaled/m_pglSailView->rect().width());

                m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
                m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
                m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

                UpdateView();
            }
        }
    }

    else if (event->buttons() & Qt::MidButton)
    {
        if(m_pSail)
        {
            UncheckPresetViews();
            m_ArcBall.Move(point.x(), m_pglSailView->rect().height()-point.y());
            UpdateView();
        }
    }

    m_LastPoint = point;
}


void SailViewWt::mousePressEvent(QMouseEvent *event)
{
    // the event has been sent by GLWidget, so event is in GL Widget coordinates
    // but m_3DWingRect is in window client coordinates
    // the difference is m_p3DWidget->geometry() !

    QPoint point(event->pos().x(), event->pos().y());
    QPoint glPoint(event->pos().x() + m_pglSailView->geometry().x(), event->pos().y()+m_pglSailView->geometry().y());

    Vector3d Real;
    bool bCtrl = false;
    if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

    m_pglSailView->ClientToGL(point, Real);

    if (event->buttons() & Qt::MidButton)
    {
        m_bArcBall = true;
        m_ArcBall.Start(event->pos().x(), m_pglSailView->rect().height()-event->pos().y());
        m_bCrossPoint = true;

        Set3DRotationCenter();
        UpdateView();
    }
    else if (event->buttons() & Qt::LeftButton)
    {
        if(m_bPickCenter)
        {
            Set3DRotationCenter(point);
            m_bPickCenter = false;
            m_pctrlPickCenter->setChecked(false);
            UpdateView();
        }
        else
        {
            m_bTrans=true;

            if(m_pSail && m_pglSailView->geometry().contains(glPoint))
            {
                m_ArcBall.Start(point.x(), m_pglSailView->rect().height()-point.y());
                m_bCrossPoint = true;
                Set3DRotationCenter();
                if (!bCtrl)
                {
                    m_bTrans = true;
                    m_pglSailView->setCursor(Qt::ClosedHandCursor);
                }
                else
                {
                    m_bArcBall = true;
                }
                UpdateView();
            }
        }
    }

    m_bPickCenter = false;
    m_pctrlPickCenter->setChecked(false);
    m_PointDown = point;
    m_LastPoint = point;
}



void SailViewWt::mouseReleaseEvent(QMouseEvent *)
{
    m_pglSailView->setCursor(Qt::CrossCursor);

    m_bTrans = false;

    m_bArcBall = false;
    m_bCrossPoint = false;
    UpdateView();

    //    We need to re-calculate the translation vector
    int i,j;
    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

    GLInverseMatrix();
    m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
    m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
    m_glViewportTrans.z =  (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);

}




void  SailViewWt::wheelEvent(QWheelEvent *event)
{
    if(m_pglSailView->rect().contains(event->pos()))
    {
        QPoint point(event->pos().x() - m_pglSailView->geometry().x(), event->pos().y() - m_pglSailView->geometry().y());
        //The mouse button has been wheeled
        //Process the message
        //    point is in client coordinates

        if(event->delta()<0) m_glScaled *= (GLfloat)1.06;
        else                 m_glScaled /= (GLfloat)1.06;

        m_bResetglCtrlPoints = true;
    }

    UpdateView();
}



void SailViewWt::GLInverseMatrix()
{
    //Step 1. Transpose the 3x3 rotation portion of the 4x4 matrix to get the inverse rotation
    int i,j;

    for(i=0 ; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            MatOut[j][i] = MatIn[i][j];
        }
    }
}


void SailViewWt::keyPressEvent(QKeyEvent *event)
{
    bool bShift = false;
    bool bCtrl  = false;
    if(event->modifiers() & Qt::ShiftModifier)   bShift =true;
    if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

    switch (event->key())
    {
        case Qt::Key_Z:
        {
            if(bCtrl)
            {
                //                if(bShift) OnRedo();
                //                else OnUndo();
                //                event->accept();
            }
            else event->ignore();
            break;
        }
        case Qt::Key_R:
        {
            Set3DScale();
            UpdateView();
            break;
        }
        case Qt::Key_Control:
        {
            m_bArcBall = true;
            UpdateView();
            break;
        }
        default:
            event->ignore();
    }
}


void SailViewWt::UpdateView()
{
    if(isVisible()) m_pglSailView->update();
}


void SailViewWt::closeEvent(QCloseEvent *)
{
    s_WindowPos = pos();
    s_WindowSize = size();
    s_bWindowMaximized = isMaximized();
}


void SailViewWt::hideEvent(QHideEvent *)
{
    s_WindowPos = pos();
    s_WindowSize = size();
    s_bWindowMaximized = isMaximized();
}


void SailViewWt::showEvent(QShowEvent *)
{
    move(s_WindowPos);
    resize(s_WindowSize);
    if(s_bWindowMaximized) setWindowState(Qt::WindowMaximized);
}
