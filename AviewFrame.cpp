/*
 * AviewFrame.cpp
 *
 * Definitions for main AnitaViewer frame. Defines all menus, children 
 * windows (both ones visible in the main frame, and ones that are in separate
 * dialog windows), most timers, and has routines for parsing input files 
 * and parsing of database queries. It receives input from control panels
 * and issues actions accordingly. DB and directory indexing is also controled
 * from here.  
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "AviewFrame.h"
#endif

#include <wx/splash.h>
#include <wx/msgdlg.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/cmdline.h>
#include <wx/textfile.h>
#include <expat.h>
#include <wx/imagpng.h>
#include <zlib.h>
#include "range.h"
#include "AviewFrame.h"
#include "sort.h"
#include "rfpower_cal.h"
#include <includes/anitaStructures.h>
#include <compressLib/compressLib.h>
#include "strstream"
#include "string.h"


extern struct SurfMapType surfMap[];

// Definitions needed to read in RF waveforms and other files
#define HIT_BUS 0x1000
#define SMP_RATE 2.6  /* LAB sampling rate in GHz */
#define ADCMV 0.939   /* mV/adc, per Gary's email of 05/04/2006 */
#define SATURATION 1300
#define N_RF_PER_FILE 100
#define N_PACKET_PER_FILE 1000
#define MAX_DB_WVAVG 200

IMPLEMENT_CLASS( AviewFrame, wxFrame )

BEGIN_EVENT_TABLE( AviewFrame, wxFrame )
  EVT_MENU( MENU_CONTROL_GPSDATA, AviewFrame::OnMenuControlGPSData )
  EVT_MENU( MENU_CONTROL_GPSSAT, AviewFrame::OnMenuControlGPSSat )
  EVT_MENU( MENU_CONTROL_TRIGPATTERN, AviewFrame::OnMenuControlTrigPattern )
  EVT_MENU( MENU_CONTROL_DUMPRF, AviewFrame::OnMenuControlDumpRF )
  EVT_MENU( MENU_CONTROL_CONVERTTIME, AviewFrame::OnMenuControlConvertTime )
  EVT_MENU( MENU_CONTROL_CLEARDBMREF, AviewFrame::OnMenuControlClearDBMRef )
  EVT_MENU( MENU_CONTROL_CLEARPOWERREF, AviewFrame::OnMenuControlClearPowerRef )
  EVT_MENU( MENU_CONTROL_OPTIONS, AviewFrame::OnMenuControlOptions )
  EVT_MENU( MENU_CONTROL_SAVEOPTIONS, AviewFrame::OnMenuControlSaveOptions )
  EVT_MENU( MENU_CONTROL_LOADOPTIONS, AviewFrame::OnMenuControlLoadOptions )
  EVT_MENU( MENU_CONTROL_QUIT, AviewFrame::OnMenuControlQuit )
  
  EVT_MENU( MENU_RAWDATA_OPEN, AviewFrame::OnMenuRawDataOpen )
  EVT_MENU( MENU_RAWDATA_NEXT, AviewFrame::OnMenuRawDataNext )
  EVT_MENU( MENU_RAWDATA_PREV, AviewFrame::OnMenuRawDataPrev )
  EVT_MENU( MENU_RAWDATA_START, AviewFrame::OnMenuRawDataStart )
  EVT_MENU( MENU_RAWDATA_STOP, AviewFrame::OnMenuRawDataStop )
  EVT_MENU( MENU_RAWDATA_PEDESTAL, AviewFrame::OnMenuRawDataPedestal )
	    
  EVT_MENU( MENU_FLIGHTDATA_CONTROL, AviewFrame::OnMenuFlightDataOpenControl )
  EVT_MENU( MENU_FLIGHTDATA_OPENRF, AviewFrame::OnMenuFlightDataOpenRF )
  EVT_MENU( MENU_FLIGHTDATA_NEXTRF, AviewFrame::OnMenuFlightDataNextRF )
  EVT_MENU( MENU_FLIGHTDATA_PREVRF, AviewFrame::OnMenuFlightDataPrevRF )
  EVT_MENU( MENU_FLIGHTDATA_AVERAGEWV, AviewFrame::OnMenuFlightDataAverageWV )
  EVT_MENU( MENU_FLIGHTDATA_PEDESTAL, AviewFrame::OnMenuFlightDataPedestal )
  EVT_MENU( MENU_FLIGHTDATA_DBMREF, AviewFrame::OnMenuFlightDataDBMRef )
  EVT_MENU( MENU_FLIGHTDATA_OPENHK, AviewFrame::OnMenuFlightDataOpenHK )
  EVT_MENU( MENU_FLIGHTDATA_CALIBHK, AviewFrame::OnMenuFlightDataCalibHK )
  EVT_MENU( MENU_FLIGHTDATA_NEXTHK, AviewFrame::OnMenuFlightDataNextHK )
  EVT_MENU( MENU_FLIGHTDATA_PREVHK, AviewFrame::OnMenuFlightDataPrevHK )
  EVT_MENU( MENU_FLIGHTDATA_OPENGPS, AviewFrame::OnMenuFlightDataOpenGPS )
  EVT_MENU( MENU_FLIGHTDATA_NEXTGPS, AviewFrame::OnMenuFlightDataNextGPS )
  EVT_MENU( MENU_FLIGHTDATA_PREVGPS, AviewFrame::OnMenuFlightDataPrevGPS )
  EVT_MENU( MENU_FLIGHTDATA_OPENMONITOR, AviewFrame::OnMenuFlightDataOpenMonitor )
  EVT_MENU( MENU_FLIGHTDATA_NEXTMONITOR, AviewFrame::OnMenuFlightDataNextMonitor )
  EVT_MENU( MENU_FLIGHTDATA_PREVMONITOR, AviewFrame::OnMenuFlightDataPrevMonitor )
  EVT_MENU( MENU_FLIGHTDATA_OPENCMD, AviewFrame::OnMenuFlightDataOpenCmd )
  EVT_MENU( MENU_FLIGHTDATA_NEXTCMD, AviewFrame::OnMenuFlightDataNextCmd )
  EVT_MENU( MENU_FLIGHTDATA_PREVCMD, AviewFrame::OnMenuFlightDataPrevCmd )

  EVT_MENU( MENU_DBDATA_CONNECT, AviewFrame::OnMenuDBDataConnect )
  EVT_MENU( MENU_DBDATA_DISCONNECT, AviewFrame::OnMenuDBDataDisconnect )
  EVT_MENU( MENU_DBDATA_CONTROL, AviewFrame::OnMenuDBDataControl )
  EVT_MENU( MENU_DBDATA_NEXT, AviewFrame::OnMenuDBDataNext )
  EVT_MENU( MENU_DBDATA_PREV, AviewFrame::OnMenuDBDataPrev )
  EVT_MENU( MENU_DBDATA_LAST, AviewFrame::OnMenuDBDataLast )
  EVT_MENU( MENU_DBDATA_AUTO, AviewFrame::OnMenuDBDataAuto )
  EVT_MENU( MENU_DBDATA_OPENEVENT, AviewFrame::OnMenuDBDataOpenEvent )
  EVT_MENU( MENU_DBDATA_WVAVG, AviewFrame::OnMenuDBDataWaveformAverage )
  EVT_MENU( MENU_DBDATA_DBMREF, AviewFrame::OnMenuDBDataDBMRef )
  EVT_MENU( MENU_DBDATA_CONFIGVIEWER, AviewFrame::OnMenuDBDataConfigViewer )
  EVT_MENU( MENU_DBDATA_TERMINAL, AviewFrame::OnMenuDBDataTerminal )
  EVT_MENU( MENU_DBDATA_CLEAR, AviewFrame::OnMenuDBDataClear )
  EVT_MENU( MENU_DBDATA_STARTINDEX, AviewFrame::OnMenuDBDataStartIndex )
  EVT_MENU( MENU_DBDATA_STOPINDEX, AviewFrame::OnMenuDBDataStopIndex )
    
  EVT_MENU( MENU_HELP_ABOUT, AviewFrame::OnMenuHelpAbout )

/////////////////////////////////////////june 27 2008 11:16 am////////////
  /////////////////////////june 30 2008 10:21 am/////////////////////////////////////
  EVT_MENU( MENU_G12_POS_VIEWER, AviewFrame::OnMenuG12PosViewer)//amir's addition
  EVT_MENU( MENU_G12_SAT_VIEWER, AviewFrame::OnMenuG12SatViewer)//amir's addition
  EVT_MENU( MENU_ADU5_PAT_VIEWER, AviewFrame::OnMenuADU5PatViewer)//amir's addition
  EVT_MENU( MENU_ADU5_SAT_VIEWER, AviewFrame::OnMenuADU5SatViewer)//amir's addition
  EVT_MENU( MENU_ADU5_VTG_VIEWER, AviewFrame::OnMenuADU5VtgViewer)//amir's addition
  EVT_MENU( MENU_HEADER_VIEWER, AviewFrame::OnMenuHeaderViewer)//amir's addition
  EVT_MENU( MENU_MONITOR_VIEWER, AviewFrame::OnMenuMonitorViewer)//amir's addition
  EVT_MENU( MENU_COMMAND_VIEWER, AviewFrame::OnMenuCommandViewer)//amir's addition
  EVT_MENU( MENU_HK_VIEWER, AviewFrame::OnMenuHKViewer)//amir's addition
  EVT_MENU( MENU_HK_CAL_VIEWER, AviewFrame::OnMenuHKCalViewer)//amir's addition
  EVT_MENU( MENU_DB_HK_VIEWER, AviewFrame::OnMenuDBHKViewer)//amir's addition
  EVT_MENU( MENU_DB_HEADER_VIEWER, AviewFrame::OnMenuDBHeaderViewer)//amir's addition
  EVT_MENU( MENU_DB_MONITOR_VIEWER, AviewFrame::OnMenuDBMonitorViewer)//amir's addition
  EVT_MENU( MENU_DB_CMD_VIEWER, AviewFrame::OnMenuDBCmdViewer)//amir's addition
//////////////////////////////////////////////////////////////////////////////

  EVT_TIMER( EVENT_LOAD_TIMER, AviewFrame::OnLoadEventTimer )
  EVT_TIMER( CURRENT_TIME_TIMER, AviewFrame::OnCurrentTimeTimer )
  EVT_TIMER( UPDATE_TIMER, AviewFrame::OnUpdateTimer )
  EVT_TIMER( DBCONNECTION_TIMER, AviewFrame::CheckDBConnection )
  EVT_TIMER( DBCONNECTION_TIMER, AviewFrame::CheckDBConnection )
  EVT_TIMER( EVENTRATE_TIMER, AviewFrame::OnEventRateTimer )
  EVT_TIMER( RETROINDEX_TIMER, AviewFrame::OnRetroIndexTimer )

  EVT_ANTENNA_SELECTED( PAYLOAD_VIEW, AviewFrame::OnAntennaSelect)
  EVT_COMMAND_RANGE(INT_MIN,-2,wxSQL_REQUEST,AviewFrame::OnSQLRequest)
  EVT_COMMAND_RANGE(RFEVENT+CONTROLWIN_OFFSET,CMDECHO+CONTROLWIN_OFFSET,wxLOAD_FILE,AviewFrame::ReadFlightData)
  EVT_COMMAND_RANGE(RFEVENT+CONTROLWIN_OFFSET,CMDECHO+CONTROLWIN_OFFSET,wxREQUEST_PACKET,AviewFrame::RequestPacket)
  EVT_COMMAND(CMDECHO+CONTROLWIN_OFFSET+1,wxNO_RESULTS, AviewFrame::NoResultsMessage)

END_EVENT_TABLE()

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

// Constructor
  AviewFrame::AviewFrame( wxWindow* parent, int argc, char **argv, wxWindowID id, 
			  const wxPoint& pos, const wxSize& size){
  using namespace AviewOpts;

  // Here we need to parse the command line
  cmdLine=new wxCmdLineParser(argc,argv);
  static const wxCmdLineEntryDesc cmdLineDesc[] ={
    { wxCMD_LINE_OPTION, _T("a"), _T("auto"),  _T("Automatically load files with specified delay in seconds"), wxCMD_LINE_VAL_NUMBER},
    { wxCMD_LINE_OPTION, _T("p"), _T("pedestal"),  _T("Raw data pedestal file") },
    { wxCMD_LINE_OPTION, _T("P"), _T("flightped"), _T("Flight data pedestal file") },
    { wxCMD_LINE_OPTION, _T("d"), _T("directory"), _T("Raw data directory") },
    { wxCMD_LINE_OPTION, _T("t"), _T("telemetry"), _T("Telemetry data top directory") },
    { wxCMD_LINE_SWITCH, _T("c"), _T("defcon"), _T("Connect to default database") },
    { wxCMD_LINE_OPTION, _T("C"), _T("connect"), _T("Connect to database with parameters given in form <host[:dbname[:user[:passwd]]]>") },	
    { wxCMD_LINE_OPTION, _T("o"), _T("options"), _T("Display options file") },	
    { wxCMD_LINE_SWITCH, _T("0"), _T("zerosca"), _T("Display 0th SCA")},
    { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("This help"),wxCMD_LINE_VAL_NONE,wxCMD_LINE_OPTION_HELP}, 
    { wxCMD_LINE_NONE }
  };

  cmdLine->SetDesc(cmdLineDesc);
  if(cmdLine->Parse()){ 
    exit(0);
  }
  // Initiate database connection now to save time
  m_dbHandler=NULL;   
  wxString dbParams;
  if(cmdLine->Found(wxT("c"))){
    m_dbHandler=new DBHandler();
    m_dbHandler->Connect(dbHost,dbUser,dbPwd,dbName);
  }else if(cmdLine->Found(wxT("C"),&dbParams)){
    dbHost=dbParams.BeforeFirst(':');
    wxString rest=dbParams.AfterFirst(':');
    if(rest.Length()){
      dbName=rest.BeforeFirst(':');
      rest=rest.AfterFirst(':');
    }
    if(rest.Length()){
      dbUser=rest.BeforeFirst(':');
      rest=rest.AfterFirst(':');
    }
    if(rest.Length()) dbPwd=rest;

    m_dbHandler=new DBHandler(dbHost,dbUser,dbPwd,dbName);
  }
  
  // Show splash

  //  printf("AveiwFrame: try to show splash frame.\n") ;
  fflush(stdout) ;

  wxBitmap bitmap;
  wxSplashScreen* splash = NULL;
  wxImage::AddHandler( new wxPNGHandler );
  if (bitmap.LoadFile(SPLASH_FILE, wxBITMAP_TYPE_PNG)){
    splash = new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
				0, NULL, -1, wxDefaultPosition, wxDefaultSize,
				//wxSTAY_ON_TOP);
				wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    splash->Show(true);
  }

  Create( parent, id, TITLE, pos, size, wxDEFAULT_FRAME_STYLE|wxFULL_REPAINT_ON_RESIZE);
  CreateStatusBar(3);

  this->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  // Create menus
  m_pMenuBar = new wxMenuBar;

  m_pControlMenu = new wxMenu;
  m_pControlMenu->Append(MENU_CONTROL_GPSDATA, _("GPS data"), 
			 _T("Open GPS data window"));
  m_pControlMenu->Append(MENU_CONTROL_GPSSAT, _("GPS satellite"), 
			 _T("Open GPS satellite window"));
  m_pControlMenu->Append(MENU_CONTROL_TRIGPATTERN, _("Trigger Pattern"), 
			 _T("Open trigger pattern histograms"));
  m_pControlMenu->AppendSeparator();
  m_pControlMenu->Append(MENU_CONTROL_CONVERTTIME, _("Convert time\tt"), 
			 _T("Convert between UTC and UNIX time"));
  m_pControlDumpRF =  m_pControlMenu->Append(MENU_CONTROL_DUMPRF, _("Dump RF\td"), 
					     _T("Write RF data to a file"));
  m_pControlMenu->Append(MENU_CONTROL_CLEARDBMREF,_("Clear dBM references"),
			 _T("Clear external RF power reference values"));
  m_pControlMenu->Append(MENU_CONTROL_CLEARPOWERREF,_("Clear RF power references"),
			 _T("Clear internal RF power reference values"));
  m_pControlMenu->AppendSeparator();
  m_pControlMenu->Append(MENU_CONTROL_OPTIONS, _("Options\to"), _T("Setup Viewer options"));
  m_pControlMenu->Append(MENU_CONTROL_SAVEOPTIONS, _("Save options"), _T("Save Viewer options"));
  m_pControlMenu->Append(MENU_CONTROL_LOADOPTIONS, _("Load options"), _T("Load Viewer options"));
  m_pControlMenu->AppendSeparator();
  m_pControlMenu->Append(MENU_CONTROL_QUIT, _("Quit\tCTRL-q"), _T("Quit Anita Viewer"));
  m_pMenuBar->Append(m_pControlMenu,_("Control"));

  m_pRawDataMenu = new wxMenu;
  m_pRawDataMenu->Append(MENU_RAWDATA_OPEN,_("Open"), _T("Open raw waveform data file"));
  m_pRawDataNext = m_pRawDataMenu->Append(MENU_RAWDATA_NEXT, _("Next\tShift-n"), 
					  _T("Next raw waveform data file"));
  m_pRawDataPrev = m_pRawDataMenu->Append(MENU_RAWDATA_PREV, _("Previous\tShift-p"), 
					  _T("Previous raw waveform data file"));
  m_pRawDataMenu->AppendSeparator();
  m_pRawDataStart =  m_pRawDataMenu->Append(MENU_RAWDATA_START, _("Start\ts"), 
					    _T("Start raw event autoloading"));
  m_pRawDataStop = m_pRawDataMenu->Append(MENU_RAWDATA_STOP, _("Stop\tShift-s"), 
					  _T("Stop raw event autoloading"));
  m_pRawDataMenu->AppendSeparator();
  m_pRawDataMenu->Append(MENU_RAWDATA_PEDESTAL,_("Load pedestals"), _T("Load raw pedestal file"));
  m_pMenuBar->Append(m_pRawDataMenu,_("Raw data"));

  m_pFlightDataMenu = new wxMenu;
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_CONTROL,_("Control Panel\tShift-c"), _T("Open flight data control panel"));
  m_pFlightDataMenu->AppendSeparator();
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_OPENRF,_("Open RF"), _T("Open waveform event file"));
  m_pFlightDataNextRF = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_NEXTRF,_("Next RF\tCTRL-n"),
						  _T("Next waveform event file"));
  m_pFlightDataPrevRF = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_PREVRF,_("Previous RF\tCTRL-p"),
						  _T("Previous waveform event file"));
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_AVERAGEWV,_("Average Waveform\ta"),
			    _T("Average all waveforms in event file"));
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_PEDESTAL,_("Load Pedestals"),
			    _T("Load pedestal file"));
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_DBMREF,_("Load dBm References\tb"),
			    _T("Load SURFHK file for reference RF power"));
  m_pFlightDataMenu->AppendSeparator();
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_OPENHK,_("Open HK"), _T("Open housekeeping file"));
  m_pFlightDataNextHK = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_NEXTHK,_("Next HK"),
						  _T("Next housekeeping file"));
  m_pFlightDataPrevHK = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_PREVHK,_("Previous HK"),
						  _T("Previous housekeeping file"));
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_CALIBHK,_("Load HK Calib"),
			    _T("Load housekeeping calibration"));
  m_pFlightDataMenu->AppendSeparator();
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_OPENGPS,_("Open GPS"), _T("Open housekeeping file"));
  m_pFlightDataNextGPS = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_NEXTGPS,_("Next GPS"),
						   _T("Next GPS file"));
  m_pFlightDataPrevGPS = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_PREVGPS,_("Previous GPS"),
						   _T("Previous GPS file"));
  m_pFlightDataMenu->AppendSeparator();
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_OPENMONITOR,_("Open monitor"), _T("Open CPU monitor file"));
  m_pFlightDataNextMonitor = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_NEXTMONITOR,_("Next monitor"),
						   _T("Next CPU monitor file"));
  m_pFlightDataPrevMonitor = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_PREVMONITOR,_("Previous monitor"),
						   _T("Previous monitor file"));
  m_pFlightDataMenu->AppendSeparator();
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_OPENCMD,_("Open cmd"), _T("Open cmd file"));
  m_pFlightDataNextCmd = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_NEXTCMD,_("Next cmd"),
						   _T("Next command echo file"));
  m_pFlightDataPrevCmd = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_PREVCMD,_("Previous cmd"),
						   _T("Previous command echo file"));
  m_pMenuBar->Append(m_pFlightDataMenu,_("Flight data"));

  m_pDBDataMenu = new wxMenu;
  m_pDBDataConnect = m_pDBDataMenu->Append(MENU_DBDATA_CONNECT,_("Connect\tc"), 
					   _T("Connect to ANITA database"));
  m_pDBDataDisconnect = m_pDBDataMenu->Append(MENU_DBDATA_DISCONNECT,_("Disconnect\tCTRL-d"), 
					      _T("Disconnect from ANITA database"));
  m_pDBDataMenu->AppendSeparator();
  m_pDBDataControl  = m_pDBDataMenu->Append(MENU_DBDATA_CONTROL,_("Control panel\tCTRL-c"),
					    _T("Open database control panel"));
  m_pDBDataNext = m_pDBDataMenu->Append(MENU_DBDATA_NEXT, _("Next event\tn"), 
					_T("Next event from database"));
  m_pDBDataPrev = m_pDBDataMenu->Append(MENU_DBDATA_PREV, _("Previous event\tp"), 
					_T("Previous event from database"));
  m_pDBDataLast = m_pDBDataMenu->Append(MENU_DBDATA_LAST, _("Last event\tl"), 
					_T("Last event from database"));
  m_pDBDataAuto = m_pDBDataMenu->Append(MENU_DBDATA_AUTO, _("Auto/unAuto all\ta"), 
					_T("Auto / unAuto checkbox"));
  m_pDBDataOpenEvent = m_pDBDataMenu->Append(MENU_DBDATA_OPENEVENT, _("Open event\te"), 
					_T("Event by number from database"));
  m_pDBDataMenu->AppendSeparator();
  m_pDBDataWaveformAverage = m_pDBDataMenu->Append(MENU_DBDATA_WVAVG,_("Average Waveform\tCtrl-a"),_T("Average waveforms from DB"));
  m_pDBDataDBMRef = m_pDBDataMenu->Append(MENU_DBDATA_DBMREF,_("Load dBm References\tCTRL-b"),_T("Load reference RF power from database"));
  m_pDBDataConfigViewer = m_pDBDataMenu->Append(MENU_DBDATA_CONFIGVIEWER,_("Config Viewer\tCTRL-v"),_T("Open config file viewer"));
  m_pDBDataMenu->AppendSeparator();
  m_pDBDataTerminal = m_pDBDataMenu->Append(MENU_DBDATA_TERMINAL,_("Terminal\tCTRL-t"),_T("Open SQL terminal"));
  m_pDBDataClear = m_pDBDataMenu->Append(MENU_DBDATA_CLEAR,_("Clear queue"),_T("Clear SQL command queue"));
  m_pDBDataMenu->AppendSeparator();
  m_pDBDataStartIndex = m_pDBDataMenu->Append(MENU_DBDATA_STARTINDEX,_("Index all packets"),_T("Index all packets in the database"));
  m_pDBDataStopIndex = m_pDBDataMenu->Append(MENU_DBDATA_STOPINDEX,_("Stop indexing"),_T("Stop index packets in the database"));

  m_pMenuBar->Append(m_pDBDataMenu,_("DB data"));

// ///////////////////////////////june 27 2008 11:36/////////////////////////////
   m_pDataView= new wxMenu;//amir's addition
//  // m_pDataViewer= m_pDataView->Append(MENU_CONTROL_DATAVIEWER, _("Data Viewer"), _T("Data Viewer"));//amir's addition
//   //////////////////////////july 1 2008/////////////////////////////////////////
  m_pGpsPosViewer= m_pDataView->Append(MENU_G12_POS_VIEWER, _("DB & Flight G12 Pos Viewer"), _T("DB & Flight G12 Pos Viewer"));//amir's addition
  m_pG12SatViewer= m_pDataView->Append(MENU_G12_SAT_VIEWER, _("DB & Flight G12 Sat Viewer"), _T("DB & Flight G12 Sat Viewer"));//amir's addition
  m_pADU5PatViewer=m_pDataView->Append(MENU_ADU5_PAT_VIEWER, _("DB & Flight ADU5 Pat Viewer"), _T("DB & Flight ADU5 Pat Viewer"));//amir's addition
  m_pADU5SatViewer=m_pDataView->Append(MENU_ADU5_SAT_VIEWER, _("DB & Flight ADU5 Sat Viewer"), _T("DB & Flight ADU5 Sat Viewer"));//amir's addition
  m_pADU5VtgViewer=m_pDataView->Append(MENU_ADU5_VTG_VIEWER, _("DB & Flight ADU5 Vtg Viewer"), _T("DB & Flight ADU5 Vtg Viewer"));//amir's addition
  m_pMonitorViewer=m_pDataView->Append(MENU_MONITOR_VIEWER, _("Flight Monitor Viewer"), _T("Flight Monitor Viewer"));//amir's addition
  m_pHeaderViewer=m_pDataView->Append(MENU_HEADER_VIEWER, _("Flight Header Viewer"), _T("Header Viewer"));//amir's addition
  m_pHKViewer=m_pDataView->Append(MENU_HK_VIEWER, _("Flight HK Viewer"), _T("Flight HK Viewer"));//amir's addition
  m_pHKCalViewer=m_pDataView->Append(MENU_HK_CAL_VIEWER, _("Flight HK Calibration Viewer"), _T("Flight HK Calibration Viewer"));//amir's addition
  m_pCommandViewer=m_pDataView->Append(MENU_COMMAND_VIEWER, _("Flight Command Viewer"), _T("Flight Command Viewer"));//amir's addition
  m_pDBHKViewer=m_pDataView->Append(MENU_DB_HK_VIEWER, _("DB HK Viewer"), _T("DB HK Viewer"));//amir's addition
  m_pDBHeaderViewer=m_pDataView->Append(MENU_DB_HEADER_VIEWER, _("DB Header Viewer"), _T("DB Header Viewer"));//amir's addition
  m_pDBMonitorViewer=m_pDataView->Append(MENU_DB_MONITOR_VIEWER, _("DB Monitor Viewer"), _T("DB Monitor Viewer"));//amir's addition
  m_pDBCmdViewer=m_pDataView->Append(MENU_DB_CMD_VIEWER, _("DB Command Viewer"), _T("DB Command Viewer"));//amir's addition
  m_pMenuBar->Append(m_pDataView,_("Data Viewer"));//amir's additon
// //////////////////////////////////////////////////////////////////////////////////

  m_pHelpMenu = new wxMenu;
  m_pHelpMenu->Append(MENU_HELP_ABOUT,_("About\tF12"),_T("What is this????"));
  m_pMenuBar->Append(m_pHelpMenu,_("Help"));

  this->SetMenuBar(m_pMenuBar);
  m_timeTranslator = new TimeTranslator(this,wxID_ANY, wxT("Time Converter"));
  m_OptionsDialog = new OptionsDialog(this);
  m_GPSDataInfo = new GPSDataInfo(this);
  m_GPSSatInfo = new GPSSatInfo(this);
  m_trigPatternInfo = new TrigPatternInfo(this);

///////////////////////june 27 2008 11:50 am//////////////////////
  //m_dataViewer = new DataViewer(this);
  ///////////////////// june 30 2008///////////////////////////////
  m_gpsposViewer=new GpsPosViewer(this);
  m_g12satViewer=new G12SatViewer(this);
  m_adu5patViewer=new ADU5PatViewer(this);
  m_adu5satViewer=new ADU5SatViewer(this);
  m_adu5vtgViewer=new ADU5VtgViewer(this);
  m_headerViewer=new HeaderViewer(this);
  m_monitorViewer=new MonitorViewer(this); 
  m_hkViewer=new HKViewer(this);
  m_hkcalViewer=new HKCalViewer(this);
  m_commandViewer=new CommandViewer(this);
  m_dbhkViewer=new DB_HKViewer(this);
  m_dbheaderViewer=new DB_HeaderViewer(this);
  m_dbmonitorViewer=new DB_MonitorViewer(this);
  m_dbcmdViewer=new DB_CmdViewer(this);
  //////////////////////////////////////////////////////////////////

  // Create control panels
  m_flightControl = new ControlPanel(this,"FlightControlItem",N_PCKTYPE,_T("File Control Panel"));
  m_dbControl = new ControlPanel(this,"DBControlItem",N_PCKTYPE,_T("Database Control Panel"));
  for(int i=0;i<N_PCKTYPE;++i){
    switch(i){
    case RFEVENT: 
      m_flightControl->SetLabel(i,_T("RF event")); 
      m_dbControl->SetLabel(i,_T("RF event")); 
      break;
    case HEADER:
      m_flightControl->SetLabel(i,_T("Header")); 
      m_dbControl->SetLabel(i,_T("Header")); 
      break;
    case HKEVENT: 
      m_flightControl->SetLabel(i,_T("HK event")); 
      m_dbControl->SetLabel(i,_T("HK event")); 
      break;
    case SSHKEVENT: 
      m_flightControl->SetLabel(i,_T("SS HK")); 
      m_dbControl->SetLabel(i,_T("SS HK")); 
      break;
    case MONITOR: 
      m_flightControl->SetLabel(i,_T("Monitor")); 
      m_dbControl->SetLabel(i,_T("Monitor")); 
      break;
    case ADU5PAT: 
      m_flightControl->SetLabel(i,_T("ADU5 pat")); 
      m_dbControl->SetLabel(i,_T("ADU5 pat")); 
      break;
    case ADU5VTG: 
      m_flightControl->SetLabel(i,_T("ADU5 vtg")); 
      m_dbControl->SetLabel(i,_T("ADU5 vtg")); 
      break;
    case ADU5SAT: 
      m_flightControl->SetLabel(i,_T("ADU5 sat")); 
      m_dbControl->SetLabel(i,_T("ADU5 sat")); 
      break;
    case G12POS: 
      m_flightControl->SetLabel(i,_T("G12 pos")); 
      m_dbControl->SetLabel(i,_T("G12 pos")); 
      break;
    case G12SAT: 
      m_flightControl->SetLabel(i,_T("G12 sat")); 
      m_dbControl->SetLabel(i,_T("G12 sat")); 
      break;
    case CMDECHO: 
      m_flightControl->SetLabel(i,_T("Cmd echo")); 
      m_dbControl->SetLabel(i,_T("Cmd echo")); 
      break;
    default: break;
    }
  }

  // Create and arrange all children
   topSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *topGrid = new wxBoxSizer(wxHORIZONTAL);
  
  wxBoxSizer *leftGrid = new wxBoxSizer(wxVERTICAL); 
  wxBoxSizer *topLeftGrid = new wxBoxSizer(wxHORIZONTAL); 
  wxBoxSizer *cpuBox = new wxBoxSizer(wxVERTICAL); 
  wxBoxSizer *diskBox = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *diskLabelSizer=new wxBoxSizer(wxVERTICAL);

  m_HeaderInfo = new HeaderInfo(this);
  m_TriggerInfo = new TriggerInfo(this);
  m_DiskInfo = new BarInfo(this,N_DISK,N_DISK,_("Disk Space"),_T("MB"),
			   DiskRefLabels,DiskLabels,DiskCaptions,DiskVars,
			   DiskTable,DiskSpec,DiskRange,DiskAverage,DiskScaleType,
			   wxSize(12,20));
#if 0  // Dummy disk entries
  for(int i=0;i<N_DISK;++i)
    m_DiskInfo->SetDisplayValue(DiskLabels[i],(double)((DiskRange[i][1]-DiskRange[i][0])/(N_DISK))*(i+1)+DiskRange[i][0]);
#endif

  diskLabelSizer->AddStretchSpacer();
//   diskLabelSizer->Add(m_BladeLabel=new wxStaticText(this,wxID_ANY,_("zeusXX")),0);
//   diskLabelSizer->Add(m_USBIntLabel=new wxStaticText(this,wxID_ANY,_("usbintXX")),0);
//   diskLabelSizer->Add(m_USBExtLabel=new wxStaticText(this,wxID_ANY,_("usbextXX")),0);

  m_QueueInfo = new BarInfo(this,N_QUEUE,QUEUE_PER_ROW, _("Queue Entries"),_T(""),
			    QueueRefLabels,QueueLabels,QueueCaptions,
			    QueueVars,QueueTable,QueueSpec,QueueRange,
			    QueueAverage,QueueScaleType,wxSize(10,20));
#if 0  // Dummy queue entries
  for(int i=0;i<N_QUEUE;++i)
    m_QueueInfo->SetDisplayValue(QueueLabels[i],(double)((QueueRange[i][1]-QueueRange[i][0])/(N_QUEUE))*(i+1)+QueueRange[i][0]);
#endif

  wxBoxSizer *rateBox=new wxBoxSizer(wxHORIZONTAL);
  m_eventRate = new wxRangedGauge(this,wxID_ANY,EventRateRange[0],EventRateRange[1],EventRateSpec,
				  EventRateScaleType,EventRateAverage,wxDefaultPosition,
				  wxSize(50,10),wxHORIZONTAL,_T("Event Rate"),_T(""),_T("Hz"));
  rateBox->Add(new wxStaticText(this,wxID_ANY,_T("Ev. rate [0-100Hz] ")),0,wxEXPAND);
  rateBox->Add(m_eventRate,1,wxEXPAND);

  diskBox->Add(m_DiskInfo,1,wxEXPAND);
  diskBox->Add(diskLabelSizer,0,wxEXPAND);

  cpuBox->Add(diskBox,3,wxEXPAND);
  cpuBox->Add(m_QueueInfo,5,wxEXPAND);
  cpuBox->Add(rateBox,1,wxEXPAND);

  topLeftGrid->Add(m_HeaderInfo,100,wxEXPAND|wxRIGHT,3);//from 88 to 110
  topLeftGrid->Add(m_TriggerInfo,62,wxEXPAND|wxRIGHT,3);
  topLeftGrid->Add(cpuBox,100,wxEXPAND);

  // Create all RF displays
  // Loop through antennas to create tabs
  for(int a=0;a<N_RFANT;++a){
    bool alfaOn;		//alfa is true when the RF signal is mixed with ALFA signal.  (nSurf=11, nChan=5)
    wxString tip;
    if(a<2*N_QRH || N_SURF>9){
      tip.Printf(_T("%d%c%c : "),surfMap[a/2].phi,(a/2+0)/32?'B':((a/2+0)/16?'M':'T'),a%2?'V':'H');// removed a/2+1 in label.
    }else if(a<2*N_QRH+4){
      tip.Printf(_T("Bicone %d : "),(a-2*N_QRH)+1);
    }else{
      tip.Printf(_T("Discone %d : "),(a-(2*N_QRH+4))+1);
    }
	       
    // Surf have 1-counting convention; channels have 1-counting convention
    int nSurf,nChan;
    if(a<2*N_QRH || N_SURF>9){
      if(a%2){
	nSurf=surfMap[a/2].v_surf-1;
	nChan=surfMap[a/2].v_chan-1;
      }else{
	nSurf=surfMap[a/2].h_surf-1;
	nChan=surfMap[a/2].h_chan-1;
      }
    }else{
      nSurf=surfMap[N_QRH+(a-2*N_QRH)].h_surf-1;
      nChan=surfMap[N_QRH+(a-2*N_QRH)].h_chan-1;
    }
 
    tip << _T("SURF ") << nSurf+1 << _T(" CH ") << nChan+1;
    if (nSurf==11 && nChan==5) alfaOn=true;
    else alfaOn=false;
    rfDisplay[nSurf][nChan] = new wxRFWindow( this, wxID_ANY, tip, wxDefaultPosition, wxSize(300,300), alfaOn);
    channelTab[nSurf][nChan] = new wxRFTab(this, tip, rfDisplay[nSurf][nChan], wxID_ANY, wxDefaultPosition, wxSize(53,33));    
    channelTab[nSurf][nChan]->SetScale(tabScaleMin,tabScaleMax);
  }
    
  // Create clock waveform tabs
  for(int j=0;j<ACTIVE_SURFS;++j){
    wxString tip;
    tip << _T("SURF ") << j+1 << _T(" CH ") << CHANNELS_PER_SURF;
    
    rfDisplay[j][CHANNELS_PER_SURF-1] = new wxRFWindow( this, wxID_ANY, tip, wxDefaultPosition, wxSize(300,300));
    channelTab[j][CHANNELS_PER_SURF-1] = new wxRFTab(this, tip, rfDisplay[j][CHANNELS_PER_SURF-1], wxID_ANY, wxDefaultPosition, wxSize(53,36));
    channelTab[j][CHANNELS_PER_SURF-1]->SetScale(clockTabScaleMin,clockTabScaleMax);
  }

  // Create lable boxes
  surfGridLabel = new wxGridSizer(1,ACTIVE_SURFS,0,1); 
  for(int i=0;i<ACTIVE_SURFS;++i){
    wxString tmp;
    tmp << _T("\n\nSURF ") << (i+1);
    surfGridLabel->Add(new wxStaticText(this,wxID_ANY,tmp,wxDefaultPosition,
					wxDefaultSize,wxALIGN_CENTRE),
		       1,wxEXPAND);
  }
  wxBoxSizer *phiGridLabelBox=new wxBoxSizer(wxHORIZONTAL);

  phiGridLabel = new wxGridSizer(1,N_PHI/2,0,1); 
  for(int i=0;i<N_PHI/2;++i){
    wxString tmp;
    tmp << _T("Phi ") << (2*i+2) << _T("/") << (2*i+1);
    phiGridLabel->Add(new wxStaticText(this,wxID_ANY,tmp,wxDefaultPosition,
					wxDefaultSize,wxALIGN_CENTRE),
		       1,wxEXPAND);
  }

  clockLabel=new wxBoxSizer(wxVERTICAL);
  clockLabel->Add(new wxStaticText(this,wxID_ANY,_T("Clocks"),wxDefaultPosition,
				   wxDefaultSize,wxALIGN_CENTRE),1,wxEXPAND|wxTop,1);

  phiGridLabelBox->Add(phiGridLabel,8,wxEXPAND|wxTOP,1);
  phiGridLabelBox->Add(clockLabel,1,wxEXPAND);

/////////////////peng///////////////
  wxBoxSizer *newphiGridLabelBox=new wxBoxSizer(wxHORIZONTAL);
  newphiGridLabel = new wxGridSizer(1,N_PHI/2,0,1); 
  for(int i=0;i<N_PHI/2;++i){
    wxString tmp;
    tmp << _T("Phi ") << (i+1) ;
    newphiGridLabel->Add(new wxStaticText(this,wxID_ANY,tmp,wxDefaultPosition,
					wxDefaultSize,wxALIGN_CENTRE),
		       1,wxEXPAND);
  }
  newclockLabel=new wxBoxSizer(wxVERTICAL);
  newclockLabel->Add(new wxStaticText(this,wxID_ANY,_T("Clocks"),wxDefaultPosition,
				   wxDefaultSize,wxALIGN_CENTRE),1,wxEXPAND);

  newphiGridLabelBox->Add(newphiGridLabel,8,wxEXPAND);
  newphiGridLabelBox->Add(newclockLabel,1,wxEXPAND);
/////////////////peng///////////////
  tierLabel = new wxBoxSizer(wxVERTICAL);
  //  tierLabel->AddStretchSpacer();
  tierLabel->Add(new wxStaticText(this,wxID_ANY,_T("\nt\no\np")),3);
  tierLabel->Add(new wxStaticText(this,wxID_ANY,_T("m\ni\nd")),3);
  tierLabel->Add(new wxStaticText(this,wxID_ANY,_T("b\no\nt\nt\no\nm")),6);
  //tierLabel->AddStretchSpacer();
  if (N_SURF>9)
    tierLabel->Add(new wxStaticText(this,wxID_ANY,_T("\nn\na\nd\ni\nr")),0);
  else
    tierLabel->Add(new wxStaticText(this,wxID_ANY,_T("\nv")),0);

////////////peng//////////
  newtierLabel = new wxBoxSizer(wxVERTICAL);
  newtierLabel->Add(new wxStaticText(this,wxID_ANY,_T(" \n t\n o\n p")),4, wxTOP,15);
  newtierLabel->Add(new wxStaticText(this,wxID_ANY,_T("\n m\n i\n d")),4);
  newtierLabel->Add(new wxStaticText(this,wxID_ANY,_T("\n b\n o\n t\n t\n o\n m")),6);
  newtierLabel->AddStretchSpacer();
  newtierLabel->Add(new wxStaticText(this,wxID_ANY,_T(" \n t\n o\n p")),4);
  newtierLabel->Add(new wxStaticText(this,wxID_ANY,_T("\n m\n i\n d")),4, wxTOP,25);
  newtierLabel->Add(new wxStaticText(this,wxID_ANY,_T("\n b\n o\n t\n t\n o\n m")),6,wxTOP,25);;
///////////peng///////////
  rfSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *rfMidSizer=new wxBoxSizer(wxHORIZONTAL);

  surfGrid = new wxGridSizer(ACTIVE_SURFS,1,1); 
  phiGrid = new wxGridSizer(PHI_SECTORS/2,1,1); 
  newphiGrid = new wxGridSizer(PHI_SECTORS/2,1,1);///peng 
  midLabel = new wxStaticText*[PHI_SECTORS/2];
  for(int i=0; i<PHI_SECTORS/2; i++) midLabel[i]=0;

  payloadView = new PayloadPanel(this, PAYLOAD_VIEW, RFRefLabels);
  rfClockGridV = new wxGridSizer(ACTIVE_SURFS,1,1,1);
  rfClockGridH = new wxGridSizer(1,ACTIVE_SURFS,1,1);
  l3Grid = new wxGridSizer(0,1,1,1);

  rfSizer->Add(surfGridLabel,1,wxEXPAND);
  rfSizer->Add(phiGridLabelBox,0,wxEXPAND);
  rfSizer->Add(newphiGridLabelBox,0,wxEXPAND);//peng


  rfSizer->Add(rfMidSizer,8,wxEXPAND);
  rfMidSizer->Add(tierLabel,0,wxEXPAND);
  rfMidSizer->Add(newtierLabel,2,wxEXPAND);//peng
  rfMidSizer->Add(surfGrid,80,wxEXPAND);
  rfMidSizer->Add(phiGrid,80,wxEXPAND);
  rfMidSizer->Add(newphiGrid,80,wxEXPAND);//peng
  rfMidSizer->Add(payloadView,80,wxEXPAND);
  rfMidSizer->Add(l3Grid,10);
  rfMidSizer->Add(rfClockGridV,10,wxEXPAND);  

  rfSizer->Add(rfClockGridH,1,wxEXPAND);//peng

  leftGrid->Add(topLeftGrid,0,wxEXPAND);
  leftGrid->Add(rfSizer,1,wxEXPAND);

  // Atach clock tabs
  switch(RFOverview){
  case PhiView:
    for(int j=0;j<ACTIVE_SURFS;++j)
      rfClockGridV->Add(channelTab[j][CHANNELS_PER_SURF-1],1,wxEXPAND);
    break;
//////////////////////peng///////////////////
  case newPhiView:
    for(int j=0;j<ACTIVE_SURFS;++j)
      rfClockGridV->Add(channelTab[j][CHANNELS_PER_SURF-1],1,wxEXPAND);
    break;
//////////////////////peng/////////////////
  case SurfView:
    for(int j=0;j<ACTIVE_SURFS;++j)
      rfClockGridH->Add(channelTab[j][CHANNELS_PER_SURF-1],1,wxEXPAND);
    break;
  case PayloadView:
  default:
    for(int j=0;j<ACTIVE_SURFS;++j)
      rfClockGridV->Add(channelTab[j][CHANNELS_PER_SURF-1],1,wxEXPAND);
    break;
  }
  // Arrange tabs
  SwitchRFView(false);

  wxBoxSizer *rightGrid = new wxBoxSizer(wxVERTICAL); 
  m_VoltageInfo = new TextInfo(this,N_VOLTAGE,VOLTAGE_PER_COL,_("Voltages"),
			       _T("Volts"),VoltageLabels,VoltageCaptions, 
			       VoltageVars,VoltageTable,VoltageSpec);
   m_CurrentInfo = new TextInfo(this,N_CURRENT,CURRENT_PER_COL,_("Currents"),
				_T("Amps"),CurrentLabels,CurrentCaptions,
				CurrentVars,CurrentTable,CurrentSpec,
				_T("%.3f"));
  m_PowerInfo = new TextInfo(this,N_POWER,POWER_PER_COL,_("Powers"),
			       _T("W"),PowerLabels,PowerCaptions, 
			       PowerVars,PowerTable,PowerSpec);
  m_TempInfo = new BarInfo(this,N_TEMP,TEMP_PER_ROW,_("Temperatures"),_T("deg C"),
			   TempLabels,TempLabels,TempCaptions,TempVars,
			   TempTable,TempSpec,TempRange,TempAverage,TempScaleType,
			   wxSize(16,36));
  m_AuxInfo = new AuxInfo(this);
  rightGrid->Add(m_VoltageInfo,0,wxEXPAND);
  rightGrid->Add(m_CurrentInfo,0,wxEXPAND);
  rightGrid->Add(m_PowerInfo,0,wxEXPAND);
  rightGrid->Add(m_TempInfo,1,wxEXPAND);
  rightGrid->Add(m_AuxInfo,0,wxEXPAND);

  topGrid->Add(leftGrid,153,wxEXPAND);
  topGrid->Add(rightGrid,60,wxEXPAND);

  m_LogWindow = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,
			       wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);
  m_LogWindow->SetSize(wxSize(200,20));
  m_logTarget = wxLog::SetActiveTarget( new wxLogTextCtrl(m_LogWindow) );
  wxLogMessage(_T("Welcome to Aview - ANITA Data Display Utility"));

  topSizer->Add(topGrid,10,wxEXPAND);
  topSizer->Add(m_LogWindow,0,wxEXPAND);

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  Centre();

  // Initialize data handlers
  for(int i=0;i<MAX_NUMBER_SAMPLES;++i) wvtimebase[i]=(double)i/SMP_RATE;
  pedestalsRead=false;
  for(int i=0;i<CHANNELS_PER_SURF;++i)
    for(int j=0;j<ACTIVE_SURFS;++j)
      rfCurve[j][i]=NULL;

  hkcal=NULL;
  hkavz=NULL;

  rawFileList=new wxArrayString();
  currentRawFile=new wxFileName();
  pedestalFile=new wxFileName();
  for(int i=0;i<N_PCKTYPE;++i){
    packetFileList[i]=new wxArrayString();
    packetDirectory[i]=_T("");
    packetList[i]=NULL;
    packetListItems[i]=0;
    packetListAllocSize[i]=0;
    packetListUpdateTime[i]=0; 
    packetListOldestTime[i]=time(NULL);
    needsIndex[i]=false;
    needsRetroIndex[i]=true;
  }

  m_rawEventLoadTimer=new wxTimer(this,EVENT_LOAD_TIMER);
  m_currentTimeTimer=new wxTimer(this,CURRENT_TIME_TIMER);
  m_currentTimeTimer->Start(1000);
  m_updateTimer=new wxTimer(this,UPDATE_TIMER);
  m_updateTimer->Start(1500);
  m_eventRateTimer=new wxTimer(this,EVENTRATE_TIMER);
  m_retroIndexTimer=new wxTimer(this,RETROINDEX_TIMER);

  m_xmlHandler=new AviewXMLHandler();
  m_fileHandler=new FileHandler();
  showProgress=false;
  lastSQLRequest=0;
  updateControlPanelIndex=false;
  initialDBIndexing=false;
  if(m_dbHandler){ // DB connection already initiated
    // Start oneshot timer which will check on connection status
    wxTimer *dbTimer=new wxTimer(this,DBCONNECTION_TIMER);
    dbTimer->Start(1,true);
  }else
    m_dbHandler=new DBHandler();
  m_dbTerminal=new DBTerminal(this);
  m_configViewer=new ConfigViewer(this);

  // Enable/disable appropriate menu items and other initializations
  init();

  // Remove splash
  if(splash){
    splash->Show(false);
    delete splash;
  }
}

// Initialize
void AviewFrame::init(){
  // Define packet filters
  packetFilter[RFEVENT]=_T("hd_*.dat*");
  packetFilter[HEADER]=_T("hd_*.dat*");
  packetFilter[HKEVENT]=_T("hk_raw_*.dat*");
  packetFilter[SSHKEVENT]=_T("sshk_raw_*.dat*");
  packetFilter[MONITOR]=_T("mon_*.dat*");
  packetFilter[ADU5PAT]=_T("pat_*.dat*");
  packetFilter[ADU5VTG]=_T("vtg_*.dat*");
  packetFilter[ADU5SAT]=_T("sat_adu5_*.dat*");
  packetFilter[G12POS]=_T("pos_*.dat*");
  packetFilter[G12SAT]=_T("sat_1*.dat*"); // Hack assuming timestamps start with 1
  packetFilter[CMDECHO]=_T("cmd_*.dat*");
  // Define packet file sorters
  packetSortFunction[RFEVENT]=singleIndexSortFunction;
  packetSortFunction[HEADER]=singleIndexSortFunction;
  packetSortFunction[HKEVENT]=doubleIndexSortFunction;
  packetSortFunction[SSHKEVENT]=doubleIndexSortFunction;
  packetSortFunction[MONITOR]=singleIndexSortFunction;
  packetSortFunction[ADU5PAT]=doubleIndexSortFunction;
  packetSortFunction[ADU5VTG]=doubleIndexSortFunction;
  packetSortFunction[ADU5SAT]=singleIndexSortFunction;
  packetSortFunction[G12POS]=doubleIndexSortFunction;
  packetSortFunction[G12SAT]=singleIndexSortFunction;
  packetSortFunction[CMDECHO]=doubleIndexSortFunction;
  // Define packet tables
  packetTable[RFEVENT]=_T("rf");
  packetTable[HEADER]=_T("hd");
  packetTable[HKEVENT]=_T("hk");
  packetTable[SSHKEVENT]=_T("SShk");
  packetTable[MONITOR]=_T("mon");
  packetTable[ADU5PAT]=_T("adu5_pat");
  packetTable[ADU5VTG]=_T("adu5_vtg");
  packetTable[ADU5SAT]=_T("adu5_sat");
  packetTable[G12POS]=_T("g12_pos");
  packetTable[G12SAT]=_T("g12_sat");
  packetTable[CMDECHO]=_T("cmd");
  packetTable[WVFORM]=_T("wv");
  // Define index size chunk to retreive from the database
  packetListRequestSize[RFEVENT]=2000;
  packetListRequestSize[HEADER]=10000;
  packetListRequestSize[HKEVENT]=10000;
  packetListRequestSize[SSHKEVENT]=10000;
  packetListRequestSize[MONITOR]=50000;
  packetListRequestSize[ADU5PAT]=50000;
  packetListRequestSize[ADU5VTG]=50000;
  packetListRequestSize[ADU5SAT]=50000;
  packetListRequestSize[G12POS]=50000;
  packetListRequestSize[G12SAT]=50000;
  packetListRequestSize[CMDECHO]=50000;

  // Could change with cmd line
  m_pControlDumpRF->Enable(false);

  m_pRawDataNext->Enable(false); 
  m_pRawDataPrev->Enable(false);
  
  m_pFlightDataNextRF->Enable(false);
  m_pFlightDataPrevRF->Enable(false);
  m_pFlightDataNextHK->Enable(false);
  m_pFlightDataPrevHK->Enable(false);
  m_pFlightDataNextGPS->Enable(false);
  m_pFlightDataPrevGPS->Enable(false);
  m_pFlightDataNextMonitor->Enable(false);
  m_pFlightDataPrevMonitor->Enable(false);
  m_pFlightDataNextCmd->Enable(false);
  m_pFlightDataPrevCmd->Enable(false);

  m_pDBDataDisconnect->Enable(false);
  m_pDBDataControl->Enable(false);
  m_pDBDataNext->Enable(false);
  m_pDBDataPrev->Enable(false);
  m_pDBDataOpenEvent->Enable(false);
  m_pDBDataWaveformAverage->Enable(false);
  m_pDBDataDBMRef->Enable(false);
  m_pDBDataConfigViewer->Enable(false);
  m_pDBDataTerminal->Enable(false);
  m_pDBDataClear->Enable(false);
  m_pDBDataStartIndex->Enable(false);
  m_pDBDataStopIndex->Enable(false);

  if(cmdLine->Found(_T("0")))
    zeroSCA=0;
  else
    zeroSCA=1;
  
  wxString tmp;
  if(cmdLine->Found(_T("o"),&tmp)){
    LoadOptions(tmp);
  }else{ // Try to load default options file 
    tmp=DEFAULT_OPTIONS_FILE;
    if(wxFileName::FileExists(tmp)) LoadOptions(tmp);
    else{
      tmp.Prepend(_T("/")).Prepend(wxFileName::GetHomeDir());
      if(wxFileName::FileExists(tmp)) LoadOptions(tmp);
    }
  }

  if(!cmdLine->Found(_T("d"),&tmp)) tmp=wxGetCwd();
  *currentRawFile=wxFileName::DirName(tmp);
  currentRawFile->MakeAbsolute();
  IndexRawDirectory();

  if(cmdLine->Found(_T("p"),&tmp)){
    (*pedestalFile)=tmp;
    ReadRawPedestals();
  }else if(cmdLine->Found(_T("P"),&tmp)){
    ReadPedestals(wxFileName(tmp));
  }

  if(cmdLine->Found(_T("t"),&tmp)){
    if(tmp.Last()!=wxFileName::GetPathSeparator()) tmp+=wxFileName::GetPathSeparator();
    packetDirectory[RFEVENT]=tmp+_T("event/");
    packetDirectory[HEADER]=tmp+_T("event/");
    packetDirectory[HKEVENT]=tmp+_T("house/hk/");
    packetDirectory[SSHKEVENT]=tmp+_T("house/hk/");
    packetDirectory[MONITOR]=tmp+_T("house/monitor/");
    packetDirectory[ADU5PAT]=tmp+_T("house/gps/adu5/pat/");
    packetDirectory[ADU5VTG]=tmp+_T("house/gps/adu5/vtg/");
    packetDirectory[ADU5SAT]=tmp+_T("house/gps/adu5/sat/");
    packetDirectory[G12POS]=tmp+_T("house/gps/g12/pos/");
    packetDirectory[G12SAT]=tmp+_T("house/gps/g12/sat/");
    packetDirectory[CMDECHO]=tmp+_T("house/cmd/");
    for(int i=0;i<N_PCKTYPE;++i){
      packetDirectory[i].MakeAbsolute();
      IndexPacketDirectory(i);
    }
  }

  // This currently only controls raw data reads...should make it multipurpose,
  // i.e. for flight data and DB reads as well
  if(cmdLine->Found(_T("a"),&m_readDelay)){
    if(m_rawEventLoadTimer->Start(m_readDelay*1000)){
      m_pRawDataStop->Enable(true); 
      m_pRawDataStart->Enable(false); 
    }
  }else{
    m_pRawDataStop->Enable(false); 
    m_pRawDataStart->Enable(true); 
    m_readDelay=AviewOpts::readDelay;
  }

  return;
}
  
AviewFrame::~AviewFrame(){
  m_rawEventLoadTimer->Stop();
  m_currentTimeTimer->Stop();
  m_updateTimer->Stop();
  m_eventRateTimer->Stop();
  m_retroIndexTimer->Stop();

  //  for(int i=0;i<CHANNELS_PER_SURF;++i)
  //  for(int j=0;j<ACTIVE_SURFS;++j)
  //    if(rfCurve[j][i]) delete rfCurve[j][i];

 delete wxLog::SetActiveTarget(m_logTarget);
 delete m_xmlHandler;
 delete m_dbHandler;
}

// wxEVT_COMMAND_MENU_SELECTED event handlers
void AviewFrame::OnMenuControlGPSData( wxCommandEvent &WXUNUSED(event) ){
  m_GPSDataInfo->Show(true);
}
void AviewFrame::OnMenuControlGPSSat( wxCommandEvent &WXUNUSED(event) ){
  m_GPSSatInfo->Show(true);
}
void AviewFrame::OnMenuControlTrigPattern( wxCommandEvent &WXUNUSED(event) ){
  m_trigPatternInfo->Show(true);
}
void AviewFrame::OnMenuControlConvertTime( wxCommandEvent &WXUNUSED(event) ){
  m_timeTranslator->Show(true);
}
void AviewFrame::OnMenuControlDumpRF( wxCommandEvent &WXUNUSED(event) ){
  static wxFileName lastSaveFile;
  wxFileDialog *dlg = new wxFileDialog(this,_T("Save RF data"),lastSaveFile.GetPath(),_T(""),_T("All files(*.*)|*.*"),wxSAVE);
  if (dlg->ShowModal() == wxID_OK){
    wxString tmp=dlg->GetPath();
    wxFile rfFile(tmp,wxFile::write);
    if(!rfFile.IsOpened()){
      wxString error;
      error.Printf(_T("Failed to open file %s for writing"),tmp.c_str());
      wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
      return;
    }
    lastSaveFile=wxFileName(tmp);
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF;++j){
	float data[MAX_NUMBER_SAMPLES];
	unsigned int k;
	for(k=0;k<MAX_NUMBER_SAMPLES;++k) data[k]=0;
	for(k=0;k<rfCurve[i][j]->GetLength();++k)
	  data[k]=(*rfCurve[i][j])[k].y;
	std::ostrstream str;
  	//str<<" surf="<<i<<" chan="<<j<<" getlength="<<rfCurve[i][j]->GetLength()<<"\n";
	for(k=0;k<MAX_NUMBER_SAMPLES;++k) {
		str<<data[k]<<" ";
	}
	str<<std::endl;
	str<<std::ends;
	if(rfFile.Write(str.str(),strlen(str.str()))!=
	   strlen(str.str())){
	  wxMessageBox(_T("Error while saving RF data!"), 
		       _T("File writing error"), wxOK|wxICON_ERROR);
	  rfFile.Close();
	  return;
	}
      }
    rfFile.Close();
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuControlClearDBMRef( wxCommandEvent &WXUNUSED(event) ){
  for(int i=0;i<ACTIVE_SURFS-1;++i)
    for(int j=0;j<CHANNELS_PER_SURF;++j)
      rfDisplay[i][j]->SetDBMRef(NO_DBM_REF);
  
  SetStatusText(_T("Cleared dBm references"),1);
}

void AviewFrame::OnMenuControlClearPowerRef( wxCommandEvent &WXUNUSED(event) ){
  for(int i=0;i<ACTIVE_SURFS-1;++i)
    for(int j=0;j<CHANNELS_PER_SURF;++j)
      rfDisplay[i][j]->ClearDBMRef();
  
  SetStatusText(_T("Cleared RF power references"),1);
}

void AviewFrame::OnMenuControlOptions( wxCommandEvent &WXUNUSED(event) ){
  AviewOpts::RFOverviewType OldRFOverview=AviewOpts::RFOverview;
  AviewOpts::RFReferenceType OldRFReference=AviewOpts::RFReference;

  if(m_OptionsDialog->ShowModal()==wxID_OK){
    // Harvest new settings from options dialogs
    m_OptionsDialog->ExportOptions();
    SetOptions();
    if(OldRFOverview!=AviewOpts::RFOverview) SwitchRFView(true);
    if(OldRFReference!=AviewOpts::RFReference) SwitchRFReference();
    // Redraw everything
    Refresh(true);
  }else{  // Reset all values to existing ones
    m_OptionsDialog->ImportOptions();
  }
}

void AviewFrame::OnMenuControlSaveOptions( wxCommandEvent &WXUNUSED(event) ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("default.arc"),_T("Aview options(*.arc)|*.arc"),wxSAVE);
  if (dlg->ShowModal() == wxID_OK) SaveOptions(dlg->GetPath());
  dlg->Destroy();
}

void AviewFrame::OnMenuControlLoadOptions( wxCommandEvent &WXUNUSED(event) ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Load options file"),_T(""),_T(""),_T("Aview options(*.arc)|*.arc|All Files|*.*"),wxOPEN);
  if (dlg->ShowModal() == wxID_OK) 
    if(LoadOptions(dlg->GetPath()))
      // Redraw everything
      Refresh(true);
  dlg->Destroy();
}

////////////////////june 30 2008 /////////////////amir's addition
 void AviewFrame::OnMenuG12PosViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
  m_gpsposViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuG12SatViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_g12satViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuADU5PatViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_adu5patViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuADU5SatViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_adu5satViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuADU5VtgViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_adu5vtgViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuHeaderViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_headerViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuMonitorViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_monitorViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuHKViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_hkViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuHKCalViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_hkcalViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuCommandViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_commandViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuDBHKViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_dbhkViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuDBHeaderViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_dbheaderViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuDBMonitorViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_dbmonitorViewer->Show(true);
 }
// ///////////////////////////////////////////////////////
 void AviewFrame::OnMenuDBCmdViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
//   // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
   m_dbcmdViewer->Show(true);
 }
///////////////////////////////////////////////////////
void AviewFrame::OnMenuControlQuit( wxCommandEvent &WXUNUSED(event) ){
  Close(true);
}
void AviewFrame::OnMenuRawDataOpen( wxCommandEvent &WXUNUSED(event) ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open raw data file"),currentRawFile->GetPath(),_T(""),_T("SURF files(surf_data.*)|surf_data.*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    *currentRawFile=dlg->GetPath();
    // Load file here
    if(currentRawFile->IsOk() && ReadRawData()){
      m_pControlDumpRF->Enable(true);
      Refresh(true);
      IndexRawDirectory();
    }else{
      m_pRawDataNext->Enable(false);
      m_pRawDataPrev->Enable(false);
    }
  }
  dlg->Destroy();

}

void AviewFrame::OnMenuRawDataNext( wxCommandEvent &WXUNUSED(event) ){
  int n=rawFileList->Index(currentRawFile->GetFullPath().c_str());
  // Relly on wxNOT_FOUND==-1 to read first file
  if(((n==wxNOT_FOUND)&&rawFileList->GetCount()>0) 
     || ((unsigned int)n<rawFileList->GetCount()-1)){
    *currentRawFile=(*rawFileList)[n+1];
    if(currentRawFile->IsOk() && ReadRawData()) 
      Refresh(true);
  }
}
void AviewFrame::OnMenuRawDataPrev( wxCommandEvent &WXUNUSED(event) ){
  int n=rawFileList->Index(currentRawFile->GetFullPath().c_str());
  if((n!=wxNOT_FOUND) && (n>0)){
    *currentRawFile=(*rawFileList)[n-1];
    if(currentRawFile->IsOk() && ReadRawData())
      Refresh(true);
  }
}

void AviewFrame::OnMenuRawDataStart( wxCommandEvent &WXUNUSED(event) ){
  if(m_rawEventLoadTimer->Start(m_readDelay*1000)){  
    m_pRawDataStart->Enable(false); 
    m_pRawDataStop->Enable(true); 
  }
}
void AviewFrame::OnMenuRawDataStop( wxCommandEvent &WXUNUSED(event) ){
  m_rawEventLoadTimer->Stop();
  m_pRawDataStart->Enable(true); 
  m_pRawDataStop->Enable(false); 
}

void AviewFrame::OnMenuRawDataPedestal( wxCommandEvent &WXUNUSED(event) ){
  wxString dir;
  if(pedestalFile->IsOk()) dir << pedestalFile->GetPath();
  else dir << wxGetCwd();
  wxFileDialog *dlg = new wxFileDialog(this,_T("Load pedestal data file"),dir,_T(""),_T("Data file(*.dat)|*.dat|All files(*.*)|*.*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    *pedestalFile=dlg->GetPath();
    // Load file here
    if(ReadRawPedestals()){
      // On success, re-read current data file
      if(currentRawFile->IsOk() && currentRawFile->HasName() && ReadRawData())
	Refresh(true);
    }
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataOpenControl( wxCommandEvent &WXUNUSED(event)){
  m_dbControl->StopTimers();
  m_dbControl->Show(false);
  m_flightControl->Show();
}

void AviewFrame::OnMenuFlightDataOpenRF( wxCommandEvent &WXUNUSED(event) ){
  wxString tmp;
  tmp.Printf(_T("Header files(%s)|%s"),packetFilter[RFEVENT].c_str(),packetFilter[RFEVENT].c_str());
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open event header data file"),packetDirectory[RFEVENT].GetPath(),_T(""),tmp,wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if(ReadRFEvent(file)){
      m_pControlDumpRF->Enable(true);
      Refresh(true);
      packetDirectory[RFEVENT].SetPath(file.GetPath());
      packetDirectory[HEADER].SetPath(file.GetPath());
      IndexPacketDirectory(RFEVENT);
      IndexPacketDirectory(HEADER);
      int n=packetFileList[RFEVENT]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND){
	m_flightControl->SetIndex(RFEVENT,n);
	m_flightControl->SetIndex(HEADER,n);
      }
    }else{
      m_pFlightDataNextRF->Enable(false);
      m_pFlightDataPrevRF->Enable(false);
    }
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataNextRF( wxCommandEvent &event ){
  m_flightControl->Next(RFEVENT,event);
}
void AviewFrame::OnMenuFlightDataPrevRF( wxCommandEvent &event ){
  m_flightControl->Prev(RFEVENT,event);
}

void AviewFrame::OnMenuFlightDataAverageWV( wxCommandEvent &WXUNUSED(event) ){
  static wxFileName lastWvFile;
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open event waveform file"),lastWvFile.GetPath(),_T(""),_T("Waveform files(*ev_*.dat*)|*ev_*.dat*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxString tmp=dlg->GetPath();
    if(AverageWaveformFile(wxFileName(tmp))){
      lastWvFile=wxFileName(tmp);
      Refresh(true);
    }
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataPedestal( wxCommandEvent &WXUNUSED(event) ){
  static wxFileName lastPedFile;
  wxFileDialog *dlg = new wxFileDialog(this,_T("Load pedestal data file"),lastPedFile.GetPath(),_T(""),_T("Pedestal file(peds_*.dat)|peds_*.dat"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxString tmp=dlg->GetPath();
    // Load file here
    if(ReadPedestals(wxFileName(tmp))) lastPedFile=wxFileName(tmp);
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataDBMRef( wxCommandEvent &WXUNUSED(event) ){
  static wxFileName lastSurfHKFile;
  wxFileDialog *dlg = new wxFileDialog(this,_T("Load SURF HK file"),lastSurfHKFile.GetPath(),_T(""),_T("SURF HK files(surfhk_*.dat*)|surfhk_*.dat*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxString tmp=dlg->GetPath();
    // Load file here
    if(ReadDBMRef(wxFileName(tmp))) lastSurfHKFile=wxFileName(tmp);
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataCalibHK( wxCommandEvent &WXUNUSED(event) ){
  wxString dir;
  if(packetDirectory[HKEVENT].IsOk()) dir << packetDirectory[HKEVENT].GetPath();
  else dir << wxGetCwd();
  wxFileDialog *dlg = new wxFileDialog(this,_T("Load HK calibration file"),dir,_T(""),_T("HK CAL/AVZ file(hk_cal_avz*.dat*)|hk_cal_avz*.dat*|All files(*.*)|*.*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    ReadHKCalibPacket(file);
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataOpenHK( wxCommandEvent &WXUNUSED(event) ){
  wxString tmp;
  tmp.Printf(_T("Housekeeping files(%s)|%s"),packetFilter[HKEVENT].c_str(),packetFilter[HKEVENT].c_str());
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open event header data file"),packetDirectory[HKEVENT].GetPath(),_T(""),tmp,wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if(ReadHKPacket(file)){
      Refresh(true);
      packetDirectory[HKEVENT].SetPath(file.GetPath());
      IndexPacketDirectory(HKEVENT);
      int n=packetFileList[HKEVENT]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(HKEVENT,n);
    }else{
      m_pFlightDataNextHK->Enable(false);
      m_pFlightDataPrevHK->Enable(false);
    }
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataNextHK( wxCommandEvent &event ){
  m_flightControl->Next(HKEVENT,event);
}

void AviewFrame::OnMenuFlightDataPrevHK( wxCommandEvent &event ){
  m_flightControl->Prev(HKEVENT,event);
}

void AviewFrame::OnMenuFlightDataOpenGPS( wxCommandEvent &WXUNUSED(event) ){
  int type=(AviewOpts::DefaultGPS==AviewOpts::ADU5?ADU5PAT:G12POS);
  wxString tmp;
  tmp.Printf(_T("GPS files(%s)|%s"),packetFilter[type].c_str(),packetFilter[type].c_str());
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open GPS data file"),packetDirectory[type].GetPath(),_T(""),tmp,wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if((type==ADU5PAT && ReadADU5PatPacket(file)) ||
       (type==G12POS && ReadG12PosPacket(file))){
      Refresh(true);
      packetDirectory[type].SetPath(file.GetPath());
      IndexPacketDirectory(type);
      int n=packetFileList[type]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(type,n);
      // Index all other GPS packets
      wxString dir=packetDirectory[type].GetPath();
      if(type==ADU5PAT){
	dir.Replace(_T("pat"),_T("vtg"));
	packetDirectory[ADU5VTG].SetPath(dir);
	dir.Replace(_T("vtg"),_T("sat"));
	packetDirectory[ADU5SAT].SetPath(dir);
	dir.Replace(_T("adu5a"),_T("g12"));
	dir.Replace(_T("adu5b"),_T("g12"));
	packetDirectory[G12SAT].SetPath(dir);
	dir.Replace(_T("sat"),_T("pos"));
	packetDirectory[G12POS].SetPath(dir);
	IndexPacketDirectory(G12POS);
	IndexPacketDirectory(G12SAT);
	IndexPacketDirectory(ADU5VTG);
	IndexPacketDirectory(ADU5SAT);
      }else{
	dir.Replace(_T("pos"),_T("sat"));
	packetDirectory[G12SAT].SetPath(dir);
	dir.Replace(_T("g12"),_T("adu5"));
	packetDirectory[ADU5SAT].SetPath(dir);
	dir.Replace(_T("sat"),_T("pat"));
	packetDirectory[ADU5PAT].SetPath(dir);
	dir.Replace(_T("pat"),_T("vtg"));
	packetDirectory[ADU5VTG].SetPath(dir);
	IndexPacketDirectory(G12SAT);
	IndexPacketDirectory(ADU5PAT);
	IndexPacketDirectory(ADU5VTG);
	IndexPacketDirectory(ADU5SAT);
      }
    }else{
      m_pFlightDataNextGPS->Enable(false);
      m_pFlightDataPrevGPS->Enable(false);
    }
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataNextGPS( wxCommandEvent &event ){
  if(AviewOpts::DefaultGPS==AviewOpts::ADU5) m_flightControl->Next(ADU5PAT,event);
  else m_flightControl->Next(G12POS,event);
}
void AviewFrame::OnMenuFlightDataPrevGPS( wxCommandEvent &event ){
  if(AviewOpts::DefaultGPS==AviewOpts::ADU5) m_flightControl->Prev(ADU5PAT,event);
  else m_flightControl->Prev(G12POS,event);
}

void AviewFrame::OnMenuFlightDataOpenMonitor( wxCommandEvent &WXUNUSED(event)){
  wxString tmp;
  tmp.Printf(_T("CPU monitor files(%s)|%s"),packetFilter[MONITOR].c_str(),packetFilter[MONITOR].c_str());
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open CPU monitor data file"),packetDirectory[MONITOR].GetPath(),_T(""),tmp,wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if(ReadMonitorPacket(file)){
      Refresh(true);
      packetDirectory[MONITOR].SetPath(file.GetPath());
      IndexPacketDirectory(MONITOR);
      int n=packetFileList[MONITOR]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(MONITOR,n);
    }else{
      m_pFlightDataNextMonitor->Enable(false);
      m_pFlightDataPrevMonitor->Enable(false);
    }
  }
  dlg->Destroy();
}

void AviewFrame::OnMenuFlightDataNextMonitor(wxCommandEvent &event){
  m_flightControl->Next(MONITOR,event);
}
void AviewFrame::OnMenuFlightDataPrevMonitor(wxCommandEvent &event){
  m_flightControl->Prev(MONITOR,event);
} 

void AviewFrame::OnMenuFlightDataOpenCmd( wxCommandEvent &WXUNUSED(event)){
  wxString tmp;
  tmp.Printf(_T("Command echo files(%s)|%s"),packetFilter[CMDECHO].c_str(),packetFilter[CMDECHO].c_str());
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open CPU monitor data file"),packetDirectory[CMDECHO].GetPath(),_T(""),tmp,wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if(ReadCmdEchoPacket(file)){
      Refresh(true);
      packetDirectory[CMDECHO].SetPath(file.GetPath());
      IndexPacketDirectory(CMDECHO);
      int n=packetFileList[CMDECHO]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(CMDECHO,n);
    }else{
      m_pFlightDataNextCmd->Enable(false);
      m_pFlightDataPrevCmd->Enable(false);
    }
  }
  dlg->Destroy();
}
void AviewFrame::OnMenuFlightDataNextCmd(wxCommandEvent &event){
  m_flightControl->Next(CMDECHO,event);
}
void AviewFrame::OnMenuFlightDataPrevCmd(wxCommandEvent &event){
  m_flightControl->Prev(CMDECHO,event);
}

void AviewFrame::OnMenuDBDataConnect( wxCommandEvent &WXUNUSED(event) ){
  if(m_dbHandler->Connect(AviewOpts::dbHost,AviewOpts::dbUser,
			  AviewOpts::dbPwd,AviewOpts::dbName)){
    wxTimerEvent e;
    CheckDBConnection(e);
  }
}

void AviewFrame::OnMenuDBDataDisconnect( wxCommandEvent &WXUNUSED(event) ){
  m_dbHandler->Disconnect();
  m_pDBDataConnect->Enable(true);
  m_pDBDataDisconnect->Enable(false);
  m_pDBDataControl->Enable(false);
  m_pDBDataNext->Enable(false);
  m_pDBDataPrev->Enable(false);
  m_pDBDataOpenEvent->Enable(false);
  m_pDBDataWaveformAverage->Enable(false);
  m_pDBDataDBMRef->Enable(false);
  m_pDBDataConfigViewer->Enable(false);
  m_pDBDataTerminal->Enable(false);
  m_pDBDataClear->Enable(false);
  m_pDBDataStartIndex->Enable(false);
  m_pDBDataStopIndex->Enable(false);
  m_dbTerminal->Show(false);
  m_configViewer->Show(false);
  m_eventRateTimer->Stop();
  m_retroIndexTimer->Stop();
  SetStatusText(_T("Disconnected from database"),2);
}

void AviewFrame::OnMenuDBDataControl( wxCommandEvent &WXUNUSED(event) ){
  m_flightControl->StopTimers();
  m_flightControl->Show(false);
  m_dbControl->Show();
}
void AviewFrame::OnMenuDBDataNext( wxCommandEvent &event ){
  //m_dbControl->Next(HEADER,event);
  m_dbControl->Next(RFEVENT,event);
  m_dbControl->Next(HKEVENT,event);
  m_dbControl->Next(SSHKEVENT,event);
  m_dbControl->Next(MONITOR,event);
  m_dbControl->Next(ADU5PAT,event);
  m_dbControl->Next(ADU5VTG,event);
  m_dbControl->Next(ADU5SAT,event);
  m_dbControl->Next(G12POS,event);
  m_dbControl->Next(G12SAT,event);
  m_dbControl->Next(CMDECHO,event);
}
void AviewFrame::OnMenuDBDataPrev( wxCommandEvent &event ){
  //m_dbControl->Prev(HEADER,event);
  m_dbControl->Prev(RFEVENT,event);
  m_dbControl->Prev(HKEVENT,event);
  m_dbControl->Prev(SSHKEVENT,event);
  m_dbControl->Prev(MONITOR,event);
  m_dbControl->Prev(ADU5PAT,event);
  m_dbControl->Prev(ADU5VTG,event);
  m_dbControl->Prev(ADU5SAT,event);
  m_dbControl->Prev(G12POS,event);
  m_dbControl->Prev(G12SAT,event);
  m_dbControl->Prev(CMDECHO,event);
}
void AviewFrame::OnMenuDBDataLast( wxCommandEvent &event ){
  //m_dbControl->Last(HEADER,event);
  m_dbControl->Last(RFEVENT,event);
  m_dbControl->Last(HKEVENT,event);
  m_dbControl->Last(SSHKEVENT,event);
  m_dbControl->Last(MONITOR,event);
  m_dbControl->Last(ADU5PAT,event);
  m_dbControl->Last(ADU5VTG,event);
  m_dbControl->Last(ADU5SAT,event);
  m_dbControl->Last(G12POS,event);
  m_dbControl->Last(G12SAT,event);
  m_dbControl->Last(CMDECHO,event);
}
void AviewFrame::OnMenuDBDataAuto( wxCommandEvent& event ){
  m_dbControl->InverseTimers();
}
void AviewFrame::OnMenuDBDataOpenEvent( wxCommandEvent &event ){
  // Launch dialog to get event number 
  wxTextEntryDialog *dlg= new wxTextEntryDialog(this,_T("Enter event number"),_T("Enter event number"),_T("1"));
  if(dlg->ShowModal()==wxID_OK){
    long val;
    if(dlg->GetValue().ToLong(&val) && val>=0){
      // Send SQL request
      sqlRequestType.Add(RFEVENT);
      updateControlPanelIndex=true;
      wxString cmd;
      cmd.Printf(_T("SELECT * FROM %s WHERE evnum=%ld;"),packetTable[RFEVENT].c_str(),val);
      wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
      sqlEvent.SetEventObject( this );
      sqlEvent.SetString(cmd);
      sqlEvent.SetClientData((void*)parseWrapper);
      ProcessEvent(sqlEvent);
    }else
      wxMessageBox(_T("Enter valid positive integer"),_T("Invalid event number"),wxOK|wxICON_ERROR);
  }
}

void AviewFrame::OnMenuDBDataWaveformAverage(wxCommandEvent &WXUNUSED(event) ){
  // Create data entry dialog
  wxDialog *dlg=new wxDialog(this,wxID_ANY,wxString(_T("Enter waveform parameters")));
  
  wxBoxSizer *topSizerAvg=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *entrySizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  entrySizer->Add(labelSizer,0,wxEXPAND);
  entrySizer->Add(infoSizer,1,wxEXPAND);
  topSizerAvg->Add(entrySizer,0,wxEXPAND);

  wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
  buttons->AddButton(new wxButton(dlg,wxID_OK,_("OK")));
  buttons->AddButton(new wxButton(dlg,wxID_CANCEL,_("Cancel")));
  buttons->Realize();
  topSizerAvg->Add(buttons,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(dlg,wxID_ANY,_T("Reference UTC: ")), 1);
  wxUTCDateCtrl *refTimeEntry = 
    new wxUTCDateCtrl(dlg,wxID_ANY,wxDateTime::Now().ToUTC().Format(_T("%Y-%m-%d %H:%M:%S")),
		      wxDefaultPosition,wxSize(145,-1));
  infoSizer->Add(refTimeEntry,0,wxEXPAND);
    
  labelSizer->Add(new wxStaticText(dlg,wxID_ANY,_T("Length [s]: ")), 1);
  wxTextCtrl *lengthEntry = new wxTextCtrl(dlg,wxID_ANY,_T("-100"),wxDefaultPosition,wxDefaultSize);
  infoSizer->Add(lengthEntry,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(dlg,wxID_ANY,_T("SQL Filter: ")), 1);
  wxTextCtrl *sqlFilterEntry = new wxTextCtrl(dlg,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize);
  infoSizer->Add(sqlFilterEntry,0,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(dlg,wxID_ANY,_T("Number of waveforms: ")), 1);
  wxSpinCtrl *numberEntry = new wxSpinCtrl(dlg,wxID_ANY);
  numberEntry->SetRange(1,MAX_DB_WVAVG);
  numberEntry->SetValue(10);
  infoSizer->Add(numberEntry,0,wxEXPAND);
  
  dlg->SetSizerAndFit(topSizerAvg);
  topSizerAvg->SetSizeHints(dlg);

  // Show dialog and process input
  if(dlg->ShowModal() == wxID_OK){
    long refTick=refTimeEntry->GetTicks();
    if(refTick<=0) return;
    long length;
    if(!lengthEntry->GetLineText(0).ToLong(&length)){
      wxMessageBox(_T("Unable to parse record length. Please use valid integer value."),_T("SQL Request Error"),wxOK|wxICON_ERROR);
      return;
    }

    wxString sqlFilter=sqlFilterEntry->GetLineText(0);
    if(sqlFilter.Length()) sqlFilter.Prepend(_T(" AND "));
    
    unsigned int num=numberEntry->GetValue();

    // Prepare SQL command
    unsigned int startTime,endTime;
    if(length>0){
      startTime=refTick;
      endTime=startTime+length;
    }else{
      endTime=refTick;
      startTime=endTime+length;
    }
    wxString cmd;
    cmd.Printf(_T("SELECT evnum FROM rf WHERE time>=%u AND time<=%u %s ORDER BY time %s LIMIT %u;"),
	       startTime,endTime,sqlFilter.c_str(),length>0?_T(""):_T("DESC"),num);
    sqlRequestType.Add(WVAVG);
    wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
    sqlEvent.SetEventObject( this );
    sqlEvent.SetString(cmd);
    sqlEvent.SetClientData((void*)parseWrapper);
    ProcessEvent(sqlEvent);
  }
  delete dlg;
  return;
}

void AviewFrame::OnMenuDBDataDBMRef(wxCommandEvent &WXUNUSED(event) ){
  // Create data entry dialog
  wxDialog *dlg=new wxDialog(this,wxID_ANY,_T("Enter DBM reference time interval"),wxDefaultPosition,wxSize(200,-1));
  
  wxBoxSizer *topSizerRef=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *entrySizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  entrySizer->Add(labelSizer,0,wxEXPAND);
  entrySizer->Add(infoSizer,1,wxEXPAND);
  topSizerRef->Add(entrySizer,0,wxEXPAND);

  wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
  buttons->AddButton(new wxButton(dlg,wxID_OK,_("OK")));
  buttons->AddButton(new wxButton(dlg,wxID_CANCEL,_("Cancel")));
  buttons->Realize();
  topSizerRef->Add(buttons,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(dlg,wxID_ANY,_T("Reference UTC: ")), 1);
  wxUTCDateCtrl *refTimeEntry = 
    new wxUTCDateCtrl(dlg,wxID_ANY,wxDateTime::Now().ToUTC().Format(_T("%Y-%m-%d %H:%M:%S")),
		      wxDefaultPosition,wxDefaultSize);
  infoSizer->Add(refTimeEntry,0,wxEXPAND);
    
  labelSizer->Add(new wxStaticText(dlg,wxID_ANY,_T("Length [s]: ")), 1);
  wxTextCtrl *lengthEntry = new wxTextCtrl(dlg,wxID_ANY,_T("-100"),wxDefaultPosition,wxDefaultSize);
  infoSizer->Add(lengthEntry,0,wxEXPAND);
  
  dlg->SetSizerAndFit(topSizerRef);
  topSizerRef->SetSizeHints(dlg);

  // Show dialog and process input
  if(dlg->ShowModal() == wxID_OK){
    long refTick=refTimeEntry->GetTicks();
    if(refTick<=0) return;
    long length;
    if(!lengthEntry->GetLineText(0).ToLong(&length)){
      wxMessageBox(_T("Unable to parse record length. Please use valid integer value."),_T("SQL Request Error"),wxOK|wxICON_ERROR);
      return;
    }

    // Prepare SQL command
    unsigned int startTime,endTime;
    if(length>0){
      startTime=refTick;
      endTime=startTime+length;
    }else{
      endTime=refTick;
      startTime=endTime+length;
    }
    wxString cmd;
    cmd.Printf(_T("SELECT rfpow FROM hk_surf WHERE time>=%u AND time<=%u;"),
	       startTime,endTime);
    sqlRequestType.Add(DBMREF);
    wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
    sqlEvent.SetEventObject( this );
    sqlEvent.SetString(cmd);
    sqlEvent.SetClientData((void*)parseWrapper);
    ProcessEvent(sqlEvent);
  }
  delete dlg;
  return;
}
void AviewFrame::OnMenuDBDataConfigViewer(wxCommandEvent &WXUNUSED(event) ){
  m_configViewer->Show(true);
}
void AviewFrame::OnMenuDBDataTerminal(wxCommandEvent &WXUNUSED(event) ){
  m_dbTerminal->Show(true);
}
void AviewFrame::OnMenuDBDataClear(wxCommandEvent &WXUNUSED(event) ){
  m_dbHandler->ClearSQLQueue();
  sqlRequestType.Clear();
  if(m_dbHandler->Reconnect()){
    wxString status;
    status.Printf(_T("Reconnected to %s"),AviewOpts::dbName.c_str());
    SetStatusText(status,2);
    SetStatusText(m_dbHandler->getHost().c_str(),0); //show host IP. peng
  }else{
    m_pDBDataConnect->Enable(true);
    m_pDBDataDisconnect->Enable(false);
    m_pDBDataControl->Enable(false);
    m_pDBDataNext->Enable(false);
    m_pDBDataPrev->Enable(false);
    m_pDBDataOpenEvent->Enable(false);
    m_pDBDataWaveformAverage->Enable(false);
    m_pDBDataDBMRef->Enable(false);
    m_pDBDataConfigViewer->Enable(false);
    m_pDBDataTerminal->Enable(false);
    m_pDBDataClear->Enable(false);
    m_pDBDataStartIndex->Enable(false);
    m_pDBDataStopIndex->Enable(false);
    m_eventRateTimer->Stop();
    m_retroIndexTimer->Stop();
    wxString status;
    status.Printf(_T("Disconnected from database"));
    SetStatusText(status,2);
    wxLogError(_T("[AviewFrame::OnMenuDBDataClear] Failed to reset database connection"));
  }
}

void AviewFrame::OnMenuDBDataStartIndex(wxCommandEvent &WXUNUSED(event) ){
  wxMessageDialog *dlg=new wxMessageDialog(this,_T("Indexing complete database might take long time and can adversly affect GUI and database performance"),_T("Full DB Indexing"),wxOK|wxCANCEL);
  if(dlg->ShowModal()==wxID_OK){
    m_retroIndexTimer->Start(0,true);
    m_pDBDataStartIndex->Enable(false);
    m_pDBDataStopIndex->Enable(true);
  }
  delete dlg;
}
void AviewFrame::OnMenuDBDataStopIndex(wxCommandEvent &WXUNUSED(event) ){
  m_retroIndexTimer->Stop();
  m_pDBDataStartIndex->Enable(true);
  m_pDBDataStopIndex->Enable(false);
}

void AviewFrame::OnMenuHelpAbout( wxCommandEvent &WXUNUSED(event) ){
printf("about1\n");
  wxMessageBox(_T("ANITA Event Viewer\nsvnversion: ddm_placeholder_for_svnversion\n(c) Predrag Miocinovic 2006"),_T("About"),wxOK);
printf("about2\n");
}

void AviewFrame::OnLoadEventTimer( wxTimerEvent &WXUNUSED(event) ){
  // For now hardcode to raw event
  wxCommandEvent tmp;
  this->OnMenuRawDataNext(tmp);
}
void AviewFrame::OnCurrentTimeTimer( wxTimerEvent &WXUNUSED(event) ){
  m_HeaderInfo->UpdateCurrentTime();
}
void AviewFrame::OnEventRateTimer( wxTimerEvent &WXUNUSED(event) ){
  time_t now=time(NULL);
  if(now-lastSQLRequest<SQL_IDLE_LIMIT){
    sqlRequestType.Add(EVENTRATE);
    // Prepare SQL request
    wxString cmd;
    cmd.Printf(_T("SELECT eventrate(%u);"),AviewOpts::EventRateStep);
    wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
    sqlEvent.SetEventObject( this );
    sqlEvent.SetString(cmd);
    sqlEvent.SetClientData((void*)parseWrapper);
    ProcessEvent(sqlEvent);
  }
}

void AviewFrame::OnRetroIndexTimer(wxTimerEvent &WXUNUSED(event) ){
  if(initialDBIndexing) return;
  showProgress=false;
  int stillNeeded=0;
  for(int i=0;i<N_PCKTYPE;++i){
    if(needsRetroIndex[i]){
      IndexDBPackets(i,false,true);
      ++stillNeeded;
    }
  }
  if(!stillNeeded)
    wxMessageBox(_T("Database fully indexed!"),_T("Indexing completed"),wxOK);
  else        
    m_retroIndexTimer->Start(RETRO_TIMER_STEP*1000,true);
}

void AviewFrame::OnAntennaSelect( wxAntennaEvent &event ){
  int surf,chan;
  event.GetRFChannel(&surf,&chan);
  if(surf<ACTIVE_SURFS && chan<CHANNELS_PER_SURF && surf>=0 && chan>=0) 
    rfDisplay[surf][chan]->Show(true);//seems not working at all.  peng
}

void AviewFrame::OnUpdateTimer( wxTimerEvent &WXUNUSED(event) ){
  static int task=0;
  static int ten=0;
  time_t now=time(NULL);
  if(m_dbHandler->IsConnected() && !initialDBIndexing && (now-lastSQLRequest)<SQL_IDLE_LIMIT && ++ten==10){
    ReceiveNotifications();
    ten=0;
  }
  // Update various open directories
  switch(task){
  case RFEVENT: 
  case HEADER: 
  case HKEVENT:
  case SSHKEVENT:
  case MONITOR:
  case ADU5PAT:
  case ADU5VTG:
  case ADU5SAT:
  case G12POS:
  case G12SAT:
  case CMDECHO:
    IndexPacketDirectory(task,false);
    if(m_dbHandler->IsConnected() && needsIndex[task]){
      showProgress=false;
      IndexDBPackets(task,false,false);
      needsIndex[task]=false;
    }
    break;
  case (CMDECHO+1): // Update raw directory
    IndexRawDirectory(false);
    break;
  default:
    task=-1;
    break;
  }
  ++task;
};

void AviewFrame::ReadFlightData(wxCommandEvent &event){
  wxFileName file=event.GetString();
  int order=(int)event.GetClientData();
  switch(event.GetId()-CONTROLWIN_OFFSET){
  case RFEVENT: 
    if(!ReadRFEvent(file,order,true)) m_flightControl->StopTimers(RFEVENT); 
    Refresh(true);
    break;
  case HEADER: 
    if(!ReadRFEvent(file,order,false)) m_flightControl->StopTimers(HEADER); 
    Refresh(true);
    break;
  case HKEVENT: 
    if(!ReadHKPacket(file,order)) m_flightControl->StopTimers(HKEVENT); 
    Refresh(true);
    break;
  case SSHKEVENT: 
    if(!ReadHKPacket(file,order)) m_flightControl->StopTimers(SSHKEVENT); 
    Refresh(true);
    break;
  case MONITOR: 
    if(!ReadMonitorPacket(file,order)) m_flightControl->StopTimers(MONITOR); 
    Refresh(true);
    break;
  case ADU5PAT: 
    if(!ReadADU5PatPacket(file,order)) m_flightControl->StopTimers(ADU5PAT); 
    Refresh(true);
    break;
  case ADU5VTG:
    if(!ReadADU5VtgPacket(file,order)) m_flightControl->StopTimers(ADU5VTG); 
    Refresh(true);
    break;
  case ADU5SAT: 
    if(!ReadADU5SatPacket(file,order)) m_flightControl->StopTimers(ADU5SAT); 
    break;
  case G12POS: 
    if(!ReadG12PosPacket(file,order)) m_flightControl->StopTimers(G12POS); 
    Refresh(true);
    break;
  case G12SAT: 
    if(!ReadG12SatPacket(file,order)) m_flightControl->StopTimers(G12SAT); 
    break;
  case CMDECHO: 
    if(!ReadCmdEchoPacket(file,order)) m_flightControl->StopTimers(CMDECHO); 
    Refresh(true);
    break;
  default:
    wxMessageBox(_T("Received request for loading of unknown packet file"),_T("Packet file request error"),wxOK|wxICON_ERROR);
  }
}

void AviewFrame::RequestPacket(wxCommandEvent &event){
  lastSQLRequest=time(NULL);
  int type=event.GetId()-CONTROLWIN_OFFSET;
  if(type<0 || type>N_PCKTYPE-1){
    wxMessageBox(_T("Received request of unknown packet file"),_T("Packet DB request error"),wxOK|wxICON_ERROR);
    return;
  }
  sqlRequestType.Add(type);
  wxString nbufString=event.GetString();
  // Prepare SQL request
  wxString cmd;
  if((type == RFEVENT || type == HEADER) && AviewOpts::fetchRunNumber )
    //    cmd.Printf(_T("select a.*,b.runnum from %s a,other b where (a.nbuf=%s AND a.evnum>=b.runevnum AND a.time >= b.runtime%s) ORDER by runnum LIMIT 1;"),
    cmd.Printf(_T("select a.* from %s a where (a.nbuf=%s) ORDER by evid LIMIT 1;"),
	       packetTable[type].c_str(),nbufString.c_str() );
	       //	       AviewOpts::crcCheck?_T(" AND b.crc=257"):_T(""));
  else 
    cmd.Printf(_T("SELECT * FROM %s WHERE nbuf=%s;"),
	       packetTable[type].c_str(),nbufString.c_str());
  //  printf("(AviewFrame)cmd: %s\n",(const char*)cmd.mb_str()) ;

  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
}

void AviewFrame::NoResultsMessage( wxCommandEvent &event ){
//void AviewFrame::NoResultsMessage( wxCommandEvent &WXUNUSED(event) ){

	  wxMessageBox(_T("Fewer than 2 point resulted from the SQL request. Graph will not be updated."),_T("Insufficient SQL Result"),wxOK);
	}

void AviewFrame::OnSQLRequest(wxCommandEvent &event){
  wxObject *requestor=event.GetEventObject();
  sqlparser parser=(sqlparser)event.GetClientData();
  wxString sql=event.GetString();
  if(AviewOpts::sqlDump) wxLogMessage(_T("[AviewFrame::OnSQLRequest] %s"),sql.c_str());
  if(!m_dbHandler->SendQuery(sql,requestor,parser)) sqlRequestType.Empty();
}
  
void AviewFrame::CheckDBConnection( wxTimerEvent &WXUNUSED(event) ){
  bool status=m_dbHandler->FinalizeConnection();
  if(status){
    m_pDBDataConnect->Enable(false);
    m_pDBDataDisconnect->Enable(true);
    m_pDBDataControl->Enable(true);
    m_pDBDataNext->Enable(true);
    m_pDBDataPrev->Enable(true);
    m_pDBDataOpenEvent->Enable(true);
    m_pDBDataWaveformAverage->Enable(true);
    m_pDBDataDBMRef->Enable(true);
    m_pDBDataConfigViewer->Enable(true);
    m_pDBDataTerminal->Enable(true);
    m_pDBDataClear->Enable(true);
    m_pDBDataStartIndex->Enable(true);
    m_eventRateTimer->Start(AviewOpts::EventRateStep*1000);
    wxString status;
    status.Printf(_T("Connected to %s"),m_dbHandler->GetDBName().c_str());
    SetStatusText(status,2);
    SetStatusText(m_dbHandler->getHost().c_str(),0); //show host IP. peng
    lastSQLRequest=time(NULL);
    // Setup notification requests & index DB tables
    showProgress=true;
    for(int i=0;i<N_PCKTYPE;++i){
      SendListen(i);
      IndexDBPackets(i,true,false);
      needsIndex[i]=false;
    }
    SetStatusText(_T("DB indexing 0% complete"),1);
    initialDBIndexing=true;
  }else{
    wxLogError(_T("Failed to connect to database. Check connection parameters."));
  }
}

// Set current options
void AviewFrame::SetOptions(){
  using namespace AviewOpts;
  for(int i=0;i<N_TEMP;++i){
    m_TempInfo->SetDisplayRangeSpec(TempLabels[i],TempSpec[i]);
    m_TempInfo->SetDisplayRange(TempLabels[i],TempRange[i][0],TempRange[i][1]);
    m_TempInfo->SetDisplayScaleType(TempLabels[i],TempScaleType[i]);
    m_TempInfo->SetDisplayAverage(TempLabels[i],TempAverage[i]);
  }

  for(int i=0;i<N_DISK;++i){
    m_DiskInfo->SetDisplayRangeSpec(DiskLabels[i],DiskSpec[i]);
    m_DiskInfo->SetDisplayRange(DiskLabels[i],DiskRange[i][0],DiskRange[i][1]);
    m_DiskInfo->SetDisplayScaleType(DiskLabels[i],DiskScaleType[i]);
    m_DiskInfo->SetDisplayAverage(DiskLabels[i],DiskAverage[i]);
  }

  for(int i=0;i<N_QUEUE;++i){
    m_QueueInfo->SetDisplayRangeSpec(QueueLabels[i],QueueSpec[i]);
    m_QueueInfo->SetDisplayRange(QueueLabels[i],QueueRange[i][0],QueueRange[i][1]);
    m_QueueInfo->SetDisplayScaleType(QueueLabels[i],QueueScaleType[i]);
    m_QueueInfo->SetDisplayAverage(QueueLabels[i],QueueAverage[i]);
  }

  for(int i=0;i<N_CURRENT;++i)
    m_CurrentInfo->SetDisplayRangeSpec(CurrentLabels[i],CurrentSpec[i]);
    
  for(int i=0;i<N_VOLTAGE;++i)
    m_VoltageInfo->SetDisplayRangeSpec(VoltageLabels[i],VoltageSpec[i]);

  for(int i=0;i<N_POWER;++i)
    m_PowerInfo->SetDisplayRangeSpec(PowerLabels[i],PowerSpec[i]);
  payloadView->SetRangeSpecs();

  for(int i=0;i<ACTIVE_SURFS;++i){
    for(int j=0;j<CHANNELS_PER_SURF-1;++j){
      channelTab[i][j]->SetScale(tabScaleMin,tabScaleMax);
      switch(AviewOpts::RFTab){
      case AviewOpts::PowerTab:
	channelTab[i][j]->SetData(rfDisplay[i][j]->GetPower());
	break;
      case AviewOpts::FFTTab:
	channelTab[i][j]->SetData(rfDisplay[i][j]->GetFFT());
	break;
      case AviewOpts::VoltageTab:
      default:
	channelTab[i][j]->SetData(rfDisplay[i][j]->GetVoltage());
	break;
      }
    }
    channelTab[i][CHANNELS_PER_SURF-1]->SetScale(clockTabScaleMin,clockTabScaleMax);
  }
  
  switch(RFOverview){
  case SurfView:
    rfClockGridH->Show(ClockRF);
    break;
  case PhiView:
    clockLabel->Show(ClockRF);
//////////peng///////////
  case newPhiView:
    newclockLabel->Show(ClockRF);
////////////peng/////////
  case PayloadView:
  default:
    rfClockGridV->Show(ClockRF);
    break;
  }
  GetSizer()->GetItem(rfSizer,true)->GetSizer()->Layout();

  m_readDelay=readDelay;
  // Reset timer steps
  for(int i=0;i<N_PCKTYPE;++i){
    m_flightControl->AdjustTimer(i,m_readDelay);
    m_dbControl->AdjustTimer(i,m_readDelay);
  }

  m_AuxInfo->SetLowPressureRangeSpec(AuxSpec[0]);
  m_AuxInfo->SetHighPressureRangeSpec(AuxSpec[0]);

  m_eventRate->SetRangeSpec(EventRateSpec);
  m_eventRate->SetRange(EventRateRange[0],EventRateRange[1]);
  m_eventRate->SetScaleType(EventRateScaleType);
  m_eventRate->SetAverage(EventRateAverage);
  m_eventRateTimer->Stop();
  m_eventRateTimer->Start(EventRateStep*1000);

  SetTriggerPattern(0,0,0,0,0,0,true);

  return;
}


void AviewFrame::SetTriggerPattern(unsigned short l1mask, unsigned short l1maskH, unsigned short l3Pat, unsigned short l3PatH, unsigned short phimask, unsigned short phimaskH, bool redo){
  using namespace AviewOpts;
  static unsigned long l1, l1H, l3, l3H, phi, phiH;
  wxColour tabColour,tabColourH, bgColour;
  
  // If new info, store it
  if(!redo){
    l1=l1mask ;
    l1H=l1maskH ;
    l3=l3Pat;
    l3H=l3PatH;
    phi=phimask ;
    phiH=phimaskH ;
  }
//phi=0xFFFF;//test: turn phi mask on
//phiH=0xFFFF;
  // Loop over channels and apply colour as needed
for(int i=0;i<N_QRH;++i){
    if(phi&(1<<(surfMap[i].phi-1))){  // Check phi mask pattern for verical
	if(l1&(1<<(surfMap[i].phi-1))){
		bgColour=L1PhiMaskColour;
	}else{
		bgColour=PhiMaskColour;
	}
    }
    else {
	if(l1&(1<<(surfMap[i].phi-1))){
		bgColour=L1MaskColour;
	}else{
		bgColour=*wxBLACK;
	}
      //payloadView->SetTriggerPatternColour(i,*wxWHITE);
    }
    if(l3&(1<<(surfMap[i].phi-1))){ //Check l3 trigger pattern for verical
	tabColour=L3TrigColour;
    }else{
	tabColour=*wxWHITE;
    }

    if(phiH&(1<<(surfMap[i].phi-1))){  // Check phi mask pattern for verical
	if(l1H&(1<<(surfMap[i].phi-1))){
		bgColour=L1PhiMaskColour;
	}else{
		bgColour=PhiMaskColour;
	}
    }
    else {
	if(l1H&(1<<(surfMap[i].phi-1))){
		bgColour=L1MaskColour;
	}else{
		bgColour=*wxBLACK;
	}
      //payloadView->SetTriggerPatternColour(i,*wxWHITE);
    }
    if(l3H&(1<<(surfMap[i].phi-1))){ //Check l3 trigger pattern for verical
	tabColourH=L3HTrigColour;
    }else{
	tabColourH=*wxWHITE;
    }
	bgColour=*wxBLACK;
      channelTab[surfMap[i].v_surf-1][surfMap[i].v_chan-1]->SetBGColour(bgColour);
      channelTab[surfMap[i].h_surf-1][surfMap[i].h_chan-1]->SetBGColour(bgColour);
      channelTab[surfMap[i].v_surf-1][surfMap[i].v_chan-1]->SetColour(tabColour);
      channelTab[surfMap[i].h_surf-1][surfMap[i].h_chan-1]->SetColour(tabColourH);
      payloadView->SetTriggerPatternColour(i,tabColour);
  }

}

// Arranges RF tabs 
void AviewFrame::OrderRFTabs(){
  // Detach first
  for(int i=0;i<CHANNELS_PER_SURF-1;++i)
    for(int j=0;j<ACTIVE_SURFS;++j)
      l3Grid->Detach(channelTab[j][i]) || surfGrid->Detach(channelTab[j][i]) || phiGrid->Detach(channelTab[j][i])|| newphiGrid->Detach(channelTab[j][i]);

  for(int i=0;i<N_PHI/2;++i) {
    if(midLabel[i]) newphiGrid->Detach(midLabel[i]);
  }


  // Attach
  switch(AviewOpts::RFOverview){
  case AviewOpts::SurfView: // Show tabs in SURF order
    for(int i=0;i<CHANNELS_PER_SURF-1;++i) //peng 
      for(int j=0;j<ACTIVE_SURFS;++j)
	surfGrid->Add(channelTab[j][i],1,wxEXPAND);
    break;
  case AviewOpts::PhiView:  // Show tabs in PHI sectors
    // First row
    for(int a=0;a<8;++a){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Second row
    for(int a=0;a<8;++a){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Third row
    for(int a=8;a<16;++a){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Fourth row
    for(int a=8;a<16;++a){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Fifth row (5/6 and 7/8 were swapped for better alignment on
    // antennas on the same phi sector. 20-Jul-08
    for(int a=17;a<32;a+=2){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Sixth row
    for(int a=17;a<32;a+=2){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Seventh row
    for(int a=16;a<32;a+=2){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Eigth row
    for(int a=16;a<32;a+=2){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	phiGrid->AddStretchSpacer();
    }
//////////////peng////////////
if(ACTIVE_SURFS==12){
    // Nine row
    for(int a=32;a<40;a++){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // Tenth row
    for(int a=32;a<40;a++){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // 11th row
    for(int a=40;a<48;a++){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	phiGrid->AddStretchSpacer();
    }
    // 12th row
    for(int a=40;a<48;a++){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	phiGrid->AddStretchSpacer();
    }
}
//////////peng//////////////
    // Ninth row
    if(ACTIVE_SURFS==9){
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][0],1,wxEXPAND);  // BC1
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][5],1,wxEXPAND);  // DC2 
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][1],1,wxEXPAND);  // BC2
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][6],1,wxEXPAND);  // DC3
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][2],1,wxEXPAND);  // BC3
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][7],1,wxEXPAND);  // DC4
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][3],1,wxEXPAND);  // BC4
      phiGrid->Add(channelTab[ACTIVE_SURFS-1][4],1,wxEXPAND);  // DC1
    }
    if (ACTIVE_SURFS==10) {
      for(int a=32;a<40;a++){
	int nSurf=surfMap[a].h_surf-1;
	int nChan=surfMap[a].h_chan-1;
	if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	  phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
	else
	  phiGrid->AddStretchSpacer();
      }
      for(int a=32;a<40;a++){
	int nSurf=surfMap[a].v_surf-1;
	int nChan=surfMap[a].v_chan-1;
	if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	  phiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
	else
	  phiGrid->AddStretchSpacer();
      }
    }

    break;
/////////////////peng///////////////////
  case AviewOpts::newPhiView:  // Show tabs in PHI sectors
    // First row
    for(int a=0;a<4;++a)
      for(int b=0;b<9;b+=8){
      int nSurf=surfMap[a+b].h_surf-1;
      int nChan=surfMap[a+b].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,0);
      else
	newphiGrid->AddStretchSpacer();
    }
    // Second row
    for(int a=0;a<4;++a)
      for(int b=0; b<9; b+=8){
      int nSurf=surfMap[a+b].v_surf-1;
      int nChan=surfMap[a+b].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	newphiGrid->AddStretchSpacer();
    }
    // Third row
    for(int a=16;a<24;++a){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	newphiGrid->AddStretchSpacer();
    }
    // Fourth row
    for(int a=16;a<24;++a){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	newphiGrid->AddStretchSpacer();
    }
    // Fifth row (5/6 and 7/8 were swapped for better alignment on
    // antennas on the same phi sector. 20-Jul-08
    for(int a=32;a<40;a++){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	newphiGrid->AddStretchSpacer();
    }
    // Sixth row
    for(int a=32;a<40;a++){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,0);
      else
	newphiGrid->AddStretchSpacer();
    }
//Middle lables
    for(int i=0;i<N_PHI/2;++i){
      wxString tmp;
      tmp << _T("\n\nPhi ") << (i+9) ;
      if(midLabel[i]==0) midLabel[i]=new wxStaticText(this,wxID_ANY,tmp,wxDefaultPosition, wxSize(63,39),wxEXPAND|wxALIGN_CENTRE);
      newphiGrid->Add(midLabel[i],1,wxEXPAND|wxTOP, 0);
    }

    // Seventh row
    for(int a=4;a<8;a++)
      for(int b=0; b<9; b+=8){
      int nSurf=surfMap[a+b].h_surf-1;
      int nChan=surfMap[a+b].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,0);
      else
	newphiGrid->AddStretchSpacer();
    }
    // Eigth row
    for(int a=4;a<8;a++)
      for(int b=0; b<9; b+=8){
      int nSurf=surfMap[a+b].v_surf-1;
      int nChan=surfMap[a+b].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	newphiGrid->AddStretchSpacer();
    }
//////////////peng////////////
if(ACTIVE_SURFS==12){
    // Ninth row
    for(int a=24;a<32;a++){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	newphiGrid->AddStretchSpacer();
    }
    // Tenth row
    for(int a=24;a<32;a++){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	newphiGrid->AddStretchSpacer();
    }
    // 11th row
    for(int a=40;a<48;a++){
      int nSurf=surfMap[a].h_surf-1;
      int nChan=surfMap[a].h_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
      else
	newphiGrid->AddStretchSpacer();
    }
    // 12th row
    for(int a=40;a<48;a++){
      int nSurf=surfMap[a].v_surf-1;
      int nChan=surfMap[a].v_chan-1;
      if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
      else
	newphiGrid->AddStretchSpacer();
    }
}
//////////peng//////////////
    // Ninth row
    if(ACTIVE_SURFS==9){
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][0],1,wxEXPAND);  // BC1
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][5],1,wxEXPAND);  // DC2 
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][1],1,wxEXPAND);  // BC2
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][6],1,wxEXPAND);  // DC3
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][2],1,wxEXPAND);  // BC3
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][7],1,wxEXPAND);  // DC4
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][3],1,wxEXPAND);  // BC4
      newphiGrid->Add(channelTab[ACTIVE_SURFS-1][4],1,wxEXPAND);  // DC1
    }
    if (ACTIVE_SURFS==10) {
      for(int a=32;a<40;a++){
	int nSurf=surfMap[a].h_surf-1;
	int nChan=surfMap[a].h_chan-1;
	if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	  newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxTOP,2);
	else
	  newphiGrid->AddStretchSpacer();
      }
      for(int a=32;a<40;a++){
	int nSurf=surfMap[a].v_surf-1;
	int nChan=surfMap[a].v_chan-1;
	if(nSurf<ACTIVE_SURFS && nChan<CHANNELS_PER_SURF) 
	  newphiGrid->Add(channelTab[nSurf][nChan],1,wxEXPAND|wxBOTTOM,2);
	else
	  newphiGrid->AddStretchSpacer();
      }
    }

    break;
/////////////////peng/////////////////////
  case AviewOpts::PayloadView: // Show L3 RF tabs
  default:
    for(int i=0;i<CHANNELS_PER_SURF-1;++i) 
      for(int j=0;j<ACTIVE_SURFS;++j)
	if(channelTab[j][i]->GetColour()==AviewOpts::L3TrigColour)
	  l3Grid->Add(channelTab[j][i],1,wxEXPAND);
	else
	  surfGrid->Add(channelTab[j][i],0);
    break;
  }
}

// Switches type of RF display
void AviewFrame::SwitchRFView(bool resize){
  OrderRFTabs();
  switch(AviewOpts::RFOverview){
  case AviewOpts::SurfView:
    for(int j=0;j<ACTIVE_SURFS;++j)
      if(rfClockGridV->Detach(channelTab[j][CHANNELS_PER_SURF-1]))
	 rfClockGridH->Add(channelTab[j][CHANNELS_PER_SURF-1],1,wxEXPAND);
    surfGridLabel->Show(true);
    phiGridLabel->Show(false);
    newphiGridLabel->Show(false);//peng
    clockLabel->Show(false);
    newclockLabel->Show(false);//peng
    tierLabel->Show(false);
    newtierLabel->Show(false);//peng
    surfGrid->Show(true);
    phiGrid->Show(false);
    newphiGrid->Show(false);//peng
    payloadView->Show(false);
    rfClockGridV->Show(false);
    l3Grid->Show(false);
    rfClockGridH->Show(AviewOpts::ClockRF);
    break;
  case AviewOpts:: PhiView:
    for(int j=0;j<ACTIVE_SURFS;++j)
      if(rfClockGridH->Detach(channelTab[j][CHANNELS_PER_SURF-1]))
	rfClockGridV->Add(channelTab[j][CHANNELS_PER_SURF-1],1,wxEXPAND);
    surfGridLabel->Show(false);
    phiGridLabel->Show(true);
    newphiGridLabel->Show(false);//peng
    clockLabel->Show(AviewOpts::ClockRF);
    newclockLabel->Show(false);//peng
    tierLabel->Show(true);
    newtierLabel->Show(false);//peng
    surfGrid->Show(false);
    phiGrid->Show(true);
    newphiGrid->Show(false);//peng
    payloadView->Show(false);
    l3Grid->Show(false);
    rfClockGridV->Show(AviewOpts::ClockRF);
    rfClockGridH->Show(false);    
    break;
///////////////////peng//////////////
  case AviewOpts:: newPhiView:
    for(int j=0;j<ACTIVE_SURFS;++j)
      if(rfClockGridH->Detach(channelTab[j][CHANNELS_PER_SURF-1]))
	rfClockGridV->Add(channelTab[j][CHANNELS_PER_SURF-1],0,wxEXPAND);
    surfGridLabel->Show(false);
    phiGridLabel->Show(false);
    newphiGridLabel->Show(true);//peng
    //clockLabel->Show(AviewOpts::ClockRF);
    clockLabel->Show(false);
    newclockLabel->Show(AviewOpts::ClockRF);//peng
    tierLabel->Show(false);
    newtierLabel->Show(true);//peng
    surfGrid->Show(false);
    phiGrid->Show(false);
    newphiGrid->Show(true);//peng
    payloadView->Show(false);
    l3Grid->Show(false);
    rfClockGridV->Show(AviewOpts::ClockRF);
    rfClockGridH->Show(false);    
    break;
////////////////peng///////////////
  case AviewOpts::PayloadView:
  default:
    for(int j=0;j<ACTIVE_SURFS;++j)
      if(rfClockGridH->Detach(channelTab[j][CHANNELS_PER_SURF-1]))
 	rfClockGridV->Add(channelTab[j][CHANNELS_PER_SURF-1],1,wxEXPAND);
    surfGridLabel->Show(false);
    phiGridLabel->Show(false);
    newphiGridLabel->Show(false);//peng
    clockLabel->Show(false);
    newclockLabel->Show(false);//peng
    tierLabel->Show(false);
    newtierLabel->Show(false);//peng
    surfGrid->Show(false);
    phiGrid->Show(false);
    newphiGrid->Show(false);//peng
    payloadView->Show(true);
    l3Grid->Show(l3Grid->GetChildren().GetCount()>0);
    rfClockGridV->Show(AviewOpts::ClockRF);
    rfClockGridH->Show(false);
    break;
  }

  if(resize){
///*
    if(GetSizer()->GetSize().GetWidth()>GetSizer()->GetMinSize().GetWidth() || 
       GetSizer()->GetSize().GetHeight()>GetSizer()->GetMinSize().GetHeight()){
;
    }
      GetSizer()->SetSizeHints(this);
      //topSizer->CalcMin();
      //SetSizerAndFit(topSizer);
      //topSizer->Fit(this);
      //topSizer->Layout();
      //GetSizer()->Fit(this);
      this->SetClientSize(GetSizer()->GetMinSize());
      this->Layout();
      //GetSizer()->Layout();
      GetSizer()->SetSizeHints(this);
      GetSizer()->Fit(this);
      GetSizer()->Layout();
//*/
//GetSizer()->Layout();
  }
  return;
}

// Switches reference for RF display color coding
void AviewFrame::SwitchRFReference(){
  // This is achived just by forcing the data back to payload view which 
  // will recalculate required reference values
  for(int i=0;i<ACTIVE_SURFS;++i)
    for(int j=0;j<CHANNELS_PER_SURF-1;++j)
      payloadView->SetValue(rfCurve[i][j],i,j);

  return;
}

// Save current display options
bool AviewFrame::SaveOptions(const wxString &file){
  using namespace AviewOpts;
  // Open file
  wxFile saveFile(file.c_str(),wxFile::write);
  if (!saveFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open file %s for writing"),file.c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }
  m_xmlHandler->WriteHead(saveFile,_T("aview"));
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("temp"),N_TEMP,TempLabels,TempSpec,TempRange,TempScaleType,TempAverage);
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("disk"),N_DISK,DiskLabels,DiskSpec,DiskRange,DiskScaleType,DiskAverage);
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("queue"),N_QUEUE,QueueLabels,QueueSpec,QueueRange,QueueScaleType,QueueAverage);
  m_xmlHandler->WriteTextItemBlock(saveFile,_T("current"),N_CURRENT,CurrentLabels,CurrentSpec);
  m_xmlHandler->WriteTextItemBlock(saveFile,_T("voltage"),N_VOLTAGE,VoltageLabels,VoltageSpec);
  m_xmlHandler->WriteTextItemBlock(saveFile,_T("power"),N_POWER,PowerLabels,PowerSpec);
  
  // Prepare extra tags for rf line
  xmlTagValuePair **extra;
  extra = new xmlTagValuePair*[16];
  switch(RFOverview){
  case SurfView: extra[0]= new xmlTagValuePair(_T("overview"),_T("surf")); break;
  case PhiView: extra[0]= new xmlTagValuePair(_T("overview"),_T("phi")); break;
  case newPhiView: extra[0]= new xmlTagValuePair(_T("overview"),_T("newphi")); break;////peng
  case PayloadView: 
  default:
    extra[0]= new xmlTagValuePair(_T("overview"),_T("payload")); break;
  }
  extra[1]=new xmlTagValuePair(_T("style"),(PayloadStyle==Mesh?_T("mesh"):_T("solid")));
  extra[2]=new xmlTagValuePair(_T("reference"),(RFReference==RMS?_T("rms"):_T("absolute")));
  extra[3]=new xmlTagValuePair(_T("voltage"),(RFView&VoltageView?_T("on"):_T("off")));
  extra[4]=new xmlTagValuePair(_T("fft"),(RFView&FFTView?_T("on"):_T("off")));
  extra[5]=new xmlTagValuePair(_T("power"),(RFView&PowerView?_T("on"):_T("off")));
  extra[6]=new xmlTagValuePair(_T("clockrf"),(ClockRF?_T("on"):_T("off")));
  switch(RFTab){
  case FFTTab: extra[7]= new xmlTagValuePair(_T("tab"),_T("fft")); break;
  case PowerTab: extra[7]= new xmlTagValuePair(_T("tab"),_T("power")); break;
  case VoltageTab: 
  default:
    extra[7]= new xmlTagValuePair(_T("tab"),_T("voltage")); break;
  }
  wxString tmp;
  tmp.Printf(_T("%d:%d"),tabScaleMin,tabScaleMax);
  extra[8]=new xmlTagValuePair(_T("tabscale"),tmp);
  tmp.Printf(_T("%d:%d"),clockTabScaleMin,clockTabScaleMax);
  extra[9]=new xmlTagValuePair(_T("clkscale"),tmp);
  extra[10]=new xmlTagValuePair(_T("delay"),wxString()<<readDelay);
  tmp.Printf(_T("#%02X%02X%02X"),PhiMaskColour.Red(),PhiMaskColour.Green(), PhiMaskColour. Blue());
  extra[11]=new xmlTagValuePair(_T("phimaskcolour_v"),tmp);
  tmp.Printf(_T("#%02X%02X%02X"),PhiHMaskColour.Red(),PhiHMaskColour.Green(), PhiHMaskColour. Blue());
  extra[12]=new xmlTagValuePair(_T("phimaskcolour_h"),tmp);
  tmp.Printf(_T("#%02X%02X%02X"),L3TrigColour.Red(),L3TrigColour.Green(), L3TrigColour. Blue());
  extra[13]=new xmlTagValuePair(_T("l3colour_v"),tmp);
  tmp.Printf(_T("#%02X%02X%02X"),L3HTrigColour.Red(),L3HTrigColour.Green(), L3HTrigColour. Blue());
  extra[14]=new xmlTagValuePair(_T("l3colour_h"),tmp);
  extra[15]=new xmlTagValuePair(_T("compass"),(Compass?_T("on"):_T("off")));
  extra[16]=NULL;
  m_xmlHandler->WriteTextItemBlock(saveFile,_T("rf"),N_RFANT,RFLabels,RFSpecRMS,extra);
  for(int i=0;i<15;++i) delete extra[i];  
  m_xmlHandler->WriteTextItemBlock(saveFile,_T("rfabs"),N_RFANT,RFLabels,RFSpecAbsolute);

  // Prepare tags for db line
  extra[0]= new xmlTagValuePair(_T("host"),dbHost);
  extra[1]= new xmlTagValuePair(_T("user"),dbUser);
  extra[2]= new xmlTagValuePair(_T("name"),dbName);
  extra[3]= new xmlTagValuePair(_T("sql"),sqlDump?_T("on"):_T("off"));
  extra[4]= new xmlTagValuePair(_T("crc"),crcCheck?_T("on"):_T("off"));
  extra[5]= new xmlTagValuePair(_T("run"),fetchRunNumber?_T("on"):_T("off"));
  extra[6]=NULL;
  m_xmlHandler->OpenTag(saveFile,_T("db"),extra,true);

  for(int i=0;i<6;++i) delete extra[i];  

  // Prepare extra tags for aux line
  extra[0]= new xmlTagValuePair(_T("gps"),(DefaultGPS==ADU5?_T("adu5"):_T("g12")));
  extra[1]=NULL;
  m_xmlHandler->WriteTextItemBlock(saveFile,_T("aux"),N_AUX,AuxLabels,AuxSpec,extra);
  delete extra[0]; 

  // Prepare extra tag for event rate line
  tmp.Printf(_T("%d"),EventRateStep);
  extra[0]= new xmlTagValuePair(_T("step"),tmp);
  extra[1]=NULL;
  wxString blank;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("rate"),1,&EventRateLabels,&EventRateSpec,&EventRateRange,&EventRateScaleType,
				  &EventRateAverage,extra);
  delete extra[0];

  m_xmlHandler->CloseTag(saveFile,_T("aview"));

  saveFile.Close();
  SetStatusText(_T("Saved options"),2);

  delete[] extra;
  return true;
}


// Load display options
bool AviewFrame::LoadOptions(const wxString &file){
  AviewOpts::RFOverviewType OldRFOverview=AviewOpts::RFOverview;
  AviewOpts::RFReferenceType OldRFReference=AviewOpts::RFReference;

  // Open file
  wxTextFile loadFile(file);
  if (!loadFile.Open()){
    wxString error;
    error.Printf(_T("Failed to open options file %s"),file.c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }
  
  // Set up XML parser
  XML_Parser parser = XML_ParserCreate(NULL);
  if (!parser) {
    wxMessageBox(_T("Couldn't allocate memory for XML parser"), 
		 _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  m_xmlHandler->Reset();
  XML_SetUserData(parser,(void*)m_xmlHandler);
  XML_SetElementHandler(parser, AviewXMLHandler::xmlAviewStart, 
			AviewXMLHandler::xmlAviewEnd);

  wxString str;
  for (str = loadFile.GetFirstLine(); !loadFile.Eof();  str = loadFile.GetNextLine() ){
    if(!XML_Parse(parser, str.mb_str(), str.Length(),0)){
      wxLogError(_T("XML parse error at line %d:%s"),
		 (int)XML_GetCurrentLineNumber(parser),
		 XML_ErrorString(XML_GetErrorCode(parser)));
      return false;
    }else if(!m_xmlHandler->Ok()) {
      wxLogError(_T("XML format error on line: %s"),str.c_str());
      return false;
    }
  }
  if(!XML_Parse(parser, str.mb_str(), str.Length(),1)){
    wxLogError(_T("XML parse error at line %d:%s"),
	       (int)XML_GetCurrentLineNumber(parser),
	       XML_ErrorString(XML_GetErrorCode(parser)));
    return false;
  }else if(!m_xmlHandler->Ok()) {
    wxLogError(_T("XML format error on line: %s"),str.c_str());
    return false;
  }

  // Clean up
  XML_ParserFree(parser);
  loadFile.Close();

  if(m_xmlHandler->Ok()){
    // Enforce new settings
    SetOptions();
    m_OptionsDialog->ImportOptions();
    // Change payload display if needed
    if(OldRFOverview!=AviewOpts::RFOverview) SwitchRFView(true);
    if(OldRFReference!=AviewOpts::RFReference) SwitchRFReference();
    SetStatusText(_T("Loaded new options"),2);
  } else 
    SetStatusText(_T("Error loading options"),2);

  return m_xmlHandler->Ok();
}

// Loads raw pedestal file
bool AviewFrame::ReadRawPedestals(){
  // Open file
  wxFile pedFile(pedestalFile->GetFullPath().c_str(),wxFile::read);
  if (!pedFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open pedestal file %s"),pedestalFile->GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file								
  float tmpPed[ACTIVE_SURFS][LABRADORS_PER_SURF][CHANNELS_PER_SURF][MAX_NUMBER_SAMPLES];  // Floats, just for Ryan
  short shortPed[ACTIVE_SURFS][CHANNELS_PER_SURF][LABRADORS_PER_SURF][MAX_NUMBER_SAMPLES];  // This is Shige's format
  bool shortPeds=false;
  if(!m_fileHandler->MultiRead(pedFile,tmpPed,sizeof(float)*ACTIVE_SURFS*CHANNELS_PER_SURF*LABRADORS_PER_SURF*MAX_NUMBER_SAMPLES,0)){
    // Try short version
    shortPeds=true;
    if(!m_fileHandler->MultiRead(pedFile,shortPed,sizeof(short)*ACTIVE_SURFS*CHANNELS_PER_SURF*LABRADORS_PER_SURF*MAX_NUMBER_SAMPLES,0)){
      wxMessageBox(_T("Raw pedestal file is not of expected size"), 
		   _T("File size error"), wxOK|wxICON_ERROR);
      pedFile.Close();
      return false;
    }
  }
  for(int i=0;i<ACTIVE_SURFS;++i)
    for(int j=0;j<LABRADORS_PER_SURF;++j)
      for(int k=0;k<CHANNELS_PER_SURF;++k)
	for(int l=0;l<MAX_NUMBER_SAMPLES;++l)
	  pedestalData[i][j][k][l]=(shortPeds?shortPed[i][k][j][l]:tmpPed[i][j][k][l]);

  pedFile.Close();
  SetStatusText(_T("Pedestals loaded"),1);
  pedestalsRead=true;
  return true;
}

// Loads raw data file
bool AviewFrame::ReadRawData(){
  // Open file
  wxFile rawFile(currentRawFile->GetFullPath().c_str(),wxFile::read);
  if (!rawFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open raw waveform file %s"),currentRawFile->GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file
  unsigned int wv_data[ACTIVE_SURFS][CHANNELS_PER_SURF][MAX_NUMBER_SAMPLES];
  if(!m_fileHandler->MultiRead(rawFile,wv_data, sizeof(unsigned int)*MAX_NUMBER_SAMPLES*CHANNELS_PER_SURF*ACTIVE_SURFS)){
    wxMessageBox(_T("Raw waveform file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    rawFile.Close();
    return false;
  }
  rawFile.Close();

  // Read associated trigger file
  wxFileName rawTrigFile=*currentRawFile;
  wxString rawTrigFileName=rawTrigFile.GetName();
  rawTrigFileName.SetChar(0,'t');
  rawTrigFile.SetName(rawTrigFileName);
  // This is hack since surf_data.(x) is associate with turf_data.(x+1) ; PM
  wxString rawTrigFileExt=rawTrigFile.GetExt();
  long val;
  if(rawTrigFileExt.ToLong(&val)){
    wxString ext;
    ext << (val+1);
    rawTrigFile.SetExt(ext);
  }

  if (!rawFile.Open(rawTrigFile.GetFullPath().c_str(),wxFile::read)){
    wxString error;
    error.Printf(_T("Failed to open raw TURFIO file %s"),rawTrigFile.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file
  // TURFIO data structure, TV test, Shige's version for raw turf data
  typedef struct {
    unsigned short rawTrig; // raw trigger number
    unsigned short l3aTrig; // same as raw trig
    unsigned long ppsNum;     // 1PPS
    unsigned long trigTime;     // trig time
    unsigned long trigInterval; // interval since last trig
    unsigned int l3rate[4][16] ; 
  } TurfioStruct_t;
  TurfioStruct_t turfdata;

  if(!m_fileHandler->MultiRead(rawFile,&turfdata,sizeof(TurfioStruct_t),0)){
    wxMessageBox(_T("TURFIO file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    rawFile.Close();
    return false;
  }
  rawFile.Close();

  // Rotation using Shige's algoritham which is not fully tested and doesn't account for
  // proper treatement of extra 4 SCAs

  /* set last_hb parameter which is used to rotate data. */
  int last_hb[ACTIVE_SURFS][CHANNELS_PER_SURF];
  for (int j=0 ; j<ACTIVE_SURFS ; ++j) 
    for (int k=0 ; k<CHANNELS_PER_SURF ; ++k) {
      last_hb[j][k]=-1 ;
      for(int i=0 ; i<MAX_NUMBER_SAMPLES ; i++) {
	if (!wv_data[j][k][i] || wv_data[j][k][i]&HIT_BUS) last_hb[j][k] = i ; 
	else if (last_hb[j][k]>=0 && i<MAX_NUMBER_SAMPLES-4) break ;
      }
    }

  // Rotate data and stuff into rawData array
  double v_data[MAX_NUMBER_SAMPLES];
  for(int j=0;j<ACTIVE_SURFS;++j){
    int i,n;
    for(i=0;(wv_data[j][0][i]==0 || wv_data[j][0][i]&HIT_BUS) && i<MAX_NUMBER_SAMPLES-1 ;++i); /* avoid hitbus data points. */
    int nsurf= (wv_data[j][0][i]&0x000f0000) >> 16; // Surf board
    //int nsurf=j+1;
    int ntype= (wv_data[j][0][i]&0x00300000) >> 20; // Board type
    int nchp = (wv_data[j][0][i]&0x00c00000) >> 22; // Chip number
    int nev =  (j==0)?(wv_data[j][0][i]&0x0f000000)>>24:0; // Event number
    bool dtype = wv_data[j][0][i]&0x10000000>>28; // Data type

    if(j==0) m_HeaderInfo->SetEventNumber(nev);
    // Checks
    wxString warning;
    if(nsurf<1 || nsurf>ACTIVE_SURFS){
      wxLogError(_T("[ReadRawEvent] Got SURF %d, expected in range 1-%d. Data will be ignored!"),nsurf,ACTIVE_SURFS);
      continue;
    }
    if(ntype!=0) wxLogWarning(_T("[ReadRawEvent] Got non-SURF board type %d for %d"),ntype,j+1);
    if(dtype) wxLogWarning(_T("[ReadRawEvent] Got HK data, expected LAB data at %d"),j+1);

    // Unwrap and store waveform
    for(int k=0;k<CHANNELS_PER_SURF;++k){
      for(n=last_hb[j][k]+1,i=0 ; i<MAX_NUMBER_SAMPLES ; ++i, ++n){
	if (n >= MAX_NUMBER_SAMPLES-4) n=0 ;  /* event counter roll over. */
	                         /* Ignoring last 4 overlapping values... */
	if((wv_data[j][k][n]&HIT_BUS) || (wv_data[j][k][n]&0xfff)==0) v_data[i]=0. ;
	else{
	    if(pedestalsRead) {
		v_data[i]=(double)((int)((wv_data[j][k][n]&0x0fff)-pedestalData[j][nchp][k][n]))*ADCMV;
		if(v_data[i]>SATURATION) v_data[i]=SATURATION;
		if(v_data[i]<-1*SATURATION) v_data[i]=-1*SATURATION;			
	    }
	    else {
		v_data[i]=(double)(wv_data[j][k][n]&0x0fff); 
		if(v_data[i]>SATURATION) v_data[i]=SATURATION;
		if(v_data[i]<-1*SATURATION) v_data[i]=-1*SATURATION;  
		
	    }
	    
	}
      }
      // Stuff data into RF tabs
      if(rfCurve[nsurf-1][k]) delete rfCurve[nsurf-1][k];
      rfCurve[nsurf-1][k] = new wxGraphCurve(wvtimebase,v_data,MAX_NUMBER_SAMPLES-4-N_HITBUS);
      rfDisplay[nsurf-1][k]->SetData(rfCurve[nsurf-1][k]); 
      if(k==CHANNELS_PER_SURF-1)
	channelTab[nsurf-1][k]->SetData(rfDisplay[nsurf-1][k]->GetVoltage()); 
      else{
	switch(AviewOpts::RFTab){
	case AviewOpts::PowerTab:
	  channelTab[nsurf-1][k]->SetData(rfDisplay[nsurf-1][k]->GetPower()); 
	  break;
	case AviewOpts::FFTTab:
	  channelTab[nsurf-1][k]->SetData(rfDisplay[nsurf-1][k]->GetFFT()); 
	  break;
	case AviewOpts::VoltageTab:
	default:
	  channelTab[nsurf-1][k]->SetData(rfDisplay[nsurf-1][k]->GetVoltage()); 
	  break;
	}
      }
      // Stuff info into payload display
      if(k<CHANNELS_PER_SURF-1) 
	payloadView->SetValue(rfCurve[nsurf-1][k],nsurf-1,k);
    }
  }

  // Stuff trigger info
  m_TriggerInfo->SetTriggerNumber(turfdata.rawTrig,0);
  m_TriggerInfo->SetL3Number(turfdata.l3aTrig);
  m_TriggerInfo->SetTriggerTimeNS(turfdata.trigTime,0);
  m_TriggerInfo->SetPPS(turfdata.ppsNum);
  //  m_TriggerInfo->SetInterval(turfdata.trigInterval);

  SetStatusText(currentRawFile->GetFullName(),1);
  return true;
}

void AviewFrame::IndexRawDirectory(bool full){
  // Index current directory
  wxDir dir;
  if(full){  // Full indexing
    rawFileList->Empty();
    dir.GetAllFiles(currentRawFile->GetPath(),rawFileList,_T("surf_data.*"),wxDIR_FILES);
    if(rawFileList->GetCount()){
      rawFileList->Sort(extensionSortFunction);
      m_pRawDataNext->Enable(true);
      m_pRawDataPrev->Enable(true);
      wxLogMessage(_T("[AviewFrame::IndexRawDirectory] Raw files indexed."));
    }else{
      m_pRawDataNext->Enable(false);
      m_pRawDataPrev->Enable(false);
    }
  }else{   // Update indexing
    wxArrayString newRawFiles;
    dir.GetAllFiles(currentRawFile->GetPath(),&newRawFiles,_T("surf_data.*"),wxDIR_FILES);
    if(newRawFiles.GetCount()>rawFileList->GetCount()){
      newRawFiles.Sort(extensionSortFunction);
      for(unsigned int i=rawFileList->GetCount();i<newRawFiles.GetCount();++i)
	rawFileList->Add(newRawFiles[i]);
      wxLogMessage(_T("[AviewFrame::IndexRawDirectory] Updated list of raw files."));
    }
  }
  return;
}

// Loads pedestal file
bool AviewFrame::ReadPedestals(const wxFileName &file){
  // Open file
  wxFile pedFile(file.GetFullPath().c_str(),wxFile::read);
  if (!pedFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open pedestal file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file								
  PedestalStruct_t ped;
  if(!m_fileHandler->ReadFromFile(pedFile,&ped,sizeof(PedestalStruct_t))){
    wxMessageBox(_T("Pedestal file is not of expected size"), 
		 _T("File size error"), wxOK|wxICON_ERROR);
    pedFile.Close();
    return false;
  }
  for(int i=0;i<ACTIVE_SURFS;++i)
    for(int j=0;j<LABRADORS_PER_SURF;++j)
      for(int k=0;k<CHANNELS_PER_SURF;++k)
	for(int l=0;l<MAX_NUMBER_SAMPLES;++l)
	  pedestalData[i][j][k][l]=ped.thePeds[i][j][k][l]<<1; 

  pedFile.Close();
  SetStatusText(_T("Pedestals loaded"),1);
  pedestalsRead=true;
  return true;
}

// Reads RF event, expects hd file and tries to read associated waveform body 
// file(s) if present
bool AviewFrame::ReadRFEvent(const wxFileName &file,int order,bool wvin){
  // Open header file
  wxFile hdFile(file.GetFullPath().c_str(),wxFile::read);
  if (!hdFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open event header file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  AnitaEventHeader_t header;
  if(!m_fileHandler->ReadFromFile(hdFile,&header,sizeof(AnitaEventHeader_t),order)){
    wxMessageBox(_T("Event header file is not of expected size."),
		 _T("File size error"), wxOK|wxICON_ERROR);
    hdFile.Close();
    return false;
  }
  hdFile.Close();

  // Fill header information
  m_HeaderInfo->SetEventTime(header.unixTime);
  if(wvin) m_flightControl->SetTime(RFEVENT,header.unixTime);
  else m_flightControl->SetTime(HEADER,header.unixTime);
  m_HeaderInfo->SetEventUSTime(header.unixTimeUs);
  m_HeaderInfo->SetEventNSTime(header.gpsSubTime);
  m_HeaderInfo->SetEventNumber(header.eventNumber);
  m_HeaderInfo->SetRunNumber((header.turfEventId&0xfff00000)>>20);
  //m_HeaderInfo->SetAntennaMask(header.antTrigMask);
//  m_TriggerInfo->SetSurfMask(surfhk.surfTrigBandMask);
  m_TriggerInfo->SetCalibStatus(header.calibStatus);
  m_TriggerInfo->SetPriority(header.priority);
  m_TriggerInfo->SetTurfMonitor(header.turfUpperWord);
  m_TriggerInfo->SetTriggerType(header.turfio.trigType);
//*  m_TriggerInfo->SetTriggerNumber(header.turfio.trigNum,header.otherFlag2);
  m_TriggerInfo->SetTriggerNumber(header.turfio.trigNum, 0);
  m_TriggerInfo->SetL3Number(header.turfio.l3Type1Count);
  m_TriggerInfo->SetTriggerTimeNS(header.turfio.trigTime,
				  header.turfio.c3poNum);
  m_TriggerInfo->SetPPS(header.turfio.ppsNum);
  //m_TriggerInfo->SetDeadtime(header.turfio.deadTime/64400.);
  //m_TriggerInfo->SetDeadtime(header.turfio.deadTime/65535.);
/////////////////////////amir's addition july 12 08///////////////////
// m_headerViewer->HeaderData(header);
//////////////////////////////////////////////////////////

  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);

  
  if(wvin) ReadWaveformFile(file.GetFullPath(),header.eventNumber,order);
  else{  // Zero existing waveforms
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF;++j){
	// Stuff data into RF tabs
	if(rfCurve[i][j]) delete rfCurve[i][j];
	rfCurve[i][j] = NULL;
	rfDisplay[i][j]->SetData(NULL); 
	channelTab[i][j]->SetData(NULL);
	// Set antenna colors
	if(j<CHANNELS_PER_SURF-1) 
	  payloadView->SetValue(NULL,i,j);
      }
  }


  // Set color based on trigger participation
  m_trigPatternInfo->SetPattern(header.turfio.l3TrigPattern, header.turfio.l3TrigPatternH);
  
  // Show new tabs if in payload view
  if(AviewOpts::RFOverview==AviewOpts::PayloadView) SwitchRFView(true);

#if 0   // Temporary for debugging of trig pattern
  wxString tmp;
  for(int i=31;i>=0;--i){
    tmp << ((l1TrigPattern&(0x1<<i))?1:0);
    if(i && i%8==0) tmp << ',';
  }
  wxLogMessage(_T("L1 antenna (32--1): %s"),tmp.c_str());
  tmp="";
  for(int i=15;i>=0;--i){
    tmp << ((header.turfio.upperL2TrigPattern&(0x1<<i))?1:0);
    if(i && i%4==0) tmp << ',';
  }
  wxLogMessage("L2 upper phi (16--1): %s",tmp.c_str());
  tmp="";
  for(int i=15;i>=0;--i){
    tmp << ((header.turfio.lowerL2TrigPattern&(0x1<<i))?1:0);
    if(i && i%4==0) tmp << ',';
  }
  wxLogMessage("L2 lower phi (16--1): %s",tmp.c_str());
  tmp="";
  for(int i=15;i>=0;--i){
    tmp << ((header.turfio.l3TrigPattern&(0x1<<i))?1:0);
    if(i && i%4==0) tmp << ',';
  }
  wxLogMessage("L3 phi sector(16--1): %s",tmp.c_str());
#endif  // End of temporary trig pattern dump

  return true;
}

bool AviewFrame::AverageWaveformFile(const wxFileName &file){
  bool goodStream=true;
  wxFile wvFile(file.GetFullPath().c_str(),wxFile::read);
  if (!wvFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open event waveform file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  if(!m_fileHandler->OpenGZ(wvFile)){
    wxMessageBox(_T("Unable to initiate waveform file reading"),
		 _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  enum FileTypeEnum{
    EV = 1,
    PSEV = 2,
    ENCEV = 3,
    PSENCEV = 4,
    UNKNOWN = -1};
  FileTypeEnum fileType=UNKNOWN;
  bool needsCalibration=false;
  if(file.GetFullName().Matches(_T("ev_*"))){
    fileType=EV;
    needsCalibration=true;
  }else if(file.GetFullName().Matches(_T("psev_*"))){
    fileType=PSEV;
    needsCalibration=false;
  }else if(file.GetFullName().Matches(_T("encev_*"))){
    fileType=ENCEV;
    needsCalibration=true;
  }else if(file.GetFullName().Matches(_T("psencev_*"))){
    fileType=PSENCEV;
    needsCalibration=false;
  }else{
    wxMessageBox(_T("Unable to determine waveform file type"),_T("File type error"),
		 wxOK|wxICON_ERROR);
    return false;
  }

  if(needsCalibration && !pedestalsRead){
    *pedestalFile=_T("latestAcqdPeds.dat");
    ReadRawPedestals();
    if(!pedestalsRead){
      wxMessageBox(_T("Unable to read default pedestals. Please load manually."),
		   _T("Pedestal loading error"),wxOK|wxICON_ERROR);
      return false;
    }
  }

  int c=0;
  wxGraphCurve *fileData[ACTIVE_SURFS][CHANNELS_PER_SURF][N_RF_PER_FILE];
  while(c<N_RF_PER_FILE && goodStream){
    SurfChannelPedSubbed_t wv[ACTIVE_SURFS][CHANNELS_PER_SURF];
    switch(fileType){
    case EV:{
      AnitaEventBody_t event;
      if(!m_fileHandler->ReadGZ(&event,sizeof(AnitaEventBody_t))){
	goodStream=false;
	break; 
      }	
      // Store data in local format
      for(int i=0;i<ACTIVE_SURFS;++i)
	for(int j=0;j<CHANNELS_PER_SURF;++j){
	  memcpy(wv[i][j].data,event.channel[i*CHANNELS_PER_SURF+j].data,sizeof(unsigned short)*MAX_NUMBER_SAMPLES);
	  wv[i][j].header=event.channel[i*CHANNELS_PER_SURF+j].header;
	}
      break;
    }
    case PSEV:{
      PedSubbedEventBody_t event;
      if(!m_fileHandler->ReadGZ(&event,sizeof(PedSubbedEventBody_t))){
	goodStream=false;
	break; 
      }	
      // Store data in local format
      for(int i=0;i<ACTIVE_SURFS;++i)
	for(int j=0;j<CHANNELS_PER_SURF;++j){
	  memcpy(wv[i][j].data,event.channel[i*CHANNELS_PER_SURF+j].data,sizeof(short)*MAX_NUMBER_SAMPLES);
	  wv[i][j].header=event.channel[i*CHANNELS_PER_SURF+j].header;
	}
      break;
    }
    case ENCEV:{
      EncodedEventWrapper_t encEventWrapper;
      if(!m_fileHandler->ReadGZ(&encEventWrapper,sizeof(EncodedEventWrapper_t))){
	goodStream=false;
	break;
      }
      unsigned long evnum=encEventWrapper.eventNumber;
      for(int i=0;goodStream && i<ACTIVE_SURFS;++i){
	EncodedSurfPacketHeader_t encSurfPacket;
	// Read surf header
	if(!m_fileHandler->ReadGZ(&encSurfPacket,sizeof(EncodedSurfPacketHeader_t))){
	  wxMessageBox(_T("Encoded event packet file is not of expected size"),
		       _T("File size error"), wxOK|wxICON_ERROR);
	  goodStream=false;
	  break;
	}
	if(evnum!=encSurfPacket.eventNumber){
	  wxMessageBox(_T("Event number mismatch in encoded event packet file."),
		       _T("Event number error"), wxOK|wxICON_ERROR);
	  goodStream=false;
	  break;
	}
	// Read surf channels
	for(int j=0;j<CHANNELS_PER_SURF;++j){
	  EncodedSurfChannelHeader_t encChanHeader;
	  if(!m_fileHandler->ReadGZ(&encChanHeader,sizeof(EncodedSurfChannelHeader_t))){
	    wxMessageBox(_T("Encoded event packet file is not of expected size"),
			 _T("File size error"), wxOK|wxICON_ERROR);
	    goodStream=false;
	    break;
	  }
	  // Read waveform data
	  unsigned char encdata[MAX_NUMBER_SAMPLES*2]; // This is maximum expected size
	  if(!m_fileHandler->ReadGZ(encdata,encChanHeader.numBytes)){
	    wxMessageBox(_T("Encoded event packet file is not of expected size"),
			 _T("File size error"), wxOK|wxICON_ERROR);
	    goodStream=false;
	    break;
	  }
	  // Decode and store
	  SurfChannelFull_t tmpwv;
	  CompressErrorCode_t retVal=decodeChannel(&encChanHeader,encdata,&tmpwv);
	  if(retVal!=COMPRESS_E_OK){
	    wxString tmp;
	    tmp.Printf(_T("Decompression error encountered (%d)"),retVal);
	    wxMessageBox(tmp,_T("Data decoding error"), wxOK|wxICON_ERROR);
	    goodStream=false;
	  }else{
	    memcpy(wv[i][j].data,tmpwv.data,sizeof(short)*MAX_NUMBER_SAMPLES);
	    wv[i][j].header=tmpwv.header;
	  }
	  if(!goodStream) break;
	}
      }
      break;
    }
    case PSENCEV:{
      EncodedEventWrapper_t encEventWrapper;
if(!m_fileHandler->ReadGZ(&encEventWrapper,sizeof(EncodedEventWrapper_t))){
	goodStream=false;
	break;
      }
      unsigned long evnum=encEventWrapper.eventNumber;
      for(int i=0;goodStream && i<ACTIVE_SURFS;++i){
	EncodedPedSubbedSurfPacketHeader_t encSurfPacket;
	// Read surf header
	if(!m_fileHandler->ReadGZ(&encSurfPacket,sizeof(EncodedPedSubbedSurfPacketHeader_t))){
	  wxMessageBox(_T("Encoded pedsubbed event packet file is not of expected size"),
		       _T("File size error"), wxOK|wxICON_ERROR);
	  goodStream=false;
	  break;
	}
	if(evnum!=encSurfPacket.eventNumber){
	  wxMessageBox(_T("Event number mismatch in encoded pedsubbed event packet file."),
		       _T("Event number error"), wxOK|wxICON_ERROR);
	  goodStream=false;
	  break;
	}
	// Read surf channels
	for(int j=0;j<CHANNELS_PER_SURF;++j){
	  EncodedSurfChannelHeader_t encChanHeader;
	  if(!m_fileHandler->ReadGZ(&encChanHeader,sizeof(EncodedSurfChannelHeader_t))){
	    wxMessageBox(_T("Encoded pedsubbed event packet file is not of expected size"),
			 _T("File size error"), wxOK|wxICON_ERROR);
	    goodStream=false;
	    break;
	  }
	  // Read waveform data
	  unsigned char encdata[MAX_NUMBER_SAMPLES*2]; // This is maximum expected size
	  if(!m_fileHandler->ReadGZ(encdata,encChanHeader.numBytes)){
	    wxMessageBox(_T("Encoded pedsubbed event packet file is not of expected size"),
			 _T("File size error"), wxOK|wxICON_ERROR);
	    goodStream=false;
	    break;
	  }
	  // Decode and store
	  CompressErrorCode_t retVal=decodePSChannel(&encChanHeader,encdata,&wv[i][j]);
	  if(retVal!=COMPRESS_E_OK){
	    wxString tmp;
	    tmp.Printf(_T("Decompression error encountered (%d)"),retVal);
	    wxMessageBox(tmp,_T("Data decoding error"), wxOK|wxICON_ERROR);
	    goodStream=false;
	  }
	  if(!goodStream) break;
	}
      }
      break;
    }
    default:
      goodStream=false;
      break;
    }
    
    // Calibrate event and store locally
    if(goodStream){
      double v[MAX_NUMBER_SAMPLES];
      double calwv[MAX_NUMBER_SAMPLES];
      for(int i=0;i<ACTIVE_SURFS;++i)
	for(int j=0;j<CHANNELS_PER_SURF;++j){
	  int nSurf=wv[i][j].header.chanId/CHANNELS_PER_SURF;
	  int nChan=wv[i][j].header.chanId%CHANNELS_PER_SURF;
	  int nChip=wv[i][j].header.chipIdFlag&0x03;
	  if(nSurf<0 || nSurf>=ACTIVE_SURFS ||
	     nChan<0 || nChan>=CHANNELS_PER_SURF ||
	     nChip<0 || nChip>=LABRADORS_PER_SURF){
	    wxLogError(_T("Bad waveform id; SURF %d CHAN %d CHIP %d @ (%d,%d)"),
		       nSurf,nChan,nChip,i,j);
	    break;
	  }
	  bool hbwrap=wv[i][j].header.chipIdFlag&0x08;
	  char hbextra=(wv[i][j].header.chipIdFlag&0xf0)>>4;
	  short hbstart=wv[i][j].header.firstHitbus;
	  short hbend=wv[i][j].header.lastHitbus+hbextra;

	  // Calibrate ...
	  for(int k=0;k<MAX_NUMBER_SAMPLES;++k){
	    if(wv[i][j].data[k]==0){
	      calwv[k]=0;
	    }else{
		if(needsCalibration) {
		    calwv[k]=((wv[i][j].data[k]&0xfff)-pedestalData[nSurf][nChip][nChan][k])*ADCMV;
		    
		    if(calwv[i]>SATURATION) calwv[i]=SATURATION;
		    if(calwv[i]<-1*SATURATION) calwv[i]=-1*SATURATION;
		}
		else {
		    calwv[k]=(wv[i][j].data[k]<<1)*ADCMV; // Data bitshifted with ped subtraction by flight CPU
		    
		    if(calwv[i]>SATURATION) calwv[i]=SATURATION;
		    if(calwv[i]<-1*SATURATION) calwv[i]=-1*SATURATION;
		}
	    }
	  }
	  // ... and rotate
	  int ir=0;
	  if(hbwrap){ // Wrapped hitbus
	    for(int k=hbstart+1;k<hbend;++k)  
	      v[ir++]=calwv[k];
	  }else{
	    for(int k=hbend+1;k<MAX_NUMBER_SAMPLES;++k)  
	      v[ir++]=calwv[k];	
	    for(int k=zeroSCA;k<hbstart;++k)  
	      v[ir++]=calwv[k];
	  }

	  // Calculate mean 
	  double mean=0;
	  for(int k=0;k<ir;++k) mean+=v[k];
	  mean/=ir;
	  // Zero-mean the waveform
	  for(int k=0;k<ir;++k) v[k]-=mean;

	  // Fill in remaining bins with zeros
	  for(int k=ir;k<MAX_NUMBER_SAMPLES;++k)
	    v[k]=0;
	  // Stuff data into array
	  fileData[nSurf][nChan][c] = new wxGraphCurve(wvtimebase,v,MAX_NUMBER_SAMPLES);
	}
      ++c;
    }
  }

  m_fileHandler->CloseGZ(); 
  wvFile.Close();

  if(c<1){
    wxLogError(_T("[AviewFrame::AverageWaveformFile] No events read."));
    return false;
  }

  // Calculate and display averages
  for(int i=0;i<ACTIVE_SURFS;++i)
    for(int j=0;j<CHANNELS_PER_SURF;++j){
      if(rfCurve[i][j]) delete rfCurve[i][j];
      rfDisplay[i][j]->SetAverage(fileData[i][j],c,&rfCurve[i][j]);
      if(j==CHANNELS_PER_SURF-1)
	channelTab[i][j]->SetData(rfDisplay[i][j]->GetVoltage()); 
      else{
	switch(AviewOpts::RFTab){
	case AviewOpts::PowerTab:
	  channelTab[i][j]->SetData(rfDisplay[i][j]->GetPower()); 
	  break;
	case AviewOpts::FFTTab:
	  channelTab[i][j]->SetData(rfDisplay[i][j]->GetFFT()); 
	  break;
	case AviewOpts::VoltageTab:
	default:
	  channelTab[i][j]->SetData(rfDisplay[i][j]->GetVoltage()); 
	  break;
	}
      }
      channelTab[i][j]->SetColour(*wxWHITE);
      payloadView->SetTriggerPatternColour(1,*wxLIGHT_GREY);
      if(j<CHANNELS_PER_SURF-1) payloadView->SetValue(rfCurve[i][j],i,j);
    }
  
  // Release memory
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF;++j)
	for(int k=0;k<c;++k)
	  delete fileData[i][j][k];
  return true;
}

// Loads surf hk data file in flight format and extracts mean power level
bool AviewFrame::ReadDBMRef(const wxFileName &file){
  // Open file
  wxFile surfFile(file.GetFullPath().c_str(),wxFile::read);
  if (!surfFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open SURF HK file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  if(!m_fileHandler->OpenGZ(surfFile)){
    wxMessageBox(_T("Unable to initiate SURF HK file reading"),
		 _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Initiate variables
  float dBm[ACTIVE_SURFS][CHANNELS_PER_SURF-1][N_PACKET_PER_FILE];
  int nev;

  // Read file and store power values
  FullSurfHkStruct_t surfdata;
  for(nev=0;nev<N_PACKET_PER_FILE;++nev){
    // Read surf header
    if(!m_fileHandler->ReadGZ(&surfdata,sizeof(FullSurfHkStruct_t))){
      if(nev)
	wxLogError(_T("Read only %d SURF HK packets, expected %d"),nev,N_PACKET_PER_FILE);
      else
	wxMessageBox(_T("SURF HK packet file is not of expected size. Failed to read dBm references."),
		     _T("File size error"), wxOK|wxICON_ERROR);
      break;
    }
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF-1;++j)
	dBm[i][j][nev]=rfpower_dBm(i,j,surfdata.rfPower[i][j]&0x7FFF);
  }
  m_fileHandler->CloseGZ(); 
  surfFile.Close();

  if(nev){
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF-1;++j){
	double mean=0;
	for(int k=0;k<nev;++k) mean+=pow(10.,(double)dBm[i][j][k]/10);
	mean/=nev;
	rfDisplay[i][j]->SetDBMRef(10*log10(mean));
      }
    SetStatusText(_T("Loaded dBm references"),1);
    return true;
  }else
    return false;
}

void AviewFrame::ReadWaveformFile(const wxString &file,unsigned long evnum,int order){
  // Try reading one of many possible formats of waveform files and fill 
  // (or reset waveforms) as needed
  bool fileFound=false;
  bool wvLoaded=false;
  bool needsCalibration=true;
  SurfChannelPedSubbed_t wv[ACTIVE_SURFS][CHANNELS_PER_SURF];
  for(int i=0;i<ACTIVE_SURFS;++i)
    for(int j=0;j<CHANNELS_PER_SURF;++j)
      for(int k=0;k<MAX_NUMBER_SAMPLES;++k)
	wv[i][j].data[k]=0;

  wxFile wvFile;

  /* There are 4 types of possible ways to store waveform data on flight computer + 2 additional formats used by GSE.
     We try for each type in turn. 
     1. Raw (non-pedsubtracted) unencoded; using structure AnitaEventBody_t, file prefix 'ev'
     2. Pedestal subtracted, unencoded; using structure PedSubbedEventBody_t, file prefix 'psev'
     3. Raw (non-pedsubtracted) encoded; using variable lenght format, but headed by structure
        EncodedSurfPacketHeader_t, file prefix 'encev'
     4. Pedestal subtracted, encoded;  using variable lenght format, but headed by structure
        EncodedPedSubbedSurfPacketHeader_t, file prefix 'psencev'
     5. Raw (non-pedsubtracted) unencoded, single event files; using structure RawWaveformPacket_t, file prefix 'wv'
     6. Pedestal subtracted, unencoded, single event files; using structure PedSubbedWaveformPacket_t, file prefix 'pswv'

     There is no guarantee that required event will be in the corresponding data file. We are only guaranteed that
     if we encounter event number larger that requested, the event is not there. 
  */

  // First try raw unencoded format of the file
  wxString wvfile=file;
  wvfile.Replace(_T("hd_"),_T("ev_"));
  if(!fileFound && !wvLoaded && wxFileName::FileExists(wvfile) && wvFile.Open(wvfile,wxFile::read)){
    fileFound=true;
    needsCalibration=true;
    AnitaEventBody_t event;
    if(!m_fileHandler->OpenGZ(wvFile)){
      wxMessageBox(_T("Unable to initiate raw unencoded  file reading"),
		   _T("File opening error"), wxOK|wxICON_ERROR);
    }else{
      while(!m_fileHandler->Eof()){ // Loop over all events in the file 
	if(!m_fileHandler->ReadGZ(&event,sizeof(AnitaEventBody_t))) break; // Failed reading
	if(event.eventNumber==evnum){  // Event found
	  wvLoaded=true;
	  break;
	}
	if(event.eventNumber>evnum) break;  // Event not in the file
      }
      if(wvLoaded){
	// Store data in local format
	for(int i=0;i<ACTIVE_SURFS;++i)
	  for(int j=0;j<CHANNELS_PER_SURF;++j){
	    memcpy(wv[i][j].data,event.channel[i*CHANNELS_PER_SURF+j].data,sizeof(unsigned short)*MAX_NUMBER_SAMPLES);
	    wv[i][j].header=event.channel[i*CHANNELS_PER_SURF+j].header;
	  }
      }
      m_fileHandler->CloseGZ(); 
    }
    wvFile.Close();
  }

  // Next try ped subtracted unencoded format of the file
  wvfile.Replace(_T("ev_"),_T("psev_"));
  if(!fileFound && !wvLoaded && wxFileName::FileExists(wvfile) && wvFile.Open(wvfile,wxFile::read)){
    fileFound=true;
    needsCalibration=false;
    PedSubbedEventBody_t event;
    if(!m_fileHandler->OpenGZ(wvFile)){
      wxMessageBox(_T("Unable to initiate pedsubbed unencoded file reading"),
		   _T("File opening error"), wxOK|wxICON_ERROR);
    }else{
      while(!m_fileHandler->Eof()){ // Loop over all events in the file 


	if(!m_fileHandler->ReadGZ(&event,sizeof(PedSubbedEventBody_t))) break; // Failed reading



	if(event.eventNumber==evnum){  // Event found
	  wvLoaded=true;
	  break;
	}
	if(event.eventNumber>evnum) break;  // Event not in the file
      }
      if(wvLoaded){
	// Store data in local format
	for(int i=0;i<ACTIVE_SURFS;++i)
	  for(int j=0;j<CHANNELS_PER_SURF;++j){
	    memcpy(wv[i][j].data,event.channel[i*CHANNELS_PER_SURF+j].data,sizeof(short)*MAX_NUMBER_SAMPLES);
	    wv[i][j].header=event.channel[i*CHANNELS_PER_SURF+j].header;
	  }
      }
      m_fileHandler->CloseGZ(); 
    }
    wvFile.Close();
  }


  // Now, try raw encoded format
  wvfile.Replace(_T("psev_"),_T("encev_"));
  if(!fileFound && !wvLoaded && wxFileName::FileExists(wvfile) && wvFile.Open(wvfile,wxFile::read)){
    fileFound=true;
    needsCalibration=true;  
    if(!m_fileHandler->OpenGZ(wvFile)){
      wxMessageBox(_T("Unable to initiate raw encoded waveform file reading"),
		   _T("File opening error"), wxOK|wxICON_ERROR);
    }else{
      EncodedEventWrapper_t encEventWrapper;
      // Loop to find requested event
      while(!m_fileHandler->Eof()){
	if(!m_fileHandler->ReadGZ(&encEventWrapper,sizeof(EncodedEventWrapper_t)) ||  // Error reading
	   encEventWrapper.eventNumber>evnum ||                                      // Event not in the file
	   (encEventWrapper.eventNumber<evnum && !m_fileHandler->ForwardGZ(encEventWrapper.numBytes))){                                                                       // Error forwarding
	  break;
	}else if(encEventWrapper.eventNumber==evnum){
	  wvLoaded=true;
	  break;
	}
      }
      // Read in requested event
      if(wvLoaded)
	for(int i=0;i<ACTIVE_SURFS;++i){
	  EncodedSurfPacketHeader_t encSurfPacket;
	  if(!m_fileHandler->ReadGZ(&encSurfPacket,sizeof(EncodedSurfPacketHeader_t))){ // Error reading
	    wxMessageBox(_T("Raw encoded event packet file is not of expected size"),
			 _T("File size error"), wxOK|wxICON_ERROR);
	    break;
	  }
	  if(encSurfPacket.eventNumber!=evnum){ // Event number mismatch
	    wxLogError(_T("Event number mismatch; expected %ld, wrapper %d, SURF header (%d) %d"),
		       evnum,encEventWrapper.eventNumber,i,encSurfPacket.eventNumber);
	    wvLoaded=false;
	    break;
	  }

	  bool goodStream=true;
	  for(int j=0;j<CHANNELS_PER_SURF;++j){
	    EncodedSurfChannelHeader_t encChanHeader;
	    if(!m_fileHandler->ReadGZ(&encChanHeader,sizeof(EncodedSurfChannelHeader_t))){
	      wxMessageBox(_T("Raw encoded event packet file is not of expected size"),
			   _T("File size error"), wxOK|wxICON_ERROR);
	      goodStream=false;
	      break;
	    }else{  // Read waveform data
	      unsigned char encdata[MAX_NUMBER_SAMPLES*2]; // This is maximum expected size
	      if(!m_fileHandler->ReadGZ(encdata,encChanHeader.numBytes)){
		wxMessageBox(_T("Raw encoded event packet file is not of expected size"),
			     _T("File size error"), wxOK|wxICON_ERROR);

		break;
	      }else{ // Decode and store 
		SurfChannelFull_t tmpwv;
		CompressErrorCode_t retVal=decodeChannel(&encChanHeader,encdata,&tmpwv);
		if(retVal!=COMPRESS_E_OK){
		  wxString tmp;
		  tmp.Printf(_T("Decompression error encountered (%d)"),retVal);
		  wxMessageBox(tmp,_T("Data decoding error"), wxOK|wxICON_ERROR);
		  goodStream=false;
		}else{
		  memcpy(wv[i][j].data,tmpwv.data,sizeof(short)*MAX_NUMBER_SAMPLES);
		  wv[i][j].header=tmpwv.header;
		}
	      }
	    }
	    if(!goodStream) break;
	  }
	  if(!goodStream) break;
	}
      m_fileHandler->CloseGZ(); 
    }
    wvFile.Close();
  }

  // Now, try pedsubbed encoded format
  wvfile.Replace(_T("encev_"),_T("psencev_"));
  if(!fileFound && !wvLoaded && wxFileName::FileExists(wvfile) && wvFile.Open(wvfile,wxFile::read)){
    fileFound=true;
    needsCalibration=false;  
    if(!m_fileHandler->OpenGZ(wvFile)){
      wxMessageBox(_T("Unable to initiate pedsubbed encoded waveform file reading"),
		   _T("File opening error"), wxOK|wxICON_ERROR);
    }else{
      EncodedEventWrapper_t encEventWrapper;
      // Loop to find requested event
      while(!m_fileHandler->Eof()){
	if(!m_fileHandler->ReadGZ(&encEventWrapper,sizeof(EncodedEventWrapper_t)) ||  // Error reading
	   encEventWrapper.eventNumber>evnum ||                                      // Event not in the file
	   (encEventWrapper.eventNumber<evnum && !m_fileHandler->ForwardGZ(encEventWrapper.numBytes))){                                                                       // Error forwarding
	  break;
	}else if(encEventWrapper.eventNumber==evnum){
	  wvLoaded=true;
	  break;
	}
      }
      // Read in requested event
      if(wvLoaded)
	for(int i=0;i<ACTIVE_SURFS;++i){
	  EncodedPedSubbedSurfPacketHeader_t encSurfPacket;
	  if(!m_fileHandler->ReadGZ(&encSurfPacket,sizeof(EncodedPedSubbedSurfPacketHeader_t))){ // Error reading
	    wxMessageBox(_T("Pedsubbed encoded event packet file is not of expected size"),
			 _T("File size error"), wxOK|wxICON_ERROR);
	    break;
	  }
	  if(encSurfPacket.eventNumber!=evnum){ // Event number mismatch
	    wxLogError(_T("Event number mismatch; expected %ld, wrapper %d, SURF header (%d) %d"),
		       evnum,encEventWrapper.eventNumber,i,encSurfPacket.eventNumber);
	    wvLoaded=false;
	    break;
	  }

	  bool goodStream=true;
	  for(int j=0;j<CHANNELS_PER_SURF;++j){
	    EncodedSurfChannelHeader_t encChanHeader;
	    if(!m_fileHandler->ReadGZ(&encChanHeader,sizeof(EncodedSurfChannelHeader_t))){
	      wxMessageBox(_T("Pedsubbed encoded event packet file is not of expected size"),
			   _T("File size error"), wxOK|wxICON_ERROR);
	      goodStream=false;
	      break;
	    }else{  // Read waveform data
	      unsigned char encdata[MAX_NUMBER_SAMPLES*2]; // This is maximum expected size
	      if(!m_fileHandler->ReadGZ(encdata,encChanHeader.numBytes)){
		wxMessageBox(_T("Pedsubbed encoded event packet file is not of expected size"),
			     _T("File size error"), wxOK|wxICON_ERROR);
		goodStream=false;
		break;
	      }else{ // Decode and store 
		CompressErrorCode_t retVal=decodePSChannel(&encChanHeader,encdata,&wv[i][j]);
		if(retVal!=COMPRESS_E_OK){
		  wxString tmp;
		  tmp.Printf(_T("Decompression error encountered (%d)"),retVal);
		  wxMessageBox(tmp,_T("Data decoding error"), wxOK|wxICON_ERROR);
		  goodStream=false;
		}
	      }
	    }
	    if(!goodStream) break;
	  }
	  if(!goodStream) break;
	}
      m_fileHandler->CloseGZ(); 
    }
    wvFile.Close();
  }


  // Next try raw waveform packet files produced by GSE
  int topnum=(evnum/100000)*100000;
  int subnum=(evnum/1000)*1000;
  wvfile.Printf(_T("%s/ev_%d/ev_%d/wv_%ld.dat.gz"),file.BeforeLast(wxFileName::GetPathSeparator()).c_str(),topnum,subnum,evnum);

  if(!fileFound && !wvLoaded && wxFileName::FileExists(wvfile) && wvFile.Open(wvfile,wxFile::read)){
    wvLoaded=true;
    fileFound=true;
    needsCalibration=true;
    if(!m_fileHandler->OpenGZ(wvFile)){
      wxMessageBox(_T("Unable to initiate raw waveform file reading."),
		   _T("File opening error"), wxOK|wxICON_ERROR);
    }else{
      bool goodStream=true;
      RawWaveformPacket_t wvPacket;
      // Loop to over all channels available
      for(int i=0;i<ACTIVE_SURFS;++i){
	for(int j=0;j<CHANNELS_PER_SURF;++j){
	  if(!m_fileHandler->ReadGZ(&wvPacket,sizeof(RawWaveformPacket_t))){
	    goodStream=false;
	    break;
	  }else{
	    memcpy(wv[i][j].data,wvPacket.waveform.data,sizeof(unsigned short)*MAX_NUMBER_SAMPLES);
	    wv[i][j].header=wvPacket.waveform.header;
	  }
	}
	if(!goodStream) break;
      }
      m_fileHandler->CloseGZ();
    }
    wvFile.Close();
  }

  // Finally try pedsubbed waveform packet files produced by GSE
  wvfile.Replace(_T("wv_"),_T("pswv_"));
 
  if(!fileFound && !wvLoaded && wxFileName::FileExists(wvfile) && wvFile.Open(wvfile,wxFile::read)){
    wvLoaded=true;
    fileFound=true;
    needsCalibration=false;
    if(!m_fileHandler->OpenGZ(wvFile)){
      wxMessageBox(_T("Unable to initiate pedsubbed waveform file reading."),
		   _T("File opening error"), wxOK|wxICON_ERROR);
    }else{
      bool goodStream=true;
      PedSubbedWaveformPacket_t wvPacket;
      // Loop to over all channels available
      for(int i=0;i<ACTIVE_SURFS;++i){
	for(int j=0;j<CHANNELS_PER_SURF;++j){
	  if(!m_fileHandler->ReadGZ(&wvPacket,sizeof(PedSubbedWaveformPacket_t))){
	    goodStream=false;
	    break;
	  }else{
	    memcpy(wv[i][j].data,wvPacket.waveform.data,sizeof(short)*MAX_NUMBER_SAMPLES);
	    wv[i][j].header=wvPacket.waveform.header;
	  }
	}
	if(!goodStream) break;
      }
      m_fileHandler->CloseGZ();
    }
    wvFile.Close();
  }

  if(wvLoaded){ // Store data
    double v[MAX_NUMBER_SAMPLES];
    double calwv[MAX_NUMBER_SAMPLES];
    bool tryPeds=true;
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF;++j){
	int nSurf=wv[i][j].header.chanId/CHANNELS_PER_SURF;
	int nChan=wv[i][j].header.chanId%CHANNELS_PER_SURF;
	int nChip=wv[i][j].header.chipIdFlag&0x03;
	if(nSurf<0 || nSurf>=ACTIVE_SURFS ||
	   nChan<0 || nChan>=CHANNELS_PER_SURF ||
	   nChip<0 || nChip>=LABRADORS_PER_SURF){
	  wxLogError(_T("Bad waveform id; SURF %d CHAN %d CHIP %d @ (%d,%d)"),
		     nSurf,nChan,nChip,i,j);
	  break;
	}
	bool hbwrap=wv[i][j].header.chipIdFlag&0x08;
	char hbextra=(wv[i][j].header.chipIdFlag&0xf0)>>4;
	short hbstart=wv[i][j].header.firstHitbus;
	short hbend=wv[i][j].header.lastHitbus+hbextra;
	// Calibrate ...  
	if(needsCalibration){
	  if(!pedestalsRead && tryPeds){
	    *pedestalFile=_T("latestAcqdPeds.dat");
	    ReadRawPedestals();
	    if(!pedestalsRead) 
	      wxMessageBox(_T("Unable to read default pedestals. Please load manually."),
			   _T("Pedestal loading error"),wxOK|wxICON_ERROR);
	    tryPeds=false;
	  }
	}
	
	for(int k=0;k<MAX_NUMBER_SAMPLES;++k){
	  if(wv[i][j].data[k]==0){
	    calwv[k]=0;
	  }else{
	    if(needsCalibration){
		if(pedestalsRead) {
		    calwv[k]=((wv[i][j].data[k]&0xfff)-pedestalData[nSurf][nChip][nChan][k])*ADCMV;		    
		    if(calwv[i]>SATURATION) calwv[i]=SATURATION;
		    if(calwv[i]<-1*SATURATION) calwv[i]=-1*SATURATION;
		}
		else {
		    calwv[k]=wv[i][j].data[k]&0xfff;	    
		    if(calwv[i]>SATURATION) calwv[i]=SATURATION;
		    if(calwv[i]<-1*SATURATION) calwv[i]=-1*SATURATION;
		}
		    
	    }
	    else {
		calwv[k]=(wv[i][j].data[k]<<1)*ADCMV; // Data bitshifted with ped subtraction by flight CPU  
		if(calwv[i]>SATURATION) calwv[i]=SATURATION;
		if(calwv[i]<-1*SATURATION) calwv[i]=-1*SATURATION;
	    }
		
	  }
	}
	
	// ... and rotate
	int ir=0;
	if(hbwrap){ // Wrapped hitbus
	  for(int k=hbstart+1;k<hbend;++k)
	    v[ir++]=calwv[k];
	}else{
	  for(int k=hbend+1;k<MAX_NUMBER_SAMPLES;++k) 
	    v[ir++]=calwv[k];	
	  for(int k=zeroSCA;k<hbstart;++k)
	    v[ir++]=calwv[k];
	}
	// Fill in remaining bins with zeros
	for(int k=ir;k<MAX_NUMBER_SAMPLES;++k)
	  v[k]=0;

	// Stuff data into RF tabs
	if(rfCurve[nSurf][nChan]) delete rfCurve[nSurf][nChan];
	rfCurve[nSurf][nChan] = new wxGraphCurve(wvtimebase,v,ir);
	rfDisplay[nSurf][nChan]->SetData(rfCurve[nSurf][nChan]); 
	if(nChan==CHANNELS_PER_SURF-1)
	  channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetVoltage()); 
	else{
	  switch(AviewOpts::RFTab){
	  case AviewOpts::PowerTab:
	    channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetPower()); 
	    break;
	  case AviewOpts::FFTTab:
	    channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetFFT()); 
	    break;
	  case AviewOpts::VoltageTab:
	  default:
	    channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetVoltage()); 
	    break;
	  }
	}
	// Set antenna colors
	if(nChan<CHANNELS_PER_SURF-1) 
{
	  payloadView->SetValue(rfCurve[nSurf][nChan],nSurf,nChan);//comment out by peng temporarily.
}
      }
  }else{
    wxLogMessage(_T("[AviewFrame::ReadWaveformFile] Unable to find waveform data of the event %ld"),evnum);
    // Zero existing waveforms
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF;++j){
	// Stuff data into RF tabs
	if(rfCurve[i][j]) delete rfCurve[i][j];
	rfCurve[i][j] = NULL;
	channelTab[i][j]->SetData(NULL);
	rfDisplay[i][j]->SetData(NULL); 
	// Set antenna colors
	if(j<CHANNELS_PER_SURF-1) 
	  payloadView->SetValue(NULL,i,j);
      }
  }
  return;
}

bool AviewFrame::ReadHKCalibPacket(const wxFileName &file){
   // Open housekeeping calibration file
  wxFile calFile(file.GetFullPath().c_str(),wxFile::read);
  if (!calFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open housekeeping calibration file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  if(!m_fileHandler->OpenGZ(calFile)){
    wxMessageBox(_T("Unable to initiate housekeeping calibration file reading."),
		 _T("File opening error"), wxOK|wxICON_ERROR);
    calFile.Close();
    return false;
  }

  HkDataStruct_t hk;
  for(int i=0;i<2;++i){ // Do it twice
    if(!m_fileHandler->ReadGZ(&hk,sizeof(HkDataStruct_t))){
      wxMessageBox(_T("Housekeeping calibration file is not of expected size"),
		   _T("File size error"), wxOK|wxICON_ERROR);
      m_fileHandler->CloseGZ();
      calFile.Close();
      return false;
    }
    if(hk.ip320.code==IP320_CAL){
      if(!hkcal) hkcal=new FullAnalogueStruct_t;
      *hkcal=hk.ip320;
    }else if(hk.ip320.code==IP320_AVZ){
      if(!hkavz) hkavz=new FullAnalogueStruct_t;
      *hkavz=hk.ip320;
    }else{
      wxMessageBox(_T("Non-calibration housekeeping packet in calibration file!"),
		   _T("Packet type error"), wxOK|wxICON_ERROR);
      m_fileHandler->CloseGZ();
      calFile.Close();
      return false;
    }
  }
  ///////////////////////amir's addition july 12 08////////////////
    m_hkcalViewer->HKCalData(hk);
  ///////////////////////////////////////////////////////////
  m_fileHandler->CloseGZ();
  calFile.Close();
  
  return (bool)(hkcal && hkavz);
}


bool AviewFrame::ReadHKPacket(const wxFileName &file,int order){
  // Open housekeeping file
  wxFile hkFile(file.GetFullPath().c_str(),wxFile::read);
  if (!hkFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open housekeeping file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  HkDataStruct_t hk;
  if(!m_fileHandler->ReadFromFile(hkFile,&hk,sizeof(HkDataStruct_t),order)){
    wxMessageBox(_T("Housekeeping file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    hkFile.Close();
    return false;
  }
  hkFile.Close();

  // Either store calibration or calibrate acromag files
  if(hk.ip320.code!=IP320_RAW){
    wxLogError(_T("Calibration packet (code=%x) found in hk packet file"),
	       hk.ip320.code);
    return false;
  }
  if(hkcal==NULL || hkavz==NULL){
    wxString path=file.GetPath();
    wxString name=file.GetName();
    // First try same directory
    name.Replace(_T("raw"),_T("cal_avz"));
    wxFileName calfile;
    calfile.SetPath(path);
    calfile.SetName(name);
    calfile.SetExt(file.GetExt());
    if(!ReadHKCalibPacket(calfile)){ // Try other directory
      path.Replace(_T("raw"),_T("cal"));
      calfile.SetPath(path);
      if(!ReadHKCalibPacket(calfile)){
	wxMessageBox(_T("Housekeeping calibration files not loaded successfully! Try manually."),
		     _T("HK calib error"),wxOK|wxICON_ERROR);
	return false;
      }
    }
  }
  
  float v[NUM_IP320_BOARDS][CHANS_PER_IP320];
  for(int i=0;i<NUM_IP320_BOARDS;++i)
    for(int j=0;j<CHANS_PER_IP320;++j){
      float slope=4.9/(hkcal->board[i].data[j]-hkavz->board[i].data[j]);
      v[i][j]=slope*(hk.ip320.board[i].data[j]-hkavz->board[i].data[j]);
    }

  // Store acromag data; this is about as complicated as it could get...
  // Accelerometer, which needs +5V reference reading
  if(v[1][6]!=0){
    float accx1=((2.5*v[1][30])/v[1][6]-2.493)/0.830;	
    float accy1=((2.5*v[1][10])/v[1][6]-2.512)/0.839;	
    float accz1=((2.5*v[1][29])/v[1][6]-2.506)/0.842;	
    float accx2=((2.5*v[1][28])/v[1][6]-2.523)/0.827;	
    float accy2=((2.5*v[1][8])/v[1][6]-2.523)/0.844;	
    float accz2=((2.5*v[1][27])/v[1][6]-2.523)/0.843;	
    float acct1=44.4*((2.5*v[1][9])/v[1][6]-1.375);	
    float acct2=44.4*((2.5*v[1][7])/v[1][6]-1.375);
    m_AuxInfo->SetAccelerometer(0,accx1,accy1,accz1);
    m_AuxInfo->SetAccelerometer(1,accx2,accy2,accz2);
    m_TempInfo->SetDisplayValue(_T("ac1"),acct1);
    m_TempInfo->SetDisplayValue(_T("ac2"),acct2);
  }
  // Sunsensor 1
  if((v[0][0]+v[0][20])!=0 && (v[0][1]+v[0][21])!=0){
    float ssx1=(v[0][0]-v[0][20])/(v[0][0]+v[0][20]);
    float ssy1=(v[0][1]-v[0][21])/(v[0][1]+v[0][21]);	
    m_AuxInfo->SetSunSensorRaw(0,ssx1,ssy1);
  }
  m_TempInfo->SetDisplayValue(_T("ss 1"),v[0][2]*100-273.15);
  // Sunsensor 2
  if((v[0][22]+v[0][3])!=0 && (v[0][23]+v[0][4])!=0){
    float ssx2=(v[0][22]-v[0][3])/(v[0][22]+v[0][3]);
    float ssy2=(v[0][23]-v[0][4])/(v[0][23]+v[0][4]);	
    m_AuxInfo->SetSunSensorRaw(1,ssx2,ssy2);
  }
  m_TempInfo->SetDisplayValue(_T("ss 2"),v[0][24]*100-273.15);
  // Sunsensor 3
  if((v[0][26]+v[0][7])!=0 && (v[0][27]+v[0][8])!=0){
    float ssx3=(v[0][26]-v[0][7])/(v[0][26]+v[0][7]);
    float ssy3=(v[0][27]-v[0][8])/(v[0][27]+v[0][8]);	
    m_AuxInfo->SetSunSensorRaw(2,ssx3,ssy3);
  }
  m_TempInfo->SetDisplayValue(_T("ss 3"),v[0][28]*100-273.15);
  // Sunsensor 4
  if((v[0][9]+v[0][29])!=0 && (v[0][20]+v[0][30])!=0){
    float ssx4=(v[0][9]-v[0][29])/(v[0][9]+v[0][29]);
    float ssy4=(v[0][10]-v[0][30])/(v[0][10]+v[0][30]);	
    m_AuxInfo->SetSunSensorRaw(3,ssx4,ssy4);
  }
  m_TempInfo->SetDisplayValue(_T("ss 4"),v[0][11]*100-273.15);

  // Pressures
  m_AuxInfo->SetHighPressure((v[2][31]-0.25)*3.75);  
  m_AuxInfo->SetLowPressure(0.00215181+4.0062*v[2][11]
			    -0.0027642*v[2][11]*v[2][11]);
  // Voltages & Currents
  m_VoltageInfo->SetDisplayValue(_T("+1.5V"),v[1][1]);
  m_VoltageInfo->SetDisplayValue(_T("+3.3V"),v[1][5]);
  m_VoltageInfo->SetDisplayValue(_T("+5V"),v[1][6]*2);
  m_VoltageInfo->SetDisplayValue(_T("+5V_M V"),v[1][2]*2);
  m_VoltageInfo->SetDisplayValue(_T("+12V"),v[1][4]*4);
  m_VoltageInfo->SetDisplayValue(_T("+24V"),v[1][36]*10.1377);
  m_VoltageInfo->SetDisplayValue(_T("PV"),v[1][37]*19.252);
  //m_VoltageInfo->SetDisplayValue(_T("PV"),v[1][37]*18.252);
  //m_VoltageInfo->SetDisplayValue(_T("-5V"),v[1][0]*-2);
  m_VoltageInfo->SetDisplayValue(_T("-12V"),v[1][3]*-4);
  m_VoltageInfo->SetDisplayValue(_T("+12V_IP V"),v[1][33]*4);
  m_VoltageInfo->SetDisplayValue(_T("+5.6V_LT V"),v[1][34]*4);

  //m_CurrentInfo->SetDisplayValue(_T("+1.5V"),v[1][20]*0.8);
  m_CurrentInfo->SetDisplayValue(_T("+3.3V"),v[1][24]*20);
  m_CurrentInfo->SetDisplayValue(_T("+5V"),v[1][25]*20);
  m_CurrentInfo->SetDisplayValue(_T("+5V_M I"),v[1][21]*12);
  m_CurrentInfo->SetDisplayValue(_T("+12V"),v[1][23]*0.8);
  m_CurrentInfo->SetDisplayValue(_T("+24V"),v[1][16]*30);
  //m_CurrentInfo->SetDisplayValue(_T("+24V"),v[1][16]*20);
  m_CurrentInfo->SetDisplayValue(_T("PV"),v[1][17]*20);
  //m_CurrentInfo->SetDisplayValue(_T("-5V"),v[1][26]*0.8);
  m_CurrentInfo->SetDisplayValue(_T("-5V V"),v[1][22]*-0.8);
  //m_CurrentInfo->SetDisplayValue(_T("IPRF1"),v[1][14]*8);	
  //m_CurrentInfo->SetDisplayValue(_T("+12V_IP I"),v[1][13]*8);	
  m_CurrentInfo->SetDisplayValue(_T("Battery"),v[1][15]*30);

// Powers
  m_PowerInfo->SetDisplayValue(_T("PV"),v[1][37]*19.252*v[1][17]*20);

  // Temperatures
  m_TempInfo->SetDisplayValue(_T("it01"),v[2][0]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it02"),v[2][20]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it03"),v[2][1]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it04"),v[2][21]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it05"),v[2][2]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it06"),v[2][22]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it07"),v[2][3]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it08"),v[2][23]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it09"),v[2][4]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it10"),v[2][24]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it11"),v[2][5]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it12"),v[2][25]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it13"),v[2][6]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it14"),v[2][26]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it15"),v[2][7]*100-273.15);
/*
  m_TempInfo->SetDisplayValue(_T("it16"),v[2][27]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it17"),v[2][8]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it18"),v[2][28]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it19"),v[2][9]*100-273.15);  
  m_TempInfo->SetDisplayValue(_T("it20"),v[2][29]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it21"),v[2][10]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it22"),v[2][30]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it23"),v[2][11]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it24"),v[2][31]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("it25"),v[2][12]*100-273.15);

*/
  m_TempInfo->SetDisplayValue(_T("et01"),v[2][32]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et02"),v[2][13]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et03"),v[2][33]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et04"),v[2][14]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et05"),v[2][34]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et06"),v[2][15]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et07"),v[2][25]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et08"),v[2][26]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et09"),v[2][36]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et10"),v[2][17]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et11"),v[2][37]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et12"),v[2][18]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et13"),v[2][38]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et14"),v[2][19]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et15"),v[2][39]*100-273.15);
  //m_TempInfo->SetDisplayValue("et16",v[0][15]*100-273.15);
  //m_TempInfo->SetDisplayValue("et17",v[0][35]*100-273.15);
  //m_TempInfo->SetDisplayValue("et18",v[0][16]*100-273.15);
  //m_TempInfo->SetDisplayValue("et19",v[0][36]*100-273.15);
  //m_TempInfo->SetDisplayValue("et20",v[0][17]*100-273.15);
  //m_TempInfo->SetDisplayValue("et21",v[0][37]*100-273.15);
  //m_TempInfo->SetDisplayValue("et22",v[0][18]*100-273.15);
  //m_TempInfo->SetDisplayValue("et23",v[0][38]*100-273.15);
  //m_TempInfo->SetDisplayValue("et24",v[0][19]*100-273.15);
  //m_TempInfo->SetDisplayValue("et25",v[0][39]*100-273.15);

//mcMurdo mapping as of 12/09
/*
  m_TempInfo->SetDisplayValue(_T("et16"),v[0][39]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et17"),v[0][19]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et18"),v[0][38]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et19"),v[0][18]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et20"),v[0][37]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et21"),v[0][17]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et22"),v[0][36]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et23"),v[0][16]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et24"),v[0][35]*100-273.15);
  m_TempInfo->SetDisplayValue(_T("et25"),v[0][15]*100-273.15);
*/

  // Store non-acromag data
  m_flightControl->SetTime(HKEVENT,hk.unixTime);
  m_AuxInfo->SetMagnetometer(hk.mag.x,hk.mag.y,hk.mag.z);
  // for ANITA-2, core temp will also be shown. 
  m_TempInfo->SetDisplayValue(_T("cpu1"),hk.sbs.temp[2]);
  m_TempInfo->SetDisplayValue(_T("cpu2"),hk.sbs.temp[3]);
  m_TempInfo->SetDisplayValue(_T("cr1"),hk.sbs.temp[2]);
  m_TempInfo->SetDisplayValue(_T("cr2"),hk.sbs.temp[3]);
/////////////////////////////////amir's addition july 12 08/////////////////////////
  m_hkViewer->HKData(hk);
////////////////////////////////////////////////////////////////////////////
  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true;
}

bool AviewFrame::ReadMonitorPacket(const wxFileName &file,int order){
  // Open CPU monitor file
  wxFile monFile(file.GetFullPath().c_str(),wxFile::read);
  if (!monFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open CPU monitor file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  MonitorStruct_t monitor;
  if(!m_fileHandler->ReadFromFile(monFile,&monitor,sizeof(MonitorStruct_t),order)){
    wxMessageBox(_T("CPU monitor file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    monFile.Close();
    return false;
  }
  monFile.Close();

  // Fill monitor information
  m_flightControl->SetTime(MONITOR,monitor.unixTime);
  wxString tmp;
  for(int i=0;i<N_DISK;++i){  
    tmp.Printf(_T("disk%d"),i+1);
    //    m_DiskInfo->SetDisplayValue(tmp,monitor.diskInfo.diskSpace[i]*(i==4?16:1)); // 750 GB disk sent /16
    if(i==4 ||i==5) m_x=2;
    else if(i==7) m_x=16;
    else m_x=1;
     	
    m_DiskInfo->SetDisplayValue(tmp,monitor.diskInfo.diskSpace[i]*m_x); 
  }
  //*  m_BladeLabel->SetLabel(_U(monitor.diskInfo.bladeLabel));
  //*  m_USBIntLabel->SetLabel(_U(monitor.diskInfo.usbIntLabel));
  //*  m_USBExtLabel->SetLabel(_U(monitor.diskInfo.usbExtLabel));
  /////////////////////////////amir's addition july 12 08//////////////////////
  m_monitorViewer->MonitorData(monitor);
  ///////////////////////////////////////////////////////////////////////
  for(int i=0;i<NUM_PRIORITIES;++i){
    tmp.Printf(_T("priority%d"),i);
    m_QueueInfo->SetDisplayValue(tmp,monitor.queueInfo.eventLinks[i]);
  }
  //*  m_QueueInfo->SetDisplayValue(_T("command LOS"),monitor.queueInfo.cmdLinksLOS);
  //*  m_QueueInfo->SetDisplayValue(_T("command SIP"),monitor.queueInfo.cmdLinksSIP);
  //*  m_QueueInfo->SetDisplayValue(_T("header"),monitor.queueInfo.headLinks);
  //*  m_QueueInfo->SetDisplayValue(_T("GPS"),monitor.queueInfo.gpsLinks);
 //*   m_QueueInfo->SetDisplayValue(_T("housekeeping"),monitor.queueInfo.hkLinks);//amir's change
  //*  m_QueueInfo->SetDisplayValue(_T("monitor"),monitor.queueInfo.monitorLinks);
  //*  m_QueueInfo->SetDisplayValue(_T("SURF hk"),monitor.queueInfo.surfHkLinks);
  //*  m_QueueInfo->SetDisplayValue(_T("TURF hk"),monitor.queueInfo.turfHkLinks);
  //*  m_QueueInfo->SetDisplayValue(_T("pedestal"),monitor.queueInfo.pedestalLinks);

  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true;
}


bool AviewFrame::ReadADU5PatPacket(const wxFileName &file,int order){
  // Open ADU5Pat GPS file
  wxFile gpsFile(file.GetFullPath().c_str(),wxFile::read);
  if (!gpsFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open GPS ADU5 pat file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  GpsAdu5PatStruct_t pat;
  if(!m_fileHandler->ReadFromFile(gpsFile,&pat,sizeof(GpsAdu5PatStruct_t),order)){
    wxMessageBox(_T("GPS ADU5 pat file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    gpsFile.Close();
    return false;
  }
  gpsFile.Close();

  // Fill GPS information
  m_flightControl->SetTime(ADU5PAT,pat.unixTime);
  if ((pat.gHdr.code & 0x60000)>>17 == 1) 
    m_GPSDataInfo->SetADU5PatData(pat);
  else 
    m_GPSDataInfo->SetADU5PatData_U2(pat);
  m_HeaderInfo->SetOrientation(pat.heading,pat.pitch,pat.roll);
  ///////////////////////amir's addition july 12 08 ////////////////////////////
   m_adu5patViewer->ADU5PatData(pat);
  /////////////////////////////////////////////////////////////////////////
  if(AviewOpts::DefaultGPS==AviewOpts::ADU5){
    m_HeaderInfo->SetLocation(pat.latitude,pat.longitude,pat.altitude/1e3); // Convert alt to km from ft
    //m_HeaderInfo->SetSpeed(-1,-1); // No speed/course info in this packet
    m_HeaderInfo->SetAttFlag(pat.attFlag);
  }
  payloadView->SetOrientation(pat.heading);

  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true; 
}

bool AviewFrame::ReadADU5VtgPacket(const wxFileName &file,int order){
  // Open ADU5Vtg GPS file
  wxFile gpsFile(file.GetFullPath().c_str(),wxFile::read);
  if (!gpsFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open GPS ADU5 vtg file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  GpsAdu5VtgStruct_t vtg;
  if(!m_fileHandler->ReadFromFile(gpsFile,&vtg,sizeof(GpsAdu5VtgStruct_t),order)){
    wxMessageBox(_T("GPS ADU5 vtg file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    gpsFile.Close();
    return false;
  }
  gpsFile.Close();

  // Fill GPS information
  m_flightControl->SetTime(ADU5VTG,vtg.unixTime);
  if ((vtg.gHdr.code & 0x60000)>>17 == 1) 
    m_GPSDataInfo->SetADU5VtgData(vtg);
  else 
    m_GPSDataInfo->SetADU5VtgData_U2(vtg);
  //////////////////////////////amir's addition july 12 08/////////////////////
   m_adu5vtgViewer->ADU5VtgData(vtg);
  ////////////////////////////////////////////////////////////////////////////
  if(AviewOpts::DefaultGPS==AviewOpts::ADU5)
    m_HeaderInfo->SetSpeed(vtg.speedInKnots,vtg.trueCourse); 

  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true; 
}

bool AviewFrame::ReadG12PosPacket(const wxFileName &file,int order){
  // Open G12Pos GPS file
  wxFile gpsFile(file.GetFullPath().c_str(),wxFile::read);
  if (!gpsFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open GPS G12 pos file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  GpsG12PosStruct_t pos;
  if(!m_fileHandler->ReadFromFile(gpsFile,&pos,sizeof(GpsG12PosStruct_t),order)){
    wxMessageBox(_T("GPS G12 pos file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    gpsFile.Close();
    return false;
  }
  gpsFile.Close();

  // Fill GPS information
  m_flightControl->SetTime(G12POS,pos.unixTime);
  m_GPSDataInfo->SetG12Data(pos);
  ////////////////////////////amir's addition july 12 08//////////////////////////
//  m_gpsposViewer->GpsPosData(pos);
  ///////////////////////////////////////////////////////////
  if(AviewOpts::DefaultGPS==AviewOpts::G12){
    m_HeaderInfo->SetLocation(pos.latitude,pos.longitude,pos.altitude/1e3); // Convert altitude to km from m
    //m_HeaderInfo->SetOrientation(0,0,0); // No orientation info in this packet
    m_HeaderInfo->SetSpeed(pos.speedInKnots,pos.trueCourse); 
  }

  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true; 
}

bool AviewFrame::ReadADU5SatPacket(const wxFileName &file,int order){
  // Open ADU5Sat GPS file
  wxFile gpsFile(file.GetFullPath().c_str(),wxFile::read);
  if (!gpsFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open GPS ADU5 sat file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  GpsAdu5SatStruct_t sat;
  if(!m_fileHandler->ReadFromFile(gpsFile,&sat,sizeof(GpsAdu5SatStruct_t),order)){
    wxMessageBox(_T("GPS ADU5 sat file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    gpsFile.Close();
    return false;
  }
  gpsFile.Close();

  // Fill GPS information
  m_flightControl->SetTime(ADU5SAT,sat.unixTime);
  if ((sat.gHdr.code & 0x60000)>>17 == 1) 
    m_GPSSatInfo->SetADU5Sat(sat);
  else 
    m_GPSSatInfo->SetADU5Sat_U2(sat);
///////////////////////////amir's addition july 12 08////////////////////////
  m_adu5satViewer->ADU5SatData(sat);
///////////////////////////////////////////////////////////////////////
  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true; 
}

bool AviewFrame::ReadG12SatPacket(const wxFileName &file,int order){
  // Open G12Sat GPS file
  wxFile gpsFile(file.GetFullPath().c_str(),wxFile::read);
  if (!gpsFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open GPS G12 sat file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  GpsG12SatStruct_t sat;
  if(!m_fileHandler->ReadFromFile(gpsFile,&sat,sizeof(GpsG12SatStruct_t),order)){
    wxMessageBox(_T("GPS G12 sat file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    gpsFile.Close();
    return false;
  }
  gpsFile.Close();

  // Fill GPS information
  m_flightControl->SetTime(G12SAT,sat.unixTime);
  m_GPSSatInfo->SetG12Sat(sat);
/////////////////////////////amir's addition july 12 08//////////////////////////
 m_g12satViewer->G12SatData(sat);
///////////////////////////////////////////////////////////////////////////
  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true; 
}

bool AviewFrame::ReadCmdEchoPacket(const wxFileName &file,int order){
  // Open command echo file
  wxFile cmdFile(file.GetFullPath().c_str(),wxFile::read);
  if (!cmdFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open command echo file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  CommandEcho_t cmd;
  if(!m_fileHandler->ReadFromFile(cmdFile,&cmd,sizeof(CommandEcho_t),order)){
    wxMessageBox(_T("Command echo file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    cmdFile.Close();
    return false;
  }
  cmdFile.Close();

  // Fill command echo information
  m_flightControl->SetTime(CMDECHO,cmd.unixTime);
  m_HeaderInfo->SetCommand(cmd.cmd,cmd.numCmdBytes);
  m_HeaderInfo->SetCommandGood(cmd.goodFlag);
///////////////////////////amir's addtion july 12 08 /////////////////////////
  m_commandViewer->CommandData(cmd);
//////////////////////////////////////////////////////////////////////////
  wxString status=file.GetFullName();
  status << _T(" ") << order+1;
  SetStatusText(status,1);
  return true;
}

void AviewFrame::IndexPacketDirectory(int type,bool full){
  if(!packetDirectory[type].IsOk()) return;
  // Index current directory
  wxDir dir;
  if(full){  // Full indexing
    packetFileList[type]->Empty();
    dir.GetAllFiles(packetDirectory[type].GetPath(),packetFileList[type],packetFilter[type],wxDIR_FILES);
    if(packetFileList[type]->GetCount()){
      packetFileList[type]->Sort(packetSortFunction[type]);
      m_flightControl->SetList(type,packetFileList[type],(type==RFEVENT || type==HEADER)?N_RF_PER_FILE:N_PACKET_PER_FILE);
      if(type==RFEVENT || type==HEADER){
	m_pFlightDataNextRF->Enable(true);
	m_pFlightDataPrevRF->Enable(true);
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Header packet files indexed."));
      }else if(type==HKEVENT){
	m_pFlightDataNextHK->Enable(true);
	m_pFlightDataPrevHK->Enable(true);
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Housekeeping packet files indexed."));
      }else if(type==SSHKEVENT){
	m_pFlightDataNextHK->Enable(true);
	m_pFlightDataPrevHK->Enable(true);
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] SS Housekeeping packet files indexed."));
      }else if(type==MONITOR){
	m_pFlightDataNextMonitor->Enable(true);
	m_pFlightDataPrevMonitor->Enable(true);
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] CPU monitor packet files indexed."));
      }else if((type==ADU5PAT && AviewOpts::DefaultGPS==AviewOpts::ADU5)||(type==G12POS && AviewOpts::DefaultGPS==AviewOpts::G12)){
	m_pFlightDataNextGPS->Enable(true);
	m_pFlightDataPrevGPS->Enable(true);
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] GPS packet files indexed."));
      }else if(type==CMDECHO){
	m_pFlightDataNextCmd->Enable(true);
	m_pFlightDataPrevCmd->Enable(true);
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Command echo packet files indexed."));
      }
    }else{
      if(type==RFEVENT || type==HEADER){
	m_pFlightDataNextRF->Enable(false);
	m_pFlightDataPrevRF->Enable(false);
      }else if(type==HKEVENT){
	m_pFlightDataNextHK->Enable(false);
	m_pFlightDataPrevHK->Enable(false);
      }else if(type==SSHKEVENT){
	m_pFlightDataNextHK->Enable(false);
	m_pFlightDataPrevHK->Enable(false);
      }else if(type==MONITOR){
	m_pFlightDataNextMonitor->Enable(false);
	m_pFlightDataPrevMonitor->Enable(false);
      }else if((type==ADU5PAT && AviewOpts::DefaultGPS==AviewOpts::ADU5)||(type==G12POS && AviewOpts::DefaultGPS==AviewOpts::G12)){
	m_pFlightDataNextGPS->Enable(false);
	m_pFlightDataPrevGPS->Enable(false);
      }else if(type==CMDECHO){
	m_pFlightDataNextCmd->Enable(false);
	m_pFlightDataPrevCmd->Enable(false);
      }
    }
  }else{  // Update indexing

    wxArrayString newPacketFiles;
    dir.GetAllFiles(packetDirectory[type].GetPath(),&newPacketFiles,packetFilter[type],wxDIR_FILES);
    if(newPacketFiles.GetCount()>packetFileList[type]->GetCount()){
      newPacketFiles.Sort(packetSortFunction[type]);
      for(unsigned int i=packetFileList[type]->GetCount();i<newPacketFiles.GetCount();++i)
	packetFileList[type]->Add(newPacketFiles[i]);
      if(type==RFEVENT || type==HEADER){
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Updated list of header packet files."));
      }else if(type==HKEVENT){
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Updated list of housekeeping packet files."));
      }else if(type==SSHKEVENT){
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Updated list of SS housekeeping packet files."));
      }else if(type==MONITOR){
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Updated list of CPU monitor packet files."));
      }else if((type==ADU5PAT && AviewOpts::DefaultGPS==AviewOpts::ADU5)||(type==G12POS && AviewOpts::DefaultGPS==AviewOpts::G12)){
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Updated list of GPS packet files."));
      }else if(type==CMDECHO){
	wxLogMessage(_T("[AviewFrame::IndexPacketDirectory] Updated list of command echo packet files."));
      }
    }
  }
  return;
}

void AviewFrame::sqlParser(PGresult *result){
  if(!sqlRequestType.GetCount())
    wxLogError(_T("[AviewFrame::sqlParser] Received SQL query result, but no request types in the buffer."));
  else{
    int type=sqlRequestType[0];
    sqlRequestType.RemoveAt(0);
    ExecStatusType status=PQresultStatus(result);
    if(status==PGRES_COMMAND_OK){ 
      //wxLogMessage("[AviewFrame::sqlParser] OK");
    }else if(status!=PGRES_TUPLES_OK){
      wxLogError(_U(PQresStatus(status)));
      wxLogError(_U(PQresultErrorMessage(result)));
    }else if(PQntuples(result)==0 && type<INDEX_OFFSET){
      wxLogError(_T("[AviewFrame::sqlParser] Received SQL query result w/o record; type %d"),type);
    }else{
      //wxLogMessage("[AviewFrame::sqlParser] Received SQL data type %d",type);
      switch(type){
      case RFEVENT: ParseHeader(result,true); break;
      case HEADER: ParseHeader(result,false); Refresh();  break;
      case HKEVENT: ParseHKPacket(result); Refresh(); break;
      case SSHKEVENT: ParseSSHKPacket(result); Refresh(); break;
      case MONITOR: ParseMonitorPacket(result); Refresh(); break;
      case ADU5PAT: ParseADU5PatPacket(result); Refresh();break;
      case ADU5VTG: ParseADU5VtgPacket(result); Refresh();break;
      case ADU5SAT: ParseADU5SatPacket(result); break;
      case G12POS: ParseG12PosPacket(result); Refresh();break;
      case G12SAT: ParseG12SatPacket(result); break;
      case CMDECHO: ParseCmdEchoPacket(result); Refresh();break;
      case WVFORM: ParseWaveformPacket(result); Refresh(); break;
      case EVENTRATE: ParseEventRate(result); Refresh(); break;
      case RFEVENT+INDEX_OFFSET: 
      case HEADER+INDEX_OFFSET: 
      case HKEVENT+INDEX_OFFSET: 
      case SSHKEVENT+INDEX_OFFSET: 
      case ADU5PAT+INDEX_OFFSET: 
      case ADU5VTG+INDEX_OFFSET: 
      case G12POS+INDEX_OFFSET: 
	StorePacketList(result,type-INDEX_OFFSET,true);
	if(showProgress){
	  wxString status;
	  status.Printf(_T("DB indexing %d%% complete"),(type-INDEX_OFFSET+1)*100/N_PCKTYPE);
	  SetStatusText(status,1);
	}
	break;
      case CMDECHO+INDEX_OFFSET:
	if(initialDBIndexing) initialDBIndexing=false;  // Mark that we are ready to receive updates
      case ADU5SAT+INDEX_OFFSET:
      case MONITOR+INDEX_OFFSET:
      case G12SAT+INDEX_OFFSET:
	StorePacketList(result,type-INDEX_OFFSET,false);
	if(showProgress){
	  wxString status;
	  status.Printf(_T("DB indexing %d%% complete"),(type-INDEX_OFFSET)*10);
	  SetStatusText(status,1);
	}
	break;
      case DBMREF:
	ParseDBMRef(result); break;
      case WVAVG:
	ParseWaveformAverage(result); break;
      default:
	wxLogError(_T("[AviewFrame::sqlParser] Received results of unknown SQL request type %d"),type);
	break;
      }
    }
  }
  PQclear(result);
  return;
}

void AviewFrame::ParseHeader(PGresult *result,bool wvin){
  int n=PQntuples(result);
  int coln;
  DBHeader_t dbheader;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseHeader] More than one header received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseHeader] No event header received."));
    return;
  }

  unsigned long evnum=0;
  if((coln=PQfnumber(result,"evnum"))>=0) evnum=atol(PQgetvalue(result,0,coln));

 // Firsy request all available waveforms at once and reset current RF display
  if(wvin){
    sqlRequestType.Add(WVFORM);
    wxString cmd;
    cmd.Printf(_T("SELECT evnum,crc,id,hbwrap,hbstart,hbend,cal FROM wv WHERE evnum=%ld;"),evnum);
    wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
    sqlEvent.SetEventObject( this );
    sqlEvent.SetString(cmd);
    sqlEvent.SetClientData((void*)parseWrapper);
    ProcessEvent(sqlEvent);
  }

  // Clear existing waveforms
  for(int i=0;i<ACTIVE_SURFS;++i)
    for(int j=0;j<CHANNELS_PER_SURF;++j){
      // Clear RF displays
      if(rfCurve[i][j]) delete rfCurve[i][j];
      rfCurve[i][j] = NULL;
      rfDisplay[i][j]->SetData(NULL); 
      channelTab[i][j]->SetData(NULL);
      // Set antenna colors
      if(j<CHANNELS_PER_SURF-1) 
	payloadView->SetValue(NULL,i,j);
    }
  Refresh();
  
  // Parse and display the rest of information
  unsigned long time=0,us=0;
  if((coln=PQfnumber(result,"time"))>=0) time=atol(PQgetvalue(result,0,coln));
  dbheader.time=time;
  m_HeaderInfo->SetEventTime(time);
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  dbheader.crc=crc;
  if(wvin) m_dbControl->SetTime(RFEVENT,time,!(crc&0x1));
  //  else m_dbControl->SetTime(HEADER,time,!(crc&0x1));
  m_dbControl->SetTime(HEADER,time,!(crc&0x1));
  if((coln=PQfnumber(result,"us"))>=0) us=atol(PQgetvalue(result,0,coln));
  m_HeaderInfo->SetEventUSTime(us);
  dbheader.us=us;
  if((coln=PQfnumber(result,"ns"))>=0) {m_HeaderInfo->SetEventNSTime(atol(PQgetvalue(result,0,coln)));
  	dbheader.ns=atol(PQgetvalue(result,0,coln));}
  m_HeaderInfo->SetEventNumber(evnum);
  dbheader.evnum=evnum;
  //  if((coln=PQfnumber(result,"runnum"))>=0) {m_HeaderInfo->SetRunNumber(atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"evid"))>=0) {m_HeaderInfo->SetRunNumber((atol(PQgetvalue(result,0,coln))&0xfff00000)>>20);
  	dbheader.runnum=atol(PQgetvalue(result,0,coln));}
  else m_HeaderInfo->SetRunNumber(0);
//  if((coln=PQfnumber(result,"antmask"))>=0) {m_HeaderInfo->SetAntennaMask(atol(PQgetvalue(result,0,coln)));
  //	dbheader.antmask=atol(PQgetvalue(result,0,coln));}
  //  unsigned short mask[ACTIVE_SURFS] ;
  //  if((coln=PQfnumber(result,"surfmask"))>=0 && ParseLongSQLArray(result,(long *)mask,ACTIVE_SURFS,coln)) {m_TriggerInfo->SetSurfMask(mask);}
  //  dbheader.surfmask=atoi(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"calib"))>=0) {m_TriggerInfo->SetCalibStatus(atoi(PQgetvalue(result,0,coln)));
  	dbheader.calib=atoi(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"priority"))>=0) {m_TriggerInfo->SetPriority(atoi(PQgetvalue(result,0,coln)));
  	dbheader.priority=atoi(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"turfword"))>=0) {m_TriggerInfo->SetTurfMonitor(atoi(PQgetvalue(result,0,coln)));
  	dbheader.turfword=atoi(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"trigtype"))>=0) {m_TriggerInfo->SetTriggerType(atoi(PQgetvalue(result,0,coln)));
  	dbheader.trigtype=atoi(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"trignum"))>=0) {m_TriggerInfo->SetTriggerNumber(atoi(PQgetvalue(result,0,coln)),0);
  	dbheader.trignum=atoi(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"l3cnt"))>=0) {m_TriggerInfo->SetL3Number(atoi(PQgetvalue(result,0,coln)));
  	dbheader.l3cnt=atoi(PQgetvalue(result,0,coln));}
  unsigned long c3po=0;
  if((coln=PQfnumber(result,"c3po"))>=0) c3po=atol(PQgetvalue(result,0,coln));
  	dbheader.c3po=atol(PQgetvalue(result,0,coln));
    if((coln=PQfnumber(result,"trigtime"))>=0) {m_TriggerInfo->SetTriggerTimeNS(atol(PQgetvalue(result,0,coln)),c3po);
    	dbheader.trigtime=atol(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"pps"))>=0){ m_TriggerInfo->SetPPS(atol(PQgetvalue(result,0,coln)));
  	dbheader.pps=atol(PQgetvalue(result,0,coln));}
  //if((coln=PQfnumber(result,"deadtime"))>=0) m_TriggerInfo->SetDeadtime(atol(PQgetvalue(result,0,coln))/64400.);
  //if((coln=PQfnumber(result,"deadtime"))>=0) {m_TriggerInfo->SetDeadtime(atol(PQgetvalue(result,0,coln))/65535.);
  //	dbheader.deadtime=(long)(atol(PQgetvalue(result,0,coln))/65535.);}

  // Set tab & antenna frame color based on trigger participation
  unsigned short l1TrigMask=0;
  if((coln=PQfnumber(result,"l1mask"))>=0) l1TrigMask=atol(PQgetvalue(result,0,coln));
  unsigned short l1TrigMaskH=0;
  if((coln=PQfnumber(result,"l1maskh"))>=0) l1TrigMaskH=atol(PQgetvalue(result,0,coln));
  unsigned short l3TrigPattern=0;
  if((coln=PQfnumber(result,"l3trigpat"))>=0) l3TrigPattern=atol(PQgetvalue(result,0,coln));
  dbheader.l3trigpat=l3TrigPattern;
  unsigned short l3TrigPatternH=0;
  if((coln=PQfnumber(result,"l3trigpath"))>=0) l3TrigPatternH=atol(PQgetvalue(result,0,coln));
  dbheader.l3trigpatH=l3TrigPatternH;
  unsigned short phiTrigMask=0;
  if((coln=PQfnumber(result,"phimask"))>=0) phiTrigMask=atol(PQgetvalue(result,0,coln));
  dbheader.phimask=phiTrigMask;
  unsigned short phiTrigMaskH=0;
  if((coln=PQfnumber(result,"phimaskh"))>=0) phiTrigMaskH=atol(PQgetvalue(result,0,coln));
  dbheader.phimaskh=phiTrigMaskH;
  SetTriggerPattern(l1TrigMask,l1TrigMaskH,l3TrigPattern,l3TrigPatternH,phiTrigMask,phiTrigMaskH);
  m_trigPatternInfo->SetPattern(l3TrigPattern, l3TrigPatternH);

  // Show new tabs if in payload view
  if(AviewOpts::RFOverview==AviewOpts::PayloadView) SwitchRFView(true);

  // Update control panel index if needed
  if(updateControlPanelIndex){
    dbRef ref={time,us,0};
    m_dbControl->SetIndex(RFEVENT,(void*)&ref);
    updateControlPanelIndex=false;
  }
  /////////////////////////////////amir's addition july 12 08/////////////////////////
   m_dbheaderViewer->DB_HeaderData(dbheader);
////////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("HD %ld"),evnum);
  SetStatusText(status,1);

  return;
}

void AviewFrame::ParseHKPacket(PGresult *result){
  int n=PQntuples(result);
  int coln,coln2;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseHKEvent] More than one hk record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseHKPacket] No HK record received."));
    return;
  }
 DBHK_t dbhk;
  // Accelerometers
  double accx[2],accy[2],accz[2],acct[2];
  if((coln=PQfnumber(result,"accx"))>=0 && ParseDoubleSQLArray(result,accx,2,coln) &&
     (coln=PQfnumber(result,"accy"))>=0 && ParseDoubleSQLArray(result,accy,2,coln) &&
     (coln=PQfnumber(result,"accz"))>=0 && ParseDoubleSQLArray(result,accz,2,coln)){
    m_AuxInfo->SetAccelerometer(0,accx[0],accy[0],accz[0]);
    m_AuxInfo->SetAccelerometer(1,accx[1],accy[1],accz[1]);
  }
  dbhk.accx[0]=accx[0];dbhk.accx[1]=accx[1];
  dbhk.accy[0]=accy[0];dbhk.accy[1]=accy[1];
  dbhk.accz[0]=accz[0];dbhk.accz[1]=accz[1];
  if((coln=PQfnumber(result,"acct"))>=0 && ParseDoubleSQLArray(result,acct,2,coln)){
    m_TempInfo->SetDisplayValue(_T("ac1"),acct[0]);
    m_TempInfo->SetDisplayValue(_T("ac2"),acct[1]);
  }
  dbhk.acct[0]=acct[0];dbhk.acct[1]=acct[1];
  // Sunsensors
  double ssel[4],ssaz[4],sst[4];
  long ssflag[4];
  if((coln=PQfnumber(result,"ssel"))>=0 && ParseDoubleSQLArray(result,ssel,4,coln) &&
     (coln=PQfnumber(result,"ssaz"))>=0 && ParseDoubleSQLArray(result,ssaz,4,coln) &&
     (coln=PQfnumber(result,"ssflag"))>=0 && ParseLongSQLArray(result,ssflag,4,coln)){
    for(int i=0;i<4;++i){
      m_AuxInfo->SetSunSensor(i,ssel[i],ssaz[i],ssflag[i]);
	}
  }
  if((coln=PQfnumber(result,"sst"))>=0 && ParseDoubleSQLArray(result,sst,4,coln)){
    wxString tmp;
    for(int i=0;i<4;++i){
      tmp.Printf(_T("ss %d"),i+1);
      m_TempInfo->SetDisplayValue(tmp,sst[i]);
    }
  }
  ////////////////////////////////////////////////////
 for(int ii=0;ii<4;ii++){
  dbhk.ssel[ii]=ssel[ii];
  dbhk.ssaz[ii]=ssaz[ii];
  dbhk.sst[ii]=sst[ii];
  dbhk.ssflag[ii]=ssflag[ii];
  	}
 //////////////////////////////////////////////////////////////////////////
  // Pressures
  if((coln=PQfnumber(result,"pressh"))>=0){ m_AuxInfo->SetHighPressure(atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.pressh=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"pressl"))>=0) {m_AuxInfo->SetLowPressure(atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
	dbhk.pressl=atof(PQgetvalue(result,0,coln));}
  // Voltages & Currents
  if((coln=PQfnumber(result,"p1_5v"))>=0) {m_VoltageInfo->SetDisplayValue(_T("+1.5V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.p1_5v=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p3_3v"))>=0) {m_VoltageInfo->SetDisplayValue(_T("+3.3V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.p3_3v=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p5v"))>=0) {m_VoltageInfo->SetDisplayValue(_T("+5V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.p5v= atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p5sbv"))>=0) {m_VoltageInfo->SetDisplayValue(_T("+5V_M V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.p5sbv=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p12v"))>=0) {m_VoltageInfo->SetDisplayValue(_T("+12V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.p12v= atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p24v"))>=0){ m_VoltageInfo->SetDisplayValue(_T("+24V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.p24v= atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"ppvv"))>=0){ m_VoltageInfo->SetDisplayValue(_T("PV"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.ppvv= atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"n12v"))>=0) {m_VoltageInfo->SetDisplayValue(_T("-12V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.n12v= atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"iprf1v"))>=0) {m_VoltageInfo->SetDisplayValue(_T("+12V_IP V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  	dbhk.iprf1v= atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"iprf2v"))>=0) {m_VoltageInfo->SetDisplayValue(_T("+5.6V_LT V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
 	dbhk.iprf2v= atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p3_3i"))>=0) {m_CurrentInfo->SetDisplayValue(_T("+3.3V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.p3_3i=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p5i"))>=0) {m_CurrentInfo->SetDisplayValue(_T("+5V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.p5i=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p5sbi"))>=0) {m_CurrentInfo->SetDisplayValue(_T("+5V_M I"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.p5sbi=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p12i"))>=0) {m_CurrentInfo->SetDisplayValue(_T("+12V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.p12i=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"p24i"))>=0) {m_CurrentInfo->SetDisplayValue(_T("+24V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.p24i=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"ppvi"))>=0) {m_CurrentInfo->SetDisplayValue(_T("PV"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.ppvi=atof(PQgetvalue(result,0,coln));}
  //if((coln=PQfnumber(result,"5vip"))>=0) {m_CurrentInfo->SetDisplayValue(_T("+5V_IP"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  //dbhk.n5i=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"n12i"))>=0) {m_CurrentInfo->SetDisplayValue(_T("-5V V"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.n12i=(-1)*atof(PQgetvalue(result,0,coln));}
 // if((coln=PQfnumber(result,"iprf1i"))>=0) {m_CurrentInfo->SetDisplayValue(_T("IPRF1"),atof(PQgetvalue(result,0,coln)));
  //dbhk.iprf1i=atof(PQgetvalue(result,0,coln));}

//  if((coln=PQfnumber(result,"iprf2i"))>=0) {m_CurrentInfo->SetDisplayValue(_T("+12V_IP I"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
//  dbhk.iprf2i=atof(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"bati"))>=0) {m_CurrentInfo->SetDisplayValue(_T("Battery"),atof(PQgetisnull(result,0,coln)?"-999":PQgetvalue(result,0,coln)));
  dbhk.bati=atof(PQgetvalue(result,0,coln));}

// Powers
  if((coln=PQfnumber(result,"ppvv"))>=0 && (coln2=PQfnumber(result,"ppvi"))>=0) 
	m_PowerInfo->SetDisplayValue(_T("PV"),(PQgetisnull(result,0,coln)&&PQgetisnull(result,0,coln2)?atof("-999"):atof(PQgetvalue(result,0,coln))*atof(PQgetvalue(result,0,coln2))));

  // Temperatures
  double it[15],et[15];
  wxString tmp;
  if((coln=PQfnumber(result,"it"))>=0 && ParseDoubleSQLArray(result,it,25,coln))
    for(int i=0;i<15;++i){  // Display only first 15, for now
      tmp.Printf(_T("it%02d"),i+1);
      m_TempInfo->SetDisplayValue(tmp,it[i]);
    }
  
  if((coln=PQfnumber(result,"et"))>=0 && ParseDoubleSQLArray(result,et,25,coln))
    for(int i=0;i<15;++i){
      tmp.Printf(_T("et%02d"),i+1);
      m_TempInfo->SetDisplayValue(tmp,et[i]);
    }
for(int ii=0;ii<15;ii++){
  dbhk.it[ii]=it[ii];
  dbhk.et[ii]=et[ii];
  	}
  // Store non-acromag data
  long time=0;
  if((coln=PQfnumber(result,"time"))>=0) time=atol(PQgetvalue(result,0,coln));
  dbhk.time=time;
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  dbhk.crc=crc;
  m_dbControl->SetTime(HKEVENT,time,!(crc&0x1));
  float magx=0,magy=0,magz=0;
  if((coln=PQfnumber(result,"magx"))>=0) magx=atof(PQgetvalue(result,0,coln));
  dbhk.magx=magx;
  if((coln=PQfnumber(result,"magy"))>=0) magy=atof(PQgetvalue(result,0,coln));
  dbhk.magy=magy;
  if((coln=PQfnumber(result,"magz"))>=0) magz=atof(PQgetvalue(result,0,coln));
  dbhk.magz=magz;
  m_AuxInfo->SetMagnetometer(magx,magy,magz);
/// database contain some factor for sbs temps. Here I recovert them.  //peng.
  if((coln=PQfnumber(result,"sbst1"))>=0) {m_TempInfo->SetDisplayValue(_T("cpu1"),atol(PQgetvalue(result,0,coln))*0.1);
  dbhk.cpu1=atol(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"sbst2"))>=0) {m_TempInfo->SetDisplayValue(_T("cpu2"),atol(PQgetvalue(result,0,coln))*0.1);
  dbhk.cpu2=atol(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"core1"))>=0) {m_TempInfo->SetDisplayValue(_T("cr1"),atol(PQgetvalue(result,0,coln))*0.1);
  dbhk.core1=atol(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"core2"))>=0) {m_TempInfo->SetDisplayValue(_T("cr2"),atol(PQgetvalue(result,0,coln))*0.1);
  dbhk.core2=atol(PQgetvalue(result,0,coln));}
/*

  if((coln=PQfnumber(result,"sbst5"))>=0) {m_TempInfo->SetDisplayValue(_T("cpu5"),atol(PQgetvalue(result,0,coln))&0x1f);
  dbhk.cpu5=atol(PQgetvalue(result,0,coln));}
  if((coln=PQfnumber(result,"sbst6"))>=0) {m_TempInfo->SetDisplayValue(_T("cpu6"),(atol(PQgetvalue(result,0,coln))&0x3e0>>5)*2);
  dbhk.cpu6=atol(PQgetvalue(result,0,coln));}
*/
  if((coln=PQfnumber(result,"sbst5"))>=0) {
    m_TempInfo->SetDisplayValue(_T("disk0"),(atol(PQgetvalue(result,0,coln))&0x1f)*2);     
    dbhk.disk0=(atol(PQgetvalue(result,0,coln))&0x1f)*2;
    m_TempInfo->SetDisplayValue(_T("disk1"),(atol(PQgetvalue(result,0,coln))&0x3e0>>5)*2); 
    dbhk.disk1=(atol(PQgetvalue(result,0,coln))&0x3e0>>5)*2;
    m_TempInfo->SetDisplayValue(_T("disk2"),(atol(PQgetvalue(result,0,coln))&0x7c00>>10)*2);
    dbhk.disk2=(atol(PQgetvalue(result,0,coln))&0x7c00>>10)*2;

  }
  if((coln=PQfnumber(result,"sbst6"))>=0) {
    m_TempInfo->SetDisplayValue(_T("disk3"),(atol(PQgetvalue(result,0,coln))&0x1f)*2);     
    dbhk.disk3=(atol(PQgetvalue(result,0,coln))&0x1f)*2;
    m_TempInfo->SetDisplayValue(_T("disk4"),(atol(PQgetvalue(result,0,coln))&0x3e0>>5)*2); 
    dbhk.disk4=(atol(PQgetvalue(result,0,coln))&0x3e0>>5)*2;
    m_TempInfo->SetDisplayValue(_T("disk5"),(atol(PQgetvalue(result,0,coln))&0x7c00>>10)*2);
    dbhk.disk5=(atol(PQgetvalue(result,0,coln))&0x7c00>>10)*2;

  }
  /////////////////////////////////amir's addition july 12 08/////////////////////////
   m_dbhkViewer->DB_HKData(dbhk);
////////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("HK %ld"),time);
  SetStatusText(status,1);
  return;
}

void AviewFrame::ParseSSHKPacket(PGresult *result){
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseSSHKEvent] More than one SS hk record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseSSHKPacket] No SS HK record received."));
    return;
  }
 DBHK_t dbhkss;
  // Sunsensors
  double ssel[4],ssaz[4],sst[4];
  long ssflag[4];
  if((coln=PQfnumber(result,"ssel"))>=0 && ParseDoubleSQLArray(result,ssel,4,coln) &&
     (coln=PQfnumber(result,"ssaz"))>=0 && ParseDoubleSQLArray(result,ssaz,4,coln) &&
     (coln=PQfnumber(result,"ssflag"))>=0 && ParseLongSQLArray(result,ssflag,4,coln)){
    for(int i=0;i<4;++i){
      m_AuxInfo->SetSunSensor(i+4,ssel[i],ssaz[i],ssflag[i]);
}
  }
  if((coln=PQfnumber(result,"sst"))>=0 && ParseDoubleSQLArray(result,sst,4,coln)){
    wxString tmp;
    for(int i=0;i<4;++i){
      tmp.Printf(_T("ss %d"),i+5);
      m_TempInfo->SetDisplayValue(tmp,sst[i]);
    }
  }
  ////////////////////////////////////////////////////
 for(int ii=0;ii<4;ii++){
  dbhkss.ssel[ii]=ssel[ii];
  dbhkss.ssaz[ii]=ssaz[ii];
  dbhkss.sst[ii]=sst[ii];
  dbhkss.ssflag[ii]=ssflag[ii];
  	}
// Store non-acromag data
  long time=0;
  if((coln=PQfnumber(result,"time"))>=0) time=atol(PQgetvalue(result,0,coln));
  dbhkss.time=time;
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  dbhkss.crc=crc;
  m_dbControl->SetTime(SSHKEVENT,time,!(crc&0x1));

  /////////////////////////////////amir's addition july 12 08/////////////////////////
  // m_dbhkViewer->DB_HKData(dbhk);
////////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("SSHK %ld"),time);
  SetStatusText(status,1);
  return;
}
void AviewFrame::ParseMonitorPacket(PGresult *result){

  int n=PQntuples(result);
  int coln;
  DBMon_t dbmon;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseMonitorPacket] More than one cpu monitor record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseMonitorPacket] No cpu monitor record received."));
    return;
  }
  
  long time=0;
  if((coln=PQfnumber(result,"time"))>=0) time=atol(PQgetvalue(result,0,coln));
  dbmon.time=time;
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  dbmon.crc=int(crc);
  m_dbControl->SetTime(MONITOR,time,!(crc&0x1));
  wxString tmp;
  long disk[N_DISK];
  long scaleFactor=0;
  if((coln=PQfnumber(result,"disk"))>=0 && ParseLongSQLArray(result,disk,N_DISK,coln))
    for(int i=0;i<N_DISK;++i){   
      tmp.Printf(_T("disk%d"),i+1);
      // m_DiskInfo->SetDisplayValue(tmp,disk[i]*(i==4?16:1));  // 750 GB disk sent /16
      if(i==4 ||i==5) scaleFactor=128;
      else if(i==6) scaleFactor=4;
      else if(i==7) scaleFactor=4;
      else scaleFactor=1;
     	
     m_DiskInfo->SetDisplayValue(tmp,disk[i]*scaleFactor); 
    }
	for(int i=0;i<N_DISK;i++){
	  dbmon.disk[i]=disk[i]*scaleFactor;
		}
  long linkev[NUM_PRIORITIES];
  if((coln=PQfnumber(result,"linkev"))>=0 && ParseLongSQLArray(result,linkev,NUM_PRIORITIES,coln))
    for(int i=0;i<NUM_PRIORITIES;++i){
      tmp.Printf(_T("priority%d"),i);
      m_QueueInfo->SetDisplayValue(tmp,linkev[i]);
    }
  for(int i=0;i<NUM_PRIORITIES;i++){
  dbmon.linkev[i]=linkev[i];
		}
  if((coln=PQfnumber(result,"linkcmdlos"))>=0) m_QueueInfo->SetDisplayValue(_T("command LOS"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linkcmdsip"))>=0) m_QueueInfo->SetDisplayValue(_T("command SIP"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linkhd"))>=0) m_QueueInfo->SetDisplayValue(_T("header"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linkgps"))>=0) m_QueueInfo->SetDisplayValue(_T("GPS"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linkhk"))>=0) m_QueueInfo->SetDisplayValue(_T("housekeeping"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linkmon"))>=0) m_QueueInfo->SetDisplayValue(_T("monitor"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linksurf"))>=0) m_QueueInfo->SetDisplayValue(_T("SURF hk"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linkturf"))>=0) m_QueueInfo->SetDisplayValue(_T("TURF hk"),atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"linkped"))>=0) m_QueueInfo->SetDisplayValue(_T("pedestal"),atol(PQgetvalue(result,0,coln)));
/////////////////////////////////amir's addition july 12 08/////////////////////////
  m_dbmonitorViewer->DB_MonitorData(dbmon);
////////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("MON %ld"),time);
  SetStatusText(status,1);
  return;
}

void AviewFrame::ParseADU5PatPacket(PGresult *result){
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseADU5PatPacket] More than one ADU5 PAT record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseADU5PatPacket] No ADU5 PAT record received."));
    return;
  }
  
  GpsAdu5PatStruct_t pat;
  AuxPacketCode_t gpstype=PACKET_FROM_G12;
  if((coln=PQfnumber(result,"gpstype"))>=0) gpstype=( AuxPacketCode_t )atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"time"))>=0) pat.unixTime=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"us"))>=0) pat.unixTimeUs=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"code"))>=0) pat.gHdr.code=(PacketCode_t)atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"tod"))>=0) pat.timeOfDay=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"heading"))>=0) pat.heading=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"pitch"))>=0) pat.pitch=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"roll"))>=0) pat.roll=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"mrms"))>=0) pat.mrms=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"brms"))>=0) pat.brms=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"latitude"))>=0) pat.latitude=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"longitude"))>=0) pat.longitude=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"altitude"))>=0) pat.altitude=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"flag"))>=0) pat.attFlag=atol(PQgetvalue(result,0,coln));

  // Fill GPS information
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  m_dbControl->SetTime(ADU5PAT,pat.unixTime,!(crc&0x1));
  if ((pat.gHdr.code & 0x60000)>>17 == 1) 
    m_GPSDataInfo->SetADU5PatData(pat);
  else 
    m_GPSDataInfo->SetADU5PatData_U2(pat);
  if(AviewOpts::DefaultGPS==AviewOpts::ADU5){
    if(gpstype==PACKET_FROM_ADU5A){
      m_HeaderInfo->SetLocation(pat.latitude,pat.longitude,pat.altitude/1e3); // Convert altitude to km from ft
      m_HeaderInfo->SetOrientation(pat.heading,pat.pitch,pat.roll);
      m_HeaderInfo->SetPatColor(true);
      m_HeaderInfo->SetPatColorB(false);
      m_HeaderInfo->SetAttFlag(pat.attFlag);
    }
    else if (gpstype==PACKET_FROM_ADU5B){
      //m_HeaderInfo->SetLocation(0,0,0);
      //m_HeaderInfo->SetOrientation(0,0,0);
      m_HeaderInfo->SetLocationB(pat.latitude,pat.longitude,pat.altitude/1e3); // Convert altitude to km from ft
      m_HeaderInfo->SetOrientationB(pat.heading,pat.pitch,pat.roll);
      m_HeaderInfo->SetPatColor(false);
      m_HeaderInfo->SetPatColorB(true);
      m_HeaderInfo->SetAttFlagB(pat.attFlag);
    }
    else
      wxLogError(_T("[AviewFrame::ParseADU5PatPacket] gpstype is not A or B."));
  }
  payloadView->SetOrientation(pat.heading);
   ///////////////////////amir's addition july 12 08 ////////////////////////////
   m_adu5patViewer->ADU5PatData(pat);
  /////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("ADU5 PAT %d"),pat.unixTime);
  SetStatusText(status,1);
  return;
}

void AviewFrame::ParseADU5VtgPacket(PGresult *result){
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseADU5VtgPacket] More than one ADU5 VTG record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseADU5VtgPacket] No ADU5 VTG record received."));
    return;
  }

  GpsAdu5VtgStruct_t vtg;
  AuxPacketCode_t gpstype=PACKET_FROM_G12;
  if((coln=PQfnumber(result,"gpstype"))>=0) gpstype=( AuxPacketCode_t )atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"code"))>=0) vtg.gHdr.code=(PacketCode_t)atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"time"))>=0) vtg.unixTime=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"us"))>=0) vtg.unixTimeUs=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"course"))>=0) vtg.trueCourse=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"mcourse"))>=0) vtg.magneticCourse=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"vkt"))>=0) vtg.speedInKnots=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"vkph"))>=0) vtg.speedInKPH=atof(PQgetvalue(result,0,coln));

  // Fill GPS information
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  m_dbControl->SetTime(ADU5VTG,vtg.unixTime,!(crc&0x1));
  if ((vtg.gHdr.code & 0x60000)>>17 == 1) 
    m_GPSDataInfo->SetADU5VtgData(vtg);
  else 
    m_GPSDataInfo->SetADU5VtgData_U2(vtg);
  if(AviewOpts::DefaultGPS==AviewOpts::ADU5)
    if(gpstype==PACKET_FROM_ADU5A){
      m_HeaderInfo->SetSpeed(vtg.speedInKnots,vtg.trueCourse); 
      m_HeaderInfo->SetVtgColor(true);
      m_HeaderInfo->SetVtgColorB(false);
    }
    else if (gpstype==PACKET_FROM_ADU5B){
      m_HeaderInfo->SetSpeedB(vtg.speedInKnots,vtg.trueCourse); 
      m_HeaderInfo->SetVtgColor(false);
      m_HeaderInfo->SetVtgColorB(true);
    }
 //////////////////////////////amir's addition july 12 08/////////////////////
  m_adu5vtgViewer->ADU5VtgData(vtg);
//   ////////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("ADU5 VTG %d"),vtg.unixTime);
  SetStatusText(status,1);
  return;
}

void AviewFrame::ParseADU5SatPacket(PGresult *result){
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseADU5SatPacket] More than one ADU5 SAT record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseADU5SatPacket] No ADU5 SAT record received."));
    return;
  }
  
  GpsAdu5SatStruct_t sat;
  if((coln=PQfnumber(result,"code"))>=0) sat.gHdr.code=(PacketCode_t)atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"time"))>=0) sat.unixTime=atol(PQgetvalue(result,0,coln));
  long numSats[4];
  if((coln=PQfnumber(result,"numsats"))>=0 && ParseLongSQLArray(result,numSats,4,coln)){
    int maxsats=0;
    for(int i=0;i<4;++i){ 
      sat.numSats[i]=numSats[i];
      if(numSats[i]>maxsats) maxsats=numSats[i];
    }
    long *prn[4],*elevation[4],*snr[4],*flag[4],*azimuth[4],maxNumSats[4];
    for(int i=0;i<4;++i){
      maxNumSats[i]=maxsats;
      prn[i]=new long[maxsats];
      elevation[i]=new long[maxsats];
      snr[i]=new long[maxsats];
      flag[i]=new long[maxsats];
      azimuth[i]=new long[maxsats];
    }
    if((coln=PQfnumber(result,"prn"))>=0 && ParseLongSQLArray2D(result,prn,4,maxNumSats,coln) &&
       (coln=PQfnumber(result,"elevation"))>=0 && ParseLongSQLArray2D(result,elevation,4,maxNumSats,coln) &&
       (coln=PQfnumber(result,"snr"))>=0 && ParseLongSQLArray2D(result,snr,4,maxNumSats,coln) && 
       (coln=PQfnumber(result,"flag"))>=0 && ParseLongSQLArray2D(result,flag,4,maxNumSats,coln) && 
       (coln=PQfnumber(result,"azimuth"))>=0 && ParseLongSQLArray2D(result,azimuth,4,maxNumSats,coln)){
      for(int i=0;i<4;++i)
	for(int j=0;j<numSats[i];++j){
	  sat.sat[i][j].prn=prn[i][j];
	  sat.sat[i][j].elevation=elevation[i][j];
	  sat.sat[i][j].snr=snr[i][j];
	  sat.sat[i][j].flag=flag[i][j];
	  sat.sat[i][j].azimuth=azimuth[i][j];
	}
    }
    for(int i=0;i<4;++i){
      delete[] prn[i];
      delete[] elevation[i];
      delete[] snr[i];
      delete[] flag[i];
      delete[] azimuth[i];
    }
  }

  // Fill GPS information
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  m_dbControl->SetTime(ADU5SAT,sat.unixTime,!(crc&0x1));
  if ((sat.gHdr.code & 0x60000)>>17 == 1) 
    m_GPSSatInfo->SetADU5Sat(sat);
  else
    m_GPSSatInfo->SetADU5Sat_U2(sat);
///////////////////////////amir's addition july 12 08////////////////////////
  m_adu5satViewer->ADU5SatData(sat);
///////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("ADU5 SAT %d"),sat.unixTime);
  SetStatusText(status,1);
  return;

}
void AviewFrame::ParseG12PosPacket(PGresult *result){
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseG12PosPacket] More than one G12 POS record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseG12PosPacket] No G12 POS record received."));
    return;
  }
  
  GpsG12PosStruct_t pos;
  if((coln=PQfnumber(result,"time"))>=0) pos.unixTime=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"us"))>=0) pos.unixTimeUs=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"tod"))>=0) pos.timeOfDay=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"numsats"))>=0) pos.numSats=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"latitude"))>=0) pos.latitude=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"longitude"))>=0) pos.longitude=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"altitude"))>=0) pos.altitude=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"course"))>=0) pos.trueCourse=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"upv"))>=0) pos.verticalVelocity=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"vkt"))>=0) pos.speedInKnots=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"pdop"))>=0) pos.pdop=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"hdop"))>=0) pos.hdop=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"vdop"))>=0) pos.vdop=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"tdop"))>=0) pos.tdop=atol(PQgetvalue(result,0,coln));

  // Fill GPS information
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  m_dbControl->SetTime(G12POS,pos.unixTime,!(crc&0x1));
  m_GPSDataInfo->SetG12Data(pos);
  if(AviewOpts::DefaultGPS==AviewOpts::G12){
    m_HeaderInfo->SetLocation(pos.latitude,pos.longitude,pos.altitude/1e3); // Convert altitude to km from m
    m_HeaderInfo->SetSpeed(pos.speedInKnots,pos.trueCourse); 
  }
////////////////////////////amir's addition july 12 08//////////////////////////
 m_gpsposViewer->GpsPosData(pos);
  ///////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("G12 POS %d"),pos.unixTime);
  SetStatusText(status,1);
  return;
}
void AviewFrame::ParseG12SatPacket(PGresult *result){
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseG12SatPacket] More than one G12 SAT record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseG12SatPacket] No G12 SAT record received."));
    return;
  }
  
  GpsG12SatStruct_t sat;
  if((coln=PQfnumber(result,"time"))>=0) sat.unixTime=atol(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"numsats"))>=0) sat.numSats=atol(PQgetvalue(result,0,coln));
  long prn[MAX_SATS],elevation[MAX_SATS],snr[MAX_SATS],flag[MAX_SATS],
      azimuth[MAX_SATS];

  if((coln=PQfnumber(result,"prn"))>=0 && ParseLongSQLArray(result,prn,sat.numSats,coln) &&
     (coln=PQfnumber(result,"elevation"))>=0 && ParseLongSQLArray(result,elevation,sat.numSats,coln) &&
     (coln=PQfnumber(result,"snr"))>=0 && ParseLongSQLArray(result,snr,sat.numSats,coln) &&
     (coln=PQfnumber(result,"flag"))>=0 && ParseLongSQLArray(result,flag,sat.numSats,coln) &&
     (coln=PQfnumber(result,"azimuth"))>=0 && ParseLongSQLArray(result,azimuth,sat.numSats,coln)){
    for(int j=0;j<(int)sat.numSats;++j){
      sat.sat[j].prn=prn[j];
      sat.sat[j].elevation=elevation[j];
      sat.sat[j].snr=snr[j];
      sat.sat[j].flag=flag[j];
      sat.sat[j].azimuth=azimuth[j];
    }
  }

  // Fill GPS information
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  m_dbControl->SetTime(G12SAT,sat.unixTime,!(crc&0x1));
  m_GPSSatInfo->SetG12Sat(sat);
/////////////////////////////amir's addition july 12 08//////////////////////////
 m_g12satViewer->G12SatData(sat);
///////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("G12 SAT %d"),sat.unixTime);
  SetStatusText(status,1);
  return;
}

void AviewFrame::ParseCmdEchoPacket(PGresult *result){
  int n=PQntuples(result);
  DBCmd_t dbcmd;
  int coln;
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseCmdEchoPacket] More than one command echo record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseCmdEchoPacket] No command echo record received."));
    return;
  }

  // Store command echo; here we're flying with our pants down, since a
  // wrong variable name, will return -1 from PQfnumber and crash the program
  unsigned long time=0;
  if((coln=PQfnumber(result,"time"))>=0) time=atol(PQgetvalue(result,0,coln));
  dbcmd.time=time;
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  dbcmd.crc=int(crc);
  m_dbControl->SetTime(CMDECHO,time,!(crc&0x1));
  unsigned short numCmdBytes=0;
  if((coln=PQfnumber(result,"bytes"))>=0) numCmdBytes=atoi(PQgetvalue(result,0,coln));
  dbcmd.numCmdBytes=numCmdBytes;
  long cmdLong[MAX_CMD_LENGTH];
  if((coln=PQfnumber(result,"cmd"))>=0){
    if(ParseLongSQLArray(result,cmdLong,numCmdBytes,coln)){
      unsigned char cmd[MAX_CMD_LENGTH];
      for(int i=0;i<numCmdBytes;++i) cmd[i]=cmdLong[i];
      m_HeaderInfo->SetCommand(cmd,numCmdBytes);
    }
  }
  	for(int i=0;i<MAX_CMD_LENGTH;i++){
	dbcmd.cmdLong[i]=cmdLong[i];
  		}
  if((coln=PQfnumber(result,"flag"))>=0) {m_HeaderInfo->SetCommandGood(atoi(PQgetvalue(result,0,coln)));
  	dbcmd.flag=atoi(PQgetvalue(result,0,coln));}
/////////////////////////////////amir's addition july 12 08/////////////////////////
  m_dbcmdViewer->DB_CmdData(dbcmd);
////////////////////////////////////////////////////////////////////////////
  wxString status;
  status.Printf(_T("CMD %ld"),time);
  SetStatusText(status,1);
  return;
}

void AviewFrame::ParseWaveformPacket(PGresult *result){
  static wxColour badcrcBg=wxColour(46,46,46);
  int n=PQntuples(result);
  
  // Find column locations
  int colev=PQfnumber(result,"evnum");
  int colid=PQfnumber(result,"id");
  int colwrap=PQfnumber(result,"hbwrap");
  int colstart=PQfnumber(result,"hbstart");
  int colend=PQfnumber(result,"hbend");
  int colcal=PQfnumber(result,"cal");
  int colcrc=PQfnumber(result,"crc");
  //int  colraw=PQfnumber(result,"raw");
    
  double v[MAX_NUMBER_SAMPLES];
  //long vlong[MAX_NUMBER_SAMPLES];

  // Parse waveforms
  unsigned long evnum=0;
  if(colev>=0) evnum=atol(PQgetvalue(result,0,colev));
  for(int i=0;i<n;++i){
    int id=0;
    if(colid>=0) id=atoi(PQgetvalue(result,i,colid));
    int nSurf=id/CHANNELS_PER_SURF;
    int nChan=id%CHANNELS_PER_SURF;
    int wvLength;
    if(!ParseDoubleSQLArray(result,v,MAX_NUMBER_SAMPLES,colcal,i)){
      //      if(!ParseLongSQLArray(result,vlong,MAX_NUMBER_SAMPLES,colraw,i)){
      wxLogError(_T("[AviewFrame::ParseWaveformPacket] Ignoring waveform for SURF %d, CHAN %d"),nSurf+1,nChan);
	continue;
//      }
//      for(int i=0;i<MAX_NUMBER_SAMPLES;++i) v[i]=(double)vlong[i];
//      wvLength=MAX_NUMBER_SAMPLES;
    }else{  
      bool hbwrap=false;
      int hbstart=0,hbend=0;
      if(colwrap>=0) hbwrap=atoi(PQgetvalue(result,i,colwrap));
      if(colstart>=0) hbstart=atoi(PQgetvalue(result,i,colstart));
      if(colend>=0)hbend=atoi(PQgetvalue(result,i,colend));
      if(hbwrap)
	wvLength=hbend-hbstart-2;
      else
	wvLength=hbstart+(MAX_NUMBER_SAMPLES-2-hbend);
    }
    
    // Stuff data into RF tabs
    if(wvLength>1){
      if(rfCurve[nSurf][nChan]) delete rfCurve[nSurf][nChan];
      rfCurve[nSurf][nChan] = new wxGraphCurve(wvtimebase,v,wvLength);
      rfDisplay[nSurf][nChan]->SetData(rfCurve[nSurf][nChan]); 

      unsigned short crc=0;
      if(colcrc>=0) crc=atoi(PQgetvalue(result,i,colcrc));
      if(crc&0x2) channelTab[nSurf][nChan]->SetBackgroundColour(*wxBLACK);
      else channelTab[nSurf][nChan]->SetBackgroundColour(badcrcBg);

      if(nChan==CHANNELS_PER_SURF-1)
	channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetVoltage()); 
      else{
	switch(AviewOpts::RFTab){
	case AviewOpts::PowerTab:
	  channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetPower()); 
	  break;
	case AviewOpts::FFTTab:
	  channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetFFT()); 
	  break;
	case AviewOpts::VoltageTab:
	default:
	  channelTab[nSurf][nChan]->SetData(rfDisplay[nSurf][nChan]->GetVoltage()); 
	  break;
	}
      }
    }
  
    if(nChan<CHANNELS_PER_SURF-1)
      payloadView->SetValue(rfCurve[nSurf][nChan],nSurf,nChan);
  }

  wxString status;
  status.Printf(_T("WV %ld"),evnum);
  SetStatusText(status,1);

  return;
}

void AviewFrame::ParseEventRate(PGresult *result){
  int n=PQntuples(result);
  if(n>1)
    wxLogError(_T("[AviewFrame::ParseEventRate] More than one event rate record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[AviewFrame::ParseEventRate] No event rate record received."));
    return;
  }

  int coln;
  if((coln=PQfnumber(result,"eventrate"))<0) return;

  m_eventRate->SetGauge(atof(PQgetvalue(result,0,coln)));

  return;
}

void AviewFrame::ParseWaveformAverage(PGresult *result){
  static int nWfmsExpecting=0;
  static int nWfmsReceived=0;
  static wxGraphCurve *wvData[ACTIVE_SURFS][CHANNELS_PER_SURF][MAX_DB_WVAVG];

  int n=PQntuples(result);
  if(!nWfmsExpecting){ // This is header request
    int coln=PQfnumber(result,"evnum");
    if(coln<0){
      wxLogError(_T("[AviewFrame::ParseWaveformAverage] Bad column (%d)."),coln);
      return;
    }else{
      nWfmsExpecting=n;
      wxLogMessage(_T("[AviewFrame::ParseWaveformAverage] Will average %d waveforms."),nWfmsExpecting);
    }
    for(int i=0;i<n;++i){
      unsigned long evnum=atol(PQgetvalue(result,i,coln));      
      sqlRequestType.Add(WVAVG);
      wxString cmd;
      cmd.Printf(_T("SELECT id,cal FROM wv WHERE evnum=%ld;"),evnum);
      wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
      sqlEvent.SetEventObject( this );
      sqlEvent.SetString(cmd);
      sqlEvent.SetClientData((void*)parseWrapper);
      ProcessEvent(sqlEvent);
    }
  }else{ // This is requested waveform
    double v[MAX_NUMBER_SAMPLES];
    int colid=PQfnumber(result,"id");
    int colcal=PQfnumber(result,"cal");
    if(colid>=0 && colcal>=0){
      for(int i=0;i<n;++i){
	int id=atoi(PQgetvalue(result,i,colid));
	int nSurf=id/CHANNELS_PER_SURF;
	int nChan=id%CHANNELS_PER_SURF;
	if(!ParseDoubleSQLArray(result,v,MAX_NUMBER_SAMPLES,colcal,i)){
	  wxLogError(_T("[AviewFrame::ParseAverageWaveform] Ignoring waveform for SURF %d, CHAN %d"),nSurf+1,nChan);
	  continue;
	}
	// Stuff data into array
	wvData[nSurf][nChan][nWfmsReceived] = new wxGraphCurve(wvtimebase,v,MAX_NUMBER_SAMPLES);
      }
    }

    wxString status;
    status.Printf(_T("WAVEFORM AVERAGE - %d of %d received"),++nWfmsReceived,nWfmsExpecting);
    SetStatusText(status,1);

    if(nWfmsReceived>= nWfmsExpecting){
      // This was last waveform expected, so calculate and display result
      for(int i=0;i<ACTIVE_SURFS;++i)
	for(int j=0;j<CHANNELS_PER_SURF;++j){

	  if(rfCurve[i][j]) delete rfCurve[i][j];
	  rfDisplay[i][j]->SetAverage(wvData[i][j],nWfmsReceived,&rfCurve[i][j]);
	  if(j==CHANNELS_PER_SURF-1)
	    channelTab[i][j]->SetData(rfDisplay[i][j]->GetVoltage()); 
	  else{
	    switch(AviewOpts::RFTab){
	    case AviewOpts::PowerTab:
	      channelTab[i][j]->SetData(rfDisplay[i][j]->GetPower()); 
	      break;
	    case AviewOpts::FFTTab:
	      channelTab[i][j]->SetData(rfDisplay[i][j]->GetFFT()); 
	      break;
	    case AviewOpts::VoltageTab:
	    default:
	      channelTab[i][j]->SetData(rfDisplay[i][j]->GetVoltage()); 
	      break;
	    }
	  }
	  channelTab[i][j]->SetColour(*wxWHITE);
	  payloadView->SetTriggerPatternColour(1,*wxLIGHT_GREY);
	  if(j<CHANNELS_PER_SURF-1) payloadView->SetValue(rfCurve[i][j],i,j);
	}
      // Release memory
      for(int i=0;i<ACTIVE_SURFS;++i)
	for(int j=0;j<CHANNELS_PER_SURF;++j)
	  for(int k=0;k<nWfmsReceived;++k)
	    delete wvData[i][j][k];
      nWfmsExpecting=nWfmsReceived=0;
    }
  }

  return;
}

void AviewFrame::ParseDBMRef(PGresult *result){
  int n=PQntuples(result);
  wxLogMessage(_T("[AviewFrame::ParseDBMRef] Received %d entries\n"),n);
  if(n<1) return;
  int coln;
  if((coln=PQfnumber(result,"rfpow"))<0) return;
  
  // Allocate memory needed
  float *dBm[ACTIVE_SURFS][CHANNELS_PER_SURF-1];
  for(int i=0;i<ACTIVE_SURFS;++i)
    for(int j=0;j<CHANNELS_PER_SURF-1;++j)
      dBm[i][j]=new float[n];
  
  long *rfpow[ACTIVE_SURFS];
  long dimRFpow[ACTIVE_SURFS];
  for(int i=0;i<ACTIVE_SURFS;++i){
    dimRFpow[i]=CHANNELS_PER_SURF-1;
    rfpow[i]=new long[CHANNELS_PER_SURF-1];
  }
  
  int nev=0;
  for(int i=0;i<n;++i){
    if(ParseLongSQLArray2D(result,rfpow,ACTIVE_SURFS,dimRFpow,coln,i)){
      for(int i=0;i<ACTIVE_SURFS;++i)
	for(int j=0;j<CHANNELS_PER_SURF-1;++j)
	  dBm[i][j][nev]=rfpower_dBm(i,j,rfpow[i][j]&0x7FFF);
      ++nev;
    }
  }

  if(nev){
    for(int i=0;i<ACTIVE_SURFS;++i)
      for(int j=0;j<CHANNELS_PER_SURF-1;++j){
	double mean=0;
	for(int k=0;k<nev;++k) mean+=pow(10.,(double)dBm[i][j][k]/10);
	mean/=nev;
	rfDisplay[i][j]->SetDBMRef(10*log10(mean));
      }
    SetStatusText(_T("Loaded dBm references"),1);
  }

  // Clear memory
  for(int i=0;i<ACTIVE_SURFS;++i){
    for(int j=0;j<CHANNELS_PER_SURF-1;++j) delete[] dBm[i][j];
    delete rfpow[i];
  }

  return;
}

void AviewFrame::SendListen(int type){
  wxString cmd;
  cmd << _T("LISTEN ") << packetTable[type] << _T(";");
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlRequestType.Add(LISTEN);
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
}

void AviewFrame::ReceiveNotifications(){
  PGnotify *notice;
  while((notice = m_dbHandler->CheckOnNotification()) != NULL){
    wxString noteStr=_U(notice->relname);
    for(int i=0;i<N_PCKTYPE;++i)
      if(noteStr==packetTable[i]){
	needsIndex[i]=true;
	break;
      }
    PQfreemem(notice);
  }
}

void AviewFrame::IndexDBPackets(int type,bool full,bool retro){
  if(full){                 // Full indexing
    packetListUpdateTime[type]=0;
    if(packetList[type]){ 
      delete[] packetList[type]; 
      packetList[type]=NULL;
      m_dbControl->SetList(type,NULL,0);  // Invalidate current list
      m_dbControl->SetIndex(type,-1);     // Reset counter
      packetListAllocSize[type]=0;
      packetListItems[type]=0;
    }
  }
  // Prepare SQL request
  sqlRequestType.Add(type+INDEX_OFFSET);
  wxString cmd;
  if(type==G12SAT || type==ADU5SAT || type==CMDECHO || type==MONITOR)
    cmd.Printf(_T("SELECT nbuf,time,now FROM %s WHERE time>0 AND now%s%ld%s ORDER BY now LIMIT %d;"),
	       packetTable[type].c_str(),retro?_T("<"):_T(">="),
	       retro?packetListOldestTime[type]:packetListUpdateTime[type],
	       AviewOpts::crcCheck?_T(" AND crc=257"):_T(""),
	       packetListRequestSize[type]);
  else if(type==RFEVENT)
    cmd.Printf(_T("SELECT nbuf,time,us,now FROM %s WHERE time>0 AND now%s%ld ORDER BY now LIMIT %d;"),
	       packetTable[type].c_str(),retro?_T("<"):_T(">="),
	       retro?packetListOldestTime[type]:packetListUpdateTime[type],
	       packetListRequestSize[type]);
  else
    cmd.Printf(_T("SELECT nbuf,time,us,now FROM %s WHERE time>0 AND now%s%ld%s ORDER BY now LIMIT %d;"),
	       packetTable[type].c_str(),retro?_T("<"):_T(">="),
	       retro?packetListOldestTime[type]:packetListUpdateTime[type],
	       AviewOpts::crcCheck?_T(" AND crc=257"):_T(""),
	       packetListRequestSize[type]);

  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
  return;
}

bool AviewFrame::ReallocateList(int type,int n){
  if(packetListAllocSize[type]==0){
    packetList[type]=new dbRef[packetListAllocSize[type]=n*2];
    if(!packetList[type]){
      wxString tmp;
      tmp.Printf(_T("Failed to allocate memory for initial index of packet type %d (%d bytes)! I suggest you restart the viewer..."),type,packetListAllocSize[type]*sizeof(dbRef));
      wxMessageBox(tmp,_T("Memory allocation error"),wxOK|wxICON_ERROR);
      packetListAllocSize[type]=0;
      return false;
    }
  }else if(n+packetListItems[type]>packetListAllocSize[type]){
    int oldSize=packetListAllocSize[type];
    int newSize=wxMax(oldSize*2,n*2+packetListItems[type]);
    dbRef *newList=new dbRef[newSize];
    if(!newList){
      wxString tmp;
      tmp.Printf(_T("Failed to re-allocate memory for index of packet type %d (%d bytes)! I suggest you restart the viewer..."),type,newSize*sizeof(dbRef));
      wxMessageBox(tmp,_T("Memory allocation error"),wxOK|wxICON_ERROR);
      packetListAllocSize[type]=oldSize;
      return false;
    }
    for(int i=0;i<packetListItems[type];++i)
      newList[i]=packetList[type][i];
    m_dbControl->SetList(type,NULL,0); // Invalidate, to be on the safe side
printf("ControlPanel::ReallocateList type %d \n", type);
    delete[] packetList[type];
    packetList[type]=newList;
  }

  return true;
}

void AviewFrame::StorePacketList(PGresult *result,int type,bool haveus){
  unsigned int n=PQntuples(result);
  if(n<=0) return;
  m_dbControl->MarkIndex(type);
  if(!ReallocateList(type,n)) return;
  int timecol=PQfnumber(result,"time");
  int uscol=(haveus?PQfnumber(result,"us"):-1);
  int nbufcol=PQfnumber(result,"nbuf");
  int nowcol=PQfnumber(result,"now");
  if(timecol<0 || nbufcol<0 || (haveus && uscol<0) ||  nowcol<0){
    wxLogError(_T("[AviewFrame::StorePacketList] Bad columns returned; type %d"),type);
    return;
  }
  packetListUpdateTime[type]=wxMax(atol(PQgetvalue(result,0,nowcol)),packetListUpdateTime[type]);
  long newOldTime=atol(PQgetvalue(result,n-1,nowcol));
  if(packetListOldestTime[type]>newOldTime){ // This is retro request
    if(n<packetListRequestSize[type]) needsRetroIndex[type]=false;
    packetListOldestTime[type]=newOldTime;
  }
  for(unsigned int i=0;i<n;++i){
    packetList[type][i+packetListItems[type]].time=atol(PQgetvalue(result,i,timecol));
    packetList[type][i+packetListItems[type]].us=(haveus?atol(PQgetvalue(result,i,uscol)):-1);
    packetList[type][i+packetListItems[type]].nbuf=atol(PQgetvalue(result,i,nbufcol));
  }
  packetListItems[type]+=n;
  qsort(packetList[type],packetListItems[type],sizeof(dbRef),dbRefComp);
  m_dbControl->SetList(type,packetList[type],packetListItems[type]);
}

