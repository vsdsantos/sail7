/****************************************************************************

    MainFrame  Class
    Copyright (C) 2012 Andre Deperrois 
    All rights reserved

*****************************************************************************/

#include <QtCore>
#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QDir>
#include <QMenuBar>
#include <QMessageBox>
#include <QPluginLoader>
#include <QAction>
#include <QFileDialog>
#include <QFontDatabase>
#include <QDesktopServices>

#include "./mainframe.h"
#include "./globals.h"
#include "./sail7/sail7.h"
#include "./sail7/sectionviewwidget.h"
#include "./sail7/boatdlg.h"
#include "./sail7/boatanalysisDlg.h"
#include "./sail7/boatpolardlg.h"
#include "./sail7/gl3dscales.h"
#include "./sail7/gl3dbodydlg.h"
#include "./sail7/saildlg.h"

#include "./sail7/bodytransdlg.h"
#include "./misc/abouts7.h"
#include "./misc/objectpropsdlg.h"
#include "./misc/linepickerdlg.h"
#include "./misc/translatordlg.h"
#include "./graph/graphdlg.h"
#include "./objects/boatpolar.h"
#include "./objects/boatopp.h"


//#ifdef Q_WS_MAC
//#include <CoreFoundation/CoreFoundation.h>
//#endif



QPointer<MainFrame> MainFrame::_self = nullptr;

MainFrame::MainFrame(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    m_VersionName = QString::fromLatin1("sail7 v1.0");


    setWindowTitle(m_VersionName);
    setWindowIcon(QIcon(":/icons/s7.png"));

    if(!QGLFormat::hasOpenGL())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Your system does not provide support for OpenGL.\nSail7 will not operate correctly."));
    }
    m_bMaximized = true;
    m_LengthUnit  = 3;
    m_AreaUnit    = 3;
    m_WeightUnit  = 1;
    m_SpeedUnit   = 0;
    m_ForceUnit   = 0;
    m_MomentUnit  = 0;

    m_pSail7 = nullptr;

    CreateDockWindows();
    m_ProjectName.clear();

    m_BorderClr       = QColor(200,200,200);
    m_BackgroundColor = QColor(0, 5, 10);
    m_TextColor       = QColor(220,220,220);

    m_TextFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_TextFont.setPointSize(10);


    m_RefGraph.SetGraphName("Reference Graph");

    m_ImageFormat = 2;
    m_ExportFileType = 1;
    m_bReverseZoom = false;
    m_bAlphaChannel = false;
    m_bSaveSettings = true;

    m_StyleName.clear();
    m_GraphExportFilter = "Comma Separated Values (*.csv)";

    m_LastDirName = QDir::homePath();

    QDesktopWidget desktop;
    QRect r = desktop.screenGeometry();
    m_DlgPos = QPoint(int(r.width()/3), int(r.height()/3));



    if(LoadSettings())
    {
#ifdef Q_WS_MAC
        QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Sail7");
#else
        QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Sail7");
#endif

        m_RefGraph.LoadSettings(&settings);

        m_pSail7->LoadSettings(&settings);

        m_pGL3DScales->LoadSettings(&settings);
    }

    SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
             m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);

    CreateActions();
    CreateMainMenu();
    CreateSail7Menus();
    CreateToolbars();
    CreateStatusBar();

    // second line
    m_crColors[0] = QColor(255,0,0);
    m_crColors[1] = QColor(0,255,0);
    m_crColors[2] = QColor(0,0,255);
    m_crColors[3] = QColor(255,255,0);
    m_crColors[4] = QColor(255,0,255);
    m_crColors[5] = QColor(0,255,255);

    // third line
    m_crColors[6]  = QColor(200,60,60);
    m_crColors[7]  = QColor(0,160,0);
    m_crColors[8]  = QColor(100,100,240);
    m_crColors[9]  = QColor(170,170,0);
    m_crColors[10] = QColor(130,0,130);
    m_crColors[11] = QColor(0,130,130);

    // fourth line
    m_crColors[12] = QColor(255,128,128);
    m_crColors[13] = QColor(0,255,128);
    m_crColors[14] = QColor(64,200,255);
    m_crColors[15] = QColor(170,170,100);
    m_crColors[16] = QColor(190,100,190);
    m_crColors[17] = QColor(100,170,170);

    // fifth line
    m_crColors[18] = QColor(228,150,70);
    m_crColors[19] = QColor(170,255,170);
    m_crColors[20] = QColor(120,120,255);
    m_crColors[21] = QColor(228,228,128);
    m_crColors[22] = QColor(255,170,255);
    m_crColors[23] = QColor(170,255,255);

    // first line
    m_crColors[24] = QColor(50,50,50);
    m_crColors[25] = QColor(90,90,90);
    m_crColors[26] = QColor(130,130,130);
    m_crColors[27] = QColor(170,170,170);
    m_crColors[28] = QColor(210,210,210);
    m_crColors[29] = QColor(255,255,255);


            m_UFOType = "";

    m_bSaved     = true;

    memset(m_aij, 0, sizeof(m_aij));
    memset(m_aijRef, 0, sizeof(m_aijRef));
    memset(m_RHS, 0, sizeof(m_RHS));
    memset(m_RHSRef, 0, sizeof(m_RHSRef));


    CPanel::s_pNode = m_Node;
    CPanel::s_VortexPos = 0.25;
    CPanel::s_CtrlPos   = 0.75;


    m_pSail7->m_paij          = m_aij;
    m_pSail7->m_paijRef       = m_aijRef;
    m_pSail7->m_pRHS          = m_RHS;
    m_pSail7->m_pRHSRef       = m_RHSRef;
    Sail7::s_pNode         = m_Node;
    Sail7::s_pMemNode      = m_MemNode;
    Sail7::s_pWakeNode     = m_WakeNode;
    Sail7::s_pRefWakeNode  = m_RefWakeNode;
    Sail7::s_pPanel        = m_Panel;
    Sail7::s_pMemPanel     = m_MemPanel;
    Sail7::s_pWakePanel    = m_WakePanel;
    Sail7::s_pRefWakePanel = m_RefWakePanel;

    BoatAnalysisDlg::s_pPanel        = m_Panel;
    BoatAnalysisDlg::s_pNode         = m_Node;
    BoatAnalysisDlg::s_pWakePanel    = m_WakePanel;
    BoatAnalysisDlg::s_pWakeNode     = m_WakeNode;
    BoatAnalysisDlg::s_pMemNode      = m_MemNode;
    BoatAnalysisDlg::s_pMemPanel     = m_MemPanel;
    BoatAnalysisDlg::s_pRefWakeNode  = m_RefWakeNode;
    BoatAnalysisDlg::s_pRefWakePanel = m_RefWakePanel;
    BoatAnalysisDlg::s_aij           = m_aij;
    BoatAnalysisDlg::s_aijWake       = m_aijRef;
    BoatAnalysisDlg::s_RHS           = m_RHS;
    BoatAnalysisDlg::s_RHSRef        = m_RHSRef;

    HideToolbars();

    SetMenus();
}


void MainFrame::AboutSail7()
{
    AboutS7 dlg(this);
    dlg.move(m_DlgPos);
    dlg.exec();
}



void MainFrame::AddRecentFile(const QString &PathName)
{
    m_RecentFiles.removeAll(PathName);
    m_RecentFiles.prepend(PathName);
    while (m_RecentFiles.size() > MAXRECENTFILES)
        m_RecentFiles.removeLast();

    updateRecentFileActions();
}


void MainFrame::closeEvent (QCloseEvent * event)
{
    if(!m_bSaved)
    {
        int resp = QMessageBox::question(this, tr("Exit"), tr("Save the project before exit ?"),
                                         QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
                                         QMessageBox::Yes);
        if(resp == QMessageBox::Yes)
        {
            if(!SaveProject(m_FileName))
            {
                event->ignore();
                return;
            }
            AddRecentFile(m_FileName);
        }
        else if (resp==QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
    }
    DeleteProject();

    SaveSettings();
    event->accept();//continue closing
}


void MainFrame::contextMenuEvent (QContextMenuEvent * event)
{
    if(m_pctrlCentralWidget->currentIndex()==0) m_p2DWidget->contextMenuEvent(event);
    else                                        m_p3DWidget->contextMenuEvent(event);
}


void MainFrame::CreateActions()
{
    newProjectAct = new QAction(QIcon(":/images/new.png"), tr("New Project"), this);
    newProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    newProjectAct->setStatusTip(tr("Save and close the current project, create a new project"));
    connect(newProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));

    closeProjectAct = new QAction(QIcon(":/images/new.png"), tr("Close the Project"), this);
    closeProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    closeProjectAct->setStatusTip(tr("Save and close the current project"));
    connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));

    m_pOpenAct = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    m_pOpenAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    m_pOpenAct->setStatusTip(tr("Open an existing file"));
    connect(m_pOpenAct, SIGNAL(triggered()), this, SLOT(OnLoadFile()));

    m_pOpenLast = new QAction(QIcon(":/images/open.png"), tr("&Open the last project file"), this);
    m_pOpenLast->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
    connect(m_pOpenLast, SIGNAL(triggered()), SLOT(OnLoadLastFile()));

    insertAct = new QAction(tr("&Insert Project..."), this);
    insertAct->setStatusTip(tr("Insert an existing project in the current project"));
    connect(insertAct, SIGNAL(triggered()), this, SLOT(OnInsertProject()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
    saveAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    saveAct->setStatusTip(tr("Save the project to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(OnSaveProject()));

    saveProjectAsAct = new QAction(tr("Save Project As..."), this);
    saveProjectAsAct->setStatusTip(tr("Save the current project under a new name"));
    connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(OnSaveProjectAs()));

    unitsAct = new QAction(tr("Units..."), this);
    unitsAct->setStatusTip(tr("Define the units for this project"));
    connect(unitsAct, SIGNAL(triggered()), this, SLOT(OnUnits()));

    languageAct = new QAction(tr("Language..."), this);
    languageAct->setStatusTip(tr("Define the default language for the application"));
    connect(languageAct, SIGNAL(triggered()), this, SLOT(OnLanguage()));

    restoreToolbarsAct     = new QAction(tr("Restore toolbars"), this);
    restoreToolbarsAct->setStatusTip(tr("Restores the toolbars to their original state"));
    connect(restoreToolbarsAct, SIGNAL(triggered()), this, SLOT(OnRestoreToolbars()));

    saveViewToImageFileAct = new QAction(tr("Save View to Image File"), this);
    saveViewToImageFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    saveViewToImageFileAct->setStatusTip(tr("Saves the current view to a file on disk"));
    connect(saveViewToImageFileAct, SIGNAL(triggered()), this, SLOT(OnSaveViewToImageFile()));


    resetSettingsAct = new QAction(tr("Reset Default Settings"), this);
    resetSettingsAct->setStatusTip(tr("will revert to default settings at the next session"));
    connect(resetSettingsAct, SIGNAL(triggered()), this, SLOT(OnResetSettings()));


    for (int i = 0; i < MAXRECENTFILES; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    styleAct = new QAction(tr("Preferences"), this);
    styleAct->setStatusTip(tr("Define the color and font options for all views and graphs"));
    connect(styleAct, SIGNAL(triggered()), this, SLOT(OnStyle()));


    exportCurGraphAct = new QAction(tr("Export Graph"), this);
    exportCurGraphAct->setStatusTip(tr("Export the current graph data to a text file"));
    connect(exportCurGraphAct, SIGNAL(triggered()), this, SLOT(OnExportCurGraph()));

    resetCurGraphScales = new QAction(QIcon(":/images/OnResetGraphScale.png"), tr("Reset Graph Scales")+"\t(R)", this);
    resetCurGraphScales->setStatusTip(tr("Restores the graph's x and y scales"));
    connect(resetCurGraphScales, SIGNAL(triggered()), this, SLOT(OnResetCurGraphScales()));

    resetGraphLegend = new QAction(tr("Reset Graph Legend"), this);
    resetGraphLegend->setStatusTip(tr("Restores the legend's position"));
    connect(resetGraphLegend, SIGNAL(triggered()), m_pSail7, SLOT(OnResetGraphLegend()));

    twoGraphs = new QAction(tr("Two Graphs")+"\t(T)", this);
    twoGraphs->setStatusTip(tr("Display the first two graphs"));
    twoGraphs->setCheckable(true);
    connect(twoGraphs, SIGNAL(triggered()), m_pSail7, SLOT(OnTwoGraphs()));

    fourGraphs = new QAction(tr("All Graphs")+"\t(A)", this);
    fourGraphs->setStatusTip(tr("Display all four graphs"));
    fourGraphs->setCheckable(true);
    connect(fourGraphs, SIGNAL(triggered()), m_pSail7, SLOT(OnFourGraphs()));

    Graph1 = new QAction(tr("Graph 1")+"\t(1)", this);
    Graph1->setStatusTip(tr("Display only the first graph"));
    Graph1->setCheckable(true);
    connect(Graph1, SIGNAL(triggered()), m_pSail7, SLOT(OnSingleGraph1()));

    Graph2 = new QAction(tr("Graph 2")+"\t(2)", this);
    Graph2->setStatusTip(tr("Display only the second graph"));
    Graph2->setCheckable(true);
    connect(Graph2, SIGNAL(triggered()), m_pSail7, SLOT(OnSingleGraph2()));

    Graph3 = new QAction(tr("Graph 3")+"\t(3)", this);
    Graph3->setStatusTip(tr("Display only the third graph"));
    Graph3->setCheckable(true);
    connect(Graph3, SIGNAL(triggered()), m_pSail7, SLOT(OnSingleGraph3()));

    Graph4 = new QAction(tr("Graph 4")+"\t(4)", this);
    Graph4->setStatusTip(tr("Display only the fourth graph"));
    Graph4->setCheckable(true);
    connect(Graph4, SIGNAL(triggered()), m_pSail7, SLOT(OnSingleGraph4()));

    allBoatPolarGraphsScalesAct = new QAction(tr("Reset All Graph Scales"), this);
    allBoatPolarGraphsScalesAct->setStatusTip(tr("Reset the scales of all four polar graphs"));
    connect(allBoatPolarGraphsScalesAct, SIGNAL(triggered()), m_pSail7, SLOT(OnAllBoatPolarGraphScales()));

    allBoatPolarGraphsSettings = new QAction(tr("All Graph Settings"), this);
    allBoatPolarGraphsSettings->setStatusTip(tr("Define the settings of all four polar graphs"));
    connect(allBoatPolarGraphsSettings, SIGNAL(triggered()), m_pSail7, SLOT(OnAllBoatPolarGraphSettings()));




    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    viewLogFile = new QAction(tr("View Log File")+"\t(L)", this);
    connect(viewLogFile, SIGNAL(triggered()), this, SLOT(OnLogFile()));


    aboutAct = new QAction(tr("&About Sail7"), this);
    aboutAct->setStatusTip(tr("More information about Sail7"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(AboutSail7()));

    aboutQtAct = new QAction(tr("About Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


    CreateSail7Actions();
}



void MainFrame::CreateDockWindows()
{
    Sail7::s_pMainFrame               = this;
    ThreeDWidget::s_pMainFrame         = this;
    TwoDWidget::s_pMainFrame           = this;
    ObjectPropsDlg::s_pMainFrame       = this;
    GraphDlg::s_pMainFrame             = this;
    BoatDlg::s_pMainFrame              = this;
    BoatPolarDlg::s_pMainFrame         = this;
    BoatPolar::s_pMainFrame            = this;
    BoatOpp::s_pMainFrame              = this;
    Body::s_pMainFrame                = this;
    BoatAnalysisDlg::s_pMainFrame      = this;
    GL3DScales::s_pMainFrame           = this;
    DisplaySettingsDlg::s_pMainFrame   = this;
    BodyTransDlg::s_pMainFrame         = this;
    SailDlg::s_pMainFrame              = this;
    GL3dBodyDlg::s_pMainFrame          = this;
    SectionViewWidget::s_pMainFrame    = this;
    Sail::s_pMainFrame                = this;
    SailViewWidget::s_pMainFrame       = this;

    m_pctrlSail7Widget = new QDockWidget("Sail7", this);
    m_pctrlSail7Widget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_pctrlSail7Widget);
    m_p2DWidget = new TwoDWidget(this);
    m_p3DWidget = new ThreeDWidget(this);


    m_pSail7 = new Sail7;

    m_pctrlSail7Widget->setWidget(m_pSail7);
    m_pctrlSail7Widget->setVisible(false);
    m_pctrlSail7Widget->setFloating(true);
    m_pctrlSail7Widget->move(960,60);


    m_pGL3DScales = new GL3DScales(this);

    m_pGL3DScales->setAttribute(Qt::WA_DeleteOnClose, false);
    m_pctrl3DScalesWidget = new QDockWidget(tr("3D Scales"), this);
    m_pctrl3DScalesWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_pctrl3DScalesWidget);
    m_pctrl3DScalesWidget->setWidget(m_pGL3DScales);
    m_pctrl3DScalesWidget->setVisible(false);
    m_pctrl3DScalesWidget->setFloating(true);
    m_pctrl3DScalesWidget->move(60,60);


    m_pctrlCentralWidget = new QStackedWidget;
    m_pctrlCentralWidget->addWidget(m_p2DWidget);
    m_pctrlCentralWidget->addWidget(m_p3DWidget);

    setCentralWidget(m_pctrlCentralWidget);


    QSizePolicy sizepol;
    sizepol.setHorizontalPolicy(QSizePolicy::Expanding);
    sizepol.setVerticalPolicy(QSizePolicy::Expanding);
    m_p2DWidget->setSizePolicy(sizepol);


    Sail7::s_p2DWidget = m_p2DWidget;
    Sail7::s_p3dWidget = m_p3DWidget;
    m_pSail7->m_ArcBall.m_p3dWidget  = m_p3DWidget;
    m_pSail7->m_poaBoat      = &m_oaBoat;
    m_pSail7->m_poaBoatPolar = &m_oaBoatPolar;
    m_pSail7->m_poaBoatOpp   = &m_oaBoatOpp;


    TwoDWidget::s_pSail7         = m_pSail7;
    ThreeDWidget::s_pSail7       = m_pSail7;
    SailViewWidget::s_pSail7     = m_pSail7;
    SailDlg::s_pSail7            = m_pSail7;
    BoatDlg::s_pSail7            = m_pSail7;
    BoatPolarDlg::s_pSail7       = m_pSail7;
    BoatAnalysisDlg::s_pSail7    = m_pSail7;
    BoatPolar::s_pSail7         = m_pSail7;
    BoatOpp::s_pSail7           = m_pSail7;
    DisplaySettingsDlg::s_pSail7 = m_pSail7;
    GL3DScales::s_pSail7         = m_pSail7;

    GraphDlg::s_ActivePage = 0;
}


void MainFrame::CreateMainMenu()
{
    // Create common File, View and Help menus
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newProjectAct);
    fileMenu->addAction(m_pOpenAct);
    fileMenu->addAction(m_pOpenLast);
    fileMenu->addAction(insertAct);
    fileMenu->addAction(closeProjectAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveProjectAsAct);
    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MAXRECENTFILES; ++i) fileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    updateRecentFileActions();

    optionsMenu = menuBar()->addMenu(tr("Options"));
    optionsMenu->addSeparator();
    optionsMenu->addAction(languageAct);
    optionsMenu->addSeparator();
    optionsMenu->addAction(unitsAct);
    optionsMenu->addSeparator();
    optionsMenu->addAction(styleAct);
    optionsMenu->addSeparator();
    optionsMenu->addAction(restoreToolbarsAct);
    optionsMenu->addSeparator();
    optionsMenu->addAction(resetSettingsAct);

    helpMenu = menuBar()->addMenu(tr("&?"));
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(aboutAct);
}


void MainFrame::CreateSail7Actions()
{
    Sail7NewBoatAct = new QAction(tr("New Boat"), m_pSail7);
    Sail7NewBoatAct->setShortcut(QKeySequence(Qt::Key_F3));
    connect(Sail7NewBoatAct, SIGNAL(triggered()), m_pSail7, SLOT(OnNewBoat()));

    Sail7EditBoatAct = new QAction(tr("Edit Boat"), m_pSail7);
    Sail7EditBoatAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F3));
    connect(Sail7EditBoatAct, SIGNAL(triggered()), m_pSail7, SLOT(OnEditCurBoat()));

    BoatPolarAct = new QAction(QIcon(":/images/OnPolarView.png"), tr("Polar View")+"\tF8", this);
    BoatPolarAct->setCheckable(true);
    BoatPolarAct->setStatusTip(tr("Switch to the Polar view"));
    connect(BoatPolarAct, SIGNAL(triggered()), m_pSail7, SLOT(OnSailPolarView()));

    Boat3DAct = new QAction(QIcon(":/images/Sailboat_256.png"), tr("3D View")+"\tF4", this);
    Boat3DAct->setCheckable(true);
    Boat3DAct->setStatusTip(tr("Switch to the 3D view"));
    connect(Boat3DAct, SIGNAL(triggered()), m_pSail7, SLOT(OnSail3DView()));

    Sail73DPrefsAct = new QAction(tr("3D Color Preferences"), this);
    Sail73DPrefsAct->setStatusTip(tr("Define the style and color preferences for the 3D view"));
    connect(Sail73DPrefsAct, SIGNAL(triggered()), m_pSail7, SLOT(On3DPrefs()));

    Sail73DScalesAct = new QAction(tr("3D Scales"), this);
    Sail73DScalesAct->setStatusTip(tr("Define the scales for the 3D display of lift, moment, drag, and downwash"));
    //    Sail73DScalesAct->setCheckable(true);
    connect(Sail73DScalesAct, SIGNAL(triggered()), m_pSail7, SLOT(OnGL3DScale()));

    Sail73DLightAct = new QAction(tr("3D Light"), this);
    Sail73DLightAct->setCheckable(true);
    Sail73DLightAct->setStatusTip(tr("Toggle the light in 3D view"));
    connect(Sail73DLightAct, SIGNAL(triggered()), m_pSail7, SLOT(OnLight()));

    Sail73DLightOptionsAct = new QAction(tr("3D Light Options"), this);
    Sail73DLightOptionsAct->setStatusTip(tr("Define the light options in 3D view"));
    connect(Sail73DLightOptionsAct, SIGNAL(triggered()), m_pSail7, SLOT(OnSetupLight()));

    renameCurBoat = new QAction(tr("Rename...")+"\tF2", this);
    renameCurBoat->setStatusTip(tr("Rename the currently selected object"));
    connect(renameCurBoat, SIGNAL(triggered()), m_pSail7, SLOT(OnRenameCurBoat()));

    deleteCurBoat = new QAction(tr("Delete..."), this);
    deleteCurBoat->setStatusTip(tr("Delete the currently selected boat"));
    connect(deleteCurBoat, SIGNAL(triggered()), m_pSail7, SLOT(OnDeleteCurBoat()));

    duplicateCurBoat = new QAction(tr("Duplicate..."), this);
    duplicateCurBoat->setStatusTip(tr("Duplicate the currently selected boat"));
    connect(duplicateCurBoat, SIGNAL(triggered()), m_pSail7, SLOT(OnDuplicateCurBoat()));

    defineBoatPolar = new QAction(tr("Define an Analysis")+" \t(F6)", this);
    defineBoatPolar->setStatusTip(tr("Define an analysis for the current boat"));
    //    defineWPolar->setShortcut(tr("F6"));
    connect(defineBoatPolar, SIGNAL(triggered()), m_pSail7, SLOT(OnDefineBoatPolar()));

    editBoatPolar= new QAction(tr("Edit ...")+" \t(Shift+F6)", this);
    editBoatPolar->setStatusTip(tr("Modify the analysis parameters of this polar"));
    connect(editBoatPolar, SIGNAL(triggered()), m_pSail7, SLOT(OnEditBoatPolar()));

    renameCurBoatPolar = new QAction(tr("Rename..."), this);
    renameCurBoatPolar->setStatusTip(tr("Rename the currently selected polar"));
    connect(renameCurBoatPolar, SIGNAL(triggered()), m_pSail7, SLOT(OnRenameCurBoatPolar()));

    deleteCurBoatPolar= new QAction(tr("Delete ..."), this);
    deleteCurBoatPolar->setStatusTip(tr("Delete the currently selected polar"));
    connect(deleteCurBoatPolar, SIGNAL(triggered()), m_pSail7, SLOT(OnDeleteCurBoatPolar()));

    resetCurBoatPolar= new QAction(tr("Reset ..."), this);
    resetCurBoatPolar->setStatusTip(tr("Delete the currently selected polar"));
    connect(resetCurBoatPolar, SIGNAL(triggered()), m_pSail7, SLOT(OnResetCurBoatPolar()));

    exportCurBoatOpp= new QAction(tr("Export"), this);
    exportCurBoatOpp->setStatusTip(tr("Export the currently selected operating point to a text file"));
    connect(exportCurBoatOpp, SIGNAL(triggered()), m_pSail7, SLOT(OnExportCurBoatOpp()));

    exportCurBoatPolar= new QAction(tr("Export"), this);
    exportCurBoatPolar->setStatusTip(tr("Export the currently selected polar to a text file"));
    connect(exportCurBoatPolar, SIGNAL(triggered()), m_pSail7, SLOT(OnExportCurBoatPolar()));

    deleteAllBoatOpps = new QAction(tr("Delete All OpPoints"), this);
    deleteAllBoatOpps->setStatusTip(tr("Delete all the operating points of all planes and polars"));
    connect(deleteAllBoatOpps, SIGNAL(triggered()), m_pSail7, SLOT(OnDeleteAllBoatOpps()));

    deleteAllBoatPolarOpps = new QAction(tr("Delete Associated OpPoints"), this);
    deleteAllBoatPolarOpps->setStatusTip(tr("Delete all the operating points of the currently selected polar"));
    connect(deleteAllBoatPolarOpps, SIGNAL(triggered()), m_pSail7, SLOT(OnDeleteAllBoatPolarOpps()));

    deleteCurBoatOpp= new QAction(tr("Delete ..."), this);
    deleteCurBoatOpp->setStatusTip(tr("Delete the currently selected Operating Point"));
    connect(deleteCurBoatOpp, SIGNAL(triggered()), m_pSail7, SLOT(OnDeleteCurBoatOpp()));

    showBoatOppProperties= new QAction(tr("Properties"), this);
    showBoatOppProperties->setStatusTip(tr("Show the properties of the currently selected operating point"));
    showBoatOppProperties->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_Return));
    connect(showBoatOppProperties, SIGNAL(triggered()), m_pSail7, SLOT(OnBoatOppProps()));

    showBoatPolarProperties= new QAction(tr("Properties"), this);
    showBoatPolarProperties->setStatusTip(tr("Show the properties of the currently selected polar"));
    showBoatPolarProperties->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Return));
    connect(showBoatPolarProperties, SIGNAL(triggered()), m_pSail7, SLOT(OnBoatPolarProps()));

    hideCurBoatPlrs = new QAction(tr("Hide Associated Polars"), this);
    hideCurBoatPlrs->setStatusTip(tr("Hide all the polar curves associated to the currently selected boat"));
    connect(hideCurBoatPlrs, SIGNAL(triggered()), m_pSail7, SLOT(OnHideCurBoatPolars()));

    showCurBoatPlrs = new QAction(tr("Show Associated Polars"), this);
    showCurBoatPlrs->setStatusTip(tr("Show all the polar curves associated to the currently selected boat"));
    connect(showCurBoatPlrs, SIGNAL(triggered()), m_pSail7, SLOT(OnShowCurBoatPolars()));

    deleteCurBoatPlrs = new QAction(tr("Delete Associated Polars"), this);
    deleteCurBoatPlrs->setStatusTip(tr("Delete all the polars associated to the currently selected boat"));
    connect(deleteCurBoatPlrs, SIGNAL(triggered()), m_pSail7, SLOT(OnDeleteCurBoatPolars()));

    hideAllBoatPlrs = new QAction(tr("Hide All Polars"), this);
    hideAllBoatPlrs->setStatusTip(tr("Hide all the polar curves of all wings and planes"));
    connect(hideAllBoatPlrs, SIGNAL(triggered()), m_pSail7, SLOT(OnHideAllBoatPolars()));

    showAllBoatPlrs = new QAction(tr("Show All Polars"), this);
    showAllBoatPlrs->setStatusTip(tr("Show all the polar curves of all wings and planes"));
    connect(showAllBoatPlrs, SIGNAL(triggered()), m_pSail7, SLOT(OnShowAllBoatPolars()));


}


void MainFrame::CreateSail7Menus()
{
    Sail7ViewMenu= menuBar()->addMenu(tr("View"));
    {
        Sail7ViewMenu->addAction(BoatPolarAct);
        Sail7ViewMenu->addAction(Boat3DAct);
        Sail7ViewMenu->addSeparator();
        Sail7ViewMenu->addAction(Sail73DScalesAct);
        Sail7ViewMenu->addAction(Sail73DPrefsAct);
        Sail7ViewMenu->addAction(Sail73DLightAct);
        Sail7ViewMenu->addAction(Sail73DLightOptionsAct);
        Sail7ViewMenu->addSeparator();
        Sail7ViewMenu->addAction(saveViewToImageFileAct);
    }

    Sail7ObjectMenu= menuBar()->addMenu(tr("Boat"));
    {
        Sail7ObjectMenu->addAction(Sail7NewBoatAct);
        Sail7ObjectMenu->addSeparator();
        CurBoatMenu = Sail7ObjectMenu->addMenu(tr("Current Boat"));
        {
            CurBoatMenu->addAction(Sail7EditBoatAct);
            CurBoatMenu->addAction(renameCurBoat);
            CurBoatMenu->addAction(deleteCurBoat);
            CurBoatMenu->addAction(duplicateCurBoat);
            CurBoatMenu->addAction(deleteAllBoatOpps);
            CurBoatMenu->addSeparator();
            CurBoatMenu->addAction(showCurBoatPlrs);
            CurBoatMenu->addAction(hideCurBoatPlrs);
            CurBoatMenu->addAction(deleteCurBoatPlrs);
        }
    }

    Sail7PlrMenu= menuBar()->addMenu(tr("Polars"));
    {
        Sail7PlrMenu->addAction(defineBoatPolar);
        Sail7PlrMenu->addAction(showAllBoatPlrs);
        Sail7PlrMenu->addAction(hideAllBoatPlrs);
        CurBoatPlrMenu = Sail7PlrMenu->addMenu(tr("Current Polar"));
        CurBoatPlrMenu->addAction(editBoatPolar);
        CurBoatPlrMenu->addAction(renameCurBoatPolar);
        CurBoatPlrMenu->addAction(deleteCurBoatPolar);
        CurBoatPlrMenu->addAction(deleteAllBoatPolarOpps);
        CurBoatPlrMenu->addAction(resetCurBoatPolar);
        CurBoatPlrMenu->addAction(showBoatPolarProperties);
        CurBoatPlrMenu->addAction(exportCurBoatPolar);

        BoatPolarGraphMenu = Sail7PlrMenu->addMenu(tr("Graphs"));
        {
            BoatPolarGraphMenu->addAction(Graph1);
            BoatPolarGraphMenu->addAction(Graph2);
            BoatPolarGraphMenu->addAction(Graph3);
            BoatPolarGraphMenu->addAction(Graph4);
            BoatPolarGraphMenu->addSeparator();
            BoatPolarGraphMenu->addAction(twoGraphs);
            BoatPolarGraphMenu->addAction(fourGraphs);
            BoatPolarGraphMenu->addSeparator();
            BoatPolarGraphMenu->addAction(allBoatPolarGraphsSettings);
            BoatPolarGraphMenu->addAction(allBoatPolarGraphsScalesAct);
            BoatPolarGraphMenu->addAction(resetGraphLegend);
        }
    }

    Sail7OppMenu= menuBar()->addMenu(tr("OpPoints"));
    {
        CurBoatOppMenu = Sail7OppMenu->addMenu(tr("Current OpPoint"));
        CurBoatOppMenu->addAction(deleteCurBoatOpp);
        CurBoatOppMenu->addAction(showBoatOppProperties);
        CurBoatOppMenu->addAction(exportCurBoatOpp);
        Sail7OppMenu->addAction(viewLogFile);
    }


    //Context menus


    //Sail7 Polar View Context Menu
    SailPlrCtxMenu = new QMenu(tr("Context Menu"),this);
    {
        SailPlrCtxMenu->addMenu(CurBoatMenu);
        SailPlrCtxMenu->addSeparator();
        SailPlrCtxMenu->addMenu(CurBoatPlrMenu);
        SailPlrCtxMenu->addSeparator();
        SailPlrCtxMenu->addMenu(CurBoatOppMenu);
        SailPlrCtxMenu->addSeparator();
        SailPlrCtxMenu->addAction(showAllBoatPlrs);
        SailPlrCtxMenu->addAction(hideAllBoatPlrs);
        SailPlrCtxMenu->addSeparator();
        SailPlrCtxMenu->addMenu(BoatPolarGraphMenu);
        SailPlrCtxMenu->addAction(exportCurGraphAct);
        SailPlrCtxMenu->addSeparator();
        SailPlrCtxMenu->addAction(viewLogFile);
        SailPlrCtxMenu->addAction(saveViewToImageFileAct);
    }

    //Sail7 3D View Context Menu
    Sail3DCtxMenu = new QMenu(tr("Context Menu"),this);
    {
        Sail3DCtxMenu->addMenu(CurBoatMenu);
        Sail3DCtxMenu->addSeparator();
        Sail3DCtxMenu->addMenu(CurBoatPlrMenu);
        Sail3DCtxMenu->addSeparator();
        Sail3DCtxMenu->addMenu(CurBoatOppMenu);
        Sail3DCtxMenu->addSeparator();
        Sail3DCtxMenu->addAction(showAllBoatPlrs);
        Sail3DCtxMenu->addAction(hideAllBoatPlrs);
        Sail3DCtxMenu->addSeparator();
        Sail3DCtxMenu->addAction(Sail73DScalesAct);
        Sail3DCtxMenu->addAction(Sail73DPrefsAct);
        Sail3DCtxMenu->addAction(Sail73DLightAct);
        Sail3DCtxMenu->addAction(Sail73DLightOptionsAct);
        Sail3DCtxMenu->addSeparator();
        Sail3DCtxMenu->addAction(viewLogFile);
        Sail3DCtxMenu->addAction(saveViewToImageFileAct);
    }
}



void MainFrame::CreateSail7Toolbar()
{
    m_pctrlBoatPolarView = new QToolButton;
    m_pctrlBoatPolarView->setDefaultAction(BoatPolarAct);
    m_pctrlBoatPolarView->setCheckable(true);
    m_pctrlBoat3dView = new QToolButton;
    m_pctrlBoat3dView->setDefaultAction(Boat3DAct);
    m_pctrlBoat3dView->setCheckable(true);

    m_pctrlBoat      = new QComboBox;
    m_pctrlBoatPolar = new QComboBox;
    m_pctrlBoatOpp   = new QComboBox;
    m_pctrlBoat->setMinimumWidth(150);
    m_pctrlBoatPolar->setMinimumWidth(150);
    m_pctrlBoatOpp->setMinimumWidth(80);
    m_pctrlBoat->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_pctrlBoatPolar->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_pctrlBoatOpp->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    m_pctrlSail7ToolBar = addToolBar(tr("UFO"));
    m_pctrlSail7ToolBar->addAction(newProjectAct);
    m_pctrlSail7ToolBar->addAction(m_pOpenAct);
    m_pctrlSail7ToolBar->addAction(saveAct);

    m_pctrlSail7ToolBar->addSeparator();
    m_pctrlSail7ToolBar->addWidget(m_pctrlBoatPolarView);
    m_pctrlSail7ToolBar->addWidget(m_pctrlBoat3dView);

    m_pctrlSail7ToolBar->addSeparator();
    m_pctrlSail7ToolBar->addWidget(m_pctrlBoat);
    m_pctrlSail7ToolBar->addWidget(m_pctrlBoatPolar);
    m_pctrlSail7ToolBar->addWidget(m_pctrlBoatOpp);

    connect(m_pctrlBoat,    SIGNAL(activated(int)), this, SLOT(OnSelChangeBoat(int)));
    connect(m_pctrlBoatPolar, SIGNAL(activated(int)), this, SLOT(OnSelChangeBoatPolar(int)));
    connect(m_pctrlBoatOpp,   SIGNAL(activated(int)), this, SLOT(OnSelChangeBoatOpp(int)));
}



void MainFrame::CreateStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    m_pctrlProjectName = new QLabel(" ");
    m_pctrlProjectName->setMinimumWidth(200);
    statusBar()->addPermanentWidget(m_pctrlProjectName);
}


void MainFrame::CreateToolbars()
{
    CreateSail7Toolbar();
}


void MainFrame::DeleteBoat(Boat *pThisBoat, bool bResultsOnly)
{
    if(!pThisBoat) return;

    SetSaveState(false);
    int i;

    //first remove all BoatOpps associated to the Boat
    BoatOpp * pBoatOpp;
    for (i=m_oaBoatOpp.size()-1; i>=0; i--)
    {
        pBoatOpp = m_oaBoatOpp.at(i);
        if(pBoatOpp->m_BoatName == pThisBoat->m_BoatName)
        {
            m_oaBoatOpp.removeAt(i);
            if(pBoatOpp == m_pSail7->m_pCurBoatOpp) m_pSail7->m_pCurBoatOpp = nullptr;
            delete pBoatOpp;
        }
        m_pSail7->m_pCurBoatOpp = nullptr;
    }

    //next remove all BoatPolars associated to the Boat
    BoatPolar* pBoatPolar;
    for (i=m_oaBoatPolar.size()-1; i>=0; i--)
    {
        pBoatPolar = m_oaBoatPolar.at(i);
        if (pBoatPolar->m_BoatName == pThisBoat->m_BoatName)
        {
            if(!bResultsOnly)
            {
                m_oaBoatPolar.removeAt(i);
                if(pBoatPolar == m_pSail7->m_pCurBoatPolar)
                {
                    m_pSail7->m_pCurBoatPolar = nullptr;
                }
                delete pBoatPolar;
            }
            else
            {
                pBoatPolar->ResetBoatPlr();
            }
        }
    }

    if(bResultsOnly) return;// that's enough

    // ... Find the Boat in the object array and remove it...
    Boat* pBoat;
    for (i=m_oaBoat.size()-1; i>=0; i--)
    {
        pBoat = m_oaBoat.at(i);
        if (pBoat == pThisBoat)
        {
            m_oaBoat.removeAt(i);
            delete pBoat;
            if(pBoat == m_pSail7->m_pCurBoat)    m_pSail7->m_pCurBoat = nullptr;
            break;
        }
    }
}


void MainFrame::DeleteProject()
{
    // clear everything
    int i;
    Boat *pBoat;
    for (i=m_oaBoat.size()-1; i>=0; i--)
    {
        pBoat = m_oaBoat.at(i);
        m_oaBoat.removeAt(i);
        delete pBoat;
    }
    BoatPolar *pBtPolar;
    for (i=m_oaBoatPolar.size()-1; i>=0; i--)
    {
        pBtPolar = m_oaBoatPolar.at(i);
        m_oaBoatPolar.removeAt(i);
        delete pBtPolar;
    }
    BoatOpp *pBtOpp;
    for (i=m_oaBoatOpp.size()-1; i>=0; i--)
    {
        pBtOpp = m_oaBoatOpp.at(i);
        m_oaBoatOpp.removeAt(i);
        delete pBtOpp;
    }

    m_pSail7->m_pCurBoat      = nullptr;
    m_pSail7->m_pCurBoatPolar = nullptr;
    m_pSail7->m_pCurBoatOpp   = nullptr;
    UpdateBoats();
    if(m_pSail7->m_iView==SAILPOLARVIEW)    m_pSail7->CreateBoatPolarCurves();

    SetProjectName("");
    SetSaveState(true);
}





QColor MainFrame::GetColor(int type)
{
    //type
    // 0=BatPolar
    // 1=Boat Opp
    bool bFound = false;


    switch (type)
    {
        case 0:
        {
            BoatPolar *pBoatPolar;
            for (int j=0; j<MAXCOLORS; j++)
            {
                for (int i=0; i<m_oaBoatPolar.size(); i++)
                {
                    pBoatPolar = m_oaBoatPolar.at(i);
                    bFound = false;
                    if(pBoatPolar->m_Color == m_crColors[j])
                    {
                        bFound = true;
                        break;
                    }
                }
                if(!bFound)
                    return m_crColors[j];
            }
            return m_crColors[m_oaBoatPolar.size()%MAXCOLORS];
        }
        case 1:
        {
            BoatOpp *pBoatOpp;
            for (int j=0; j<MAXCOLORS; j++)
            {
                for (int i=0; i<m_oaBoatOpp.size(); i++)
                {
                    pBoatOpp = m_oaBoatOpp.at(i);
                    bFound = false;
                    if(pBoatOpp->m_Color == m_crColors[j])
                    {
                        bFound = true;
                        break;
                    }
                }
                if(!bFound)
                    return m_crColors[j];
            }
            return m_crColors[m_oaBoatOpp.size()%MAXCOLORS];
        }

        default:
        {
            return QColor(255,0,0);
        }
    }
}


void MainFrame::keyPressEvent(QKeyEvent *event)
{
    if(m_pSail7)
    {
        m_pSail7->keyPressEvent(event);
    }
    else
    {
        switch (event->key())
        {
            case Qt::Key_L:
            {
                OnLogFile();
                break;
            }
            case Qt::Key_Control:
            {
                m_pSail7->m_bArcball = true;
                UpdateView();
                break;
            }
            default:
                event->ignore();
        }
    }
}


void MainFrame::keyReleaseEvent(QKeyEvent *event)
{
    if(m_pSail7)
    {
        /*        if (event->key()==Qt::Key_Control)
        {
            pSail7->m_bArcball = false;
            UpdateView();
        }
        else*/
        m_pSail7->keyReleaseEvent(event);
    }
}


bool MainFrame::LoadSettings()
{
    QPoint pt;
    bool bFloat;
    int SettingsFormat;
    QSize size;

#ifdef Q_WS_MAC
    QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Sail7");
#else
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Sail7");
#endif

    settings.beginGroup("MainFrame");
    {
        SettingsFormat = settings.value("SettingsFormat").toInt();
        if(SettingsFormat != SETTINGSFORMAT) return false;

        m_StyleName = settings.value("StyleName","").toString();
        //        if(!m_StyleName.length()) return false;

        m_GraphExportFilter = settings.value("GraphExportFilter",".csv").toString();

        m_LanguageFilePath = settings.value("LanguageFilePath").toString();


        bFloat  = settings.value("Sail7_Float").toBool();
        pt.rx() = settings.value("Sail7_x").toInt();
        pt.ry() = settings.value("Sail7_y").toInt();
        size    = settings.value("Sail7Size").toSize();
        m_pctrlSail7Widget->setFloating(bFloat);
        if(bFloat) m_pctrlSail7Widget->move(pt);
        m_pctrlSail7Widget->resize(size);

        m_LastDirName       = settings.value("LastDirName").toString();
        m_ImageDirName      = settings.value("ImageDirName").toString();
        m_ExportLastDirName = settings.value("ExportLastDirName").toString();
        m_XMLPath           = settings.value("XMLPath").toString();
        m_LengthUnit  = settings.value("LengthUnit").toInt();
        m_AreaUnit    = settings.value("AreaUnit").toInt();
        m_WeightUnit  = settings.value("WeightUnit").toInt();
        m_SpeedUnit   = settings.value("SpeedUnit").toInt();
        m_ForceUnit   = settings.value("ForceUnit").toInt();
        m_MomentUnit  = settings.value("MomentUnit").toInt();
        m_BackgroundColor.setRed(settings.value("BackgroundColorRed").toInt());
        m_BackgroundColor.setGreen(settings.value("BackgroundColorGreen").toInt());
        m_BackgroundColor.setBlue(settings.value("BackgroundColorBlue").toInt());
        m_TextColor.setRed(settings.value("TextColorRed").toInt());
        m_TextColor.setGreen(settings.value("TextColorGreen").toInt());
        m_TextColor.setBlue(settings.value("TextColorBlue").toInt());

        m_TextFont.setFamily(settings.value("TextFontFamily", "Courier").toString());
        m_TextFont.setPointSize(settings.value("TextFontPointSize", 10).toInt());
        m_TextFont.setStyleStrategy(QFont::OpenGLCompatible);

        m_ImageFormat = settings.value("ImageFormat").toInt();
        m_DlgPos.rx() = settings.value("DlgPos_x").toInt();
        m_DlgPos.ry() = settings.value("DlgPos_y").toInt();

        QString RecentF,strange;
        m_RecentFiles.clear();
        int n=0;
        do
        {
            RecentF = QString("RecentFile_%1").arg(n);
            strange = settings.value(RecentF).toString();
            if(strange.length())
            {
                m_RecentFiles.append(strange);
                n++;
            }
            else break;
        }while(n<MAXRECENTFILES);

        m_bReverseZoom = settings.value("ReverseZoom").toBool();
        m_bAlphaChannel = settings.value("AlphaChannel").toBool();
    }

    return true;
}


MainFrame* MainFrame::self()
{
    if (!_self) {
        _self = new MainFrame(nullptr, nullptr);
    }
    return _self;
}


bool MainFrame::LoadFile(QString PathName)
{
    QFile XFile(PathName);
    if (!XFile.open(QIODevice::ReadOnly))
    {
        QString strange = tr("Could not read the file\n")+PathName;
        QMessageBox::information(window(), tr("Info"), strange);
        return false;
    }
    QString end;
    //QFileInfo fileinfo(XFile);
    end = PathName.right(4);
    end = end.toLower();
    //QString dir = fileinfo.canonicalPath();

    int pos = PathName.lastIndexOf("/");
    if(pos>0) m_LastDirName = PathName.left(pos);

    if(end==".sl7")
    {
        if(!m_bSaved)
        {
            QString strong = tr("Save the current project ?");
            int resp =  QMessageBox::question(this ,tr("Save"), strong,  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if(resp==QMessageBox::Cancel)
            {
                XFile.close();
                return 0;
            }
            else if (resp==QMessageBox::Yes)
            {
                if(!SaveProject(m_FileName))
                {
                    XFile.close();
                    return false;
                }
            }
        }

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        DeleteProject();

        QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
        ar.setVersion(QDataStream::Qt_4_5);
#endif
        ar.setByteOrder(QDataStream::LittleEndian);
        if(SerializeProject(ar, false))
        {
        }
        else
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this,tr("Warning"), tr("Error reading the file"));
            DeleteProject();
            XFile.close();
            return false;
        }
        QApplication::restoreOverrideCursor();

        AddRecentFile(PathName);
        SetSaveState(true);
        SetProjectName(PathName);

        XFile.close();

        if(m_oaBoat.size()) return true;
    }

    return true;
}



void MainFrame::OnExportCurGraph()
{
    QGraph *pGraph = nullptr;

    pGraph = m_pSail7->m_pCurGraph;

    if(!pGraph) return;

    QString FileName;

    pGraph->GetGraphName(FileName);
    FileName.replace("/", " ");
    FileName = QFileDialog::getSaveFileName(this, tr("Export Graph"), m_ExportLastDirName,
                                            tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
                                            &m_GraphExportFilter);
    if(!FileName.length()) return;

    int pos, type;
    pos = FileName.lastIndexOf("/");
    if(pos>0) m_ExportLastDirName = FileName.left(pos);

    QFile XFile(FileName);

    if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;
    pos = FileName.indexOf(".csv");
    if(pos>0) type=2; else type=1;

    pGraph->ExportToFile(XFile, type);
}



void MainFrame::OnInsertProject()
{
    QString PathName;

    PathName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                            m_LastDirName,
                                            tr("Project file (*.sl7)"));
    if(!PathName.length())        return;
    int pos = PathName.lastIndexOf("/");
    if(pos>0) m_LastDirName = PathName.left(pos);


    QFile XFile(PathName);
    if (!XFile.open(QIODevice::ReadOnly))
    {
        QString strange = tr("Could not read the file\n") + PathName;
        QMessageBox::information(window(), tr("Warning"), strange);
        return;
    }

    QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
    ar.setVersion(QDataStream::Qt_4_5);
#endif
    ar.setByteOrder(QDataStream::LittleEndian);

    SerializeProject(ar, false);

    SetSaveState(false);

    UpdateBoats();
    m_pSail7->SetBoat();

    UpdateView();
}



void MainFrame::OnLanguage()
{
    TranslatorDlg dlg;
    QDir TranslationsDir;
#ifdef Q_WS_MAC
    TranslationsDir.setPath(qApp->applicationDirPath());
#endif
#ifdef Q_WS_WIN
    TranslationsDir.setPath(qApp->applicationDirPath());
#endif
#ifdef Q_WS_X11
    TranslationsDir.setPath(qApp->applicationDirPath());
#endif

    dlg.m_TranslationDirPath = TranslationsDir.canonicalPath() + "/translations" ;
    dlg.m_LanguageFilePath = m_LanguageFilePath;
    dlg.InitDialog();
    dlg.move(m_DlgPos);
    if(dlg.exec()==QDialog::Accepted)
    {
        m_LanguageFilePath = dlg.m_LanguageFilePath;
    }
    m_DlgPos = dlg.pos();
}



void MainFrame::OnLoadFile()
{
    QString PathName;

    PathName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                            m_LastDirName,
                                            tr("Sail7 file (*.sl7)"));
    if(!PathName.length()) return;
    int pos = PathName.lastIndexOf("/");
    if(pos>0) m_LastDirName = PathName.left(pos);

    if(!LoadFile(PathName))
    {

    }

    m_pSail7->SetBoat();
    UpdateBoats();

    m_pSail7->Set2DScale();
    m_pSail7->SetControls();
    UpdateView();
}



void MainFrame::OnLoadLastFile()
{
    if(!m_RecentFiles.size()) return;
    QString PathName;
    PathName = m_RecentFiles.at(0);

    if(!PathName.length()) return;
    int pos = PathName.lastIndexOf("/");
    if(pos>0) m_LastDirName = PathName.left(pos);

    if(!LoadFile(PathName))
    {
    }

    m_pSail7->SetBoat();
    UpdateBoats();

    m_pSail7->Set2DScale();
    m_pSail7->SetControls();
    UpdateView();
}


void MainFrame::OnLogFile()
{
    QString FileName = QDir::tempPath() + "/sail7.log";
    qDebug()<< FileName;
    // 20090605 Francesco Meschia
    qDebug()<<QDesktopServices::openUrl(QUrl::fromLocalFile(FileName));
    qDebug()<<QDesktopServices::openUrl(QUrl(FileName, QUrl::TolerantMode));
}



void MainFrame::OnNewProject()
{
    m_pSail7->m_bArcball = false;
    if(!m_bSaved)
    {
        int resp = QMessageBox::question(window(), tr("Question"), tr("Save the current project ?"),
                                         QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if (QMessageBox::Cancel == resp)
        {
            return;
        }
        else if (QMessageBox::Yes == resp)
        {
            if(SaveProject(m_FileName))
            {
                SetSaveState(true);
                statusBar()->showMessage(tr("The project ") + m_ProjectName + tr(" has been saved"));
            }
            else return; //save failed, don't close
        }
        else if (QMessageBox::No == resp)
        {
            DeleteProject();
        }
    }
    else
    {
        DeleteProject();
    }

    m_pSail7->SetControls();

    UpdateView();
}


void MainFrame::OnResetCurGraphScales()
{
    QGraph *pGraph = nullptr;


    if(!pGraph) return;

    pGraph->SetAuto(true);
    UpdateView();
}




void MainFrame::OnResetSettings()
{
    int resp = QMessageBox::question(this, tr("Default Settings"), tr("Are you sure you want to reset the default settings ?"),
                                     QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
    if(resp == QMessageBox::Yes)
    {
        QMessageBox::warning(this,tr("Default Settings"), tr("The settings will be reset at the next session"));
#ifdef Q_WS_MAC
        QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Sail7");
#else
        QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Sail7");
#endif
        settings.clear();
        m_LastDirName = QDir::homePath();
        // do not save on exit
        m_bSaveSettings = false;
    }
}


void MainFrame::OnRestoreToolbars()
{
    HideToolbars();
    HideControlWidgets();

    m_pctrlSail7Widget->show();
    m_pctrlSail7ToolBar->show();
}


void MainFrame::OnSaveProject()
{
    if (!m_ProjectName.length() || m_ProjectName=="*")
    {
        OnSaveProjectAs();
        return;
    }
    if(SaveProject(m_FileName))
    {
        SetSaveState(true);
        statusBar()->showMessage(tr("The project ") + m_ProjectName + tr(" has been saved"));
    }
    m_pSail7->m_bArcball = false;
    m_pSail7->UpdateView();
}



bool MainFrame::OnSaveProjectAs()
{
    if(SaveProject())
    {
        SetProjectName(m_FileName);
        AddRecentFile(m_FileName);
        statusBar()->showMessage(tr("The project ") + m_ProjectName + tr(" has been saved"));
        SetSaveState(true);
    }
    m_pSail7->m_bArcball = false;
    return true;
}


void MainFrame::OnSaveViewToImageFile()
{
    m_pSail7->m_bArcball = false;

    QSize sz(m_p2DWidget->geometry().width(), m_p2DWidget->geometry().height());
    QImage img(sz, QImage::Format_RGB32);
    QPainter painter(&img);
    QString FileName, Filter;

    switch(m_ImageFormat)
    {
        case 0 :
        {
            Filter = "Portable Network Graphics (*.png)";
            break;
        }
        case 1 :
        {
            Filter = "JPEG (*.jpg)";
            break;
        }
        case 2 :
        {
            Filter = "Windows Bitmap (*.bmp)";
            break;
        }
    }

    FileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                            m_ImageDirName,
                                            "Portable Network Graphics (*.png);;JPEG (*.jpg);;Windows Bitmap (*.bmp)",
                                            &Filter);

    if(!FileName.length()) return;

    int pos = FileName.lastIndexOf("/");
    if(pos>0) m_ImageDirName = FileName.left(pos);

    if(Filter == "Portable Network Graphics (*.png)")
    {
        if(FileName.right(4)!=".png") FileName+= ".png";
        m_ImageFormat = 0;
    }
    else if(Filter == "JPEG (*.jpg)")
    {
        if(FileName.right(4)!=".jpg") FileName+= ".jpg";
        m_ImageFormat = 1;
    }
    else if(Filter == "Windows Bitmap (*.bmp)")
    {
        if(FileName.right(4)!=".bmp") FileName+= ".bmp";
        m_ImageFormat = 2;
    }

    m_pSail7->m_bArcball = false;

    if(m_pSail7->m_iView==SAIL3DVIEW)
    {
        m_pSail7->UpdateView();
        m_pSail7->SnapClient(FileName);
        return;
    }
    else m_pSail7->PaintView(painter);

    img.save(FileName);
}




void MainFrame::OnSelChangeBoat(int i)
{
    // Gets the new selected Sail name and notifies Miarex
    // then updates SailPolar combobox
    // and selects either the current SailPolar
    // or the first one in the list, if any
    //    pSAil7->StopAnimate();
    QString strong;

    if (i >=0) strong = m_pctrlBoat->itemText(i);
    m_pSail7->SetBoat(strong);

    UpdateBoatPolars();
    m_pSail7->SetBoatPolar();
    m_pSail7->m_bIs2DScaleSet = false;
    m_pSail7->Set2DScale();
    m_pSail7->SetControls();
    m_pSail7->UpdateView();

}


void MainFrame::OnSelChangeBoatPolar(int i)
{
    m_pSail7->StopAnimate();

    QString strong;

    if (i>=0) strong = m_pctrlBoatPolar->itemText(i);
    m_pSail7->SetBoatPolar(nullptr, strong);

    UpdateBoatOpps();
    m_pSail7->SetBoatOpp(true);
    m_pSail7->SetControls();
    m_pSail7->UpdateView();
}


void MainFrame::OnSelChangeBoatOpp(int i)
{
    QString strong;
    m_pSail7->StopAnimate();

    // Gets the new selected BoatOpp name and notifies Miarex
    if(!m_pctrlBoatOpp->count())
    {
        m_pSail7->m_pCurBoatOpp = nullptr;
        m_pSail7->UpdateView();
        return;
    }

    if (i>=0) strong = m_pctrlBoatOpp->itemText(i);
    else
    {
        m_pctrlBoatOpp->setCurrentIndex(0);
        strong = m_pctrlBoatOpp->itemText(0);
    }

    if(strong.length())
    {
        bool bOK;
        double x = strong.toDouble(&bOK);
        if(bOK)
        {
            m_pSail7->SetBoatOpp(false, x);
            m_pSail7->UpdateView();
        }
        else
        {
            QMessageBox::warning(window(), tr("Warning"), tr("Unidentified Operating Point"));
            m_pSail7->m_pCurBoatOpp = nullptr;
            m_pSail7->SetBoatOpp(true);
        }
    }
}


void MainFrame::OnStyle()
{
    m_DisplaySettingsDlg.move(m_DlgPos);
    m_DisplaySettingsDlg.m_BackgroundColor = m_BackgroundColor;
    m_DisplaySettingsDlg.m_TextColor       = m_TextColor;
    m_DisplaySettingsDlg.m_TextFont        = m_TextFont;
    m_DisplaySettingsDlg.m_pRefGraph       = &m_RefGraph;
    m_DisplaySettingsDlg.m_StyleName       = m_StyleName;
    m_DisplaySettingsDlg.m_bReverseZoom    = m_bReverseZoom;
    m_DisplaySettingsDlg.m_bAlphaChannel   = m_bAlphaChannel;
    m_DisplaySettingsDlg.InitDialog();

    if(m_DisplaySettingsDlg.exec() ==QDialog::Accepted)
    {
        m_BackgroundColor = m_DisplaySettingsDlg.m_BackgroundColor;
        m_TextColor       = m_DisplaySettingsDlg.m_TextColor;
        m_TextFont        = m_DisplaySettingsDlg.m_TextFont;
        m_StyleName       = m_DisplaySettingsDlg.m_StyleName;
        m_bReverseZoom    = m_DisplaySettingsDlg.m_pctrlReverseZoom->isChecked();
        m_bAlphaChannel   = m_DisplaySettingsDlg.m_pctrlAlphaChannel->isChecked();
        m_pSail7->m_bResetglSailGeom = true;
        m_pSail7->m_bResetglLegend = true;
        m_p3DWidget->initializeGL();

        if(m_DisplaySettingsDlg.m_bIsGraphModified)
        {
            SetGraphSettings(&m_RefGraph);
        }
    }
    m_DlgPos = m_DisplaySettingsDlg.pos();

    UpdateView();
}


void MainFrame::OnUnits()
{
    m_UnitsDlg.move(m_DlgPos);
    m_UnitsDlg.m_Length = m_LengthUnit;
    m_UnitsDlg.m_Area   = m_AreaUnit;
    m_UnitsDlg.m_Weight = m_WeightUnit;
    m_UnitsDlg.m_Speed  = m_SpeedUnit;
    m_UnitsDlg.m_Force  = m_ForceUnit;
    m_UnitsDlg.m_Moment = m_MomentUnit;
    m_UnitsDlg.InitDialog();

    if(m_UnitsDlg.exec()==QDialog::Accepted)
    {
        m_LengthUnit = m_UnitsDlg.m_Length;
        m_AreaUnit   = m_UnitsDlg.m_Area;
        m_WeightUnit = m_UnitsDlg.m_Weight;
        m_SpeedUnit  = m_UnitsDlg.m_Speed;
        m_ForceUnit  = m_UnitsDlg.m_Force;
        m_MomentUnit = m_UnitsDlg.m_Moment;

        SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
                 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);

        SetSaveState(false);

        m_pSail7->UpdateUnits();
    }
    m_DlgPos = m_UnitsDlg.pos();
}




void MainFrame::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    if(!LoadFile(action->data().toString()))
    {
        m_RecentFiles.removeOne(action->data().toString());
        CreateMainMenu();
    }

    m_pSail7->Set2DScale();
    OnSail7();
}




bool MainFrame::SaveProject(QString PathName)
{
    QString Filter = "Sail7 v0.01 Project File (*.sl7)";
    QString FileName = m_ProjectName;

    if(!PathName.length())
    {
        if(FileName.right(1)=="*")     FileName = FileName.left(FileName.length()-1);
        FileName.replace("/", " ");


        PathName = QFileDialog::getSaveFileName(this, tr("Save the Project File"),
                                                m_LastDirName+"/"+FileName,
                                                "Sail7 Project File (*.sl7)",
                                                &Filter);

        if(!PathName.length()) return false;//nothing more to do
        int pos = PathName.indexOf(".sl7", Qt::CaseInsensitive);
        if(pos<0) PathName += ".sl7";
        pos = PathName.lastIndexOf("/");
        if(pos>0) m_LastDirName = PathName.left(pos);
    }

    QFile fp(PathName);

    if (!fp.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(window(), tr("Warning"), tr("Could not open the file for writing"));
        return false;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QDataStream ar(&fp);
#if QT_VERSION >= 0x040500
    ar.setVersion(QDataStream::Qt_4_5);
#endif
    ar.setByteOrder(QDataStream::LittleEndian);

    SerializeProject(ar,true);
    m_FileName = PathName;
    fp.close();

    SaveSettings();
    QApplication::restoreOverrideCursor();
    return true;
}


void MainFrame::SaveSettings()
{
    if(!m_bSaveSettings) return;
#ifdef Q_WS_MAC
    QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"Sail7");
#else
    QSettings settings(QSettings::IniFormat,QSettings::UserScope,"Sail7");
#endif
    settings.beginGroup("MainFrame");
    {
        settings.setValue("SettingsFormat", SETTINGSFORMAT);
        settings.setValue("FrameGeometryx", frameGeometry().x());
        settings.setValue("FrameGeometryy", frameGeometry().y());
        settings.setValue("SizeWidth", size().width());
        settings.setValue("SizeHeight", size().height());
        settings.setValue("SizeMaximized", isMaximized());
        settings.setValue("StyleName", m_StyleName);
        settings.setValue("GraphExportFilter", m_GraphExportFilter);
        settings.setValue("LanguageFilePath", m_LanguageFilePath);
        settings.setValue("Sail7_Float", m_pctrlSail7Widget->isFloating());
        settings.setValue("Sail7_x", m_pctrlSail7Widget->frameGeometry().x());
        settings.setValue("Sail7_y", m_pctrlSail7Widget->frameGeometry().y());
        settings.setValue("Sail7Size", m_pctrlSail7Widget->size());
        settings.setValue("LastDirName", m_LastDirName);
        settings.setValue("ImageDirName", m_ImageDirName);
        settings.setValue("ExportLastDirName", m_ExportLastDirName);
        settings.setValue("XMLPath", m_XMLPath);
        settings.setValue("LengthUnit", m_LengthUnit);
        settings.setValue("AreaUnit", m_AreaUnit);
        settings.setValue("WeightUnit", m_WeightUnit);
        settings.setValue("SpeedUnit", m_SpeedUnit);
        settings.setValue("ForceUnit", m_ForceUnit);
        settings.setValue("MomentUnit", m_MomentUnit);
        settings.setValue("BackgroundColorRed", m_BackgroundColor.red());
        settings.setValue("BackgroundColorGreen", m_BackgroundColor.green());
        settings.setValue("BackgroundColorBlue", m_BackgroundColor.blue());
        settings.setValue("TextColorRed", m_TextColor.red());
        settings.setValue("TextColorGreen", m_TextColor.green());
        settings.setValue("TextColorBlue", m_TextColor.blue());
        settings.setValue("TextFontFamily", m_TextFont.family());
        settings.setValue("TextFontPointSize", m_TextFont.pointSize());
        settings.setValue("ImageFormat", m_ImageFormat);
        settings.setValue("DlgPos_x", m_DlgPos.x());
        settings.setValue("DlgPos_y", m_DlgPos.y());
        settings.setValue("RecentFileSize", m_RecentFiles.size());
        settings.setValue("ReverseZoom", m_bReverseZoom);
        settings.setValue("AlphaChannel", m_bAlphaChannel);

        QString RecentF;
        for(int i=0; i<m_RecentFiles.size() && i<MAXRECENTFILES; i++)
        {
            RecentF = QString("RecentFile_%1").arg(i);
            if(m_RecentFiles[i].length()) settings.setValue(RecentF, m_RecentFiles.at(i));
            else                          settings.setValue(RecentF, "");
        }
        for(int i=m_RecentFiles.size(); i<MAXRECENTFILES; i++)
        {
            RecentF = QString("RecentFile_%1").arg(i);
            settings.setValue(RecentF, "");
        }
    }
    settings.endGroup();


    m_pSail7->SaveSettings(&settings);

    m_pGL3DScales->SaveSettings(&settings);
    m_RefGraph.SaveSettings(&settings);
}


void MainFrame::SetCentralWidget()
{
    if(m_pSail7->m_iView==SAILPOLARVIEW)
    {
        m_pctrlCentralWidget->setCurrentIndex(0);
    }
    else if(m_pSail7->m_iView==SAIL3DVIEW)
    {
        m_pctrlCentralWidget->setCurrentIndex(1);
    }

    else m_pctrlCentralWidget->setCurrentIndex(0);
}



void MainFrame::SelectBoatOpp(double x)
{
    //Selects pOpp in the combobox and returns true
    //On error, selects the first and returns false
    double alpha;

    for(int i=0; i<m_pctrlBoatOpp->count(); i++)
    {
        alpha = m_pctrlBoatOpp->itemText(i).toDouble();
        if(fabs(alpha-x)<0.001)
        {
            m_pctrlBoatOpp->setCurrentIndex(i);
        }
    }
}


bool MainFrame::SerializeProject(QDataStream &ar, bool bIsStoring)
{
    int n;
    if (bIsStoring)
    {
        // storing code
        ar << 100030;
        // 100030; tentative format for Sail7 objects
        ar << m_LengthUnit;
        ar << m_AreaUnit;
        ar << m_WeightUnit;
        ar << m_SpeedUnit;
        ar << m_ForceUnit;
        ar << m_MomentUnit;


        //serialize Sail7 objects
        ar << m_oaBoat.size();
        for(int ib=0; ib<m_oaBoat.size(); ib++)
        {
            Boat* pBoat = m_oaBoat.at(ib);
            if(!pBoat->SerializeBoat(ar, true)) return false;
        }

        //serialize boat polars
        ar << m_oaBoatPolar.size();
        for(int ib=0; ib<m_oaBoatPolar.size(); ib++)
        {
            BoatPolar* pBoatPolar = m_oaBoatPolar.at(ib);
            if(!pBoatPolar->SerializeBoatPlr(ar, true)) return false;
        }

        //serialize boat opps
        ar << m_oaBoatOpp.size();
        for(int ib=0; ib<m_oaBoatOpp.size(); ib++)
        {
            BoatOpp* pBoatOpp = m_oaBoatOpp.at(ib);
            if(!pBoatOpp->SerializeBoatOpp(ar, true)) return false;
        }

        return true;
    }
    else
    {
        // LOADING CODE

        int ArchiveFormat;
        ar >> n; //n is the ArchiveFormat number
        ArchiveFormat = n;
        ar >> m_LengthUnit;
        ar >> m_AreaUnit;
        ar >> m_WeightUnit;
        ar >> m_SpeedUnit;
        ar >> m_ForceUnit;
        ar >> m_MomentUnit;

        SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
                 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);


        //serialize Sail7 objects
        if(ArchiveFormat>=100030)
        {
            //serialize boats
            ar >>n;
            for(int ib=0; ib<n; ib++)
            {
                Boat* pBoat= new Boat;
                if(!pBoat->SerializeBoat(ar, false)) return false;
                m_oaBoat.append(pBoat);
            }

            //serialize boat polars
            ar >> n ;
            for(int ib=0; ib<n; ib++)
            {
                BoatPolar* pBoatPolar= new BoatPolar;
                if(!pBoatPolar->SerializeBoatPlr(ar, false)) return false;
                m_oaBoatPolar.append(pBoatPolar);
            }
            //serialize boat oppss
            ar >> n ;
            for(int ib=0; ib<n; ib++)
            {
                BoatOpp* pBoatOpp= new BoatOpp;
                if(!pBoatOpp->SerializeBoatOpp(ar, false)) return false;
                m_oaBoatOpp.append(pBoatOpp);
            }
        }

        return true;
    }
}



void MainFrame::SetMenus()
{
    menuBar()->clear();
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(Sail7ViewMenu);
    menuBar()->addMenu(Sail7ObjectMenu);
    menuBar()->addMenu(Sail7PlrMenu);
    menuBar()->addMenu(Sail7OppMenu);
    menuBar()->addMenu(optionsMenu);
    menuBar()->addMenu(helpMenu);
}




void MainFrame::SetProjectName(QString PathName)
{
    m_FileName = PathName;
    int pos = PathName.lastIndexOf("/");
    if (pos>0) m_ProjectName = PathName.right(PathName.length()-pos-1);
    else m_ProjectName = PathName;
    if(m_ProjectName.length()>4)
    {
        m_ProjectName = m_ProjectName.left(m_ProjectName.length()-4);
        m_pctrlProjectName->setText(m_ProjectName);
    }
}


void MainFrame::SetSaveState(bool bSaved)
{
    m_bSaved = bSaved;

    int len = m_ProjectName.length();
    if(m_ProjectName.right(1)=="*") m_ProjectName = m_ProjectName.left(len-1);
    if (!bSaved)
    {
        m_ProjectName += "*";
    }
    m_pctrlProjectName->setText(m_ProjectName);
}


void MainFrame::SetGraphSettings(Graph *pGraph)
{
    for(int ig=0; ig<4; ig++)
    {
        m_pSail7->m_BoatGraph[ig].CopySettings(pGraph, false);
    }
}


QString MainFrame::ShortenFileName(QString &PathName)
{
    QString strong, strange;
    if(PathName.length()>60)
    {
        int pos = PathName.lastIndexOf('/');
        if (pos>0)
        {
            strong = '/'+PathName.right(PathName.length()-pos-1);
            strange = PathName.left(60-strong.length()-6);
            pos = strange.lastIndexOf('/');
            if(pos>0) strange = strange.left(pos)+"/...  ";
            strong = strange+strong;
        }
        else
        {
            strong = PathName.left(40);
        }
    }
    else strong = PathName;
    return strong;
}



void MainFrame::UpdateBoats()
{
    // fills combobox with wing and plane names
    // then selects the current wing or plane, if any
    // else selects the first, if any
    // else disables the combobox
    int i;
    QString strong;

    Boat *pBoat=nullptr, *pCurBoat=nullptr;

    m_pctrlBoat->clear();

    pCurBoat  = m_pSail7->m_pCurBoat;

    QStringList BoatNames;
    for (i=0; i<m_oaBoat.size(); i++)
    {
        pBoat = m_oaBoat[i];
        BoatNames.append(pBoat->m_BoatName);
    }
    BoatNames.sort();
    m_pctrlBoat->addItems(BoatNames);

    if (m_pctrlBoat->count())
    {
        m_pctrlBoat->setEnabled(true);
        //select the current Sail, if any...
        if(pCurBoat)
        {
            int pos = m_pctrlBoat->findText(pCurBoat->m_BoatName);
            if(pos>=0)
            {
                m_pctrlBoat->setCurrentIndex(pos);
            }
            else
            {
                // if error, select the first
                m_pctrlBoat->setCurrentIndex(0);
                strong = m_pctrlBoat->itemText(0);
            }
        }
        //... else select the first
        else
        {
            m_pctrlBoat->setCurrentIndex(0);
            strong = m_pctrlBoat->itemText(0);
        }
    }
    else
    {
        m_pctrlBoat->clear();
        m_pctrlBoat->setEnabled(false);

        m_pSail7->m_pCurBoat      = nullptr;
        m_pSail7->m_pCurBoatPolar = nullptr;
        m_pSail7->m_pCurBoatOpp   = nullptr;
    }

    UpdateBoatPolars();
}


void MainFrame::UpdateBoatPolars()
{
    //    fills the combobox with BoatPolar names associated to Sail7's current wing
    //    then selects Sail7 current BoatPolar if any, else selects the first, if any
    //    else disables the combobox
    BoatPolar *pBoatPolar;
    QString BoatName;
    int i;

    m_pctrlBoatPolar->clear();

    Boat  *pCurBoat = m_pSail7->m_pCurBoat;
    BoatPolar *pCurBoatPolar  = m_pSail7->m_pCurBoatPolar;

    if(pCurBoat)      BoatName = pCurBoat->m_BoatName;
    else              BoatName = "";

    if(!BoatName.length())
    {
        m_pSail7->m_pCurBoatPolar = nullptr;
        //        pSail7->SetBoatPolar();
        m_pctrlBoatPolar->setEnabled(false);
        UpdateBoatOpps();
        return;
    }

    int size = 0;
    //count the number of BoatPolars associated to the current Wing
    for (i=0; i<m_oaBoatPolar.size(); i++)
    {
        pBoatPolar = m_oaBoatPolar[i];
        if(pBoatPolar->m_BoatName == BoatName) size++;
    }
    if(size)
    {
        // if any
        m_pctrlBoatPolar->setEnabled(true);
        for (i=0; i<m_oaBoatPolar.size(); i++)
        {
            pBoatPolar = m_oaBoatPolar[i];
            if(pBoatPolar->m_BoatName == BoatName)
            {
                m_pctrlBoatPolar->addItem(pBoatPolar->m_BoatPolarName);
            }
        }
        //select the current Wing Polar, if any...
        if(pCurBoatPolar)
        {
            int pos = m_pctrlBoatPolar->findText(pCurBoatPolar->m_BoatPolarName);
            if(pos>=0) m_pctrlBoatPolar->setCurrentIndex(pos);
            else
            {
                // if error, select the first
                m_pctrlBoatPolar->setCurrentIndex(0);
                //                m_pctrlBoatPolar->GetLBText(0, strong);
                //                m_pSail7->SetBoatPolar(false, strong);
            }
        }
        //... else select the first
        else
        {
            m_pctrlBoatPolar->setCurrentIndex(0);
            //            m_pctrlBoatPolar->GetLBText(0, strong);
            //            m_pSail7->SetBoatPolar(false, strong);
        }

    }
    else
    {
        // otherwise disable control
        m_pctrlBoatPolar->setEnabled(false);
        m_pSail7->m_pCurBoatPolar = nullptr;
        m_pSail7->m_pCurBoatOpp = nullptr;
        //        m_pSail7->SetBoatPolar();
    }
}


void MainFrame::UpdateBoatOpps()
{
    // fills the combobox with BoatOpp names associated to Miarex' current BoatPlr
    // then selects Sail7 current BoatOpp if any, else selects the first, if any
    // else disables the combobox

    BoatOpp *pBoatOpp = nullptr;

    QString str;
    m_pctrlBoatOpp->clear();

    Boat   *pCurBoat= m_pSail7->m_pCurBoat;
    BoatPolar *pCurBoatPlr    = m_pSail7->m_pCurBoatPolar;

    if (!pCurBoat || !pCurBoat->m_BoatName.length() || !pCurBoatPlr || !pCurBoatPlr->m_BoatPolarName.length())
    {
        m_pctrlBoatOpp->setEnabled(false);
        m_pSail7->m_pCurBoatOpp = nullptr;
        return;
    }

    for (int i=0; i<m_oaBoatOpp.size(); i++)
    {
        pBoatOpp = m_oaBoatOpp[i];
        if (pBoatOpp->m_BoatName == pCurBoat->m_BoatName && pBoatOpp->m_BoatPolarName == pCurBoatPlr->m_BoatPolarName)
        {
            str = QString("%1").arg(pBoatOpp->m_Ctrl,8,'f',3);
            m_pctrlBoatOpp->addItem(str);
        }
    }

    if(m_pSail7->m_pCurBoatOpp)
    {
        str = QString("%1").arg(m_pSail7->m_pCurBoatOpp->m_Ctrl,8,'f',3);

        int pos = m_pctrlBoatOpp->findText(str);
        if(pos >=0) m_pctrlBoatOpp->setCurrentIndex(pos);
        else        m_pctrlBoatOpp->setCurrentIndex(0);
    }
    else
    {
        m_pctrlBoatOpp->setCurrentIndex(0);
    }

    m_pctrlBoatOpp->setEnabled(m_pctrlBoatOpp->count());
}


void MainFrame::updateRecentFileActions()
{
    int numRecentFiles = qMin(m_RecentFiles.size(), MAXRECENTFILES);

    QString text;
    for (int i = 0; i < numRecentFiles; ++i)
    {
        text = tr("&%1 %2").arg(i + 1).arg(ShortenFileName(m_RecentFiles[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(m_RecentFiles[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}



void MainFrame::UpdateView()
{
    m_pSail7->UpdateView();
}



void MainFrame::SetDlgPos(QDialog &Dlg)
{
    QPoint Position = m_DlgPos;
    QDesktopWidget desk;

    if(Dlg.frameGeometry().width() +m_DlgPos.x()>desk.width())  Position.rx() += desk.width() -(Dlg.frameGeometry().width() +m_DlgPos.x());
    if(Dlg.frameGeometry().height()+m_DlgPos.y()>desk.height()) Position.ry() += desk.height()-(Dlg.frameGeometry().height()+m_DlgPos.y());

    if(m_DlgPos.x()<0) Position.rx()=0;

    if(Dlg.width()>desk.rect().width())   Position.rx()=0;
    if(Dlg.height()>desk.rect().height()) Position.ry()=0;

    Dlg.move(Position);
}



void MainFrame::OnSail7()
{
    m_p3DWidget->m_iView=GLSAIL7VIEW;

    HideToolbars();
    HideControlWidgets();

    m_pctrlSail7ToolBar->show();
    m_pctrlSail7Widget->show();
    m_pSail7->SetControls();
    m_pSail7->SetBoat();
    m_pSail7->m_bArcball = false;
    UpdateBoats();
    SetMenus();
    SetCentralWidget();

    m_pSail7->Set3DScale();

    UpdateView();
}


void MainFrame::HideToolbars()
{
    m_pctrlSail7ToolBar->hide();
}



void MainFrame::HideControlWidgets()
{
    m_pctrlSail7Widget->hide();
}


void *MainFrame::Get3DWidget()
{
    return m_p3DWidget;
}

