/* 
 * BaseControlItem.h
 */

#ifndef _BASECONTROLITEMH_
#define _BASECONTROLITEMH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BaseControlItem.cpp"
#endif

#include <wx/panel.h>
#include <wx/timer.h>
#include "utcdatectrl.h"

#define CONTROLWIN_OFFSET 0x100
#define NEXT_BUTTON   0x1000
#define PREV_BUTTON   0x1001
#define LAST_BUTTON   0x1002
#define SELECT_BUTTON 0x1003
#define AUTO_CHECKBOX 0x1004
#define AUTO_TIMER    0x1005
#define UTC_DISPLAY   0x1006

class BaseControlItem:public wxPanel{

  DECLARE_DYNAMIC_CLASS( BaseControlItem )

 public:
  BaseControlItem();
  BaseControlItem(wxWindow *parent, int type,int utcStyle=wxTE_PROCESS_ENTER);
  virtual ~BaseControlItem();
  virtual void Create(wxWindow *parent, int type,int utcStyle=wxTE_PROCESS_ENTER);

  // Event handlers
  virtual void Next(wxCommandEvent &event)=0;
  virtual void Prev(wxCommandEvent &event)=0;
  virtual void Last(wxCommandEvent &event)=0;
  virtual void Select(wxCommandEvent &event)=0;
  virtual void TimeSelect(wxCommandEvent &event)=0;
  // This are uniform for all derived classes
  void TimerNext(wxTimerEvent &event);
  void Auto(wxCommandEvent& event);
  
  virtual void SetIndex(int n);
  virtual void SetIndex(void *data);

  void SetTime(time_t tick,bool mark=false);
  time_t GetTime();
  void SetLabel(const wxString &label);
  void InverseTimer();
  void StartTimer();
  void StopTimer();
  void AdjustTimer(int delay);

  // Each derived class should handle its own list and markers
  virtual void SetList(void *list, int n)=0;
  virtual void MarkIndex()=0;

 protected:
  DECLARE_EVENT_TABLE()
  // Each derived class should handle its own event generation  
    void SendEvent(wxEvtHandler *handler,WXTYPE type,const wxString &str, int n=0);

  int m_index;
  wxUTCDateCtrl *m_timeDisplay;

 private:
  bool autoOn;
  int m_delay;
  wxTimer *m_autotimer;
};

#endif // _BASECONTROLITEMH_
