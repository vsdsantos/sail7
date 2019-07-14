/****************************************************************************

         ScaleSailDlg Class
         Copyright (C) 2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/


#ifndef ScaleSailDlg_H
#define ScaleSailDlg_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include "../misc/floatedit.h"
#include "../objects/sail.h"


class ScaleSailDlg : public QDialog
{
    Q_OBJECT

    friend class SailDlg;
    friend class CBody;
    friend class MainFrame;

public:
    ScaleSailDlg(void *pParent = nullptr);

private slots:
    void OnOK();
    void OnEditingFinished();

private:
    void SetupLayout();
    void InitDialog(Sail *pSail);
    void EnableControls();
    void keyPressEvent(QKeyEvent *event);

private:
    static void *s_pMainFrame;

    QPushButton *OKButton, *CancelButton;
    FloatEdit *m_pctrlXScaleFactor;
    FloatEdit *m_pctrlYScaleFactor;
    FloatEdit *m_pctrlZScaleFactor;


private:
    void *m_pSailDlg;
    double m_XFactor, m_YFactor, m_ZFactor;
    Sail * m_pSail;
};


#endif // ScaleSailDlg_H
