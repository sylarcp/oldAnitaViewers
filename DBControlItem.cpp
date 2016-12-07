/*
 * DBControlItem.cpp
 *
 * Definitions for class that is used to control interaction with database.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DBControlItem.h"
#endif

#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/log.h>
#include "DBControlItem.h"
#include "sort.h"

DEFINE_EVENT_TYPE(wxREQUEST_PACKET)

IMPLEMENT_DYNAMIC_CLASS(DBControlItem,BaseControlItem)

DBControlItem::DBControlItem():nList(0),m_list(NULL){}

DBControlItem::DBControlItem(wxWindow *parent, int type)
  :nList(0),m_list(NULL){
  DBControlItem::Create(parent,type);
}

void DBControlItem::Create(wxWindow *parent, int type){
  BaseControlItem::Create(parent,type);
}

DBControlItem::~DBControlItem(){}

void DBControlItem::MarkIndex(){
  if(m_index>=0 && m_index<nList) m_oldIndex=m_list[m_index];
}

void DBControlItem::SetList(void *list,int n){  
  nList=n;
  m_list=(dbRef*)list;
  if(nList && m_index>=0){ // Need to recover old index position
    dbRef *newIndex_p=(dbRef*)bsearch(&m_oldIndex,m_list,nList,sizeof(dbRef),dbRefComp);
    if(newIndex_p) m_index=newIndex_p-m_list;
    else m_index=-1;
  }
}

void DBControlItem::Next(wxCommandEvent &WXUNUSED(event)){
  if(m_list && m_index>=-1 && m_index+1<nList){
    wxString tmp;
    // avoid duplicated entry
    m_index++;
    while (m_list[m_index].nbuf == m_list[m_index-1].nbuf){
        m_index++;
      if (m_index+1==nList) break ;
        }

    tmp << (long)m_list[m_index].nbuf;
    SendEvent(GetParent()->GetParent()->GetEventHandler(),wxREQUEST_PACKET,tmp);
  }
}

void DBControlItem::Prev(wxCommandEvent &WXUNUSED(event)){
  if(m_list && m_index>0 && nList>0){
    wxString tmp;
    // avoid duplicated entry
    m_index--;
    while (m_list[m_index].nbuf == m_list[m_index+1].nbuf) {
        m_index--;
      if (m_index==0) break ;
        }

    tmp << (long)m_list[m_index].nbuf;
    SendEvent(GetParent()->GetParent()->GetEventHandler(),wxREQUEST_PACKET,tmp);
  }
}

void DBControlItem::Last(wxCommandEvent &WXUNUSED(event)){
  if(m_list && nList>0){
     wxString tmp;
     m_index=nList-1;
     // avoid outlyers by time. for ANITA-2
     //while (m_list[m_index].time>1280000000) // this is after 2010-07-24
     while (m_list[m_index].time>1435708800) // this is after 2015-07-01
       if (--m_index==0) return ;
     tmp << (long)m_list[m_index].nbuf;
     SendEvent(GetParent()->GetParent()->GetEventHandler(),wxREQUEST_PACKET,tmp);
  }
}

int DBControlItem::GetHourSelection(){
  static wxSingleChoiceDialog *dlg=NULL;
  if(!dlg){
    wxString hours[]={_T("00:00"),_T("01:00"),_T("02:00"),_T("03:00"),
		      _T("04:00"),_T("05:00"),_T("06:00"),_T("07:00"),
		      _T("08:00"),_T("09:00"),_T("10:00"),_T("11:00"),
		      _T("12:00"),_T("13:00"),_T("14:00"),_T("15:00"),
		      _T("16:00"),_T("17:00"),_T("18:00"),_T("19:00"),
		      _T("20:00"),_T("21:00"),_T("22:00"),_T("23:00")};
    dlg=new wxSingleChoiceDialog(NULL,_T("Select packet UTC hour"),_T("Select hour"),24,hours);
  }
  if(dlg->ShowModal()==wxID_OK)
    return dlg->GetSelection();
  else
    return -1;
}

int DBControlItem::GetMinuteSelection(){
  static wxSingleChoiceDialog *dlg=NULL;
  if(!dlg){
    wxString minutes[]={_T("00:00"),_T("01:00"),_T("02:00"),_T("03:00"),
			_T("04:00"),_T("05:00"),_T("06:00"),_T("07:00"),
			_T("08:00"),_T("09:00"),_T("10:00"),_T("11:00"),
			_T("12:00"),_T("13:00"),_T("14:00"),_T("15:00"),
			_T("16:00"),_T("17:00"),_T("18:00"),_T("19:00"),
			_T("20:00"),_T("21:00"),_T("22:00"),_T("23:00"),
			_T("24:00"),_T("25:00"),_T("26:00"),_T("27:00"),
			_T("28:00"),_T("29:00"),_T("30:00"),_T("31:00"),
			_T("32:00"),_T("33:00"),_T("34:00"),_T("35:00"),
			_T("36:00"),_T("37:00"),_T("38:00"),_T("39:00"),
			_T("40:00"),_T("41:00"),_T("42:00"),_T("43:00"),
			_T("44:00"),_T("45:00"),_T("46:00"),_T("47:00"),
			_T("48:00"),_T("49:00"),_T("50:00"),_T("51:00"),
			_T("52:00"),_T("53:00"),_T("54:00"),_T("55:00"),
			_T("56:00"),_T("57:00"),_T("58:00"),_T("59:00")};
    dlg=new wxSingleChoiceDialog(NULL,_T("Select packet UTC minute"),_T("Select minute"),60,minutes);
  }
  
  if(dlg->ShowModal()==wxID_OK)
    return dlg->GetSelection();
  else
    return -1;
}

// Overrides parent's selection interaction, to create two level selection
// using dates and times
void DBControlItem::Select(wxCommandEvent &WXUNUSED(event)){
  if(!m_list || nList<1){
    wxMessageBox(_T("Packet list is unavailable, either there are no packets of this type or you are not connected to the database."),
		 _T("No packets available"),wxOK|wxICON_ERROR);
    return;
  }

  // Create list of dates
  long firstUTC=m_list[0].time;
  long lastUTC=m_list[nList-1].time;
  firstUTC-=firstUTC%86400;
  lastUTC-=lastUTC%86400;

  unsigned int nDay=(lastUTC-firstUTC)/86400+1; 
  wxDateTime date(firstUTC);
  date.MakeUTC();
  wxString dayList[nDay];
  for(unsigned int i=0;i<nDay;++i){
    dayList[i]=date.Format(_T("%Y-%m-%d"));
    date+=wxDateSpan::Day();
  }

  // Offer dates for selection
  wxSingleChoiceDialog *datedlg=
    new wxSingleChoiceDialog(this,_T("Select packet date"),_T("Select date"),nDay,dayList);
  int hour,min;
  if(datedlg->ShowModal()==wxID_OK && 
     (hour=GetHourSelection())>=0 &&
     (min=GetMinuteSelection())>=0){
    int day=datedlg->GetSelection();
    long startUTC=firstUTC+day*86400+hour*3600+min*60;
    long endUTC=startUTC+59;
    dbRef *pFirstTime=(dbRef*)bsearch(&startUTC,m_list,nList-1,sizeof(dbRef),dbRefSearch);
    if(pFirstTime && nList>1) ++pFirstTime; // Move up by one for first time
    else if(startUTC < m_list[0].time) 
      pFirstTime=m_list;   // If didn't find it, start at the beginning
    if(pFirstTime){
      unsigned int iFirstTime=pFirstTime-m_list;
      dbRef *pLastTime=(dbRef*)bsearch(&endUTC,pFirstTime,nList-iFirstTime-1,sizeof(dbRef),dbRefSearch);
      if(!pLastTime && endUTC>m_list[nList-1].time) 
	pLastTime=m_list+(nList-1); // If didn't find it, end at the end 
      if(pLastTime){
	int iLastTime=pLastTime-m_list;
	int nTime=iLastTime-iFirstTime+1;
	wxString timeList[nTime];
	for(int i=0;i<nTime;++i){
	  wxDateTime time(pFirstTime[i].time);
	  wxString tmp;
	  if(pFirstTime[i].us>=0)
	    tmp.Printf(_T("%s.%06ld"),time.ToUTC().Format(_T("%H:%M:%S")).c_str(),pFirstTime[i].us);
	  else
	    tmp=time.ToUTC().Format(_T("%H:%M:%S"));
	  timeList[i]=tmp;
	}
	wxSingleChoiceDialog *timedlg=
	  new wxSingleChoiceDialog(this,_T("Select packet time"),_T("Select time"),nTime,timeList);
	if(timedlg->ShowModal()==wxID_OK){
	  m_index=timedlg->GetSelection()+iFirstTime;
	  wxString tmp;
	  tmp << m_list[m_index].nbuf;
	  SendEvent(GetParent()->GetParent()->GetEventHandler(),wxREQUEST_PACKET,tmp);
	}
	delete timedlg;
      }else{
	wxMessageBox(_T("No packets in the selected time range"),_T("No data"),wxOK);
      }
    }else{
      wxMessageBox(_T("No packets in the selected time range"),_T("No data"),wxOK);
    }
  }
  delete datedlg;
}

void DBControlItem::TimeSelect(wxCommandEvent &event){
  if(!m_list || nList<1){
    wxMessageBox(_T("Packet list is unavailable, either there are no packets of this type or you are not connected to the database."),
		 _T("No packets available"),wxOK|wxICON_ERROR);
    return;
  }

  time_t utcTime=m_timeDisplay->GetTicks();
  dbRef *pUTCTime=(dbRef*)bsearch(&utcTime,m_list,nList-1,sizeof(dbRef),dbRefSearch);
  if(pUTCTime && nList>1) ++pUTCTime; // Move up by one 
  else{
    if(utcTime<=m_list[0].time) pUTCTime=m_list;   // Use first one
    else pUTCTime=&m_list[nList-1];                 // Use last one
  }
  m_index=pUTCTime-m_list;
  wxString tmp;
  tmp << (long)pUTCTime->nbuf;
  SendEvent(GetParent()->GetParent()->GetEventHandler(),wxREQUEST_PACKET,tmp);
}

void DBControlItem::SetIndex(void *data){
  dbRef *timeMatch=(dbRef*)bsearch(data,m_list,nList-1,sizeof(dbRef),dbRefComp);
  if(timeMatch) m_index=timeMatch-m_list;
}
  
