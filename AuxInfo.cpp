/*
 * AuxInfo.cpp
 *
 * Definition for subwindow that keeps some auxilary housekeeping information 
 * displays, like pressures, sun sensors, etc.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "AuxInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "AuxInfo.h"
#include "options.h"

// Constructor
AuxInfo::AuxInfo( wxWindow* parent ):wxPanel(parent){
  using namespace AviewOpts;
  // Create and arrange all children
  wxStaticBox *auxBox = new wxStaticBox(this,wxID_ANY,_("Auxilary Info"));
  wxStaticBoxSizer *topSizer=new wxStaticBoxSizer(auxBox,wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  topSizer->Add(labelSizer,0,wxEXPAND);
  topSizer->Add(infoSizer,1,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,AuxLabels[0]+_T(":")),0);
  m_LowPressureDisplay = new wxRangedStaticText(this,wxID_ANY,
						AuxSpec[0],_T("%.2f"),
						wxDefaultPosition,
						wxDefaultSize,0,
						AuxCaptions[0],
						_T("min"),_T("torr"),
						AuxVars[0],AuxTable[0]);
  infoSizer->Add(m_LowPressureDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,AuxLabels[1]+_T(":")),0);
  m_HighPressureDisplay = new wxRangedStaticText(this,wxID_ANY,
						 AuxSpec[1],_T("%.2f"),
						 wxDefaultPosition,
						 wxDefaultSize,0,
						 AuxCaptions[1],
						 _T("min"),_T("PSI"),
						 AuxVars[1],AuxTable[1]);
  infoSizer->Add(m_HighPressureDisplay,0,wxEXPAND);

  wxString xUnitsSS[2]={_T("min"),_T("min")};
  wxString yUnitsSS[2]={_T("deg"),_T("deg")};
  wxString dbVarSS[2];
  for(int i=0;i<8;++i){
    wxString tmp=_T("SunSensor ");
    tmp << i+1;
    dbVarSS[0].Printf(_T("ssel[%d]"),i+1);
    dbVarSS[1].Printf(_T("ssaz[%d]"),i+1);
    labelSizer->Add(new wxStaticText(this,wxID_ANY,tmp+_T(": ")), 0);
    m_SunSensorDisplay[i] = new wxHistoryStaticText(this,wxID_ANY,2,
						    _T("%+5.3f"),_T(":"),
						    wxDefaultPosition,
						    wxDefaultSize,0,tmp,
						    xUnitsSS,yUnitsSS,
						    dbVarSS,_T("hk"));
    infoSizer->Add(m_SunSensorDisplay[i],0,wxEXPAND);
  }

  wxString xUnitsAcc[3]={_T("min"),_T("min"),_T("min")};
  wxString yUnitsAcc[3]={_T("X"),_T("Y"),_T("Z")};
  wxString dbVarAcc[3];
  // Prepare smaller point font
  wxFont font=GetFont();
  font.SetPointSize(font.GetPointSize()-1);
  for(int i=0;i<2;++i){
    wxString tmp=_T("Accel ");
    tmp << i+1;
    dbVarAcc[0].Printf(_T("accx[%d]"),i+1);
    dbVarAcc[1].Printf(_T("accy[%d]"),i+1);
    dbVarAcc[2].Printf(_T("accz[%d]"),i+1);
    labelSizer->Add(new wxStaticText(this,wxID_ANY,tmp+_T(":")), 0);
    m_AccelerometerDisplay[i] = new wxHistoryStaticText(this,wxID_ANY,3,
							_T("%+5.3f"),_T(":"),
							wxDefaultPosition,
							wxDefaultSize,0,tmp,
							xUnitsAcc,yUnitsAcc,
							dbVarAcc,_T("hk"));
    m_AccelerometerDisplay[i]->SetFont(font);
    infoSizer->Add(m_AccelerometerDisplay[i],0,wxEXPAND);
  }

  wxString xUnitsMag[3]={_T("min"),_T("min"),_T("min")};
  wxString yUnitsMag[3]={_T("X"),_T("Y"),_T("Z")};
  wxString dbVarMag[3]={_T("magx"),_T("magy"),_T("magz")};
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Mag: ")), 0);
  m_MagnetometerDisplay = new wxHistoryStaticText(this,wxID_ANY,3,
						  _T("%+5.3f"),_T(":"),
						  wxDefaultPosition,
						  wxDefaultSize,0,
						  _T("Magnetometer"),
						  xUnitsMag,yUnitsMag,
						  dbVarMag,_T("hk"));
  m_MagnetometerDisplay->SetFont(font);
  infoSizer->AddStretchSpacer();
  infoSizer->Add(m_MagnetometerDisplay,0,wxEXPAND);
  
  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);

}

AuxInfo::~AuxInfo(){}

void AuxInfo::SetLowPressure(double press){
  m_LowPressureDisplay->SetLabel(press);
}

void AuxInfo::SetLowPressureRangeSpec(wxRangeSpec *spec){
  m_LowPressureDisplay->SetRangeSpec(spec);
}

void AuxInfo::SetHighPressure(double press){
  m_HighPressureDisplay->SetLabel(press);
}

void AuxInfo::SetHighPressureRangeSpec(wxRangeSpec *spec){
  m_HighPressureDisplay->SetRangeSpec(spec);
}

void AuxInfo::SetSunSensor(int n, double el, double az, bool good){
  // Set color based on elevation being in 0 to 40 deg
  bool good_el = (el>=0 && el<=40);
  // Make good readings bold and color code them
  if(good){
    if(good_el)
      m_SunSensorDisplay[n]->SetForegroundColour(wxColour(0x09,0x09,0xd8));
    else
      m_SunSensorDisplay[n]->SetForegroundColour(*wxRED);
  }else
    m_SunSensorDisplay[n]->SetForegroundColour(*wxBLACK);

  double tmp[2]={el,az};
   m_SunSensorDisplay[n]->SetLabel(tmp);
}

void AuxInfo::SetSunSensorRaw(int n, double x, double y){
  if(n<0 || n>3) return;
  // Calculate and display elevation and azimuth angle
  double gainX[4]={5.0288,4.8515,5.0599,5.0288};
  double gainY[4]={5.0,5.0,5.0,5.0};
  double offsetX[4]={0.0800,-0.32940,0.05541,-0.23773};
  double offsetY[4]={-0.1572,-0.17477,-0.08458,-0.50356};
  double sep[4]={3.704391,3.618574,3.512025,3.554451};
  double gamma_corr[4]={0.4,-0.2,0.3,0.5};
  double gamma=67.5;
  double sens_az[4]={45,-45,-135,135};

  double x0 = x*gainX[n]-offsetX[n];
  double y0 =-y*gainY[n]-offsetY[n];

  double cosg=cos((gamma-gamma_corr[n])*M_PI/180.);
  double sing=sin((gamma-gamma_corr[n])*M_PI/180.);

  // Sun is now at (-x0,-y0,sep[n]) in frame of sensor
  // Rotate to frame with z pointing up
  double xs=-x0*cosg-sep[n]*sing;
  double ys=-y0;
  double zs=-x0*sing+sep[n]*cosg;
  
  double az_meas = atan2(ys,-xs)*180./M_PI;
  double el = atan2(zs,sqrt(xs*xs+ys*ys))*180./M_PI;
  
  // Convert to azimuth relative to ADU5 pointing 
  double az = az_meas + sens_az[n] + 360;
  while(az>360) az-=360;
  while(az<0) az+=360;

  // Determine if this sunsensor reading is good, i.e. sees the Sun
  bool good_reading = ((fabs(x)<=1) && (fabs(y)<=1));

  SetSunSensor(n,el,az,good_reading);
}

void AuxInfo::SetAccelerometer(int n, double x, double y, double z){
  double tmp[3]={x,y,z};
  if(n>=0 && n<2) m_AccelerometerDisplay[n]->SetLabel(tmp);
}

void AuxInfo::SetMagnetometer(float x, float y, float z){
  double tmp[3]={x,y,z};
  m_MagnetometerDisplay->SetLabel(tmp);
}

