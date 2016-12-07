/*
 * ConfigViewer.h
 */

#ifndef _CONFIGVIEWERH_
#define _CONFIGVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ConfigViewer.cpp"
#endif

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include "utcdatectrl.h"
#include "BaseDBHandler.h"

#define PREV_BUTTON_ID   10
#define NEXT_BUTTON_ID   11

#define FILE_SELECT_ID   101

#define TIME_DISPLAY_ID  1001

#define N_CONFIG_FILE_TYPES  18

class ConfigViewer:public wxDialog,public BaseDBHandler{
 public:
  ConfigViewer(wxWindow *parent);
  ~ConfigViewer();

  void FileSelect(wxCommandEvent &event); 
  void TimeSelect(wxCommandEvent &event); 
  void Next(wxCommandEvent &event);
  void Prev(wxCommandEvent &event);
  
 protected:
  DECLARE_EVENT_TABLE()
  void SendSQLCmd(wxString cmd);  
  void sqlParser(PGresult *result);

 private:
  wxTextCtrl *m_display;
  wxChoice *m_fileList;
  wxUTCDateCtrl *m_timeDisplay;
  wxCheckBox *m_timeLock;

  wxString m_fileNames[N_CONFIG_FILE_TYPES];

 DECLARE_CLASS(ConfigViewer)
};

#endif // _CONFIGVIEWERH_
