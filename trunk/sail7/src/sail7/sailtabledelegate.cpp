/****************************************************************************

    SailTableDelegate Class
    Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

            All rights reserved

*****************************************************************************/


#include "sailtabledelegate.h"
#include "saildlg.h"
#include <QComboBox>

SailTableDelegate::SailTableDelegate(QObject *parent)
 : QItemDelegate(parent)
{
    m_pSailDlg = parent;
    m_pNPanels = nullptr;
    m_pPrecision = nullptr;
}


QWidget *SailTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex & index ) const
{
    if(index.column()!=5)
    {
        FloatEdit *editor = new FloatEdit(parent);
        editor->setAlignment(Qt::AlignRight);
        editor->SetPrecision(m_pPrecision[index.column()]);
        return editor;
    }
    else
    {
        QComboBox *editor = new QComboBox(parent);
        editor->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        //fill comboboxes here

        if(index.column()==5)
        {
            editor->addItem(tr("Uniform"));
            editor->addItem(tr("Cosine"));
            editor->addItem(tr("Sine"));
            editor->addItem(tr("-Sine"));
        }
        return editor;
    }
    return nullptr;
}


void SailTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column()!=5)
    {
        double value = index.model()->data(index, Qt::EditRole).toDouble();
        FloatEdit *floatEdit = static_cast<FloatEdit*>(editor);
        floatEdit->SetValue(value);
    }
    else
    {
        QString strong = index.model()->data(index, Qt::EditRole).toString();
        QComboBox *pCbBox = static_cast<QComboBox*>(editor);
        int pos = pCbBox->findText(strong);
        if (pos>=0) pCbBox->setCurrentIndex(pos);
        else        pCbBox->setCurrentIndex(0);
    }
}


void SailTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column()!=5)
    {
        FloatEdit *floatEdit = static_cast<FloatEdit*>(editor);
        double value = floatEdit->Value();
        model->setData(index, value, Qt::EditRole);
    }
    else
    {
        QString strong;
        QComboBox *pCbBox = static_cast<QComboBox*>(editor);
        int sel = pCbBox->currentIndex();
        if (sel >=0) strong = pCbBox->itemText(sel);
        model->setData(index, strong, Qt::EditRole);
    }
}


void SailTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString strong;
    QStyleOptionViewItem myOption = option;
    if(index.column()!=5)
    {
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        strong = QString("%1").arg(index.model()->data(index, Qt::DisplayRole).toDouble(),0,'f', m_pPrecision[index.column()]);
    }
    else
    {
        myOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        strong = index.model()->data(index, Qt::DisplayRole).toString();
    }

/*    if(index.row()> *m_pNPanels) strong=" ";
    if(index.row()== *m_pNPanels)
    {
        if(index.column()==3 ||index.column()>=6) strong = " ";
    }*/
    drawDisplay(painter, myOption, myOption.rect, strong);
    drawFocus(painter, myOption, myOption.rect);
}


void SailTableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}


void SailTableDelegate::SetPrecision(int *PrecisionTable)
{
    m_pPrecision = PrecisionTable;
}











