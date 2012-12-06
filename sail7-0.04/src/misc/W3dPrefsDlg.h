/****************************************************************************

	W3dPrefsDlg Class
	Copyright (C) 2009 Andre Deperrois sail7@xflr5.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#ifndef W3DPREFSDLG_H
#define W3DPREFSDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QSettings>
#include "../misc/LineButton.h"
#include "../misc/ColorButton.h"



class W3dPrefsDlg : public QDialog
{
	Q_OBJECT

	friend class QSail7;
	friend class MainFrame;
	friend class GL3dBodyDlg;
	friend class SailViewWidget;

public:
    W3dPrefsDlg();
	void InitDialog();

private slots:
	void On3DAxis();
	void OnOutlineColor();
	void OnXCP();
	void OnMoments();
	void OnIDrag();
	void OnDownwash();
	void OnWakePanels();
	void OnStreamLines();
	void OnVLMMesh();
	void OnShowWake();

private:
	void SetupLayout();
	void SaveSettings(QSettings *pSettings);
	void LoadSettings(QSettings *pSettings);
	void *m_pSail7;

	LineButton *m_pctrlAxis, *m_pctrlOutline, *m_pctrlVLMMesh;
	LineButton *m_pctrlLift, *m_pctrlMoments, *m_pctrlInducedDrag, *m_pctrlDownwash;
	LineButton *m_pctrlStreamLines, *m_pctrlWakePanels;


	QCheckBox *m_pctrlShowWake;

public:
	static int s_3DAxisStyle, s_3DAxisWidth;
	static QColor s_3DAxisColor;
	static int s_VLMStyle, s_VLMWidth;
	static QColor s_VLMColor;
	static int s_OutlineStyle, s_OutlineWidth;
	static QColor s_OutlineColor;
	static int s_XCPStyle, s_XCPWidth;
	static QColor s_XCPColor;
	static int s_MomentStyle, s_MomentWidth;
	static QColor s_MomentColor;
	static int s_IDragStyle, s_IDragWidth;
	static QColor s_IDragColor;

	static int s_DownwashStyle, s_DownwashWidth;
	static QColor s_DownwashColor;
	static int s_StreamLinesStyle, s_StreamLinesWidth;
	static QColor s_StreamLinesColor;
	static int s_WakeStyle, s_WakeWidth;
	static QColor s_WakeColor;

	static int s_CpStyle, s_CpWidth;
	static QColor s_CpColor;

	static bool s_bWakePanels;

};

#endif // W3DPREFSDLG_H
