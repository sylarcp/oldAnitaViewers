/* 3d vector class with all possible operators, taken from Eview GUI written 
 * for AMANDA collaboration. Used heavily in Payload View.

/* Vector class functions 
 * $Id: vector.cc,v 1.5 2001/08/29 18:24:04 ped Exp $
 * $Log: vector.cc,v $
 * Revision 1.5  2001/08/29 18:24:04  ped
 * Fixed up some conflicts that emerged from addition of hit plot window (cds). Changed signals/slots and redrawing sequence for cds. Made cds compliant with color inversion (printing inversion still questionable). Moved button delay selection to motion window.
 *
// Revision 1.4  1999/06/18  19:42:11  ped
// Printing, animation, track view provided. Lot of bug fixes. Dicumentation
// fixed. Ready for 3.0 beta realease.
//
// Revision 1.3  1999/06/05  00:11:54  ped
// Lot of changes. This is now 3.0 alpha version.
//
// Revision 1.2  1999/05/21  01:35:05  ped
// Big rework of code; Observer class added, full I/O support, organized global
// variables into namespaces, include files optimized for fastest compilation.
//
// Revision 1.1  1999/03/17  01:16:20  ped
// Integration with I/O library event++, file name changes, addition of some user and private variables
//
 */

#include <cmath>

#include "vector.h"

/****************************************************************************/
/* getAngles.cpp                                                            */
/****************************************************************************/
/* Written: 1997-06-05                                                      */
/* Written by: Predrag Miocinovic, UC Berkeley                              */
/* Contact: ped@grimm.berkeley.edu                                          */
/* Purpose: Converts vector direction to polar angles                       */
/* Assumptions: Arccosine function returns values in zero to pi range       */
/* Input: Vector direction                                                  */
/* Output: Polar angles                                                     */
/* History: GetAngles.c 1.0   1997-06-05                                    */
/*          getAngles.c 1.3   1997-07-14                                    */
/*          getAngles.c               2.0    1997-09-25                     */
/*          Vector::angles.cpp  3.0   1998-06-08                            */

/* Calculates and returns the theta and phi angles of a vector */
void Vector::angles(double &theta,double &phi)
{
  Vector dir(X,Y,Z);      //Create a copy of the vector
  dir.normalize();        //Normalize the direction

  theta=acos(dir.z());
  if(theta<0) theta+=M_PI;
  if((fabs(theta)<ZERO)||(fabs(theta-M_PI)<ZERO)) 
    phi=0;              //phi is undefined, so set zero for it
  else{
    double cosPhi=dir.x()/sin(theta);
    double sinPhi=dir.y()/sin(theta);
    if(fabs(sinPhi)<ZERO) sinPhi=0;  //Avoid -0.0
    if(cosPhi> 1) cosPhi=1;    //Avoid some funny round off
    if(cosPhi<-1) cosPhi=-1;   //Avoid some funny round off, again
    if(sinPhi>=0)
      phi=acos(cosPhi);
    else 
      phi=2*M_PI-acos(cosPhi);
  }
  phi=Degree(phi);
  theta=Degree(theta);
  return;
}

/****************************************************************************/
/* normalize.cpp                                                            */
/****************************************************************************/
/* Rewritten by: Patrik Ekstrom, Stockholm University.                      */
/*               Predrag Miocinovic, UC Berkeley.                           */
/* Contact: ekstrom@physto.se                                               */
/*          ped@grimm.berkeley.edu                                          */
/* Purpose: Normalizes the vector                                           */
/* Assumptions: The coordinates are addressed directly.                     */
/* Input: The three coordinates of the vector to be normalized.             */
/* Output: The three normalized coordinates of the vector.                  */
/* History: Normalize.c   1.1    1994-11-01                                 */
/*          normalize.c   2.0    1997-02-27                                 */
/*          normalize.c   2.1    1997-07-16                                 */
/*          normalize.c               2.0    1997-09-25                     */
/*          Vector::normalize.cpp 3.0    1998-06-08                         */

void Vector::normalize()
{
/* First calculate the length of the vector and then use that to normalize  */
/* the inputted coordinates which are addressed directly.                   */

  double normalization=sqrt(sqr(X)+sqr(Y)+sqr(Z));
  if(normalization>0){
    X/=normalization;
    Y/=normalization;
    Z/=normalization;
  }
  return;
}
  
/* Sets vector coordinates from angles given */
void Vector::set(double theta,double phi)
{
  X = sin(Radian(theta))*cos(Radian(phi));
  Y = sin(Radian(theta))*sin(Radian(phi));
  Z = cos(Radian(theta));
  // Avoid -0.0
  if(fabs(X)<ZERO) X=0;
  if(fabs(Y)<ZERO) Y=0;
  if(fabs(Z)<ZERO) Z=0;
  normalize();   // Just in case
}

