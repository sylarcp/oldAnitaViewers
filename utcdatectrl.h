/*
 * utcdatectrl.h
 */

#ifndef _UTCDATECTRLH_
#define _UTCDATECTRLH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "utcdatectrl.cpp"
#endif

#include <wx/textctrl.h>
#include <wx/datetime.h>

class wxUTCDateCtrl:public wxTextCtrl{
 public:
  wxUTCDateCtrl();
  wxUTCDateCtrl(wxWindow* parent, wxWindowID id, const wxString& value = wxT(""), 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, 
		long style = 0, 
		const wxValidator& validator = wxDefaultValidator, 
		const wxString& name = wxT("wxutcdatectrl"));
  time_t GetTicks(int line=0);

 private:
  static time_t tzOffset;
};

#endif // _UTCDATECTRLH_
