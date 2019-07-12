/****************************************************************************

	DisplaySettingsDlg Class
	Copyright (C) 2008-2008 Andre Deperrois sail7@xflr5.com

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

#include "DisplaySettingsDlg.h"
#include "../mainframe.h"
#include "../graph/graphdlg.h"
#include "../sail7/sail7.h"
#include <QApplication>
#include <QGroupBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QStyleFactory>
#include <QHBoxLayout>
#include <QVBoxLayout>


void * DisplaySettingsDlg::s_pMainFrame = nullptr;
void * DisplaySettingsDlg::s_pSail7 = nullptr;


DisplaySettingsDlg::DisplaySettingsDlg()
{
	setWindowTitle(tr("General Display Settings"));

	m_bIsGraphModified = false;
	m_bReverseZoom = false;
	m_bAlphaChannel = false;
	SetupLayout();

	connect(m_pctrlStyles, SIGNAL(activated(const QString &)),this, SLOT(OnStyleChanged(const QString &)));

	connect(m_pctrlBackColor, SIGNAL(clicked()),this, SLOT(OnBackgroundColor()));
	connect(m_pctrlGraphSettings, SIGNAL(clicked()),this, SLOT(OnGraphSettings()));
	connect(m_pctrlTextClr, SIGNAL(clicked()),this, SLOT(OnTextColor()));
	connect(m_pctrlTextFont, SIGNAL(clicked()),this, SLOT(OnTextFont()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void DisplaySettingsDlg::SetupLayout()
{
	QVBoxLayout *MainLayout = new QVBoxLayout;

	m_pctrlStyles = new QComboBox;

	QRegExp regExp("Q(.*)Style");
	QString defaultStyle = QApplication::style()->metaObject()->className();
	if (defaultStyle == QLatin1String("QMacStyle"))
		defaultStyle = QLatin1String("Macintosh (Aqua)");
	else if (defaultStyle == QLatin1String("OxygenStyle"))
		defaultStyle = QLatin1String("Oxygen");
	else if (regExp.exactMatch(defaultStyle))
		defaultStyle = regExp.cap(1);

	m_pctrlStyles->addItems(QStyleFactory::keys());
	m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(defaultStyle));


	m_pctrlGraphSettings  = new QPushButton(tr("All Graph Settings"));
	m_pctrlGraphSettings->setMinimumWidth(120);
	QHBoxLayout *GraphLayout = new QHBoxLayout;
	GraphLayout->addWidget(m_pctrlGraphSettings);
	QGroupBox *GraphBox = new QGroupBox(tr("Graph Settings"));
	GraphBox->setLayout(GraphLayout);


	QHBoxLayout *BackLayout = new QHBoxLayout;
	m_pctrlBackColor      = new ColorButton;
	m_pctrlBackColor->setMinimumWidth(120);
	BackLayout->addWidget(m_pctrlBackColor);
	QGroupBox *BackBox = new QGroupBox(tr("Background Color"));
	BackBox->setLayout(BackLayout);

	QHBoxLayout *FontLayout = new QHBoxLayout;
	m_pctrlTextFont       = new QPushButton;
	m_pctrlTextClr        = new QPushButton(tr("Text Color"));
	m_pctrlTextFont->setMinimumWidth(120);
	m_pctrlTextClr->setMinimumWidth(120);
	FontLayout->addWidget(m_pctrlTextFont);
	FontLayout->addWidget(m_pctrlTextClr);
	QGroupBox *FontBox = new QGroupBox(tr("Font"));
	FontBox->setLayout(FontLayout);

	m_pctrlReverseZoom = new QCheckBox(tr("Reverse zoom direction using mouse wheel"));
	m_pctrlAlphaChannel = new QCheckBox(tr("Enable 3D transparency"));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlStyles);
	MainLayout->addStretch(1);
	MainLayout->addWidget(BackBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(FontBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(GraphBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlReverseZoom);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlAlphaChannel);
	MainLayout->addSpacing(20);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void DisplaySettingsDlg::InitDialog()
{
	m_MemGraph.CopySettings(m_pRefGraph);
	m_pctrlBackColor->SetColor(m_BackgroundColor);
	QString FontName = m_TextFont.family() + QString(" %1").arg(m_TextFont.pointSize());
	m_pctrlTextFont->setText(FontName);
	m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(m_StyleName));
	m_pctrlReverseZoom->setChecked(m_bReverseZoom);
	m_pctrlAlphaChannel->setChecked(m_bAlphaChannel);
	QPalette palette = m_pctrlTextClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
		palette.setColor(QPalette::Button, m_BackgroundColor);
		palette.setColor(QPalette::ButtonText, m_TextColor);
		m_pctrlTextClr->setPalette(palette);
//		m_pctrlTextClr->setAutoFillBackground(true);
		m_pctrlTextClr->setFont(m_TextFont);
	}
}


void DisplaySettingsDlg::OnStyleChanged(const QString &StyleName)
{
	qApp->setStyle(StyleName);
//	QMessageBox::information(window(), tr("Warning"), "The new style will take effect at the next session");

	m_StyleName = StyleName;
}


void DisplaySettingsDlg::OnBackgroundColor()
{
	QColor Color = QColorDialog::getColor(m_BackgroundColor);
	if(Color.isValid()) m_BackgroundColor = Color;

	m_pctrlBackColor->SetColor(m_BackgroundColor);

	QPalette palette = m_pctrlTextClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_BackgroundColor);
		palette.setColor(QPalette::Button, m_BackgroundColor);
		palette.setColor(QPalette::ButtonText, m_TextColor);
		m_pctrlTextClr->setPalette(palette);
	}
}


void DisplaySettingsDlg::reject()
{
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	Sail7 *pSail7 = (Sail7*)s_pSail7;
	for(int ig=0; ig<4; ig++)
	{
		pSail7->m_BoatGraph[ig].CopySettings(&m_MemGraph);
	}
	QDialog::reject();
}



void DisplaySettingsDlg::OnGraphSettings()
{
	if(!m_pRefGraph) return;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	Sail7 *pSail7 = (Sail7*)s_pSail7;

	GraphDlg dlg;

	dlg.m_GraphArray[0] = pSail7->m_BoatGraph;
	dlg.m_GraphArray[1] = pSail7->m_BoatGraph+1;
	dlg.m_GraphArray[2] = pSail7->m_BoatGraph+2;
	dlg.m_GraphArray[3] = pSail7->m_BoatGraph+3;

	dlg.m_NGraph = 4;

	QGraph graph;
	graph.CopySettings(m_pRefGraph);

	dlg.m_pMemGraph = m_pRefGraph;
	dlg.m_pGraph = &graph;

	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_pRefGraph->CopySettings(dlg.m_pGraph);
		m_bIsGraphModified = true;
	}
}


void DisplaySettingsDlg::OnTextColor()
{
	QColor Color = QColorDialog::getColor(m_TextColor);
	if(Color.isValid()) m_TextColor = Color;

	QPalette palette = m_pctrlTextClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_BackgroundColor);
		palette.setColor(QPalette::Button, m_BackgroundColor);
		palette.setColor(QPalette::ButtonText, m_TextColor);
		m_pctrlTextClr->setPalette(palette);
//		m_pctrlTextClr->setAutoFillBackground(true);
	}
}



void DisplaySettingsDlg::OnTextFont()
{
	bool ok;
	QFont TextFont;
	TextFont.setStyleHint(QFont::TypeWriter, QFont::OpenGLCompatible);

#ifdef Q_WS_MAC
	//20090604 Mac OS Native font dialog does not work well under QT 4.5.1
	//QFont font = QFontDialog::getFont(&ok, m_TextFont, this);
        //20110324 Works again under QT 4.6, though it loses focus is mouse is moved outside of it (QT bug?)
        //QFont font = QFontDialog::getFont(&ok, m_TextFont, this, "",QFontDialog::DontUseNativeDialog);
	   TextFont = QFontDialog::getFont(&ok, m_TextFont, this);
#else
	TextFont = QFontDialog::getFont(&ok, m_TextFont, this);
#endif

	if (ok)
	{
		m_TextFont = TextFont;
		m_pctrlTextFont->setText(m_TextFont.family());
		m_pctrlTextFont->setFont(m_TextFont);
		m_pctrlTextClr->setFont(m_TextFont);
	}
}








