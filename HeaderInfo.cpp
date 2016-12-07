/*
 * HeaderInfo.cpp
 *
 * Definitions for display of most basic AnitaViewer data, like event time, 
 * and number, GPS location, current time, etc.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "HeaderInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "HeaderInfo.h"

// Constructor
HeaderInfo::HeaderInfo( wxWindow* parent ):wxPanel(parent){

  //this->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  // Create and arrange all children
  wxBoxSizer *topSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  topSizer->Add(labelSizer,0,wxEXPAND);
  topSizer->Add(infoSizer,1,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Current UTC: ")), 0);
  m_CurrentTimeDisplay = new wxStaticText(this,wxID_ANY,_T(""),wxDefaultPosition,
					  wxSize(145,-1),wxST_NO_AUTORESIZE);
  infoSizer->Add(m_CurrentTimeDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Event UTC: ")), 0);
  m_EventTimeDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_EventTimeDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Event us: ")), 0);
  m_EventUSTimeDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_EventUSTimeDisplay,0,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Event ns: ")), 0);
  m_EventNSTimeDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_EventNSTimeDisplay,0,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Event #: ")), 0);
  m_EventNumberDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
						 _T("Event Number"),
						 _T("min"),_T("evnum"),
						 _T("evnum"),_T("hd"));
  infoSizer->Add(m_EventNumberDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Run #: ")), 0);
  m_RunNumberDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
					       _T("Run Number"),
					       _T("min"),_T("runnum"),
					       _T("runnum"),_T("other"));
  infoSizer->Add(m_RunNumberDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Location A: ")), 0);
  wxString xUnitsLoc[3]={_T("min"),_T("min"),_T("min")};
  wxString yUnitsLoc[3]={_T("Lat [deg]"),_T("Lon [deg]"),_T("Alt [km or m]")};
  wxString dbVarLoc[3]={_T("latitude"),_T("longitude"),_T("altitude")};
  m_LocationDisplay = new wxHistoryStaticText(this,wxID_ANY,3,
					      _T("%+8.4f"),_T(":"),
					      wxDefaultPosition,
					      wxDefaultSize,0,
					      _T("Location A"),
					      xUnitsLoc,yUnitsLoc,
					      dbVarLoc,_T("adu5_pat"));
  infoSizer->Add(m_LocationDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Orientation A: ")), 0);
  wxString xUnitsOrient[3]={_T("min"),_T("min"),_T("min")};
  wxString yUnitsOrient[3]={_T("Heading [deg]"),_T("Pitch [deg]"),_T("Roll [deg]")};
  wxString dbVarOrient[3]={_T("heading"),_T("pitch"),_T("roll")};
  m_OrientationDisplay = new wxHistoryStaticText(this,wxID_ANY,3,_T("%+6.2f"),_T(":"),
						 wxDefaultPosition,
						 wxDefaultSize,0,
						 _T("Orientation A"),
						 xUnitsOrient,yUnitsOrient,
						 dbVarOrient,_T("adu5_pat"));
  infoSizer->Add(m_OrientationDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Speed A[kt@deg]: ")), 0);
  wxString xUnitsSpeed[2]={_T("min"),_T("min")};
  wxString yUnitsSpeed[2]={_T("Speed [knot]"),_T("Course [deg]")};
  wxString dbVarSpeed[2]={_T("vkt"),_T("course")};
  m_SpeedDisplay = new wxHistoryStaticText(this,wxID_ANY,2,_T("%+5.1f"),_T("@"),
					   wxDefaultPosition,
					   wxDefaultSize,0,
					   _T("Speed/Course A"),
					   xUnitsSpeed,yUnitsSpeed,
					   dbVarSpeed,_T("adu5_vtg"));
					   //dbVarSpeed,_T("g12_pos"));
  infoSizer->Add(m_SpeedDisplay,0,wxEXPAND);
////peng////
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Location B: ")), 0);
  m_LocationDisplayB = new wxHistoryStaticText(this,wxID_ANY,3,
					      _T("%+8.4f"),_T(":"),
					      wxDefaultPosition,
					      wxDefaultSize,0,
					      _T("Location B"),
					      xUnitsLoc,yUnitsLoc,
					      dbVarLoc,_T("adu5_pat"));
  infoSizer->Add(m_LocationDisplayB,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Orientation B: ")), 0);
  m_OrientationDisplayB = new wxHistoryStaticText(this,wxID_ANY,3,_T("%+6.2f"),_T(":"),
						 wxDefaultPosition,
						 wxDefaultSize,0,
						 _T("Orientation B"),
						 xUnitsOrient,yUnitsOrient,
						 dbVarOrient,_T("adu5_pat"));
  infoSizer->Add(m_OrientationDisplayB,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Speed B[kt@deg]: ")), 0);
  m_SpeedDisplayB = new wxHistoryStaticText(this,wxID_ANY,2,_T("%+5.1f"),_T("@"),
					   wxDefaultPosition,
					   wxDefaultSize,0,
					   _T("Speed/Course B"),
					   xUnitsSpeed,yUnitsSpeed,
					   dbVarSpeed,_T("adu5_vtg"));
					   //dbVarSpeed,_T("g12_pos"));
  infoSizer->Add(m_SpeedDisplayB,0,wxEXPAND);
////peng////

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Last command : ")), 0);
  m_CommandDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_CommandDisplay,0,wxEXPAND);

  //labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Antennas off : ")), 0);
  //m_AntennaMaskDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  //infoSizer->Add(m_AntennaMaskDisplay,0,wxEXPAND);

  m_CurrentTime = new wxDateTime();
  UpdateCurrentTime();

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);

}

HeaderInfo::~HeaderInfo(){}

void HeaderInfo::UpdateCurrentTime(){
  m_CurrentTime->SetToCurrent();
  m_CurrentTime->MakeUTC();
  m_CurrentTimeDisplay->SetLabel(m_CurrentTime->Format(_T("%Y-%m-%d %H:%M:%S")));
}

void HeaderInfo::SetEventTime(unsigned long time){
  wxString tmp=wxDateTime((time_t)time).ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_EventTimeDisplay->SetLabel(tmp);
}

void HeaderInfo::SetEventUSTime(unsigned long us){
  wxString tmp;
  tmp << us;
  m_EventUSTimeDisplay->SetLabel(tmp);
}

void HeaderInfo::SetEventNSTime(unsigned long ns){
  wxString tmp;
  double nano;
  if ((nano=ns/1.e7) > 1.0) tmp.Printf(_T("n/a")) ;
  else 
    tmp.Printf(_T("%3d,%03d,%03d"), (unsigned int)(nano*1e3),
	       (unsigned int)(nano*1e6)%1000,(unsigned int)(nano*1e9)%1000);
  m_EventNSTimeDisplay->SetLabel(tmp);
}

void HeaderInfo::SetEventNumber(unsigned long evnum){
  m_EventNumberDisplay->SetLabel((double)evnum);
}

void HeaderInfo::SetRunNumber(unsigned long runnum){
  m_RunNumberDisplay->SetLabel((double)runnum);
}

void HeaderInfo::SetLocation(float lat,float lon,float alt){
  double tmp[3]={lat,lon,alt};
  m_LocationDisplay->SetLabel(tmp);
}

void HeaderInfo::SetOrientation(float head,float pitch,float roll){
  double tmp[3]={head,pitch,roll};
  m_OrientationDisplay->SetLabel(tmp);
}

void HeaderInfo::SetSpeed(float speed,float course){
  double tmp[2]={speed,course};
  m_SpeedDisplay->SetLabel(tmp);
}
///////peng///////////
void HeaderInfo::SetLocationB(float lat,float lon,float alt){
  double tmp[3]={lat,lon,alt};
  m_LocationDisplayB->SetLabel(tmp);
}

void HeaderInfo::SetOrientationB(float head,float pitch,float roll){
  double tmp[3]={head,pitch,roll};
  m_OrientationDisplayB->SetLabel(tmp);
}

void HeaderInfo::SetSpeedB(float speed,float course){
  double tmp[2]={speed,course};
  m_SpeedDisplayB->SetLabel(tmp);
}
///////peng///////////
void HeaderInfo::SetCommand(unsigned char cmd[],short n){
  wxString tmp;
  tmp.Printf(_T("%d"),cmd[0]);
  for(int i=1;i<n;++i) tmp.Printf(_T("%s 0x%02x"),tmp.c_str(),cmd[i]);
  m_CommandDisplay->SetLabel(tmp);
}

void HeaderInfo::SetCommandGood(bool good){
  if(!good)
    m_CommandDisplay->SetForegroundColour(*wxRED);
  else
    m_CommandDisplay->SetForegroundColour(*wxBLACK);
}

void HeaderInfo::SetAttFlag(bool good){
  if(good){
    m_LocationDisplay->SetForegroundColour(*wxRED);
    m_OrientationDisplay->SetForegroundColour(*wxRED);
    //m_SpeedDisplay->SetForegroundColour(*wxBLACK);//  adu5_vtg does not contain quality flag.
  }else{
    m_LocationDisplay->SetForegroundColour(*wxBLACK);
    m_OrientationDisplay->SetForegroundColour(*wxBLACK);
    //m_SpeedDisplay->SetForegroundColour(*wxRED);
  }
}
void HeaderInfo::SetAttFlagB(bool good){
  if(good){
    m_LocationDisplayB->SetForegroundColour(*wxRED);
    m_OrientationDisplayB->SetForegroundColour(*wxRED);
    //m_SpeedDisplayB->SetForegroundColour(*wxBLACK);
  }else{
    m_LocationDisplayB->SetForegroundColour(*wxBLACK);
    m_OrientationDisplayB->SetForegroundColour(*wxBLACK);
    //m_SpeedDisplayB->SetForegroundColour(*wxRED);
  }
}
   wxColour *wxDARK_GREY=new wxColour(84,84,84);
void HeaderInfo::SetPatColor(bool good){
  if(good){
    m_LocationDisplay->SetForegroundColour(*wxBLACK);
    m_OrientationDisplay->SetForegroundColour(*wxBLACK);
   // m_LocationDisplay->Refresh();
  }else{
    m_LocationDisplay->SetForegroundColour(*wxDARK_GREY);
    m_OrientationDisplay->SetForegroundColour(*wxDARK_GREY);
  }
}
void HeaderInfo::SetPatColorB(bool good){
  if(good){
    m_LocationDisplayB->SetForegroundColour(*wxBLACK);
    m_OrientationDisplayB->SetForegroundColour(*wxBLACK);
  }else{
    m_LocationDisplayB->SetForegroundColour(*wxDARK_GREY);
    m_OrientationDisplayB->SetForegroundColour(*wxDARK_GREY);
  }
}
void HeaderInfo::SetVtgColor(bool good){
  if(good){
    m_SpeedDisplay->SetForegroundColour(*wxBLACK);
  }else{
    m_SpeedDisplay->SetForegroundColour(*wxDARK_GREY);
  }
}
void HeaderInfo::SetVtgColorB(bool good){
  if(good){
    m_SpeedDisplayB->SetForegroundColour(*wxBLACK);
  }else{
    m_SpeedDisplayB->SetForegroundColour(*wxDARK_GREY);
  }
}
/*   //peng removed.  cause Anttena off is disabled.
void HeaderInfo::SetAntennaMask(unsigned long mask){
  wxString tmp;



  for(int ant=1;ant<=32;ant++) {	    
      int bit=(ant%4);
      if(bit) bit=4-bit;
      int nibble=7-(ant-1)/4;	    
      int bitShift=bit+4*nibble;
      int test=(1<<bitShift);
      if(mask&test) {
	  tmp << (ant) << ','; 
      }
  }
  if(tmp.Length())
      tmp.RemoveLast();
  else
    tmp=_T("None");
  m_AntennaMaskDisplay->SetLabel(tmp);
}
*/
