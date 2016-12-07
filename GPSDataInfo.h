/* 
 * GPSDataInfo.h
 */

#ifndef _GPSDATAINFOH_
#define _GPSDATAINFOH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "GPSDataInfo.cpp"
#endif

#include "includes/anitaStructures.h"
#include <wx/dialog.h>
#include "historystattext.h"

class GPSDataInfo:public wxDialog{
 public:
  GPSDataInfo(wxWindow *parent);
  ~GPSDataInfo();

  void SetG12Data(const GpsG12PosStruct_t &gps);
  void SetADU5PatData(const GpsAdu5PatStruct_t &gps);
  void SetADU5PatData_U2(const GpsAdu5PatStruct_t &gps);
  void SetADU5VtgData(const GpsAdu5VtgStruct_t &gps);
  void SetADU5VtgData_U2(const GpsAdu5VtgStruct_t &gps);

 private:
  wxStaticText *m_G12TimeDisplay;
  wxStaticText *m_G12TimeUSDisplay;
  wxStaticText *m_G12TodDisplay;
  wxHistoryStaticText *m_G12NumSatellitesDisplay;
  wxHistoryStaticText *m_G12LocationDisplay;
  wxHistoryStaticText *m_G12CourseDisplay;
  wxHistoryStaticText *m_G12VerticalVelocityDisplay;
  wxHistoryStaticText *m_G12SpeedDisplay;
  wxHistoryStaticText *m_G12PdopDisplay;
  wxHistoryStaticText *m_G12HdopDisplay;
  wxHistoryStaticText *m_G12VdopDisplay;
  wxHistoryStaticText *m_G12TdopDisplay;

  wxStaticText *m_ADU5PatTimeDisplay;
  wxStaticText *m_ADU5PatTimeUSDisplay;
  wxStaticText *m_ADU5TodDisplay;
  wxHistoryStaticText *m_ADU5OrientationDisplay;
  wxHistoryStaticText *m_ADU5RmsDisplay;
  wxHistoryStaticText *m_ADU5LocationDisplay;
  wxStaticText *m_ADU5AttFlagDisplay;
  wxStaticText *m_ADU5VtgTimeDisplay;
  wxStaticText *m_ADU5VtgTimeUSDisplay;
  wxHistoryStaticText *m_ADU5TrueCourseDisplay;
  wxHistoryStaticText *m_ADU5MagCourseDisplay;
  wxHistoryStaticText *m_ADU5SpeedKtDisplay;
  wxHistoryStaticText *m_ADU5SpeedKphDisplay;

  wxStaticText *m_ADU5PatTimeDisplay2;
  wxStaticText *m_ADU5PatTimeUSDisplay2;
  wxStaticText *m_ADU5TodDisplay2;
  wxHistoryStaticText *m_ADU5OrientationDisplay2;
  wxHistoryStaticText *m_ADU5RmsDisplay2;
  wxHistoryStaticText *m_ADU5LocationDisplay2;
  wxStaticText *m_ADU5AttFlagDisplay2;
  wxStaticText *m_ADU5VtgTimeDisplay2;
  wxStaticText *m_ADU5VtgTimeUSDisplay2;
  wxHistoryStaticText *m_ADU5TrueCourseDisplay2;
  wxHistoryStaticText *m_ADU5MagCourseDisplay2;
  wxHistoryStaticText *m_ADU5SpeedKtDisplay2;
  wxHistoryStaticText *m_ADU5SpeedKphDisplay2;

  DECLARE_CLASS(GPSDataInfo)
};

#endif // _GPSDATAINFOH_
