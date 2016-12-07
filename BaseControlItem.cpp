/*
 * BaseControlItem.cpp
 *
 * Base class for control panel items. This is true virtual class from which
 * usable classes are derived.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BaseControlItem.h"
#endif

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/valgen.h>
#include "BaseControlItem.h"

IMPLEMENT_CLASS(BaseControlItem,wxPanel)
BEGIN_EVENT_TABLE(BaseControlItem,wxPanel)
  EVT_BUTTON(NEXT_BUTTON,BaseControlItem::Next)
  EVT_BUTTON(PREV_BUTTON,BaseControlItem::Prev)
  EVT_BUTTON(LAST_BUTTON,BaseControlItem::Last)
  EVT_BUTTON(SELECT_BUTTON,BaseControlItem::Select)
  EVT_CHECKBOX(AUTO_CHECKBOX,BaseControlItem::Auto)
  EVT_TIMER(AUTO_TIMER,BaseControlItem::TimerNext)
  EVT_TEXT_ENTER(UTC_DISPLAY,BaseControlItem::TimeSelect)
END_EVENT_TABLE()

BaseControlItem::BaseControlItem():m_index(-1),autoOn(false),m_delay(2000){}

BaseControlItem::BaseControlItem(wxWindow *parent, int type,int utcStyle)
  :m_index(-1),autoOn(false),m_delay(2000){
  BaseControlItem::Create(parent,type,utcStyle);
}

void  BaseControlItem::Create(wxWindow *parent, int type,int utcStyle){
  wxPanel::Create(parent,type+CONTROLWIN_OFFSET);

  wxBoxSizer *sizer=new wxBoxSizer(wxHORIZONTAL);
  wxStaticText *label=new wxStaticText(this,wxID_ANY,_T(""),wxDefaultPosition,
				       wxSize(60,-1),
				       wxST_NO_AUTORESIZE|wxALIGN_CENTRE);
  sizer->Add(label,0,wxEXPAND|wxTOP,5);

  m_timeDisplay=new wxUTCDateCtrl(this,UTC_DISPLAY,_T("YYYY-MM-DD HH:MM:SS"),wxDefaultPosition,wxSize(135,-1),utcStyle);
  sizer->Add(m_timeDisplay,0,wxEXPAND);

  sizer->Add(new wxButton(this,PREV_BUTTON,_T("<"),wxDefaultPosition,
			  wxSize(22,-1),wxBU_EXACTFIT),0,wxEXPAND);
  sizer->Add(new wxButton(this,NEXT_BUTTON,_T(">"),wxDefaultPosition,
			  wxSize(22,-1),wxBU_EXACTFIT),0,wxEXPAND);
  sizer->Add(new wxButton(this,LAST_BUTTON,_T("last"),wxDefaultPosition,
			  wxDefaultSize,wxBU_EXACTFIT),0,wxEXPAND);
  sizer->Add(new wxButton(this,SELECT_BUTTON,_T("select"),wxDefaultPosition,
			  wxDefaultSize,wxBU_EXACTFIT),0,wxEXPAND);
  sizer->Add(new wxCheckBox(this,AUTO_CHECKBOX,_T(""),wxDefaultPosition,
			    wxDefaultSize,wxCHK_2STATE,
			    wxGenericValidator(&autoOn)),0);

  m_autotimer=new wxTimer(this,AUTO_TIMER);

  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
}

BaseControlItem::~BaseControlItem(){
  m_autotimer->Stop();
}

void BaseControlItem::SetIndex(int n){
  m_index=n;
}

void BaseControlItem::SetIndex(void *data){
  SetIndex(*(static_cast<int*>(data)));
}

void BaseControlItem::SetTime(time_t tick,bool mark){
  wxDateTime time(tick);
  time.MakeUTC();
  m_timeDisplay->Clear();
  if(mark) m_timeDisplay->SetBackgroundColour(*wxLIGHT_GREY);
  else m_timeDisplay->SetBackgroundColour(*wxWHITE);
  *m_timeDisplay << time.Format(_T("%Y-%m-%d %H:%M:%S"));
}

time_t BaseControlItem::GetTime(){
  return m_timeDisplay->GetTicks();
}

void BaseControlItem::TimerNext(wxTimerEvent &WXUNUSED(event)){
  wxCommandEvent tmp;
  Next(tmp);
}

void BaseControlItem::Auto(wxCommandEvent &WXUNUSED(event)){
  if(Validate() && TransferDataFromWindow()){
    if(autoOn) m_autotimer->Start(m_delay);
    else m_autotimer->Stop();
  }
}
void BaseControlItem::InverseTimer(){
  autoOn=!autoOn;
    if(autoOn) m_autotimer->Start(m_delay);
    else m_autotimer->Stop();
  TransferDataToWindow();
}
 
void BaseControlItem::StartTimer(){
  autoOn=true;
  m_autotimer->Start(m_delay);				
  TransferDataToWindow();
}

void BaseControlItem::StopTimer(){
  autoOn=false;
  m_autotimer->Stop();				
  TransferDataToWindow();
}
    

void BaseControlItem::AdjustTimer(int delay){
  m_delay=delay*1000;
  if(m_delay==0) m_delay=350; // Give at least 350 millisecond delay
  if(autoOn){
    m_autotimer->Stop();
    m_autotimer->Start(m_delay);
  }
}

void BaseControlItem::SendEvent(wxEvtHandler *handler,WXTYPE type, const wxString &str, int n){
  wxCommandEvent e(type,GetId());
  e.SetEventObject(this);
  e.SetString(str);
  e.SetClientData((void*)n);
  handler->ProcessEvent( e );
}

void BaseControlItem::SetLabel(const wxString &label){
  wxStaticText *lwin;
  if(lwin=dynamic_cast<wxStaticText*>(GetSizer()->GetItem((size_t)0)->GetWindow()))
    lwin->SetLabel(label);
}
