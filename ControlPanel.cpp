/*
 * ControlPanel.cpp
 *
 * Container class for ControlItems providing the window for them. It is 
 * ignorant of its content and just passes function calls to its members.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ControlPanel.h"
#endif

#include <wx/sizer.h>
#include <wx/stattext.h>
#include "ControlPanel.h"
#include "FlightControlItem.h" 

IMPLEMENT_CLASS(ControlPanel,wxDialog)

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

ControlPanel::ControlPanel(wxWindow *parent,char *itemClassName,int n,
			   const wxString &caption):nItem(n){
  Create(parent,wxID_ANY,caption);

  wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
  topSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Pck Type |                   UTC               |Prev|Next|                Auto")),0,wxEXPAND);
  //  topSizer->Add(new wxStaticText(this,wxID_ANY,_T(" Pck Type |            UTC          |Prev|Next|        Auto")),0,wxEXPAND);

  m_ItemList=new BaseControlItem*[nItem];

  wxClassInfo *itemClass=wxClassInfo::FindClass(_U(itemClassName));

  for(int i=0;i<nItem;++i){
    wxObject *tmpObject=itemClass->CreateObject();
    if((m_ItemList[i]=dynamic_cast<BaseControlItem*>(tmpObject)))
      m_ItemList[i]->Create(this,i);
  }
  
  // Here we will crash if items weren't created properly...which is good I guess
  for(int i=0;i<nItem;++i)
    topSizer->Add(m_ItemList[i],0,wxEXPAND);

  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);
}

ControlPanel::~ControlPanel(){
  for(int i=0;i<nItem;++i) delete m_ItemList[i];
  delete[] m_ItemList;
}

void ControlPanel::SetIndex(int type,int n){
   m_ItemList[type]->SetIndex(n);
}

void ControlPanel::SetIndex(int type,void *data){
   m_ItemList[type]->SetIndex(data);
}

void ControlPanel::SetTime(int type,long time,bool mark){
  m_ItemList[type]->SetTime(time,mark);
}

time_t ControlPanel::GetTime(int type){
  return m_ItemList[type]->GetTime();
}

void ControlPanel::SetList(int type,void *list,int n){
  m_ItemList[type]->SetList(list,n);
}

void ControlPanel::SetLabel(int type,const wxString &label){
  m_ItemList[type]->SetLabel(label);
}

void ControlPanel::Next(int type,wxCommandEvent &event){
  m_ItemList[type]->Next(event);
}

void ControlPanel::Prev(int type,wxCommandEvent &event){
  m_ItemList[type]->Prev(event);
}

void ControlPanel::Last(int type,wxCommandEvent &event){
  m_ItemList[type]->Last(event);
}

void ControlPanel::InverseTimers(){
    for(int i=0;i<nItem;++i) m_ItemList[i]->InverseTimer();
}
void ControlPanel::StopTimers(int type){
  if(type<0)
    for(int i=0;i<nItem;++i) m_ItemList[i]->StopTimer();
  else
    m_ItemList[type]->StopTimer();
}

void ControlPanel::StartTimers(int type){
  if(type<0)
    for(int i=0;i<nItem;++i) m_ItemList[i]->StartTimer();
  else
    m_ItemList[type]->StartTimer();
}

void ControlPanel::AdjustTimer(int type,int delay){
  m_ItemList[type]->AdjustTimer(delay);
}

void ControlPanel::MarkIndex(int type){
  m_ItemList[type]->MarkIndex();
}

