/*
 * historystattext.h
 */

#ifndef _HISTORYSTATICTEXTH_
#define _HISTORYSTATICTEXTH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "historystatictext.cpp"
#endif

#include "historydisplay.h"
#include <wx/stattext.h> 

class wxHistoryStaticText: public wxStaticText, public wxHistoryDisplay{
 public:
  wxHistoryStaticText(){}
  wxHistoryStaticText(wxWindow *parent,
		      wxWindowID id,
		      int dim=1,
		      const wxString &format=_T("%.2f"),
		      const wxString &separator=_T(" "),
		      const wxPoint &pos=wxDefaultPosition,
		      const wxSize &size=wxDefaultSize,
		      long style = 0,
		      const wxString &name=_T("historystatictext"),
		      const wxString *xUnits=NULL,
		      const wxString *yUnits=NULL,
		      const wxString *dbVar=NULL,
		      const wxString &dbTable=_T(""),
		      unsigned int buffer=wxDEFAULT_HISTORY_BUFFER);
  wxHistoryStaticText(wxWindow *parent,
		      wxWindowID id,
		      const wxString &format=_T("%.2f"),
		      const wxString &name=_T("historystatictext"),
		      const wxString &xUnits=_T(""),
		      const wxString &yUnits=_T(""),
		      const wxString &dbVar=_T(""),
		      const wxString &dbTable=_T(""),
		      unsigned int buffer=wxDEFAULT_HISTORY_BUFFER);
  
  
  ~wxHistoryStaticText();

  void SetLabel(double val){SetLabel(&val);}
  void SetLabel(double *val);

  /* Event handlers */
  void OpenGraph( wxMouseEvent& event );

 protected:
  DECLARE_EVENT_TABLE()

  wxString m_format;
  wxString m_separator;

  DECLARE_DYNAMIC_CLASS(HistoryStaticText)
};

#endif // _HISTORYSTATICTEXTH_
