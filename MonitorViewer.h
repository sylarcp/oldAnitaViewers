/*
 * MonitorViewer.h
 */

#ifndef _MONITORVIEWERH_
#define _MONITORVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MonitorViewer.cpp"
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
#define TERM1_CLEAR_BUTTON_ID   100001
#define SAVE_FILE1_ID  110001
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class MonitorViewer:public wxDialog{
 public:
  MonitorViewer(wxWindow *parent);
  ~MonitorViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event);
void MonitorData(const MonitorStruct_t &gps);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(MonitorViewer)
};

#endif // _GPSPOSVIEWERH_



