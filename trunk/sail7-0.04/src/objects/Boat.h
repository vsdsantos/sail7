/****************************************************************************

			   CBoat class
			   Copyright (C) 2011-2012 Andre Deperrois sail7@xflr5.com
			   All rights reserved

*****************************************************************************/

#ifndef BOAT_H
#define BOAT_H

#include "Sail.h"
#include "Body.h"
#include <QList>

class CBoat
{
public:
    CBoat();

	QSail* GetSail(QString const &SailName);
	CBody* GetBody(QString const &BodyName);
	void DuplicateBoat(CBoat *pBoat);
	bool SerializeBoat(QDataStream &ar, bool bIsStoring);
	double Height();

	QList <QSail*> m_poaSail;
	QList <CBody*> m_poaHull;

	QString m_BoatName;
	QString m_BoatDescription;

};

#endif // BOAT_H
