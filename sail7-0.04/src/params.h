#ifndef PARAMS_H
#define PARAMS_H

//General
#define SAIL7APP
#define PI             3.14159265358979
#define MAXRECENTFILES         8
#define SETTINGSFORMAT    100623
#define PRECISION  0.0000001 //values are assumed 0 if less than this value
#define MAXCOLORS     30
#define MAXSTACKPOS   50 // max number of undo pictures on the stack in direct design

#define SAIL7    5



//2DVIEW
#define SAIL7VIEW    1
#define SECTIONVIEW  7


//Sail7 View
#define SAILPOLARVIEW        2
#define SAIL3DVIEW           3

//Object sizing
#define MAXBODIES            3
#define MAXSAILS            10
#define MAXSAILPANELS       30

//GLVIEW
#define GLSAIL7VIEW  11
#define GLBOATVIEW   13
#define GLSAILVIEW   17
#define GLBODYVIEW   19

#define MAXSAILSTATIONS   100

#define REFLENGTH   10.0


// 3D Analysis methods
#define VLMMETHOD           2
#define PANELMETHOD         3

//3D analysis parameters
#define MAXCHORDPANELS	   50
#define VLMMAXMATSIZE    5000
#define VLMHALF          2500
#define VLMMAXRHS         100 // max number of points which may be calculated in a single sequence
#define MAXPICTURESIZE     40 // maximum number of undo operations in direct design
#define MAXBODYFRAMES      60
#define MAXSIDELINES       40

#define BODYPANELTYPE       1
#define BODYSPLINETYPE      2

//OTHER, compatibility with XFLR5
#define MAXMASSES      10

//3D OpenGl list references

#define STREAMLINES             1137
#define SURFACESPEEDS           1138

#define LIFTFORCE               1222
#define VLMMOMENTS              1223

#define VLMCTRLPTS              1385
#define VLMVORTICES             1386

#define PANELCP                 1390
#define PANELCPLEGENDCOLOR      1391
#define PANELFORCEARROWS        1397
#define PANELFORCELEGENDTXT     1398

#define ARCBALL                 1514
#define ARCPOINT                1515

#define BODYGEOMBASE            4321
#define BODYMESHBASE            5322
#define BODYCPBASE              6173
#define BODYFORCELISTBASE       6245

#define SAILGEOMBASE            6789
#define SAILMESHBASE            7890
#define SAILCPBASE              8901
#define SAILFORCELISTBASE       9012

#define LIGHTSPHERE             9876


typedef enum {CTRL, VINF, BETA, PHI, LIFT, DRAG, FFFX, FFFY, FFFZ, FX, FY, FZ, MX, MY, MZ} enumPolarVar;


#define QUESTION (BB || !BB) //Shakespeare

#endif // PARAMS_H
