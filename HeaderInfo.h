/*
 * HeaderInfo.h
 * 
 * Handles general info about event
 */

#ifndef _HEADERINFO_H_
#define _HEADERINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "HeaderInfo.cpp"
#endif

#include <wx/panel.h>
#include <wx/datetime.h>
#include "historystattext.h"

class HeaderInfo: public wxPanel{
 public:
  // Constructor
  HeaderInfo(wxWindow* parent);

  // Destructor
  ~HeaderInfo();

  // Various functions to set data fields
  void UpdateCurrentTime();
  void SetEventTime(unsigned long time);
  void SetEventUSTime(unsigned long us);
  void SetEventNSTime(unsigned long ns);
  void SetEventNumber(unsigned long evnum);
  void SetRunNumber(unsigned long runnum);
  void SetLocation(float lat,float lon,float alt);
  void SetOrientation(float head,float pitch,float roll);
  void SetSpeed(float speed,float course);
////peng////
  void SetLocationB(float lat,float lon,float alt);
  void SetOrientationB(float head,float pitch,float roll);
  void SetSpeedB(float speed,float course);
////peng///
  void SetCommand(unsigned char cmd[],short n);
 // void SetAntennaMask(unsigned long mask);

  void SetCommandGood(bool good=true); // Changes color of cmd echo
  void SetAttFlag(bool good=true); // Changes color of GPS data 
  void SetAttFlagB(bool good=true); // Changes color of GPS data 
  void SetPatColor(bool);//change backgound color of ADU5 Pat A data.
  void SetPatColorB(bool);//change backgound color of ADU5 Pat B data.
  void SetVtgColor(bool);//change backgound color of ADU5 Vtg A data.
  void SetVtgColorB(bool);//change backgound color of ADU5 Vtg B data.

 protected:

 private:
  wxStaticText *m_CurrentTimeDisplay;
  wxStaticText *m_EventTimeDisplay;
  wxStaticText *m_EventUSTimeDisplay;
  wxStaticText *m_EventNSTimeDisplay;
  wxHistoryStaticText *m_EventNumberDisplay;
  wxHistoryStaticText *m_RunNumberDisplay;
  wxHistoryStaticText *m_LocationDisplay;
  wxHistoryStaticText *m_OrientationDisplay;
  wxHistoryStaticText *m_SpeedDisplay;
////peng////////
  wxHistoryStaticText *m_LocationDisplayB;
  wxHistoryStaticText *m_OrientationDisplayB;
  wxHistoryStaticText *m_SpeedDisplayB;
////peng///////
  wxStaticText *m_CommandDisplay;
  //wxStaticText *m_AntennaMaskDisplay;
  
  wxDateTime *m_CurrentTime;

};

#endif //_HEADERINFO_H_
    
 




