/*
 * TimeTranslator.cpp
 *
 * Utility class that provides dialog which translates time between human 
 * readable from and UNIX timestamps.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TimeTranslator.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "TimeTranslator.h"

IMPLEMENT_CLASS( TimeTranslator, wxDialog )

BEGIN_EVENT_TABLE( TimeTranslator, wxDialog )
  EVT_BUTTON(TOUTC_BUTTON_ID,TimeTranslator::OnToUTC)
  EVT_BUTTON(TOTICK_BUTTON_ID,TimeTranslator::OnToTick)
END_EVENT_TABLE()

/*
 * Constructor
 */
TimeTranslator::TimeTranslator( wxWindow* parent, wxWindowID id, 
				    const wxString &caption,
				    const wxPoint& pos, 
				    const wxSize& size, 
				    long style )
  :wxDialog(parent,id,caption,pos,size,style){
  
  // Allocate all arrays we need

  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *fieldSizer=new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(fieldSizer,0,wxEXPAND);

  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  fieldSizer->Add(labelSizer,0,wxEXPAND);
  fieldSizer->Add(infoSizer,1,wxEXPAND);

  wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
  wxButton *utcButton=new wxButton(this,TOUTC_BUTTON_ID,_("To UTC"));
  wxButton *tickButton=new wxButton(this,TOTICK_BUTTON_ID,_("To UNIX"));
  buttonSizer->Add(utcButton,0,wxRIGHT,5);
  buttonSizer->Add(tickButton,0,wxLEFT,5);
  sizer->Add(buttonSizer);

  wxDateTime now=wxDateTime::Now();
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("UTC time: ")), 1);
  m_utcEntry = new wxUTCDateCtrl(this,UTCTIME_DISPLAY,
				 now.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S")),
				 wxDefaultPosition,wxDefaultSize);
  infoSizer->Add(m_utcEntry,0,wxEXPAND);
    
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("UNIX time: ")), 1);
  wxString tmp;
  tmp << now.GetTicks();
  m_tickEntry = new wxTextCtrl(this,UNIXTICK_DISPLAY,tmp,
			       wxDefaultPosition,wxDefaultSize);
  infoSizer->Add(m_tickEntry,0,wxEXPAND);

  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);

  return;
}

/*
 * Destructor
 */
TimeTranslator::~TimeTranslator(){}

void TimeTranslator::OnToUTC(wxCommandEvent &WXUNUSED(event)){
  long val;
  if(m_tickEntry->GetLineText(0).ToLong(&val)){
    wxDateTime time((time_t)val);
    wxString utc=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
    m_utcEntry->Clear();
    (*m_utcEntry) << utc;
  }else
    wxMessageBox(_T("Invalid UNIX time!"),_T("Invalid UNIX time"),wxOK|wxICON_ERROR);
}

void TimeTranslator::OnToTick(wxCommandEvent &WXUNUSED(event)){
  time_t tick=m_utcEntry->GetTicks();
  if(tick>=0){
    m_tickEntry->Clear();
    (*m_tickEntry) << tick;
  }
}
