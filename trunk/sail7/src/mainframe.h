/****************************************************************************

       MainFrame Class
       Copyright (C) 2012 Andre Deperrois 
       (C) All rights reserved

*****************************************************************************/

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QMainWindow>
#include <QDockWidget>
#include <QStackedWidget>
#include <QToolButton>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QPointer>
#include <QTranslator>
#include <QToolBar>
#include <QStatusBar>
#include <QList>
#include <QStringList>

#include "view/twodwidget.h"
#include "params.h"
#include "graph/qgraph.h"
#include "objects/panel.h"
#include "misc/displaysettingsdlg.h"
#include "misc/renamedlg.h"
#include "misc/unitsdlg.h"

class Sail7;
class GL3DScales;
class Boat;
class BoatPolar;
class BoatOpp;
class glSail7View;

class MainFrame : public QMainWindow
{
    friend class TwoDWidget;
    friend class ThreeDWidget;
    friend class GL3DScales;
    friend class SectionViewWidget;
    friend class BoatPolarDlg;
    friend class GL3dBodyDlg;
    friend class BoatDlg;
    friend class Sail7;
    friend class Body;
    friend class SailDlg;
    friend class SailcutSail;
    friend class NURBSSail;
    friend class glSail7View;

    Q_OBJECT

    public:
        MainFrame(QWidget *parent = nullptr, Qt::WindowFlags flags = nullptr);

        bool LoadFile(QString PathName);
        void ClientToGL(QPoint const &point, Vector3d &real);
        void DeleteProject();
        void DeleteBoat(Boat *pObj, bool bResultsOnly = false);
        void GLToClient(Vector3d const &real, QPoint &point);
        void RemoveOpPoint(bool bCurrent);
        void SaveSettings();
        void SelectBoatOpp(double x);
        void SetCentralWidget();
        void SetDlgPos(QDialog &Dlg);
        void SetGraphSettings(Graph *pGraph);
        void SetProjectName(QString PathName);
        void SetMenus();
        void SetSaveState(bool bSaved);

        void UpdateBoats();
        void UpdateBoatPolars();
        void UpdateBoatOpps();

        void UpdateView();

        bool SaveProject(QString PathName="");
        bool LoadSettings();
        bool LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
        bool SerializeProject(QDataStream &ar, bool bIsStoring);

        QColor GetColor(int type);

        //recent file
        QString ShortenFileName(QString &PathName);
        void AddRecentFile(const QString &PathNAme);
        void updateRecentFileActions();

        void HideToolbars();
        void HideControlWidgets();
        void *Get3DWidget();

        static MainFrame* self();

    public slots:
        void OnSail7();

    private slots:
        void AboutSail7();
        void OnExportCurGraph();
        void OnInsertProject();
        void OnNewProject();
        void OnLanguage();
        void OnLoadFile();
        void OnLoadLastFile();
        void OnLogFile();
        void OnResetCurGraphScales();
        void OnResetSettings();
        void OnRestoreToolbars();
        bool OnSaveProjectAs();
        void OnSaveViewToImageFile();
        void OnSelChangeBoat(int i);
        void OnSelChangeBoatOpp(int i);
        void OnSelChangeBoatPolar(int i);
        void OnSaveProject();
        void OnDisplayOptions();
        void OnUnits();
        void openRecentFile();

        /*___________________________________________Methods_______________________________*/
    private:
        void closeEvent (QCloseEvent * event);
        void contextMenuEvent (QContextMenuEvent * event) ;
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);

        void CreateDockWindows();
        void CreateToolbars();
        void CreateStatusBar();
        void CreateActions();
        void CreateMainMenu();
        void CreateSail7Menus();
        void CreateSail7Actions();
        void CreateSail7Toolbar();


        /*___________________________________________Variables_______________________________*/
    public:
        bool m_bMaximized;
        QString m_StyleName;
        QString m_VersionName;
        QString m_GraphExportFilter;

    private:
        //the dialog boxes are declared as member variables to enable translations... seems to be a Qt bug
        DisplaySettingsDlg m_DisplaySettingsDlg;
        RenameDlg m_RenameDlg;
        UnitsDlg m_UnitsDlg;

        Sail7 *m_pSail7;

        static QPointer<MainFrame> _self;

        QStackedWidget *m_pctrlCentralWidget;
        TwoDWidget   *m_p2DWidget;
        glSail7View *m_pglSail7View;

        QDockWidget *m_pctrlSail7Widget;
        QDockWidget *m_pctrl3DScalesWidget;

        QToolBar *m_pctrlSail7ToolBar;

        //Common Menus
        QMenu * MainMenu;
        QMenu *fileMenu, *optionsMenu, *helpMenu;




        //Sail7 Menus
        QMenu *Sail7ViewMenu, *Sail7ObjectMenu, *Sail7PlrMenu, *Sail7OppMenu;
        QMenu *SailPlrCtxMenu, *Sail3DCtxMenu;
        QMenu *CurBoatMenu, *CurBoatPlrMenu, *CurBoatOppMenu;
        QMenu *BoatPolarGraphMenu,*CurGraphMenu;

        //MainFrame actions
        QAction *restoreToolbarsAct, *exportCurGraphAct, *resetCurGraphScales;
        QAction *m_pOpenAct, *m_pOpenLast, *insertAct, *styleAct;
        QAction *saveAct, *saveProjectAsAct,*newProjectAct, *closeProjectAct;
        QAction *unitsAct;
        QAction *languageAct;
        QAction *exitAct;
        QAction *aboutAct, *aboutQtAct;
        QAction *recentFileActs[MAXRECENTFILES];
        QAction *separatorAct;
        QAction *saveViewToImageFileAct, *resetSettingsAct;
        QAction *viewLogFile;

        //Sail7 actions
        QAction *BoatPolarAct, *Boat3DAct;
        QAction *Sail73DLightAct, *Sail73DLightOptionsAct, *Sail73DPrefsAct, *Sail73DScalesAct;
        QAction *Sail7NewBoatAct, *Sail7EditBoatAct;
        QAction *Sail7OpenAct, *Sail7ExportAct, *Sail7ImportAct;
        QAction *renameCurBoat, *deleteCurBoat, *duplicateCurBoat;
        QAction *deleteCurBoatOpp, *deleteAllBoatOpps, * deleteAllBoatPolarOpps;
        QAction *showBoatOppProperties, *showBoatPolarProperties;
        QAction *defineBoatPolar, *editBoatPolar, *renameCurBoatPolar,*deleteCurBoatPolar, *resetCurBoatPolar;
        QAction *exportCurBoatPolar, *exportCurBoatOpp;
        QAction *hideAllBoatPlrs, *showAllBoatPlrs;
        QAction *hideCurBoatPlrs, *showCurBoatPlrs, *deleteCurBoatPlrs;
        QToolButton *m_pctrlBoat3dView, *m_pctrlBoatPolarView;

        QAction *twoGraphs, *fourGraphs;
        QAction *Graph1,*Graph2,*Graph3,*Graph4;
        QAction *allBoatPolarGraphsScalesAct,  *allBoatPolarGraphsSettings, *resetGraphLegend;

        QComboBox *m_pctrlBoat, *m_pctrlBoatPolar, *m_pctrlBoatOpp;
        QLabel *m_pctrlProjectName;


        QStringList m_RecentFiles;

        QList <Boat *> m_oaBoat;
        QList <BoatPolar *> m_oaBoatPolar;
        QList <BoatOpp *> m_oaBoatOpp;


        bool m_bSaved;
        bool m_bSaveSettings;
        bool m_bReverseZoom;                    // true if the rolling forward zooms in
        //    bool m_bSaveOpps, m_bSaveWOpps;
        //    bool m_bHighlightOpp, m_bHighlightWOpp;


        QString m_ProjectName, m_FileName, m_LanguageFilePath, m_XMLPath;
        QString m_LastDirName, m_ExportLastDirName, m_ImageDirName;
        QString m_UFOType;
        QColor m_crColors[MAXCOLORS];

        QGraph m_RefGraph;//Reference setttings
        QColor m_BorderClr;

        int m_ImageFormat;

    public:
        QFont m_TextFont;
        QColor m_TextColor;
        QColor m_BackgroundColor;
        QPoint m_DlgPos;//preferred position for dialog boxes
        GL3DScales *m_pGL3DScales;

        double m_mtoUnit;
        double m_mstoUnit;
        double m_m2toUnit;
        double m_kgtoUnit;
        double m_NtoUnit;
        double m_NmtoUnit;
        int m_LengthUnit;
        int m_AreaUnit;
        int m_WeightUnit;
        int m_SpeedUnit;
        int m_ForceUnit;
        int m_MomentUnit;
        int m_ExportFileType;
        bool m_bAlphaChannel;

        double m_aij[VLMMAXMATSIZE*VLMMAXMATSIZE];    // coefficient matrix
        double m_aijRef[VLMMAXMATSIZE*VLMMAXMATSIZE]; // coefficient matrix
        double m_RHS[VLMMAXMATSIZE*VLMMAXRHS];            // RHS vector
        double m_RHSRef[VLMMAXMATSIZE*VLMMAXRHS];        // RHS vector


        Vector3d m_Node[2*VLMMAXMATSIZE];        // the node array for the currently loaded UFO
        Vector3d m_TempWakeNode[2*VLMMAXMATSIZE];    // the temporary wake node array during relaxation calc
        Vector3d m_MemNode[2*VLMMAXMATSIZE];         // used if the analysis should be performed on the tilted geometry
        Vector3d m_WakeNode[2*VLMMAXMATSIZE];        // the reference current wake node array
        Vector3d m_RefWakeNode[2*VLMMAXMATSIZE];     // the reference wake node array if wake needs to be reset
        CPanel m_Panel[VLMMAXMATSIZE];        // the panel array for the currently loaded UFO
        CPanel m_MemPanel[VLMMAXMATSIZE];           // used if the analysis should be performed on the tilted geometry
        CPanel m_WakePanel[VLMMAXMATSIZE];          // the reference current wake panel array
        CPanel m_RefWakePanel[VLMMAXMATSIZE];       // the reference wake panel array if wake needs to be reset

};

#endif // MAINFRAME_H
