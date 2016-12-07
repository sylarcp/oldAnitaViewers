/*
 * TrigMonFrame.h
 * 
 * Handles menus and provides docking for subwindows
 */

#ifndef _TRIGMONFRAME_H_
#define _TRIGMONFRAME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TrigMonFrame.cpp"
#endif

#include "trigmondefs.h"

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
#include "TrigMonXMLHandler.h"
#include "TURFInfo.h"
#include "SURFInfo.h"
#include "BarInfo.h"
#include "TrigMonOptions.h"
#include "DBHandler.h"
#include "FileHandler.h"
#include "DBTerminal.h"
#include "FlightControlItem.h"
#include "DBControlItem.h"
#include "ControlPanel.h"
#include "TurfViewer.h"
#include "SurfViewer.h"

#define N_PCKTYPE 2
#define SURFPCK     0x0
#define TURFPCK     0x1

#define INDEX_OFFSET    1000
#define LISTEN -1

// Declare dynamic int array
WX_DEFINE_ARRAY(int, ArrayOfInts);

class TrigMonFrame: public wxFrame, public BaseDBHandler{
 public:
  // Constructor
  TrigMonFrame(wxWindow* parent, int argc, char **argv, wxWindowID id = wxID_ANY, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);

  // Destructor
  ~TrigMonFrame();

  // wxEVT_COMMAND_MENU_SELECTED event handlers
  void OnMenuControlPanel( wxCommandEvent& event );
  void OnMenuControlOptions( wxCommandEvent& event );
  void OnMenuControlSaveOptions( wxCommandEvent& event );
  void OnMenuControlLoadOptions( wxCommandEvent& event );
  void OnMenuControlQuit( wxCommandEvent& event );
  void OnMenuTurfRawOpen( wxCommandEvent& event );
  void OnMenuTurfOpen( wxCommandEvent& event );
  void OnMenuTurfNext( wxCommandEvent& event );
  void OnMenuTurfPrev( wxCommandEvent& event );
  void OnMenuTurfStart( wxCommandEvent& event );
  void OnMenuTurfStop( wxCommandEvent& event );
  void OnMenuSurfRawOpen( wxCommandEvent& event );
  void OnMenuSurfOpen( wxCommandEvent& event );
  void OnMenuSurfNext( wxCommandEvent& event );
  void OnMenuSurfPrev( wxCommandEvent& event );
  void OnMenuSurfStart( wxCommandEvent& event );
  void OnMenuSurfStop( wxCommandEvent& event );
/////////////////////////june 30 2008////////////////////////////////////  
void OnMenuTurfViewer( wxCommandEvent& event );//amir's addition  
void OnMenuSurfViewer( wxCommandEvent& event );//amir's addition  
//////////////////////////////////////////////////////////////////
  void OnMenuDBDataConnect( wxCommandEvent& event );
  void OnMenuDBDataDisconnect( wxCommandEvent& event );
  void OnMenuDBDataNext( wxCommandEvent &event );
  void OnMenuDBDataPrev( wxCommandEvent &event );
  void OnMenuDBDataLast( wxCommandEvent &event );
  void OnMenuDBDataAuto( wxCommandEvent &event );
  void OnMenuDBDataControl( wxCommandEvent& event );
  void OnMenuDBDataTerminal( wxCommandEvent& event );

  void OnMenuHelpAbout( wxCommandEvent& event );

  void OnCurrentTimeTimer( wxTimerEvent& event );
  void OnUpdateTimer( wxTimerEvent& event );
  void CheckDBConnection( wxTimerEvent& event );
 
  void OnSQLRequest( wxCommandEvent &event );
  void ReadFlightData( wxCommandEvent &event );
  void RequestPacket( wxCommandEvent &event );

 protected:
  DECLARE_EVENT_TABLE()
  void init();
  void SetSurfOptions();
  void SetTurfOptions();
  bool SaveOptions(const wxString &file);
  bool LoadOptions(const wxString &file);
  bool ReadTurfRawData();
  bool ReadSurfRawData();

  bool ReadTurfData(const wxFileName &file,int order=0);
  bool ReadSurfData(const wxFileName &file,int order=0);
  void IndexTurfDirectory(bool full=true);
  void IndexSurfDirectory(bool full=true);

  void sqlParser(PGresult *result);
  void ParseSurfPacket(PGresult *result);
  void ParseTurfPacket(PGresult *result);

  void SetMaskedBackground(int surf,unsigned long mask);

  void SendListen(int type);
  void ReceiveNotifications();
  void IndexDBPackets(int type,bool full=true);
  bool ReallocateList(int type, int n);
  void StorePacketList(PGresult *result,int type,bool haveus);

 private:
  wxMenuBar *m_pMenuBar;
  wxMenu *m_pControlMenu;
  wxMenu *m_pTurfMenu;
  wxMenu *m_pSurfMenu;
  wxMenu *m_pDBDataMenu;
  wxMenu *m_pHelpMenu;
 ////////////////////////////////july 09 08 
 wxMenu *m_pDataView;
 //amir's addition ///////////////////////////////
  wxMenuItem *m_pTurfNext;
  wxMenuItem *m_pTurfPrev;

  wxMenuItem *m_pSurfNext;
  wxMenuItem *m_pSurfPrev;

  wxMenuItem *m_pDBDataConnect;
  wxMenuItem *m_pDBDataDisconnect;
  wxMenuItem *m_pDBDataControl;
  wxMenuItem *m_pDBDataTerminal;
///////////////////////////////////july 09 08  
wxMenuItem *m_pSurfViewer;  
wxMenuItem *m_pTurfViewer;

  enum{

    MENU_CONTROL_PANEL,
    MENU_CONTROL_OPTIONS,
    MENU_CONTROL_SAVEOPTIONS,
    MENU_CONTROL_LOADOPTIONS,
    MENU_CONTROL_QUIT,

    MENU_TURF_OPEN,
    MENU_TURF_OPEN_RAW,
    MENU_TURF_NEXT,
    MENU_TURF_PREV,

    MENU_SURF_OPEN,
    MENU_SURF_OPEN_RAW,
    MENU_SURF_NEXT,
    MENU_SURF_PREV,

    MENU_DBDATA_CONNECT,
    MENU_DBDATA_DISCONNECT,
    MENU_DBDATA_NEXT,
    MENU_DBDATA_PREV,
    MENU_DBDATA_LAST,
    MENU_DBDATA_AUTO,
    MENU_DBDATA_CONTROL,
    MENU_DBDATA_TERMINAL,

    MENU_HELP_ABOUT,
    MENU_TURF_VIEWER,    
    MENU_SURF_VIEWER
  };

  // Command line handler
  wxCmdLineParser *cmdLine;

  // Menu dialogs
  TrigMonOptions *m_optionsDialog;
  DBTerminal *m_dbTerminal;
  ControlPanel *m_flightControl;
  ControlPanel *m_dbControl;
  TurfViewer *m_turfViewer;  
  SurfViewer *m_surfViewer;

  // Directory and files
  wxFileName *currentRawTurfFile;
  wxFileName *currentRawSurfFile;
  bool turfRaw;
  bool surfRaw;
  wxFileName packetDirectory[N_PCKTYPE];
  wxArrayString *packetFileList[N_PCKTYPE];
  wxString packetTable[N_PCKTYPE];
  dbRef *packetList[N_PCKTYPE];
  int packetListItems[N_PCKTYPE];     // Keep track of length of each list
  int packetListAllocSize[N_PCKTYPE]; // Keep track of allocated size of each list
  time_t packetListUpdateTime[N_PCKTYPE]; // Keep time of update
  bool needsIndex[N_PCKTYPE]; // Store notification info

  // Delay timer 
  long m_turfReadDelay;
  long m_surfReadDelay;
  wxTimer *m_currentTimeTimer;
  wxTimer *m_updateTimer;

  wxLog *m_logTarget;
  // Child windows  
  wxTextCtrl *m_LogWindow;
  TURFInfo *m_TURFInfo;
  SURFInfo *m_SURFInfo;

  BarInfo *m_L1Mask;
  BarInfo *m_L1hMask;
  BarInfo *m_L1Info;
  BarInfo *m_L1hInfo;
  BarInfo *m_L3Info;
  BarInfo *m_L3hInfo;

  BarInfo *m_RFInfo1;
  BarInfo *m_RFInfo2;
  BarInfo *m_ScalerInfo1;
  BarInfo *m_ScalerInfo2;
  BarInfo *m_ThresholdInfo1;
  BarInfo *m_ThresholdInfo2;

  // Data handlers
  TrigMonXMLHandler *m_xmlHandler;
  DBHandler *m_dbHandler;
  FileHandler *m_fileHandler;

  // DB parser associated
  ArrayOfInts sqlRequestType;
  bool showProgress;
  time_t lastSQLRequest;

  // Data display associated
  wxRangeSpec **dynamicScalerSpec;
  wxRangeSpec **dynamicThresholdSpec;
};

#endif //_TRIGMONFRAME_H_
    
 




