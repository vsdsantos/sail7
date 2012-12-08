/****************************************************************************

		 ScaleSailDlg Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#ifndef ScaleSailDlg_H
#define ScaleSailDlg_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include "../misc/FloatEdit.h"
#include "../objects/Sail.h"


class ScaleSailDlg : public QDialog
{
	Q_OBJECT

	friend class SailDlg;
	friend class CBody;
	friend class MainFrame;

public:
	ScaleSailDlg(void *pParent = NULL);

private slots:
	void OnOK();
	void OnEditingFinished();

private:
	void SetupLayout();
	void InitDialog(QSail *pSail);
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
	QSail * m_pSail;
};


#endif // ScaleSailDlg_H
