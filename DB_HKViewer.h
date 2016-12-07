/*
 * HKViewer.h
 */

#ifndef _DBHKVIEWERH_
#define _DBHKVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DB_HKViewer.cpp"
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
#define TERM13_CLEAR_BUTTON_ID   100013
#define SAVE_FILE13_ID  110013
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class DB_HKViewer:public wxDialog{
 public:
  DB_HKViewer(wxWindow *parent);
  ~DB_HKViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event); 
void DB_HKData(const DBHK_t &dbhk);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(DB_HKViewer)
};

#endif // _ADU5PATVIEWERH_






