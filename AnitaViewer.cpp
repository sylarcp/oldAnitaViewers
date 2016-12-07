/*
 * AnitaViewer.cpp
 * 
 * This class just defines global variable defaults and launches application.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "AnitaViewer.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "options_default.h"
#include "surfMapping.h"
#include "AnitaViewer.h"
#include "AviewFrame.h"

/*
 * Application instance implementation
 */
IMPLEMENT_APP( AnitaViewerApp )

/*
 * Constructor for AnitaViewerApp
 */
AnitaViewerApp::AnitaViewerApp(){}

/*
 * Initialisation for AnitaViewerApp
 */
bool AnitaViewerApp::OnInit(){    
  using namespace AviewOpts;
  // Initialize default options
  // Temperatures
  TempSpec = new wxRangeSpec*[N_TEMP];
  TempRange = new double*[N_TEMP];
  TempScaleType = new ScaleType[N_TEMP];
  TempAverage = new unsigned int[N_TEMP];
  TempLabels = new wxString[N_TEMP];
  TempCaptions = new wxString[N_TEMP];
  TempVars = new wxString[N_TEMP];
  TempTable = new wxString[N_TEMP];

  printf ("AnitaViewer: starting AnitaViewer program.\n") ;
  fflush (stdout) ;

  for(int i=0;i<N_TEMP;++i){  // 2 extra is for core temp on CR11
    TempSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    TempRange[i]=new double[2];
    TempScaleType[i]=Linear;
    TempAverage[i]=1;

    if(i<15){
      TempLabels[i].Printf(_T("it%02d"),i+1);
      TempCaptions[i].Printf(_T("Internal Temp %02d"),i+1);
      TempVars[i].Printf(_T("it[%d]"),i+1);
      TempSpec[i][0]=wxRangeSpec(0,20,wxBLUE);
      TempSpec[i][1]=wxRangeSpec(20,45,wxGREEN);
      TempSpec[i][2]=wxRangeSpec(45,55,wxYELLOW);
      TempSpec[i][3]=wxRangeSpec();
      TempRange[i][0]=-10;
      TempRange[i][1]=100;
    }else if(i<30){
      TempLabels[i].Printf(_T("et%02d"),i-14);
      TempCaptions[i].Printf(_T("External Temp %02d"),i-14);
      TempVars[i].Printf(_T("et[%d]"),i-14);
      TempSpec[i][0]=wxRangeSpec(0,20,wxBLUE);
      TempSpec[i][1]=wxRangeSpec(20,45,wxGREEN);
      TempSpec[i][2]=wxRangeSpec(45,55,wxYELLOW);
      TempSpec[i][3]=wxRangeSpec();
      TempRange[i][0]=-10;
      TempRange[i][1]=100;
    }else if(i<38){
      TempLabels[i].Printf(_T("ss%2d"),i-29);
      TempCaptions[i].Printf(_T("Sun Sensor Temp %02d"),i-29);
      TempVars[i].Printf(_T("sst[%d]"),i-29);
      TempSpec[i][0]=wxRangeSpec(0,20,wxBLUE);
      TempSpec[i][1]=wxRangeSpec(20,45,wxGREEN);
      TempSpec[i][2]=wxRangeSpec(45,55,wxYELLOW);
      TempSpec[i][3]=wxRangeSpec();
      TempRange[i][0]=-10;
      TempRange[i][1]=100;
    }else if(i<40){
      TempLabels[i].Printf(_T("ac%1d"),i-37);
      TempCaptions[i].Printf(_T("Accelerometer Temp %02d"),i-37);
      TempVars[i].Printf(_T("acct[%d]"),i-37);
      TempSpec[i][0]=wxRangeSpec(0,20,wxBLUE);
      TempSpec[i][1]=wxRangeSpec(20,45,wxGREEN);
      TempSpec[i][2]=wxRangeSpec(45,55,wxYELLOW);
      TempSpec[i][3]=wxRangeSpec();
      TempRange[i][0]=-10;
      TempRange[i][1]=100;
    }else if(i<42){
      TempLabels[i].Printf(_T("cpu%1d"),i-39);
      TempCaptions[i].Printf(_T("CPU Temp %02d"),i-39);
      TempVars[i].Printf(_T("sbst%1d"),i-39);
      TempSpec[i][0]=wxRangeSpec(0,20,wxBLUE);
      TempSpec[i][1]=wxRangeSpec(20,50,wxGREEN);
      TempSpec[i][2]=wxRangeSpec(50,70,wxYELLOW);
      TempSpec[i][3]=wxRangeSpec();
      TempRange[i][0]=-10;
      TempRange[i][1]=100;
    }else if(i<44){
      TempLabels[i].Printf(_T("cr%1d"),i-41);
      TempCaptions[i].Printf(_T("CPU Temp %02d"),i-39);
      TempVars[i].Printf(_T("core%1d"),i-41);
      TempSpec[i][0]=wxRangeSpec(0,30,wxBLUE);
      TempSpec[i][1]=wxRangeSpec(30,60,wxGREEN);
      TempSpec[i][2]=wxRangeSpec(60,80,wxYELLOW);
      TempSpec[i][3]=wxRangeSpec();
      TempRange[i][0]=-10;
      TempRange[i][1]=100;
    }
     else{
      TempLabels[i].Printf(_T("disk%1d"),i-44);
      TempCaptions[i].Printf(_T("Disk Temp %02d"),i-44);
      TempVars[i].Printf(_T("disk%1d"),i-44);
      TempSpec[i][0]=wxRangeSpec(0,20,wxBLUE);
      TempSpec[i][1]=wxRangeSpec(20,50,wxGREEN);
      TempSpec[i][2]=wxRangeSpec(50,70,wxYELLOW);
      TempSpec[i][3]=wxRangeSpec();
      TempRange[i][0]=-10;
      TempRange[i][1]=100;
    }
    TempTable[i]=_T("hk");
  }

  // Disk Space
  DiskSpec = new wxRangeSpec*[N_DISK];
  DiskRange = new double*[N_DISK];
  DiskScaleType = new ScaleType[N_DISK];
  DiskAverage = new unsigned int[N_DISK];
  DiskLabels = new wxString[N_DISK];
  DiskRefLabels = new wxString[N_DISK];
  DiskCaptions = new wxString[N_DISK];
  DiskVars = new wxString[N_DISK];
  DiskTable = new wxString[N_DISK];
  for(int i=0;i<N_DISK;++i){
    DiskSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    DiskRange[i]=new double[2];
    DiskScaleType[i]=Linear;
    DiskAverage[i]=1;

    DiskVars[i].Printf(_T("disk[%d]"),i+1);
    DiskLabels[i].Printf(_T("disk%d"),i+1);
    if(i==0){
      DiskRefLabels[i].Printf(_T("/tmp"));
      DiskCaptions[i].Printf(_T("/tmp Disk Space"));
      DiskSpec[i][0]=wxRangeSpec(400,5000,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(100,400,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=4000;
    }else if(i==1){
      DiskRefLabels[i]=_T("var");
      DiskCaptions[i]=_T("Var Disk Space");
      DiskSpec[i][0]=wxRangeSpec(150,2000,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(40,150,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=1000;
    }else if(i==2){
      DiskRefLabels[i]=_T("home");
      DiskCaptions[i]=_T("Home Disk Space");
      DiskSpec[i][0]=wxRangeSpec(300,3500,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(80,300,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=2500;
    }else if(i==3){
      DiskRefLabels[i]=_T("/");
      DiskCaptions[i]=_T("/ Disk Space");
      DiskSpec[i][0]=wxRangeSpec(300,4000,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(50,300,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=3000;
     
    }else if(i==4){
      DiskRefLabels[i]=_T("He1");
      DiskCaptions[i]=_T("/mnt/helium1 Disk Space");
      DiskSpec[i][0]=wxRangeSpec(1000000,7000000,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(6000,100000,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=6000000;
    }else if(i==5){
      DiskRefLabels[i]=_T("He2");
      DiskCaptions[i]=_T("/mnt/helium2 Disk Space");
      DiskSpec[i][0]=wxRangeSpec(1000000,7000000,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(6000,1000000,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=6000000;
    }else if(i==6){
      DiskRefLabels[i]=_T("int");
      DiskCaptions[i]=_T("USB Int Disk Space");
      DiskSpec[i][0]=wxRangeSpec(300,4000,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(80,300,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=3000;
    }else{
      DiskRefLabels[i]=_T("NTU");
      DiskCaptions[i]=_T("NTU Disk Space");
      DiskSpec[i][0]=wxRangeSpec(25000,350000,wxGREEN);
      DiskSpec[i][1]=wxRangeSpec(2500,25000,wxYELLOW);
      DiskSpec[i][2]=wxRangeSpec();
      DiskRange[i][0]=0;
      DiskRange[i][1]=250000;
    }
    DiskTable[i]=_T("mon");

  }

  // Queue Space
  QueueSpec = new wxRangeSpec*[N_QUEUE];
  QueueRange = new double*[N_QUEUE];
  QueueScaleType = new ScaleType[N_QUEUE];
  QueueAverage = new unsigned int[N_QUEUE];
  QueueLabels = new wxString[N_QUEUE];
  QueueRefLabels = new wxString[N_QUEUE];
  QueueCaptions = new wxString[N_QUEUE];
  QueueVars = new wxString[N_QUEUE];
  QueueTable = new wxString[N_QUEUE];
  for(int i=0;i<N_QUEUE;++i){
    QueueSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    QueueRange[i]=new double[2];
    QueueScaleType[i]=Linear;
    QueueAverage[i]=1;

    if(i<10){
      QueueRefLabels[i].Printf(_T("p%d"),i);
      QueueLabels[i].Printf(_T("priority%d"),i);
      QueueCaptions[i].Printf(_T("Priority %d queue"),i);
      QueueVars[i].Printf(_T("linkev[%d]"),i+1);
    }else switch(i){
    case 10:
      QueueRefLabels[i]=_T("cmdL");
      QueueLabels[i]=_T("command LOS");
      QueueCaptions[i]=_T("Command LOS echo queue");
      QueueVars[i]=_T("linkcmdlos");
      break;
    case 11:
      QueueRefLabels[i]=_T("cmdS");
      QueueLabels[i]=_T("command SIP");
      QueueCaptions[i]=_T("Command Sip echo queue");
      QueueVars[i]=_T("linkcmdsip");
      break;
    case 12:
      QueueRefLabels[i]=_T("hd");
      QueueLabels[i]=_T("header");
      QueueCaptions[i]=_T("Header queue");
      QueueVars[i]=_T("linkhd");
      break;
    case 13:
      QueueRefLabels[i]=_T("gps");
      QueueLabels[i]=_T("GPS");
      QueueCaptions[i]=_T("GPS queue");
      QueueVars[i]=_T("linkgps");
      break;
    case 14:
      QueueRefLabels[i]=_T("hk");
      QueueLabels[i]=_T("housekeeping");
      QueueCaptions[i]=_T("Housekeeping queue");
      QueueVars[i]=_T("linkhk");
      break;
    case 15:
      QueueRefLabels[i]=_T("mon");
      QueueLabels[i]=_T("monitor");
      QueueCaptions[i]=_T("CPU monitor queue");
      QueueVars[i]=_T("linkmon");
      break;
    case 16:
      QueueRefLabels[i]=_T("surf");
      QueueLabels[i]=_T("SURF hk");
      QueueCaptions[i]=_T("SURF housekeeping queue");
      QueueVars[i]=_T("linksurf");
      break;
    case 17:
      QueueRefLabels[i]=_T("turf");
      QueueLabels[i]=_T("TURF hk");
      QueueCaptions[i]=_T("TURF housekeeping queue");
      QueueVars[i]=_T("linkturf");
      break;
    case 18:
      QueueRefLabels[i]=_T("ped");
      QueueLabels[i]=_T("pedestal");
      QueueCaptions[i]=_T("Pedestal queue");
      QueueVars[i]=_T("linkped");
      break;
    }
    QueueTable[i]=_T("mon");

    QueueSpec[i][0]=wxRangeSpec(0,15,wxGREEN);
    QueueSpec[i][1]=wxRangeSpec(15,50,wxYELLOW);
    QueueSpec[i][2]=wxRangeSpec();
    QueueRange[i][0]=0;
    QueueRange[i][1]=100;
  }

  // Currents
  CurrentSpec = new wxRangeSpec*[N_CURRENT];
  CurrentLabels = new wxString[N_CURRENT];
  CurrentCaptions = new wxString[N_CURRENT];
  CurrentVars = new wxString[N_CURRENT];
  CurrentTable = new wxString[N_CURRENT];
  for(int i=0;i<N_CURRENT;++i){
    CurrentSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    CurrentTable[i]=_T("hk");
  }

  CurrentLabels[0]=_T("+3.3V");
  CurrentVars[0]=_T("p3_3i");
  CurrentSpec[0][0]=wxRangeSpec(8.3,9.4, wxColour(0x09,0x09,0xd8));
  CurrentLabels[1]=_T("+5V");
  CurrentVars[1]=_T("p5i");
  CurrentSpec[1][0]=wxRangeSpec(19, 22.5, wxColour(0x09,0x09,0xd8));
  CurrentLabels[2]=_T("+12V");
  CurrentVars[2]=_T("p12i");
  CurrentSpec[2][0]=wxRangeSpec(1.5,2.5, wxColour(0x09,0x09,0xd8));
  CurrentLabels[3]=_T("+24V");
  CurrentVars[3]=_T("p24i");    
  CurrentSpec[3][0]=wxRangeSpec(13.4, 19, wxColour(0x09,0x09,0xd8));
  CurrentLabels[4]=_T("+5V_M I");
  CurrentVars[4]=_T("+5SB");
  CurrentSpec[4][0]=wxRangeSpec(4.3, 5.3, wxColour(0x09,0x09,0xd8));
  //CurrentLabels[5]=_T("+5_IP");
  //CurrentVars[6]=_T("5vip");
  //CurrentSpec[6][0]=wxRangeSpec(0.005, 0.03, wxColour(0x09,0x09,0xd8));
  CurrentLabels[5]=_T("-5V V");
  CurrentVars[5]=_T("n12i");//this could be voltage not current
  CurrentSpec[5][0]=wxRangeSpec(4.9, 5.3, wxColour(0x09,0x09,0xd8));
  CurrentLabels[6]=_T("PV");
  CurrentVars[6]=_T("ppvi");
  CurrentSpec[6][0]=wxRangeSpec(7, 11, wxColour(0x09,0x09,0xd8));
  //CurrentLabels[7]=_T("+12V_IP I");
  //CurrentVars[7]=_T("iprf2i");
  //CurrentSpec[7][0]=wxRangeSpec(6, 12, wxColour(0x09,0x09,0xd8));
  CurrentLabels[7]=_T("Battery");
  CurrentVars[7]=_T("bati");
  CurrentSpec[7][0]=wxRangeSpec(-2, 2, wxColour(0x09,0x09,0xd8));
  for(int i=0;i<N_CURRENT;++i){
    CurrentSpec[i][2]=wxRangeSpec();
    CurrentCaptions[i] << CurrentLabels[i] << _T(" current");
  }

  // Voltages
  VoltageSpec = new wxRangeSpec*[N_VOLTAGE];
  VoltageLabels = new wxString[N_VOLTAGE];
  VoltageCaptions = new wxString[N_VOLTAGE];
  VoltageVars = new wxString[N_VOLTAGE];
  VoltageTable = new wxString[N_VOLTAGE];
  for(int i=0;i<N_VOLTAGE;++i){
    VoltageSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    VoltageTable[i]=_T("hk");
  }

  VoltageLabels[0]=_T("+1.5V");
  VoltageVars[0]=_T("p1_5v");
  VoltageSpec[0][0]=wxRangeSpec(1.4, 1.6, wxColour(0x09,0x09,0xd8));
  VoltageLabels[1]=_T("+3.3V");
  VoltageVars[1]=_T("p3_3v");
  VoltageSpec[1][0]=wxRangeSpec(3.2, 3.7, wxColour(0x09,0x09,0xd8));
  VoltageLabels[2]=_T("+5V");
  VoltageVars[2]=_T("p5v");
  VoltageSpec[2][0]=wxRangeSpec(5.1, 5.6, wxColour(0x09,0x09,0xd8));
  VoltageLabels[3]=_T("+12V");
  VoltageVars[3]=_T("p12v");
  VoltageSpec[3][0]=wxRangeSpec(11.8, 12.6, wxColour(0x09,0x09,0xd8));
  VoltageLabels[4]=_T("+24V");
  VoltageVars[4]=_T("p24v");  
  VoltageSpec[4][0]=wxRangeSpec(23.6, 30.5, wxColour(0x09,0x09,0xd8));
  VoltageLabels[5]=_T("+5V_M V");
  VoltageVars[5]=_T("p5sbv");
  VoltageSpec[5][0]=wxRangeSpec(5, 5.5, wxColour(0x09,0x09,0xd8));
  //VoltageLabels[6]=_T("-5V");
  //VoltageVars[6]=_T("n5v");   
  //VoltageSpec[6][0]=wxRangeSpec(-5.2, -4.8, wxColour(0x09,0xd8,0x09));
  VoltageLabels[6]=_T("-12V");
  VoltageVars[6]=_T("n12v");
  VoltageSpec[6][0]=wxRangeSpec(-12.4, -11.8, wxColour(0x09,0x09,0xd8));
  VoltageLabels[7]=_T("PV");
  VoltageVars[7]=_T("ppvv");
  VoltageSpec[7][0]=wxRangeSpec(47, 61, wxColour(0x09,0x09,0xd8));
  VoltageLabels[8]=_T("+12V_IP V");
  VoltageVars[8]=_T("iprf1v");
  VoltageSpec[8][0]=wxRangeSpec(11.8, 12.7, wxColour(0x09,0x09,0xd8));
  VoltageLabels[9]=_T("+5.6V_LT V");       
  VoltageVars[9]=_T("iprf2v");
  VoltageSpec[9][0]=wxRangeSpec(5.4, 5.7, wxColour(0x09,0x09,0xd8));
  for(int i=0;i<N_VOLTAGE;++i){
    VoltageSpec[i][2]=wxRangeSpec();
    VoltageCaptions[i] << VoltageLabels[i] << _T(" potential");
  }

  // Powers
  PowerSpec = new wxRangeSpec*[N_POWER];
  PowerLabels = new wxString[N_POWER];
  PowerCaptions = new wxString[N_POWER];
  PowerVars = new wxString[N_POWER];
  PowerTable = new wxString[N_POWER];
  for(int i=0;i<N_POWER;++i){
    PowerSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    PowerTable[i]=_T("hk");
  }
  PowerLabels[0]=_T("PV");
  PowerVars[0]=_T("ppvp");
  PowerSpec[0][0]=wxRangeSpec(330, 670, wxColour(0x09,0x09,0xd8));
  // RF
  RFSpecRMS = new wxRangeSpec*[N_RFANT];
  RFSpecAbsolute = new wxRangeSpec*[N_RFANT];
  RFLabels = new wxString[N_RFANT];
  RFRefLabels = new wxString[N_RFANT];
  for(int i=0;i<N_RFANT;++i){
    RFSpecRMS[i]=new wxRangeSpec[MAX_SPECS+1];
    RFSpecAbsolute[i]=new wxRangeSpec[MAX_SPECS+1];
    int iAnt=i/2;

    if(i<(8)*(N_CHAN-1) || N_SURF>9){ // QRH channels (incl. nadir ones)
      if(i%2){ // Vertical channel
	RFRefLabels[i].Printf(_T("%d:%d"),surfMap[iAnt].v_surf,surfMap[iAnt].v_chan);
	RFLabels[i] << (i/2+1) << 'V';
      }else{    // Horizontal channel
	RFRefLabels[i].Printf(_T("%d:%d"),surfMap[iAnt].h_surf,surfMap[iAnt].h_chan);
	RFLabels[i] << (i/2+1) << 'H';
      }
    }else if(i<(9)*(N_CHAN-1)){ // Veto channels
      RFRefLabels[i].Printf(_T("veto%d"),(i%(N_CHAN-1))+1);
      RFLabels[i]=RFRefLabels[i];
    }else{ // Nadir channels (chenged to define in QRH above)
      RFRefLabels[i].Printf(_T("nad%d"),(i%(N_CHAN-1))+1);
      RFLabels[i].Printf(_T("nadir%d"),(i%(N_CHAN-1))+1);
    }

//     RFSpecRMS[i][0]=wxRangeSpec(0.0, 2.0,wxColour(0xc0,0xc0,0xc0));
//     RFSpecRMS[i][1]=wxRangeSpec(2.0, 3.0,wxColour(0xcd,0xcd,0xcd));
//     RFSpecRMS[i][2]=wxRangeSpec(3.0, 4.0,wxColour(0xb8,0xb8,0xb8));
//     RFSpecRMS[i][3]=wxRangeSpec(4.0, 5.5,wxColour(0xa9,0xa9,0xa9));
//     RFSpecRMS[i][4]=wxRangeSpec(5.5, 7.5,wxColour(0x7c,0x7d,0x8b));
//     RFSpecRMS[i][5]=wxRangeSpec(7.5, 9.5,wxColour(0x50,0x53,0x83));
//     RFSpecRMS[i][6]=wxRangeSpec(9.5, 13,wxColour(0x32,0x37,0x8a));
//     RFSpecRMS[i][7]=wxRangeSpec(13.,18.0,wxColour(0x18,0x20,0x9a));
//     RFSpecRMS[i][8]=wxRangeSpec(18.,25.0,wxColour(0x0c,0x15,0xb5));
//     RFSpecRMS[i][9]=wxRangeSpec(25.,40.0,wxColour(0x00,0x00,0xff));

    RFSpecRMS[i][0]=wxRangeSpec(0.0, 2.0,wxColour(0xc0,0xc0,0xc0));
    RFSpecRMS[i][1]=wxRangeSpec(2.0, 3.0,wxColour(0x9f,0xaf,0xb8));
    RFSpecRMS[i][2]=wxRangeSpec(3.0, 4.0,wxColour(0x89,0xac,0xbd));
    RFSpecRMS[i][3]=wxRangeSpec(4.0, 5.5,wxColour(0x74,0xaa,0xc5));
    RFSpecRMS[i][4]=wxRangeSpec(5.5, 7.5,wxColour(0x60,0xa9,0xce));
    RFSpecRMS[i][5]=wxRangeSpec(7.5, 9.5,wxColour(0x49,0xa6,0xd4));
    RFSpecRMS[i][6]=wxRangeSpec(9.5, 13.,wxColour(0x33,0xa2,0xda));
    RFSpecRMS[i][7]=wxRangeSpec(13.,18.0,wxColour(0x18,0xa1,0xe5));
    RFSpecRMS[i][8]=wxRangeSpec(18.,25.0,wxColour(0x00,0xa9,0xfd));
    RFSpecRMS[i][9]=wxRangeSpec(25.,40.0,wxColour(0x00,0x00,0xff));
    RFSpecRMS[i][10]=wxRangeSpec();

//     RFSpecAbsolute[i][0]=wxRangeSpec(0.0,30.0,wxColour(0xc0,0xc0,0xc0));
//     RFSpecAbsolute[i][1]=wxRangeSpec(30.,70.0,wxColour(0xcd,0xcd,0xcd));
//     RFSpecAbsolute[i][2]=wxRangeSpec(70.,120.,wxColour(0xb8,0xb8,0xb8));
//     RFSpecAbsolute[i][3]=wxRangeSpec(120.,180.,wxColour(0xa9,0xa9,0xa8));
//     RFSpecAbsolute[i][4]=wxRangeSpec(180.,250.,wxColour(0x7c,0x7d,0x8b));
//     RFSpecAbsolute[i][5]=wxRangeSpec(250.,330.,wxColour(0x50,0x53,0x83));
//     RFSpecAbsolute[i][6]=wxRangeSpec(330.,430.,wxColour(0x32,0x37,0x8a));
//     RFSpecAbsolute[i][7]=wxRangeSpec(420.,540.,wxColour(0x18,0x20,0x9a));
//     RFSpecAbsolute[i][8]=wxRangeSpec(540.,670.,wxColour(0x0c,0x15,0xb5));
//     RFSpecAbsolute[i][9]=wxRangeSpec(670.,810.,wxColour(0x00,0x00,0xff));
    RFSpecAbsolute[i][0]=wxRangeSpec(0.0, 30.0,wxColour(0xc0,0xc0,0xc0));
    RFSpecAbsolute[i][1]=wxRangeSpec(30., 70.0,wxColour(0x80,0x80,0x80));
    RFSpecAbsolute[i][2]=wxRangeSpec(70., 120.,wxColour(0x93,0x72,0x72));
    RFSpecAbsolute[i][3]=wxRangeSpec(120.,180.,wxColour(0xa4,0x5f,0x60));
    RFSpecAbsolute[i][4]=wxRangeSpec(180.,250.,wxColour(0xb7,0x4f,0x51));
    RFSpecAbsolute[i][5]=wxRangeSpec(250.,330.,wxColour(0xca,0x40,0x42));
    RFSpecAbsolute[i][6]=wxRangeSpec(330.,430.,wxColour(0xda,0x30,0x31));
    RFSpecAbsolute[i][7]=wxRangeSpec(420.,540.,wxColour(0xe7,0x1e,0x20));
    RFSpecAbsolute[i][8]=wxRangeSpec(540.,670.,wxColour(0xf2,0x12,0x14));
    RFSpecAbsolute[i][9]=wxRangeSpec(670.,810.,wxColour(0xf6,0x15,0x07));
    RFSpecAbsolute[i][10]=wxRangeSpec();
  }
  
  // All auxilary values
  AuxSpec = new wxRangeSpec*[N_AUX];
  AuxLabels = new wxString[N_AUX];
  AuxCaptions = new wxString[N_AUX];
  AuxVars = new wxString[N_AUX];
  AuxTable = new wxString[N_AUX];
  for(int i=0;i<N_AUX;++i){
    AuxSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    AuxTable[i]=_T("hk");
  }

  AuxLabels[0]=_T("Press [torr]");
  AuxCaptions[0]=_T("Low pressure");
  AuxVars[0]=_T("pressl");
  AuxSpec[0][0]=wxRangeSpec(2, 3.5, wxColour(0x09,0xd8,0x09));
  AuxSpec[0][1]=wxRangeSpec(1, 4, wxYELLOW);
  AuxLabels[1]=_T("Press [PSI]");
  AuxCaptions[1]=_T("High pressure");
  AuxVars[1]=_T("pressh");
  AuxSpec[1][0]=wxRangeSpec(0, 1, wxColour(0x09,0xd8,0x09));
  AuxSpec[1][1]=wxRangeSpec(1, 16, wxYELLOW);
  for(int i=0;i<N_AUX;++i)
    AuxSpec[i][2]=wxRangeSpec();

  EventRateSpec=new wxRangeSpec[MAX_SPECS+1];
/*
  EventRateSpec[0]=wxRangeSpec(6, 14, wxGREEN);
  EventRateSpec[1]=wxRangeSpec(3, 20, wxYELLOW);
*/
  EventRateSpec[0]=wxRangeSpec(20, 50, wxGREEN);
  EventRateSpec[1]=wxRangeSpec(10, 65, wxYELLOW);
  EventRateSpec[2]=wxRangeSpec();
  EventRateRange=new double[2];
  EventRateRange[0]=0;
  EventRateRange[1]=100;
  EventRateScaleType=Linear;
  EventRateAverage=1;
  EventRateStep=30;

  AviewFrame* aviewWindow = new AviewFrame( (wxWindow*) NULL, this->argc, (char **)this->argv );
  aviewWindow->Show(true);
  return true;
}

/*
 * Cleanup for AnitaViewerApp
 */
int AnitaViewerApp::OnExit(){    
    return wxApp::OnExit();
}
