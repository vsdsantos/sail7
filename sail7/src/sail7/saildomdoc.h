/****************************************************************************

         SailDomDoc Class
         Copyright (C) 2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/


#ifndef SAILDOMDOC_H
#define SAILDOMDOC_H

#include <QDomDocument>
#include <QFile>

class SailDomDoc : public QDomDocument
{
public:
    SailDomDoc(void *pSail=nullptr);
    QDomElement GetElement(const QDomNode &node, QString const &TagName, QString const &AttributeName);
    void ReadNodes(QDomNode node);
    void Export(QFile *pFile);
    bool Import(QFile *pFile);

    void *m_pSail;
};

#endif // SAILDOMDOC_H
