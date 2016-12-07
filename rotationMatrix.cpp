/* RotationMatrix class that implements all possible calculations associated 
 * with 3d rotations between coordinate frames. Taken from Eview GUI written
 * for AMANDA collaboration. Some bug fixes, though!!!
 * Used in Payload View to construct payload from antenna elements.
 */ 
/* RotationMatrix class functions 
 * $Id: rotationMatrix.cc,v 1.4 2003/06/29 16:34:33 ped Exp $
 * Written by: Predrag Miocinovic, UC Berkeley
 * Created on: 1998-06-01
 * Conntact: predrag@physics.berkeley.edu
 * History: Eview 3.0  1998-06-01   
 * $Log: rotationMatrix.cc,v $
 * Revision 1.4  2003/06/29 16:34:33  ped
 * Upgraded to compile with gcc3 and is now compatible with Qt3.x
 *
 * Revision 1.3  1999/06/18 19:41:58  ped
 * Printing, animation, track view provided. Lot of bug fixes. Dicumentation
 * fixed. Ready for 3.0 beta realease.
 *
// Revision 1.2  1999/05/21  01:34:52  ped
// Big rework of code; Observer class added, full I/O support, organized global
// variables into namespaces, include files optimized for fastest compilation.
//
// Revision 1.1  1999/03/17  01:16:08  ped
// Integration with I/O library event++, file name changes, addition of some user and private variables
//
 */  

#include "rotationMatrix.h"

/* Default constructor, creates unit matrix */
RotationMatrix::RotationMatrix(){
  reset();
}

/* Constructor that initializes matrix to given 3x3 array */
RotationMatrix::RotationMatrix(const double array[3][3]){
  setMatrix(array);
}

/* Constructor that sets matrix elements equal to arguments given */
RotationMatrix::RotationMatrix(const double e11,const double e12,
			       const double e13,const double e21,
			       const double e22,const double e23,
			       const double e31,const double e32,
			       const double e33){
  setMatrix(e11,e12,e13,e21,e22,e23,e31,e32,e33);
}

/* Constructor that calcultes matrix required to go from one pair of vectors
 * to other pair of vectors */
RotationMatrix::RotationMatrix(const Vector &a1,const Vector &b1,
			       const Vector &a2,const Vector &b2){
  
  Vector u;
  double th=0;
  if((a1!=a2)&&(b1!=b2)){
    // Compute rotation axis
    u=(a1-a2)%(b1-b2);
    u.normalize();

    //Compute rotation angle
    Vector t1=u%a1;
    Vector t2=u%a2;
    th=acos((t1*t2)/(t1.magnitude()*t2.magnitude()));
    
    // Correct if rotation more than 180 degrees
    Vector z(0,0,1);
    if((a1%a2)*z<0 || (b1%b2)*z<0) th*=-1;
  }else if(a1==a2){
    u=b1%b2;
    th=acos(b1*b2);
  }else if(b2==b2){
    u=a1%a2;
    u.normalize();
    th=acos(a1*a2);
  }

  double costh=cos(th);
  double sinth=sin(th);	       
  double costhm1=1-costh;

  // Set rotation matrix elements
  setMatrix(costh+costhm1*sqr(u.x()),u.x()*u.y()*costhm1-u.z()*sinth,
	    u.x()*u.z()*costhm1+u.y()*sinth,
	    u.x()*u.y()*costhm1+u.z()*sinth,costh+costhm1*sqr(u.y()),
	    u.y()*u.z()*costhm1-u.x()*sinth,
	    u.x()*u.z()*costhm1-u.y()*sinth,u.y()*u.z()*costhm1+u.x()*sinth,
	    costh+costhm1*sqr(u.z()));
}

RotationMatrix::~RotationMatrix(){}

/* Check's if 3x3 array is valid rotational matrix */
bool RotationMatrix::orthogonal(const double array[3][3]){
  double det = array[0][0]*(array[1][1]*array[2][2]-array[2][1]*array[1][2])
             -array[0][1]*(array[1][0]*array[2][2]-array[2][0]*array[1][2])
             +array[0][2]*(array[1][0]*array[2][1]-array[2][0]*array[1][1]);
  return fabs(det-1.0)<ZERO;
}

/* Inverts the matrix and returns the reference */
RotationMatrix &RotationMatrix::inverse(){
  double iarray[3][3];

  for(int i=0;i<3;++i)
    for(int j=0;j<3;++j)
      iarray[i][j]=element[j][i];
  for(int i=0;i<3;++i)
    for(int j=0;j<3;++j)
      element[i][j]=iarray[i][j];

  return(*this);
}

/* Resets the matrix to unit matrix */
void RotationMatrix::reset(){
  for(int i=0;i<3;++i)
    for(int j=0;j<3;++j){
      if(i==j)
	element[i][j]=1.0;
      else
	element[i][j]=0.0;
    }
}

/* Set matrix elements */
void RotationMatrix::setMatrix(const double e11,const double e12,
			       const double e13,const double e21,
			       const double e22,const double e23,
			       const double e31,const double e32,
			       const double e33){
  element[0][0]=e11;
  element[0][1]=e12;
  element[0][2]=e13;
  element[1][0]=e21;
  element[1][1]=e22;
  element[1][2]=e23;
  element[2][0]=e31;
  element[2][1]=e32;
  element[2][2]=e33;
  if(!orthogonal(element)){
    // Integrate with wxLog later
    //cerr << "Internal error: Non-orthogonal rotation matrix.\n";
    reset(); //If not orthogonal, default to identity 
  }
}

/* Sets matrix to given 3x3 array */
void RotationMatrix::setMatrix(const double array[3][3]){
  if(orthogonal(array))           //Acceptable array
    for(int i=0;i<3;++i)
      for(int j=0;j<3;++j)
	element[i][j]=array[i][j];
  else{
    // Integrate with wxLog later
    //message("Internal error: Non-orthogonal rotation matrix.");
    reset();                   //Default to identity matrix
  }
}

/* Rotates matrix by given angle about given axis */
void RotationMatrix::rotate(double angle,int axis){
  double matrix[3][3];
  double sinAng = sin(Radian(angle));
  double cosAng = cos(Radian(angle));

  // Create rotation matrix
  switch(axis){
  case 0: //x-axis rotation (roll)
    matrix[0][0]=1;        matrix[0][1]=0;        matrix[0][2]=0;
    matrix[1][0]=0;        matrix[1][1]= cosAng;  matrix[1][2]=sinAng;
    matrix[2][0]=0;        matrix[2][1]=-sinAng;  matrix[2][2]=cosAng;
    break;
  case 1: //y-axis rotation (pitch)
    matrix[0][0]=cosAng;  matrix[0][1]=0;        matrix[0][2]=sinAng;
    matrix[1][0]=0;       matrix[1][1]=1;        matrix[1][2]=0;
    matrix[2][0]=-sinAng;  matrix[2][1]=0;        matrix[2][2]= cosAng;
    break;
  case 2: //z-axis rotation (yaw)
    matrix[0][0]= cosAng;  matrix[0][1]=sinAng;   matrix[0][2]=0;
    matrix[1][0]=-sinAng;  matrix[1][1]=cosAng;   matrix[1][2]=0;
    matrix[2][0]=0;        matrix[2][1]=0;        matrix[2][2]=1;
    break;
  default:
    return;         // Invalid axis, get out
  }
  
  rotate(RotationMatrix(matrix));
  return;
}

/* Roates current matrix by a give matrix */
void RotationMatrix::rotate(const RotationMatrix &a){
  double temp[3][3];

  //Multiply matrices
  temp[0][0]=a.e11()*element[0][0]+a.e12()*element[1][0]+a.e13()*element[2][0];
  temp[0][1]=a.e11()*element[0][1]+a.e12()*element[1][1]+a.e13()*element[2][1];
  temp[0][2]=a.e11()*element[0][2]+a.e12()*element[1][2]+a.e13()*element[2][2];
  temp[1][0]=a.e21()*element[0][0]+a.e22()*element[1][0]+a.e23()*element[2][0];
  temp[1][1]=a.e21()*element[0][1]+a.e22()*element[1][1]+a.e23()*element[2][1];
  temp[1][2]=a.e21()*element[0][2]+a.e22()*element[1][2]+a.e23()*element[2][2];
  temp[2][0]=a.e31()*element[0][0]+a.e32()*element[1][0]+a.e33()*element[2][0];
  temp[2][1]=a.e31()*element[0][1]+a.e32()*element[1][1]+a.e33()*element[2][1];
  temp[2][2]=a.e31()*element[0][2]+a.e32()*element[1][2]+a.e33()*element[2][2];
  //Copy result to permanent matrix
  for(int i=0;i<3;++i)
    for(int j=0;j<3;++j)
      element[i][j]=temp[i][j];
  
  return; 
}

/* Rotates given vector by a current rotation matrix */
Vector RotationMatrix::rotatePoint(const Vector point){
  return(Vector(point.x()*element[0][0]+
		point.y()*element[0][1]+
		point.z()*element[0][2],
		point.x()*element[1][0]+
		point.y()*element[1][1]+
		point.z()*element[1][2],
		point.x()*element[2][0]+
		point.y()*element[2][1]+
		point.z()*element[2][2]));
}

/* Rotates given vector by a current antirotation matrix */
Vector RotationMatrix::antiRotatePoint(const Vector point){
  return(Vector(point.x()*element[0][0]+
		point.y()*element[1][0]+
		point.z()*element[2][0],
		point.x()*element[0][1]+
		point.y()*element[1][1]+
		point.z()*element[2][1],
		point.x()*element[0][2]+
		point.y()*element[1][2]+
		point.z()*element[2][2]));
}


