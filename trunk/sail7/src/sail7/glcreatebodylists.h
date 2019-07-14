/****************************************************************************

    GLCreateLists 

    Copyright (C) 2012 Andre Deperrois 
         All rights reserved

*****************************************************************************/


#ifndef GLCREATEBODYLISTS_H
#define GLCREATEBODYLISTS_H

#include <QtOpenGL>
#include "../objects/body.h"

class MainFrame;

void GLCreateBody3DFlatPanels(MainFrame *pMainFrame, GLuint iList, Body *pBody);
void GLCreateBody3DSplines(MainFrame *pMainFrame, GLuint iList, Body *pBody, int nx, int nh);
void GLCreateBodyMesh(MainFrame *pMainFrame, GLuint iList, Body *pBody);

#endif
