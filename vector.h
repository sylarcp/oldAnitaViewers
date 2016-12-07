/* Class definition for 3D vectors
 * $Id: vector.h,v 1.8 2001/08/29 23:05:23 ped Exp $
 * Written by: Predrag Miocinovic, UC Berkeley
 * Created on: 1998-06-08
 * Conntact: predrag@physics.berkeley.edu
 * History: Eview 3.0  1998-06-08   
 * $Log: vector.h,v $
 * Revision 1.8  2001/08/29 23:05:23  ped
 * Merged official and main branches
 *
 * Revision 1.7  2001/08/28  19:13:14  jodi
 * Updates to plot widget, including printing, documentation, added display
 * variables and general code cleanup.
 *
 * Revision 1.6.2.1  2001/07/31  22:24:14  ped
 * Upgraded to be gcc-3.0 compliant
 *
 * Revision 1.6  2000/02/16  19:22:36  ped
 * Fixed horizontal fit planeOffset "bug".
 *
 * Revision 1.5  1999/06/18  19:42:12  ped
 * Printing, animation, track view provided. Lot of bug fixes. Dicumentation
 * fixed. Ready for 3.0 beta realease.
 *
 * Revision 1.4  1999/05/21  01:35:06  ped
 * Big rework of code; Observer class added, full I/O support, organized global
 * variables into namespaces, include files optimized for fastest compilation.
 *
 * Revision 1.3  1999/03/17  01:16:21  ped
 * Integration with I/O library event++, file name changes, addition of some user and private variables
 *
 */  

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

#include "aviewdefs.h"
//#include <math.h>

/// Class that implements representation of a 3D mathematical vector.
class Vector {
public:
  /// Constructs null vector
  Vector(){
    set(0,0,0);
  }

  /// Constructs vector from direction angles given
  Vector(const double t,const double p){
    set(t,p);
  }
	    
  /// Constructs a vector from supplied values 
  Vector(const double xi,const double yi,const double zi){ 
    set(xi,yi,zi); 
  }

  /// Destructor
  virtual ~Vector(){}

  /// Set x-coordinate
  inline void setX(const double xi) {X=xi;}             
  /// Set y-coordinate
  inline void setY(const double yi) {Y=yi;}
  /// Set z-coordinate
  inline void setZ(const double zi) {Z=zi;}              
  /// Set vector coordinates
  inline void set(const double xi,const double yi,const double zi){
    X=xi;Y=yi;Z=zi;
  } 

  /** Set vector coordinates from direction angles given
   * @param Zenith angle
   * @param Azimuthal angle */
  void set(const double,const double); 

  /// Get x-coordinate  
  inline double x() const {return X;}
  /// Get y-coordinate
  inline double y() const {return Y;}              
  /// Get z-coordinate  
  inline double z() const {return Z;}              

  /** Get zenith and azimuthal direction angles of a vector
   * @param Zenith angle
   * @param Azimuth angle */
  void angles(double &,double &);

  /// Normalize a vector to the unit vector
  void normalize();             

  /// Get vector magnitude
  inline double magnitude() const {return sqrt(sqr(X)+sqr(Y)+sqr(Z));}

  /** Check if the vector is a null vector
   * @return TRUE if all vector components are equal to 0.0, FALSE otherwise
   */
  inline bool null() const {return (X==0.0)&&(Y==0.0)&&(Z==0.0);}

  /// Addition operator
  inline Vector &operator += (const Vector &a){
    X+=a.X;Y+=a.Y;Z+=a.Z;
    return *this;
  }

  /// Difference operator
  inline Vector &operator -= (const Vector &a){ 
    X-=a.X;Y-=a.Y;Z-=a.Z;
    return *this;
  }

  /// Multiplication operator
  inline Vector &operator *= (const double a){
    X*=a;Y*=a;Z*=a;
    return *this;
  }

  /// Division operator
  inline Vector &operator /= (const double a){
    if(!a) throw Zero_Division();
    X/=a;Y/=a;Z/=a;
    return *this;
  }

  /// Exception class
  struct Zero_Division{};					 

  /* Friend methods */
  friend inline Vector operator % (const Vector &,const Vector &);
  friend inline double operator * (const Vector &,const Vector &);
  friend inline Vector operator * (const double,const Vector &);
  friend inline Vector operator / (const Vector &,const double);
  friend inline Vector operator + (const Vector &,const Vector &);
  friend inline Vector operator - (const Vector &,const Vector &);
  friend inline Vector operator - (const Vector &);
  friend inline bool operator == (const Vector &,const Vector &);
  friend inline bool operator != (const Vector &,const Vector &); 

protected:
  /// Vector X component 
  double X; 
  /// Vector Y component 
  double Y; 
  /// Vector Z component 
  double Z; 
};

/** @name Vector friend methods */
//@{
/// Cross product
inline Vector operator % (const Vector &a,const Vector &b){
  return Vector(a.Y*b.Z-a.Z*b.Y,
		a.Z*b.X-a.X*b.Z,
		a.X*b.Y-a.Y*b.X);
}

/// Dot product
inline double operator * (const Vector &a,const Vector &b){
  return(a.X*b.X+a.Y*b.Y+a.Z*b.Z);
}

/// Multiplication
inline Vector operator * (const double a,const Vector &b){
  return Vector(a*b.X,a*b.Y,a*b.Z);
}

/// Division
inline Vector operator / (const Vector &a,const double b){
  if(!b) throw Vector::Zero_Division();
  return Vector(a.X/b,a.Y/b,a.Z/b);
}

/// Addition
inline Vector operator + (const Vector &a,const Vector &b){
  return Vector(a.X+b.X,a.Y+b.Y,a.Z+b.Z);
}

/// Difference
inline Vector operator - (const Vector &a,const Vector &b){
  return Vector(a.X-b.X,a.Y-b.Y,a.Z-b.Z);
}

/// Negative
inline Vector operator - (const Vector &a){
  // Avoiding -0.0 stuff
  return Vector(fabs(a.X)<ZERO?0:-a.X,           
		fabs(a.Y)<ZERO?0:-a.Y,
		fabs(a.Z)<ZERO?0:-a.Z);
}

/// Equality
inline bool operator == (const Vector &a,const Vector &b){ 
  return (a.X==b.X)&&(a.Y==b.Y)&&(a.Z==b.Z);
}

/// Inequality
inline bool operator != (const Vector &a,const Vector &b){ 
  return !(a==b);
}
//@}
#endif  //VECTOR_H
