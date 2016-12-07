/*
 * SlowInfo.cpp
 * 
 * Definitions for display of information in slow rate packets.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SlowInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SlowInfo.h"

// Constructor
SlowInfo::SlowInfo( wxWindow* parent ):wxPanel(parent){

  // Create and arrange all children
  wxBoxSizer *topSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  topSizer->Add(labelSizer,0,wxEXPAND);
  topSizer->Add(infoSizer,1,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Current UTC: ")), 0);
  m_CurrentTimeDisplay = new wxStaticText(this,wxID_ANY,_T(""),wxDefaultPosition,
                                          wxSize(150,-1),wxST_NO_AUTORESIZE);
  infoSizer->Add(m_CurrentTimeDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("UTC time: ")), 0);
  m_TimeDisplay = new wxStaticText(this,wxID_ANY,_T("YYYY-MM-DD HH:MM:SS"));
  infoSizer->Add(m_TimeDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Last Event #: ")), 0);
  m_EventNumberDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
						 _T("Event Number"),
						 _T("min"),_T("evnum"),
						 _T("evnum"),_T("slow"));
  infoSizer->Add(m_EventNumberDisplay,0,wxEXPAND);


  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Location: ")), 0);
  wxString xUnitsLoc[3]={_T("min"),_T("min"),_T("min")};
  wxString yUnitsLoc[3]={_T("Lat [deg]"),_T("Lon [deg]"),_T("Alt [m]")};
  wxString dbVarLoc[3]={_T("latitude"),_T("longitude"),_T("altitude")};
  m_LocationDisplay = new wxHistoryStaticText(this,wxID_ANY,3,
 					      _T("%+7.3f"),_T(":"),
 					      wxDefaultPosition,
 					      wxDefaultSize,0,
 					      _T("Location"),
 					      xUnitsLoc,yUnitsLoc,
 					      dbVarLoc,_T("slow"));
  infoSizer->Add(m_LocationDisplay,0,wxEXPAND);
  
  m_CurrentTime = new wxDateTime();
  UpdateCurrentTime();

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);

}

SlowInfo::~SlowInfo(){}

void SlowInfo::UpdateCurrentTime(){
  m_CurrentTime->SetToCurrent();
  m_CurrentTime->MakeUTC();
  m_CurrentTimeDisplay->SetLabel(m_CurrentTime->Format(_T("%Y-%m-%d %H:%M:%S")));
}

void SlowInfo::SetTime(unsigned long time){
  wxDateTime t((time_t)time);
  t.MakeUTC();
  m_TimeDisplay->SetLabel(t.Format(_T("%Y-%m-%d %H:%M:%S")));
}

void SlowInfo::SetEventNumber(unsigned long evnum){
  m_EventNumberDisplay->SetLabel((double)evnum);
}

void SlowInfo::SetLocation(float lat,float lon,float alt){
  double tmp[3]={lat,lon,alt};
  m_LocationDisplay->SetLabel(tmp);
}

