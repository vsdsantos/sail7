/****************************************************************************

	SailTableDelegate Class

		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#ifndef SAILTABLEDELEGATE_H
#define SAILTABLEDELEGATE_H

#include <QList>
#include <QItemDelegate>
#include "../misc/FloatEdit.h"

class SailTableDelegate : public QItemDelegate
{
	Q_OBJECT
	friend class SailDlg;

public:
	SailTableDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void SetPrecision(int*PrecisionTable);

private:
	void *m_pSailDlg;
	int *m_pNPanels;
	int *m_pPrecision; ///table of float precisions for each column
};

#endif // QSailTableDelegate_H
