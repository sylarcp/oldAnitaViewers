/*
 * DBTerminal.cpp
 *
 * Definition for simple SQL terminal interface. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DBTerminal.h"
#endif

#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/ffile.h>
#include "DBTerminal.h"

IMPLEMENT_CLASS(DBTerminal,wxDialog)
BEGIN_EVENT_TABLE(DBTerminal,wxDialog)
  EVT_BUTTON(CLEAR_BUTTON_ID, DBTerminal::OnClear)
  EVT_BUTTON(SEND_BUTTON_ID, DBTerminal::OnSend)
  EVT_TEXT_ENTER(COMMAND_ENTRY_ID,DBTerminal::OnSend)
  EVT_CHAR(DBTerminal::OnKey)
END_EVENT_TABLE()

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

// Constructor
DBTerminal::DBTerminal(wxWindow *parent){
  Create(parent,wxID_ANY,_T("DB Terminal"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);

 SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL, false, _T("Arial")));

 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);

 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxButton *sendButton=new wxButton(this,SEND_BUTTON_ID,_("Send"));
 wxButton *clearButton=new wxButton(this,CLEAR_BUTTON_ID,_("Clear"));
 wxButton *closeButton=new wxButton(this,wxID_OK,_("Close"));
 buttonSizer->Add(sendButton,0,wxALL,5);
 buttonSizer->Add(clearButton,0,wxALL,5);
 buttonSizer->AddStretchSpacer();
 buttonSizer->Add(closeButton,0,wxALL,5);

 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);

 m_display = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,wxSize(400,250),
			    wxTE_MULTILINE|wxTE_READONLY);
 m_entry =  new wxTextCtrl(this,COMMAND_ENTRY_ID,_T(""),wxDefaultPosition,
			   wxDefaultSize,wxTE_PROCESS_ENTER);
 m_display->SetFont(ttfont);
 m_entry->SetFont(ttfont);

 topSizer->Add(m_display,1,wxEXPAND|wxBOTTOM,10);
 topSizer->Add(m_entry,0,wxEXPAND);
 topSizer->Add(buttonSizer,0,wxEXPAND);

 SetSizerAndFit(topSizer);
 GetSizer()->SetSizeHints(this);

 return;
}
		 
DBTerminal::~DBTerminal(){}

void DBTerminal::OnSend(wxCommandEvent &WXUNUSED(event)){
  wxString tmp=m_entry->GetLineText(0);
  if(!tmp.Strip(wxString::both).Length()) return;
  lastCommand=tmp;
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(lastCommand);
  sqlEvent.SetClientData((void*)parseWrapper);
  // Send it
  GetParent()->GetEventHandler()->ProcessEvent( sqlEvent );
  m_entry->Clear();
  currentCommand.Clear();
}

void DBTerminal::OnClear(wxCommandEvent &WXUNUSED(event)){
  m_display->Clear();
}

void DBTerminal::OnKey(wxKeyEvent &event){
  switch(event.GetKeyCode()){
  case WXK_UP:
    if(lastCommand.Length()){
      currentCommand=m_entry->GetLineText(0);
      m_entry->Clear();
      *m_entry << lastCommand;
    }
    break;
  case WXK_DOWN:
    if(currentCommand.Length()){
      m_entry->Clear();
      *m_entry << currentCommand;
    }
    break;
  default:
    event.Skip();
  }
}

void DBTerminal::sqlParser(PGresult *result){
  ExecStatusType status=PQresultStatus(result);
  if(status!=PGRES_TUPLES_OK){
    (*m_display)<<_U(PQresStatus(status))<<'\n'<<_U(PQresultErrorMessage(result));
  }else{
    wxFFile tempFile(_T("temp_sql.out"),_T("w+"));
    if(tempFile.IsOpened()){
      PQprintOpt opt={1,1,0,0,0,1," | ","","",NULL};
      PQprint(tempFile.fp(),result,&opt);      
      tempFile.Seek(0);
      wxString tmp;
      if(tempFile.ReadAll(&tmp))
	(*m_display) << tmp;
      else
	wxLogError(_T("Unable to read temporary file with SQL query output."));
      tempFile.Close();
    }else{
      wxLogError(_T("Unable to open temporary file needed to parse SQL query output."));
    }
  }
  PQclear(result);
}
    
