/*
 * BarInfo.h
 * 
 * Handles bar like info display about event
 */

#ifndef _BARINFO_H_
#define _BARINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BarInfo.cpp"
#endif

#include <wx/panel.h>
#include <wx/arrstr.h>
#include <wx/sizer.h>
#include "BarItem.h"

class BarInfo: public wxPanel{
 public:
  BarInfo(){Init();}
  // Constructor
  BarInfo(wxWindow* parent,int nItem, int itemPerRow,
	  const wxString &boxLabel, const wxString &units,wxString *labels, 
	  wxString *refLabels,wxString *captions,
	  const wxString *dbVar, const wxString *dbTable,
	  wxRangeSpec **rangeSpecs, double **ranges, unsigned int *avgs,
	    ScaleType *scaleTypes,const wxSize &size=wxDefaultSize, 
	  const wxString yScale=wxT("") ){
    Init();
    Create(parent,nItem,itemPerRow,boxLabel,units,labels,refLabels,captions,
	   dbVar,dbTable,rangeSpecs,ranges,avgs,scaleTypes,size,yScale);
  }

  bool Create(wxWindow* parent,int nItem, int itemPerRow,
	      const wxString &boxLabel, const wxString &units,wxString *labels,
	      wxString *refLabels,wxString *captions,
	      const wxString *dbVar, const wxString *dbTable,
	      wxRangeSpec **rangeSpecs, double **ranges, unsigned int *avgs,
	      ScaleType *scaleTypes,
	      const wxSize &size=wxDefaultSize, const wxString yScale=wxT(""));

  // Destructor
  ~BarInfo();

  // Various functions to set data fields
  void SetDisplayValue(const wxString &label, double val);
  void SetDisplayRange(const wxString &label, double low, double high);
  void SetDisplayRangeSpec(const wxString &label,const wxRangeSpec *rangeSpec);
  void SetDisplayScaleType(const wxString &label, ScaleType type);
  void SetDisplayAverage(const wxString &label, unsigned int avg);
  void SetDisplayBackground(const wxString &label, const wxColour &colour);
  void SetScale(const wxString &yScale);

 private:
  BarItem **m_Display;
  wxArrayString m_labels;
  int nRows;
  wxBoxSizer **m_scale;
  
  DECLARE_CLASS( BarInfo )
};

#endif //_BARINFO_H_
    
 




