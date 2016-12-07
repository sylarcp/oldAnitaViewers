/*
 * CommandViewer.h
 */

#ifndef _COMMANDVIEWERH_
#define _COMMANDVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CommandViewer.cpp"
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
#define TERM6_CLEAR_BUTTON_ID   100006
#define SAVE_FILE6_ID  110006
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class CommandViewer:public wxDialog{
 public:
CommandViewer(wxWindow *parent);
  ~CommandViewer();

 void OnClear(wxCommandEvent &event);
 void SaveFile(wxCommandEvent &event); 
void CommandData(const CommandEcho_t &cmd);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
 // wxCheckBox *checkbox;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(CommandViewer)
};

#endif // _COMMANDVIEWERH_






