/* 
 * GPSSatInfo.h
 */

#ifndef _GPSSATINFOH_
#define _GPSSATINFOH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "GPSSatInfo.cpp"
#endif

#include <includes/anitaStructures.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include "historystattext.h"

class GPSSatInfo:public wxDialog{
 public:
  GPSSatInfo(wxWindow *parent, int max=MAX_SATS);
  ~GPSSatInfo();

  void SetG12Sat(const GpsG12SatStruct_t &gps);
  void SetADU5Sat(const GpsAdu5SatStruct_t &gps);
  void SetADU5Sat_U2(const GpsAdu5SatStruct_t &gps);

 protected:
  void SetList(wxListCtrl *list, const GpsSatInfo_t sat[],long nSat);

 private:
  wxStaticText *m_G12TimeDisplay;
  wxStaticText *m_ADU5TimeDisplay[4];
  wxStaticText *m_ADU5TimeDisplay2[4];
  wxHistoryStaticText *m_G12NumSatellitesDisplay;
  wxHistoryStaticText *m_ADU5NumSatellitesDisplay[4];
  wxHistoryStaticText *m_ADU5NumSatellitesDisplay2[4];
  wxListCtrl *m_G12SatList;
  wxListCtrl *m_ADU5SatList[4];
  wxListCtrl *m_ADU5SatList2[4];
  int maxSat;

  DECLARE_CLASS(GPSSatInfo)
};

#endif // _GPSSATINFOH_
