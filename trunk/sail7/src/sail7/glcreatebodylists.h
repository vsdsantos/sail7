/****************************************************************************

    GLCreateLists 

    Copyright (C) 2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/


#ifndef GLCREATEBODYLISTS_H
#define GLCREATEBODYLISTS_H

#include <QtOpenGL>
#include "../objects/body.h"

void GLCreateBody3DFlatPanels(void *pParent, int iList, Body *pBody);
void GLCreateBody3DSplines(void *pParent, int iList, Body *pBody, int nx, int nh);
void GLCreateBodyMesh(void *pParent, int iList, Body *pBody);

#endif
