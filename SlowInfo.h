/*
 * SlowInfo.h
 * 
 * Handles general info about trigger
 */

#ifndef _SLOWINFO_H_
#define _SLOWINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SlowInfo.cpp"
#endif

#include <wx/panel.h>
#include <wx/datetime.h>
#include "historystattext.h"

class SlowInfo: public wxPanel{
 public:
  // Constructor
  SlowInfo(wxWindow* parent);

  // Destructor
  ~SlowInfo();

  // Various functions to set data fields
  void UpdateCurrentTime();
  void SetTime(unsigned long time);
  void SetEventNumber(unsigned long evnum);
  void SetLocation(float lat,float lon,float alt);

 protected:

 private:
  wxStaticText *m_CurrentTimeDisplay;
  wxStaticText *m_TimeDisplay;
  wxHistoryStaticText *m_EventNumberDisplay; 
  wxHistoryStaticText *m_LocationDisplay;

  wxDateTime *m_CurrentTime;

};

#endif //_SLOWINFO_H_
    
 




