/*
 * TrigMonFrame.cpp
 *
 * Definitions of TrigMon main frame, includes definitions of all menus and
 * children, as well as file and DB query parsing and indexing. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TrigMonFrame.h"
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
#include "TrigMonFrame.h"
#include "sort.h"
#include "DBTerminal.h"
#include <includes/anitaStructures.h>
#include "rfpower_cal.h"

BEGIN_EVENT_TABLE( TrigMonFrame, wxFrame )
  EVT_MENU( MENU_CONTROL_PANEL, TrigMonFrame::OnMenuControlPanel )
  EVT_MENU( MENU_CONTROL_OPTIONS, TrigMonFrame::OnMenuControlOptions )
  EVT_MENU( MENU_CONTROL_SAVEOPTIONS, TrigMonFrame::OnMenuControlSaveOptions )
  EVT_MENU( MENU_CONTROL_LOADOPTIONS, TrigMonFrame::OnMenuControlLoadOptions )
  EVT_MENU( MENU_CONTROL_QUIT, TrigMonFrame::OnMenuControlQuit )
  
  EVT_MENU( MENU_TURF_OPEN, TrigMonFrame::OnMenuTurfOpen )
  EVT_MENU( MENU_TURF_OPEN_RAW, TrigMonFrame::OnMenuTurfRawOpen )
  EVT_MENU( MENU_TURF_NEXT, TrigMonFrame::OnMenuTurfNext )
  EVT_MENU( MENU_TURF_PREV, TrigMonFrame::OnMenuTurfPrev )

  EVT_MENU( MENU_SURF_OPEN, TrigMonFrame::OnMenuSurfOpen )
  EVT_MENU( MENU_SURF_OPEN_RAW, TrigMonFrame::OnMenuSurfRawOpen )
  EVT_MENU( MENU_SURF_NEXT, TrigMonFrame::OnMenuSurfNext )
  EVT_MENU( MENU_SURF_PREV, TrigMonFrame::OnMenuSurfPrev )

  EVT_MENU( MENU_DBDATA_CONNECT, TrigMonFrame::OnMenuDBDataConnect )
  EVT_MENU( MENU_DBDATA_DISCONNECT, TrigMonFrame::OnMenuDBDataDisconnect )
  EVT_MENU( MENU_DBDATA_NEXT, TrigMonFrame::OnMenuDBDataNext )
  EVT_MENU( MENU_DBDATA_PREV, TrigMonFrame::OnMenuDBDataPrev )
  EVT_MENU( MENU_DBDATA_LAST, TrigMonFrame::OnMenuDBDataLast )
  EVT_MENU( MENU_DBDATA_AUTO, TrigMonFrame::OnMenuDBDataAuto )
  EVT_MENU( MENU_DBDATA_CONTROL, TrigMonFrame::OnMenuDBDataControl )
  EVT_MENU( MENU_DBDATA_TERMINAL, TrigMonFrame::OnMenuDBDataTerminal )
    
  EVT_MENU( MENU_HELP_ABOUT, TrigMonFrame::OnMenuHelpAbout )
//////////////////////////amir's addition july9 08////////////////
 EVT_MENU( MENU_TURF_VIEWER, TrigMonFrame::OnMenuTurfViewer)//amir's addition
 EVT_MENU( MENU_SURF_VIEWER, TrigMonFrame::OnMenuSurfViewer)//amir's addition
////////////////////////////////////////////////////////////////////
  EVT_TIMER( UPDATE_TIMER, TrigMonFrame::OnUpdateTimer )
  EVT_TIMER( CURRENT_TIME_TIMER, TrigMonFrame::OnCurrentTimeTimer )
  EVT_TIMER( DBCONNECTION_TIMER, TrigMonFrame::CheckDBConnection )

  EVT_COMMAND_RANGE(INT_MIN,-2,wxSQL_REQUEST,TrigMonFrame::OnSQLRequest)
  EVT_COMMAND_RANGE(SURFPCK+CONTROLWIN_OFFSET,TURFPCK+CONTROLWIN_OFFSET,wxLOAD_FILE,TrigMonFrame::ReadFlightData)
  EVT_COMMAND_RANGE(SURFPCK+CONTROLWIN_OFFSET,TURFPCK+CONTROLWIN_OFFSET,wxREQUEST_PACKET,TrigMonFrame::RequestPacket)

END_EVENT_TABLE()

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

// Constructor
TrigMonFrame::TrigMonFrame( wxWindow* parent, int argc, char **argv, 
			    wxWindowID id, 
			    const wxPoint& pos, const wxSize& size){
  using namespace TrigMonOpts;

  // Here we need to parse the command line
  cmdLine=new wxCmdLineParser(argc,argv);
  static const wxCmdLineEntryDesc cmdLineDesc[] ={
    { wxCMD_LINE_OPTION, _T("a"), _T("autoturf"),  _T("Automatically load TURF files with specified delay in seconds"), wxCMD_LINE_VAL_NUMBER,},
    { wxCMD_LINE_OPTION, _T("A"), _T("autosurf"),  _T("Automatically load SURF files with specified delay in seconds"), wxCMD_LINE_VAL_NUMBER,},
    { wxCMD_LINE_OPTION, _T("s"), _T("surfdir"), _T("Flight format SURF data directory") },
    { wxCMD_LINE_OPTION, _T("t"), _T("turfdir"), _T("Flight format TURF data directory") },
    { wxCMD_LINE_OPTION, _T("S"), _T("rawsurfdir"), _T("Raw format SURF data directory") },
    { wxCMD_LINE_OPTION, _T("T"), _T("rawturfdir"), _T("Raw format TURF data directory") },
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
  if (bitmap.LoadFile(_T("trigmon_splash.png"), wxBITMAP_TYPE_PNG)){
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
  m_pControlMenu->Append(MENU_CONTROL_PANEL, _("Control Panel"), _T("Open flight data control panel"));
  m_pControlMenu->AppendSeparator();
  m_pControlMenu->Append(MENU_CONTROL_OPTIONS, _("Options\to"), _T("Setup options"));
  m_pControlMenu->Append(MENU_CONTROL_SAVEOPTIONS, _("Save options\tCTRL-s"), _T("Save TrigMon options"));
  m_pControlMenu->Append(MENU_CONTROL_LOADOPTIONS, _("Load options\tCTRL-l"), _T("Load TrigMon options"));
  m_pControlMenu->AppendSeparator();
  m_pControlMenu->Append(MENU_CONTROL_QUIT, _("Quit\tCTRL-q"), _T("Quit Trigger Monitor"));
  m_pMenuBar->Append(m_pControlMenu,_("Control"));

  m_pTurfMenu = new wxMenu;
  m_pTurfMenu->Append(MENU_TURF_OPEN,_("Open"), _T("Open flight format TURF rate file"));
  m_pTurfMenu->Append(MENU_TURF_OPEN_RAW,_("Open Raw"), _T("Open raw format TURF rate file"));
  m_pTurfNext = m_pTurfMenu->Append(MENU_TURF_NEXT, _("Next TURF\tn"), _T("Next TURF file"));
  m_pTurfPrev = m_pTurfMenu->Append(MENU_TURF_PREV, _("Previous TURF\tp"), _T("Previous TURF file"));
  m_pMenuBar->Append(m_pTurfMenu,_("TURF"));

  m_pSurfMenu = new wxMenu;
  m_pSurfMenu->Append(MENU_SURF_OPEN,_("Open"), _T("Open flight format SURF HK file"));
  m_pSurfMenu->Append(MENU_SURF_OPEN_RAW,_("Open Raw"), _T("Open flight format SURF HK file"));
  m_pSurfNext = m_pSurfMenu->Append(MENU_SURF_NEXT, _("Next SURF\tShift-n"), _T("Next SURF file"));
  m_pSurfPrev = m_pSurfMenu->Append(MENU_SURF_PREV, _("Previous SURF\tShift-p"), _T("Previous SURF file"));
  m_pMenuBar->Append(m_pSurfMenu,_("SURF"));

  m_pDBDataMenu = new wxMenu;
  m_pDBDataConnect = m_pDBDataMenu->Append(MENU_DBDATA_CONNECT,_("Connect\tc"),
					   _T("Connect to ANITA database"));
  m_pDBDataDisconnect = m_pDBDataMenu->Append(MENU_DBDATA_DISCONNECT,_("Disconnect"), 
					      _T("Disconnect from ANITA database"));
  m_pDBDataMenu->AppendSeparator();
  m_pDBDataControl  = m_pDBDataMenu->Append(MENU_DBDATA_NEXT,_("Next event\tn"),
					    _T("Next surf and turf event"));
  m_pDBDataControl  = m_pDBDataMenu->Append(MENU_DBDATA_PREV,_("Prev event\tp"),
					    _T("Previous surf and turf event"));
  m_pDBDataControl  = m_pDBDataMenu->Append(MENU_DBDATA_LAST,_("Last event\tl"),
					    _T("Go to the last surf and turf event"));
  m_pDBDataControl  = m_pDBDataMenu->Append(MENU_DBDATA_AUTO,_("Auto/unAuto all\ta"),
					    _T("Check/ uncheck all checkbox"));
  m_pDBDataControl  = m_pDBDataMenu->Append(MENU_DBDATA_CONTROL,_("Control panel\tCTRL-c"),
					    _T("Open database control panel"));
  m_pDBDataTerminal = m_pDBDataMenu->Append(MENU_DBDATA_TERMINAL,_("Terminal\tCTRL-t"),_T("Open SQL terminal"));

  m_pMenuBar->Append(m_pDBDataMenu,_("DB data"));

  m_pHelpMenu = new wxMenu;
  m_pHelpMenu->Append(MENU_HELP_ABOUT,_("About\tF12"),_T("What is this????"));
  m_pMenuBar->Append(m_pHelpMenu,_("Help"));
///////////////////////////////june 27 2008 11:36/////////////////////////////
  m_pDataView= new wxMenu;//amir's addition
 // m_pDataViewer= m_pDataView->Append(MENU_CONTROL_DATAVIEWER, _("Data Viewer"), _T("Data Viewer"));//amir's addition
  //////////////////////////july 1 2008/////////////////////////////////////////
  m_pTurfViewer= m_pDataView->Append(MENU_TURF_VIEWER, _("Turf Viewer"), _T("Turf Viewer"));//amir's addition
  m_pSurfViewer= m_pDataView->Append(MENU_SURF_VIEWER, _("Surf Viewer"), _T("Surf Viewer"));//amir's addition
  m_pMenuBar->Append(m_pDataView,_("Data Viewer"));//amir's additon
//////////////////////////////////////////////////////////////////////////////////
  this->SetMenuBar(m_pMenuBar);
   m_turfViewer=new TurfViewer(this);
   m_surfViewer=new SurfViewer(this);
  // Create control panels
  m_optionsDialog=new TrigMonOptions(this);
  m_flightControl = new ControlPanel((wxWindow *)this,"FlightControlItem",N_PCKTYPE,_T("File Control Panel"));
  m_dbControl = new ControlPanel((wxWindow *)this,"DBControlItem",N_PCKTYPE,_T("Database Control Panel"));
  for(int i=0;i<N_PCKTYPE;++i){
    switch(i){
    case SURFPCK: 
      m_flightControl->SetLabel(i,_T("SURF HK")); 
      m_dbControl->SetLabel(i,_T("SURF HK")); 
      break;
    case TURFPCK: 
      m_flightControl->SetLabel(i,_T("TURF")); 
      m_dbControl->SetLabel(i,_T("TURF")); 
      break;
    default: break;
    }
  }

  // Create and arrange all children
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  wxStaticBox *turfBox = new wxStaticBox(this,wxID_ANY,_("TURF Monitor"));
  wxStaticBoxSizer *turfSizer = new wxStaticBoxSizer(turfBox,wxHORIZONTAL);
  wxStaticBox *surfBox = new wxStaticBox(this,wxID_ANY,_("SURF Monitor"));
  wxStaticBoxSizer *surfSizer = new wxStaticBoxSizer(surfBox,wxHORIZONTAL);
  topSizer->Add(turfSizer,0,wxEXPAND);
  topSizer->Add(surfSizer,1,wxEXPAND);

  //wxBoxSizer *turfLeftSizer=new wxBoxSizer(wxVERTICAL);
  m_TURFInfo = new TURFInfo(this);
#if 0  // Dummy L1 entries
  for(int i=0;i<N_L1;++i)
    m_L1Info->SetDisplayValue(L1Labels[i],(double)((L1Range[i][1]-L1Range[i][0])/(N_L1))*(i+1)+L1Range[i][0]);
#endif  

  wxBoxSizer *turfRightSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *turfRightTopSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *turfRightMidSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *turfRightBottomSizer=new wxBoxSizer(wxHORIZONTAL);
  //wxBoxSizer *turfRightSizer=new wxBoxSizer(wxVERTICAL);
#if 0  // Dummy L2 entries
  for(int i=0;i<N_L2;++i)
    m_L2Info->SetDisplayValue(L2Labels[i],(double)((L2Range[i][1]-L2Range[i][0])/(N_L2))*(i+1)+L2Range[i][0]);
#endif
  
  m_L1Mask = new BarInfo(this,N_L1,L1_PER_ROW, _("L1 v Mask"),_T(""),L1MaskLabels,
			 L1MaskLabels,L1MaskCaptions, L1MaskVars,L1MaskTable,
			 L1MaskSpec,L1MaskRange,L1MaskAverage,L1MaskScaleType,wxSize(14,40),L1MaskScale);
  m_L1hMask = new BarInfo(this,N_L1,L1_PER_ROW, _("L1 h Mask"),_T(""),L1MaskLabels,
			 L1MaskLabels,L1MaskCaptions, L1MaskVars,L1MaskTable,
			 L1MaskSpec,L1MaskRange,L1MaskAverage,L1MaskScaleType,wxSize(14,40),L1MaskScale);
  m_L1Info = new BarInfo(this,N_L1,L1_PER_ROW, _("L1 v Rates"),_T("Hz"),L1Labels,
			 L1Labels,L1Captions, L1Vars,L1Table,
			 L1Spec,L1Range,L1Average,L1ScaleType,wxSize(14,40),L1Scale);
  m_L1hInfo = new BarInfo(this,N_L1,L1_PER_ROW, _("L1 h Rates"),_T("Hz"),L1Labels,
			 L1Labels,L1Captions, L1Vars,L1Table,
			 L1Spec,L1Range,L1Average,L1ScaleType,wxSize(14,40),L1Scale);
  m_L3Info = new BarInfo(this,N_L3,L3_PER_ROW, _("L3 v Rates"),_T("Hz"),L3Labels,
			 L3Labels,L3Captions, L3Vars,L3Table,
			 L3Spec,L3Range,L3Average,L3ScaleType,wxSize(14,40),L3Scale);
  m_L3hInfo = new BarInfo(this,N_L3,L3_PER_ROW, _("L3 h Rates"),_T("Hz"),L3Labels,
			 L3Labels,L3Captions, L3Vars,L3Table,
			 L3Spec,L3Range,L3Average,L3ScaleType,wxSize(14,40),L3Scale);
#if 0  // Dummy L3 entries
  for(int i=0;i<N_L3;++i)
    m_L3Info->SetDisplayValue(L3Labels[i],(double)((L3Range[i][1]-L3Range[i][0])/(N_L3))*(i+1)+L3Range[i][0]);
#endif

  turfRightTopSizer->Add(m_L1Mask,2,wxEXPAND|wxLEFT,20);
  turfRightTopSizer->Add(m_L1hMask,2,wxEXPAND|wxLEFT,20);
  turfRightMidSizer->Add(m_L1Info,2,wxEXPAND);
  turfRightMidSizer->Add(m_L1hInfo,2,wxEXPAND);
  turfRightBottomSizer->Add(m_L3Info,2,wxEXPAND|wxLEFT,15);
  turfRightBottomSizer->Add(m_L3hInfo,2,wxEXPAND|wxLEFT,15);
  turfRightSizer->Add(turfRightTopSizer,2,wxEXPAND);
  turfRightSizer->Add(turfRightMidSizer,2,wxEXPAND);
  turfRightSizer->Add(turfRightBottomSizer,2,wxEXPAND);
  turfSizer->Add(m_TURFInfo,0,wxEXPAND);
  turfSizer->Add(turfRightSizer,3,wxEXPAND);

//  wxStaticBox *helpBox=new wxStaticBox(this,wxID_ANY,_T("Scaler/Threshold legend:"));
//  wxStaticBoxSizer *helpSizer=new wxStaticBoxSizer(helpBox,wxVERTICAL);
 // turfSizer->Add(helpSizer,0,wxEXPAND);

//  wxString help;

//   help << "To open graphs -\n\t double click on bars or variable text\n"
//        << "For easy event control -\n\t 'Control Panel' from control menu\n" 
//        << "On graphs\n\t - Shift down will give crosshairs"
//        << "\n\t - Zoom with left/right dragging"
//        << "\n\t - Reset zoom with middle click";
//  help << _T("Format DDx; where DD is antenna number and x is the band.\n")
//  help<< _T("Band mapping:\n\t L - low (200 - 350 MHz)\n\t M - mid (330 - 600 MHz)\n")
//        << _T("\t H - high (630 - 1100 MHz)\n\t F - Full(150 - 1240 MHz)\n");
//        //<< _T("For each trigger band there are two DDx labels, ")
//        //<< _T("the first is RCP, the second is LCP");
 // wxStaticText *helpText=new wxStaticText(this,wxID_ANY,help,wxDefaultPosition,
 // 				  wxSize(220,-1));
 // helpText->Wrap(220);
 // helpSizer->Add(helpText,0);

  wxNotebook *m_notebook = new wxNotebook(this,wxID_ANY);
  surfSizer->Add(m_notebook,1,wxEXPAND);
  wxString boxLabel;
  
  wxPanel *rfPage=new wxPanel(m_notebook,wxID_ANY);
  wxBoxSizer *rfSizer=new wxBoxSizer(wxHORIZONTAL);
  int halfSurf=N_SURF/2;
  boxLabel.Printf(_T("RF Power SURF 1-%d"),halfSurf);
  m_RFInfo1=new BarInfo(rfPage,halfSurf*RFPOWER_PER_ROW,RFPOWER_PER_ROW,
			boxLabel,_T(""),
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

  boxLabel.Printf(_T("RF Power SURF %d-%d"),halfSurf+1,N_SURF);
  m_RFInfo2=new BarInfo(rfPage,N_RFPOWER-halfSurf*RFPOWER_PER_ROW,
			RFPOWER_PER_ROW,boxLabel,_T(""),
			&RFPowerLabels[halfSurf*RFPOWER_PER_ROW],
			&RFPowerLabels[halfSurf*RFPOWER_PER_ROW],
			&RFPowerCaptions[halfSurf*RFPOWER_PER_ROW],
			&RFPowerVars[halfSurf*RFPOWER_PER_ROW],
			&RFPowerTable[halfSurf*RFPOWER_PER_ROW],
			&RFPowerSpec[halfSurf*RFPOWER_PER_ROW],
			&RFPowerRange[halfSurf*RFPOWER_PER_ROW],
			&RFPowerAverage[halfSurf*RFPOWER_PER_ROW],
			&RFPowerScaleType[halfSurf*RFPOWER_PER_ROW],
			wxSize(16,25),RFPowerScale);
#if 0  // Dummy RF entries
  for(int i=0;i<halfSurf*RFPOWER_PER_ROW;++i)
    m_RFInfo1->SetDisplayValue(RFPowerLabels[i],(double)((RFPowerRange[i][1]-RFPowerRange[i][0])/(halfSurf*RFPOWER_PER_ROW))*(i+1)+RFPowerRange[i][0]);
  for(int i=halfSurf*RFPOWER_PER_ROW;i<N_RFPOWER;++i)
    m_RFInfo2->SetDisplayValue(RFPowerLabels[i],(double)((RFPowerRange[i][1]-RFPowerRange[i][0])/((N_SURF-halfSurf)*RFPOWER_PER_ROW))*(i+1-halfSurf*RFPOWER_PER_ROW)+RFPowerRange[i][0]);
#endif

  m_SURFInfo = new SURFInfo(rfPage);

  rfSizer->Add(m_RFInfo1,1,wxEXPAND);
  rfSizer->Add(m_RFInfo2,1,wxEXPAND);
  rfSizer->Add(m_SURFInfo,1);   // 3->2 to keep window size. 14-Oct SM.

  rfPage->SetSizerAndFit(rfSizer);
  m_notebook->AddPage(rfPage,_("RF Power"),true);

  wxPanel *scalerPage=new wxPanel(m_notebook,wxID_ANY);
  wxBoxSizer *scalerSizer=new wxBoxSizer(wxHORIZONTAL);
  boxLabel.Printf(_T("Scalers SURF 1-%d"),halfSurf);
  m_ScalerInfo1=new BarInfo(scalerPage,halfSurf*SCALER_PER_ROW,SCALER_PER_ROW,
			    boxLabel,_T("Hz"),
			    ScalerLabels,
			    ScalerRefLabels,
			    ScalerCaptions,
			    ScalerVars,
			    ScalerTable,
			    ScalerSpec,
			    ScalerRange,
			    ScalerAverage,
			    ScalerScaleType,
			    wxSize(12,25),
			    ScalerScale);

  boxLabel.Printf(_T("Scalers SURF %d-%d"),halfSurf+1,(N_SURF));
  m_ScalerInfo2=new BarInfo(scalerPage,N_SCALER-halfSurf*SCALER_PER_ROW,
			    SCALER_PER_ROW,boxLabel,_T("Hz"),
			    &ScalerLabels[halfSurf*SCALER_PER_ROW],
			    &ScalerRefLabels[halfSurf*SCALER_PER_ROW],
			    &ScalerCaptions[halfSurf*SCALER_PER_ROW],
			    &ScalerVars[halfSurf*SCALER_PER_ROW],
			    &ScalerTable[halfSurf*SCALER_PER_ROW],
			    &ScalerSpec[halfSurf*SCALER_PER_ROW],
			    &ScalerRange[halfSurf*SCALER_PER_ROW],
			    &ScalerAverage[halfSurf*SCALER_PER_ROW],
			    &ScalerScaleType[halfSurf*SCALER_PER_ROW],
			    wxSize(12,25),ScalerScale);
#if 0  // Dummy scaler entries
  for(int i=0;i<halfSurf*SCALER_PER_ROW;++i)
    m_ScalerInfo1->SetDisplayValue(ScalerRefLabels[i],(double)((ScalerRange[i][1]-ScalerRange[i][0])/(halfSurf*SCALER_PER_ROW))*(i+1)+ScalerRange[i][0]);
  for(int i=halfSurf*SCALER_PER_ROW;i<N_SCALER;++i)
    m_ScalerInfo2->SetDisplayValue(ScalerRefLabels[i],(double)((ScalerRange[i][1]-ScalerRange[i][0])/(N_SCALER-halfSurf*SCALER_PER_ROW))*(i+1-halfSurf*SCALER_PER_ROW)+ScalerRange[i][0]);
#endif  

  scalerSizer->Add(m_ScalerInfo1,1,wxEXPAND);
  scalerSizer->Add(m_ScalerInfo2,1,wxEXPAND);
  scalerPage->SetSizerAndFit(scalerSizer);
  m_notebook->AddPage(scalerPage,_("Scalers"),false);

  wxPanel *thresholdPage=new wxPanel(m_notebook,wxID_ANY);
  wxBoxSizer *thresholdSizer=new wxBoxSizer(wxHORIZONTAL);
  boxLabel.Printf(_T("Threshold SURF 1-%d"),halfSurf);
  m_ThresholdInfo1=new BarInfo(thresholdPage,halfSurf*THRESHOLD_PER_ROW,
			       THRESHOLD_PER_ROW,boxLabel,_T("DAC"),
			       ThresholdLabels,
			       ThresholdRefLabels,
			       ThresholdCaptions,
			       ThresholdVars,
			       ThresholdTable,
			       ThresholdSpec,
			       ThresholdRange,
			       ThresholdAverage,
			       ThresholdScaleType,
			       wxSize(12,25),ThresholdScale);

  boxLabel.Printf(_T("Threshold SURF %d-%d"),halfSurf+1,(N_SURF));
  m_ThresholdInfo2=new BarInfo(thresholdPage,
			       N_THRESHOLD-halfSurf*THRESHOLD_PER_ROW,
			       THRESHOLD_PER_ROW,boxLabel,_T("DAC"),
			       &ThresholdLabels[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdRefLabels[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdCaptions[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdVars[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdTable[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdSpec[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdRange[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdAverage[halfSurf*THRESHOLD_PER_ROW],
			       &ThresholdScaleType[halfSurf*THRESHOLD_PER_ROW],
			       wxSize(12,25),ThresholdScale);
#if 0  // Dummy threshold entries
  for(int i=0;i<halfSurf*THRESHOLD_PER_ROW;++i)
    m_ThresholdInfo1->SetDisplayValue(ThresholdRefLabels[i],(double)((ThresholdRange[i][1]-ThresholdRange[i][0])/(halfSurf*THRESHOLD_PER_ROW))*(i+1)+ThresholdRange[i][0]);
  for(int i=halfSurf*THRESHOLD_PER_ROW;i<N_THRESHOLD;++i)
    m_ThresholdInfo2->SetDisplayValue(ThresholdRefLabels[i],(double)((ThresholdRange[i][1]-ThresholdRange[i][0])/(N_THRESHOLD-halfSurf*THRESHOLD_PER_ROW))*(i+1-halfSurf*THRESHOLD_PER_ROW)+ThresholdRange[i][0]);
#endif 
 
  thresholdSizer->Add(m_ThresholdInfo1,1,wxEXPAND);
  thresholdSizer->Add(m_ThresholdInfo2,1,wxEXPAND);
  thresholdPage->SetSizerAndFit(thresholdSizer);
  m_notebook->AddPage(thresholdPage,_("Thresholds"),false);

  m_LogWindow = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,
				   wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);
  m_LogWindow->SetSize(wxSize(200,50));
  m_logTarget = wxLog::SetActiveTarget( new wxLogTextCtrl(m_LogWindow) );
  wxLogMessage(_T("Welcome to TrigMon - ANITA Master Trigger Display Utility"));

  topSizer->Add(m_LogWindow,0,wxEXPAND);

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);
  Centre();

  // Initialize data handlers
  currentRawTurfFile=new wxFileName();
  currentRawSurfFile=new wxFileName();
  turfRaw=false;
  surfRaw=false;
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
  m_updateTimer->Start(1500);

  m_xmlHandler=new TrigMonXMLHandler();
  showProgress=false;
  lastSQLRequest=0;  
  if(m_dbHandler){ // DB connection already initiated
    // Start oneshot timer which will check on connection status
    wxTimer *dbTimer=new wxTimer(this,DBCONNECTION_TIMER);
    dbTimer->Start(1,true);
  }else
    m_dbHandler=new DBHandler();
  m_dbTerminal=new DBTerminal(this);

  // Allocate memory for dynamic specs
  dynamicScalerSpec=new wxRangeSpec*[N_SCALER];
  for(int i=0;i<N_SCALER;++i){
    dynamicScalerSpec[i]=new wxRangeSpec[2];
    dynamicScalerSpec[i][0]=wxRangeSpec(0,0,wxGREEN);
    dynamicScalerSpec[i][1]=wxRangeSpec();
  }
  dynamicThresholdSpec=new wxRangeSpec*[N_THRESHOLD];
  for(int i=0;i<N_THRESHOLD;++i){
    dynamicThresholdSpec[i]=new wxRangeSpec[2];
    dynamicThresholdSpec[i][0]=wxRangeSpec(0,0,wxGREEN);
    dynamicThresholdSpec[i][1]=wxRangeSpec();
  }

  // Enable/disable appropriate menu items and other initializations
  init();

  // Remove splash
  if(splash){
    splash->Show(false);
    delete splash;
  }
}

// Initialize
void TrigMonFrame::init(){
  packetTable[0]=_T("hk_surf");
  packetTable[1]=_T("turf");

  // Could change with cmd line
  m_pTurfNext->Enable(false); 
  m_pTurfPrev->Enable(false);
  m_pSurfNext->Enable(false); 
  m_pSurfPrev->Enable(false);

  m_pDBDataDisconnect->Enable(false);
  m_pDBDataControl->Enable(false);
  m_pDBDataTerminal->Enable(false);
  
  wxString tmp;
  if(cmdLine->Found(_T("o"),&tmp)){
    LoadOptions(tmp);
  }else{ // Try to load default options file 
    tmp=_U(DEFAULT_OPTIONS_FILE);
    if(wxFileName::FileExists(tmp)) LoadOptions(tmp);
    else{
      tmp.Prepend(_T("/")).Prepend(wxFileName::GetHomeDir());
      if(wxFileName::FileExists(tmp)) LoadOptions(tmp);
    }
  }

  if(!cmdLine->Found(_T("t"),&tmp)){
    if(!cmdLine->Found(_T("T"),&tmp))
       tmp=wxGetCwd();
    else
      turfRaw=true;
  }
  if(turfRaw){
    *currentRawTurfFile=wxFileName::DirName(tmp);
    currentRawTurfFile->MakeAbsolute();
  }else{
    packetDirectory[TURFPCK]=wxFileName::DirName(tmp);
    packetDirectory[TURFPCK].MakeAbsolute();
  }
  IndexTurfDirectory();
  
  if(!cmdLine->Found(_T("s"),&tmp)){
    if(!cmdLine->Found(_T("S"),&tmp))
      tmp=wxGetCwd();
    else
      surfRaw=true;
  }
  if(surfRaw){
    *currentRawSurfFile=wxFileName::DirName(tmp);
    currentRawSurfFile->MakeAbsolute();
  }else{
    packetDirectory[SURFPCK]=wxFileName::DirName(tmp);
    packetDirectory[SURFPCK].MakeAbsolute();
  }
  IndexSurfDirectory();
  
  if(!cmdLine->Found(_T("a"),&m_turfReadDelay))
    m_turfReadDelay=TrigMonOpts::turfReadDelay;

  if(!cmdLine->Found(_T("A"),&m_surfReadDelay))
    m_surfReadDelay=TrigMonOpts::surfReadDelay;

  return;
}
  
TrigMonFrame::~TrigMonFrame(){
  m_currentTimeTimer->Stop();
  m_updateTimer->Stop();
  delete wxLog::SetActiveTarget(m_logTarget);
  delete m_xmlHandler;
  delete m_dbHandler;
  for(int i=0;i<N_SCALER;++i) delete[] dynamicScalerSpec[i];
  delete[] dynamicScalerSpec;
  for(int i=0;i<N_THRESHOLD;++i) delete[] dynamicThresholdSpec[i];
  delete[] dynamicThresholdSpec;
}
////////////////////june 30 2008 /////////////////amir's addition
void TrigMonFrame::OnMenuTurfViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
  // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
  m_turfViewer->Show(true);
}
///////////////////////////////////////////////////////
void TrigMonFrame::OnMenuSurfViewer( wxCommandEvent &WXUNUSED(event) ){//amir's addition
  // wxMessageBox("Hello World from\n(c) Predrag Miocinovic & Amir Javaid 2008","About",wxOK);
  m_surfViewer->Show(true);
}
///////////////////////////////////////////////////////
// wxEVT_COMMAND_MENU_SELECTED event handlers
void TrigMonFrame::OnMenuControlPanel(wxCommandEvent &WXUNUSED(event) ){
  m_dbControl->StopTimers();
  m_dbControl->Show(false);
  m_flightControl->Show();
}

void TrigMonFrame::OnMenuControlOptions( wxCommandEvent& event ){
  if(m_optionsDialog->ShowModal()==wxID_OK){
    // Harvest new settings from turf options dialogs
    m_optionsDialog->ExportOptions();
    SetTurfOptions();
    SetSurfOptions();
    // Redraw everything
    Refresh(true);
  }else{  // Reset all values to existing ones
    m_optionsDialog->ImportOptions();
  }
}

void TrigMonFrame::OnMenuControlSaveOptions( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("default.trc"),_T("TrigMon options(*.trc)|*.trc"),wxSAVE);
  if (dlg->ShowModal() == wxID_OK) SaveOptions(dlg->GetPath());
  dlg->Destroy();
}

void TrigMonFrame::OnMenuControlLoadOptions( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Load options file"),_T(""),_T(""),_T("TrigMon options(*.trc)|*.trc"),wxOPEN);
  if (dlg->ShowModal() == wxID_OK) 
    if(LoadOptions(dlg->GetPath()))
      // Redraw everything
      Refresh(true);
  dlg->Destroy();
}

void TrigMonFrame::OnMenuControlQuit( wxCommandEvent& event ){
  Close(true);
}

void TrigMonFrame::OnMenuTurfOpen( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open TURF file"),packetDirectory[TURFPCK].GetPath(),_T(""),_T("TURF files(turfhk_*.dat*)|turfhk_*.dat*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if(ReadTurfData(file)){
      turfRaw=false;
      packetDirectory[TURFPCK].SetPath(file.GetPath());
      IndexTurfDirectory();
      int n=packetFileList[TURFPCK]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(TURFPCK,n);
    }else{
	m_pTurfNext->Enable(false);
	m_pTurfPrev->Enable(false);
    }
  }
  dlg->Destroy();
}

void TrigMonFrame::OnMenuTurfRawOpen( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open TURF file"),currentRawTurfFile->GetPath(),_T(""),_T("TURF files(turf_data.*)|turf_data.*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    *currentRawTurfFile=dlg->GetPath();
    // Load file here
    if(currentRawTurfFile->IsOk() && ReadTurfRawData()){
      turfRaw=true;
      IndexTurfDirectory();
      int n=packetFileList[TURFPCK]->Index(currentRawTurfFile->GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(TURFPCK,n);
    }else{
	m_pTurfNext->Enable(false);
	m_pTurfPrev->Enable(false);
    }
  }
  dlg->Destroy();
}

void TrigMonFrame::OnMenuTurfNext( wxCommandEvent& event ){
  m_flightControl->Next(TURFPCK,event);
}

void TrigMonFrame::OnMenuTurfPrev( wxCommandEvent& event ){
  m_flightControl->Prev(TURFPCK,event);
}

void TrigMonFrame::OnMenuSurfOpen( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open SURF HK file"),packetDirectory[SURFPCK].GetPath(),_T(""),_T("SURF HK files(surfhk_*.dat*)|surfhk_*.dat*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    wxFileName file=dlg->GetPath();
    // Load file here
    if(ReadSurfData(file)){
      surfRaw=false;
      packetDirectory[SURFPCK].SetPath(file.GetPath());
      IndexSurfDirectory();
      int n=packetFileList[SURFPCK]->Index(file.GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(SURFPCK,n);
    }else{
	m_pSurfNext->Enable(false);
	m_pSurfPrev->Enable(false);
    }
  }
  dlg->Destroy();
}

void TrigMonFrame::OnMenuSurfRawOpen( wxCommandEvent& event ){
  wxFileDialog *dlg = new wxFileDialog(this,_T("Open SURF HK file"),currentRawSurfFile->GetPath(),_T(""),_T("SURF HK files(hksurf_data.*)|hksurf_data.*"),wxOPEN);

  if (dlg->ShowModal() == wxID_OK){
    *currentRawSurfFile=dlg->GetPath();
    // Load file here
    if(currentRawSurfFile->IsOk() && ReadSurfRawData()){
      surfRaw=true;
      IndexSurfDirectory();
      int n=packetFileList[SURFPCK]->Index(currentRawSurfFile->GetFullPath().c_str());
      if(n!=wxNOT_FOUND) m_flightControl->SetIndex(SURFPCK,n);
    }else{
	m_pSurfNext->Enable(false);
	m_pSurfPrev->Enable(false);
    }
  }
  dlg->Destroy();
}

void TrigMonFrame::OnMenuSurfNext( wxCommandEvent& event ){
  m_flightControl->Next(SURFPCK,event);
}

void TrigMonFrame::OnMenuSurfPrev( wxCommandEvent& event ){
  m_flightControl->Prev(SURFPCK,event);
}

void TrigMonFrame::OnMenuDBDataConnect( wxCommandEvent &WXUNUSED(event) ){
  if(m_dbHandler->Connect(TrigMonOpts::dbHost,TrigMonOpts::dbUser,
			  TrigMonOpts::dbPwd,TrigMonOpts::dbName)){
    wxTimerEvent e;
    CheckDBConnection(e);
  }
}

void TrigMonFrame::OnMenuDBDataDisconnect( wxCommandEvent &WXUNUSED(event) ){
  m_dbHandler->Disconnect();
  m_pDBDataConnect->Enable(true);
  m_pDBDataDisconnect->Enable(false);
  m_pDBDataControl->Enable(false);
  m_pDBDataTerminal->Enable(false);
  m_dbTerminal->Show(false);
  SetStatusText(_T("Disconnected from database"),2);
}

void TrigMonFrame::OnMenuDBDataNext( wxCommandEvent &event ){
  m_dbControl->Next(SURFPCK,event);
  m_dbControl->Next(TURFPCK,event);
}

void TrigMonFrame::OnMenuDBDataPrev( wxCommandEvent &event ){
  m_dbControl->Prev(SURFPCK,event);
  m_dbControl->Prev(TURFPCK,event);
}

void TrigMonFrame::OnMenuDBDataLast( wxCommandEvent &event ){
  m_dbControl->Last(SURFPCK,event);
  m_dbControl->Last(TURFPCK,event);
}

void TrigMonFrame::OnMenuDBDataAuto( wxCommandEvent &event ){
  m_dbControl->InverseTimers();
}

void TrigMonFrame::OnMenuDBDataControl( wxCommandEvent &WXUNUSED(event) ){
  m_flightControl->StopTimers();
  m_flightControl->Show(false);
  m_dbControl->Show();
}

void TrigMonFrame::OnMenuDBDataTerminal(wxCommandEvent &WXUNUSED(event) ){
  m_dbTerminal->Show(true);
}

void TrigMonFrame::OnMenuHelpAbout( wxCommandEvent& event ){
  wxMessageBox(_T("ANITA Master Trigger Monitor\nsvnversion: ddm_placeholder_for_svnversion\n(c) Predrag Miocinovic 2006"),_T("About"),wxOK);
}
void TrigMonFrame::OnCurrentTimeTimer( wxTimerEvent &WXUNUSED(event) ){
  m_TURFInfo->UpdateCurrentTime();
}
void TrigMonFrame::OnUpdateTimer( wxTimerEvent &WXUNUSED(event) ){
  static int task=0;
  static int five=0;
  time_t now=time(NULL);
  if(m_dbHandler->IsConnected() && (now-lastSQLRequest)<3600 && ++five==5){// peng : changed from 30 to 3600 seconds.
    ReceiveNotifications();
    five=0;
  }
  // Update various open directories
  switch(task){
  case SURFPCK:
    IndexSurfDirectory(false);
    if(needsIndex[task]){
      showProgress=false;
      IndexDBPackets(task,false);
      needsIndex[task]=false;
    }
    break;
  case TURFPCK:
    IndexTurfDirectory(false);
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

void TrigMonFrame::ReadFlightData(wxCommandEvent &event){
  wxFileName file=event.GetString();
  int order=(int)event.GetClientData();
  switch(event.GetId()-CONTROLWIN_OFFSET){
  case SURFPCK: 
    if(surfRaw){
      *currentRawSurfFile=file; 
      ReadSurfRawData(); 
    }else
      ReadSurfData(file,order);
    break;
  case TURFPCK:
    if(turfRaw){
      *currentRawTurfFile=file; 
      ReadTurfRawData();
    }else
      ReadTurfData(file,order);
      break;
  default:
    wxMessageBox(_T("Received request for loading of unknown packet file"),_T("Packet file request error"),wxOK|wxICON_ERROR);
  }
}

void TrigMonFrame::RequestPacket(wxCommandEvent &event){
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
  cmd.Printf(_T("SELECT * FROM %s WHERE nbuf=%s;"),
	     packetTable[type].c_str(),nbufString.c_str());
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
}

void TrigMonFrame::OnSQLRequest(wxCommandEvent &event){
  wxObject *requestor=event.GetEventObject();
  sqlparser parser=(sqlparser)event.GetClientData();
  wxString sql=event.GetString();
  if(TrigMonOpts::sqlDump) wxLogMessage(sql);
  if(!m_dbHandler->SendQuery(sql,requestor,parser)) sqlRequestType.Empty();
}
  
void TrigMonFrame::CheckDBConnection( wxTimerEvent &WXUNUSED(event) ){
  bool status=m_dbHandler->FinalizeConnection();
  if(status){
    m_pDBDataConnect->Enable(false);
    m_pDBDataDisconnect->Enable(true);
    m_pDBDataControl->Enable(true);
    m_pDBDataTerminal->Enable(true);
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

void TrigMonFrame::SetTurfOptions(){
  using namespace TrigMonOpts;
  wxString yScale;
  for(int i=0;i<N_L1;++i){
    m_L1Info->SetDisplayRangeSpec(L1Labels[i],L1Spec[i]);
    m_L1Info->SetDisplayRange(L1Labels[i],L1Range[i][0],L1Range[i][1]);
    m_L1Info->SetDisplayScaleType(L1Labels[i],L1ScaleType[i]);
    m_L1Info->SetDisplayAverage(L1Labels[i],L1Average[i]);
    m_L1hInfo->SetDisplayRangeSpec(L1Labels[i],L1Spec[i]);
    m_L1hInfo->SetDisplayRange(L1Labels[i],L1Range[i][0],L1Range[i][1]);
    m_L1hInfo->SetDisplayScaleType(L1Labels[i],L1ScaleType[i]);
    m_L1hInfo->SetDisplayAverage(L1Labels[i],L1Average[i]);
  }
  for(int i=0;i<N_L3;++i){
    m_L3Info->SetDisplayRangeSpec(L3Labels[i],L3Spec[i]);
    m_L3Info->SetDisplayRange(L3Labels[i],L3Range[i][0],L3Range[i][1]);
    m_L3Info->SetDisplayScaleType(L3Labels[i],L3ScaleType[i]);
    m_L3Info->SetDisplayAverage(L3Labels[i],L3Average[i]);
    m_L3hInfo->SetDisplayRangeSpec(L3Labels[i],L3Spec[i]);
    m_L3hInfo->SetDisplayRange(L3Labels[i],L3Range[i][0],L3Range[i][1]);
    m_L3hInfo->SetDisplayScaleType(L3Labels[i],L3ScaleType[i]);
    m_L3hInfo->SetDisplayAverage(L3Labels[i],L3Average[i]);
  }

  m_turfReadDelay=turfReadDelay;
  // Reset timer steps
  m_flightControl->AdjustTimer(TURFPCK,m_turfReadDelay);
  m_dbControl->AdjustTimer(TURFPCK,m_turfReadDelay);

  m_L1Info->SetScale(L1Scale);
  m_L1hInfo->SetScale(L1Scale);
  m_L3Info->SetScale(L3Scale);
  m_L3hInfo->SetScale(L3Scale);

  return;
}  

void TrigMonFrame::SetSurfOptions(){
  using namespace TrigMonOpts;
  int i;
  int halfSurf=N_SURF/2;
  for(i=0;i<halfSurf*RFPOWER_PER_ROW;++i){
    m_RFInfo1->SetDisplayRangeSpec(RFPowerLabels[i],RFPowerSpec[i]);
    m_RFInfo1->SetDisplayRange(RFPowerLabels[i],RFPowerRange[i][0],RFPowerRange[i][1]);
    m_RFInfo1->SetDisplayScaleType(RFPowerLabels[i],RFPowerScaleType[i]);
    m_RFInfo1->SetDisplayAverage(RFPowerLabels[i],RFPowerAverage[i]);
  }
  for(;i<N_RFPOWER;++i){
    m_RFInfo2->SetDisplayRangeSpec(RFPowerLabels[i],RFPowerSpec[i]);
    m_RFInfo2->SetDisplayRange(RFPowerLabels[i],RFPowerRange[i][0],RFPowerRange[i][1]);
    m_RFInfo2->SetDisplayScaleType(RFPowerLabels[i],RFPowerScaleType[i]);
    m_RFInfo2->SetDisplayAverage(RFPowerLabels[i],RFPowerAverage[i]);
  }

  for(i=0;i<halfSurf*SCALER_PER_ROW;++i){
    if(!DynamicTarget) m_ScalerInfo1->SetDisplayRangeSpec(ScalerRefLabels[i],ScalerSpec[i]);
    else m_ScalerInfo1->SetDisplayRangeSpec(ScalerRefLabels[i],dynamicScalerSpec[i]);
    m_ScalerInfo1->SetDisplayRange(ScalerRefLabels[i],ScalerRange[i][0],ScalerRange[i][1]);
    m_ScalerInfo1->SetDisplayScaleType(ScalerRefLabels[i],ScalerScaleType[i]);
    m_ScalerInfo1->SetDisplayAverage(ScalerRefLabels[i],ScalerAverage[i]);
  }
  for(;i<N_SCALER;++i){
    if(!DynamicTarget) m_ScalerInfo2->SetDisplayRangeSpec(ScalerRefLabels[i],ScalerSpec[i]);
    else m_ScalerInfo2->SetDisplayRangeSpec(ScalerRefLabels[i],dynamicScalerSpec[i]);
    m_ScalerInfo2->SetDisplayRange(ScalerRefLabels[i],ScalerRange[i][0],ScalerRange[i][1]);
    m_ScalerInfo2->SetDisplayScaleType(ScalerRefLabels[i],ScalerScaleType[i]);
    m_ScalerInfo2->SetDisplayAverage(ScalerRefLabels[i],ScalerAverage[i]);
  }

  for(i=0;i<halfSurf*THRESHOLD_PER_ROW;++i){
    if(!DynamicTarget) m_ThresholdInfo1->SetDisplayRangeSpec(ThresholdRefLabels[i],ThresholdSpec[i]);
    else m_ThresholdInfo1->SetDisplayRangeSpec(ThresholdRefLabels[i],dynamicThresholdSpec[i]);
    m_ThresholdInfo1->SetDisplayRange(ThresholdRefLabels[i],ThresholdRange[i][0],ThresholdRange[i][1]);
    m_ThresholdInfo1->SetDisplayScaleType(ThresholdRefLabels[i],ThresholdScaleType[i]);
    m_ThresholdInfo1->SetDisplayAverage(ThresholdRefLabels[i],ThresholdAverage[i]);
  }
  for(;i<N_THRESHOLD;++i){
    if(!DynamicTarget) m_ThresholdInfo2->SetDisplayRangeSpec(ThresholdRefLabels[i],ThresholdSpec[i]);
    else m_ThresholdInfo2->SetDisplayRangeSpec(ThresholdRefLabels[i],dynamicThresholdSpec[i]);
    m_ThresholdInfo2->SetDisplayRange(ThresholdRefLabels[i],ThresholdRange[i][0],ThresholdRange[i][1]);
    m_ThresholdInfo2->SetDisplayScaleType(ThresholdRefLabels[i],ThresholdScaleType[i]);
    m_ThresholdInfo2->SetDisplayAverage(ThresholdRefLabels[i],ThresholdAverage[i]);
  }
  m_surfReadDelay=surfReadDelay;
  // Reset timer steps
  m_flightControl->AdjustTimer(SURFPCK,m_surfReadDelay);
  m_dbControl->AdjustTimer(SURFPCK,m_surfReadDelay);

  m_RFInfo1->SetScale(RFPowerScale);
  m_RFInfo2->SetScale(RFPowerScale);
  m_ScalerInfo1->SetScale(ScalerScale);
  m_ScalerInfo2->SetScale(ScalerScale);
  m_ThresholdInfo1->SetScale(ThresholdScale);
  m_ThresholdInfo2->SetScale(ThresholdScale);

  return;
}

// Save current display options
bool TrigMonFrame::SaveOptions(const wxString &file){
  using namespace TrigMonOpts;
  // Open file
  wxFile saveFile(file.c_str(),wxFile::write);
  if (!saveFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open file %s for writing"),file.c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }
  
  m_xmlHandler->WriteHead(saveFile,_T("trigmon"));

  xmlTagValuePair **scale;
  scale = new xmlTagValuePair*[2];
  scale[0]= new xmlTagValuePair(_T("scale"));
  scale[1]=NULL;

  wxString tmp;
  // Output TURF options
  m_xmlHandler->OpenTag(saveFile,_T("turf"));
  scale[0]->value=L3Scale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("l3"),N_L3,L3Labels,L3Spec,L3Range,L3ScaleType,L3Average,scale);
  m_xmlHandler->CloseTag(saveFile,_T("turf"));
  
  // Ouput SURF HK options
  xmlTagValuePair **extra;
  extra = new xmlTagValuePair*[4];
  extra[0]= new xmlTagValuePair(_T("lock"),Lockstep?_T("on"):_T("off"));
  extra[1]= new xmlTagValuePair(_T("dynamic"),DynamicTarget?_T("on"):_T("off"));
  switch(rfPowerType){
  case dBm: extra[2]= new xmlTagValuePair(_T("rf"),_T("dBm")); break;
  case T: extra[2]= new xmlTagValuePair(_T("rf"),_T("t")); break;
  case ADC: default: extra[2]= new xmlTagValuePair(_T("rf"),_T("adc")); break;
  }
  extra[3]=NULL;
  m_xmlHandler->OpenTag(saveFile,_T("surf"),extra);
  // Clean up 
  for(int i=0;i<3;++i) delete extra[i];  
  delete[] extra;

  scale[0]->value=RFPowerScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("rfpower"),N_RFPOWER,RFPowerLabels,RFPowerSpec,RFPowerRange,RFPowerScaleType,RFPowerAverage,scale);
  scale[0]->value=ScalerScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("scaler"),N_SCALER,ScalerRefLabels,ScalerSpec,ScalerRange,ScalerScaleType,ScalerAverage,scale);
  scale[0]->value=ThresholdScale;
  m_xmlHandler->WriteBarItemBlock(saveFile,_T("threshold"),N_THRESHOLD,ThresholdRefLabels,ThresholdSpec,ThresholdRange,ThresholdScaleType,ThresholdAverage,scale);
  m_xmlHandler->CloseTag(saveFile,_T("surf"));

  // Clean up
  delete scale[0];
  delete[] scale;

  // Prepare extra tags for db line
  extra = new xmlTagValuePair*[6];
  extra[0]= new xmlTagValuePair(_T("host"),dbHost);
  extra[1]= new xmlTagValuePair(_T("user"),dbUser);
  extra[2]= new xmlTagValuePair(_T("name"),dbName);
  extra[3]= new xmlTagValuePair(_T("sql"),sqlDump?_T("on"):_T("off"));  
  extra[4]= new xmlTagValuePair(_T("crc"),crcCheck?_T("on"):_T("off"));  
  extra[5]=NULL;
  m_xmlHandler->OpenTag(saveFile,_T("db"),extra,true);
  for(int i=0;i<5;++i) delete extra[i];  
  delete[] extra;
  
  m_xmlHandler->CloseTag(saveFile,_T("trigmon"));
    
  saveFile.Close();
  SetStatusText(_T("Saved options"),2);

  return true;
}

// Save current display options
bool TrigMonFrame::LoadOptions(const wxString &file){
  using namespace TrigMonOpts;
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
  XML_SetElementHandler(parser, TrigMonXMLHandler::xmlTrigMonStart,
			TrigMonXMLHandler::xmlTrigMonEnd);

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
    SetTurfOptions();
    SetSurfOptions();
    m_optionsDialog->ImportOptions();
    SetStatusText(_T("Loaded new options"),2);
  }else
    SetStatusText(_T("Error loading options"),2);
  
  return m_xmlHandler->Ok();
}

// Loads turf data file in flight format
bool TrigMonFrame::ReadTurfData(const wxFileName &file,int order){
  // Open file
  wxFile turfFile(file.GetFullPath().c_str(),wxFile::read);
  if (!turfFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open TURF file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file
  TurfRateStruct_t turfdata;
  if(!m_fileHandler->ReadFromFile(turfFile,&turfdata,sizeof(TurfRateStruct_t),order)){
    wxMessageBox(_T("TURF rate packet file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    turfFile.Close();
    return false;
  }
  turfFile.Close();

  // Fill in data here
   m_turfViewer->TurfRateData(turfdata);
  m_flightControl->SetTime(TURFPCK,turfdata.unixTime);
  m_TURFInfo->SetTime(turfdata.unixTime);
 // m_TURFInfo->SetTimeUS(turfdata.unixTimeUs);//amir's addition july 12 08
  m_TURFInfo->SetTriggerNumber(0);
  m_TURFInfo->SetL3Count(0);
  m_TURFInfo->SetTriggerType(0);
  m_TURFInfo->SetPPS(0);
  m_TURFInfo->SetTrigTime(0);
  m_TURFInfo->SetC3PO(0);


  for(int i=0;i<N_L3;++i)
    m_L3Info->SetDisplayValue(TrigMonOpts::L3Labels[i], turfdata.l3Rates[i][0]);//TODO peng, it depends on the future data format.
  for(int i=0;i<N_L3;++i)
    m_L3hInfo->SetDisplayValue(TrigMonOpts::L3Labels[i], turfdata.l3Rates[i][1]);//TODO peng

  Refresh(true);
  wxString status=file.GetFullName();
  status << ' ' << order+1;
  SetStatusText(status,1);
  return true;
}

// Loads turf data file in raw format
bool TrigMonFrame::ReadTurfRawData(){
  // Open file
  wxFile turfFile(currentRawTurfFile->GetFullPath().c_str(),wxFile::read);
  if (!turfFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open TURF file %s"),currentRawTurfFile->GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file
  // TURFIO data structure, TV test 
  typedef struct {
    unsigned short rawtrig; // raw trigger number
    unsigned short L3Atrig; // same as raw trig
    unsigned long pps1;     // 1PPS
    unsigned long time;     // trig time
    unsigned long interval; // interval since last trig
    unsigned int rate[40][4]; // Antenna trigger output rates (Hz)
    unsigned int l2rate[32]; // level 2 trigger rate
    unsigned int l3rate[4][16] ;
  } TURFIO_struct; 
  TURFIO_struct turfdata;

  if (turfFile.Read(&turfdata, sizeof(TURFIO_struct) ) != 
      sizeof(TURFIO_struct)){
    wxMessageBox(_T("Raw TURF file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    turfFile.Close();
    return false;
  }

  turfFile.Close();

  // Fill in data here
  m_TURFInfo->SetTime(0);
  m_TURFInfo->SetTimeUS(0);
  m_TURFInfo->SetTriggerNumber(turfdata.rawtrig);
  m_TURFInfo->SetTriggerType(0);
  m_TURFInfo->SetL3Count(turfdata.L3Atrig);
  m_TURFInfo->SetPPS(turfdata.pps1);
  m_TURFInfo->SetTrigTime(turfdata.time);
  m_TURFInfo->SetC3PO(turfdata.interval);

  for(int i=0;i<L3_PER_ROW;++i)
    for(int j=0;j<N_L3/L3_PER_ROW;++j)
      m_L3Info->SetDisplayValue(TrigMonOpts::L3Labels[L3Id(i,j)],//TODO peng, also depends on data format.
				turfdata.l3rate[j][i]);

  Refresh(true);
  SetStatusText(currentRawTurfFile->GetFullName(),1);
  return true;
}

// Loads surf data file in flight format
bool TrigMonFrame::ReadSurfData(const wxFileName &file,int order){
  using namespace TrigMonOpts;
  // Open file
  wxFile surfFile(file.GetFullPath().c_str(),wxFile::read);
  if (!surfFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open SURF file %s"),file.GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file
  FullSurfHkStruct_t surfdata;
  if(!m_fileHandler->ReadFromFile(surfFile,&surfdata,sizeof(FullSurfHkStruct_t),order)){
    wxMessageBox(_T("SURF HK packet file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    surfFile.Close();
    return false;
  }
  surfFile.Close();

  // Lockstep mode
  if(Lockstep){
    time_t turfTime=m_flightControl->GetTime(TURFPCK);
    if(turfTime>0 && surfdata.unixTime<(unsigned long)turfTime){
      wxCommandEvent e;
      m_flightControl->Next(SURFPCK,e);
      return false;
    }
  }

  // Fill in data here
  m_surfViewer->SurfHKData(surfdata);
  m_flightControl->SetTime(SURFPCK,surfdata.unixTime);
  m_SURFInfo->SetTime(surfdata.unixTime);
  m_SURFInfo->SetTimeUS(surfdata.unixTimeUs);
  m_SURFInfo->SetGlobalThreshold(surfdata.globalThreshold);
  m_SURFInfo->SetErrorFlag(surfdata.errorFlag);
  for(int i=0;i<N_SURF;++i) m_SURFInfo->SetUpperWords(i,surfdata.upperWords[i]);

  for(int i=0;i<SCALER_PER_ROW;++i)
    for(int j=0;j<N_SCALER/SCALER_PER_ROW;++j){
  //    if(surfdata.scalerGoal>0){
//	if(ScalerScaleType[SCALERId(i,j)]==Linear)
//	    dynamicScalerSpec[SCALERId(i,j)][0]=
//	      wxRangeSpec(surfdata.scalerGoal*(1-SCALER_GOAL_PRECISION)*1e3,
//			  surfdata.scalerGoal*(1+SCALER_GOAL_PRECISION)*1e3,wxGREEN);
//	else
//	  dynamicScalerSpec[SCALERId(i,j)][0]=
//	    wxRangeSpec(log10(surfdata.scalerGoal*(1-SCALER_GOAL_PRECISION)*1e3),
//			log10(surfdata.scalerGoal*(1+SCALER_GOAL_PRECISION)*1e3),wxGREEN);
 //     }
      if(j<N_SCALER/SCALER_PER_ROW/2){
//	if(DynamicTarget && surfdata.scalerGoal>0)
	  m_ScalerInfo1->SetDisplayRangeSpec(ScalerRefLabels[SCALERId(i,j)],dynamicScalerSpec[SCALERId(i,j)]);
	m_ScalerInfo1->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],
				       surfdata.scaler[j][i]*1e3);
      }else{
//	if(DynamicTarget && surfdata.scalerGoal>0)
	   m_ScalerInfo2->SetDisplayRangeSpec(ScalerRefLabels[SCALERId(i,j)],dynamicScalerSpec[SCALERId(i,j)]);
	m_ScalerInfo2->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],
				       surfdata.scaler[j][i]*1e3);
      }
    }

  for(int i=0;i<THRESHOLD_PER_ROW;++i)
    for(int j=0;j<N_THRESHOLD/THRESHOLD_PER_ROW;++j){
      if(surfdata.setThreshold[j][i]>0){
	if(ThresholdScaleType[THRESHOLDId(i,j)]==Linear)
	    dynamicThresholdSpec[THRESHOLDId(i,j)][0]=
	      wxRangeSpec(surfdata.setThreshold[j][i],surfdata.setThreshold[j][i]+1,wxGREEN);
	  else
	    dynamicThresholdSpec[THRESHOLDId(i,j)][0]=
	      wxRangeSpec(log10(surfdata.setThreshold[j][i]),log10(surfdata.setThreshold[j][i]+1),wxGREEN);
      }
      if(j<N_THRESHOLD/THRESHOLD_PER_ROW/2){
	if(DynamicTarget && surfdata.setThreshold[j][i]>0)
	  m_ThresholdInfo1->SetDisplayRangeSpec(ThresholdRefLabels[THRESHOLDId(i,j)],dynamicThresholdSpec[THRESHOLDId(i,j)]);
	m_ThresholdInfo1->SetDisplayValue(ThresholdRefLabels[THRESHOLDId(i,j)],
					  //surfdata.threshold[j][i]*2.5/4096); // in V
					  surfdata.threshold[j][i]); 
      }else{
	if(DynamicTarget && surfdata.setThreshold[j][i]>0)
	  m_ThresholdInfo2->SetDisplayRangeSpec(ThresholdRefLabels[THRESHOLDId(i,j)],dynamicThresholdSpec[THRESHOLDId(i,j)]);
	m_ThresholdInfo2->SetDisplayValue(ThresholdRefLabels[THRESHOLDId(i,j)],
					  //surfdata.threshold[j][i]*2.5/4096);  // in V
					  surfdata.threshold[j][i]);  
      }
    }  
  
  for(int i=0;i<RFPOWER_PER_ROW;++i)
    for(int j=0;j<N_RFPOWER/RFPOWER_PER_ROW;++j){
      double rf;
      switch(rfPowerType){
      case dBm:
	rf = rfpower_dBm(j,i,surfdata.rfPower[j][i]&0x7FFF);
	break;
      
      case ADC:
	// default:
	rf = surfdata.rfPower[j][i]&0x7FFF;
	break;
      case T: 
      default:
	rf= rfpower_T(j, i, surfdata.rfPower[j][i]&0x7FFF);
	break;
      }
      if(j<N_RFPOWER/RFPOWER_PER_ROW/2)
	m_RFInfo1->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
      else
	m_RFInfo2->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
    }      
/*//amir's addition
  for(int i=0;i<N_SURF;++i){
    unsigned long trueMask=m_SURFInfo->SetBandMask(i,surfdata.surfTrigBandMask[i]);
    if(i<N_SCALER/SCALER_PER_ROW) SetMaskedBackground(i,trueMask);
  }
  */
  Refresh(true);
  wxString status=file.GetFullName();
  status << ' ' << order+1;
  SetStatusText(status,2);
  return true;
}

void TrigMonFrame::SetMaskedBackground(int surf,unsigned long mask){
  static wxColour maskBg=wxColour(92,92,92);
  // Set background colour on scaler displays according to mask
  for(int i=0;i<SCALER_PER_ROW;++i)
    if(surf<N_SCALER/SCALER_PER_ROW/2){
      if(mask&(0x1<<i))
	m_ScalerInfo1->SetDisplayBackground(TrigMonOpts::ScalerRefLabels[SCALERId(i,surf)],maskBg);
      else
	m_ScalerInfo1->SetDisplayBackground(TrigMonOpts::ScalerRefLabels[SCALERId(i,surf)],*wxBLACK);
    }else{
      if(mask&(0x1<<i))
	m_ScalerInfo2->SetDisplayBackground(TrigMonOpts::ScalerRefLabels[SCALERId(i,surf)],maskBg);
      else
	m_ScalerInfo2->SetDisplayBackground(TrigMonOpts::ScalerRefLabels[SCALERId(i,surf)],*wxBLACK);
    }
  // Set background colour on threshold displays according to mask
  for(int i=0;i<THRESHOLD_PER_ROW;++i)
    if(surf<N_THRESHOLD/THRESHOLD_PER_ROW/2){
      if(mask&(0x1<<i))
	m_ThresholdInfo1->SetDisplayBackground(TrigMonOpts::ThresholdRefLabels[THRESHOLDId(i,surf)],maskBg);
      else
	m_ThresholdInfo1->SetDisplayBackground(TrigMonOpts::ThresholdRefLabels[THRESHOLDId(i,surf)],*wxBLACK);
    }else{
      if(mask&(0x1<<i))
	m_ThresholdInfo2->SetDisplayBackground(TrigMonOpts::ThresholdRefLabels[THRESHOLDId(i,surf)],maskBg);
      else
	m_ThresholdInfo2->SetDisplayBackground(TrigMonOpts::ThresholdRefLabels[THRESHOLDId(i,surf)],*wxBLACK);
    }
  return;
}

// Loads surf data file in raw format
bool TrigMonFrame::ReadSurfRawData(){
  using namespace TrigMonOpts;

  // Open file
  wxFile surfFile(currentRawSurfFile->GetFullPath().c_str(),wxFile::read);
  if (!surfFile.IsOpened()){
    wxString error;
    error.Printf(_T("Failed to open SURF file %s"),currentRawSurfFile->GetFullName().c_str());
    wxMessageBox(error, _T("File opening error"), wxOK|wxICON_ERROR);
    return false;
  }

  // Read file
  // Raw SURF HK data structure.
  typedef struct {
    unsigned short globalThreshold; //set to zero if there isn't one
    unsigned short errorFlag; //Upper Word consistency
    unsigned short upperWords[N_SURF];
    unsigned short scaler[N_SURF][SCALER_PER_ROW];
    unsigned short threshold[N_SURF][THRESHOLD_PER_ROW];
    unsigned short rfPower[N_SURF][N_CHN-1];
  } RawSurfHkStruct_t;
  RawSurfHkStruct_t surfdata;

  if(surfFile.Read(&surfdata, sizeof(RawSurfHkStruct_t) ) != 
     sizeof(RawSurfHkStruct_t)){
    wxMessageBox(_T("Raw SURF HK file is not of expected size"),
		 _T("File size error"), wxOK|wxICON_ERROR);
    surfFile.Close();
    return false;
  }
  surfFile.Close();

  // Fill in data here
  m_SURFInfo->SetTime(0);
  m_SURFInfo->SetTimeUS(0);
  m_SURFInfo->SetGlobalThreshold(surfdata.globalThreshold);
  m_SURFInfo->SetErrorFlag(surfdata.errorFlag);
  for(int i=0;i<N_SURF;++i) m_SURFInfo->SetUpperWords(i,surfdata.upperWords[i]);

  for(int i=0;i<SCALER_PER_ROW;++i)
    for(int j=0;j<N_SCALER/SCALER_PER_ROW;++j)
      if(j<N_SCALER/SCALER_PER_ROW/2){
	m_ScalerInfo1->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],
				       surfdata.scaler[j][i]*1e3);
      }else{
	m_ScalerInfo2->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],
				       surfdata.scaler[j][i]*1e3);
      }
  
  for(int i=0;i<THRESHOLD_PER_ROW;++i)
    for(int j=0;j<N_THRESHOLD/THRESHOLD_PER_ROW;++j)
      if(j<N_THRESHOLD/THRESHOLD_PER_ROW/2){
	m_ThresholdInfo1->SetDisplayValue(ThresholdRefLabels[THRESHOLDId(i,j)],
					  //surfdata.threshold[j][i]*2.5/4096); // in V
					  surfdata.threshold[j][i]); 
      }else{
	m_ThresholdInfo2->SetDisplayValue(ThresholdRefLabels[THRESHOLDId(i,j)],
					  //surfdata.threshold[j][i]*2.5/4096);  // in V
					  surfdata.threshold[j][i]); 
      }

  for(int i=0;i<RFPOWER_PER_ROW;++i)
    for(int j=0;j<N_RFPOWER/RFPOWER_PER_ROW;++j){
      double rf;
      switch(rfPowerType){
      case dBm:
	rf = rfpower_dBm(j,i,surfdata.rfPower[j][i]&0x7FFF);
	break;
      case T: 
	rf= rfpower_T(j, i, surfdata.rfPower[j][i]&0x7FFF);
	break;
      case ADC:
      default:
	rf = surfdata.rfPower[j][i]&0x7FFF;
	break;
      }
      if(j<N_RFPOWER/RFPOWER_PER_ROW/2)
	m_RFInfo1->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
      else
	m_RFInfo2->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
    }  
  
  SetStatusText(currentRawSurfFile->GetFullName(),2);
  return true;
}

void TrigMonFrame::IndexTurfDirectory(bool full){
  wxString filter;
  wxString dirname;
  wxArrayString::CompareFunction comp;
  if(turfRaw){
    dirname=currentRawTurfFile->GetPath();
    filter=_T("turf_data.*");
    comp=extensionSortFunction;
  }else{
    dirname=packetDirectory[TURFPCK].GetPath();
    filter=_T("turfhk_*.dat*");
    comp=singleIndexSortFunction;
  }
  // Index current directory
  wxDir dir;
  if(full){ // Full indexing
    packetFileList[TURFPCK]->Empty();
    dir.GetAllFiles(dirname,packetFileList[TURFPCK],filter,wxDIR_FILES);
    if(packetFileList[TURFPCK]->GetCount()){
      packetFileList[TURFPCK]->Sort(comp);
      m_flightControl->SetList(TURFPCK,packetFileList[TURFPCK],1000);
      m_pTurfNext->Enable(true);
      m_pTurfPrev->Enable(true);
      wxLogMessage(_T("[TrigMonFrame::IndexTurfDirectory] TURF rate files indexed."));
    }else{
      m_pTurfNext->Enable(false);
      m_pTurfPrev->Enable(false);
    }
  }else{
    wxArrayString newFiles;
    dir.GetAllFiles(dirname,&newFiles,filter,wxDIR_FILES);
    if(newFiles.GetCount()>packetFileList[TURFPCK]->GetCount()){
      newFiles.Sort(comp);
      for(unsigned int i=packetFileList[TURFPCK]->GetCount();i<newFiles.GetCount();++i)
	packetFileList[TURFPCK]->Add(newFiles[i]);
      wxLogMessage(_T("[TrigMonFrame::IndexTurfDirectory] Updated list of TURF rate files."));
    }
  }
  return;
}

void TrigMonFrame::IndexSurfDirectory(bool full){
  wxString filter;
  wxString dirname;
  wxArrayString::CompareFunction comp;
  if(surfRaw){
    dirname=currentRawSurfFile->GetPath();
    filter=_T("hksurf_data.*");
    comp=extensionSortFunction;
  }else{
    dirname=packetDirectory[SURFPCK].GetPath(); 
    filter=_T("surfhk_*.dat*");
    comp=singleIndexSortFunction;
  }
  // Index current directory
  wxDir dir;
  if(full){ // Full indexing
    packetFileList[SURFPCK]->Empty();
    dir.GetAllFiles(dirname,packetFileList[SURFPCK],filter,wxDIR_FILES);
    if(packetFileList[SURFPCK]->GetCount()){
      packetFileList[SURFPCK]->Sort(comp);
      m_flightControl->SetList(SURFPCK,packetFileList[SURFPCK],1000);
      m_pSurfNext->Enable(true);
      m_pSurfPrev->Enable(true);
      wxLogMessage(_T("[TrigMonFrame::IndexSurfDirectory] SURF HK files indexed."));
    }else{
      m_pSurfNext->Enable(false);
      m_pSurfPrev->Enable(false);
    }
  }else{
    wxArrayString newFiles;
    dir.GetAllFiles(dirname,&newFiles,filter,wxDIR_FILES);
    if(newFiles.GetCount()>packetFileList[SURFPCK]->GetCount()){
      newFiles.Sort(comp);
      for(unsigned int i=packetFileList[SURFPCK]->GetCount();i<newFiles.GetCount();++i)
	packetFileList[SURFPCK]->Add(newFiles[i]);
      wxLogMessage(_T("[TrigMonFrame::IndexSurfDirectory] Updated list of SURF HK files."));
    }
  }
  return;
}

void TrigMonFrame::sqlParser(PGresult *result){
  if(!sqlRequestType.GetCount())
    wxLogError(_T("[TrigMonFrame::sqlParser] Received SQL query result, but no request types in the buffer."));
  else{
    int type=sqlRequestType[0];
    sqlRequestType.RemoveAt(0);
    ExecStatusType status=PQresultStatus(result);
    if(status==PGRES_COMMAND_OK){ 
      //wxLogMessage("[TrigMonFrame::sqlParser] OK");
    }else if(status!=PGRES_TUPLES_OK){
      wxLogError(_U(PQresStatus(status)));
      wxLogError(_U(PQresultErrorMessage(result)));
    }else if(PQntuples(result)==0 && type<INDEX_OFFSET){
      wxLogError(_T("[SlowMoFrame::sqlParser] Received SQL query result w/o record; type %d"),type);
    }else{
      //wxLogMessage("[SlowMoFrame::sqlParser] Received SQL data type %d",type);
      switch(type){
      case SURFPCK: ParseSurfPacket(result); Refresh(); break;
      case TURFPCK: ParseTurfPacket(result); Refresh(); break;
      case SURFPCK+INDEX_OFFSET: 
      case TURFPCK+INDEX_OFFSET: 
	StorePacketList(result,type-INDEX_OFFSET,true);
	if(showProgress){
	  wxString status;
	  status.Printf(_T("DB indexing %d%% complete"),(type-INDEX_OFFSET+1)*100/N_PCKTYPE);
	  SetStatusText(status,1);
	}
	break;
      default:
	wxLogError(_T("[TrigMonFrame::sqlParser] Received results of unknown SQL request type %d"),type);
	break;
      }
    }
  }
  PQclear(result);
}

void TrigMonFrame::ParseSurfPacket(PGresult *result){
  using namespace TrigMonOpts;
   int n=PQntuples(result);
   int coln;
   if(n>1)
     wxLogError(_T("[TrigMonFrame::ParseSurfPacket] More than one SURF HK record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[TrigMonFrame::ParseSurfPacket] No SURF HK record received."));
    return;
  }
   
  unsigned long t=0;
  if((coln=PQfnumber(result,"time"))>=0) t=atol(PQgetvalue(result,0,coln));
  m_SURFInfo->SetTime(t);
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  m_dbControl->SetTime(SURFPCK,t,!(crc&0x1));
  if((coln=PQfnumber(result,"us"))>=0) m_SURFInfo->SetTimeUS(atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"global"))>=0) m_SURFInfo->SetGlobalThreshold(atol(PQgetvalue(result,0,coln)));
  if((coln=PQfnumber(result,"error"))>=0) m_SURFInfo->SetErrorFlag(atol(PQgetvalue(result,0,coln)));

  long upper[N_SURF];
  if((coln=PQfnumber(result,"upper"))>=0 && ParseLongSQLArray(result,upper,N_SURF,coln))
    for(int i=0;i<N_SURF;++i) 
      m_SURFInfo->SetUpperWords(i,upper[i]);
//////////////////////////////////////////////////////////////////////////////
long SGoals[ANT_LAYERS];
long scalergoals[ANT_LAYERS];
  if((coln=PQfnumber(result,"scalergoals"))>=0 && ParseLongSQLArray(result,SGoals,ANT_LAYERS,coln))
    for(int i=0;i<ANT_LAYERS;++i) 
     scalergoals[i]=SGoals[i];
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// long NGoals[ANTS_PER_SURF];
// long nadirgoals[ANTS_PER_SURF];
//   if((coln=PQfnumber(result,"nadirgoals"))>=0 && ParseLongSQLArray(result,NGoals,ANTS_PER_SURF,coln))
//   for(int i=0;i<ANTS_PER_SURF;++i) 
//      nadirgoals[i]=NGoals[i];
///////////////////////////////////////////////////////////////////////
 // long scalerGoal=0;
//  if((coln=PQfnumber(result,"scalergoal"))>=0) scalerGoal=atol(PQgetvalue(result,0,coln));//amir's addition

  long *scaler[N_SCALER/SCALER_PER_ROW];
  long *thresh[N_THRESHOLD/THRESHOLD_PER_ROW];
  long *threshset[N_THRESHOLD/THRESHOLD_PER_ROW];
  long *rfpow[N_RFPOWER/RFPOWER_PER_ROW];
  long dimScaler[N_SCALER/SCALER_PER_ROW];
  for(int i=0;i<N_SCALER/SCALER_PER_ROW;++i){
    dimScaler[i]=SCALER_PER_ROW;
    scaler[i]=new long[SCALER_PER_ROW];
  }
  long dimThresh[N_THRESHOLD/THRESHOLD_PER_ROW];
  for(int i=0;i<N_THRESHOLD/THRESHOLD_PER_ROW;++i){
    dimThresh[i]=THRESHOLD_PER_ROW;
    thresh[i]=new long[THRESHOLD_PER_ROW];
    threshset[i]=new long[THRESHOLD_PER_ROW];
  }
  long dimRFpow[N_RFPOWER/RFPOWER_PER_ROW];
  for(int i=0;i<N_RFPOWER/RFPOWER_PER_ROW;++i){
    dimRFpow[i]=RFPOWER_PER_ROW;
    rfpow[i]=new long[RFPOWER_PER_ROW];
  }
/*
  if((coln=PQfnumber(result,"scaler"))>=0 && ParseLongSQLArray2D(result,scaler,N_SCALER/SCALER_PER_ROW, dimScaler,coln)){
    for(int i=0;i<SCALER_PER_ROW;++i)
      for(int j=0;j<N_SCALER/SCALER_PER_ROW;++j){
	if(scalerGoal>0){
	  if(ScalerScaleType[SCALERId(i,j)]==Linear)
	    dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(scalerGoal*(1-SCALER_GOAL_PRECISION)*1e3,
							    scalerGoal*(1+SCALER_GOAL_PRECISION)*1e3,
							    wxGREEN);
	  else
	    dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(scalerGoal*(1-SCALER_GOAL_PRECISION)*1e3),
							    log10(scalerGoal*(1+SCALER_GOAL_PRECISION)*1e3),
							    wxGREEN);
	}
	if(j<N_SCALER/SCALER_PER_ROW/2){
	  if(DynamicTarget && scalerGoal>0)
	    m_ScalerInfo1->SetDisplayRangeSpec(ScalerRefLabels[SCALERId(i,j)],dynamicScalerSpec[SCALERId(i,j)]);
	  m_ScalerInfo1->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],scaler[j][i]*1e3);
	}else{
	  if(DynamicTarget && scalerGoal>0)
	    m_ScalerInfo2->SetDisplayRangeSpec(ScalerRefLabels[SCALERId(i,j)],dynamicScalerSpec[SCALERId(i,j)]);
	  m_ScalerInfo2->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],scaler[j][i]*1e3);
	}
      }
  }
  */
//////////////////////////////////////////////////////////////////////////////////////////////
  if((coln=PQfnumber(result,"scaler"))>=0 && ParseLongSQLArray2D(result,scaler,N_SCALER/SCALER_PER_ROW, dimScaler,coln)){
    for(int i=0;i<SCALER_PER_ROW;++i)
      for(int j=0;j<N_SCALER/SCALER_PER_ROW;++j){
	if((scalergoals[0] >0||scalergoals[1]>0 ||scalergoals[2]>0)){
	  if(j%3==0){
	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(scalergoals[0]*(1-SCALER_GOAL_PRECISION)*1e3, scalergoals[0]*(1+SCALER_GOAL_PRECISION)*1e3, wxGREEN);
	    else
	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(scalergoals[0]*(1-SCALER_GOAL_PRECISION)*1e3),log10(scalergoals[0]*(1+SCALER_GOAL_PRECISION)*1e3), wxGREEN);
	  }
	  if(j%3==1){
	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
	      dynamicScalerSpec[SCALERId(i,j)][1]=wxRangeSpec(scalergoals[1]*(1-SCALER_GOAL_PRECISION)*1e3,scalergoals[1]*(1+SCALER_GOAL_PRECISION)*1e3, wxGREEN);
	    else
	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(scalergoals[1]*(1-SCALER_GOAL_PRECISION)*1e3),log10(scalergoals[1]*(1+SCALER_GOAL_PRECISION)*1e3), wxGREEN);
	  }
	  if(j%3==2){
	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
	      dynamicScalerSpec[SCALERId(i,j)][1]=wxRangeSpec(scalergoals[2]*(1-SCALER_GOAL_PRECISION)*1e3, scalergoals[2]*(1+SCALER_GOAL_PRECISION)*1e3, wxGREEN);
	    else
	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(scalergoals[2]*(1-SCALER_GOAL_PRECISION)*1e3), log10(scalergoals[2]*(1+SCALER_GOAL_PRECISION)*1e3), wxGREEN);
	  }
// 	  if(j%4==3){
// 	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
// 	      dynamicScalerSpec[SCALERId(i,j)][1]=wxRangeSpec(scalergoals[3]*(1-SCALER_GOAL_PRECISION)*1e3,scalergoals[1]*(1+SCALER_GOAL_PRECISION)*1e3, wxGREEN);
// 	    else
// 	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(scalergoals[3]*(1-SCALER_GOAL_PRECISION)*1e3), log10(scalergoals[3]*(1+SCALER_GOAL_PRECISION)*1e3), wxGREEN);
// 	  }		
			
// no nadirgoals for ANITA-3  SM  13-Oct-14			}
// 	if((nadirgoals[0] >0||nadirgoals[1]>0 ||nadirgoals[2]>0) && i>8 ){
// 	  if(j%4==0){
// 	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
// 	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(nadirgoals[0]*(1-SCALER_GOAL_PRECISION)*1e3, nadirgoals[0]*(1+SCALER_GOAL_PRECISION)*1e3, wxGREEN);
// 	    else
// 	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(nadirgoals[0]*(1-SCALER_GOAL_PRECISION)*1e3), log10(nadirgoals[0]*(1+SCALER_GOAL_PRECISION)*1e3), wxGREEN);
// 	  }
// 	  if(j%4==1){
// 	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
// 	      dynamicScalerSpec[SCALERId(i,j)][1]=wxRangeSpec(nadirgoals[1]*(1-SCALER_GOAL_PRECISION)*1e3, nadirgoals[1]*(1+SCALER_GOAL_PRECISION)*1e3,wxGREEN);
// 	    else
// 	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(nadirgoals[1]*(1-SCALER_GOAL_PRECISION)*1e3),log10(nadirgoals[1]*(1+SCALER_GOAL_PRECISION)*1e3),  wxGREEN);
// 	  }
// 	  if(j%4==2){
// 	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
// 	      dynamicScalerSpec[SCALERId(i,j)][1]=wxRangeSpec(nadirgoals[2]*(1-SCALER_GOAL_PRECISION)*1e3, nadirgoals[2]*(1+SCALER_GOAL_PRECISION)*1e3, wxGREEN);
// 	    else
// 	      dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(nadirgoals[2]*(1-SCALER_GOAL_PRECISION)*1e3),log10(nadirgoals[2]*(1+SCALER_GOAL_PRECISION)*1e3), wxGREEN);
// 	  }
// 	  if(j%4==3){
// 	    if(ScalerScaleType[SCALERId(i,j)]==Linear)
// 	      dynamicScalerSpec[SCALERId(i,j)][1]=wxRangeSpec(nadirgoals[3]*(1-SCALER_GOAL_PRECISION)*1e3,nadirgoals[1]*(1+SCALER_GOAL_PRECISION)*1e3,    wxGREEN);
// 	  else
// 	    dynamicScalerSpec[SCALERId(i,j)][0]=wxRangeSpec(log10(nadirgoals[3]*(1-SCALER_GOAL_PRECISION)*1e3),
// 							    log10(nadirgoals[3]*(1+SCALER_GOAL_PRECISION)*1e3),
// 							    wxGREEN);
// 					}
	}
	
	if(j<N_SCALER/SCALER_PER_ROW/2){
	  //if(DynamicTarget && (scalergoals[0] >0||scalergoals[1]>0 ||scalergoals[2]>0)
	  m_ScalerInfo1->SetDisplayRangeSpec(ScalerRefLabels[SCALERId(i,j)],dynamicScalerSpec[SCALERId(i,j)]);
	  m_ScalerInfo1->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],scaler[j][i]*1e3);
	} 
	else {
	   //if(DynamicTarget && (scalergoals[0] >0||scalergoals[1]>0 ||scalergoals[2]>0)
	  m_ScalerInfo2->SetDisplayRangeSpec(ScalerRefLabels[SCALERId(i,j)],dynamicScalerSpec[SCALERId(i,j)]);
	  m_ScalerInfo2->SetDisplayValue(ScalerRefLabels[SCALERId(i,j)],scaler[j][i]*1e3);
	}
      }
  }
/////////////////////////////////////////////////////////////////////////////////////////////
  if((coln=PQfnumber(result,"thresh"))>=0 && 
     ParseLongSQLArray2D(result,thresh,N_THRESHOLD/THRESHOLD_PER_ROW, dimThresh,coln) &&
     (coln=PQfnumber(result,"threshset"))>=0 && 
     ParseLongSQLArray2D(result,threshset,N_THRESHOLD/THRESHOLD_PER_ROW, dimThresh,coln)){
    for(int i=0;i<THRESHOLD_PER_ROW;++i)
      for(int j=0;j<N_THRESHOLD/THRESHOLD_PER_ROW;++j){
	if(threshset[j][i]>0){
	  if(ThresholdScaleType[THRESHOLDId(i,j)]==Linear)
	    dynamicThresholdSpec[THRESHOLDId(i,j)][0]=wxRangeSpec(threshset[j][i],threshset[j][i]+1,
								  wxGREEN);
	  else
	    dynamicThresholdSpec[THRESHOLDId(i,j)][0]=wxRangeSpec(log10(threshset[j][i]),
								  log10(threshset[j][i]+1),
								  wxGREEN);
	}
	if(j<N_THRESHOLD/THRESHOLD_PER_ROW/2){
	  if(DynamicTarget && threshset[j][i]>0)
	    m_ThresholdInfo1->SetDisplayRangeSpec(ThresholdRefLabels[THRESHOLDId(i,j)],
						  dynamicThresholdSpec[THRESHOLDId(i,j)]);
	  m_ThresholdInfo1->SetDisplayValue(ThresholdRefLabels[THRESHOLDId(i,j)],
					    //thresh[j][i]*2.5/4096); // in V
					    thresh[j][i]); // in V
	}else{
	  if(DynamicTarget && threshset[j][i]>0)
	    m_ThresholdInfo2->SetDisplayRangeSpec(ThresholdRefLabels[THRESHOLDId(i,j)],
						  dynamicThresholdSpec[THRESHOLDId(i,j)]);
	  m_ThresholdInfo2->SetDisplayValue(ThresholdRefLabels[THRESHOLDId(i,j)],
					    //thresh[j][i]*2.5/4096);  // in V
					    thresh[j][i]);
	}
      }
  }

  if((coln=PQfnumber(result,"rfpow"))>0 && ParseLongSQLArray2D(result,rfpow,N_RFPOWER/RFPOWER_PER_ROW, dimRFpow,coln)){
    for(int i=0;i<RFPOWER_PER_ROW;++i)
      for(int j=0;j<N_RFPOWER/RFPOWER_PER_ROW;++j){
	double rf;
	switch(rfPowerType){
	case dBm:
	  rf = rfpower_dBm(j,i,rfpow[j][i]&0x7FFF);
	  break;
	case ADC:
	  rf = rfpow[j][i]&0x7FFF;//TODO  mask off the highest bit.
	  break;
  	case T:
	default:
	  rf=rfpower_T(j, i, rfpow[j][i]&0x7FFF);
	  break;
	}
	if(j<N_RFPOWER/RFPOWER_PER_ROW/2) {
	  m_RFInfo1->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
	  //	std::cout<<"rf="<<rf<<std::endl; //amir's addition debug
	}
	else {
	  m_RFInfo2->SetDisplayValue(RFPowerLabels[RFPOWERId(i,j)],rf);
	  //	std::cout<<"rf ="<<rf<<std::endl;//amir's addtion debug
	}
      }
  }

/*
  long surfmask[N_SURF];
  if((coln=PQfnumber(result,"surfmask"))>=0 && ParseLongSQLArray(result,surfmask,N_SURF,coln))
    for(int i=0;i<N_SURF;++i){
      unsigned long trueMask=(unsigned long)surfmask[i];
      m_SURFInfo->SetBandMask(i,trueMask);
      if(i<N_SCALER/SCALER_PER_ROW) SetMaskedBackground(i,trueMask);
    }
  */
  // Clear memory
  for(int i=0;i<N_SCALER/SCALER_PER_ROW;++i) delete[] scaler[i];
  for(int i=0;i<N_THRESHOLD/THRESHOLD_PER_ROW;++i){
    delete[] thresh[i];
    delete[] threshset[i];
  }
  for(int i=0;i<N_RFPOWER/RFPOWER_PER_ROW;++i) delete[] rfpow[i];

  wxString status;
  status.Printf(_T("SURF HK %ld"),t);
  SetStatusText(status,1);
  return;
} 

void TrigMonFrame::ParseTurfPacket(PGresult *result){
  int n=PQntuples(result);
  int coln;
  if(n>1)
    wxLogError(_T("[TrigMonFrame::ParseTurfPacket] More than one TURF rate record received, ignoring all but first."));
  if(n<1){
    wxLogError(_T("[TrigMonFrame::ParseTurfPacket] No TURF rate record received."));
    return;
  }

  
  unsigned long t=0;
  if((coln=PQfnumber(result,"time"))>=0) t=atol(PQgetvalue(result,0,coln));  
  m_TURFInfo->SetTime(t);
  unsigned short crc=0;
  if((coln=PQfnumber(result,"crc"))>=0) crc=atoi(PQgetvalue(result,0,coln));
  unsigned long phitrigmask=0;
  if((coln=PQfnumber(result,"phitrigmask"))>=0) phitrigmask=atol(PQgetvalue(result,0,coln));
  //std::cout<<"phitrigmask="<<phitrigmask<<std::endl;
  // unsigned long deadtime=0;
  //  unsigned long d_time=0;
  if((coln=PQfnumber(result,"deadtime"))>=0)  m_TURFInfo->SetTrigTime(atol(PQgetvalue(result,0,coln))/65535.);

  unsigned short l1TrigMask=0;
  if((coln=PQfnumber(result,"l1trigmask"))>=0) l1TrigMask=atol(PQgetvalue(result,0,coln));
	for(int i=0;i<16;++i){
    		if(l1TrigMask&(1<<i))
      			m_L1Mask->SetDisplayValue(TrigMonOpts::L1MaskLabels[i],1);
		else
      			m_L1Mask->SetDisplayValue(TrigMonOpts::L1MaskLabels[i],0);
	}		
  unsigned short l1TrigMaskH=0;
  if((coln=PQfnumber(result,"l1trigmaskh"))>=0) l1TrigMaskH=atol(PQgetvalue(result,0,coln));
	for(int i=0;i<16;++i){
    		if(l1TrigMaskH&(1<<i))
      			m_L1hMask->SetDisplayValue(TrigMonOpts::L1MaskLabels[i],1);
		else
      			m_L1hMask->SetDisplayValue(TrigMonOpts::L1MaskLabels[i],0);
	}

  m_dbControl->SetTime(TURFPCK,t,!(crc&0x1));
  m_TURFInfo->SetTriggerNumber(0);
  m_TURFInfo->SetTimeUS(phitrigmask);
  m_TURFInfo->SetL3Count(0);
  m_TURFInfo->SetTriggerType(0);
  m_TURFInfo->SetPPS(0);
  // m_TURFInfo->SetTrigTime(deadtime);
  m_TURFInfo->SetC3PO(0);
/*
  long l1mask[N_L1];
  if((coln=PQfnumber(result,"l1"))>=0 && ParseLongSQLArray(result,l1mask,N_L1,coln))
    for(int i=0;i<N_L1;++i) 
      m_L1Mask->SetDisplayValue(TrigMonOpts::L1Labels[i],l1mask[i]);
  long l1hmask[N_L1];
  if((coln=PQfnumber(result,"l1h"))>=0 && ParseLongSQLArray(result,l1hmask,N_L1,coln))
    for(int i=0;i<N_L1;++i) 
      m_L1hMask->SetDisplayValue(TrigMonOpts::L1Labels[i],l1hmask[i]);
*/
  long l1[N_L1];
  if((coln=PQfnumber(result,"l1"))>=0 && ParseLongSQLArray(result,l1,N_L1,coln))
    for(int i=0;i<N_L1;++i) 
      m_L1Info->SetDisplayValue(TrigMonOpts::L1Labels[i],l1[i]);
  long l1h[N_L1];
  if((coln=PQfnumber(result,"l1h"))>=0 && ParseLongSQLArray(result,l1h,N_L1,coln))
    for(int i=0;i<N_L1;++i) 
      m_L1hInfo->SetDisplayValue(TrigMonOpts::L1Labels[i],l1h[i]);
  long l3[N_L3];
  if((coln=PQfnumber(result,"l3"))>=0 && ParseLongSQLArray(result,l3,N_L3,coln))
    for(int i=0;i<N_L3;++i) 
      m_L3Info->SetDisplayValue(TrigMonOpts::L3Labels[i],l3[i]);
  long l3h[N_L3];
  if((coln=PQfnumber(result,"l3h"))>=0 && ParseLongSQLArray(result,l3h,N_L3,coln))
    for(int i=0;i<N_L3;++i) 
      m_L3hInfo->SetDisplayValue(TrigMonOpts::L3Labels[i],l3h[i]);
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /*
  long *l1[L1_PER_ROW];
  long *l2[N_L2/L2_PER_ROW];
  long diml1[L1_PER_ROW];
  long diml2[N_L2/L2_PER_ROW];
  for(int i=0;i<L1_PER_ROW;++i){
    diml1[i]=N_L1/L1_PER_ROW;
    l1[i]=new long[N_L1/L1_PER_ROW];
  }
  for(int i=0;i<N_L2/L2_PER_ROW;++i){
    diml2[i]=L2_PER_ROW;
    l2[i]=new long[L2_PER_ROW];
  }

  if((coln=PQfnumber(result,"l1"))>=0 && ParseLongSQLArray2D(result,l1,L1_PER_ROW, diml1,coln)){
    for(int i=0;i<L1_PER_ROW;++i)
      for(int j=0;j<N_L1/L1_PER_ROW;++j)
	m_L1Info->SetDisplayValue(TrigMonOpts::L1Labels[L1Id(i,j)],l1[i][j]*16u); // Multiply by 16
  }
*/


  wxString status;
  status.Printf(_T("TURF %ld"),t);
  SetStatusText(status,1);
  return;
} 

void TrigMonFrame::SendListen(int type){
  wxString cmd;
  cmd << _T("LISTEN ") << packetTable[type] << ';';
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlRequestType.Add(LISTEN);
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
}

void TrigMonFrame::ReceiveNotifications(){
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

void TrigMonFrame::IndexDBPackets(int type,bool full){
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
  cmd.Printf(_T("SELECT nbuf,time,now FROM %s WHERE now>=%ld %s ORDER by now DESC;"),
	     packetTable[type].c_str(),packetListUpdateTime[type],
	     TrigMonOpts::crcCheck?_T("AND crc=257"):_T(""));
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  ProcessEvent(sqlEvent);
  return;
}

bool TrigMonFrame::ReallocateList(int type,int n){
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

void TrigMonFrame::StorePacketList(PGresult *result,int type,bool haveus){
  int n=PQntuples(result);
  if(n<=0) return;
  m_dbControl->MarkIndex(type);
  ReallocateList(type,n);
  int timecol=PQfnumber(result,"time");
 // int uscol=(haveus?PQfnumber(result,"us"):-1);//amir's addition july 20 2008
  int nbufcol=PQfnumber(result,"nbuf");
  int nowcol=PQfnumber(result,"now");
 // if(timecol<0 || nbufcol<0 || (haveus && uscol<0) ||  nowcol<0){
	if(timecol<0 || nbufcol<0 ||  nowcol<0){
    wxLogError(_T("[TrigMonFrame::StorePacketList] Bad columns returned; type %d"),type);
    return;
  }
  packetListUpdateTime[type]=wxMax(atol(PQgetvalue(result,0,nowcol)),packetListUpdateTime[type]);
  for(int i=0;i<n;++i){
    packetList[type][i+packetListItems[type]].time=atol(PQgetvalue(result,i,timecol));
   // packetList[type][i+packetListItems[type]].us=(haveus?atol(PQgetvalue(result,i,uscol)):-1);//amir's addition july 20 2008
    packetList[type][i+packetListItems[type]].nbuf=atol(PQgetvalue(result,i,nbufcol));
  }
  packetListItems[type]+=n;
  qsort(packetList[type],packetListItems[type],sizeof(dbRef),dbRefComp);
  m_dbControl->SetList(type,packetList[type],packetListItems[type]);
}


