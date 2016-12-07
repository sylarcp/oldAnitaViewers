/*
 * SlowMoFrame.h
 * 
 * Handles menus and provides docking for subwindows
 */

#ifndef _SLOWMOFRAME_H_
#define _SLOWMOFRAME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SlowMoFrame.cpp"
#endif

#include "slowmodefs.h"

#define CURRENT_TIME_TIMER 11
#define UPDATE_TIMER       12
#define DBCONNECTION_TIMER 13

#include <wx/cmdline.h>
#include <wx/frame.h>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/timer.h>
#include <wx/textctrl.h>
#include <wx/log.h>
#include "SlowMoXMLHandler.h"
#include "SlowInfo.h"
#include "BarInfo.h"
#include "SlowMoOptions.h"
#include "DBHandler.h"
#include "FileHandler.h"
#include "DBTerminal.h"
#include "FlightControlItem.h"
#include "DBControlItem.h"
#include "ControlPanel.h"
#include "SlowMoViewer.h"

#define N_PCKTYPE 1
#define SLOWPCK     0x0

#define INDEX_OFFSET    1000
#define LISTEN -1

// Declare dynamic int array
WX_DEFINE_ARRAY(int, ArrayOfInts);

class SlowMoFrame: public wxFrame, public BaseDBHandler{
 public:
  // Constructor
  SlowMoFrame(wxWindow* parent, int argc, char **argv, wxWindowID id = wxID_ANY, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);

  // Destructor
  ~SlowMoFrame();

  // wxEVT_COMMAND_MENU_SELECTED event handlers
  void OnMenuControlOptions( wxCommandEvent& event );
  void OnMenuControlSaveOptions( wxCommandEvent& event );
  void OnMenuControlLoadOptions( wxCommandEvent& event );
  void OnMenuControlQuit( wxCommandEvent& event );

  void OnMenuFlightDataControl( wxCommandEvent& event );
  void OnMenuFlightDataOpen( wxCommandEvent& event );
  void OnMenuFlightDataNext( wxCommandEvent& event );
  void OnMenuFlightDataPrev( wxCommandEvent& event );

  void OnMenuDBDataConnect( wxCommandEvent& event );
  void OnMenuDBDataDisconnect( wxCommandEvent& event );
  void OnMenuDBDataControl( wxCommandEvent& event );
  void OnMenuDBDataTerminal( wxCommandEvent& event );
  void OnMenuDBDataNext( wxCommandEvent& event );
  void OnMenuDBDataPrev( wxCommandEvent& event );
  void OnMenuDBDataLast( wxCommandEvent& event );
  void OnMenuDBDataAuto( wxCommandEvent& event );

  void OnMenuHelpAbout( wxCommandEvent& event );
/////////////////////////july 9 08 2008////////////////////////////////////
  void OnMenuSlowMoViewer( wxCommandEvent& event );//amir's addition
  ////////////////////////////////////////////////////////////////////
  void OnCurrentTimeTimer( wxTimerEvent& event );
  void OnUpdateTimer( wxTimerEvent& event );
  void CheckDBConnection( wxTimerEvent& event );
 
  void OnSQLRequest( wxCommandEvent &event );
  void ReadFlightData( wxCommandEvent &event );
  void RequestPacket( wxCommandEvent &event );

 protected:
  DECLARE_EVENT_TABLE()
  void init();
  void SetOptions();
  bool SaveOptions(const wxString &file);
  bool LoadOptions(const wxString &file);

  bool ReadSlowData(const wxFileName &file,int order=0);
  void IndexSlowDirectory(bool full=true);

  void sqlParser(PGresult *result);
  void ParseSlowPacket(PGresult *result);

  void SendListen(int type);
  void ReceiveNotifications();
  void IndexDBPackets(int type,bool full=true);
  bool ReallocateList(int type, int n);
  void StorePacketList(PGresult *result,int type,bool haveus);

 private:
  wxMenuBar *m_pMenuBar;
  wxMenu *m_pControlMenu;
  wxMenu *m_pFlightDataMenu;
  wxMenu *m_pDBDataMenu;
  wxMenu *m_pHelpMenu;
 ////////////////////////////////july 09 08
  wxMenu *m_pDataView;//amir's addition
  ///////////////////////////////
  wxMenuItem *m_pFlightDataNext;
  wxMenuItem *m_pFlightDataPrev;

  wxMenuItem *m_pDBDataConnect;
  wxMenuItem *m_pDBDataDisconnect;
  wxMenuItem *m_pDBDataControl;
  wxMenuItem *m_pDBDataTerminal;
  wxMenuItem *m_pDBDataNext;
  wxMenuItem *m_pDBDataPrev;
  wxMenuItem *m_pDBDataLast;
  wxMenuItem *m_pDBDataAuto;
  ///////////////////////////////////july 09 08
  wxMenuItem *m_pSlowMoViewer;

  enum{
    MENU_CONTROL_OPTIONS,
    MENU_CONTROL_SAVEOPTIONS,
    MENU_CONTROL_LOADOPTIONS,
    MENU_CONTROL_QUIT,

    MENU_FLIGHTDATA_CONTROL,
    MENU_FLIGHTDATA_OPEN,
    MENU_FLIGHTDATA_NEXT,
    MENU_FLIGHTDATA_PREV,

    MENU_DBDATA_CONNECT,
    MENU_DBDATA_DISCONNECT,
    MENU_DBDATA_CONTROL,
    MENU_DBDATA_TERMINAL,
    MENU_DBDATA_NEXT,
    MENU_DBDATA_PREV,
    MENU_DBDATA_LAST,
    MENU_DBDATA_AUTO,

    MENU_HELP_ABOUT,
    MENU_SLOW_MO_VIEWER
  };

  // Command line handler
  wxCmdLineParser *cmdLine;

  // Menu dialogs
  SlowMoOptions *m_optionsDialog;
  DBTerminal *m_dbTerminal;
  ControlPanel *m_flightControl;
  ControlPanel *m_dbControl;
//////////////////////july 8 08
  SlowMoViewer *m_slowmoViewer;
  ///////////////////////////////
  // Directory and files
  wxFileName packetDirectory[N_PCKTYPE];
  wxArrayString *packetFileList[N_PCKTYPE];
  wxString packetTable[N_PCKTYPE];
  dbRef *packetList[N_PCKTYPE];
  int packetListItems[N_PCKTYPE];     // Keep track of length of each list
  int packetListAllocSize[N_PCKTYPE]; // Keep track of allocated size of each list
  time_t packetListUpdateTime[N_PCKTYPE]; // Keep time of update
  bool needsIndex[N_PCKTYPE]; // Store notification info

  // Delay timer 
  long m_readDelay;
  wxTimer *m_currentTimeTimer;
  wxTimer *m_updateTimer;

  wxLog *m_logTarget;
  // Child windows  
  wxTextCtrl *m_LogWindow;
  SlowInfo *m_slowInfo;

  BarInfo *m_EventRateInfo;
  BarInfo *m_TempInfo;
  BarInfo *m_PowerInfo;
  
  BarInfo *m_L1Info;
  BarInfo *m_L1hInfo;
//  BarInfo *m_L2Info;
  BarInfo *m_L3Info;
  BarInfo *m_L3hInfo;

  BarInfo *m_RFInfo;
  BarInfo *m_ScalerInfo;
 // BarInfo *m_ScalerRMSInfo;

  // Data handlers
  SlowMoXMLHandler *m_xmlHandler;
  DBHandler *m_dbHandler;
  FileHandler *m_fileHandler;

  // DB parser associated
  ArrayOfInts sqlRequestType;
  bool showProgress;
  time_t lastSQLRequest;
};

#endif //_SLOWMOFRAME_H_
    
 




