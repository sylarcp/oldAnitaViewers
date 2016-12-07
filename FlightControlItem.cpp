/*
 * FlightControlItem.cpp
 *
 * Class for control of intraction with packet files.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "FlightControlItem.h"
#endif

#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include <wx/textdlg.h>
#include <wx/filename.h>
#include <wx/log.h>
#include "FlightControlItem.h"

DEFINE_EVENT_TYPE(wxLOAD_FILE)

IMPLEMENT_DYNAMIC_CLASS(FlightControlItem,BaseControlItem)

FlightControlItem::FlightControlItem()
  :m_subIndex(0),m_list(NULL){}

FlightControlItem::FlightControlItem(wxWindow *parent, int type)
  :m_subIndex(0),m_list(NULL){
  FlightControlItem::Create(parent,type);
}

void FlightControlItem::Create(wxWindow *parent, int type){
  BaseControlItem::Create(parent,type,wxTE_READONLY);
}

FlightControlItem::~FlightControlItem(){}

void FlightControlItem::Next(wxCommandEvent &WXUNUSED(event)){
  if(m_list && m_index>=-1 && m_index<(int)m_list->GetCount()){
    if(m_index==-1){  // First attempt 
      ++m_index;
    }else{// if((*m_list)[m_index].Find(".gz")!=wxNOT_FOUND){// Zipped, multipacket file
      if(++m_subIndex>=m_subMax ){ // Roll over subcount if needed 
	if(++m_index<(int)m_list->GetCount()){
	  m_subIndex=0;
	}else{
	  return; // No next event ... Here should go hook for next directory
	}
      }
//     }else if(m_index+1<(int)m_list->GetCount()){// Not zipped, multipacket file
//       ++m_index;
    }
    SendEvent(GetParent()->GetParent()->GetEventHandler(),
	      wxLOAD_FILE,(*m_list)[m_index],m_subIndex);
  }
}
void FlightControlItem::Prev(wxCommandEvent &WXUNUSED(event)){
  if(m_list && m_index>=0 && m_list->GetCount()>0){
//    if((*m_list)[m_index].Find(".gz")!=wxNOT_FOUND){// Zipped, multipacket file
      if(--m_subIndex<0){ // Roll under subcount if needed
	if(--m_index>=0){
	  m_subIndex=m_subMax-1;
	}else{
	  return; // No previous event ... Here should go hook for previous directory
	}
      }
//    }else if(m_index>0){    // Not zipped, multipacket file
//      --m_index;
//    }
    SendEvent(GetParent()->GetParent()->GetEventHandler(),
	      wxLOAD_FILE,(*m_list)[m_index],m_subIndex);
  }
}

void FlightControlItem::Last(wxCommandEvent &WXUNUSED(event)){
  if(m_list && m_list->GetCount()>0){
    m_subIndex=0;
    m_index=m_list->GetCount()-1;
    SendEvent(GetParent()->GetParent()->GetEventHandler(),
	      wxLOAD_FILE,(*m_list)[m_index],m_subIndex);
  }
}


void FlightControlItem::Select(wxCommandEvent &WXUNUSED(event)){
  if(!m_list){
    wxMessageBox(_T("Packet list is unavailable, please select packet directory."),
		 _T("No packets available"),wxOK|wxICON_ERROR);
    return;
  }
  int N=m_list->GetCount();
  wxString shortList[N];
  for(int i=0;i<N;++i){
    int c=(*m_list)[i].Find(wxFileName::GetPathSeparator(),true);
    shortList[i]=(*m_list)[i].Mid(c+1);
  }
  
  wxSingleChoiceDialog *dlg=
    new wxSingleChoiceDialog(this,_T("Select packet file"),_T("Select packet"),N,shortList);
  if(dlg->ShowModal()==wxID_OK){
    m_index=dlg->GetSelection();
//     if((*m_list)[m_index].Find(".gz")==wxNOT_FOUND){
//       SendEvent(GetParent()->GetParent()->GetEventHandler(),
// 		wxLOAD_FILE,(*m_list)[m_index]);
//     }else{
    wxTextEntryDialog *subdlg= new wxTextEntryDialog(this,_T("Select order of packet in file (1-N)"),_T("Enter packet order"),_T("1"));
    if(subdlg->ShowModal()==wxID_OK){
      long val;
      if(subdlg->GetValue().ToLong(&val) && val>0 && val<=m_subMax){
	m_subIndex=val-1;
	SendEvent(GetParent()->GetParent()->GetEventHandler(),
		  wxLOAD_FILE,(*m_list)[m_index],m_subIndex);
	  //	}
      }
    }
  }
  delete dlg;
}

void FlightControlItem::SetList(void *list,int n){
  m_list=(wxArrayString*)list;
  m_subMax=n;
}

void FlightControlItem::SetIndex(int n){
  BaseControlItem::SetIndex(n);
  m_subIndex=0;
}


