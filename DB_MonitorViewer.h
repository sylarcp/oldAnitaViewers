/*
 * DB_HeaderViewer.h
 */

#ifndef _DBMONITORVIEWERH_
#define _DBMONITORVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DB_MonitorViewer.cpp"
#endif

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/file.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include "utcdatectrl.h"
#include <includes/anitaStructures.h>
#include "GSE_Structures.h"

//#include "BaseDBHandler.h"

//#define PREV_BUTTON_ID   10
//#define NEXT_BUTTON_ID   11
#define TERM15_CLEAR_BUTTON_ID   100015
#define SAVE_FILE15_ID  110015
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class DB_MonitorViewer:public wxDialog{
 public:
  DB_MonitorViewer(wxWindow *parent);
  ~DB_MonitorViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event); 
void DB_MonitorData(const DBMon_t &dbmon);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(DB_MonitorViewer)
};

#endif // _DBMONITORVIEWERH_








