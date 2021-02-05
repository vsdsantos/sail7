/****************************************************************************

            CBoat class
            Copyright (C) 2011-2012 Andre Deperrois 
            All rights reserved

*****************************************************************************/


#include "../globals.h"
#include "boat.h"
#include "nurbssail.h"
#include "sailcutsail.h"


Boat::Boat()
{
    m_poaSail.clear();
    m_poaHull.clear();
    m_BoatName = "Boat Name";
}



Sail *Boat::GetSail(const QString &SailName)
{
    for(int is=0; is<m_poaSail.size(); is++)
    {
        Sail* pSail = m_poaSail.at(is);
        if(pSail->m_SailName==SailName) return pSail;
    }
    return nullptr;
}


Body *Boat::GetBody(const QString &BodyName)
{
    for(int is=0; is<m_poaHull.size(); is++)
    {
        Body* pBody = (Body*)m_poaHull.at(is);
        if(pBody->m_BodyName==BodyName) return pBody;
    }
    return nullptr;
}



bool Boat::SerializeBoat(QDataStream &ar, bool bIsStoring)
{
    //saves or loads the wing data to the archive ar

//    MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
    int ArchiveFormat;// identifies the format of the file
    int k, n;

    if(bIsStoring)
    {    // storing code
        ar << 456701;
        //456701 : initial format

        WriteCString(ar, m_BoatName);
        WriteCString(ar, m_BoatDescription);

        ar << m_poaSail.size();
        for(int is=0; is<m_poaSail.size(); is++)
        {
            Sail *pSail = (Sail*)m_poaSail.at(is);
            if(!pSail) return false;
            if(pSail->IsNURBSSail()) ar<<1;
            else                     ar<<2;
            if(!pSail->SerializeSail(ar, true)) return false;
        }

        ar << m_poaHull.size();
        for(int ih=0; ih<m_poaHull.size(); ih++)
        {
            Body *pHull = (Body*)m_poaHull.at(ih);
            if(!pHull) return false;
            if(!pHull->SerializeBody(ar, true)) return false;
        }

        return true;
    }
    else
    {
        // loading code
        ar >> ArchiveFormat;
        if (ArchiveFormat!=456701)  return false;

        ReadCString(ar,m_BoatName);
        ReadCString(ar, m_BoatDescription);

        ar>>n;
        for(int is=0; is<n; is++)
        {
            ar >> k;
            if(k==1)
            {
                NURBSSail *pNSail = new NURBSSail;
                if(!pNSail->SerializeSail(ar, false)) return false;
                m_poaSail.append(pNSail);
            }
            else if(k==2)
            {
                SailcutSail *pSCSail = new SailcutSail;
                if(!pSCSail->SerializeSail(ar, false)) return false;
                m_poaSail.append(pSCSail);
            }
            else return false;
        }

        ar>>n;
        for(int ih=0; ih<n; ih++)
        {
            Body *pHull = new Body;
            if(!pHull->SerializeBody(ar, false)) return false;
            m_poaHull.append(pHull);
        }
        return true;
    }
}


void Boat::DuplicateBoat(Boat*pBoat)
{
    for(int is=0; is<pBoat->m_poaSail.size(); is++)
    {
        Sail *pNewSail = nullptr;
        Sail *pSail = pBoat->m_poaSail.at(is);
        if(pSail->IsNURBSSail())        pNewSail = new NURBSSail;
        else if(pSail->IsSailcutSail()) pNewSail = new SailcutSail;

        if(pNewSail)
        {
            pNewSail->Duplicate(pSail);
            m_poaSail.append(pNewSail);
        }
    }


    for(int ih=0; ih<pBoat->m_poaHull.size(); ih++)
    {
        Body *pHull = new Body;
        pHull->Duplicate(pBoat->m_poaHull.at(ih));
        m_poaHull.append(pHull);
    }

    m_BoatName = pBoat->m_BoatName;
}


double Boat::Height()
{
    //approximate boat height
    double h = 0.0;
    for (int is=0; is<m_poaSail.size(); is++)
    {
        if(m_poaSail.at(is)->LuffLength()>h) h=m_poaSail.at(is)->LuffLength() + m_poaSail.at(is)->m_LEPosition.z;
    }
    return h;
}
