/****************************************************************************

		 BoatPolarDlg Class
		 Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#ifndef BOATPOLARDLG_H
#define BOATPOLARDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QTableView>
#include <QStandardItemModel>
#include "../misc/FloatEdit.h"
#include "../misc/FloatEditDelegate.h"
#include "../objects/vector3d.h"
#include "../objects/Boat.h"

class BoatPolarDlg : public QDialog
{
	Q_OBJECT
	friend class Sail7;
	friend class CBoatDef;
	friend class QSail;
	friend class MainFrame;

public:
	BoatPolarDlg();

private:
	void keyPressEvent(QKeyEvent *event);

	void SetupLayout();
	void Connect();
	void InitDialog(Boat *pBoat, BoatPolar *pBoatPolar=NULL);
	void SetDensity();
	void FillVariableList();
	void ReadData();
	bool eventFilter(QObject* o, QEvent* e);

private slots:
	void OnOK();
	void OnUnit();

private:
	//Variables
	static void *s_pMainFrame;
	static void* s_pSail7;
	static BoatPolar s_BoatPolar;

	QString m_BoatPolarName;

	Boat *m_pBoat;

	static int s_UnitType;//1= International, 2= English

	int m_NXWakePanels;
	double m_TotalWakeLength;
	double m_WakePanelFactor;



	QTableView *m_pctrlVariableTable;
	QStandardItemModel *m_pVariableModel;
	FloatEditDelegate *m_pVariableDelegate;

	QTableView *m_pctrlWindGradientTable;
	QStandardItemModel *m_pWindGradientModel;
	FloatEditDelegate *m_pWindGradientDelegate;

	FloatEdit *m_pctrlXCmRef, *m_pctrlZCmRef;
	FloatEdit *m_pctrlDensity;
	FloatEdit *m_pctrlViscosity;
	QLineEdit *m_pctrlBoatPolarName;

	QCheckBox *m_pctrlGroundEffect;

	QRadioButton *m_pctrlPanelMethod;
	QRadioButton *m_pctrlUnit1, *m_pctrlUnit2;

	QLabel *m_pctrlQInfCl;
	QLabel *m_pctrlBoatName;

	QLabel *m_pctrlRho, *m_pctrlNu;
	QLabel *m_pctrlDensityUnit, *m_pctrlViscosityUnit;

	QLabel *m_pctrlLengthUnit1, *m_pctrlLengthUnit3;

	QPushButton	*OKButton, *CancelButton;
};

#endif // BOATPOLARDLG_H
