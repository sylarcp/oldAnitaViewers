/*
 * SURFInfo.cpp
 *
 * Definitions for display of SURF housekeeping information.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SURFInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SURFInfo.h"

// Constructor
SURFInfo::SURFInfo( wxWindow* parent ):wxPanel(parent){

  //this->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  // Create and arrange all children
  wxBoxSizer *topSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  topSizer->Add(labelSizer,0,wxEXPAND);
  topSizer->Add(infoSizer,1,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("UTC time: ")), 0);
  m_TimeDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_TimeDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Time us: ")), 0);
  m_TimeUSDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_TimeUSDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Global threshold: ")), 0);
  m_GlobalThresholdDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_GlobalThresholdDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Error flag: ")), 0);
  m_ErrorDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_ErrorDisplay,0,wxEXPAND);
  
  for(int i=0;i<N_SURF;++i){
    wxString tmp=_T("Upper Word ");
    tmp << i+1;
    labelSizer->Add(new wxStaticText(this,wxID_ANY,tmp+_T(": ")), 0);
    m_UpperWordDisplay[i] = new wxStaticText(this,wxID_ANY,_T(""));
    infoSizer->Add(m_UpperWordDisplay[i],0,wxEXPAND);
  }

/* //surfMask does not exit any more.
  wxBoxSizer *surfMaskLabelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *surfMaskInfoSizer=new wxBoxSizer(wxVERTICAL);
  topSizer->Add(surfMaskLabelSizer,0,wxEXPAND);
  topSizer->Add(surfMaskInfoSizer,3,wxEXPAND);
  surfMaskLabelSizer->AddStretchSpacer();
  surfMaskInfoSizer->AddStretchSpacer();
  for(int i=0;i<N_SURF;++i){
    wxString tmp=_T("BandMask ");
    tmp << i+1;
    surfMaskLabelSizer->Add(new wxStaticText(this,wxID_ANY,tmp+_T(": ")), 0);
    m_surfBandMaskDisplay[i] = new wxStaticText(this,wxID_ANY,_T(""));
    surfMaskInfoSizer->Add(m_surfBandMaskDisplay[i],0,wxEXPAND);
  }
*/

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);

}

SURFInfo::~SURFInfo(){}

void SURFInfo::SetTime(unsigned long time){
  wxDateTime t((time_t)time);
  t.MakeUTC();
  m_TimeDisplay->SetLabel(t.Format(_T("%Y-%m-%d %H:%M:%S")));
}

void SURFInfo::SetTimeUS(unsigned long us){
  wxString tmp;
  tmp << us;
  m_TimeUSDisplay->SetLabel(tmp);
}

void SURFInfo::SetGlobalThreshold(unsigned short thresh){
  wxString tmp;
  tmp << thresh;
  m_GlobalThresholdDisplay->SetLabel(tmp);
}

void SURFInfo::SetErrorFlag(unsigned short err){
  wxString tmp;
  tmp << (err?_T("on"):_T("off"));
  m_ErrorDisplay->SetLabel(tmp);
}

void SURFInfo::SetUpperWords(int n,unsigned short uw){
  // if(n>=0 && n<N_SURF){ // original, not really necessary.  14-Oct SM. 
  if(n<N_SURF){
    wxString tmp;
    tmp << (uw & 0xf) << ',' << ((uw & 0xc0) >> 6) << ',' << ((uw & 0xf00) >> 8);
    m_UpperWordDisplay[n]->SetLabel(tmp);
  }
}

unsigned long SURFInfo::SetBandMask(int n,unsigned short mask[2]){
  unsigned long trueMask=mask[0]+(mask[1]<<16); // LSB logic 1--32 bits
  SetBandMask(n,trueMask);
  return trueMask;
}

void SURFInfo::SetBandMask(int n,unsigned long trueMask){
  wxString tmp;
  tmp.Printf(_T("0x%04lx, 0x%04lx"),(trueMask&0xffff),(trueMask>>16));
  m_surfBandMaskDisplay[n]->SetLabel(tmp);
}
