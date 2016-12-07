/* Class definition for 3D rotation matrix
 * $Id: rotationMatrix.h,v 1.4 1999/06/18 19:41:59 ped Exp $
 * Written by: Predrag Miocinovic, UC Berkeley
 * Created on: 1998-06-01
 * Conntact: predrag@physics.berkeley.edu
 * History: Eview 3.0  1998-06-01   
 * $Log: rotationMatrix.h,v $
 * Revision 1.4  1999/06/18 19:41:59  ped
 * Printing, animation, track view provided. Lot of bug fixes. Dicumentation
 * fixed. Ready for 3.0 beta realease.
 *
 * Revision 1.3  1999/05/21  01:34:52  ped
 * Big rework of code; Observer class added, full I/O support, organized global
 * variables into namespaces, include files optimized for fastest compilation.
 *
 * Revision 1.2  1999/03/17  01:16:09  ped
 * Integration with I/O library event++, file name changes, addition of some user and private variables
 *
 * NOTE: Elements of this class based on original Eview sources 
 *       (versions 2.3 and earlier)
 * WARNING: This class assumes orthogonal (i.e. rotation) matrices
 */  

#ifndef ROTATIONMATRIX_H
#define ROTATIONMATRIX_H

#include "vector.h"

/** Class defining 3D rotation matrix. Requirements are that the matrix is
 * unitary. */
class RotationMatrix {
public: 
  /// Constructs identity matrix
  RotationMatrix();

  /// Constructs matrix, initialized by 3x3 unitary array
  RotationMatrix(const double[3][3]);  

  /// Constructs matrix, initialized by given values
  RotationMatrix(const double,const double,const double,const double,
		 const double,const double,const double,const double,
		 const double);

  /** Constructs matrix as rotation between two sets of vectors
   *  i.e. initial direction and facing, and final direction and facing
   */
  RotationMatrix(const Vector &,const Vector &,const Vector &,const Vector &);

  /// Destructor
  virtual ~RotationMatrix();

  /** Inverts the matrix. This is also the transpose for unitary matrices. 
   * @return Inverted matrix */
  RotationMatrix &inverse();

  /// Resets matrix to identity matrix
  void reset();

  /** Rotates matrix by {\em double} degrees about 0==x, 1==y, 2==z axis. 
   * If {\em int} > 2, no action is taken 
   * @param double Rotation angle
   * @param int Rotation axis */
  void rotate(double,int);

  /** Rotates matrix by supplied rotation matrix
   * @param RotationMatrix Unitary rotation matrix */
  void rotate(const RotationMatrix &);  

  /** Applies inverse rotation to a point given. 
   * @param Vector Coordinates of a point to rotate
   * @Return Coordinates of rotated point */
  Vector antiRotatePoint(const Vector); 

  /** Applies rotation to a point given. 
   * @param Vector Coordinates of a point to rotate
   * @Return Coordinates of rotated point */
  Vector rotatePoint(const Vector); 

  /** Explicitly sets matrix elements. These should for a unitary matrix. */
  void setMatrix(const double=1,const double=0,const double=0,const double=0,
		 const double=1,const double=0,const double=0,const double=0,
		 const double=1); 
                       
  /** Sets matrix equal to array provided. The elements of an array should
   * form a unitary matrix */
  void setMatrix(const double[3][3]);

  /** @name Methods to inspect contents of a matrix */
  //@{ 
  /// Returns (1,1) element
  inline double e11() const {return(element[0][0]);}  
  /// Returns (1,2) element
  inline double e12() const {return(element[0][1]);}  
  /// Returns (1,3) element
  inline double e13() const {return(element[0][2]);}  
  /// Returns (2,1) element
  inline double e21() const {return(element[1][0]);}  
  /// Returns (2,2) element
  inline double e22() const {return(element[1][1]);}  
  /// Returns (2,3) element
  inline double e23() const {return(element[1][2]);}  
  /// Returns (3,1) element
  inline double e31() const {return(element[2][0]);}  
  /// Returns (3,2) element
  inline double e32() const {return(element[2][1]);}  
  /// Returns (3,3) element
  inline double e33() const {return(element[2][2]);}  
  //@}

private:
  /** Check is array provided forms a unitary matrix
   * @return TRUE if unitary, FALSE otherwise */
  bool orthogonal(const double[3][3]); 

  /// Rotation matrix elements
  double element[3][3];
};

#endif //ROTATIONMATRIX_H








