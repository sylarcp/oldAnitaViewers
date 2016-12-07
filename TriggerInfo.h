/*
 * TriggerInfo.h
 * 
 * Handles trigger info about event
 */

#ifndef _TRIGGERINFO_H_
#define _TRIGGERINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TriggerInfo.cpp"
#endif

#include <wx/panel.h>
#include "codedstattext.h"

class TriggerInfo: public wxPanel{
 public:
  // Constructor
  TriggerInfo(wxWindow* parent);

  // Destructor
  ~TriggerInfo();

  // Various functions to set data fields
  void SetPriority(unsigned short priority);   
  void SetTriggerNumber(unsigned short trnum, unsigned char otherFlag2);
  void SetL3Number(unsigned char l3cnt);
  void SetTriggerTimeNS(unsigned long ns,unsigned long c3po);
  void SetPPS(unsigned short pps);
//  void SetDeadtime(double deadtime);
  //  void SetC3PO(unsigned long c3po);
  void SetTurfMonitor(unsigned char word);
  void SetSurfMask(unsigned short mask[]);
  void SetCalibStatus(unsigned short calib);
  void SetTriggerType(unsigned char type);  
  //void SetGlobalThreshold(unsigned short thresh);
  //  void SetInterval(unsigned long dt);

 protected:

 private:
  wxStaticText *m_PriorityDisplay;
  wxCodedStaticText *m_TypeDisplay;
  wxStaticText *m_NumberDisplay;
  wxHistoryStaticText *m_L3NumberDisplay;
  wxStaticText *m_TimeNSDisplay;
  wxHistoryStaticText *m_PPSDisplay;
  //wxHistoryStaticText *m_DeadtimeDisplay;
  wxHistoryStaticText *m_C3PODisplay;
  wxStaticText *m_TurfMonitorDisplay;
  wxStaticText *m_SurfMaskDisplay;
  wxStaticText *m_CalibDisplay;
  //wxStaticText *m_ThresholdDisplay;
  //wxHistoryStaticText *m_IntervalDisplay;
};

#endif //_TRIGGERINFO_H_
    
 




