/*
 * AuxInfo.h
 * 
 * Handles aux info about event
 */

#ifndef _AUXINFO_H_
#define _AUXINFO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AuxInfo.cpp"
#endif

#include <wx/panel.h>
#include "rangedstattext.h"

class AuxInfo: public wxPanel{
 public:
  // Constructor
  AuxInfo(wxWindow* parent);

  // Destructor
  ~AuxInfo();

  // Various functions to set data fields
  void SetLowPressure(double press);
  void SetHighPressure(double press);
  void SetSunSensor(int n, double el,double az,bool good);
  void SetSunSensorRaw(int n, double x, double y);
  void SetAccelerometer(int n, double x,double y,double z);
  void SetMagnetometer(float x, float y, float z);

  void SetLowPressureRangeSpec(wxRangeSpec *spec);
  void SetHighPressureRangeSpec(wxRangeSpec *spec);

 private:
  wxRangedStaticText *m_LowPressureDisplay;
  wxRangedStaticText *m_HighPressureDisplay;
  wxHistoryStaticText *m_SunSensorDisplay[8];
  wxHistoryStaticText *m_AccelerometerDisplay[2];
  wxHistoryStaticText *m_MagnetometerDisplay;

};

#endif //_AUXINFO_H_
    
 




