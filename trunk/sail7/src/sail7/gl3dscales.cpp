/****************************************************************************

    GL3DScales Class
    Copyright (C) 2009 Andre Deperrois 

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

#include "../sail7/sail7.h"
#include "../mainframe.h"
#include "../globals.h"
#include "gl3dscales.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QDockWidget>
#include <QAction>
#include <QtDebug>

void *GL3DScales::s_pSail7     = nullptr;
void *GL3DScales::s_pMainFrame = nullptr;

bool GL3DScales::s_bAutoCpScale = true;

double GL3DScales::s_LiftScale     = 1.0;
double GL3DScales::s_DragScale     = 1.0;
double GL3DScales::s_VelocityScale = 1.0;
double GL3DScales::s_LegendMin = -1.0;
double GL3DScales::s_LegendMax = 1.0;


enumStreamLine GL3DScales::s_pos =  TRAILINGEDGE;
int GL3DScales::s_NX  = 30;
int GL3DScales::s_NStreamLines  = 50;
double GL3DScales::s_StreamlineSpacing = .5;
double GL3DScales::s_DeltaL = 0.1;
double GL3DScales::s_XFactor = 1.07;
double GL3DScales::s_XOffset = 0.0;
double GL3DScales::s_YOffset = 0.0;
double GL3DScales::s_ZOffset = 0.0;


GL3DScales::GL3DScales(QWidget *pParent) : QWidget(pParent)
{
    setWindowTitle(tr("3D Scales Settings"));

    SetupLayout();

    SetControls();

    connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));

    connect(m_pctrlAutoCpScale, SIGNAL(clicked()), this, SLOT(OnCpScale()));
    connect(m_pctrlLegendMin, SIGNAL(editingFinished()), this, SLOT(OnCpScale()));
    connect(m_pctrlLegendMax, SIGNAL(editingFinished()), this, SLOT(OnCpScale()));

    connect(m_pctrlLiftScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnLiftScale(int)));
    connect(m_pctrlDragScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnDragScale(int)));
    connect(m_pctrlVelocityScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnVelocityScale(int)));
    connect(m_pctrlVelocityScaleSlider, SIGNAL(sliderReleased()), this, SLOT(OnVelocitySet()));

    connect(m_pctrlLE,   SIGNAL(clicked()), this, SLOT(OnStreamParams()));
    connect(m_pctrlTE,   SIGNAL(clicked()), this, SLOT(OnStreamParams()));
    connect(m_pctrlYLine, SIGNAL(clicked()), this, SLOT(OnStreamParams()));
    connect(m_pctrlZLine, SIGNAL(clicked()), this, SLOT(OnStreamParams()));
    connect(m_pctrlXOffset, SIGNAL(editingFinished()), this, SLOT(OnStreamParams()));
    connect(m_pctrlYOffset, SIGNAL(editingFinished()), this, SLOT(OnStreamParams()));
    connect(m_pctrlZOffset, SIGNAL(editingFinished()), this, SLOT(OnStreamParams()));
    connect(m_pctrlNXPoint, SIGNAL(editingFinished()), this, SLOT(OnStreamParams()));
    connect(m_pctrlDeltaL,  SIGNAL(editingFinished()), this, SLOT(OnStreamParams()));
    connect(m_pctrlXFactor, SIGNAL(editingFinished()), this, SLOT(OnStreamParams()));
}


void GL3DScales::SetControls()
{
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    if(s_pos==LEADINGEDGE)       m_pctrlLE->setChecked(true);
    else if(s_pos==TRAILINGEDGE) m_pctrlTE->setChecked(true);
    else if(s_pos==YLINE)        m_pctrlYLine->setChecked(true);
    else if(s_pos==ZLINE)        m_pctrlZLine->setChecked(true);

    m_pctrlDeltaL->SetValue(s_DeltaL  * pMainFrame->m_mtoUnit);
    m_pctrlXOffset->SetValue(s_XOffset* pMainFrame->m_mtoUnit);
    m_pctrlYOffset->SetValue(s_YOffset* pMainFrame->m_mtoUnit);
    m_pctrlZOffset->SetValue(s_ZOffset* pMainFrame->m_mtoUnit);
    m_pctrlXFactor->SetValue(s_XFactor);
    m_pctrlNXPoint->SetValue(s_NX);
    m_pctrlNStreamLines->SetValue(s_NStreamLines);
    m_pctrlStreamLineSpacing->SetValue(s_StreamlineSpacing);

    SetStreamControls();
}


void GL3DScales::SetupLayout()
{
    QSizePolicy szPolicyMinimum;
    szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
    szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

    setSizePolicy(szPolicyMinimum);

    QSizePolicy szPolicyExpanding;
    szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
    szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

    QGroupBox *CpScaleBox = new QGroupBox(tr("Cp Scale"));
    {
        QGridLayout *CpScaleLayout = new QGridLayout;
        m_pctrlAutoCpScale = new QCheckBox(tr("Auto Scales"));
        m_pctrlLegendMin = new FloatEdit(61.234);
        m_pctrlLegendMax = new FloatEdit(3.5555);
        m_pctrlLegendMin->SetPrecision(2);
        m_pctrlLegendMax->SetPrecision(2);
        QLabel *lab0 = new QLabel(tr("Min"));
        QLabel *lab1 = new QLabel(tr("Max"));
        lab0->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
        lab1->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
        CpScaleLayout->addWidget(m_pctrlAutoCpScale,1,2);
        CpScaleLayout->addWidget(lab0,2,1);
        CpScaleLayout->addWidget(m_pctrlLegendMin,2,2);
        CpScaleLayout->addWidget(lab1,3,1);
        CpScaleLayout->addWidget(m_pctrlLegendMax,3,2);
        CpScaleBox->setLayout(CpScaleLayout);
    }

//_______________________3D Scales

    QGroupBox *ScaleBox = new QGroupBox(tr("Vector Scales"));
    {
        QVBoxLayout *SliderLayout = new QVBoxLayout;
        m_pctrlLiftScaleSlider  = new QSlider(Qt::Horizontal);
        m_pctrlLiftScaleSlider->setMinimum(0);
        m_pctrlLiftScaleSlider->setMaximum(100);
        m_pctrlLiftScaleSlider->setSliderPosition(50);
        m_pctrlLiftScaleSlider->setTickInterval(5);
        m_pctrlLiftScaleSlider->setTickPosition(QSlider::TicksBelow);
        m_pctrlDragScaleSlider = new QSlider(Qt::Horizontal);
        m_pctrlDragScaleSlider->setMinimum(0);
        m_pctrlDragScaleSlider->setMaximum(100);
        m_pctrlDragScaleSlider->setSliderPosition(50);
        m_pctrlDragScaleSlider->setTickInterval(5);
        m_pctrlDragScaleSlider->setTickPosition(QSlider::TicksBelow);
        m_pctrlVelocityScaleSlider  = new QSlider(Qt::Horizontal);
        m_pctrlVelocityScaleSlider->setMinimum(0);
        m_pctrlVelocityScaleSlider->setMaximum(100);
        m_pctrlVelocityScaleSlider->setSliderPosition(50);
        m_pctrlVelocityScaleSlider->setTickInterval(5);
        m_pctrlVelocityScaleSlider->setTickPosition(QSlider::TicksBelow);
        SliderLayout->addWidget(m_pctrlLiftScaleSlider);
        SliderLayout->addWidget(m_pctrlDragScaleSlider);
        SliderLayout->addWidget(m_pctrlVelocityScaleSlider);

        QVBoxLayout *LabelLayout = new QVBoxLayout;
        QLabel *lab2 = new QLabel(tr("Lift "));
        QLabel *lab3 = new QLabel(tr("Drag "));
        QLabel *lab4 = new QLabel(tr("Velocity "));
        LabelLayout->addWidget(lab2);
        LabelLayout->addWidget(lab3);
        LabelLayout->addWidget(lab4);

        QHBoxLayout *ScaleLayout = new QHBoxLayout;
        ScaleLayout->addLayout(LabelLayout);
        ScaleLayout->addLayout(SliderLayout);
        ScaleBox->setLayout(ScaleLayout);
    }

//__________________________________    Streamlines

    QGroupBox *LengthBox = new QGroupBox(tr("Streamline length"));
    {
        m_pctrlNXPoint = new FloatEdit(33,0);
        m_pctrlNXPoint->SetPrecision(0);
        m_pctrlDeltaL = new FloatEdit(12.34,2);
        m_pctrlXFactor       = new FloatEdit(1.23,2);
        m_pctrlLengthUnit1 = new QLabel("miles");
        QLabel *lab5 = new QLabel(tr("X-axis points"));
        QLabel *lab6 = new QLabel(tr("1st segment"));
        QLabel *lab7 = new QLabel(tr("X factor"));
        lab5->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
        lab6->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
        lab7->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
        QGridLayout *LengthLayout = new QGridLayout;
        LengthLayout->addWidget(lab5, 1, 1);
        LengthLayout->addWidget(m_pctrlNXPoint , 1, 2);
        LengthLayout->addWidget(lab6, 2, 1);
        LengthLayout->addWidget(m_pctrlDeltaL, 2, 2);
        LengthLayout->addWidget(m_pctrlLengthUnit1, 2, 3);
        LengthLayout->addWidget(lab7, 3, 1);
        LengthLayout->addWidget(m_pctrlXFactor, 3, 2);
        LengthBox->setLayout(LengthLayout);
    }

    QGroupBox *StartBox = new QGroupBox(tr("Start Streamline at"));
    {
        QHBoxLayout *LineLayout = new QHBoxLayout;
        {
            m_pctrlLE = new QRadioButton(tr("L.E."));
            m_pctrlTE = new QRadioButton(tr("T.E."));
            m_pctrlYLine = new QRadioButton(tr("Y-Line"));
            m_pctrlZLine = new QRadioButton(tr("Z-Line"));
            LineLayout->addWidget(m_pctrlLE);
            LineLayout->addWidget(m_pctrlTE);
            LineLayout->addWidget(m_pctrlYLine);
            LineLayout->addWidget(m_pctrlZLine);
        }
        QGridLayout *OffsetLayout = new QGridLayout;
        {
            m_pctrlXOffset       = new FloatEdit(3.21,3);
            m_pctrlYOffset       = new FloatEdit(4.56,3);
            m_pctrlZOffset       = new FloatEdit(7.89,3);
            m_pctrlLengthUnit2 = new QLabel("km");
            m_pctrlLengthUnit3 = new QLabel("m");
            m_pctrlLengthUnit4 = new QLabel("light-year");
            m_pctrlLengthUnit5 = new QLabel("nanometer");
            QLabel *lab8  = new QLabel(tr("X-Offset"));
            QLabel *lab9  = new QLabel(tr("Y-Offset"));
            QLabel *lab10 = new QLabel(tr("Z-Offset"));
            lab8->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
            lab9->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
            lab10->setAlignment(Qt::AlignVCenter |Qt::AlignRight);

            m_pctrlNStreamLines = new FloatEdit(29, 0);
            m_pctrlStreamLineSpacing = new FloatEdit(.3, 2);
            QLabel *lab11 = new QLabel(tr("N Streamlines ="));
            QLabel *lab12 = new QLabel(tr("Spacing ="));
            lab11->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
            lab12->setAlignment(Qt::AlignVCenter |Qt::AlignRight);

            OffsetLayout->addWidget(lab8,1,1);
            OffsetLayout->addWidget(m_pctrlXOffset,1,2);
            OffsetLayout->addWidget(m_pctrlLengthUnit2,1,3);
            OffsetLayout->addWidget(lab9,2,1);
            OffsetLayout->addWidget(m_pctrlYOffset,2,2);
            OffsetLayout->addWidget(m_pctrlLengthUnit3,2,3);
            OffsetLayout->addWidget(lab10,3,1);
            OffsetLayout->addWidget(m_pctrlZOffset,3,2);
            OffsetLayout->addWidget(m_pctrlLengthUnit4,3,3);
            OffsetLayout->addWidget(lab11,4,1);
            OffsetLayout->addWidget(m_pctrlNStreamLines,4,2);
            OffsetLayout->addWidget(lab12,5,1);
            OffsetLayout->addWidget(m_pctrlStreamLineSpacing,5,2);
            OffsetLayout->addWidget(m_pctrlLengthUnit5,5,3);
        }
        QVBoxLayout *StartLayout = new QVBoxLayout;
        {
            StartLayout->addLayout(LineLayout);
            StartLayout->addLayout(OffsetLayout);
            StartBox->setLayout(StartLayout);
        }
    }

    QGroupBox *StreamBox = new QGroupBox(tr("Streamlines"));
    {
        ApplyButton = new QPushButton(tr("Apply"));
        QVBoxLayout *StreamLayout = new QVBoxLayout;
        StreamLayout->addWidget(LengthBox);
        StreamLayout->addWidget(StartBox);
        StreamLayout->addStretch(1);
        StreamLayout->addWidget(ApplyButton);
        StreamLayout->addStretch(1);
        StreamBox->setLayout(StreamLayout);
    }


    QVBoxLayout *MainLayout = new QVBoxLayout;
    MainLayout->addStretch(1);
    MainLayout->addWidget(ScaleBox);
    MainLayout->addSpacing(15);
    MainLayout->addStretch(1);
    MainLayout->addWidget(CpScaleBox);
    MainLayout->addSpacing(15);
    MainLayout->addStretch(1);
    MainLayout->addWidget(StreamBox);
    MainLayout->addStretch(1);

    setLayout(MainLayout);
}


void GL3DScales::InitDialog()
{
    if(!s_pMainFrame) return;
    Sail7* pSail7 = (Sail7*)s_pSail7;
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    QString str;

    GetLengthUnit(str, pMainFrame->m_LengthUnit);
    m_pctrlLengthUnit1->setText(str);
    m_pctrlLengthUnit2->setText(str);
    m_pctrlLengthUnit3->setText(str);
    m_pctrlLengthUnit4->setText(str);
    m_pctrlLengthUnit5->setText(str);

    m_pctrlAutoCpScale->setChecked(s_bAutoCpScale);
    m_pctrlLegendMin->SetValue(s_LegendMin);
    m_pctrlLegendMax->SetValue(s_LegendMax);
    m_pctrlLegendMin->setEnabled(!s_bAutoCpScale);
    m_pctrlLegendMax->setEnabled(!s_bAutoCpScale);

}



void GL3DScales::OnCpScale()
{
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    Sail7 *pSail7 = (Sail7*)s_pSail7;

    s_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
    m_pctrlLegendMin->setEnabled(!s_bAutoCpScale);
    m_pctrlLegendMax->setEnabled(!s_bAutoCpScale);

    s_LegendMax = m_pctrlLegendMax->Value();
    s_LegendMin = m_pctrlLegendMin->Value();

//        pSail7->m_bResetglOpp = true;
    pSail7->m_bResetglLegend = true;
    pSail7->m_bResetglCPForces = true;

    pMainFrame->UpdateView();
}


void GL3DScales::OnApply()
{
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    Sail7 *pSail7 = (Sail7*)s_pSail7;
    s_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
    s_LegendMax = m_pctrlLegendMax->Value();
    s_LegendMin = m_pctrlLegendMin->Value();

    OnStreamParams();
    pSail7->m_bResetglStream = true;

    pMainFrame->UpdateView();
}


void GL3DScales::OnLiftScale(int pos)
{
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    Sail7 *pSail7 = (Sail7*)s_pSail7;

    s_LiftScale    = pos/100.0/sqrt(1.01-pos/100.0);
    pSail7->m_bResetglLift = true;
    pSail7->m_bResetglCPForces = true;

    pMainFrame->UpdateView();
}


void GL3DScales::OnDragScale(int pos)
{
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    Sail7 *pSail7 = (Sail7*)s_pSail7;

    s_DragScale    = pos/100.0/sqrt(1.01-pos/100.0);
    pSail7->m_bResetglDrag = true;

    pMainFrame->UpdateView();
}


void GL3DScales::OnVelocityScale(int pos)
{
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    Sail7 *pSail7 = (Sail7*)s_pSail7;
    s_VelocityScale    = pos/100.0/sqrt(1.01-pos/100.0);
    pSail7->m_bResetglDownwash = true;
    pMainFrame->UpdateView();
}


void GL3DScales::OnVelocitySet()
{
    MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
    Sail7 *pSail7 = (Sail7*)s_pSail7;

    int pos = m_pctrlVelocityScaleSlider->sliderPosition();

    s_VelocityScale    = pos/100.0/sqrt(1.01-pos/100.0);
    pSail7->m_bResetglSpeeds   = true;
    pSail7->m_bResetglDownwash = true;
    pMainFrame->UpdateView();
}


void GL3DScales::showEvent(QShowEvent *event)
{
    InitDialog();
    SetControls();
}


void GL3DScales::ReadStreamParams()
{
    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
    s_NX = m_pctrlNXPoint->Value();
    s_DeltaL  = m_pctrlDeltaL->Value()  / pMainFrame->m_mtoUnit;
    s_XFactor = m_pctrlXFactor->Value();

    s_XOffset = m_pctrlXOffset->Value() / pMainFrame->m_mtoUnit;
    s_YOffset = m_pctrlYOffset->Value() / pMainFrame->m_mtoUnit;
    s_ZOffset = m_pctrlZOffset->Value() / pMainFrame->m_mtoUnit;

    s_NStreamLines = (int)m_pctrlNStreamLines->Value();
    s_StreamlineSpacing = m_pctrlStreamLineSpacing->Value();

    if(m_pctrlLE->isChecked())             s_pos=LEADINGEDGE;
    else if(m_pctrlTE->isChecked())      s_pos=TRAILINGEDGE;
    else if(m_pctrlYLine->isChecked())   s_pos=YLINE;
    else if(m_pctrlZLine->isChecked())   s_pos=ZLINE;
}


void GL3DScales::OnStreamParams()
{
    ReadStreamParams();
    SetStreamControls();
}


void GL3DScales::SetStreamControls()
{
    m_pctrlNStreamLines->setEnabled(m_pctrlYLine->isChecked() || m_pctrlZLine->isChecked());
    m_pctrlStreamLineSpacing->setEnabled(m_pctrlYLine->isChecked() || m_pctrlZLine->isChecked());
//    m_pctrlYOffset->setEnabled(!m_pctrlYLine->isChecked());
//    m_pctrlZOffset->setEnabled(!m_pctrlZLine->isChecked());
}


bool GL3DScales::LoadSettings(QSettings *pSettings)
{
    Sail7 *pSail7 = (Sail7*)s_pSail7;
    int l,d,v;
    l=d=v=50;

    pSettings->beginGroup("GL3DScales");
    {
        s_bAutoCpScale = pSettings->value("AutoCpScale", true).toBool();
        s_LegendMin    = pSettings->value("LegendMin", -1.0).toDouble();
        s_LegendMax    = pSettings->value("LegendMax", 1.0).toDouble();
        s_NX      = pSettings->value("NX",30).toInt();
        s_DeltaL  = pSettings->value("DeltaL",0.01).toDouble();
        s_XFactor = pSettings->value("XFactor",1.1).toDouble();
        s_XOffset = pSettings->value("XOffset",0.0).toDouble();
        s_YOffset = pSettings->value("YOffset",0.0).toDouble();
        s_ZOffset = pSettings->value("ZOffset",0.0).toDouble();
        s_NStreamLines = pSettings->value("NStreamLines", 30).toInt();
        s_StreamlineSpacing = pSettings->value("StreamlineSpacing", .3).toDouble();
        l = pSettings->value("LiftScale", 50).toInt();
        d = pSettings->value("DragScale", 50).toInt();
        v = pSettings->value("VelocityScale",50).toInt();

    }
    pSettings->endGroup();

    m_pctrlLiftScaleSlider->setSliderPosition(l);
    m_pctrlDragScaleSlider->setSliderPosition(d);
    m_pctrlVelocityScaleSlider->setSliderPosition(v);
    s_LiftScale     = (double)l/100.0/sqrt(1.01-(double)l/100.0);
    s_DragScale     = (double)d/100.0/sqrt(1.01-(double)d/100.0);
    s_VelocityScale = (double)v/100.0/sqrt(1.01-(double)v/100.0);


    return true;
}



bool GL3DScales::SaveSettings(QSettings *pSettings)
{
    Sail7 *pSail7 = (Sail7*)s_pSail7;
//    ReadStreamParams();
    pSettings->beginGroup("GL3DScales");
    {
        pSettings->setValue("AutoCpScale", s_bAutoCpScale);
        pSettings->setValue("LegendMin", s_LegendMin);
        pSettings->setValue("LegendMax", s_LegendMax);
        pSettings->setValue("NX", s_NX);
        pSettings->setValue("DeltaL", s_DeltaL);
        pSettings->setValue("XFactor", s_XFactor);
        pSettings->setValue("XOffset", s_XOffset);
        pSettings->setValue("YOffset", s_YOffset);
        pSettings->setValue("ZOffset", s_ZOffset);
        pSettings->setValue("NStreamLines", s_NStreamLines);
        pSettings->setValue("StreamlineSpacing", s_StreamlineSpacing);
        pSettings->setValue("LiftScale", m_pctrlLiftScaleSlider->sliderPosition());
        pSettings->setValue("DragScale", m_pctrlDragScaleSlider->sliderPosition());
        pSettings->setValue("VelocityScale", m_pctrlVelocityScaleSlider->sliderPosition());
    }
    pSettings->endGroup();
    return true;
}



void GL3DScales::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Escape:
        {
            MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
            pMainFrame->m_pctrl3DScalesWidget->setVisible(false);
            return;
        }
        case  Qt::Key_Return:
        {
            if(!ApplyButton->hasFocus()) ApplyButton->setFocus();
            else                         OnApply();
            break;
        }
        default:
            event->ignore();
    }
}




