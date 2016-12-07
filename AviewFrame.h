/*
 * AviewFrame.h
 * 
 * Handles menus and provides docking for subwindows
 */

#ifndef _AVIEWFRAME_H_
#define _AVIEWFRAME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AviewFrame.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "aviewdefs.h"

#define EVENT_LOAD_TIMER   10
#define CURRENT_TIME_TIMER 11
#define UPDATE_TIMER       12
#define DBCONNECTION_TIMER 13
#define EVENTRATE_TIMER    14
#define RETROINDEX_TIMER   15
#define PAYLOAD_VIEW       101

#define SQL_IDLE_LIMIT     3600  // seconds 
#define RETRO_TIMER_STEP   60  // seconds

#include <time.h>
#include <wx/cmdline.h>
#include <wx/frame.h>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/timer.h>
#include <wx/textctrl.h>
#include <wx/log.h>
#include "HeaderInfo.h"
#include "TriggerInfo.h"
#include "AuxInfo.h"
#include "TextInfo.h"
#include "BarInfo.h"
#include "OptionsDialog.h"
#include "rftab.h"
#include "payload.h"
#include "GPSDataInfo.h"
#include "GPSSatInfo.h"
#include "TrigPatternInfo.h"
#include "AviewXMLHandler.h"
#include "DBHandler.h"
#include "FileHandler.h"
#include "DBTerminal.h"
#include "FlightControlItem.h"
#include "DBControlItem.h"
#include "ControlPanel.h"
#include "TimeTranslator.h"
#include "ConfigViewer.h"
/////////////////////////////////amir's addition/////////////
#include "GpsPosViewer.h"
#include "ADU5PatViewer.h"
#include "ADU5SatViewer.h"
#include "ADU5VtgViewer.h"
#include "MonitorViewer.h"
#include "HeaderViewer.h"
#include "CommandViewer.h"
#include "HKViewer.h"
#include "HKCalViewer.h"
#include "G12SatViewer.h"
#include "DB_HKViewer.h"
#include "DB_HeaderViewer.h"
#include "DB_MonitorViewer.h"
#include "DB_CmdViewer.h"
////////////////////////amir's addition/////////////////////

#define N_PCKTYPE 11
#define RFEVENT       0x0
#define HEADER        0x1
#define HKEVENT       0x2
#define SSHKEVENT     0x3
#define MONITOR       0x4
#define ADU5PAT       0x5
#define ADU5VTG       0x6
#define ADU5SAT       0x7
#define G12POS        0x8
#define G12SAT        0x9
#define CMDECHO       0xa
#define WVFORM        0xb 
/*
#define MONITOR       0x3
#define ADU5PAT       0x4
#define ADU5VTG       0x5
#define ADU5SAT       0x6
#define G12POS        0x7
#define G12SAT        0x8
#define CMDECHO       0x9
#define WVFORM (CMDECHO+1)
#define SSHKEVENT       0xb
*/
#define DBMREF        100
#define EVENTRATE     101
#define WVAVG         102
#define INDEX_OFFSET  1000
#define LISTEN        -1

// Declare dynamic int array
WX_DEFINE_ARRAY(int, ArrayOfInts);

class AviewFrame: public wxFrame, public BaseDBHandler{
  DECLARE_CLASS(AviewFrame)
  
 public:
  // Constructor
  AviewFrame(wxWindow* parent, int argc, char **argv, wxWindowID id = wxID_ANY, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize);

  // Destructor
  ~AviewFrame();

  // wxEVT_COMMAND_MENU_SELECTED event handlers
  void OnMenuControlGPSData( wxCommandEvent& event );
  void OnMenuControlGPSSat( wxCommandEvent& event );
  void OnMenuControlTrigPattern( wxCommandEvent& event );
  void OnMenuControlDumpRF( wxCommandEvent& event );
  void OnMenuControlClearDBMRef( wxCommandEvent& event );
  void OnMenuControlClearPowerRef( wxCommandEvent& event );
  void OnMenuControlConvertTime( wxCommandEvent& event );
  void OnMenuControlOptions( wxCommandEvent& event );
  void OnMenuControlSaveOptions( wxCommandEvent& event );
  void OnMenuControlLoadOptions( wxCommandEvent& event );
   /////////////////////////////june 27 2008 11:24 am///////////////////////
  //void OnMenuControlDataViewer( wxCommandEvent& event );//amir's addition
  /////////////////////////june 30 2008////////////////////////////////////
  void OnMenuG12PosViewer( wxCommandEvent& event );//amir's addition
  void OnMenuG12SatViewer( wxCommandEvent& event );//amir's addition
  //////////////////////////july 1 2008 ////////////////////////////////
  void OnMenuADU5PatViewer( wxCommandEvent& event );//amir's addition
  void OnMenuADU5SatViewer( wxCommandEvent& event );//amir's addition
  void OnMenuADU5VtgViewer( wxCommandEvent& event );//amir's addition
  void OnMenuHeaderViewer( wxCommandEvent& event );//amir's addition
  void OnMenuMonitorViewer( wxCommandEvent& event );//amir's addition
  void OnMenuCommandViewer( wxCommandEvent& event );//amir's addition
  void OnMenuHKViewer( wxCommandEvent& event );//amir's addition
  void OnMenuHKCalViewer( wxCommandEvent& event );//amir's addition
   void OnMenuDBHKViewer( wxCommandEvent& event );//amir's addition
   void OnMenuDBHeaderViewer( wxCommandEvent& event );//amir's addition
    void OnMenuDBMonitorViewer( wxCommandEvent& event );//amir's addition
     void OnMenuDBCmdViewer( wxCommandEvent& event );//amir's addition
  ///////////////////////////////////////////////////////////////////
  void OnMenuControlQuit( wxCommandEvent& event );

  void OnMenuRawDataOpen( wxCommandEvent& event );
  void OnMenuRawDataNext( wxCommandEvent& event );
  void OnMenuRawDataPrev( wxCommandEvent& event );
  void OnMenuRawDataStart( wxCommandEvent& event );
  void OnMenuRawDataStop( wxCommandEvent& event );
  void OnMenuRawDataPedestal( wxCommandEvent& event );

  void OnMenuFlightDataOpenControl( wxCommandEvent& event );
  void OnMenuFlightDataOpenRF( wxCommandEvent& event );
  void OnMenuFlightDataNextRF( wxCommandEvent& event );
  void OnMenuFlightDataPrevRF( wxCommandEvent& event );
  void OnMenuFlightDataAverageWV( wxCommandEvent& event );
  void OnMenuFlightDataPedestal( wxCommandEvent& event );
  void OnMenuFlightDataDBMRef( wxCommandEvent& event );
  void OnMenuFlightDataOpenHK( wxCommandEvent& event );
  void OnMenuFlightDataCalibHK( wxCommandEvent& event );
  void OnMenuFlightDataNextHK( wxCommandEvent& event );
  void OnMenuFlightDataPrevHK( wxCommandEvent& event );
  void OnMenuFlightDataOpenGPS( wxCommandEvent& event );
  void OnMenuFlightDataNextGPS( wxCommandEvent& event );
  void OnMenuFlightDataPrevGPS( wxCommandEvent& event );
  void OnMenuFlightDataOpenMonitor( wxCommandEvent& event );
  void OnMenuFlightDataNextMonitor( wxCommandEvent& event );
  void OnMenuFlightDataPrevMonitor( wxCommandEvent& event );
  void OnMenuFlightDataOpenCmd( wxCommandEvent& event );
  void OnMenuFlightDataNextCmd( wxCommandEvent& event );
  void OnMenuFlightDataPrevCmd( wxCommandEvent& event );

  void OnMenuDBDataConnect( wxCommandEvent& event );
  void OnMenuDBDataDisconnect( wxCommandEvent& event );
  void OnMenuDBDataControl( wxCommandEvent& event );
  void OnMenuDBDataNext( wxCommandEvent& event );
  void OnMenuDBDataPrev( wxCommandEvent& event );
  void OnMenuDBDataLast( wxCommandEvent &event );
  void OnMenuDBDataAuto( wxCommandEvent &event );
  void OnMenuDBDataOpenEvent( wxCommandEvent& event );
  void OnMenuDBDataWaveformAverage( wxCommandEvent& event );
  void OnMenuDBDataDBMRef( wxCommandEvent& event );
  void OnMenuDBDataConfigViewer( wxCommandEvent& event );
  void OnMenuDBDataTerminal( wxCommandEvent& event );
  void OnMenuDBDataClear( wxCommandEvent& event );
  void OnMenuDBDataStartIndex( wxCommandEvent& event );
  void OnMenuDBDataStopIndex( wxCommandEvent& event );

  void OnMenuHelpAbout( wxCommandEvent& event );

  void OnLoadEventTimer( wxTimerEvent& event );
  void OnCurrentTimeTimer( wxTimerEvent& event );
  void OnUpdateTimer( wxTimerEvent& event );
  void CheckDBConnection( wxTimerEvent& event );
  void OnEventRateTimer( wxTimerEvent& event );
  void OnRetroIndexTimer( wxTimerEvent& event );

  void OnAntennaSelect( wxAntennaEvent& event );
  void OnSQLRequest( wxCommandEvent &event );
  void ReadFlightData( wxCommandEvent &event );
  void RequestPacket( wxCommandEvent &event );
  void NoResultsMessage( wxCommandEvent &event );

 protected:
  DECLARE_EVENT_TABLE()
  void init();
  void SetOptions();
  bool SaveOptions(const wxString &file);
  bool LoadOptions(const wxString &file);
  void SwitchRFView(bool resize=true);
  void OrderRFTabs();
  void SetTriggerPattern(unsigned short l1TrigMask, unsigned short l1TrigMaskH,unsigned short l3Pat, unsigned short l3PatH, unsigned short phiTrigMask, unsigned short phiTrigMaskH, bool redo=false);
  void SwitchRFReference();
  bool ReadRawData();
  bool ReadRawPedestals();
  void IndexRawDirectory(bool full=true);
  void IndexPacketDirectory(int type,bool full=true);

  bool ReadPedestals(const wxFileName &file);
  bool ReadRFEvent(const wxFileName &file,int order=0,bool wvin=true);
  bool ReadHKPacket(const wxFileName &file,int order=0);
  bool ReadMonitorPacket(const wxFileName &file,int order=0);
  bool ReadADU5PatPacket(const wxFileName &file,int order=0);
  bool ReadADU5VtgPacket(const wxFileName &file,int order=0);
  bool ReadADU5SatPacket(const wxFileName &file,int order=0);
  bool ReadG12PosPacket(const wxFileName &file,int order=0);
  bool ReadG12SatPacket(const wxFileName &file,int order=0);
  bool ReadCmdEchoPacket(const wxFileName &file,int order=0);
  void ReadWaveformFile(const wxString &file,unsigned long evnum,int order=0);
  bool ReadHKCalibPacket(const wxFileName &file);

  bool AverageWaveformFile(const wxFileName &file);
  bool ReadDBMRef(const wxFileName &file);
  
  void sqlParser(PGresult *result);
  void ParseHeader(PGresult *result,bool wvin=true);
  void ParseHKPacket(PGresult *result);
  void ParseSSHKPacket(PGresult *result);
  void ParseMonitorPacket(PGresult *result);
  void ParseADU5PatPacket(PGresult *result);
  void ParseADU5VtgPacket(PGresult *result);
  void ParseADU5SatPacket(PGresult *result);
  void ParseG12PosPacket(PGresult *result);
  void ParseG12SatPacket(PGresult *result);
  void ParseCmdEchoPacket(PGresult *result);
  void ParseWaveformPacket(PGresult *result);
  void ParseDBMRef(PGresult *result);
  void ParseEventRate(PGresult *result);
  void ParseWaveformAverage(PGresult *result);

  void SendListen(int type);
  void ReceiveNotifications();
  void IndexDBPackets(int type,bool full=true, bool retro=false);
  bool ReallocateList(int type, int n);
  void StorePacketList(PGresult *result,int type,bool haveus);

 private:
  wxMenuBar *m_pMenuBar;
  wxMenu *m_pControlMenu;
  wxMenu *m_pRawDataMenu;
  wxMenu *m_pFlightDataMenu;
  wxMenu *m_pDBDataMenu;
  wxMenu *m_pHelpMenu;
wxBoxSizer *topSizer; ///peng

 //////////////////////////////////////////////////june 27 2008 11:31 am////////////////////
  wxMenu *m_pDataView;//amir's addition
///////////////////////////////////////////////////////////////////////////////////

  wxMenuItem *m_pControlDumpRF;

  wxMenuItem *m_pRawDataNext;
  wxMenuItem *m_pRawDataPrev;
  wxMenuItem *m_pRawDataStart;
  wxMenuItem *m_pRawDataStop;

  wxMenuItem *m_pFlightDataNextRF;
  wxMenuItem *m_pFlightDataPrevRF;
  wxMenuItem *m_pFlightDataNextHK;
  wxMenuItem *m_pFlightDataPrevHK;
  wxMenuItem *m_pFlightDataNextGPS;
  wxMenuItem *m_pFlightDataPrevGPS;
  wxMenuItem *m_pFlightDataNextMonitor;
  wxMenuItem *m_pFlightDataPrevMonitor;
  wxMenuItem *m_pFlightDataNextCmd;
  wxMenuItem *m_pFlightDataPrevCmd;

  wxMenuItem *m_pDBDataConnect;
  wxMenuItem *m_pDBDataDisconnect;
  wxMenuItem *m_pDBDataControl;
  wxMenuItem *m_pDBDataNext;
  wxMenuItem *m_pDBDataPrev;
  wxMenuItem *m_pDBDataLast;
  wxMenuItem *m_pDBDataAuto;
  wxMenuItem *m_pDBDataOpenEvent;
  wxMenuItem *m_pDBDataWaveformAverage;
  wxMenuItem *m_pDBDataDBMRef;
  wxMenuItem *m_pDBDataConfigViewer;

   //////////////////////////june 27 2008 11:41 am//////////////////////////////
 // wxMenuItem *m_pDataViewer;
   //////////////////////////june 30 2008 10:20 am//////////////////////////////
  wxMenuItem *m_pGpsPosViewer;
  wxMenuItem *m_pG12SatViewer;
  wxMenuItem *m_pADU5SatViewer;
  wxMenuItem *m_pADU5PatViewer;
  wxMenuItem *m_pADU5VtgViewer;
  wxMenuItem *m_pMonitorViewer;
  wxMenuItem *m_pHeaderViewer;
  wxMenuItem *m_pCommandViewer;
  wxMenuItem *m_pHKViewer;
  wxMenuItem *m_pHKCalViewer;
  wxMenuItem *m_pDBHKViewer;
  wxMenuItem *m_pDBHeaderViewer;
   wxMenuItem *m_pDBMonitorViewer;
   wxMenuItem *m_pDBCmdViewer;
  //////////////////////////////////////////////////////////////////////////
  
  wxMenuItem *m_pDBDataTerminal;
  wxMenuItem *m_pDBDataClear;
  wxMenuItem *m_pDBDataStartIndex;
  wxMenuItem *m_pDBDataStopIndex;

  enum{
    MENU_CONTROL_GPSDATA,
    MENU_CONTROL_GPSSAT,
    MENU_CONTROL_TRIGPATTERN,
    MENU_CONTROL_DUMPRF,
    MENU_CONTROL_CONVERTTIME,
    MENU_CONTROL_CLEARDBMREF,
    MENU_CONTROL_CLEARPOWERREF,
    MENU_CONTROL_OPTIONS,
    MENU_CONTROL_SAVEOPTIONS,
    MENU_CONTROL_LOADOPTIONS,
    MENU_CONTROL_QUIT,

    MENU_RAWDATA_OPEN,
    MENU_RAWDATA_NEXT,
    MENU_RAWDATA_PREV,    
    MENU_RAWDATA_START,
    MENU_RAWDATA_STOP,
    MENU_RAWDATA_PEDESTAL,

    MENU_FLIGHTDATA_CONTROL,
    MENU_FLIGHTDATA_OPENRF,
    MENU_FLIGHTDATA_NEXTRF,
    MENU_FLIGHTDATA_PREVRF,
    MENU_FLIGHTDATA_AVERAGEWV,
    MENU_FLIGHTDATA_PEDESTAL,
    MENU_FLIGHTDATA_DBMREF,
    MENU_FLIGHTDATA_OPENHK,
    MENU_FLIGHTDATA_NEXTHK,
    MENU_FLIGHTDATA_PREVHK,
    MENU_FLIGHTDATA_CALIBHK,
    MENU_FLIGHTDATA_OPENGPS,
    MENU_FLIGHTDATA_NEXTGPS,
    MENU_FLIGHTDATA_PREVGPS,
    MENU_FLIGHTDATA_OPENMONITOR,
    MENU_FLIGHTDATA_NEXTMONITOR,
    MENU_FLIGHTDATA_PREVMONITOR,
    MENU_FLIGHTDATA_OPENCMD,
    MENU_FLIGHTDATA_NEXTCMD,
    MENU_FLIGHTDATA_PREVCMD,

    MENU_DBDATA_CONNECT,
    MENU_DBDATA_DISCONNECT,
    MENU_DBDATA_CONTROL,
    MENU_DBDATA_NEXT,
    MENU_DBDATA_PREV,
    MENU_DBDATA_LAST,
    MENU_DBDATA_AUTO,
    MENU_DBDATA_OPENEVENT,
    MENU_DBDATA_WVAVG,
    MENU_DBDATA_DBMREF,
    MENU_DBDATA_CONFIGVIEWER,
    MENU_DBDATA_TERMINAL,
    MENU_DBDATA_CLEAR,
    MENU_DBDATA_STARTINDEX,
    MENU_DBDATA_STOPINDEX,
    
    MENU_HELP_ABOUT,
      /////////////////////////////////////////////////june 27 2008 10:17 am////////////////////////
    //MENU_CONTROL_DATAVIEWER,//amir's addition
    MENU_G12_POS_VIEWER,
    MENU_G12_SAT_VIEWER,
    MENU_ADU5_PAT_VIEWER,
    MENU_ADU5_SAT_VIEWER,
    MENU_ADU5_VTG_VIEWER,
    MENU_HEADER_VIEWER,
    MENU_MONITOR_VIEWER,
    MENU_COMMAND_VIEWER,
    MENU_HK_VIEWER,
    MENU_HK_CAL_VIEWER,
    MENU_DB_HK_VIEWER,
    MENU_DB_HEADER_VIEWER,
    MENU_DB_MONITOR_VIEWER,
    MENU_DB_CMD_VIEWER
    ///////////////////////////////////////////////////////////////////////////////////////////
  };

  // Command line handler
  wxCmdLineParser *cmdLine;

  // Menu dialogs
  OptionsDialog *m_OptionsDialog;
  ConfigViewer *m_configViewer;

  ////////////////////june 30 2008 //////////////////////////
  GpsPosViewer *m_gpsposViewer;
  G12SatViewer *m_g12satViewer;
  ADU5PatViewer *m_adu5patViewer;
  ADU5SatViewer *m_adu5satViewer;
  ADU5VtgViewer *m_adu5vtgViewer;
  MonitorViewer *m_monitorViewer;
  HeaderViewer *m_headerViewer;
  CommandViewer *m_commandViewer;
  HKViewer *m_hkViewer;
  HKCalViewer *m_hkcalViewer;
  DB_HKViewer *m_dbhkViewer;
  DB_HeaderViewer *m_dbheaderViewer;
  DB_MonitorViewer *m_dbmonitorViewer;
  DB_CmdViewer *m_dbcmdViewer;
  /////////////////////////////////////////////////////////////
  
  DBTerminal *m_dbTerminal;
  ControlPanel *m_flightControl;
  ControlPanel *m_dbControl;

  // Directory and files
  wxFileName *currentRawFile;
  wxFileName *pedestalFile;
  wxArrayString *rawFileList;
  wxFileName packetDirectory[N_PCKTYPE];
  wxArrayString *packetFileList[N_PCKTYPE];
  wxString packetTable[N_PCKTYPE+1];
  dbRef *packetList[N_PCKTYPE];
  unsigned int packetListRequestSize[N_PCKTYPE];
  int packetListItems[N_PCKTYPE];     // Keep track of length of each list
  int packetListAllocSize[N_PCKTYPE]; // Keep track of allocated size of each list
  time_t packetListUpdateTime[N_PCKTYPE]; // Keep time of update
  time_t packetListOldestTime[N_PCKTYPE]; // Keep time of earliest packet indexed
  bool needsIndex[N_PCKTYPE];  // Store notification info
  bool needsRetroIndex[N_PCKTYPE];  // Store retrograde indexing status

  // Delay timer 
  wxTimer *m_rawEventLoadTimer;
  wxTimer *m_currentTimeTimer;
  wxTimer *m_updateTimer;
  wxTimer *m_eventRateTimer;
  wxTimer *m_retroIndexTimer;
  long m_readDelay;

  // Child windows 
  wxLog *m_logTarget;
  wxTextCtrl *m_LogWindow;
  HeaderInfo *m_HeaderInfo;
  TriggerInfo *m_TriggerInfo;
  BarInfo *m_DiskInfo;
  wxStaticText *m_BladeLabel;
  wxStaticText *m_USBExtLabel;
  wxStaticText *m_USBIntLabel;
  BarInfo *m_QueueInfo;
  TextInfo *m_CurrentInfo;
  TextInfo *m_VoltageInfo;
  TextInfo *m_PowerInfo;
  BarInfo *m_TempInfo;
  AuxInfo *m_AuxInfo;
  wxRangedGauge *m_eventRate;
  wxBoxSizer *rfSizer;
  wxGridSizer *surfGrid;
  wxGridSizer *phiGrid;
  wxStaticText **midLabel;
  wxGridSizer *newphiGrid;//peng
  wxGridSizer *surfGridLabel;
  wxGridSizer *phiGridLabel;
  wxGridSizer *newphiGridLabel;//peng
  wxBoxSizer *clockLabel;
  wxBoxSizer *newclockLabel;//peng
  wxBoxSizer *tierLabel;
  wxBoxSizer *newtierLabel;//peng
  wxGridSizer *rfClockGridH;
  wxGridSizer *rfClockGridV;
  wxGridSizer *l3Grid;
  PayloadPanel *payloadView;
  wxRFTab *channelTab[N_SURF][N_CHAN];  // Small summary RF tab windows
  wxRFWindow *rfDisplay[N_SURF][N_CHAN]; // Full RF display window
  GPSDataInfo *m_GPSDataInfo;
  GPSSatInfo *m_GPSSatInfo;
  TrigPatternInfo *m_trigPatternInfo;
  TimeTranslator *m_timeTranslator;

  // Data storage
  int zeroSCA;
  bool pedestalsRead;
  float pedestalData[N_SURF][N_CHIP][N_CHAN][N_SCA];
  wxGraphCurve *rfCurve[N_SURF][N_CHAN];
  double wvtimebase[MAX_NUMBER_SAMPLES];
  FullAnalogueStruct_t *hkcal;
  FullAnalogueStruct_t *hkavz;

  // Data handlers
  AviewXMLHandler *m_xmlHandler;
  DBHandler *m_dbHandler;
  FileHandler *m_fileHandler;

  // DB parser associated
  ArrayOfInts sqlRequestType;
  bool showProgress;
  time_t lastSQLRequest;
  bool updateControlPanelIndex;
  bool initialDBIndexing;

  wxString packetFilter[N_PCKTYPE];
  wxArrayString::CompareFunction packetSortFunction[N_PCKTYPE];
};

#endif //_AVIEWFRAME_H_
    
 




