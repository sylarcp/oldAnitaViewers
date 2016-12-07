/*
 * historydisplay.h
 */

#ifndef _HISTORYDISPLAYH_
#define _HISTORYDISPLAYH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "historydisplay.cpp"
#endif

#include "multigraphdialog.h"

#define wxDEFAULT_HISTORY_BUFFER 100

/* Class that stores given number of values and memorizes last N_BUFFER 
   instances of stored variables. It can display them on request, but
   the actual invocation is left up to derived classes. 
*/
class wxHistoryDisplay{
 public:
  wxHistoryDisplay(int dim=1,const wxString &name=_T("historydisplay"),
		   const wxString *xUnits=NULL,
		   const wxString *yUnits=NULL,
		   const wxString *dbVar=NULL,
		   const wxString &dbTable=_T(""),
		   unsigned int buffer=wxDEFAULT_HISTORY_BUFFER);
  ~wxHistoryDisplay();

  void SetValue(double val){SetValues(&val);}
  void SetValues(double *val);
  double GetValue(unsigned int dim,unsigned int navg=1);
  
 protected:
  void _NewCurve();  

  // Past value memory related variables
  bool wrap;
  unsigned int m_dim;
  unsigned int m_nBuffer;
  double **m_buffer;
  unsigned int m_iBuffer;
  wxGraphCurve **dataCurve;

  // Graph and SQL related variables
  wxString m_name;
  wxString *m_xUnits;
  wxString *m_yUnits;
  wxString *m_dbVar;
  wxString m_dbTable;

  // Display graph
  wxMultiGraphDialog *m_graph;
};

#endif // _HISTORYDISPLAYH_
