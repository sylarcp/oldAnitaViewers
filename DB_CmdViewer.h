/*
 * DB_CmdViewer.h
 */

#ifndef _DBMCMDVIEWERH_
#define _DBCMDVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DB_CmdViewer.cpp"
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
#define TERM16_CLEAR_BUTTON_ID   100016
#define SAVE_FILE16_ID  110016
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class DB_CmdViewer:public wxDialog{
 public:
  DB_CmdViewer(wxWindow *parent);
  ~DB_CmdViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event); 
void DB_CmdData(const DBCmd_t &dbcmd);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(DB_CmdViewer)
};

#endif // _DBCMDVIEWERH_









