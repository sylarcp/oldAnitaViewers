/*
 * TURFInfo.cpp
 *
 * Definitions for display of TURF rate information.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TURFInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "TURFInfo.h"

// Constructor
TURFInfo::TURFInfo( wxWindow* parent ):wxPanel(parent){

  //this->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

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

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Masked Phi: ")), 0);
  m_TimeUSDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_TimeUSDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Dead time")), 0);
  m_TrigTimeDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.6f"),_T("Time count")
					      _T("0:1e8:2e8:3e8:4e8:5e8:6e8:7e8:9e8:1e9:"),
					      _T("Time count"),_T("trigtime"),_T("hd"));
  infoSizer->Add(m_TrigTimeDisplay,0,wxEXPAND);
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Trigger #:Disabled ")), 0);
  m_TriggerNumberDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
						   _T("Trigger Number"),_T("min"),
						   _T("TrigNum"),_T("trignum"),_T("hd"));
  infoSizer->Add(m_TriggerNumberDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Type:Disabled ")), 0);
  m_TypeDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
					  _T("Trig Type"),_T("0:1:2:4:8:16:32:"),_T("trig type"),
					  _T("type"),_T("hd"));
  infoSizer->Add(m_TypeDisplay,0,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("L3 count:Disabled ")), 0);
  m_L3CountDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
					     _T("L3 Count"),_T("min"),_T("L3 count"),
					     _T("l3cnt"),_T("hd"));
  infoSizer->Add(m_L3CountDisplay,0,wxEXPAND);
  
  //labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Dead time")), 0);
  //  m_TrigTimeDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.6f"),_T("Time count")
  //					      _T("0:1e8:2e8:3e8:4e8:5e8:6e8:7e8:9e8:1e9:"),
  //					      _T("Time count"),_T("trigtime"),_T("hd"));
  // infoSizer->Add(m_TrigTimeDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("PPS:Disabled ")), 0);
  m_PPSDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),_T("PPS")
					 _T("0:1e8:2e8:3e8:4e8:5e8:6e8:7e8:9e8:1e9:"),
					 _T("PPS"),_T("ppsnum"),_T("hd"));
  infoSizer->Add(m_PPSDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("C3PO:Disabled ")), 0);
  m_C3PODisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
					  _T("C3PO")
					  _T("0:1e8:2e8:3e8:4e8:5e8:6e8:7e8:9e8:1e9:"),
					  _T("c3po"),_T("c3po"),_T("hd"));
  infoSizer->Add(m_C3PODisplay,0,wxEXPAND);
  m_CurrentTime = new wxDateTime();
  UpdateCurrentTime();

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);

}

TURFInfo::~TURFInfo(){}

void TURFInfo::UpdateCurrentTime(){
  m_CurrentTime->SetToCurrent();
  m_CurrentTime->MakeUTC();
  m_CurrentTimeDisplay->SetLabel(m_CurrentTime->Format(_T("%Y-%m-%d %H:%M:%S")));
}

void TURFInfo::SetTime(unsigned long time){
  wxDateTime t((time_t)time);
  t.MakeUTC();
  m_TimeDisplay->SetLabel(t.Format(_T("%Y-%m-%d %H:%M:%S")));
}

void TURFInfo::SetTimeUS(unsigned long us){
  wxString tmp;
  /////////////////////////////////////////
  wxString *phimasks =new wxString[16];
  wxString phi=_T(" ");
  for(int i=0;i<16;i++){
  if(us&(1<<i)){
	phimasks[i].Printf(_T("%d%c"), i+1,',');
	phi=phi+phimasks[i];
	}
}
  ///////////////////////////////////////
  tmp << phi;
  m_TimeUSDisplay->SetLabel(tmp);
}

void TURFInfo::SetL3Count(unsigned char l3cnt){
  m_L3CountDisplay->SetLabel((double)l3cnt);
}

void TURFInfo::SetTriggerType(unsigned char type){
  m_TypeDisplay->SetLabel((double)type);
}

void TURFInfo::SetTriggerNumber(unsigned short trignum){
  m_TriggerNumberDisplay->SetLabel((double)trignum);
}

void TURFInfo::SetPPS(unsigned long pps){
  m_PPSDisplay->SetLabel((double)pps);
}

void TURFInfo::SetC3PO(unsigned long c3po){
  m_C3PODisplay->SetLabel((double)c3po);
}

//void TURFInfo::SetTrigTime(unsigned long time){
//  m_TrigTimeDisplay->SetLabel((double)time);
//}
void TURFInfo::SetTrigTime(double time){
  m_TrigTimeDisplay->SetLabel(time);
}

