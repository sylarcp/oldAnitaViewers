/*
 *  HistInfo.h
 */

#ifndef _HISTINFO_H_
#define _HISTINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "HistInfo.cpp"
#endif

#include "aviewdefs.h"
#include "BarInfo.h"

class HistInfo:protected BarInfo{
 public:
  HistInfo();
  // Constructor
  HistInfo(wxWindow* parent,int nItem, int itemPerRow,
	   const wxString &boxLabel, const wxString &units=wxT(""),
	   const wxSize &size=wxDefaultSize);
  //Destructor
  ~HistInfo();

  void Clear();
  void SetValues(double *val);
  void IncrementValues(double *val);

 private:
  int m_nItem;
  double *m_value;
  double m_scaleMin;;
  double m_scaleMax;;

  DECLARE_CLASS( HistInfo )
};

#endif // _HISTINFO_H_
