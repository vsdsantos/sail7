/****************************************************************************

	Globals Class
	Copyright (C) 2008-2012 Andre Deperrois sail7@xflr5.com
	All rights reserved

*****************************************************************************/

//Global functions
#include "globals.h"
#include "params.h"

#include <QtDebug>

#include <QVector3D>
#include <QPen>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime> 
#include <QByteArray>
#include <math.h>


void ExpFormat(double &f, int &exp)
{
	if (f==0.0)
	{
		exp = 0;
		f = 0.0;
		return;
	}
	double f1 = fabs(f);
//	int sgn = int(f/f1);
	if(f1<1)
		exp = (int)log10(f1)-1;
	else
		exp = (int)log10(f1);

	f = f/pow(10.0,exp);

}



bool Gauss(double *A, int n, double *B, int m, bool *pbCancel)
{
	static int row, i, j, pivot_row, k;
	static double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n)
	{
//		qApp->processEvents();
		if(*pbCancel) return false;
		//  find the pivot row
		A_pivot_row = pa;
		max = fabs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
		{
			if ((dum = fabs(*(pA+row))) > max)
			{
				max = dum;
				A_pivot_row = pA;
				pivot_row = i;
			}
		}
		if (max <= 0.0)
		{
			return false;                // the matrix A is singular
		}
			// and if it differs from the current row, interchange the two rows.

		if (pivot_row != row)
		{
			for (i = row; i < n; i++)
			{
				dum = *(pa + i);
				*(pa + i) = *(A_pivot_row + i);
				*(A_pivot_row + i) = dum;
			}
			for(k=0; k<m; k++)
			{
				dum = B[row+k*n];
				B[row+k*n] = B[pivot_row+k*n];
				B[pivot_row+k*n] = dum;
			}
		}

		// Perform forward substitution
		for (i = row+1; i<n; i++)
		{
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<m; k++)
				B[i+k*n] += dum * B[row+k*n];
		}
	}
	// Perform backward substitution

	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--)
	{
//		qApp->processEvents();
		if(*pbCancel) return false;

		if ( *(pa + row) == 0.0 )
		{
			return false;           // matrix is singular
		}
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum;
		for(k=0; k<m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++)
		{
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<m; k++)
				B[i+k*n] -= dum * B[row+k*n];
		}
	}
	return true;
}



void GetAreaUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="mm"+QString::fromUtf8("²");
			break;
		}
		case 1:
		{
			str="cm"+QString::fromUtf8("²");
			break;
		}
		case 2:
		{
			str="dm"+QString::fromUtf8("²");
			break;
		}
		case 3:
		{
			str="m"+QString::fromUtf8("²");
			break;
		}
		case 4:
		{
			str="in"+QString::fromUtf8("²");
			break;
		}
		case 5:
		{
			str="ft"+QString::fromUtf8("²");
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}



void GetLengthUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="mm";
			break;
		}
		case 1:
		{
			str="cm";
			break;
		}
		case 2:
		{
			str="dm";
			break;
		}
		case 3:
		{
			str="m";
			break;
		}
		case 4:
		{
			str="in";
			break;
		}
		case 5:
		{
			str="ft";
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}

void GetForceUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:{
			str="N";
			break;
		}
		case 1:{
			str="lbf";
			break;
		}

		default:{
			str=" ";
			break;
		}
	}
}

void GetMomentUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="N.m";
			break;
		}
		case 1:
		{
			str="lbf.in";
			break;
		}
		case 2:
		{
			str="lbf.ft";
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}


void GetSpeedUnit(QString &str, int unit)
{
	switch(unit){
		case 0:{
			str="m/s";
			break;
		}
		case 1:{
			str="km/h";
			break;
		}
		case 2:{
			str="ft/s";
			break;
		}
		case 3:{
			str="kt";
			break;
		}
		case 4:{
			str="mph";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}


void GetWeightUnit(QString &str, int unit)
{
	switch(unit){
		case 0:{
			str="g";
			break;
		}
		case 1:{
			str="kg";
			break;
		}
		case 2:{
			str="oz";
			break;
		}
		case 3:{
			str="lb";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}

bool IsBetween(int f, int f1, int f2)
{
	if (f2 < f1)
	{
		int tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(f<f1) return false;
	else if(f>f2) return false;
	return true;
}


bool IsEven(int n)
{
	if(n%2==0) return true;
	else return false;
}

bool IsBetween(int f, double f1, double f2)
{
	double ff = f;
	if (f2 < f1)
	{
		double tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(ff<f1)       return false;
	else if(ff>f2) return false;
	return true;
}


Qt::PenStyle GetStyle(int s)
{
	if(s==0)      return Qt::SolidLine;
	else if(s==1) return Qt::DashLine;
	else if(s==2) return Qt::DotLine;
	else if(s==3) return Qt::DashDotLine;
	else if(s==4) return Qt::DashDotDotLine;
	return Qt::SolidLine;
}



bool ReadAVLString(QTextStream &in, int &Line, QString &strong)
{
	bool bComment = true;
	int pos;

	while(bComment && !in.atEnd())
	{
		bComment = false;

		strong = in.readLine();
		if(in.atEnd()) return false;

		strong = strong.trimmed();
		pos = strong.indexOf("#",0);
		if(pos>=0) strong = strong.left(pos);
		pos = strong.indexOf("!",0);
		if(pos>=0) strong = strong.left(pos);

		if(strong.isEmpty()) bComment = true;

		Line++;
	}

	if(in.atEnd())
	{
		return false;
	}
	return true;
}


void ReadCOLORREF(QDataStream &ar, QColor &color)
{
	qint32 colorref;
	int r,g,b;

	ar >> colorref;
	b = (int)(colorref/256/256);
	colorref -= b*256*256;
	g = (int)(colorref/256);
	r = colorref - g*256;
	color = QColor(r,g,b,255);

}

void ReadCString(QDataStream &ar, QString &strong)
{
	qint8 qi, ch;
	char c;

	ar >> qi;
	strong.clear();
	for(int j=0; j<qi;j++)
	{
		strong += " ";
		ar >> ch;
		c = char(ch);
		strong[j] = c;
	}
}


bool Rewind1Line(QTextStream &in, int &Line, QString &strong)
{
	int length = strong.length() * 1+2;//1 char takes one byte in the file ?

	int pos = in.pos();
	in.seek(pos-length);
	Line--;
	return true;
}


void ReynoldsFormat(QString &str, double f)
{
	int i, q, r, exp;
	f = (int(f/1000.0))*1000.0;

	exp = (int)log10(f);
	r = exp%3;
	q = (exp-r)/3;

	QString strong;
	strong = QString("%1").arg(f,0,'f',0);

	int l = strong.length();

	for (i=0; i<q; i++){
		strong.insert(l-3*(i+1)-i," ");
		l++;
	}

	for (i=strong.length(); i<9; i++){
		strong = " "+strong;
	}

	str = strong;
}


void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit,
			  double &mtoUnit, double &m2toUnit, double &mstoUnit,  double &kgtoUnit, double &NtoUnit, double &NmtoUnit)
{
	switch(LUnit)
	{
		case 0:
		{//mdm
			mtoUnit  = 1000.0;
			break;
		}
		case 1:{//cm
			mtoUnit  = 100.0;
			break;
		}
		case 2:{//dm
			mtoUnit  = 10.0;
			break;
		}
		case 3:{//m
			mtoUnit  = 1.0;
			break;
		}
		case 4:{//in
			mtoUnit  = 1000.0/25.4;
			break;
		}
		case 5:{///ft
			mtoUnit  = 1000.0/25.4/12.0;
			break;
		}
		default:{//m
			mtoUnit  = 1.0;
			break;
		}
	}
	switch(AUnit)
	{
		case 0:{//mm²
			m2toUnit = 1000000.0;
			break;
		}
		case 1:{//cm²
			m2toUnit = 10000.0;
			break;
		}
		case 2:{//dm²
			m2toUnit = 100.0;
			break;
		}
		case 3:{//m²
			m2toUnit = 1.0;
			break;
		}
		case 4:{//in²
			m2toUnit = 1./0.254/0.254*100.0;
			break;
		}
		case 5:{//ft²
			m2toUnit = 1./0.254/0.254/144.0*100.0;
			break;
		}
		default:{
			m2toUnit = 1.0;
			break;
		}
	}

	switch(WUnit){
		case 0:{///g
			kgtoUnit = 1000.0;
			break;
		}
		case 1:{//kg
			kgtoUnit = 1.0;

			break;
		}
		case 2:{//oz
			kgtoUnit = 1./ 2.83495e-2;
			break;
		}
		case 3:{//lb
			kgtoUnit = 1.0/0.45359237;
			break;
		}
		default:{
			kgtoUnit = 1.0;
			break;
		}
	}
	switch(SUnit){
		case 0:{// m/s
			mstoUnit = 1.0;
			break;
		}
		case 1:{// km/h
			mstoUnit = 3600.0/1000.0;
			break;
		}
		case 2:{// ft/s
			mstoUnit = 100.0/2.54/12.0;
			break;
		}
		case 3:{// kt (int.)
			mstoUnit = 1.0/0.514444;
			break;
		}
		case 4:{// mph
			mstoUnit = 3600.0/1609.344;
			break;
		}
		default:{
			mstoUnit = 1.0;
			break;
		}
	}

	switch(FUnit){
		case 0:{//N
			NtoUnit = 1.0;
			break;
		}
		case 1:{//lbf
			NtoUnit = 1.0/4.44822;
			break;
		}
		default:{
			NtoUnit = 1.0;
			break;
		}
	}
	switch(MUnit){
		case 0:{//N.m
			NmtoUnit = 1.0;
			break;
		}
		case 1:{//lbf.in
			NmtoUnit = 1.0/4.44822/0.0254;
			break;
		}
		case 2:{//lbf.0t
			NmtoUnit = 1.0/4.44822/12.0/0.0254;
			break;
		}
		default:{
			NmtoUnit = 1.0;
			break;
		}
	}
}


void Trace(int n)
{
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;


	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	ts << str << n;

	tf->close();
}


void Trace(QString msg)
{
//	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	str += msg;
	str += "\n";

	ts << str;

	tf->close();
}


void Trace(QString msg, int n)
{
//	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;



	strong = QString("  %1").arg(n);
	strong = msg + strong;

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	str += strong;
	str += "\n";
	ts << str;;
	tf->close();
}


void Trace(QString msg, double f)
{
//	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;


	strong = QString("  %1").arg(f);
	strong = msg + strong;


	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	str += strong;
	str += "\n";
	ts << str;


	tf->close();
}



void WriteCOLORREF(QDataStream &ar, QColor const &color)
{
	qint32 colorref;
	int r,g,b;

	color.getRgb(&r,&g,&b);

	colorref = b*256*256+g*256+r;
	ar << colorref;
}



void WriteCString(QDataStream &ar, QString const &strong)
{
	qint8 qi = strong.length();

	QByteArray textline;
	char *text;
	textline = strong.toAscii();
	text = textline.data();
	ar << qi;
	ar.writeRawData(text, qi);
}



double GLGetRed(double tau)
{
	if(tau>2.0/3.0)      return 1.0;
	else if(tau>1.0/3.0) return (3.0*(tau-1.0/3.0));
	else                 return 0.0;
}


double GLGetGreen(double tau)
{
	if(tau<0.f || tau>1.0) 	return 0.0;
	else if(tau<1.0/4.0) 	return (4.0*tau);
	else if(tau>3.0/4.0) 	return (1.0-4.0*(tau-3.0/4.0));
	else                    return 1.0;
}


double GLGetBlue(double tau)
{
	if(tau>2.0/3.0)      return 0.0;
	else if(tau>1.0/3.0) return (1.0-3.0*(tau-1.0/3.0));
	else                 return 1.0;
}


void ReadValues(QString line, int &res, double &x, double &y, double &z)
{
/*	char *sx = new char[30];
	char *sy = new char[30];
	char *text;*/
	QString str;
	bool bOK;

	line = line.simplified();
	int pos = line.indexOf(" ");
	res = 0;
	if(pos>0)
	{
		str = line.left(pos);
		line = line.right(line.length()-pos);
	}
	else
	{
		str = line;
		line = "";
	}
	x = str.toDouble(&bOK);
	if(bOK) res++;
	else
	{
		y=z=0.0;
		return;
	}

	line = line.trimmed();
	pos = line.indexOf(" ");
	if(pos>0)
	{
		str = line.left(pos);
		line = line.right(line.length()-pos);
	}
	else
	{
		str = line;
		line = "";
	}
	y = str.toDouble(&bOK);
	if(bOK) res++;
	else
	{
		z=0.0;
		return;
	}

	line = line.trimmed();
	if(!line.isEmpty())
	{
		z = line.toDouble(&bOK);
		if(bOK) res++;
	}
	else z=0.0;
}



////////////////////////////////////////////////////////////////////////////////
//  int Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n)   //
//                                                                            //
//  Unknown author http://mymathlib.webtrellis.net/index.html                 //
//                                                                            //
//  Description:                                                              //
//     This routine uses Crout's method to decompose a row interchanged       //
//     version of the n x n matrix A into a lower triangular matrix L and a   //
//     unit upper triangular matrix U such that A = LU.                       //
//     The matrices L and U replace the matrix A so that the original matrix  //
//     A is destroyed.                                                        //
//     Note!  In Crout's method the diagonal elements of U are 1 and are      //
//            not stored.                                                     //
//     Note!  The determinant of A is the product of the diagonal elements    //
//            of L.  (det A = det L * det U = det L).                         //
//     The LU decomposition is convenient when one needs to solve the linear  //
//     equation Ax = B for the vector x while the matrix A is fixed and the   //
//     vector B is varied.  The routine for solving the linear system Ax = B  //
//     after performing the LU decomposition for A is                         //
//                      Crout_LU_with_Pivoting_Solve.                         //
//     (see below).                                                           //
//                                                                            //
//     The Crout method with partial pivoting is: Determine the pivot row and //
//     interchange the current row with the pivot row, then assuming that     //
//     row k is the current row, k = 0, ..., n - 1 evaluate in order the      //
//     the following pair of expressions                                      //
//       L[i][k] = (A[i][k] - (L[i][0]*U[0][k] + . + L[i][k-1]*U[k-1][k]))    //
//                                 for i = k, ... , n-1,                      //
//       U[k][j] = A[k][j] - (L[k][0]*U[0][j] + ... + L[k][k-1]*U[k-1][j])    //
//                                                                  / L[k][k] //
//                                      for j = k+1, ... , n-1.               //
//       The matrix U forms the upper triangular matrix, and the matrix L     //
//       forms the lower triangular matrix.                                   //
//                                                                            //
//  Arguments:                                                                //
//     double *A       Pointer to the first element of the matrix A[n][n].    //
//     int    pivot[]  The i-th element is the pivot row interchanged with    //
//                     row i.                                                 //
//     int     n       The number of rows or columns of the matrix A.         //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  Failure - The matrix A is singular.                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


bool Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n, bool *pbCancel, double TaskSize, double &Progress)
{
	int i, j, k;
	double *p_k, *p_row, *p_col;
	double max=0.0;

	p_col = NULL;

	//  For each row and column, k = 0, ..., n-1,
	for (k=0, p_k=A; k<n; p_k+=n, k++)
	{
	//  find the pivot row
		pivot[k] = k;
		p_col = p_k+k;
		max = fabs( *(p_k + k) );
		for (j=k+1, p_row=p_k+n; j<n; j++, p_row+=n)
		{
			if (max<fabs(*(p_row+k)))
			{
				max = fabs(*(p_row+k));
				pivot[k] = j;
				p_col = p_row;
			}
		}
		if(!p_col) return false;

		// and if the pivot row differs from the current row, then
		// interchange the two rows.
		if (pivot[k] != k)
		{
			for (j=0; j<n; j++)
			{
				max = *(p_k + j);
				*(p_k + j) = *(p_col + j);
				*(p_col + j) = max;
			}
		}

		// and if the matrix is singular, return error
		if ( *(p_k + k) == 0.0 ) return false;

		// otherwise find the upper triangular matrix elements for row k.
		for (j = k+1; j < n; j++) *(p_k + j) /= *(p_k + k);

		// update remaining matrix
		for (i = k+1, p_row = p_k + n; i < n; p_row += n, i++)
			for (j = k+1; j < n; j++) *(p_row + j) -= *(p_row + k) * *(p_k + j);

		Progress += TaskSize/(double)(n);
		qApp->processEvents();
		if(*pbCancel) return false;		
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////////
//  int Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[],     //
//                                                        double x[], int n)  //
//                                                                            //
// Unknown author http://mymathlib.webtrellis.net/index.html                  //
//                                                                            //
//  Description:                                                              //
//     This routine uses Crout's method to solve the linear equation Ax = B.  //
//     This routine is called after the matrix A has been decomposed into a   //
//     product of a lower triangular matrix L and a unit upper triangular     //
//     matrix U without pivoting.  The argument LU is a pointer to the matrix //
//     the superdiagonal part of which is U and the subdiagonal together with //
//     the diagonal part is L. (The diagonal part of U is 1 and is not        //
//     stored.)   The matrix A = LU.                                          //
//     The solution proceeds by solving the linear equation Ly = B for y and  //
//     subsequently solving the linear equation Ux = y for x.                 //
//                                                                            //
//  Arguments:                                                                //
//     double *LU      Pointer to the first element of the matrix whose       //
//                     elements form the lower and upper triangular matrix    //
//                     factors of A.                                          //
//     double *B       Pointer to the column vector, (n x 1) matrix, B.       //
//     int    pivot[]  The i-th element is the pivot row interchanged with    //
//                     row i.                                                 //
//     double *x       Solution to the equation Ax = B.                       //
//     int     n       The number of rows or columns of the matrix LU.        //
//                                                                            //
//  Return Values:                                                            //
//     true  : Success                                                        //
//     false : Failure - The matrix A is singular.                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
bool Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[], double x[], int Size, bool *pbCancel)
{
	int i, k;
	double *p_k;
	double dum;

	//  Solve the linear equation Lx = B for x, where L is a lower triangular matrix.
	for (k=0, p_k=LU; k<Size; p_k+=Size, k++)
	{
		if (pivot[k] != k)
		{
			dum=B[k]; B[k]=B[pivot[k]]; B[pivot[k]]=dum;
		}

		x[k] = B[k];
		for (i=0; i<k; i++) x[k]-=x[i] * *(p_k+i);
		x[k] /= *(p_k+k);
		
		qApp->processEvents();
		if(*pbCancel) return false;
	}

	//  Solve the linear equation Ux = y, where y is the solution
	//  obtained above of Lx = B and U is an upper triangular matrix.
	//  The diagonal part of the upper triangular part of the matrix is
	//  assumed to be 1.0.
	for (k=Size-1, p_k=LU+Size*(Size-1); k>=0; k--, p_k-=Size)
	{
		if (pivot[k] != k)
		{
			dum=B[k]; B[k]=B[pivot[k]]; B[pivot[k]]=dum;
		}

		for (i=k+1; i<Size; i++) x[k]-=x[i] * *(p_k+i);
		if (*(p_k+k)==0.0)
		{
			return false;
		}
		
		qApp->processEvents();
		if(*pbCancel) return false;
	}

	return true;
}



double Det33(double *aij)
{
	//returns the determinant of a 3x3 matrix

	double det;

	det  = aij[0]*aij[4]*aij[8];
	det -= aij[0]*aij[5]*aij[7];

	det -= aij[1]*aij[3]*aij[8];
	det += aij[1]*aij[5]*aij[6];

	det += aij[2]*aij[3]*aij[7];
	det -= aij[2]*aij[4]*aij[6];

	return det;
}




void CharacteristicPol(double m[][4], double p[5])
{
	// input : 4x4 matrix m[][]
	// output : coefficients p[] of the matrix characteristic polynomial
	// thanks Mapple !
	// polynom can then be solved for complex roots using Bairstow's algorithm


	// lambda^4
	p[4] =  1;

	// lambda^3
	p[3] =  - m[0][0]- m[1][1]-m[2][2]- m[3][3];

	// lambda^2
	p[2] =
		+ m[0][0] * m[1][1]
		+ m[0][0] * m[2][2]
		+ m[0][0] * m[3][3]
		+ m[1][1] * m[3][3]
		+ m[1][1] * m[2][2]
		+ m[2][2] * m[3][3]
		- m[1][0] * m[0][1]
		- m[2][1] * m[1][2]
		- m[2][0] * m[0][2]
		- m[2][3] * m[3][2]
		- m[3][1] * m[1][3]
		- m[3][0] * m[0][3];

	// lambda^1
	p[1] =
		+ m[2][1] * m[1][2] * m[3][3]
		+ m[0][0] * m[2][1] * m[1][2]
		- m[3][1] * m[1][2] * m[2][3]
		+ m[3][1] * m[1][3] * m[2][2]
		+ m[1][0] * m[0][1] * m[3][3]
		+ m[1][0] * m[0][1] * m[2][2]
		- m[2][0] * m[0][1] * m[1][2]
		- m[1][0] * m[3][1] * m[0][3]
		- m[1][0] * m[2][1] * m[0][2]
		+ m[3][0] * m[1][1] * m[0][3]
		- m[3][0] * m[0][1] * m[1][3]
		+ m[2][0] * m[0][2] * m[3][3]
		- m[2][0] * m[3][2] * m[0][3]
		+ m[2][0] * m[1][1] * m[0][2]
		- m[3][0] * m[0][2] * m[2][3]
		+ m[3][0] * m[0][3] * m[2][2]
		- m[2][1] * m[3][2] * m[1][3]
		- m[0][0] * m[1][1] * m[2][2]
		+ m[0][0] * m[2][3] * m[3][2]
		+ m[1][1] * m[2][3] * m[3][2]
		- m[0][0] * m[2][2] * m[3][3]
		+ m[0][0] * m[3][1] * m[1][3]
		- m[1][1] * m[2][2] * m[3][3]
		- m[0][0] * m[1][1] * m[3][3];

	// lambda^0
	p[0] =
		+ m[2][0] * m[0][1] * m[1][2] * m[3][3]
		- m[2][0] * m[1][1] * m[0][2] * m[3][3]
		+ m[2][0] * m[1][1] * m[3][2] * m[0][3]
		- m[2][0] * m[0][1] * m[3][2] * m[1][3]
		+ m[1][0] * m[3][1] * m[0][3] * m[2][2]
		- m[1][0] * m[3][1] * m[0][2] * m[2][3]
		+ m[1][0] * m[2][1] * m[0][2] * m[3][3]
		- m[1][0] * m[2][1] * m[3][2] * m[0][3]
		- m[3][0] * m[1][1] * m[0][3] * m[2][2]
		+ m[3][0] * m[0][1] * m[1][3] * m[2][2]
		- m[3][0] * m[0][1] * m[1][2] * m[2][3]
		- m[2][0] * m[3][1] * m[1][2] * m[0][3]
		+ m[2][0] * m[3][1] * m[0][2] * m[1][3]
		- m[3][0] * m[2][1] * m[0][2] * m[1][3]
		+ m[3][0] * m[1][1] * m[0][2] * m[2][3]
		+ m[3][0] * m[2][1] * m[1][2] * m[0][3]
		- m[0][0] * m[2][1] * m[1][2] * m[3][3]
		+ m[0][0] * m[2][1] * m[3][2] * m[1][3]
		- m[1][0] * m[0][1] * m[2][2] * m[3][3]
		+ m[1][0] * m[0][1] * m[2][3] * m[3][2]
		+ m[0][0] * m[3][1] * m[1][2] * m[2][3]
		- m[0][0] * m[3][1] * m[1][3] * m[2][2]
		+ m[0][0] * m[1][1] * m[2][2] * m[3][3]
		- m[0][0] * m[1][1] * m[2][3] * m[3][2];
}




bool SplineInterpolation(int n, double *x, double *y, double *a, double *b, double *c, double *d)
{
//
// Given an array of n+1 pairs (x[i], y[i]), with i ranging from 0 to n,
// this function calculates the 3rd order cubic spline which interpolate the pairs.
//
// The spline is defined for each interval [x[j], x[j+1]) by n third order polynomial functions
//              p_j(x) = ax3 + bx2 + cx + d
//
// The equations to determine the coefficients a,b,c,d are
//
// Interpolation : 2n conditions
//    p_j(x[j])   = y[j];
//    p_j(x[j+1]) = y[j+1];
//
// Continuity of 1st and 2nd order derivatives at internal points: 2(n-1) conditions
//    p_j'(x[j]) = p_j+1'(x[j])
//    p_j"(x[j]) = p_j+1"(x[j])
//
// Second order derivative is zero at the end points : 2 conditions
//    p_j"(x[0]) =  p_j"(x[n]) =0
//
//
// This sets a linear system of size 4n which is solved by the Gauss algorithm for coefs a,b,c and d
// The RHS vector is
//	  a[0]
//	  b[0]
//	  c[0]
//	  d[0]
//	  a[1]
//    ...
//	  d[n-1]
//

	if(n>50) return false;
	int i,size;

	double M[16*MAXSAILSTATIONS*MAXSAILSTATIONS];// size is 4 coefs x MAXSAILSTATIONS
	double RHS[4*MAXSAILSTATIONS*MAXSAILSTATIONS];

	memset(M, 0, 16*n*n*sizeof(double));
	memset(RHS, 0, 4*n*sizeof(double));

	size = 4*n;
//	Interpolation conditions
	for (i=0; i<n; i++)
	{
		//pj(x[i]) = y[i]
		M[2*i*size +4*i]     = x[i]*x[i]*x[i];
		M[2*i*size +4*i + 1] = x[i]*x[i];
		M[2*i*size +4*i + 2] = x[i];
		M[2*i*size +4*i + 3] = 1.0;

		//pj(x[i+1]) = y[i+1]
		M[(2*i+1)*size +4*i]     = x[i+1]*x[i+1]*x[i+1];
		M[(2*i+1)*size +4*i + 1] = x[i+1]*x[i+1];
		M[(2*i+1)*size +4*i + 2] = x[i+1];
		M[(2*i+1)*size +4*i + 3] = 1.0;

		RHS[2*i]   = y[i];
		RHS[2*i+1] = y[i+1];
	}

//  Derivation conditions
	for (i=1; i<n; i++)
	{
		//continuity of 1st order derivatives

		M[(2*n+i)*size + 4*(i-1)]     =  3.0*x[i]*x[i];
		M[(2*n+i)*size + 4*(i-1)+1]   =  2.0     *x[i];
		M[(2*n+i)*size + 4*(i-1)+2]   =  1.0;

		M[(2*n+i)*size + 4*i]   = -3.0*x[i]*x[i];
		M[(2*n+i)*size + 4*i+1] = -2.0     *x[i];
		M[(2*n+i)*size + 4*i+2] = -1.0;

		RHS[2*n+i]   = 0.0;

		//continuity of 2nd order derivatives
		M[(3*n+i)*size + 4*(i-1)]     =  6.0*x[i];
		M[(3*n+i)*size + 4*(i-1)+1]   =  2.0     ;

		M[(3*n+i)*size + 4*i]   = -6.0*x[i];
		M[(3*n+i)*size + 4*i+1] = -2.0     ;

		RHS[3*n+i]   = 0.0;
	}

//	second order derivative is zero at end points = "natural spline"
	M[2*n*size]     = 6.0*x[0];
	M[2*n*size+1]   = 2.0;
	RHS[2*n]        = 0.0;

	M[3*n*size + size-4]   = 6.0*x[n];
	M[3*n*size + size-3]   = 2.0;
	RHS[3*n+1]             = 0.0;

	bool bCancel = false;
	if(!Gauss(M, 4*n, RHS, 1, &bCancel)) return false;

	for(i=0; i<n; i++)
	{
		a[i] = RHS[4*i];
		b[i] = RHS[4*i+1];
		c[i] = RHS[4*i+2];
		d[i] = RHS[4*i+3];
	}

	return true;
}


double GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d)
{
	if(t<=y[0])
		return a[0] * t*t*t + b[0] *t*t + c[0] * t +d[0];

	for (int k=1; k<m; k++)
	{
		if(t<=y[k])
			return a[k-1] * t*t*t + b[k-1] *t*t + c[k-1] * t +d[k-1];

	}
	return a[m-1] * t*t*t + b[m-1] *t*t + c[m-1] * t +d[m-1];
}




void VLMCmn(CVector const &A, CVector const &B, CVector const &WindDirection, CVector const &C, CVector &V, bool const &bAll, double const &CoreSize)
{
	// CLASSIC VLM FORMULATION/
	//
	//    LA__________LB               |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    \/          \/
	//
	// Note : the geometry has been rotated by sideslip, hence, there is no need to align the trailing vortices with sideslip
	//
	// V is the resulting speed at point C
	//
	// Vectorial operations are written inline to save computing times
	// -->longer code, but 4x more efficient....

	static double Omega, ftmp;
	static CVector h, r0,r1, r2, Psi, Far, t;

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	if(bAll)
	{
		r0.x = B.x - A.x;
		r0.y = B.y - A.y;
		r0.z = B.z - A.z;

		r1.x = C.x - A.x;
		r1.y = C.y - A.y;
		r1.z = C.z - A.z;

		r2.x = C.x - B.x;
		r2.y = C.y - B.y;
		r2.z = C.z - B.z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) >CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
				   -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

			V.x = Psi.x * Omega/4.0/PI;
			V.y = Psi.y * Omega/4.0/PI;
			V.z = Psi.z * Omega/4.0/PI;
		}
	}
	// We create Far points to align the trailing vortices with the reference axis
	// The trailing vortex legs are not aligned with the free-stream, i.a.w. the small angle approximation
	// If this approximation is not valid, then the geometry should be tilted in the polar definition

	// calculate left contribution
	Far.x = A.x +  WindDirection.x*50000.0;
	Far.y = A.y +  WindDirection.y*50000.0;
	Far.z = A.z +  WindDirection.z*50000.0;

	r0.x = A.x - Far.x;
	r0.y = A.y - Far.y;
	r0.z = A.z - Far.z;

	r1.x = C.x - A.x;
	r1.y = C.y - A.y;
	r1.z = C.z - A.z;

	r2.x = C.x - Far.x;
	r2.y = C.y - Far.y;
	r2.z = C.z - Far.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	t.x=1.0; t.y=0.0; t.z=0.0;

	h.x =  r1.y*t.z - r1.z*t.y;
	h.y = -r1.x*t.z + r1.z*t.x;
	h.z =  r1.x*t.y - r1.y*t.x;

	//Next add 'left' semi-infinite contribution
	//eq.6-56

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
			   -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/PI;
		V.y += Psi.y * Omega/4.0/PI;
		V.z += Psi.z * Omega/4.0/PI;
	}

	// calculate right vortex contribution
	Far.x = B.x  +  WindDirection.x*50000.0;
	Far.y = B.y  +  WindDirection.y*50000.0;
	Far.z = B.z  +  WindDirection.z*50000.0;

	r0.x = Far.x - B.x;
	r0.y = Far.y - B.y;
	r0.z = Far.z - B.z;

	r1.x = C.x - Far.x;
	r1.y = C.y - Far.y;
	r1.z = C.z - Far.z;

	r2.x = C.x - B.x;
	r2.y = C.y - B.y;
	r2.z = C.z - B.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	//Last add 'right' semi-infinite contribution
	//eq.6-57
	h.x =  r2.y*t.z - r2.z*t.y;
	h.y = -r2.x*t.z + r2.z*t.x;
	h.z =  r2.x*t.y - r2.y*t.x;

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
			   -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/PI;
		V.y += Psi.y * Omega/4.0/PI;
		V.z += Psi.z * Omega/4.0/PI;
	}
}




void VLMQmn(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &C, CVector &V, double const &CoreSize)
{
	//Quadrilateral VLM FORMULATION
	// LA, LB, TA, TB are the vortex's four corners
	// LA and LB are at the 3/4 point of panel nx
	// TA and TB are at the 3/4 point of panel nx+1
	//
	//    LA__________LB               |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    TA__________TB
	//
	//
	//
	// C is the point where the induced speed is calculated
	// V is the resulting speed

	//
	// Vectorial operations are written explicitly to save computing times (4x more efficient)
	//
	static CVector R[5];
	static CVector r0, r1, r2, Psi, t;
	static double ftmp, r1v, r2v, Omega;
	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	R[0].x = LB.x;
	R[0].y = LB.y;
	R[0].z = LB.z;
	R[1].x = TB.x;
	R[1].y = TB.y;
	R[1].z = TB.z;
	R[2].x = TA.x;
	R[2].y = TA.y;
	R[2].z = TA.z;
	R[3].x = LA.x;
	R[3].y = LA.y;
	R[3].z = LA.z;
	R[4].x = LB.x;
	R[4].y = LB.y;
	R[4].z = LB.z;

	for (int i=0; i<4; i++)
	{
		r0.x = R[i+1].x - R[i].x;
		r0.y = R[i+1].y - R[i].y;
		r0.z = R[i+1].z - R[i].z;
		r1.x = C.x - R[i].x;
		r1.y = C.y - R[i].y;
		r1.z = C.z - R[i].z;
		r2.x = C.x - R[i+1].x;
		r2.y = C.y - R[i+1].y;
		r2.z = C.z - R[i+1].z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		r1v = sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z));
		r2v = sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) > CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/r1v - (r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/r2v;
			V.x += Psi.x * Omega/4.0/PI;
			V.y += Psi.y * Omega/4.0/PI;
			V.z += Psi.z * Omega/4.0/PI;
		}
	}
}




bool Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal,
			   CVector const &A,  CVector const &U,  CVector &I, double &dist)
{
	//
	// Returns the intersection of a ray with the object's panels
	// The ray is defined by a mouse click and is perpendicular to the viewport
	//	A is the ray's origin,
	//	U is the ray's direction
	//	LA, LB, TA, TB define a quadrangle in 3D space.
	//	N is the normal to the quadrangle
	//	I is the resulting intersection point of the ray and the quadrangle, if inside the quadrangle
	//	dist = |AI|
	//	The return value is true if intersection inside the quadrangle, false otherwise
	//
	static CVector P, W, V, T;
	bool b1, b2, b3, b4;
	double r,s;

	r = (LA.x-A.x)*Normal.x + (LA.y-A.y)*Normal.y + (LA.z-A.z)*Normal.z ;
	s = U.x*Normal.x + U.y*Normal.y + U.z*Normal.z;

	dist = 10000.0;

	if(fabs(s)>0.0)
	{
		dist = r/s;

		//inline operations to save time
		P.x = A.x + U.x * dist;
		P.y = A.y + U.y * dist;
		P.z = A.z + U.z * dist;

		// P is inside the panel if on left side of each panel side
		W.x = P.x  - TA.x;
		W.y = P.y  - TA.y;
		W.z = P.z  - TA.z;
		V.x = TB.x - TA.x;
		V.y = TB.y - TA.y;
		V.z = TB.z - TA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b1 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		W.x = P.x  - TB.x;
		W.y = P.y  - TB.y;
		W.z = P.z  - TB.z;
		V.x = LB.x - TB.x;
		V.y = LB.y - TB.y;
		V.z = LB.z - TB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b2 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		W.x = P.x  - LB.x;
		W.y = P.y  - LB.y;
		W.z = P.z  - LB.z;
		V.x = LA.x - LB.x;
		V.y = LA.y - LB.y;
		V.z = LA.z - LB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b3 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		W.x = P.x  - LA.x;
		W.y = P.y  - LA.y;
		W.z = P.z  - LA.z;
		V.x = TA.x - LA.x;
		V.y = TA.y - LA.y;
		V.z = TA.z - LA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b4 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		if(b1 && b2 && b3 && b4)
		{
			I.Set(P.x, P.y, P.z);
			return true;
		}
	}
	return false;
}



double Cofactor44(double *aij, int &i, int &j)
{
	//returns the complex cofactor	of element i,j, in the 4x4 matrix aij
	int p,q;
	double a33[9];

	p=q=0;
	memset(a33,0, 9*sizeof(double));

	p = 0;
	for(int k=0; k<4; k++)
	{
		if(k!=i)
		{
			q = 0;
			for(int l=0; l<4; l++)
			{
				if(l!=j)
				{
					a33[p*3+q] = *(aij+4*k+l);
					q++;
				}
			}
			p++;
		}
	}
	return Det33(a33);
}



double Det44(double *aij)
{
	//	returns the determinant of a 4x4 matrix

	static int i,j,k,l,p,q;
	static double sign;
	static double det, a33[9];
	det = 0.0;

	i=0;
	for(j=0; j<4; j++)
	{
		p = 0;
		for(k=0; k<4; k++)
		{
			if(k!=i)
			{
				q = 0;
				for(l=0; l<4; l++)
				{
					if(l!=j)
					{
						a33[p*3+q] = aij[4*k+l];
						q++;
					}
				}
				p++;
			}
		}
		sign = pow(-1.0,i+j);
		det += sign * aij[4*i+j] * Det33(a33);
	}

	return det;
}


bool Invert22(double*ain, double *aout)
{
	//small size, use the direct method
	double det=0.0;

	det = ain[0]*ain[3]-ain[2]*ain[1];

	if(abs(det)<PRECISION) return false;

	aout[0]  =  ain[3]/det;
	aout[1]  = -ain[1]/det;
	aout[2]  = -ain[2]/det;
	aout[3]  =  ain[0]/det;
	return true;
}


bool Invert44(double*ain, double *aout)
{
	//small size, use the direct method
	double det=0.0;
	double sign=1.0;

	det = Det44(ain);

	if(abs(det)<PRECISION) return false;

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			sign = pow(-1.0,i+j);
			aout[4*j+i] = sign * Cofactor44(ain, i, j)/det;
		}
	}
	return true;
}



double SplineBlend(int const &index, int const &degree, double const &t, double *knots)
{
//	Calculate the blending value, this is done recursively.
//	If the numerator and denominator are 0 the expression is 0.
//	If the denominator is 0 the expression is 0
//
//	   index    is the control point's index
//	   degree   is the spline's degree
//	   t        is the spline parameter
//
	static double eps = 1.e-6;

	if(degree==0)
	{
		if ((knots[index] <= t) && (t < knots[index+1]) ) return 1.0;
		else                                              return 0.0;
	}
	else
	{
		if (fabs(knots[index+degree] - knots[index])<eps && fabs(knots[index+degree+1] - knots[index+1])<eps)
			return 0.0;

		else if (fabs(knots[index+degree] - knots[index])<eps)
			return (knots[index+degree+1]-t) / (knots[index+degree+1]-knots[index+1])  * SplineBlend(index+1, degree-1, t, knots);

		else if (fabs(knots[index+degree+1] - knots[index+1])<eps)
			return (t-knots[index])     / (knots[index+degree] - knots[index])    * SplineBlend(index,   degree-1, t, knots);

		else
			return (t-knots[index])     / (knots[index+degree]  -knots[index])    * SplineBlend(index,   degree-1, t, knots) +
				   (knots[index+degree+1]-t) / (knots[index+degree+1]-knots[index+1])  * SplineBlend(index+1 ,degree-1, t, knots);
	}
}






