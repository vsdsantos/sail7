/****************************************************************************

	GL3DScales Class
	Copyright (C) 2011 Andre Deperrois sail7@xflr5.com

	All rights reserved


*****************************************************************************/

#ifndef GL3DSCALES_H
#define GL3DSCALES_H



#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QSettings>
#include "../misc/FloatEdit.h"



typedef enum {LEADINGEDGE, TRAILINGEDGE, YLINE, ZLINE} enumStreamLine;


class GL3DScales : public QWidget
{
	Q_OBJECT
	friend class MainFrame;
//	friend class QSail7;

public:
	GL3DScales(QWidget *parent = NULL);
	void InitDialog();

private slots:
	void OnCpScale();
	void OnApply();
	void OnLiftScale(int pos);
	void OnDragScale(int pos);
	void OnVelocityScale(int pos);
	void OnVelocitySet();
	void OnStreamParams();


private:
	void showEvent(QShowEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void ReadStreamParams();
	void SetControls();
	void SetStreamControls();
	void SetupLayout();

	bool LoadSettings(QSettings *pSettings);
	bool SaveSettings(QSettings *pSettings);

	QSlider *m_pctrlLiftScaleSlider, *m_pctrlDragScaleSlider, *m_pctrlVelocityScaleSlider;
	QPushButton *ApplyButton;
	QCheckBox *m_pctrlAutoCpScale;
	FloatEdit	*m_pctrlLegendMin, *m_pctrlLegendMax;

	FloatEdit *m_pctrlNXPoint, *m_pctrlDeltaL, *m_pctrlXFactor;
	FloatEdit *m_pctrlXOffset, *m_pctrlYOffset, *m_pctrlZOffset;
	FloatEdit *m_pctrlNStreamLines, *m_pctrlStreamLineSpacing;
	QRadioButton *m_pctrlLE, *m_pctrlTE, *m_pctrlYLine, *m_pctrlZLine;

	QLabel *m_pctrlLengthUnit1, *m_pctrlLengthUnit2, *m_pctrlLengthUnit3, *m_pctrlLengthUnit4, *m_pctrlLengthUnit5;

	static void *s_pSail7;
	static void *s_pMainFrame;

	
public:
	static enumStreamLine s_pos;
	static int s_NX, s_NStreamLines;
	static double s_StreamlineSpacing;
	static double s_DeltaL;
	static double s_XFactor;
	static double s_XOffset, s_YOffset, s_ZOffset;
};

#endif // GL3DSCALES_H
