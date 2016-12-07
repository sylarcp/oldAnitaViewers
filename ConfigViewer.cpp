/*
 * ConfigViewer.cpp
 *
 * Class that is used to display configuration and log files stored in the 
 * database. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ConfigViewer.h"
#endif

#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "ConfigViewer.h"

IMPLEMENT_CLASS(ConfigViewer,wxDialog)
BEGIN_EVENT_TABLE(ConfigViewer,wxDialog)
  EVT_BUTTON(NEXT_BUTTON_ID, ConfigViewer::Next)
  EVT_BUTTON(PREV_BUTTON_ID, ConfigViewer::Prev)
  EVT_TEXT_ENTER(TIME_DISPLAY_ID,ConfigViewer::TimeSelect)
  EVT_CHOICE(FILE_SELECT_ID,ConfigViewer::FileSelect)
END_EVENT_TABLE()

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

// Constructor
ConfigViewer::ConfigViewer(wxWindow *parent){
  Create(parent,wxID_ANY,_T("Config File Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);

 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);

 wxBoxSizer *controlSizer=new wxBoxSizer(wxHORIZONTAL);
 
 m_fileNames[0]=_T("Acqd.config");
 m_fileNames[1]=_T("Archived.config");
 m_fileNames[2]=_T("anitaSoft.config");
 m_fileNames[3]=_T("Calibd.config");
 m_fileNames[4]=_T("Cmdd.config");
 m_fileNames[5]=_T("GPSd.config");
 m_fileNames[6]=_T("Eventd.config");
 m_fileNames[7]=_T("Hkd.config");
 m_fileNames[8]=_T("HkReadout.config");
 m_fileNames[9]=_T("LogWatchd.config");
 m_fileNames[10]=_T("LOSd.config");
 m_fileNames[11]=_T("Monitord.config");
 m_fileNames[12]=_T("Neobrickd.config");
 m_fileNames[13]=_T("Playbackd.config");
 m_fileNames[14]=_T("Prioritizerd.config");
 m_fileNames[15]=_T("SIPd.config");
 m_fileNames[16]=_T("tail_messages");
 m_fileNames[17]=_T("tail_anita.log");
 m_fileList = new wxChoice(this,FILE_SELECT_ID,wxDefaultPosition,wxDefaultSize,
			   N_CONFIG_FILE_TYPES,m_fileNames);
 m_timeDisplay = new wxUTCDateCtrl(this,TIME_DISPLAY_ID,_T("YYYY-MM-DD HH:MM:SS"),wxDefaultPosition,wxSize(135,-1),wxTE_PROCESS_ENTER);
 m_timeLock = new wxCheckBox(this,wxID_ANY,_T("Lock time"));
 
 controlSizer->Add(m_fileList,0,wxEXPAND|wxLEFT|wxRIGHT,5);
 controlSizer->Add(m_timeDisplay,0,wxLEFT,5);
 controlSizer->Add(m_timeLock,0,wxRIGHT|wxLEFT,5);
 controlSizer->Add(new wxButton(this,PREV_BUTTON_ID,_T("<"),wxDefaultPosition,
				wxDefaultSize,wxBU_EXACTFIT),0,wxEXPAND|wxLEFT,5);
 controlSizer->Add(new wxButton(this,NEXT_BUTTON_ID,_T(">"),wxDefaultPosition,
				wxDefaultSize,wxBU_EXACTFIT),0,wxEXPAND|wxRIGHT,5);

 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 
 m_display = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,wxSize(400,250),
			    wxTE_MULTILINE|wxTE_READONLY);
 m_display->SetFont(ttfont);

 topSizer->Add(m_display,1,wxEXPAND|wxBOTTOM,10);
 topSizer->Add(controlSizer,0,wxEXPAND);

 m_fileList->SetSelection(wxNOT_FOUND);

 SetSizerAndFit(topSizer);
 GetSizer()->SetSizeHints(this);

 return;
}
		 
ConfigViewer::~ConfigViewer(){}

void ConfigViewer::FileSelect(wxCommandEvent &WXUNUSED(event)){
  wxString sqlCmd;
  wxString file=m_fileList->GetStringSelection();
  if(m_timeLock->IsChecked()){ // Use time reference to locate file
    time_t t=m_timeDisplay->GetTicks();
    if(t<=0) return;
    sqlCmd.Printf(_T("SELECT * FROM file WHERE filename='%s' AND time<=%ld ORDER BY TIME DESC LIMIT 1;"),
		  file.c_str(),t);
  }else
    sqlCmd.Printf(_T("SELECT * FROM file WHERE filename='%s' ORDER BY TIME DESC LIMIT 1;"),file.c_str());
  SendSQLCmd(sqlCmd);
}

void ConfigViewer::TimeSelect(wxCommandEvent &WXUNUSED(event)){
  wxString sqlCmd;
  wxString file=m_fileList->GetStringSelection();
  time_t t=m_timeDisplay->GetTicks();
  if(t<=0) return;
  sqlCmd.Printf(_T("SELECT * FROM file WHERE filename='%s' AND time<=%ld ORDER BY TIME DESC LIMIT 1;"),
		file.c_str(),t);
  SendSQLCmd(sqlCmd);
}

void ConfigViewer::Next(wxCommandEvent &WXUNUSED(event)){
  wxString sqlCmd;
  wxString file=m_fileList->GetStringSelection();
  time_t t=m_timeDisplay->GetTicks();
  if(t<=0) return;
  sqlCmd.Printf(_T("SELECT * FROM file WHERE filename='%s' AND time>%ld ORDER BY TIME LIMIT 1;"),
		file.c_str(),t);
  SendSQLCmd(sqlCmd);
}

void ConfigViewer::Prev(wxCommandEvent &WXUNUSED(event)){
  wxString sqlCmd;
  wxString file=m_fileList->GetStringSelection();
  time_t t=m_timeDisplay->GetTicks();
  if(t<=0) return;
  sqlCmd.Printf(_T("SELECT * FROM file WHERE filename='%s' AND time<%ld ORDER BY TIME DESC LIMIT 1;"),
		file.c_str(),t);
  SendSQLCmd(sqlCmd);
}

void ConfigViewer::SendSQLCmd(wxString cmd){
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  // Send it
  GetParent()->GetEventHandler()->ProcessEvent( sqlEvent );
}

void ConfigViewer::sqlParser(PGresult *result){
  ExecStatusType status=PQresultStatus(result);
  if(status!=PGRES_TUPLES_OK){
    (*m_display)<<_U(PQresStatus(status))<<_T("\n")<<_U(PQresultErrorMessage(result));
  }else{
    int n=PQntuples(result);
    if(n>1) wxLogError(_T("[ConfigViewer::sqlParser] More than one file received, ignoring all but first."));
    if(n<1){
      if(m_display->GetLastPosition()==0) (*m_display) << _T("No file available.");
      return;
    }

    m_display->Clear();
    int coln;
    if((coln=PQfnumber(result,"time"))>=0){
      wxDateTime time(atol(PQgetvalue(result,0,coln)));
      time.MakeUTC();
      m_timeDisplay->Clear();
      *m_timeDisplay << time.Format(_T("%Y-%m-%d %H:%M:%S"));
    }
    if((coln=PQfnumber(result,"content"))>=0) (*m_display)<<_U(PQgetvalue(result,0,coln));
    m_display->ShowPosition(0);
  }
  PQclear(result);
}
    
