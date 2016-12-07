/*
 * DBTerminal.h
 */

#ifndef _DBTERMINALH_
#define _DBTERMINALH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DBTerminal.cpp"
#endif

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include "BaseDBHandler.h"

#define CLEAR_BUTTON_ID  10
#define SEND_BUTTON_ID   11
#define COMMAND_ENTRY_ID 101

class DBTerminal:public wxDialog,public BaseDBHandler{
 public:
  DBTerminal(wxWindow *parent);
  ~DBTerminal();

  void OnSend(wxCommandEvent &event); 
  void OnClear(wxCommandEvent &event); 
  void OnKey(wxKeyEvent &event); 
  
 protected:
  DECLARE_EVENT_TABLE()
  void sqlParser(PGresult *result);

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m_entry;
  wxString lastCommand;
  wxString currentCommand;

 DECLARE_CLASS(DBTerminal)
};

#endif // _DBTERMINALH_
