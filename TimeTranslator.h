/*
 * TimeTranslator.h
 */

#ifndef _TIMETRANSLATORH_
#define _TIMETRANSLATORH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TimeTranslator.cpp"
#endif

#include <wx/dialog.h>
#include "utcdatectrl.h"

#define TOUTC_BUTTON_ID    10
#define TOTICK_BUTTON_ID   11

#define UTCTIME_DISPLAY     1001
#define UNIXTICK_DISPLAY    1002

/*
 * Timetranslator class declaration
 */
class TimeTranslator: public wxDialog{    

  DECLARE_CLASS( TimeTranslator )
  
public:

  // Constructors
  TimeTranslator( wxWindow* parent, wxWindowID id, const wxString &caption, 
		  const wxPoint& pos = wxDefaultPosition, 
		  const wxSize& size = wxDefaultSize, 
		  long style = wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|
		  wxMINIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU);

  // Destructor 
  ~TimeTranslator();

  // Event handlers
  void OnToUTC(wxCommandEvent &event);
  void OnToTick(wxCommandEvent &event);

 protected:
  DECLARE_EVENT_TABLE()

 private:

  wxUTCDateCtrl *m_utcEntry;
  wxTextCtrl *m_tickEntry; 
};

#endif // _TIMETRANSLATORH_
