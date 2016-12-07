/*
 * TextInfo.h
 * 
 * Handles colored text items
 */

#ifndef _TEXTINFO_H_
#define _TEXTINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TextInfo.cpp"
#endif

#include <wx/panel.h>
#include "rangedstattext.h"

class TextInfo: public wxPanel{
 public:
  TextInfo();
  // Constructor
  TextInfo(wxWindow* parent,int nItem, int itemPerCol, 
	   const wxString &boxLabel, const wxString &units,
	   const wxString *labels, const wxString *captions,
	   const wxString *dbVar, const wxString *dbTable, 
	   wxRangeSpec **rangeSpecs, const wxString &format=wxT("%.2f"));

  // Destructor
  ~TextInfo();

  // Various functions to set data fields
  void SetDisplayValue(const wxString &label, double val);
  void SetDisplayRangeSpec(const wxString &label,const wxRangeSpec *rangeSpec);

 protected:

 private:
  wxRangedStaticText **m_Display;
  wxArrayString m_labels;
  wxString m_format;

  DECLARE_CLASS( TextInfo )
};

#endif //_TEXTINFO_H_
    
 




