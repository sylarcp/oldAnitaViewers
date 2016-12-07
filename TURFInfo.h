/*
 * TURFInfo.h
 * 
 * Handles general info about trigger
 */

#ifndef _TURFINFO_H_
#define _TURFINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TURFInfo.cpp"
#endif

#include <wx/panel.h>
#include <wx/datetime.h>
#include "historystattext.h"

class TURFInfo: public wxPanel{
 public:
  // Constructor
  TURFInfo(wxWindow* parent);

  // Destructor
  ~TURFInfo();

  // Various functions to set data fields
  void UpdateCurrentTime();
  void SetTime(unsigned long time);
  void SetTimeUS(unsigned long us);
  void SetTriggerType(unsigned char type);
  void SetL3Count(unsigned char l3cnt);
  void SetTriggerNumber(unsigned short trignum);
  void SetPPS(unsigned long pps);
  // void SetTrigTime(unsigned long time);
  void SetTrigTime(double time);
  void SetC3PO(unsigned long interval);

 protected:

 private:
  wxStaticText *m_CurrentTimeDisplay;
  wxStaticText *m_TimeDisplay;
  wxStaticText *m_TimeUSDisplay;
  wxHistoryStaticText *m_TriggerNumberDisplay;
  wxHistoryStaticText *m_TypeDisplay;
  wxHistoryStaticText *m_L3CountDisplay;
  wxHistoryStaticText *m_PPSDisplay;
  wxHistoryStaticText *m_TrigTimeDisplay;
  wxHistoryStaticText *m_C3PODisplay;

  wxDateTime *m_CurrentTime;

};

#endif //_TURFINFO_H_
    
 




