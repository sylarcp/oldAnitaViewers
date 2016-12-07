/* 
 * BaseDBhandler.h
 */

#ifndef _BASEDBHANDLERH_
#define _BASEDBHANDLERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BaseDBHandler.cpp"
#endif

#include <libpq-fe.h>
#include <wx/event.h>

// Event used to make SQL queries
BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxSQL_REQUEST,-1)
END_DECLARE_EVENT_TYPES()

// Define type for parser function pointer
typedef void (*sqlparser)(wxObject *object,PGresult *result);

class BaseDBHandler{
 public:
  BaseDBHandler();
  virtual ~BaseDBHandler();

  static void parseWrapper(wxObject *object,PGresult *result);

  bool ParseLongSQLArray(PGresult *result,long *target,int n,int col,int row=0);
  bool ParseLongSQLArray2D(PGresult *result,long **target,int n,long *m,int col,int row=0);
  bool ParseDoubleSQLArray(PGresult *result,double *target,int n,int col,int row=0);

 protected:
  virtual void sqlParser(PGresult *result) = 0; // True virtual
};


#endif // _BASEDBHANDLERH_
