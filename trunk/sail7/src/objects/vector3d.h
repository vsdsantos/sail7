/****************************************************************************

    CVector Class
	Copyright (C) 2008 Andre Deperrois 

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


#include <math.h>

#ifndef CVector_H
#define CVector_H

class Vector3d
{
public: 
	double x;
	double y;
	double z;

	//inline constructors
       Vector3d()
	{
		x  = 0.0;
		y  = 0.0;
		z  = 0.0;
       }

       Vector3d(double const &xi, double const &yi, double const &zi)
	{
		x  = xi;
		y  = yi;
		z  = zi;
       }

       float xf() const {return float(x);}
       float yf() const {return float(y);}
       float zf() const {return float(z);}

	//inline operators
	double operator[](const int &i)
	{
		if(i==0) return x;
		if(i==1) return y;
		if(i==2) return z;
              return 0.0;
	}

       bool operator ==(Vector3d const &V)
	{
		//used only to compare point positions
		return (V.x-x)*(V.x-x) + (V.y-y)*(V.y-y) + (V.z-z)*(V.z-z)<0.000000001;
       }
	

       void operator+=(Vector3d const &T)
	{
		x += T.x;
		y += T.y;
		z += T.z;
       }
	
       void operator-=(Vector3d const &T)
	{
		x -= T.x;
		y -= T.y;
		z -= T.z;
       }

	void operator*=(double const &d)
	{
		x *= d;
		y *= d;
		z *= d;
       }
				   
       Vector3d operator *(double const &d)
	{
              Vector3d T(x*d, y*d, z*d);
		return T;
       }
	
       Vector3d operator *(Vector3d const &T)
	{
              Vector3d C;
		C.x =  y*T.z - z*T.y;
		C.y = -x*T.z + z*T.x;
		C.z =  x*T.y - y*T.x;
		return C;
       }
	
       Vector3d operator /(double const &d)
	{
              Vector3d T(x/d, y/d, z/d);
		return T;
       }
	
       Vector3d operator +(Vector3d const &V) const
	{
              Vector3d T(x+V.x, y+V.y, z+V.z);
		return T;
       }
	

       Vector3d operator -(Vector3d const &V) const
	{
              Vector3d T(x-V.x, y-V.y, z-V.z);
		return T;
       }

	
	//inline methods
       void Copy(Vector3d const &V)
	{	
		x = V.x;
		y = V.y;
		z = V.z;
       }
	
	void Set(double const &x0, double const &y0, double const &z0)
	{	
		x = x0;
		y = y0;
		z = z0;
       }
	
       void Set(Vector3d const &V)
	{	
		x = V.x;
		y = V.y;
		z = V.z;
       }

	void Normalize()
	{
		double abs = VAbs();
		if(abs< 1.e-10) return;
		x/=abs;
		y/=abs;
		z/=abs;
       }
		
	double VAbs()
	{
		return sqrt(x*x+y*y+z*z);
       }
	
       double dot(Vector3d const &V)
	{	
		return x*V.x + y*V.y + z*V.z;
       }
	
       bool IsSame(Vector3d const &V)
	{
		//used only to compare point positions
		return (V.x-x)*(V.x-x) + (V.y-y)*(V.y-y) + (V.z-z)*(V.z-z)<0.000000001;
       }

       void Translate(Vector3d const &T)
	{
		x += T.x;
		y += T.y;
		z += T.z;
       }

	void Translate(const double &tx, const double &ty, const double &tz)
	{
		x += tx;
		y += ty;
		z += tz;
       }

	int size() const
	{
		return 3;//dimension
	}

	//other methods
       void Rotate(Vector3d const &R, double Angle);
       void Rotate(Vector3d &O, Vector3d const &R, double Angle);
       void RotateX(Vector3d const &O, double XTilt);
       void RotateY(Vector3d const &O, double YTilt);
       void RotateZ(Vector3d const &O, double ZTilt);
	void RotateX(double Bank);
	void RotateY(double YTilt);
	void RotateZ(double ZRot);

 
};

#endif
