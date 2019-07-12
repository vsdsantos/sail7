/****************************************************************************

    CFrame Class
	Copyright (C) 2007-2012 Andre Deperrois adeperrois@xflr5.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/
 
#ifndef CFRAME_H
#define CFRAME_H

#include "vector3d.h"
#include <QDataStream>
#include <QList>

class CFrame
{
public:
	CFrame(int nCtrlPts=0);
	~CFrame();
    bool SerializeFrame(QDataStream &ar, bool bIsStoring);

	int IsPoint(Vector3d const &Point, double const &ZoomFactor);
	void AppendPoint(Vector3d const& Pt);
	void InsertPoint(int n);
	void InsertPoint(int n, const Vector3d &Pt);
	int InsertPoint(Vector3d const &Real, int iAxis);
	bool RemovePoint(int n);

	void CopyFrame(CFrame *pFrame);
	void CopyPoints(QList<Vector3d> *m_pPointList);

	void SetPosition(Vector3d Pos);
	void SetuPosition(double u);
	void SetvPosition(double v);
	void SetwPosition(double w);

	void RotateFrameY(double Angle);

       int PointCount() {return m_CtrlPoint.size();}

	double Height();
	double zPos();

	int m_iHighlight, m_iSelect;

	Vector3d m_Position;
	QList <Vector3d> m_CtrlPoint;	// the point's positions
};


#endif

