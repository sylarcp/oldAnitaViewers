/* 
 * DBHandler.cpp
 * 
 * Class that provides interaction to SQL database. All queries are actually 
 * sent from and received here.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DBHandler.h"
#endif

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/utils.h>
#include <wx/progdlg.h>
#include <wx/datetime.h>
#include "DBHandler.h"

// Connection dialog implementation
IMPLEMENT_CLASS(ConnectDialog, wxDialog)

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

ConnectDialog::ConnectDialog(const wxString &host,const wxString &user,
			     const wxString &pwd,const wxString &name){
  Create(NULL,wxID_ANY,_T("Enter DB connection parameters"));
  
  wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *editSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  editSizer->Add(labelSizer,1,wxEXPAND);
  editSizer->Add(infoSizer,2,wxEXPAND);
  topSizer->Add(editSizer,1,wxEXPAND);

  wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
  buttons->AddButton(new wxButton(this,wxID_OK,_("Connect")));
  buttons->AddButton(new wxButton(this,wxID_CANCEL,_("Cancel")));
  buttons->Realize();
  topSizer->Add(buttons,0,wxEXPAND|wxTOP,5);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_("Host address")),1);
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_("Username")),1);
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_("Password")),1);
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_("DB name")),1);

  m_hostEntry = new wxTextCtrl(this,wxID_ANY,host);
  m_hostEntry->SetMaxLength(80);
  infoSizer->Add(m_hostEntry,0,wxEXPAND);

  m_userEntry = new wxTextCtrl(this,wxID_ANY,user);
  m_userEntry->SetMaxLength(80);
  infoSizer->Add(m_userEntry,0,wxEXPAND);

  m_pwdEntry = new wxTextCtrl(this,wxID_ANY,pwd,wxDefaultPosition,
			      wxDefaultSize,wxTE_PASSWORD);
  m_pwdEntry->SetMaxLength(80);
  infoSizer->Add(m_pwdEntry,0,wxEXPAND);

  m_nameEntry = new wxTextCtrl(this,wxID_ANY,name);
  m_nameEntry->SetMaxLength(80);
  infoSizer->Add(m_nameEntry,0,wxEXPAND);

  SetSizerAndFit(topSizer);
  GetSizer()->SetSizeHints(this);

  return;
}

ConnectDialog::~ConnectDialog(){}

void ConnectDialog::GetConnectionParameters(wxString &host,wxString &user,
					    wxString &pwd,wxString &name){
  host=m_hostEntry->GetLineText(0).Strip(wxString::both);
  user=m_userEntry->GetLineText(0).Strip(wxString::both);
  pwd=m_pwdEntry->GetLineText(0);
  name=m_nameEntry->GetLineText(0).Strip(wxString::both);
}

// Connection thread implementation
ConnectThread::ConnectThread(const wxString &connstr)
  :wxThread(wxTHREAD_JOINABLE),m_connstr(connstr){
  Create();
}
void *ConnectThread::Entry(){
  return (void*)PQconnectdb((const char*)m_connstr.mb_str());
}

// DB handler implementation
IMPLEMENT_CLASS(DBHandler,wxEvtHandler)

BEGIN_EVENT_TABLE(DBHandler,wxEvtHandler)
  EVT_TIMER(SQLTIMER,DBHandler::CheckOnQuery)
END_EVENT_TABLE()

  DBHandler::DBHandler():conn(NULL){
  sqlTimer=new wxTimer(this,SQLTIMER);
}
DBHandler::DBHandler(const wxString &host,const wxString &user,
		     const wxString &pwd,const wxString &name)
  :m_host(host),m_user(user),m_pwd(pwd),m_name(name),conn(NULL),inQuery(false){
  Connect(m_host,m_user,m_pwd,m_name,false);
  sqlTimer=new wxTimer(this,SQLTIMER);
}

DBHandler::~DBHandler(){
  sqlTimer->Stop();
  if(conn) Disconnect();
}
  
bool DBHandler::Connect(wxString &host,wxString &user,
			wxString &pwd,wxString &name,
			bool verify,bool block){
  if(verify){
    ConnectDialog dlg(host,user,pwd,name);
    if(dlg.ShowModal()==wxID_OK){
      dlg.GetConnectionParameters(host,user,pwd,name);
    }else{
      return false;
    }
  }
  m_host=host;
  m_user=user;
  m_pwd=pwd;
  m_name=name;

  if(m_name.IsSameAs(_U(DBNAME_TEMPLATE)))
    m_name = currentDBName();

  wxString tmp;
  tmp.Printf(_T("host='%s' user='%s' password='%s' dbname='%s'\n"),
	     m_host.c_str(),m_user.c_str(),m_pwd.c_str(),m_name.c_str());
  //  printf("(DBHandler)tmp: %s\n", (const char*)tmp.mb_str()) ;
  Disconnect() ; // strange, but this solved connection problem. SM 29Jan08
  if(!block){
    conn_thread=new ConnectThread(tmp);
    return conn_thread->Run()==wxTHREAD_NO_ERROR;
  }else{
    conn=PQconnectdb((const char*)tmp.mb_str());
    if(conn) return PQstatus(conn)==CONNECTION_OK;
    else return false;
  }
}

wxString DBHandler::currentDBName(){
  wxDateTime now=wxDateTime::Now().ToUTC();
  wxString name=now.Format(_T("anita_%m%d"));
  int hour=now.GetHour();

  if(hour<6){
    name.Append('a');
  }else if(hour<12){
    name.Append('b');
  }else if(hour<18){
    name.Append('c');
  }else{
    name.Append('d');
  }

  return name;
}

bool DBHandler::Reconnect(){
  Disconnect();
  return Connect(m_host,m_user,m_pwd,m_name,false,true);
}

void DBHandler::Disconnect(){
  ClearSQLQueue();
  if(conn) PQfinish(conn);
  conn=NULL;
}

bool DBHandler::FinalizeConnection(){
  void *exit=conn_thread->Wait();
  delete conn_thread;
  if(!exit) return false;
  conn=(PGconn*)exit;
  return PQstatus(conn)==CONNECTION_OK;
}

bool DBHandler::Error(){
  wxString msg=_U(PQerrorMessage(conn));
  msg.Trim();
  wxLogError(_T("[DBHandler::Error] inQuery=%d; %s"),inQuery,msg.c_str());
  return false;
}

void DBHandler::ClearSQLQueue(){
  int n=sqlCmdBuffer.Count();
  sqlCmdBuffer.Empty();
  sqlObjectBuffer.Empty();
  sqlParserBuffer.Empty();
  inQuery=false;
  if(n) wxLogMessage(_T("[DBHandler::ClearSQLQueue] Cleared %d queued SQL commands."),n);
  return;
}

wxString DBHandler::getHost(){
  return m_host;
}

int DBHandler::getQueueCount(){
  return sqlCmdBuffer.Count();
}

bool DBHandler::SendQuery(const wxString &sql,wxObject *object,
			       sqlparser parser){
  if(!conn){
    wxLogError(_T("[DBHandler::SendQuery] Not connected to database!"));
    return false;
  }
  if(inQuery){ // Buffer current request if busy
    sqlCmdBuffer.Add(sql);
    sqlObjectBuffer.Add((void*)object);
    sqlParserBuffer.Add((void*)parser);
    return true;
  }
  inQuery=true;
  if(!PQsendQuery(conn,sql.mb_str())){  // Clear buffer
    ClearSQLQueue();
    return Error();
  }
  m_requestor=object;
  m_parser=parser;
  sqlTimer->Start(1,true); 
  return true;
}

PGnotify *DBHandler::CheckOnNotification(){
  if(PQconsumeInput(conn)){
    return PQnotifies(conn);
  }else
    return NULL;
}

void DBHandler::CheckOnQuery(wxTimerEvent &event){
  if(PQconsumeInput(conn)){ 
    if(PQisBusy(conn)){  // No input available, so wait
      sqlTimer->Start(100,true); // CHANGE to "setable" delay
      return;
    }
    PGresult *result=PQgetResult(conn);
    if(result){
      m_parser(m_requestor,result);
      CheckOnQuery(event); // Call recursively to get all data that is ready
    }else
      inQuery=false;
  }else{ // There was an error
    Error();
    inQuery=false;
  }

  // If result is NULL we just end since the timer is stopped at this point
  // However, if there are some buffered queries execute them now
  if(!inQuery && sqlCmdBuffer.GetCount()){ // Recover buffered request
    wxString sql=sqlCmdBuffer[0];
    sqlCmdBuffer.RemoveAt(0);
    wxObject *object=(wxObject*)sqlObjectBuffer[0];
    sqlObjectBuffer.RemoveAt(0);
    sqlparser parser=(sqlparser)sqlParserBuffer[0];
    sqlParserBuffer.RemoveAt(0);
    SendQuery(sql,object,parser);
  }
}
