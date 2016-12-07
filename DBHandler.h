/* 
 * DBHandler.h
 */

#ifndef _DBHANDLERH_
#define _DBHANDLERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DBHandler.cpp"
#endif

#include <libpq-fe.h>
#include <wx/string.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include "BaseDBHandler.h"

#define DBNAME_TEMPLATE "anita_MMDDx"

// Class that gets connection info
class ConnectDialog:public wxDialog{
 public:
  ConnectDialog(const wxString &host=wxT(""),const wxString &user=wxT(""),
		const wxString &pwd=wxT(""),const wxString &name=wxT(""));
  ~ConnectDialog();
  
  void GetConnectionParameters(wxString &host,wxString &user,
			       wxString &pwd,wxString &name);

 private:
  wxTextCtrl *m_hostEntry;
  wxTextCtrl *m_userEntry;
  wxTextCtrl *m_pwdEntry;
  wxTextCtrl *m_nameEntry;

  DECLARE_CLASS(ConnectDialog);
};

// Thread class that actually makes db connection
class ConnectThread: public wxThread{
 public:
  ConnectThread(const wxString &connstr);
 private:
  void *Entry();
  wxString m_connstr;
};
  
// Declare dynamic pointer array
WX_DEFINE_ARRAY(void*, ArrayOfPointers);

// Class acting as interface to database
#define SQLTIMER 101
class DBHandler:public wxEvtHandler{
 public:
  DBHandler();
  DBHandler(const wxString &host,const wxString &user,
	    const wxString &pwd,const wxString &name);
  ~DBHandler();
  
  bool Connect(wxString &host,wxString &user,
	       wxString &pwd,wxString &name,
	       bool verify=true,bool block=false);
  bool FinalizeConnection();
  bool Reconnect();
  void Disconnect();

  inline wxString GetDBName() const {return m_name;}

  bool IsConnected(){return conn!=NULL;}

  bool SendQuery(const wxString &sql, wxObject *object, sqlparser parser);
  void CheckOnQuery(wxTimerEvent &event);
  PGnotify *CheckOnNotification();
  void ClearSQLQueue();
  int getQueueCount();
  wxString getHost();

 protected:
  DECLARE_EVENT_TABLE()
  bool Error();
  wxString currentDBName();

 private:
  wxString m_host;
  wxString m_user;
  wxString m_pwd;
  wxString m_name;

  ConnectThread *conn_thread;
  PGconn *conn;
  wxObject *m_requestor;
  sqlparser m_parser;
  wxTimer *sqlTimer;
  bool inQuery;					
  wxArrayString sqlCmdBuffer;
  ArrayOfPointers sqlObjectBuffer;
  ArrayOfPointers sqlParserBuffer;

  DECLARE_CLASS(DBHandler)
};

#endif // _DBHANDLERH_
