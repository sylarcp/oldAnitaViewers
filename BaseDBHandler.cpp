/*
 * BaseDBHandler.cpp
 * 
 * Definitions for object that implements hook for routing of database 
 * responses on SQL queries. It also provides parsers for array like SQL
 * entries.
 * Any class that wants to receive SQL query results must descend from this
 * class.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BaseDBHandler.h"
#endif

#include <wx/log.h>
#include "BaseDBHandler.h"

DEFINE_EVENT_TYPE(wxSQL_REQUEST)

BaseDBHandler::BaseDBHandler(){}
BaseDBHandler::~BaseDBHandler(){}
void BaseDBHandler::parseWrapper(wxObject *object,PGresult *result){
  if(BaseDBHandler* mySelf = dynamic_cast<BaseDBHandler*>(object)){
    mySelf->sqlParser(result);
  }else{
    wxLogError(_T("[BaseDBHandler::parseWrapper] Object %x is not of type BaseDBHandler"),(unsigned int)object);
  }
}

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

bool BaseDBHandler::ParseLongSQLArray(PGresult *result,long *target,int n,
				      int col,int row){
  bool retval=true;
  wxString str=_U(PQgetvalue(result,row,col));
  str=str.Mid(1).RemoveLast(); // Drop '{' and '}'
  if(!str.Length()) return false;
  int cexp;
  if((cexp=str.Freq(',')+1)!=n){
    wxLogError(_T("[BaseDBHandler::ParseLongSQLArray] Unexpected array length of %d (hoped for %d)."),cexp,n);
    wxLogError(str);
    retval=false;
  }
  int c=0;
  while(c<n && str.Length() && str.BeforeFirst(',').ToLong(&target[c])){
    str=str.AfterFirst(',');
    ++c;
  }
  if(c!=cexp){
    wxLogError(_T("[BaseDBHandler::ParseLongSQLArray] Processed %d array entries, expected %d."),c,cexp);
    retval=false;
  }
  return retval;
}

bool BaseDBHandler::ParseLongSQLArray2D(PGresult *result,long **target,int n,
					long *m,int col,int row){
  bool retval=true;
  wxString str=_U(PQgetvalue(result,row,col));
  str=str.AfterFirst('{').BeforeLast('}'); // Drop outer '{' and '}'
  int c1exp;
  if((c1exp=str.Freq('}'))!=n){
    wxLogError(_T("[BaseDBHandler::ParseLongSQLArray2D] Unexpected array first dimension length of %d (hoped for %d)."),c1exp,n);
    retval=false;
  }
  if(c1exp==0) return false;
  int c1=0;
  while(c1<n && str.Length()){
    wxString str2=str.Mid(1).BeforeFirst('}'); // This is now first subarray
    int c2exp;
    if((c2exp=str2.Freq(',')+1)!=m[c1]){
      wxLogError(_T("[BaseDBHandler::ParseLongSQLArray2D] Unexpected array second dimension length of %d (hoped for %ld)."),c2exp,m[c1]);
      retval=false;
    }
    int c2=0;
    while(c2<m[c1] && str2.Length() && str2.BeforeFirst(',').ToLong(&target[c1][c2])){
      str2=str2.AfterFirst(',');
      ++c2;
    }
    if(c2!=c2exp){
      wxLogError(_T("[BaseDBHandler::ParseLongSQLArray2D] Processed %d subarray entries, expected %d."),c2,c2exp);
      retval=false;
    }
    str=str.AfterFirst('}').AfterFirst(','); // Removes first subarray and trailing ','
    ++c1;
  }
  if(c1!=c1exp){
    wxLogError(_T("[BaseDBHandler::ParseLongSQLArray2D] Processed %d array entries, expected %d."),c1,c1exp);
    retval=false;
  }
  return retval;
}

bool BaseDBHandler::ParseDoubleSQLArray(PGresult *result,double *target,int n,
					int col,int row){
  bool retval=true;
  wxString str=_U(PQgetvalue(result,row,col));
  str=str.Mid(1).RemoveLast(); // Drop '{' and '}'
  if(!str.Length()) return false;
  int cexp;
  if((cexp=str.Freq(',')+1)!=n){
    wxLogError(_T("[BaseDBHandler::ParseDoubleSQLArray] Unexpected array length of %d (hoped for %d)."),cexp,n);
    wxLogError(str);
    retval=false;
  }
  int c=0;
  while(c<n && str.Length() && str.BeforeFirst(',').ToDouble(&target[c])){
    str=str.AfterFirst(',');
    ++c;
  }
  if(c!=cexp){
    wxLogError(_T("[BaseDBHandler::ParseDoubleSQLArray] Processed %d array entries, expected %d."),c,cexp);
    retval=false;
  }
  return retval;
}

