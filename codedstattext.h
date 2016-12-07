/*
 * codedstattext.h
 */

#ifndef _CODEDSTATICTEXTH_
#define _CODEDSTATICTEXTH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "codedstatictext.cpp"
#endif

#include "historystattext.h"

class wxCodedStaticText: public wxHistoryStaticText{
 public:
  wxCodedStaticText(){}
  wxCodedStaticText(wxWindow *parent,
		    wxWindowID id,
		    const wxString &codeMap=_T(""),
		    const wxString &name=_T("codedstatictext"),
		    const wxString &xUnits=_T(""),
		    const wxString &yUnits=_T(""),
		    const wxString &dbVar=_T(""),
		    const wxString &dbTable=_T(""),
		    unsigned int buffer=wxDEFAULT_HISTORY_BUFFER);
  
  
  ~wxCodedStaticText();

  void SetLabel(int val);

 protected:
  int nCode;
  long *m_code;
  wxString *m_codeText;

  DECLARE_DYNAMIC_CLASS(CodedStaticText)
};

#endif // _HISTORYSTATICTEXTH_
