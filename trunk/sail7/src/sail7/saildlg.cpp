/****************************************************************************

         SailDlg Class
         Copyright (C) 2012 Andre Deperrois
         All rights reserved

*****************************************************************************/

#include <QDesktopWidget>
#include <QFileDialog>
#include <QSplitter>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QColorDialog>
#include <QHeaderView>
#include <QToolBar>
#include <math.h>
#include <QtDebug>


#include "saildlg.h"
#include "sailviewwt.h"
#include "../mainframe.h"
#include "../globals.h"
#include "../view/threedwidget.h"
#include "../objects/nurbssail.h"
#include "../objects/sailcutsail.h"
#include "sail7.h"
#include "saildlg.h"
#include "scalesaildlg.h"

MainFrame* SailDlg::s_pMainFrame;
Sail7* SailDlg::s_pSail7;


QPoint SailDlg::s_WindowPos;
QSize  SailDlg::s_WindowSize;
bool SailDlg::s_bWindowMaximized;


SailDlg::SailDlg(QWidget *pParent) : QDialog(pParent)
{
    setWindowTitle("Sail Edition");
    setWindowFlags(Qt::Window);

    SailViewWt::s_pSailDlg = this;

    m_pSail=nullptr;

    m_bChanged  = false;
    m_iSection  = -1;

    move(s_WindowPos);
    resize(s_WindowSize);
    if(s_bWindowMaximized) setWindowState(Qt::WindowMaximized);

    SetupLayout();
    Connect();
}


void SailDlg::Connect()
{
    connect(m_pctrlSailColor, SIGNAL(clicked()), this, SLOT(OnSailColor()));

    connect(m_pctrlInsertBefore, SIGNAL(clicked()),this, SLOT(OnInsertBefore()));
    connect(m_pctrlInsertAfter, SIGNAL(clicked()),this, SLOT(OnInsertAfter()));
    connect(m_pctrlDeleteSection, SIGNAL(clicked()),this, SLOT(OnDeleteSection()));

    connect(m_pctrlNXPanels,   SIGNAL(editingFinished()), this, SLOT(OnUpdate()));
    connect(m_pctrlNZPanels,   SIGNAL(editingFinished()), this, SLOT(OnUpdate()));
    connect(m_pctrlLuffAngle,  SIGNAL(editingFinished()), this, SLOT(OnUpdate()));
    connect(m_pctrlLeechRound,    SIGNAL(editingFinished()), this, SLOT(OnUpdateLeechRound()));
    connect(m_pctrlLeechRoundPos, SIGNAL(editingFinished()), this, SLOT(OnUpdateLeechRound()));

    connect(m_pctrlScaleSail,  SIGNAL(clicked()), this, SLOT(OnScaleSail()));
    connect(m_pctrlImportSail,  SIGNAL(clicked()), this, SLOT(OnImportSail()));
    connect(m_pctrlExportSail,  SIGNAL(clicked()), this, SLOT(OnExportSail()));

    connect(m_pctrlGaffTwist,    SIGNAL(editingFinished()), this, SLOT(OnTwistChanged()));

    connect(m_pctrlNXDegree, SIGNAL(editingFinished()), this, SLOT(OnUpdate()));
    connect(m_pctrlNZDegree, SIGNAL(editingFinished()), this, SLOT(OnUpdate()));

    connect(m_pctrlEdgeWeightu, SIGNAL(editingFinished()), this, SLOT(OnUpdate()));
    connect(m_pctrlEdgeWeightv, SIGNAL(editingFinished()), this, SLOT(OnUpdate()));


    connect(m_pZoomYOnly,      SIGNAL(triggered()), m_pSectionWidget, SLOT(OnZoomYScale()));
    connect(m_pZoomIn, SIGNAL(triggered()), m_pSectionWidget, SLOT(OnZoomIn()));
    connect(m_pZoomOut, SIGNAL(triggered()), m_pSectionWidget, SLOT(OnZoomOut()));
    connect(m_pResetXScale, SIGNAL(triggered()), m_pSectionWidget, SLOT(OnResetXScale()));
    connect(m_pResetYScale, SIGNAL(triggered()), m_pSectionWidget, SLOT(OnResetYScale()));
    connect(m_pResetScales,    SIGNAL(triggered()), m_pSectionWidget, SLOT(OnResetScale()));
    connect(m_pUndo, SIGNAL(triggered()), m_pSectionWidget, SLOT(OnUndo()));
    connect(m_pRedo, SIGNAL(triggered()), m_pSectionWidget, SLOT(OnRedo()));

    connect(m_pctrlAlignedLE, SIGNAL(toggled(bool)), this, SLOT(OnAlignedLE()));

    connect(m_pctrlOK, SIGNAL(clicked()),this, SLOT(OnOK()));
    connect(m_pctrlCancel, SIGNAL(clicked()),this, SLOT(OnCancel()));
}



void SailDlg::OnUpdateLeechRound()
{
    m_bChanged = true;
    if(m_pSail->IsNURBSSail())
    {
    }
    else if(m_pSail->IsSailcutSail())
    {
        SailcutSail *pSCSail = (SailcutSail*)m_pSail;
        pSCSail->m_LeechRound = m_pctrlLeechRound->Value();
        pSCSail->m_LeechRoundPos = m_pctrlLeechRoundPos->Value()/100.0;
        pSCSail->SetLeechRoundEquation();

        FillSectionTable();//to update mid chord
    }

    UpdateView();
}


bool SailDlg::InitDialog(Sail *pSail)
{
    if(!pSail) return false;

    m_pSail = pSail;
    m_pSectionWidget->m_pSailDlg = this;
    m_pSectionWidget->m_pSail = pSail;
    m_pSailWidget->InitDialog(pSail);

    QString str;
    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
    GetLengthUnit(str, pMainFrame->m_LengthUnit);

    //Set Frame Table
    m_pSectionModel = new QStandardItemModel;
    m_pSectionModel->setRowCount(10);//temporary
    if(m_pSail->IsNURBSSail())            m_pSectionModel->setColumnCount(3);
    else if(m_pSail->IsSailcutSail())    m_pSectionModel->setColumnCount(6);
    m_pSectionModel->setHeaderData(0, Qt::Horizontal, tr("z (")+str+")");
    m_pSectionModel->setHeaderData(1, Qt::Horizontal, tr("chord (")+str+")");
    m_pSectionModel->setHeaderData(2, Qt::Horizontal, tr("angle")+QString::fromUtf8("(°)"));

    if(m_pSail->IsNURBSSail())
    {
    }
    else if(m_pSail->IsSailcutSail())
    {
        m_pSectionModel->setHeaderData(3, Qt::Horizontal, "Depth"+QString::fromUtf8("(%)"));
        m_pSectionModel->setHeaderData(4, Qt::Horizontal, "AV");
        m_pSectionModel->setHeaderData(5, Qt::Horizontal, "AR");
    }

    m_pctrlSectionTable->setModel(m_pSectionModel);
    QItemSelectionModel *selSectionModel = new QItemSelectionModel(m_pSectionModel);
    m_pctrlSectionTable->setSelectionModel(selSectionModel);
    m_pSectionDelegate = new FloatEditDelegate(this);
    m_pctrlSectionTable->setItemDelegate(m_pSectionDelegate);
    QHeaderView *pHorizontalHeader = m_pctrlSectionTable->horizontalHeader();
    pHorizontalHeader->setStretchLastSection(true);

    int  *pSectionPrecision = new int[6];
    pSectionPrecision[0] = 3;
    pSectionPrecision[1] = 3;
    pSectionPrecision[2] = 1;
    pSectionPrecision[3] = 1;
    pSectionPrecision[4] = 3;
    pSectionPrecision[5] = 3;
    m_pSectionDelegate->SetPrecision(pSectionPrecision);
    connect(selSectionModel, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(OnSectionItemClicked(QModelIndex)));
    connect(m_pSectionDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnSectionCellChanged(QWidget *)));


    //Set Point Table
    m_pPointModel = new QStandardItemModel(this);
    m_pPointModel->setRowCount(10);//temporary
    m_pPointModel->setColumnCount(3);
    m_pPointModel->setHeaderData(0, Qt::Horizontal, "x ("+str+")");
    m_pPointModel->setHeaderData(1, Qt::Horizontal, "y ("+str+")");
    m_pPointModel->setHeaderData(2, Qt::Horizontal, "z ("+str+")");
    m_pctrlPointTable->setModel(m_pPointModel);
    QItemSelectionModel *selPointModel = new QItemSelectionModel(m_pPointModel);
    m_pctrlPointTable->setSelectionModel(selPointModel);
    pHorizontalHeader = m_pctrlPointTable->horizontalHeader();
    pHorizontalHeader->setStretchLastSection(true);

    m_pPointDelegate = new FloatEditDelegate;
    m_pctrlPointTable->setItemDelegate(m_pPointDelegate);
    int  *pPointPrecision = new int[3];
    pPointPrecision[0] = 3;
    pPointPrecision[1] = 3;
    pPointPrecision[2] = 3;
    m_pPointDelegate->SetPrecision(pPointPrecision);
    connect(selPointModel, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(OnPointItemClicked(QModelIndex)));
    connect(m_pPointDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnPointCellChanged(QWidget *)));

    m_pctrlNURBSBox->setVisible(m_pSail->IsNURBSSail());
    m_pctrlSectionCommands->setVisible(m_pSail->IsNURBSSail());

    SetSailData(pSail);
    QModelIndex index = m_pSectionModel->index(m_iSection, 0, QModelIndex());
    m_pctrlSectionTable->setCurrentIndex(index);


    UpdateSailDataOutput();
    SetControls();
    return true;
}


void SailDlg::SetSailData(Sail* )
{
    m_pctrlSailName->setText(m_pSail->m_SailName);
    m_pctrlSailColor->SetColor(m_pSail->m_SailColor);

    if(m_pSail->IsNURBSSail())
    {
        setWindowTitle("NURBS type sail");
        m_pctrlStackedSailWidget->setCurrentIndex(0);
        m_pctrlSectionTable->hideColumn(1);
        NURBSSail *pNSail = dynamic_cast<NURBSSail*>(m_pSail);
        m_pctrlAlignedLE->setChecked(pNSail->m_bAlignedLE);
    }
    else if(m_pSail->IsSailcutSail())
    {
        setWindowTitle("Sailcut type sail");
        SailcutSail *pSCSail = (SailcutSail*)m_pSail;
        m_pctrlStackedSailWidget->setCurrentIndex(1);

        m_pctrlLeechRound->SetValue(pSCSail->m_LeechRound);
        m_pctrlLeechRoundPos->SetValue(pSCSail->m_LeechRoundPos*100.0);

        m_pctrlGaffTwist->SetValue(pSCSail->m_Twist);
    }

    m_pctrlNXPanels->SetValue(m_pSail->m_NXPanels);
    m_pctrlNZPanels->SetValue(m_pSail->m_NZPanels);

    m_pctrlLuffAngle->SetValue(m_pSail->m_LuffAngle);

    m_iSection = 0;
    FillSectionTable();
    FillPointTable();
    m_pSectionWidget->SetSection(m_pSail->m_oaSection.at(m_iSection));
}


void SailDlg::SetControls()
{
    QString str;
    str = tr("Insert after section") +" %1";
    str = QString(str).arg(m_iSection+1);
    m_pctrlInsertAfter->setText(str);

    str = tr("Insert before section") +" %1";
    str = QString(str).arg(m_iSection+1);
    m_pctrlInsertBefore->setText(str);

    str = tr("Delete section") +" %1";
    str = QString(str).arg(m_iSection+1);
    m_pctrlDeleteSection->setText(str);

    m_pctrlInsertBefore->setEnabled(m_pSail && m_pSail->m_SailType==NURBSSAIL);
    m_pctrlInsertAfter->setEnabled(m_pSail && m_pSail->m_SailType==NURBSSAIL);
    m_pctrlDeleteSection->setEnabled(m_pSail && m_pSail->m_SailType==NURBSSAIL);

    //    m_pctrlSectionTable->selectRow(m_iSection);

    if(m_pSail->IsNURBSSail())
    {
        NURBSSail * pNSail = dynamic_cast<NURBSSail*>(m_pSail);
        m_pctrlNXDegree->SetValue(pNSail->m_SplineSurface.m_ivDegree);
        m_pctrlNZDegree->SetValue(pNSail->m_SplineSurface.m_iuDegree);
        QString str = QString("(<%1)").arg(pNSail->m_SplineSurface.FramePointCount());
        m_pctrlNXDegMin->setText(str);
        str = QString("(<%1)").arg(pNSail->m_SplineSurface.m_pFrame.size());
        m_pctrlNZDegMin->setText(str);

        m_pctrlEdgeWeightu->SetValue(pNSail->m_SplineSurface.m_EdgeWeightu);
        m_pctrlEdgeWeightv->SetValue(pNSail->m_SplineSurface.m_EdgeWeightv);
    }
}


void SailDlg::SetupLayout()
{
    QString strLength;
    GetLengthUnit(strLength, s_pMainFrame->m_LengthUnit);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QFrame *pRightFrame = new QFrame;
    {
        QVBoxLayout *pRightLayout = new QVBoxLayout;
        {
            QTabWidget *pTabWidget = new QTabWidget(this);
            {
                //            pTabWidget->setSizePolicy(szPolicyPreferred);
                QWidget *pGeneralPage   = new QWidget(this);
                QWidget *pSectionPage   = new QWidget(this);
                pTabWidget->addTab(pGeneralPage, tr("Sail Description"));
                pTabWidget->addTab(pSectionPage, tr("Sections"));
                pTabWidget->setCurrentWidget(pSectionPage);

                QVBoxLayout *pGeneralLayout = new QVBoxLayout;
                {
                    QHBoxLayout *pSailInfoLayout = new QHBoxLayout;
                    {
                        m_pctrlSailName = new QLineEdit;
                        m_pctrlSailColor = new ColorButton;
                        //                    m_pctrlSailName->setSizePolicy(szPolicyIgnored);
                        //                    m_pctrlSailColor->setSizePolicy(szPolicyMaximum);
                        pSailInfoLayout->addWidget(m_pctrlSailName);
                        pSailInfoLayout->addWidget(m_pctrlSailColor);
                    }
                    m_pctrlStackedSailWidget = new QStackedWidget(this);
                    {
                        QGroupBox *pNURBSSailWidget = new QGroupBox("NURBS parameters");
                        {
                            QGridLayout *NURBSDataLayout = new QGridLayout;
                            {
                                QLabel *labNote = new QLabel(tr("The degree must be strictly less than the number of control points"));
                                NURBSDataLayout->addWidget(labNote, 1,1,1,3);
                                m_pctrlNXDegMin = new QLabel("(>=2)");
                                m_pctrlNZDegMin = new QLabel("(>=2)");
                                m_pctrlNXDegree = new FloatEdit(2, 0);
                                m_pctrlNZDegree = new FloatEdit(2, 0);
                                m_pctrlEdgeWeightu = new FloatEdit(1.0, 1);
                                m_pctrlEdgeWeightv = new FloatEdit(1.0, 1);
                                m_pctrlAlignedLE = new QCheckBox("Align Luff points");
                                QLabel *labNXdeg = new QLabel(tr("Spline degree in x direction="));
                                QLabel *labNZdeg = new QLabel(tr("Spline degree in z direction="));
                                QLabel *LabEdgeWeightu = new QLabel(tr("Luff and Leech Edge Weight ="));
                                QLabel *LabEdgeWeightv = new QLabel(tr("Foot and Gaff Edge Weight ="));
                                NURBSDataLayout->addWidget(labNXdeg,2, 1);
                                NURBSDataLayout->addWidget(m_pctrlNXDegree,2, 2);
                                NURBSDataLayout->addWidget(labNZdeg,3, 1);
                                NURBSDataLayout->addWidget(m_pctrlNZDegree,3, 2);
                                NURBSDataLayout->addWidget(m_pctrlNXDegMin,2, 3);
                                NURBSDataLayout->addWidget(m_pctrlNZDegMin,3, 3);
                                NURBSDataLayout->addWidget(LabEdgeWeightu, 5, 1, Qt::AlignRight| Qt::AlignVCenter);
                                NURBSDataLayout->addWidget(m_pctrlEdgeWeightu, 5, 2, Qt::AlignLeft | Qt::AlignVCenter);
                                NURBSDataLayout->addWidget(LabEdgeWeightv, 6, 1, Qt::AlignRight| Qt::AlignVCenter);
                                NURBSDataLayout->addWidget(m_pctrlEdgeWeightv, 6, 2, Qt::AlignLeft | Qt::AlignVCenter);
                                NURBSDataLayout->addWidget(m_pctrlAlignedLE, 8,1);
                                NURBSDataLayout->setColumnStretch(4,1);
                                NURBSDataLayout->setRowStretch(4,1);
                                NURBSDataLayout->setRowStretch(7,1);
                                NURBSDataLayout->setRowStretch(9,2);
                                pNURBSSailWidget->setLayout(NURBSDataLayout);
                            }
                        }
                        QGroupBox *pSailcutSailWidget = new QGroupBox("Sailcut parameters");
                        {
                            QGridLayout *pSailShapeLayout = new QGridLayout;
                            {
                                m_pctrlLuffAngle     = new FloatEdit(0,1);
                                m_pctrlGaffTwist     = new FloatEdit(0.0,1);
                                m_pctrlLeechRound    = new FloatEdit(0.0,2);
                                m_pctrlLeechRoundPos = new FloatEdit(40,1);
                                QLabel *labLuff = new QLabel("Luff angle =");
                                QLabel *labdeg1 = new QLabel(QString::fromUtf8("°"));
                                QLabel *labdeg2 = new QLabel(QString::fromUtf8("°"));
                                QLabel *pLabTwistValue = new QLabel("Gaff Twist =");
                                QLabel *labLeechRound = new QLabel("Leech Round =");
                                QLabel *labLeechRoundPos = new QLabel("Leech Round Position =");
                                QLabel *LengthUnit = new QLabel(strLength);
                                QLabel *percentUnit = new QLabel(QString::fromUtf8("%"));


                                pSailShapeLayout->addWidget(labLuff,1,1, Qt::AlignRight|Qt::AlignVCenter);
                                pSailShapeLayout->addWidget(m_pctrlLuffAngle,1,2);
                                pSailShapeLayout->addWidget(labdeg1,1,3);
                                pSailShapeLayout->addWidget(pLabTwistValue,3,1, Qt::AlignRight|Qt::AlignVCenter);
                                pSailShapeLayout->addWidget(m_pctrlGaffTwist,3,2);
                                pSailShapeLayout->addWidget(labdeg2,3,3);
                                pSailShapeLayout->addWidget(labLeechRound,5,1, Qt::AlignRight|Qt::AlignVCenter);
                                pSailShapeLayout->addWidget(m_pctrlLeechRound,5,2);
                                pSailShapeLayout->addWidget(LengthUnit,5,3);
                                pSailShapeLayout->addWidget(labLeechRoundPos,6,1, Qt::AlignRight|Qt::AlignVCenter);
                                pSailShapeLayout->addWidget(m_pctrlLeechRoundPos,6,2);
                                pSailShapeLayout->addWidget(percentUnit,6,3);
                                pSailShapeLayout->setRowStretch(2, 1);
                                pSailShapeLayout->setRowStretch(4, 1);
                                pSailShapeLayout->setColumnStretch(4, 3);
                            }

                            pSailcutSailWidget->setLayout(pSailShapeLayout);
                        }

                        m_pctrlStackedSailWidget->addWidget(pNURBSSailWidget);
                        m_pctrlStackedSailWidget->addWidget(pSailcutSailWidget);
                    }
                    QGroupBox *pSailDataBox = new QGroupBox("Calculated values");
                    {
                        QVBoxLayout *pSailDataLayout = new QVBoxLayout;
                        {
                            m_pctrlLeechLength = new QLabel("Leech Length = 10 m");
                            m_pctrlLuffLength = new QLabel("Luff Length = 9.7654 m");
                            m_pctrlFootLength = new QLabel("Foot Length = 5.675m");
                            pSailDataLayout->addWidget(m_pctrlLuffLength);
                            pSailDataLayout->addWidget(m_pctrlLeechLength);
                            pSailDataLayout->addWidget(m_pctrlFootLength);
                        }
                        pSailDataBox->setLayout(pSailDataLayout);
                    }

                    QGroupBox *pMeshBox = new QGroupBox("Mesh data");
                    {
                        QGridLayout *pMeshDataLayout = new QGridLayout;
                        {
                            m_pctrlNXPanels = new FloatEdit(17,0);
                            m_pctrlNZPanels = new FloatEdit(17,0);
                            QLabel *labNX = new QLabel("Number of X panels=");
                            QLabel *labNZ = new QLabel("Number of Z panels=");
                            pMeshDataLayout->addWidget(labNX,2,2);
                            pMeshDataLayout->addWidget(m_pctrlNXPanels,2,3);
                            pMeshDataLayout->addWidget(labNZ,3,2);
                            pMeshDataLayout->addWidget(m_pctrlNZPanels,3,3);
                            pMeshDataLayout->setColumnStretch(4,1);
                            pMeshDataLayout->setRowStretch(1,1);
                            pMeshDataLayout->setRowStretch(4,1);
                            pMeshBox->setLayout(pMeshDataLayout);
                        }
                    }

                    pGeneralLayout->addLayout(pSailInfoLayout);
                    pGeneralLayout->addStretch();
                    pGeneralLayout->addWidget(m_pctrlStackedSailWidget);
                    pGeneralLayout->addStretch();
                    pGeneralLayout->addWidget(pMeshBox);
                    pGeneralLayout->addStretch();
                    pGeneralLayout->addWidget(pSailDataBox);
                    pGeneralLayout->addStretch();

                    pGeneralPage->setLayout(pGeneralLayout);
                }

                QVBoxLayout *pSectionLayout = new QVBoxLayout;
                {
                    QToolBar *ActionButtons = new QToolBar(this);
                    {
                        m_pZoomIn= new QAction(QIcon(":/images/OnZoomIn.png"), tr("Zoom in"), this);
                        m_pZoomIn->setCheckable(true);
                        m_pZoomIn->setStatusTip(tr("Zoom the view by drawing a rectangle in the client area"));

                        m_pZoomOut= new QAction(QIcon(":/images/OnZoomLess.png"), tr("Zoom Less"), this);
                        m_pZoomOut->setStatusTip(tr("Zoom Less"));

                        m_pZoomYOnly= new QAction(QIcon(":/images/OnZoomYScale.png"), tr("Zoom Y Scale Only"), this);
                        m_pZoomYOnly->setCheckable(true);
                        m_pZoomYOnly->setStatusTip(tr("Zoom Y scale Only"));

                        m_pResetXScale = new QAction(QIcon(":/images/OnResetXScale.png"), tr("Reset X Scale"), this);
                        m_pResetXScale->setStatusTip(tr("Resets the scale to fit the current screen width"));

                        m_pResetYScale = new QAction(QIcon(":/images/OnResetYScale.png"), tr("Reset Y Scale"), this);
                        m_pResetYScale->setStatusTip(tr("Resets the Y-scale to unity"));

                        m_pResetScales= new QAction(QIcon(":/images/OnResetXYScale.png"), tr("Reset Scales")+"\t(R)", this);
                        m_pResetScales->setStatusTip(tr("Resets the x and y scales to screen size"));

                        m_pUndo = new QAction(QIcon(":/images/OnUndo.png"), tr("Undo"), this);
                        m_pUndo->setShortcut(Qt::CTRL + Qt::Key_Z);
                        m_pUndo->setStatusTip(tr("Cancels the last modifiction made to the splines"));

                        m_pRedo = new QAction(QIcon(":/images/OnRedo.png"), tr("Redo"), this);
                        m_pRedo->setShortcut(Qt::CTRL + Qt::Key_Y);
                        m_pRedo->setStatusTip(tr("Restores the last cancelled modifiction made to the splines"));


                        ActionButtons->addAction(m_pZoomIn);
                        ActionButtons->addAction(m_pZoomOut);
                        ActionButtons->addSeparator();
                        ActionButtons->addAction(m_pResetXScale);
                        ActionButtons->addAction(m_pResetScales);
                        ActionButtons->addAction(m_pZoomYOnly);
                        ActionButtons->addAction(m_pResetYScale);
                        ActionButtons->addSeparator();
                        ActionButtons->addAction(m_pUndo);
                        ActionButtons->addAction(m_pRedo);
                    }

                    m_pSectionWidget = new SectionViewWidget;
                    m_pSectionWidget->setMinimumHeight(200);
                    m_pSectionWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

                    QHBoxLayout *pTableLayout = new QHBoxLayout;
                    {
                        QGroupBox *pSectionBox = new QGroupBox(tr("Section Data"));
                        {
                            //lays out the section table and the control point tabe
                            QHBoxLayout *SectionDataLayout = new QHBoxLayout;
                            {
                                m_pctrlSectionTable = new QTableView(this);
                                m_pctrlSectionTable->installEventFilter(this);

                                m_pctrlSectionTable->setWindowTitle(QObject::tr("Sail definition"));
                                //                            m_pctrlSectionTable->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
                                m_pctrlSectionTable->setSelectionMode(QAbstractItemView::SingleSelection);
                                m_pctrlSectionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
                                m_pctrlSectionTable->setEditTriggers(QAbstractItemView::CurrentChanged |
                                                                     QAbstractItemView::DoubleClicked |
                                                                     QAbstractItemView::SelectedClicked |
                                                                     QAbstractItemView::AnyKeyPressed |
                                                                     QAbstractItemView::EditKeyPressed );
                                m_pctrlSectionTable->horizontalHeader()->setStretchLastSection(true);


                                m_pctrlSectionCommands = new QWidget;
                                {
                                    QVBoxLayout *ActionLayout = new QVBoxLayout;
                                    {
                                        m_pctrlInsertBefore  = new QPushButton("Insert Before");
                                        m_pctrlInsertAfter   = new QPushButton("Insert After");
                                        m_pctrlDeleteSection = new QPushButton("Delete Section");
                                        ActionLayout->addWidget(m_pctrlInsertBefore);
                                        ActionLayout->addWidget(m_pctrlInsertAfter);
                                        ActionLayout->addWidget(m_pctrlDeleteSection);
                                        ActionLayout->addStretch();
                                    }
                                    m_pctrlSectionCommands->setLayout(ActionLayout);
                                }
                                SectionDataLayout->addWidget(m_pctrlSectionTable);
                                SectionDataLayout->addWidget(m_pctrlSectionCommands);
                                pSectionBox->setLayout(SectionDataLayout);
                            }
                        }

                        //lays out the control point table
                        m_pctrlNURBSBox = new QGroupBox(tr("NURBS Points coordinates"));
                        {
                            QVBoxLayout *PointDataLayout = new QVBoxLayout;
                            {
                                m_pctrlPointTable = new QTableView(this);
                                m_pctrlPointTable->installEventFilter(this);
                                m_pctrlPointTable->setWindowTitle(QObject::tr("Point coordinates"));
                                m_pctrlPointTable->setSelectionMode(QAbstractItemView::SingleSelection);
                                m_pctrlPointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
                                m_pctrlPointTable->setEditTriggers(QAbstractItemView::CurrentChanged |
                                                                   QAbstractItemView::DoubleClicked |
                                                                   QAbstractItemView::SelectedClicked |
                                                                   QAbstractItemView::EditKeyPressed |
                                                                   QAbstractItemView::AnyKeyPressed);
                                m_pctrlPointTable->horizontalHeader()->setStretchLastSection(true);

                                PointDataLayout->addWidget(m_pctrlPointTable);
                                m_pctrlNURBSBox->setLayout(PointDataLayout);
                            }
                        }

                        pTableLayout->addWidget(pSectionBox);
                        pTableLayout->addWidget(m_pctrlNURBSBox);
                    }

                    pSectionLayout->addWidget(ActionButtons);
                    pSectionLayout->addWidget(m_pSectionWidget);
                    pSectionLayout->addLayout(pTableLayout);
                    pSectionPage->setLayout(pSectionLayout);
                }
            }

            QHBoxLayout *pCommandButtons = new QHBoxLayout;
            {
                m_pctrlScaleSail = new QPushButton(tr("Scale Sail"));
                pCommandButtons->addWidget(m_pctrlScaleSail);
                m_pctrlImportSail = new QPushButton(tr("Import Sail"));
                pCommandButtons->addWidget(m_pctrlImportSail);
                m_pctrlExportSail = new QPushButton(tr("Export Sail"));
                pCommandButtons->addWidget(m_pctrlExportSail);

                m_pctrlOK = new QPushButton(tr("Save and Close"));
                m_pctrlOK->setAutoDefault(true);
                m_pctrlCancel = new QPushButton(tr("Cancel"));
                m_pctrlCancel->setAutoDefault(false);

                pCommandButtons->addStretch();
                pCommandButtons->addWidget(m_pctrlOK);
                pCommandButtons->addWidget(m_pctrlCancel);
            }
            pRightLayout->addWidget(pTabWidget);
            pRightLayout->addLayout(pCommandButtons);
        }
        pRightFrame->setLayout(pRightLayout);
    }
    QHBoxLayout *pMainLayout = new QHBoxLayout;
    {
        m_pSailWidget = new SailViewWt;

        QSplitter *pMainSplitter = new QSplitter;
        pMainSplitter->addWidget(m_pSailWidget);
        pMainSplitter->addWidget(pRightFrame);

        pMainLayout->addWidget(pMainSplitter);
    }

    setLayout(pMainLayout);
}


void SailDlg::OnOK()
{
    ReadData();
    m_pSail->SplineSurface();
    accept();
}


void SailDlg::OnCancel()
{
    if(m_bChanged)
    {
        int res = QMessageBox::question(this, tr("Sail Dlg Exit"), tr("Save the changes ?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (QMessageBox::No == res)
        {
            QDialog::reject();
        }
        else if (QMessageBox::Cancel == res) return;
        else
        {
            done(QDialog::Accepted);
            return;
        }
    }
    reject();
}


void SailDlg::OnSailColor()
{
    if(!m_pSail) return;

    QColor SailColor = QColorDialog::getColor(m_pSail->m_SailColor,
                                              this, "Select the Sail color", QColorDialog::ShowAlphaChannel);
    if(SailColor.isValid()) m_pSail->m_SailColor = SailColor;

    m_pctrlSailColor->SetColor(m_pSail->m_SailColor);
    UpdateView();
}


void SailDlg::keyPressEvent(QKeyEvent *event)
{
    bool bShift = false;
    bool bCtrl  = false;
    if(event->modifiers() & Qt::ShiftModifier)   bShift =true;
    if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

    switch (event->key())
    {
        case Qt::Key_Return:
        {
            if(!m_pctrlOK->hasFocus() && !m_pctrlCancel->hasFocus()) m_pctrlOK->setFocus();
            else if(m_pctrlOK->hasFocus()) OnOK();
            else if(m_pctrlCancel->hasFocus()) reject();
            break;
        }
        case Qt::Key_Escape:
        {
            OnCancel();
            return;
        }
            //        default:
            //            event->ignore();
    }
}


void SailDlg::closeEvent(QCloseEvent *)
{    
    s_WindowPos = pos();
    s_WindowSize = size();
    s_bWindowMaximized = isMaximized();
}


void SailDlg::showEvent(QShowEvent *)
{
    m_pSailWidget->Set3DScale();
    //    UpdateView();
}


void SailDlg::resizeEvent(QResizeEvent *)
{
    int wc    = (int)((double)m_pctrlSectionTable->width() *0.9/ (double)m_pSectionModel->columnCount());
    for(int ic=0; ic<m_pSectionModel->columnCount(); ic++)
        m_pctrlSectionTable->setColumnWidth(ic, wc);


    int w2  = (int)((double)m_pctrlPointTable->width()*.9 / 2.);
    m_pctrlPointTable->setColumnWidth(0, w2);
    m_pctrlPointTable->setColumnWidth(1, w2);

    //    UpdateView(); //"No drawing need be (or should be) done inside this handler."
}



void SailDlg::OnTwistChanged()
{
    if(!m_pSail->IsSailcutSail()) return;
    SailcutSail *pSCSail = (SailcutSail*)m_pSail;
    pSCSail->m_Twist = m_pctrlGaffTwist->Value();
    m_bChanged = true;
    UpdateView();
}


void SailDlg::FillPointTable()
{
    SailSection *pSection = m_pSail->m_oaSection.at(m_iSection);

    if(!pSection || m_iSection<0 || m_iSection>=m_pSail->m_oaSection.size()) return;

    QModelIndex ind;

    m_pPointModel->setRowCount(pSection->m_CtrlPoint.count());

    for(int ip=0; ip<pSection->m_CtrlPoint.count(); ip++)
    {
        ind = m_pPointModel->index(ip, 0, QModelIndex());
        m_pPointModel->setData(ind, pSection->m_CtrlPoint[ip].x);

        ind = m_pPointModel->index(ip, 1, QModelIndex());
        m_pPointModel->setData(ind, pSection->m_CtrlPoint[ip].y);

        ind = m_pPointModel->index(ip, 2, QModelIndex());
        m_pPointModel->setData(ind, pSection->m_CtrlPoint[ip].z);
    }
}



void SailDlg::FillSectionTable()
{
    if(!m_pSail) return;
    SailcutSail *pSCSail = (SailcutSail*)m_pSail;

    QModelIndex ind;
    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

    m_pSectionModel->setRowCount(m_pSail->m_oaSection.size());

    for(int is=0; is<m_pSail->m_oaSection.size(); is++)
    {
        SailSection *pSection = m_pSail->m_oaSection.at(is);

        ind = m_pSectionModel->index(is, 0, QModelIndex());
        m_pSectionModel->setData(ind, pSection->m_Position.z * pMainFrame->m_mtoUnit);

        ind = m_pSectionModel->index(is, 1, QModelIndex());
        if(m_pSail->IsSailcutSail())  m_pSectionModel->setData(ind, pSCSail->Chord(pSection) * pMainFrame->m_mtoUnit);
        else                          m_pSectionModel->setData(ind, 1.0);//no matter, won't be displayed

        ind = m_pSectionModel->index(is, 2, QModelIndex());
        m_pSectionModel->setData(ind, pSection->m_Angle);

        if(m_pSail->IsSailcutSail())
        {
            ind = m_pSectionModel->index(is, 3, QModelIndex());
            m_pSectionModel->setData(ind, pSection->m_SCSpline.Depth()*100.0);

            ind = m_pSectionModel->index(is, 4, QModelIndex());
            m_pSectionModel->setData(ind, pSection->m_SCSpline.AV);

            ind = m_pSectionModel->index(is, 5, QModelIndex());
            m_pSectionModel->setData(ind, pSection->m_SCSpline.AR);
        }
    }
}



void SailDlg::ReadData()
{
    if(!m_pSail) return;
    m_pSail->m_SailName = m_pctrlSailName->text();

    m_pSail->m_NXPanels = m_pctrlNXPanels->Value();
    m_pSail->m_NZPanels = m_pctrlNZPanels->Value();


    if(m_pSail->IsSailcutSail())
    {
        SailcutSail *pSCSail = (SailcutSail*)m_pSail;
        pSCSail->m_LuffAngle = m_pctrlLuffAngle->Value();
        pSCSail->m_Twist = m_pctrlGaffTwist->Value();
        pSCSail->m_LeechRound = m_pctrlLeechRound->Value();
        pSCSail->m_LeechRoundPos = m_pctrlLeechRoundPos->Value()/100.0;
        pSCSail->SetLeechRoundEquation();

        ReadSectionData();
        pSCSail->SplineSurface();
    }
    else
    {
        NURBSSail *pNSail =dynamic_cast<NURBSSail*>(m_pSail);
        Vector3d LE = pNSail->m_SplineSurface.LeadingEdgeAxis();
        pNSail->m_LuffAngle = atan2(LE.x, LE.z) * 180./PI;
    }
}


void SailDlg::OnUpdate()
{
    m_bChanged = true;

    m_pSail->m_NXPanels = m_pctrlNXPanels->Value();
    m_pSail->m_NZPanels = m_pctrlNZPanels->Value();

    if(m_pSail->IsNURBSSail())
    {
        NURBSSail *pNSail = dynamic_cast<NURBSSail*>(m_pSail);
        pNSail->m_SplineSurface.SetvDegree((int)m_pctrlNXDegree->Value());
        //the degree may have been adjusted, so set the returned value
        m_pctrlNXDegree->SetValue(pNSail->m_SplineSurface.m_ivDegree);

        pNSail->m_SplineSurface.SetuDegree((int)m_pctrlNZDegree->Value());
        //the degree may have been adjusted, so set the returned value
        m_pctrlNZDegree->SetValue(pNSail->m_SplineSurface.m_iuDegree);

        pNSail->m_SplineSurface.m_EdgeWeightu = m_pctrlEdgeWeightu->Value();
        pNSail->m_SplineSurface.m_EdgeWeightv = m_pctrlEdgeWeightv->Value();
    }
    else if(m_pSail->IsSailcutSail())
    {
        SailcutSail *pSCSail = (SailcutSail*)m_pSail;
        pSCSail->SetLuffAngle(m_pctrlLuffAngle->Value());
    }

    UpdateSailDataOutput();
    UpdateView();
}



void SailDlg::ReadSectionData()
{
    if(!m_pSail) return;
    double d;
    bool bOK;

    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
    QString strong;
    QStandardItem *pItem;

    for(int is=0; is<m_pSectionModel->rowCount(); is++)
    {
        SailSection *pSection = m_pSail->m_oaSection.at(is);

        pItem = m_pSectionModel->item(is,0);
        strong = pItem->text();
        strong.replace(" ","");
        d = strong.toDouble(&bOK);
        if(bOK) pSection->SetSpanPosition(d / pMainFrame->m_mtoUnit);

        pItem = m_pSectionModel->item(is,1);
        strong = pItem->text();
        strong.replace(" ","");
        d = strong.toDouble(&bOK);
        if(bOK) pSection->m_Chord = d / pMainFrame->m_mtoUnit;

        pItem = m_pSectionModel->item(is,2);
        strong = pItem->text();
        strong.replace(" ","");
        d = strong.toDouble(&bOK);
        if(bOK)
        {
            if(fabs(pSection->m_Angle-d)>PRECISION)
            {
                pSection->RotateFrameY(d-pSection->m_Angle);
                pSection->m_Angle = d;
            }
        }

        if(m_pSail->IsSailcutSail())
        {
            pItem = m_pSectionModel->item(is,3);
            strong =pItem->text();
            strong.replace(" ","");
            d = strong.toDouble(&bOK)/100.0;
            if(bOK) pSection->m_SCSpline.DepthtoK(d);

            pItem = m_pSectionModel->item(is,4);
            strong =pItem->text();
            strong.replace(" ","");
            d = strong.toDouble(&bOK);
            if(bOK) pSection->m_SCSpline.AV = d;

            pItem = m_pSectionModel->item(is,5);
            strong =pItem->text();
            strong.replace(" ","");
            d = strong.toDouble(&bOK);
            if(bOK) pSection->m_SCSpline.AR = d;
        }
    }
}



void SailDlg::OnPointCellChanged(QWidget *)
{
    m_bChanged = true;
    ReadPointData();
    m_pSail->SplineSurface();
    UpdateSailDataOutput();

    UpdateView();
}



void SailDlg::OnSectionCellChanged(QWidget *)
{
    m_bChanged = true;
    ReadSectionData();

    FillPointTable();//in case an angle has been set

    m_pSail->SplineSurface();
    UpdateSailDataOutput();

    UpdateView();
}


void SailDlg::OnSectionItemClicked(const QModelIndex &index)
{
    m_iSection = index.row();

    m_pSectionWidget->SetSection(m_pSail->m_oaSection.at(m_iSection));
    if(m_pSail->IsNURBSSail())
    {
        FillPointTable();
    }
    else
    {
        //        FillSectionTable();
    }

    SetControls();

    m_pSailWidget->m_bResetglSectionHighlight = true;
    m_pSailWidget->m_bResetglCtrlPoints       = true;
    UpdateView();
}


void SailDlg::OnPointItemClicked(const QModelIndex &index)
{
    int iCtrlPt = index.row();
    m_pSail->m_oaSection.at(m_iSection)->m_iSelect = iCtrlPt;
    m_pSectionWidget->UpdateView();
    m_pSailWidget->m_bResetglCtrlPoints = true;
    m_pSailWidget->UpdateView();
}



void SailDlg::UpdateView()
{
    if(isVisible())
    {
        m_pSectionWidget->UpdateView();
        UpdateSailView();
    }
}


void SailDlg::UpdateSailView()
{
    m_pSailWidget->m_bResetglGeom = true;
    m_pSailWidget->m_bResetglSailMesh = true;
    m_pSailWidget->UpdateView();
}


void SailDlg::OnInsertBefore()
{
    if(!m_pSail || m_iSection<0 || m_iSection>=m_pSail->m_oaSection.size() || m_pSail->IsSailcutSail()) return;

    m_pSail->CreateSection(m_iSection);

    FillSectionTable();
    m_pSectionWidget->SetSection(m_pSail->m_oaSection.at(m_iSection));

    m_bChanged = true;
    SetControls();
    UpdateView();
}


void SailDlg::OnDeleteSection()
{
    if(!m_pSail || m_iSection<0 || m_iSection>=m_pSail->m_oaSection.size() || m_pSail->IsSailcutSail()) return;

    if(m_pSail->m_oaSection.size()<=2)
    {
        QString strong = tr("At least two sections are required to define the sail");
        QMessageBox::warning(window(), tr("Warning"), strong);
        return;
    }

    m_pSail->DeleteSection(m_iSection);

    FillSectionTable();

    if(m_iSection>=m_pSail->m_oaSection.size()) m_iSection--;
    if(m_iSection<0) m_iSection=0;

    m_pSectionWidget->SetSection(m_pSail->m_oaSection.at(m_iSection));

    if(m_pSail->IsNURBSSail())
    {
        //Check the degree
        NURBSSail *pNSail = dynamic_cast<NURBSSail*>(m_pSail);
        int oldDegree = pNSail->m_SplineSurface.m_iuDegree;
        int newDegree = pNSail->m_SplineSurface.SetuDegree((int)m_pctrlNZDegree->Value());
        pNSail->SplineSurface();
        UpdateView();

        if(oldDegree != newDegree)
        {
            //the degree has been adjusted, so set the returned value
            m_pctrlNZDegree->SetValue(pNSail->m_SplineSurface.m_iuDegree);
            QString strong = tr("The degree has been reduced to be less than the number of sections");
            QMessageBox::warning(window(), tr("Warning"), strong);
        }
        m_bChanged = true;
        SetControls();
        return;
    }

    m_bChanged = true;
    SetControls();
    UpdateView();
}


void SailDlg::OnInsertAfter()
{
    if(!m_pSail) return;

    m_pSail->CreateSection(m_iSection+1);
    m_iSection++;
    FillSectionTable();
    m_pctrlSectionTable->selectRow(m_iSection);
    m_pSectionWidget->SetSection(m_pSail->m_oaSection.at(m_iSection));
    m_bChanged = true;
    SetControls();
    UpdateView();
}



void SailDlg::OnScaleSail()
{
    if(!m_pSail) return;
    ScaleSailDlg dlg(this);
    dlg.InitDialog(m_pSail);
    if(dlg.exec()==QDialog::Accepted)
    {
        m_bChanged = true;
        m_pSail->ScaleSail(dlg.m_XFactor, dlg.m_YFactor, dlg.m_ZFactor);
        if(m_pSail->IsSailcutSail()) m_pSail->SetLuffAngle();
        m_pSail->SplineSurface();
        FillSectionTable();
        UpdateView();
    }
}



void SailDlg::UpdateSailDataOutput()
{
    QString strLength;
    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

    //Update Sail Geometry
    //    m_pSail->SplineSurface();

    //Update Sail Data
    GetLengthUnit(strLength, pMainFrame->m_LengthUnit);

    QString result = QString("Leech Length = %1").arg(m_pSail->LeechLength()*pMainFrame->m_mtoUnit,8,'f',3)    ;
    result += strLength;
    m_pctrlLeechLength->setText(result);

    result = QString("Luff Length = %1").arg(m_pSail->LuffLength()*pMainFrame->m_mtoUnit,8, 'f',3)    ;
    result += strLength;
    m_pctrlLuffLength->setText(result);

    result = QString("Foot Length = %1").arg(m_pSail->FootLength()*pMainFrame->m_mtoUnit,8, 'f',3)    ;
    result += strLength;
    m_pctrlFootLength->setText(result);
}


void SailDlg::LoadSettings(QSettings *pSettings)
{
    pSettings->beginGroup("SailDlg");
    {
        s_WindowPos        = pSettings->value("SailDlgPos", QPoint(20,40)).toPoint();
        s_WindowSize       = pSettings->value("SailDlgSize", QSize(450,400)).toSize();
        s_bWindowMaximized = pSettings->value("SailDlgMaximized", false).toBool();
    }
    pSettings->endGroup();

}


void SailDlg::SaveSettings(QSettings *pSettings)
{
    pSettings->beginGroup("SailDlg");
    {
        pSettings->setValue("SailDlgPos", s_WindowPos );
        pSettings->setValue("SailDlgSize", s_WindowSize );
        pSettings->setValue("SailDlgMaximized", s_bWindowMaximized );
    }
    pSettings->endGroup();
}



void SailDlg::ReadPointData()
{
    double d;
    bool bOK;
    QString strong;
    QStandardItem *pItem;
    SailSection *pSection = m_pSail->m_oaSection.at(m_iSection);
    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

    for (int i=0; i< m_pPointModel->rowCount(); i++)
    {
        pItem = m_pPointModel->item(i,0);
        if(pItem)
        {
            strong = pItem->text();
            strong.replace(" ","");
            d = strong.toDouble(&bOK);
            if(bOK) pSection->m_CtrlPoint[i].x = d / pMainFrame->m_mtoUnit;
        }

        pItem = m_pPointModel->item(i,1);
        if(pItem)
        {
            strong = pItem->text();
            strong.replace(" ","");
            d = strong.toDouble(&bOK);
            if(bOK) pSection->m_CtrlPoint[i].y = d / pMainFrame->m_mtoUnit;
        }

        pItem = m_pPointModel->item(i,2);
        if(pItem)
        {
            strong = pItem->text();
            strong.replace(" ","");
            d = strong.toDouble(&bOK);
            if(bOK) pSection->m_CtrlPoint[i].z = d / pMainFrame->m_mtoUnit;
        }
    }
}



void SailDlg::OnExportSail()
{
    if(!m_pSail) return;
    m_pSail->Export();
}



void SailDlg::OnImportSail()
{
    if(!m_pSail) return;
    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),pMainFrame->m_XMLPath,
                                                    tr("Sailcut files (*.saildef *.xml)"));
    //    QString filePath = "/home/windsoarer/sailcut.saildef";
    if (!filePath.isEmpty())
    {
        pMainFrame->m_XMLPath = filePath;
        QFile file(filePath);
        m_pSail->Import(&file);

        SetSailData(m_pSail);

        UpdateView();
    }
}


bool SailDlg::eventFilter( QObject* o, QEvent* e )
{
    if(o==m_pctrlSectionTable && e->type() == QEvent::Resize)
    {
        // get size from event, resize columns here
        int wc    = (int)((double)m_pctrlSectionTable->width() *0.9/ (double)m_pSectionModel->columnCount());
        for(int ic=0; ic<m_pSectionModel->columnCount(); ic++)
            m_pctrlSectionTable->setColumnWidth(ic, wc);
    }
    else if(o==m_pctrlPointTable && e->type() == QEvent::Resize)
    {
        // get size from event, resize columns here
        int w2  = (int)((double)m_pctrlPointTable->width() / 3.5);
        m_pctrlPointTable->setColumnWidth(0, w2);
        m_pctrlPointTable->setColumnWidth(1, w2);
        m_pctrlPointTable->setColumnWidth(2, w2);
    }
    return QDialog::eventFilter( o, e );
}


void SailDlg::OnAlignedLE()
{
    if(!m_pSail->IsNURBSSail()) return;
    if(!m_pSail->m_oaSection.size()) return;
    if(!m_pSail->m_oaSection.first()->m_CtrlPoint.size()) return;

    NURBSSail *pNSail = dynamic_cast<NURBSSail*>(m_pSail);

    pNSail->m_bAlignedLE = m_pctrlAlignedLE->isChecked();
    if(pNSail->m_bAlignedLE)
    {
        //translate  middle section
        if(pNSail->m_oaSection.size() && pNSail->m_oaSection[1]->m_CtrlPoint.size())
        {
            Vector3d newpos;
            newpos.x = (pNSail->m_oaSection.first()->m_CtrlPoint.first().x + pNSail->m_oaSection.last()->m_CtrlPoint.first().x)/2.0;
            newpos.y = (pNSail->m_oaSection.first()->m_CtrlPoint.first().y + pNSail->m_oaSection.last()->m_CtrlPoint.first().y)/2.0;
            newpos.z = pNSail->m_oaSection[1]->m_Position.z;
            pNSail->m_oaSection[1]->MoveTo(newpos);
        }
        pNSail->SplineSurface();
        UpdateView();
    }
}












