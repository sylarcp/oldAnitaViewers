/*
 * SlowMoFrame.cpp
 *
 * Definitions for main frame of SlowMo. All definitions of menus and children,
 * as well as file and database parsing and indexing.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SlowMoFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/splash.h>
#include <wx/msgdlg.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/notebook.h>
#include <expat.h>
#include <wx/imagpng.h>
#include "range.h"
#include "SlowMoFrame.h"
#include "sort.h"
#include "DBTerminal.h"
#include <includes/anitaStructures.h>
#include "rfpower_cal.h"

BEGIN_EVENT_TABLE( SlowMoFrame, wxFrame )
  EVT_MENU( MENU_CONTROL_OPTIONS, SlowMoFrame::OnMenuControlOptions )
  EVT_MENU( MENU_CONTROL_SAVEOPTIONS, SlowMoFrame::OnMenuControlSaveOptions )
  EVT_MENU( MENU_CONTROL_LOADOPTIONS, SlowMoFrame::OnMenuControlLoadOptions )
  EVT_MENU( MENU_CONTROL_QUIT, SlowMoFrame::OnMenuControlQuit )
  
  EVT_MENU( MENU_FLIGHTDATA_CONTROL, SlowMoFrame::OnMenuFlightDataControl )
  EVT_MENU( MENU_FLIGHTDATA_OPEN, SlowMoFrame::OnMenuFlightDataOpen )
  EVT_MENU( MENU_FLIGHTDATA_NEXT, SlowMoFrame::OnMenuFlightDataNext )
  EVT_MENU( MENU_FLIGHTDATA_PREV, SlowMoFrame::OnMenuFlightDataPrev )

  EVT_MENU( MENU_DBDATA_CONNECT, SlowMoFrame::OnMenuDBDataConnect )
  EVT_MENU( MENU_DBDATA_DISCONNECT, SlowMoFrame::OnMenuDBDataDisconnect )
  EVT_MENU( MENU_DBDATA_CONTROL, SlowMoFrame::OnMenuDBDataControl )
  EVT_MENU( MENU_DBDATA_TERMINAL, SlowMoFrame::OnMenuDBDataTerminal )
  EVT_MENU( MENU_DBDATA_NEXT, SlowMoFrame::OnMenuDBDataNext )
  EVT_MENU( MENU_DBDATA_PREV, SlowMoFrame::OnMenuDBDataPrev )
  EVT_MENU( MENU_DBDATA_LAST, SlowMoFrame::OnMenuDBDataLast )
  EVT_MENU( MENU_DBDATA_AUTO, SlowMoFrame::OnMenuDBDataAuto )
    
  EVT_MENU( MENU_HELP_ABOUT, SlowMoFrame::OnMenuHelpAbout )
//////////////////////////amir's addition july9 08////////////////
 EVT_MENU( MENU_SLOW_MO_VIEWER, SlowMoFrame::OnMenuSlowMoViewer)//amir's addition
 /////////////////////////////////////////////////////////////////////////////
  EVT_TIMER( UPDATE_TIMER, SlowMoFrame::OnUpdateTimer )
  EVT_TIMER( CURRENT_TIME_TIMER, SlowMoFrame::OnCurrentTimeTimer )
  EVT_TIMER( DBCONNECTION_TIMER, SlowMoFrame::CheckDBConnection )

  EVT_COMMAND_RANGE(INT_MIN,-2,wxSQL_REQUEST,SlowMoFrame::OnSQLRequest)
  EVT_COMMAND_RANGE(SLOWPCK+CONTROLWIN_OFFSET,SLOWPCK+CONTROLWIN_OFFSET,wxLOAD_FILE,SlowMoFrame::ReadFlightData)
  EVT_COMMAND_RANGE(SLOWPCK+CONTROLWIN_OFFSET,SLOWPCK+CONTROLWIN_OFFSET,wxREQUEST_PACKET,SlowMoFrame::RequestPacket)

END_EVENT_TABLE()

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

// Constructor
SlowMoFrame::SlowMoFrame( wxWindow* parent, int argc, char **argv, 
			  wxWindowID id, 
			  const wxPoint& pos, const wxSize& size){
  using namespace SlowMoOpts;
  
  // Here we need to parse the command line
  cmdLine=new wxCmdLineParser(argc,argv);
  static const wxCmdLineEntryDesc cmdLineDesc[] ={
    { wxCMD_LINE_OPTION, _T("s"), _T("slowdir"), _T("Flight format slow data directory") },
    { wxCMD_LINE_SWITCH, _T("c"), _T("defcon"), _T("Connect to default database") },
    { wxCMD_LINE_OPTION, _T("C"), _T("connect"), _T("Connect to database with parameters given in form <host[:dbname[:user[:passwd]]]>") },	
    { wxCMD_LINE_OPTION, _T("o"), _T("options"), _T("Display options file") },	
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
  if(cmdLine->Found(_T("c"))){
    m_dbHandler=new DBHandler();
    m_dbHandler->Connect(dbHost,dbUser,dbPwd,dbName);
  }else if(cmdLine->Found(_T("C"),&dbParams)){
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
  wxImage::AddHandler( new wxPNGHandler );
  wxBitmap bitmap;
  wxSplashScreen* splash = NULL;
  if (bitmap.LoadFile(_T("slowmo_splash.png"), wxBITMAP_TYPE_PNG)){
    splash = new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
				0, NULL, -1, wxDefaultPosition, wxDefaultSize,
				wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    splash->Show(true);
  }

  Create( parent, id, TITLE, pos, size, wxDEFAULT_FRAME_STYLE|wxFULL_REPAINT_ON_RESIZE);
  CreateStatusBar(3);

  this->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  // Create menus
  m_pMenuBar = new wxMenuBar;

  m_pControlMenu = new wxMenu;
  m_pControlMenu->Append(MENU_CONTROL_OPTIONS, _("Options\to"), _T("Setup options"));
  m_pControlMenu->Append(MENU_CONTROL_SAVEOPTIONS, _("Save options\tCTRL-s"), _T("Save SlowMo options"));
  m_pControlMenu->Append(MENU_CONTROL_LOADOPTIONS, _("Load options\tCTRL-l"), _T("Load SlowMo options"));
  m_pControlMenu->AppendSeparator();
  m_pControlMenu->Append(MENU_CONTROL_QUIT, _("Quit\tCTRL-q"), _T("Quit Slow Monitor"));
  m_pMenuBar->Append(m_pControlMenu,_("Control"));

  m_pFlightDataMenu = new wxMenu;
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_CONTROL, _("Control Panel"), _T("Open flight data control panel"));
  m_pFlightDataMenu->AppendSeparator();
  m_pFlightDataMenu->Append(MENU_FLIGHTDATA_OPEN,_("Open"), _T("Open flight format slow file"));
  m_pFlightDataNext = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_NEXT, _("Next slow\tCTRL-n"), _T("Next slow packet"));
  m_pFlightDataPrev = m_pFlightDataMenu->Append(MENU_FLIGHTDATA_PREV, _("Previous slow\tCTRL-p"), _T("Previous slow packet"));
  m_pMenuBar->Append(m_pFlightDataMenu,_("Flight Data"));

  m_pDBDataMenu = new wxMenu;
  m_pDBDataConnect = m_pDBDataMenu->Append(MENU_DBDATA_CONNECT,_("Connect\tc"),
					   _T("Connect to ANITA database"));
  m_pDBDataDisconnect = m_pDBDataMenu->Append(MENU_DBDATA_DISCONNECT,_("Disconnect"), 
					      _T("Disconnect from ANITA database"));
  m_pDBDataMenu->AppendSeparator();
  m_pDBDataControl  = m_pDBDataMenu->Append(MENU_DBDATA_CONTROL,_("Control panel\tCTRL-c"),
					    _T("Open database control panel"));
  m_pDBDataTerminal = m_pDBDataMenu->Append(MENU_DBDATA_TERMINAL,_("Terminal\tCTRL-t"),_T("Open SQL terminal"));
  m_pDBDataMenu->AppendSeparator();
  m_pDBDataNext = m_pDBDataMenu->Append(MENU_DBDATA_NEXT, _("Next slow\tn"), 
					_T("Next slow packet from database"));
  m_pDBDataPrev = m_pDBDataMenu->Append(MENU_DBDATA_PREV, _("Previous slow\tp"), 
					_T("Previous slow packet from database"));
  m_pDBDataLast = m_pDBDataMenu->Append(MENU_DBDATA_LAST, _("Last slow\tl"), 
					_T("Last slow packet from database"));
  m_pDBDataAuto = m_pDBDataMenu->Append(MENU_DBDATA_AUTO, _("Auto/Unauto all\ta"), 
					_T("Auto or unauto checkbox"));

  m_pMenuBar->Append(m_pDBDataMenu,_("DB data"));

m_pDataView= new wxMenu;//amir's addition
 // m_pDataViewer= m_pDataView->Append(MENU_CONTROL_DATAVIEWER, _("Data Viewer"), _T("Data Viewer"));//amir's addition
  //////////////////////////july 1 2008/////////////////////////////////////////
  m_pSlowMoViewer= m_pDataView->Append(MENU_SLOW_MO_VIEWER, _("SlowMo Viewer"), _T("SlowMo Viewer"));//amir's addition
  m_pMenuBar->Append(m_pDataView,_("Data Viewer"));//amir's additon
/////////////////////////////////////////////////////////////////////////////////
  m_pHelpMenu = new wxMenu;
  m_pHelpMenu->Append(MENU_HELP_ABOUT,_("About\tF12"),_T("What is this????"));
  m_pMenuBar->Append(m_pHelpMenu,_("Help"));

  this->SetMenuBar(m_pMenuBar);

  // Create control panels
  //////////////////////amir's addition july 12 08///////////////
  m_slowmoViewer=new SlowMoViewer(this);
  ////////////////////////////////////////////////////////
  m_optionsDialog=new SlowMoOptions(this);
  m_flightControl = new ControlPanel(this,"FlightControlItem",N_PCKTYPE,_T("File Control Panel"));
  m_dbControl = new ControlPanel(this,"DBControlItem",N_PCKTYPE,_T("Database Control Panel"));
  for(int i=0;i<N_PCKTYPE;++i){
    switch(i){
    case SLOWPCK: 
      m_flightControl->SetLabel(i,_T("Slow")); 
      m_dbControl->SetLabel(i,_T("Slow")); 
      break;
    default: break;
    }
  }

  // Create and arrange all children
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *dataSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *midSizer = new wxBoxSizer(wxHORIZONTAL);

  topSizer->Add(dataSizer,1,wxEXPAND);
  dataSizer->Add(leftSizer,1,wxEXPAND);
  dataSizer->Add(midSizer,2,wxEXPAND);

  m_slowInfo = new SlowInfo(this);

  wxBoxSizer *leftMidSizer = new wxBoxSizer(wxHORIZONTAL);  
  m_EventRateInfo = new BarInfo(this,2,2, _("Ev. rate"),_T("Hz"),
				EventRateLabels,EventRateLabels,
				EventRateCaptions,EventRateVars,
				EventRateTable,EventRateSpec,
				EventRateRange,EventRateAverage,
				EventRateScaleType,wxSize(14,30),
				EventRateScale); 
  leftMidSizer->Add(m_EventRateInfo,1,wxEXPAND);
  m_PowerInfo = new BarInfo(this,N_POWER,N_POWER, _("Voltage Current"),_T("V A"),
			    PowerLabels,PowerLabels,PowerCaptions,PowerVars,
			    PowerTable,PowerSpec,PowerRange,PowerAverage,
			    PowerScaleType,wxSize(14,30),PowerScale); 
  leftMidSizer->Add(m_PowerInfo,2,wxEXPAND);

  m_TempInfo = new BarInfo(this,N_TEMP,N_TEMP, _("Temperatures"),_T("deg C"),
			   TempLabels,TempLabels,TempCaptions,TempVars,
			   TempTable,TempSpec,TempRange,TempAverage,
			   TempScaleType,wxSize(14,30),TempScale); 
/*

  m_L1Info = new BarInfo(this,N_L1,L1_PER_ROW, _("L1v Rates"),_T("Hz"),L1Labels,
			 L1Labels,L1Captions,L1Vars,L1Table,
			 L1Spec,L1Range,L1Average,L1ScaleType,wxSize(14,30),L1Scale); 
  m_L1hInfo = new BarInfo(this,N_L1,L1_PER_ROW, _("L1h Rates"),_T("Hz"),L1Labels,
			 L1Labels,L1Captions,L1Vars,L1Table,
			 L1Spec,L1Range,L1Average,L1ScaleType,wxSize(14,30),L1Scale); 
  
  m_L3Info = new BarInfo(this,N_L3,L3_PER_ROW, _("L3v Rates"),_T("Hz"),L3Labels,
			 L3Labels,L3Captions, L3Vars,L3Table,
			 L3Spec,L3Range,L3Average,L3ScaleType,wxSize(14,20),L3Scale);
  m_L3hInfo = new BarInfo(this,N_L3,L3_PER_ROW, _("L3h Rates"),_T("Hz"),L3Labels,
			 L3Labels,L3Captions, L3Vars,L3Table,
			 L3Spec,L3Range,L3Average,L3ScaleType,wxSize(14,20),L3Scale);
*/


  leftSizer->Add(m_slowInfo,0,wxEXPAND);
  leftSizer->Add(leftMidSizer,1,wxEXPAND);
  leftSizer->Add(m_TempInfo,1,wxEXPAND);
  leftSizer->AddStretchSpacer();
  leftSizer->AddStretchSpacer();
/*
  leftSizer->Add(m_L1Info,1,wxEXPAND);
  leftSizer->Add(m_L1hInfo,1,wxEXPAND);
  //leftSizer->Add(m_L2Info,2,wxEXPAND);
  leftSizer->Add(m_L3Info,1,wxEXPAND);
  leftSizer->Add(m_L3hInfo,1,wxEXPAND);
*/

  m_ScalerInfo=new BarInfo(this,N_SCALER,SCALER_PER_ROW,
			   _T("Scalers"),_T("Hz"),
			   ScalerLabels,
			   ScalerLabels,
			   ScalerCaptions,
			   ScalerVars,
			   ScalerTable,
			   ScalerSpec,
			   ScalerRange,
			   ScalerAverage,
			   ScalerScaleType,
			   wxSize(12,25),
			   ScalerScale);

#if 0  // Dummy scaler entries
  for(int i=0;i<N_SCALER;++i)
    m_ScalerInfo->SetDisplayValue(ScalerLabels[i],(double)((ScalerRange[i][1]-ScalerRange[i][0])/N_SCALER)*(i+1)+ScalerRange[i][0]);
#endif  

  midSizer->Add(m_ScalerInfo,1,wxEXPAND);
/*
  m_ScalerRMSInfo=new BarInfo(this,N_SCALERRMS,SCALERRMS_PER_ROW,
			      _T("Scaler RMS"),_T("Hz"),
			      ScalerRMSLabels,
			      ScalerRMSLabels,
			      ScalerRMSCaptions,
			      ScalerRMSVars,
			      ScalerRMSTable,
			      ScalerRMSSpec,
			      ScalerRMSRange,
			      ScalerRMSAverage,
			      ScalerRMSScaleType,
			      wxSize(12,25),
			      ScalerRMSScale);
  
#if 0  // Dummy scaler RMS entries
  for(int i=0;i<N_SCALERRMS;++i)
    m_ScalerRMSInfo->SetDisplayValue(ScalerRMSLabels[i],(double)((ScalerRMSRange[i][1]-ScalerRMSRange[i][0])/N_SCALERRMS)*(i+1)+ScalerRMSRange[i][0]);
#endif  

  midSizer->Add(m_ScalerRMSInfo,1,wxEXPAND);
*/

  m_RFInfo=new BarInfo(this,N_RFPOWER,RFPOWER_PER_ROW,
		       _T("RF Power [ADC]"),_T(""),
		       RFPowerLabels,
		       RFPowerLabels,
		       RFPowerCaptions,
		       RFPowerVars,
		       RFPowerTable,
		       RFPowerSpec,
		       RFPowerRange,
		       RFPowerAverage,
		       RFPowerScaleType,
		       wxSize(16,25),
		       RFPowerScale);
#if 0  // Dummy RF entries
  for(int i=0;i<N_RFPOWER;++i)
    m_RFInfo->SetDisplayValue(RFPowerLabels[i],(double)((RFPowerRange[i][1]-RFPowerRange[i][0])/N_RFPOWER)*(i+1)+RFPowerRange[i][0]);
#endif

  midSizer->Add(m_RFInfo,1,wxEXPAND);
  //dataSizer->Add(m_RFInfo,1,wxEXPAND);

  m_LogWindow = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,
			       wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);
  m_LogWindow->SetSize(wxSize(200,50));
  m_logTarget = wxLog::SetActiveTarget( new wxLogTextCtrl(m_LogWindow) );
  wxLogMessage(_T("Welcome to SlowMo - ANITA Slow telemetry Display Utility"));

  topSizer->Add(m_LogWindow,0,wxEXPAND);
  
  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);
  Centre();

  // Initialize data handlers
  for(int i=0;i<N_PCKTYPE;++i){
    packetFileList[i]=new wxArrayString();
    packetDirectory[i]=_T("");
    packetList[i]=NULL;
    packetListItems[i]=0;
    packetListAllocSize[i]=0;
    packetListUpdateTime[i]=0;
    needsIndex[i]=false;
  }

  m_currentTimeTimer=new wxTimer(this,CURRENT_TIME_TIMER);
  m_currentTimeTimer->Start(1000);
  m_updateTimer=new wxTimer(this,UPDATE_TIMER);
  m_updateTimer->Start(3000);

  m_xmlHandler=new SlowMoXMLHandler();
  showProgress=false;
  lastSQLRequest=0;  
  if(m_dbHandler){ // DB connection already initiated
    // Start oneshot timer which will check on connection status
    wxTimer *dbTimer=new wxTimer(this,DBCONNECTION_TIMER);
    dbTimer->Start(1,true);
  }else
    m_dbHandler=new DBHandler();
  m_dbTerminal=new DBTerminal(this);


  // Enable/disable appropriate menu items and other initializations
  init();

  // Remove splash
  if(splash){
    splash->Show(false);
    delete splash;
  }
}

// Initialize
void SlowMoFrame::init(){
  packetTable[0]=_T("slow");

  // Could change with cmd line
  m_pFlightDataNext->Enable(false); 
  m_pFlightDataPrev->Enable(false);

  m_pDBDataDisconnect->Enable(false);
  m_pDBDataControl->Enable(false);
  m_pDBDataTerminal->Enable(false);
  m_pDBDataNext->Enable(false); 
  m_pDBDataPrev->Enable(false);

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

  if(!cmdLine->Found(_T("s"),&tmp)) tmp=wxGetCwd();
  packetDirectory[SLOWPCK]=wxFileName::DirName(tmp);
  packetDirectory[SLOWPCK].MakeAbsolute();
  IndexSlowDirectory();
  
  return;
}
  
SlowMoFrame::~SlowMoFrame(){
  m_currentTimeTimer->Stop();
  m_updateTimer->Stop();
  delete wxLog::SetActiveTarget(m_logTarget);
  delete m_xmlHandler;
  delete m_dbHandler;
}

// wxEVT_COMMAND_MENU_SELECTED event handlers
void SlowMoFrame::OnMenuControlOptions( wxCommandEvent& event ){
  if(m_optionsDialog->ShowModal()==wxID_OK){
    // Harvest new settings from turf options dialogs
    m_optionsDialog->ExportOptions();
    SetOptions();
    // Redraw everything
    Refresh(true);
  }else{  // Reset all values to existing ones
    m_optionsDialog->ImportOptions();
  }
}

void SlowMoFrame::OnMenuControlSaveOptions( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("default.src"),_T("SlowMo options(*.src)|*.src"),wxSAVE);
  if (dlg->ShowModal() == wxID_OK) SaveOptions(dlg->GetPath());
  dlg->Destroy();
}
////////////////////june 30 2008 /////////////////amir's addition
void SlowMoFrame::OnMenuSlowMoViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
  // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
  m_slowmoViewer->Show(true);
}
///////////////////////////////////////////////////////
void SlowMoFrame::OnMenuControlLoadOptions( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Load options file"),_T(""),_T(""),_T("SlowMo options(*.src)|*.src"),wxOPEN);
  if (dlg->ShowModal() == wxID_OK) 
    if(LoadOptions(dlg->GetPath()))
      // Redraw everything
      Refresh(true);
  dlg->Destroy();
}

void SlowMoFrame::OnMenuControlQuit( wxCommandEvent& event ){
  Close(true);
}

void SlowMoFrame::OnMenuFlightDataControl(wxCommandEvent &WXUNUSED(event) ){
  m_dbControl->StopTimers();
  m_dbControl->Show(false);
  m_flightControl->Show();
}

void SlowMoFrame::OnMenuFlightDataOpen( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open slow file"),packetDirectory[SLOWPCK].GetPath(),_T(""),_T("Slow files(slow_*.dat*)|slow_*.dat*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if(ReadSlowData(file)){
      packetDirectory[SLOWPCK].SetPath(file.GetPath());
      IndexSlowDirectory();
      int n=packetFileList[SLOWPCK]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(SLOWPCK,n);
    }else{
      m_pFlightDataNext->Enable(false);
      m_pFlightDataPrev->Enable(false);
    }
  }
  dlg->Destroy();
}

void SlowMoFrame::OnMenuFlightDataNext( wxCommandEvent& event ){
  m_flightControl->Next(SLOWPCK,event);
}

void SlowMoFrame::OnMenuFlightDataPrev( wxCommandEvent& event ){
  m_flightControl->Prev(SLOWPCK,event);
}

void SlowMoFrame::OnMenuDBDataConnect( wxCommandEvent &WXUNUSED(event) ){
  if(m_dbHandler->Connect(SlowMoOpts::dbHost,SlowMoOpts::dbUser,
			  SlowMoOpts::dbPwd,SlowMoOpts::dbName)){
    wxTimerEvent e;
    CheckDBConnection(e);
  }
}

void SlowMoFrame::OnMenuDBDataDisconnect( wxCommandEvent &WXUNUSED(event) ){
  m_dbHandler->Disconnect();
  m_pDBDataConnect->Enable(true);
  m_pDBDataDisconnect->Enable(false);
  m_pDBDataControl->Enable(false);
  m_pDBDataTerminal->Enable(false);
  m_pDBDataNext->Enable(false);
  m_pDBDataPrev->Enable(false);
  m_dbTerminal->Show(false);
  SetStatusText(_T("Disconnected from database"),2);
}

void SlowMoFrame::OnMenuDBDataControl( wxCommandEvent &WXUNUSED(event) ){
  m_flightControl->StopTimers();
  m_flightControl->Show(false);
  m_dbControl->Show();
}

void SlowMoFrame::OnMenuDBDataTerminal(wxCommandEvent &WXUNUSED(event) ){
  m_dbTerminal->Show(true);
}

void SlowMoFrame::OnMenuDBDataNext( wxCommandEvent& event ){
  m_dbControl->Next(SLOWPCK,event);
}

void SlowMoFrame::OnMenuDBDataPrev( wxCommandEvent& event ){
  m_dbControl->Prev(SLOWPCK,event);
  m_dbControl->StopTimers(0);
}

void SlowMoFrame::OnMenuDBDataLast( wxCommandEvent& event ){
  m_dbControl->Last(SLOWPCK,event);
}

void SlowMoFrame::OnMenuDBDataAuto( wxCommandEvent& event ){
  m_dbControl->InverseTimers();
}
void SlowMoFrame::OnMenuHelpAbout( wxCommandEvent& event ){
  wxMessageBox(_T("ANITA Slow Monitor\nsvnversion: ddm_placeholder_for_svnversion\n(c) Predrag Miocinovic 2014"),_T("About"),wxOK);
}
void SlowMoFrame::OnCurrentTimeTimer( wxTimerEvent &WXUNUSED(event) ){
  m_slowInfo->UpdateCurrentTime();
}
void SlowMoFrame::OnUpdateTimer( wxTimerEvent &WXUNUSED(event) ){
  static int task=0;
  static int ten=0;
  time_t now=time(NULL);
  if(m_dbHandler->IsConnected() && (now-lastSQLRequest)<3600 && ++ten==10){
    ReceiveNotifications();
    ten=0;
  }

  // Update open directories
  switch(task){
  case SLOWPCK:
    IndexSlowDirectory(false);
    if(needsIndex[task]){
      showProgress=false;
      IndexDBPackets(task,false);
      needsIndex[task]=false;
    }
    break;
  default:
    task=-1;
    break;
  }
  ++task;
};

void SlowMoFrame::ReadFlightData(wxCommandEvent &event){
  wxFileName file=event.GetString();
  int order=(int)event.GetClientData();
  switch(event.GetId()-CONTROLWIN_OFFSET){
  case SLOWPCK: 
    ReadSlowData(file,order);
    break;
  default:
    wxMessageBox(_T("Received request for loading of unknown packet file"),_T("Packet file request error"),wxOK|wxICON_ERROR);
  }
}

void SlowMoFrame::RequestPacket(wxCommandEvent &event){
  lastSQLRequest=time(NULL);
  int type=event.GetId()-CONTROLWIN_OFFSET;
  sqlRequestType.Add(type);
  wxString nbufString=event.GetString();
  if(type<0 || type>N_PCKTYPE-1){
    wxMessageBox(_T("Received request of unknown packet file"),_T("Packet DB request error"),wxOK|wxICON_ERROR);
    return;
  }
  // Prepare SQL request
  wxString cmd;
  cmd.Printf(_T("SELECT * FROM %s WHERE time=%s;"),
	     packetTable[type].c_str(),nbufString.c_str());
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
}

void SlowMoFrame::OnSQLRequest(wxCommandEvent &event){
  wxObject *requestor=event.GetEventObject();
  sqlparser parser=(sqlparser)event.GetClientData();
  wxString sql=event.GetString();
  if(SlowMoOpts::sqlDump) wxLogMessage(sql);
  if(!m_dbHandler->SendQuery(sql,requestor,parser)) sqlRequestType.Empty();
}
  
void SlowMoFrame::CheckDBConnection( wxTimerEvent &WXUNUSED(event) ){
  bool status=m_dbHandler->FinalizeConnection();
  if(status){
    m_pDBDataConnect->Enable(false);
    m_pDBDataDisconnect->Enable(true);
    m_pDBDataControl->Enable(true);
    m_pDBDataTerminal->Enable(true);
    m_pDBDataNext->Enable(true);
    m_pDBDataPrev->Enable(true);
    wxString status;
    status.Printf(_T("Connected to database %s"),m_dbHandler->GetDBName().c_str());
    SetStatusText(status,2);
    SetStatusText(m_dbHandler->getHost().c_str(),0); //show host IP. peng
    // Setup notification requests
    showProgress=true;
    for(int i=0;i<N_PCKTYPE;++i){
      SendListen(i);
      IndexDBPackets(i,true);
      needsIndex[i]=false;
    }
    SetStatusText(_T("DB indexing 0% complete"),1);
  }else{
    wxLogError(_T("Failed to connect to database. Check connection parameters."));
  }
}

void SlowMoFrame::SetOptions(){
  using namespace SlowMoOpts;
  for(int i=0;i<N_TEMP;++i){
    m_TempInfo->SetDisplayRangeSpec(TempLabels[i],TempSpec[i]);
    m_TempInfo->SetDisplayRange(TempLabels[i],TempRange[i][0],TempRange[i][1]);
    m_TempInfo->SetDisplayScaleType(TempLabels[i],TempScaleType[i]);
    m_TempInfo->SetDisplayAverage(TempLabels[i],TempAverage[i]);
  }

  for(int i=0;i<N_POWER;++i){
    m_PowerInfo->SetDisplayRangeSpec(PowerLabels[i],PowerSpec[i]);
    m_PowerInfo->SetDisplayRange(PowerLabels[i],PowerRange[i][0],PowerRange[i][1]);
    m_PowerInfo->SetDisplayScaleType(PowerLabels[i],PowerScaleType[i]);
    m_PowerInfo->SetDisplayAverage(PowerLabels[i],PowerAverage[i]);
  }

/*
  for(int i=0;i<N_L1;++i){
    m_L1Info->SetDisplayRangeSpec(L1Labels[i],L1Spec[i]);
    m_L1Info->SetDisplayRange(L1Labels[i],L1Range[i][0],L1Range[i][1]);
    m_L1Info->SetDisplayScaleType(L1Labels[i],L1ScaleType[i]);
    m_L1Info->SetDisplayAverage(L1Labels[i],L1Average[i]);
  }
  for(int i=0;i<N_L1;++i){
    m_L1hInfo->SetDisplayRangeSpec(L1Labels[i],L1Spec[i]);
    m_L1hInfo->SetDisplayRange(L1Labels[i],L1Range[i][0],L1Range[i][1]);
    m_L1hInfo->SetDisplayScaleType(L1Labels[i],L1ScaleType[i]);
    m_L1hInfo->SetDisplayAverage(L1Labels[i],L1Average[i]);
  }
*/
/*
  for(int i=0;i<N_L2;++i){
    m_L2Info->SetDisplayRangeSpec(L2Labels[i],L2Spec[i]);
    m_L2Info->SetDisplayRange(L2Labels[i],L2Range[i][0],L2Range[i][1]);
    m_L2Info->SetDisplayScaleType(L2Labels[i],L2ScaleType[i]);
    m_L2Info->SetDisplayAverage(L2Labels[i],L2Average[i]);
  }
  for(int i=0;i<N_L3;++i){
    m_L3Info->SetDisplayRangeSpec(L3Labels[i],L3Spec[i]);
    m_L3Info->SetDisplayRange(L3Labels[i],L3Range[i][0],L3Range[i][1]);
    m_L3Info->SetDisplayScaleType(L3Labels[i],L3ScaleType[i]);
    m_L3Info->SetDisplayAverage(L3Labels[i],L3Average[i]);
  }
  for(int i=0;i<N_L3;++i){
    m_L3hInfo->SetDisplayRangeSpec(L3Labels[i],L3Spec[i]);
    m_L3hInfo->SetDisplayRange(L3Labels[i],L3Range[i][0],L3Range[i][1]);
    m_L3hInfo->SetDisplayScaleType(L3Labels[i],L3ScaleType[i]);
    m_L3hInfo->SetDisplayAverage(L3Labels[i],L3Average[i]);
  }

  m_L1Info->SetScale(L1Scale);
  m_L1hInfo->SetScale(L1Scale);
//  m_L2Info->SetScale(L2Scale);
  m_L3Info->SetScale(L3Scale);
  m_L3hInfo->SetScale(L3Scale);
*/

  for(int i=0;i<N_RFPOWER;++i){
    m_RFInfo->SetDisplayRangeSpec(RFPowerLabels[i],RFPowerSpec[i]);
    m_RFInfo->SetDisplayRange(RFPowerLabels[i],RFPowerRange[i][0],RFPowerRange[i][1]);
    m_RFInfo->SetDisplayScaleType(RFPowerLabels[i],RFPowerScaleType[i]);
    m_RFInfo->SetDisplayAverage(RFPowerLabels[i],RFPowerAverage[i]);
  }
  for(int i=0;i<N_SCALER;++i){
    m_ScalerInfo->SetDisplayRangeSpec(ScalerLabels[i],ScalerSpec[i]);
    m_ScalerInfo->SetDisplayRange(ScalerLabels[i],ScalerRange[i][0],ScalerRange[i][1]);
    m_ScalerInfo->SetDisplayScaleType(ScalerLabels[i],ScalerScaleType[i]);
    m_ScalerInfo->SetDisplayAverage(ScalerLabels[i],ScalerAverage[i]);
  }
/*
  for(int i=0;i<N_SCALERRMS;++i){
    m_ScalerRMSInfo->SetDisplayRangeSpec(ScalerRMSLabels[i],ScalerRMSSpec[i]);
    m_ScalerRMSInfo->SetDisplayRange(ScalerRMSLabels[i],ScalerRMSRange[i][0],ScalerRMSRange[i][1]);
    m_ScalerRMSInfo->SetDisplayScaleType(ScalerRMSLabels[i],ScalerRMSScaleType[i]);
    m_ScalerRMSInfo->SetDisplayAverage(ScalerRMSLabels[i],ScalerRMSAverage[i]);
  }
*/
  m_readDelay=readDelay;
  // Reset timer steps
  m_flightControl->AdjustTimer(SLOWPCK,m_readDelay);
  m_dbControl->AdjustTimer(SLOWPCK,m_readDelay);

  m_RFInfo->SetScale(RFPowerScale);
  m_ScalerInfo->SetScale(ScalerScale);
 // m_ScalerRMSInfo->SetScale(ScalerRMSScale);

  return;
}

// Save current display options
bool SlowMoFrame::SaveOptions(const wxString &file){
  using namespace SlowMoOpts;
  // Open file
  wxFile saveFile(file.c_str(),wxFile::write);
  if (!saveFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open file %s for writing"),file.c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  wxString tmp;
  tmp.Printf(_T("%d"),readDelay);
  xmlTagValuePair **extra;
  extra = new xmlTagValuePair*[3];
  extra[0]= new xmlTagValuePair(_T("delay"),tmp);
  switch(rfPowerType){
  case dBm: extra[1]= new xmlTagValuePair(_T("rf"),_T("dBm")); break;
  case T: extra[1]= new xmlTagValuePair(_T("rf"),_T("t")); break;
  case ADC: default: extra[1]= new xmlTagValuePair(_T("rf"),_T("adc")); break;
  }
  extra[2]=NULL;

  m_xmlHandler->OpenTag(saveFile,_T("slowmo"),extra);

  delete extra[0];
  delete extra[1];
  delete[] extra;

  xmlTagValuePair **scale;
  scale = new xmlTagValuePair*[2];
  scale[0]= new xmlTagValuePair(_T("scale"));
  scale[1]=NULL;

  scale[0]->value=EventRateScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("rate"),2,EventRateLabels,EventRateSpec,EventRateRange,EventRateScaleType,EventRateAverage,scale);
  scale[0]->value=TempScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("temp"),N_TEMP,TempLabels,TempSpec,TempRange,TempScaleType,TempAverage,scale);
  scale[0]->value=PowerScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("power"),N_POWER,PowerLabels,PowerSpec,PowerRange,PowerScaleType,PowerAverage,scale);
  //scale[0]->value=L1Scale;
  //m_xmlHandler->WriteBarItemBlock(saveFile,_T("l1"),N_L1,L1Labels,L1Spec,L1Range,L1ScaleType,L1Average,scale);
  //scale[0]->value=L2Scale;
  //m_xmlHandler->WriteBarItemBlock(saveFile,_T("l2"),N_L2,L2Labels,L2Spec,L2Range,L1ScaleType,L2Average,scale);
  //scale[0]->value=L3Scale;
  //m_xmlHandler->WriteBarItemBlock(saveFile,_T("l3"),N_L3,L3Labels,L3Spec,L3Range,L3ScaleType,L3Average,scale);
  
  scale[0]->value=RFPowerScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("rfpower"),N_RFPOWER,RFPowerLabels,RFPowerSpec,RFPowerRange,RFPowerScaleType,RFPowerAverage,scale);
  scale[0]->value=ScalerScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("scaler"),N_SCALER,ScalerLabels,ScalerSpec,ScalerRange,ScalerScaleType,ScalerAverage,scale);
  //scale[0]->value=ScalerRMSScale;
  //m_xmlHandler->WriteBarItemBlock(saveFile,_T("scalerrms"),N_SCALERRMS,ScalerRMSLabels,ScalerRMSSpec,ScalerRMSRange,ScalerRMSScaleType,ScalerRMSAverage,scale);

  // Clean up
  delete scale[0];
  delete[] scale;

  // Prepare extra tags for db line
  extra = new xmlTagValuePair*[4];
  extra[0]= new xmlTagValuePair(_T("host"),dbHost);
  extra[1]= new xmlTagValuePair(_T("user"),dbUser);
  extra[2]= new xmlTagValuePair(_T("name"),dbName);
  extra[3]= new xmlTagValuePair(_T("sql"),sqlDump?_T("on"):_T("off"));  
  extra[4]=NULL;
  m_xmlHandler->OpenTag(saveFile,_T("db"),extra,true);
  for(int i=0;i<4;++i) delete extra[i];  
  delete[] extra;
  
  m_xmlHandler->CloseTag(saveFile,_T("slowmo"));
    
  saveFile.Close();
  SetStatusText(_T("Saved options"),2);

  return true;
}

// Save current display options
bool SlowMoFrame::LoadOptions(const wxString &file){
  using namespace SlowMoOpts;
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
  XML_SetUserData(parser, (void*)m_xmlHandler);
  XML_SetElementHandler(parser, SlowMoXMLHandler::xmlSlowMoStart,
			SlowMoXMLHandler::xmlSlowMoEnd);

  wxString str;
  for (str = loadFile.GetFirstLine(); !loadFile.Eof();  str = loadFile.GetNextLine() ){
    if (!XML_Parse(parser, str.mb_str(), str.Length(),0)){
      wxLogError(_T("XML parse error at line %d:\n%s"),
		 (int)XML_GetCurrentLineNumber(parser),
		 XML_ErrorString(XML_GetErrorCode(parser)));
      return false;
    }else if(!m_xmlHandler->Ok()){
      wxLogError(_T("XML format error on line: %s"),str.c_str());
      return false;
    }
  }
  if(! XML_Parse(parser, str.mb_str(), str.Length(),1)){
    wxLogError(_T("XML parse error at line %d:\n%s"),
	       (int)XML_GetCurrentLineNumber(parser),
	       XML_ErrorString(XML_GetErrorCode(parser)));
    return false;
  }else if(!m_xmlHandler->Ok()){
    wxLogError(_T("XML format error on line: %s"),str.c_str());
    return false;
  }


  // Clean up
  XML_ParserFree(parser);
  loadFile.Close();

  if(m_xmlHandler->Ok()){
    // Enforce new options
    SetOptions();
    m_optionsDialog->ImportOptions();
    SetStatusText(_T("Loaded new options"),2);
  }else
    SetStatusText(_T("Error loading options"),2);
  
  return m_xmlHandler->Ok();
}

// Loads slow data file in flight format
bool SlowMoFrame::ReadSlowData(const wxFileName &file,int order){
  using namespace SlowMoOpts;

  // Open file
  wxFile slowFile(file.GetFullPath().c_str(),wxFile::read);
  if (!slowFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open slow file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file
  SlowRateFull_t slowdata;
  if(!m_fileHandler->ReadFromFile(slowFile,&slowdata,sizeof(SlowRateFull_t),order)){
    wxMessageBox(_T("Slow rate packet file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    slowFile.Close();
    return false;
  }
  slowFile.Close();

  // Fill in data here
  ////////////////////////amir's addition july 12 08///////////////
m_slowmoViewer->SlowMoData(slowdata);
  ///////////////////////////////////////////////////
  m_flightControl->SetTime(SLOWPCK,slowdata.unixTime);
  m_slowInfo->SetTime(slowdata.unixTime);
  m_slowInfo->SetEventNumber(slowdata.rf.eventNumber);
  m_slowInfo->SetLocation(slowdata.hk.latitude,slowdata.hk.longitude,slowdata.hk.altitude);

  m_EventRateInfo->SetDisplayValue(EventRateLabels[0],slowdata.rf.eventRate1Min/8.);
  m_EventRateInfo->SetDisplayValue(EventRateLabels[1],slowdata.rf.eventRate10Min/8.);

  for(int i=0;i<N_TEMP;++i)
    m_TempInfo->SetDisplayValue(TempLabels[i],slowdata.hk.temps[i]);
  for(int i=0;i<N_POWER;++i)
    m_PowerInfo->SetDisplayValue(PowerLabels[i],slowdata.hk.powers[i]);
/*   peng: TODO
  for(int i=0;i<N_L1;++i)
    m_L1Info->SetDisplayValue(L1Labels[i],
			      slowdata.rf.avgL1Rates[i]*512u); // Multiply by 512

  
  for(int i=0;i<L2_PER_ROW;++i){
    //  m_L2Info->SetDisplayValue(L2Labels[i],
    //			      slowdata.rf.avgUpperL2Rates[i]*64u); // Multiply by 64
   m_L2Info->SetDisplayValue(L2Labels[i],
			      slowdata.rf.avgL2Rates[i]*64u); // Multiply by 64
    // m_L2Info->SetDisplayValue(L2Labels[i+L2_PER_ROW],
    //			      slowdata.rf.avgLowerL2Rates[i]*64u); // Multiply by 64
  }
  
  for(int i=0;i<N_L3;++i)
    m_L3Info->SetDisplayValue(L3Labels[i],slowdata.rf.avgL3Rates[i]/32.0);
  */
   for(int i=0;i<SCALER_PER_ROW;++i)
     for(int j=0;j<N_SCALER/SCALER_PER_ROW;++j)
       m_ScalerInfo->SetDisplayValue(ScalerLabels[SCALERId(i,j)],
				     slowdata.rf.avgScalerRates[i][j]*128*1e3);
/*   peng: TODO
   for(int i=0;i<SCALERRMS_PER_ROW;++i)
     for(int j=0;j<N_SCALERRMS/SCALERRMS_PER_ROW;++j)
       m_ScalerRMSInfo->SetDisplayValue(ScalerRMSLabels[SCALERRMSId(i,j)],
					slowdata.rf.rmsScalerRates[i][j]*1e3);
  */

   for(int i=0;i<RFPOWER_PER_ROW;++i)
     for(int j=0;j<N_RFPOWER/RFPOWER_PER_ROW;++j){
       double rf;
       switch(rfPowerType){
       case dBm:
	 rf = rfpower_dBm(j,i,slowdata.rf.rfPwrAvg[j][i]&0x7FFF);
	 break;
       case T: 
	 rf=rfpower_T(j, i, slowdata.rf.rfPwrAvg[j][i]&0x7FFF);
	 break;
       case ADC:
       default:
	 rf = slowdata.rf.rfPwrAvg[j][i]&0x7FFF;
	 break;
       }
       m_RFInfo->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
     }

  Refresh(true);
  wxString status=file.GetFullName();
  status << ' ' << order+1;
  SetStatusText(status,1);
  return true;
}


void SlowMoFrame::IndexSlowDirectory(bool full){
  wxString filter=_T("slow_*.dat*");
  wxString dirname=packetDirectory[SLOWPCK].GetPath();;
  wxArrayString::CompareFunction comp=singleIndexSortFunction;

  // Index current directory
  wxDir dir;
  if(full){ // Full indexing
    packetFileList[SLOWPCK]->Empty();
    dir.GetAllFiles(dirname,packetFileList[SLOWPCK],filter,wxDIR_FILES);
    if(packetFileList[SLOWPCK]->GetCount()){
      packetFileList[SLOWPCK]->Sort(comp);
      m_flightControl->SetList(SLOWPCK,packetFileList[SLOWPCK],1000);
      m_pFlightDataNext->Enable(true);
      m_pFlightDataPrev->Enable(true);
      wxLogMessage(_T("[SlowMoFrame::IndexSlowDirectory] Slow files indexed."));
    }else{
      m_pFlightDataNext->Enable(false);
      m_pFlightDataPrev->Enable(false);
    }
  }else{
    wxArrayString newFiles;
    dir.GetAllFiles(dirname,&newFiles,filter,wxDIR_FILES);
    if(newFiles.GetCount()>packetFileList[SLOWPCK]->GetCount()){
      newFiles.Sort(comp);
      for(unsigned int i=packetFileList[SLOWPCK]->GetCount();i<newFiles.GetCount();++i)
	packetFileList[SLOWPCK]->Add(newFiles[i]);
      wxLogMessage(_T("[SlowMoFrame::IndexSlowDirectory] Updated list of slow files."));
    }
  }
  return;
}

void SlowMoFrame::sqlParser(PGresult *result){
  if(!sqlRequestType.GetCount())
    wxLogError(_T("[SlowMoFrame::sqlParser] Received SQL query result, but no request types in the buffer."));
  else{
    int type=sqlRequestType[0];
    sqlRequestType.RemoveAt(0);
    ExecStatusType status=PQresultStatus(result);
    if(status==PGRES_COMMAND_OK){ 
      //wxLogMessage(_T("[SlowMoFrame::sqlParser] OK"));
    }else if(status!=PGRES_TUPLES_OK){
      wxLogError(_U(PQresStatus(status)));
      wxLogError(_U(PQresultErrorMessage(result)));
    }else if(PQntuples(result)==0 && type<INDEX_OFFSET){
      wxLogError(_T("[SlowMoFrame::sqlParser] Received SQL query result w/o record; type %d"),type);
    }else{
      //wxLogMessage(_T("[SlowMoFrame::sqlParser] Received SQL data type %d"),type);
      switch(type){
      case SLOWPCK: ParseSlowPacket(result); Refresh(); break;
      case SLOWPCK+INDEX_OFFSET: 
	StorePacketList(result,type-INDEX_OFFSET,false);
	if(showProgress){
	  wxString status;
	  status.Printf(_T("DB indexing %d%% complete"),(type-INDEX_OFFSET+1)*100/N_PCKTYPE);
	  SetStatusText(status,1);
	}
	break;
      default:
	wxLogError(_T("[SlowMoFrame::sqlParser] Received results of unknown SQL request type %d"),type);
	break;
      }
    }
  }
  PQclear(result);
}

void SlowMoFrame::ParseSlowPacket(PGresult *result){
  using namespace SlowMoOpts;
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[SlowMoFrame::ParseSlowPacket] More than one slow record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[SlowMoFrame::ParseSlowPacket] No slow record received."));
    return;
  }

  unsigned long t=0;
  if((coln=PQfnumber(result,"time"))>=0) t=atol(PQgetvalue(result,0,coln));
  m_slowInfo->SetTime(t);
  m_dbControl->SetTime(SLOWPCK,t);

  if((coln=PQfnumber(result,"evnum"))>=0) m_slowInfo->SetEventNumber(atol(PQgetvalue(result,0,coln)));

  float lat=0,lon=0,alt=0;
  if((coln=PQfnumber(result,"latitude"))>=0) lat=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"longitude"))>=0) lon=atof(PQgetvalue(result,0,coln));
  if((coln=PQfnumber(result,"altitude"))>=0) alt=atof(PQgetvalue(result,0,coln));
  if (alt<-5000) alt+=65535;   //convert  according to AWARE.
  m_slowInfo->SetLocation(lat,lon,alt);
  
  if((coln=PQfnumber(result,"rate1"))>=0) m_EventRateInfo->SetDisplayValue(EventRateLabels[0],0.5*atof(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"rate10"))>=0) m_EventRateInfo->SetDisplayValue(EventRateLabels[1],0.5*atof(PQgetvalue(result,0,coln)));

  // Temperatures
  double temp[8];
  if((coln=PQfnumber(result,"temp"))>=0 && ParseDoubleSQLArray(result,temp,8,coln)){
    for(int i=0;i<4;++i)
      m_TempInfo->SetDisplayValue(TempLabels[i],temp[i]);
  }
  // Powers
  //long power[N_POWER];
  //if((coln=PQfnumber(result,"power"))>=0 && ParseLongSQLArray(result,power,N_POWER,coln)){
  //  for(int i=0;i<N_POWER;++i)
  //    m_PowerInfo->SetDisplayValue(PowerLabels[i],power[i]);
  //}
  if((coln=PQfnumber(result,"ppvv"))>=0) m_PowerInfo->SetDisplayValue(PowerLabels[0],1.054*atof(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"p24v"))>=0) m_PowerInfo->SetDisplayValue(PowerLabels[1],atof(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"bati"))>=0) m_PowerInfo->SetDisplayValue(PowerLabels[2],atof(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"p24i"))>=0) m_PowerInfo->SetDisplayValue(PowerLabels[3],1.5*atof(PQgetvalue(result,0,coln)));

  // SURF housekeeping
  long *scaler[N_SCALER/SCALER_PER_ROW];
  long *rfpow[N_RFPOWER/RFPOWER_PER_ROW];
  long dimScaler[N_SCALER/SCALER_PER_ROW];
  for(int i=0;i<N_SCALER/SCALER_PER_ROW;++i){
    dimScaler[i]=SCALER_PER_ROW;
    scaler[i]=new long[SCALER_PER_ROW];
  }
/* peng
  long *scalerRMS[SCALERRMS_PER_ROW];
  long dimScalerRMS[SCALERRMS_PER_ROW];
  for(int i=0;i<SCALERRMS_PER_ROW;++i){
    dimScalerRMS[i]=N_SCALERRMS/SCALERRMS_PER_ROW;
    scalerRMS[i]=new long[N_SCALERRMS/SCALERRMS_PER_ROW];
  }
*/
  long dimRFpow[N_RFPOWER/RFPOWER_PER_ROW];
  for(int i=0;i<N_RFPOWER/RFPOWER_PER_ROW;++i){
    dimRFpow[i]=RFPOWER_PER_ROW;
    rfpow[i]=new long[RFPOWER_PER_ROW];
  }
  if((coln=PQfnumber(result,"avgscaler"))>=0 && ParseLongSQLArray2D(result,scaler,N_SCALER/SCALER_PER_ROW, dimScaler,coln)){
    for(int i=0;i<SCALER_PER_ROW;++i)
      for(int j=0;j<N_SCALER/SCALER_PER_ROW;++j)
	m_ScalerInfo->SetDisplayValue(ScalerLabels[SCALERId(i,j)],scaler[j][i]*1e3);
  }
/* commented out by peng
  if((coln=PQfnumber(result,"rmsscaler"))>=0 && ParseLongSQLArray2D(result,scalerRMS,SCALERRMS_PER_ROW, dimScalerRMS,coln)){
    for(int i=0;i<SCALERRMS_PER_ROW;++i)
      for(int j=0;j<N_SCALERRMS/SCALERRMS_PER_ROW;++j)
	m_ScalerRMSInfo->SetDisplayValue(ScalerRMSLabels[SCALERRMSId(i,j)],scalerRMS[i][j]*1e3);
  }
*/
  if((coln=PQfnumber(result,"avgrfpow"))>0 && ParseLongSQLArray2D(result,rfpow,N_RFPOWER/RFPOWER_PER_ROW, dimRFpow,coln)){
    for(int i=0;i<RFPOWER_PER_ROW;++i)
      for(int j=0;j<N_RFPOWER/RFPOWER_PER_ROW;++j){
	double rf;
	switch(rfPowerType){
	case dBm:
	  rf = rfpower_dBm(j,i,rfpow[j][i]&0x7FFF);
	  break;
	case ADC:
	
	  rf = rfpow[j][i]&0x7FFF;
	  break;
	case T: 
	  default:
	  rf=rfpower_T(j, i, rfpow[j][i]&0x7FFF);
	  break;
	}
	m_RFInfo->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
      }
  }

  // Clear memory
  for(int i=0;i<N_SCALER/SCALER_PER_ROW;++i) delete[] scaler[i];
//  for(int i=0;i<SCALERRMS_PER_ROW;++i) delete[] scalerRMS[i];
  for(int i=0;i<N_RFPOWER/RFPOWER_PER_ROW;++i) delete[] rfpow[i];
  
  wxString status;
  status.Printf(_T("SLOW %ld"),t);
  SetStatusText(status,1);
  return;
} 

void SlowMoFrame::SendListen(int type){
  wxString cmd;
  cmd << _T("LISTEN ") << packetTable[type] << ';';
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlRequestType.Add(LISTEN);
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
}

void SlowMoFrame::ReceiveNotifications(){

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

void SlowMoFrame::IndexDBPackets(int type,bool full){
  if(full){                 // Full indexing
    if(packetList[type]){ 
      delete[] packetList[type]; 
      m_dbControl->SetList(type,NULL,0);  // Invalidate current list
      packetListAllocSize[type]=0;
      packetListItems[type]=0;
      packetListUpdateTime[type]=0;
    }
  }
  // Prepare SQL request
  sqlRequestType.Add(type+INDEX_OFFSET);
  wxString cmd;
  cmd.Printf(_T("SELECT time,time,now FROM %s WHERE now>=%ld ORDER BY now DESC;"),
	     packetTable[type].c_str(),packetListUpdateTime[type]);
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
  return;
}

bool SlowMoFrame::ReallocateList(int type,int n){
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
    delete[] packetList[type];
    packetList[type]=newList;
  }

  return true;
}

void SlowMoFrame::StorePacketList(PGresult *result,int type,bool haveus){
  int n=PQntuples(result);
  if(n<=0) return;
  m_dbControl->MarkIndex(type);
  ReallocateList(type,n);
  int timecol=PQfnumber(result,"time");
  int uscol=(haveus?PQfnumber(result,"us"):-1);
  int nbufcol=PQfnumber(result,"time");
  int nowcol=PQfnumber(result,"now");
  if(timecol<0 || nbufcol<0 || (haveus && uscol<0) ||  nowcol<0){
    wxLogError(_T("[SlowMoFrame::StorePacketList] Bad columns returned; type %d"),type);
    return;
  }
  packetListUpdateTime[type]=wxMax(atol(PQgetvalue(result,0,nowcol)),packetListUpdateTime[type]);
  for(int i=0;i<n;++i){
    packetList[type][i+packetListItems[type]].time=atol(PQgetvalue(result,i,timecol));
    packetList[type][i+packetListItems[type]].us=(haveus?atol(PQgetvalue(result,i,uscol)):-1);
    packetList[type][i+packetListItems[type]].nbuf=atol(PQgetvalue(result,i,nbufcol));
  }
  packetListItems[type]+=n;
  qsort(packetList[type],packetListItems[type],sizeof(dbRef),dbRefComp);
  m_dbControl->SetList(type,packetList[type],packetListItems[type]);
}


