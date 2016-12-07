/*
 * SlowMoViewer.h
 */

#ifndef _SLOWMOVIEWERH_
#define _SLOWMOVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SlowMoViewer.cpp"
#endif

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/file.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include "utcdatectrl.h"
#include <includes/anitaStructures.h>

//#include "BaseDBHandler.h"

//#define PREV_BUTTON_ID   10
//#define NEXT_BUTTON_ID   11
#define TERM12_CLEAR_BUTTON_ID   100012
#define SAVE_FILE12_ID  110012
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class SlowMoViewer:public wxDialog{
 public:
  SlowMoViewer(wxWindow *parent);
  ~SlowMoViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event); 
void SlowMoData(const SlowRateFull_t &surf);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(SlowMoViewer)
};

#endif // _SLOWMOVIEWERH_








