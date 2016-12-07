/*
 * rangedstattext.h
 */

#ifndef _RANGEDSTATTEXTH_
#define _RANGEDSTATTEXTH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "rangedstattext.cpp"
#endif

#include "range.h"
#include "historystattext.h"

class wxRangedStaticText: public wxHistoryStaticText{
 public:
  wxRangedStaticText(){}
  wxRangedStaticText(wxWindow *parent,
		     wxWindowID id,
		     const wxRangeSpec *rangeSpec,
		     const wxString &format=_T("%.2f"),
		     const wxPoint &pos=wxDefaultPosition,
		     const wxSize &size=wxDefaultSize,
		     long style = 0,
		     const wxString &name=_T("rangedstatictext"),
		     const wxString &xUnits=_T(""),
		     const wxString &yUnits=_T(""),
		     const wxString &dbVar=_T(""),
		     const wxString &dbTable=_T(""),
		     unsigned int buffer=wxDEFAULT_HISTORY_BUFFER);
  
  ~wxRangedStaticText();

  void SetLabel(double val);
  void SetRangeSpec(const wxRangeSpec *rangeSpec);
  
  const wxRangeSpec *GetRangeSpec(){return m_rangeSpec;}
 private:
  static wxColour m_OutOfRange_Colour;
  const wxRangeSpec *m_rangeSpec;

  DECLARE_DYNAMIC_CLASS( wxRangedStaticText )
};
  
#endif // _RANGEDSTATTEXTH_
