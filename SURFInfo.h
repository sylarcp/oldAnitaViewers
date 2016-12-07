/*
 * SURFInfo.h
 * 
 * Handles general info about trigger
 */

#ifndef _SURFINFO_H_
#define _SURFINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SURFInfo.cpp"
#endif

#include "trigmondefs.h"
#include <wx/panel.h>
#include <wx/datetime.h>
#include "historystattext.h"

class SURFInfo: public wxPanel{
 public:
  // Constructor
  SURFInfo(wxWindow* parent);

  // Destructor
  ~SURFInfo();

  // Various functions to set data fields
  void SetTime(unsigned long unixTime);
  void SetTimeUS(unsigned long us);
  void SetGlobalThreshold(unsigned short thresh);
  void SetErrorFlag(unsigned short err);
  void SetUpperWords(int n,unsigned short uw);
  unsigned long SetBandMask(int n,unsigned short mask[2]);
  void SetBandMask(int n,unsigned long trueMask);

 private:
  wxStaticText *m_TimeDisplay;
  wxStaticText *m_TimeUSDisplay;
  wxStaticText *m_GlobalThresholdDisplay;
  wxStaticText *m_ErrorDisplay;
  wxStaticText *m_UpperWordDisplay[N_SURF];
  wxStaticText *m_surfBandMaskDisplay[N_SURF];
};

#endif //_SURFINFO_H_
    
 




