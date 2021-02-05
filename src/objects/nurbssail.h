/****************************************************************************

         NURBSSail Class
         Copyright (C) 2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/


#ifndef NURBSSAIL_H
#define NURBSSAIL_H

#include "sail.h"
#include "nurbssurface.h"

class NURBSSail : public Sail
{
public:
    NURBSSail();

    bool SerializeSail(QDataStream &ar, bool bIsStoring);

    Vector3d GetPoint(double xrel, double zrel);
    Vector3d GetSectionPoint(int iSection, double xrel);
    Vector3d GetSectionPoint(SailSection *pSailSection, double xrel);

    void CreateSection(int iSection);
    void DeleteSection(int iSection);

    void Duplicate(Sail *pSail);
    bool Import(QFile *pFile);
    void Export();
    void ScaleSail(double XFactor, double YFactor, double ZFactor);

    void SplineSurface();

    double LuffLength();
    double LeechLength();
    double FootLength();
    double Chord(double zrel);


    void DrawFrame(int iSection, QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset, bool bSelected=false);

    NURBSSurface m_SplineSurface;

    bool m_bAlignedLE;
};

#endif // NURBSSAIL_H
