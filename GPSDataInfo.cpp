/*
 * GPSDataInfo.cpp
 *
 * Definitions for display of full GPS data info in separate window.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "GPSDataInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/datetime.h>
#include "GPSDataInfo.h"

IMPLEMENT_CLASS(GPSDataInfo, wxDialog )

/*
 * Constructor
 */
GPSDataInfo::GPSDataInfo( wxWindow* parent ){
  Create(parent,wxID_ANY,_T("GPS Data"),wxDefaultPosition,wxDefaultSize,
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU);
  
  SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
  buttons->AddButton(new wxButton(this,wxID_OK,_("OK")));
  buttons->Realize();

  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *gpsSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *adu5Sizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *adu5Sizer2=new wxBoxSizer(wxVERTICAL);

  wxStaticBox *g12box=new wxStaticBox(this,wxID_ANY,_("G12"));
  wxStaticBoxSizer *g12Sizer=new wxStaticBoxSizer(g12box,wxHORIZONTAL);
  wxBoxSizer *g12labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *g12infoSizer=new wxBoxSizer(wxVERTICAL);
  g12Sizer->Add(g12labelSizer,0,wxEXPAND);
  g12Sizer->Add(g12infoSizer,1,wxEXPAND);
  
  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("UTC Time: ")),0);
  m_G12TimeDisplay = new wxStaticText(this,wxID_ANY,_T("                      ")); 
  g12infoSizer->Add(m_G12TimeDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Microsec: ")),0);
  m_G12TimeUSDisplay = new wxStaticText(this,wxID_ANY,_T("                    ")); 
  g12infoSizer->Add(m_G12TimeUSDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("TimeOfDay: ")),0);
  m_G12TodDisplay = new wxStaticText(this,wxID_ANY,_T("")); 
  g12infoSizer->Add(m_G12TodDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("# satellites: ")),0);
  m_G12NumSatellitesDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
						      _T("G12 Sat #"),_T("min"),_T("Sat #"),
						      _T("numsats"),_T("g12_pos"));
  g12infoSizer->Add(m_G12NumSatellitesDisplay,0,wxEXPAND);

  wxString xUnitsLoc[3]={_T("min"),_T("min"),_T("min")};
  wxString yUnitsLoc[3]={_T("Lat [deg]"),_T("Lon [deg]"),_T("Alt [m]")};
  wxString dbVarLoc[3]={_T("latitude"),_T("longitude"),_T("altitude")};
  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Location: ")),0);
  m_G12LocationDisplay = new wxHistoryStaticText(this,wxID_ANY,3,_T("%+8.3f"),_T(":"),
						 wxDefaultPosition,
						 wxDefaultSize,0,
						 _T("G12 Location"),
						 xUnitsLoc,yUnitsLoc,
						 dbVarLoc,_T("g12_pos")); 
  g12infoSizer->Add(m_G12LocationDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Course: ")),0);
  m_G12CourseDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%+.3f"),
					       _T("G12 Course"),_T("min"),_T("deg"),
					       _T("course"),_T("g12_pos"));
  g12infoSizer->Add(m_G12CourseDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Vert. velocity: ")),0);
  m_G12VerticalVelocityDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%+.3f"),
							 _T("G12 Vert. Velocity"),
							 _T("min"),_T("m/s?"),
							 _T("upv"),_T("g12_pos"));
  g12infoSizer->Add(m_G12VerticalVelocityDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Speed [kt]: ")),0);
  m_G12SpeedDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
					      _T("G12 Speed"),
					      _T("min"),_T("kt"),
					      _T("vkt"),_T("g12_pos"));
  g12infoSizer->Add(m_G12SpeedDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("PDOP: ")),0);
  m_G12PdopDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),_T("G12 PDOP"),
					     _T("min"),_T("pdop"),_T("pdop"),_T("g12_pos"));
  g12infoSizer->Add(m_G12PdopDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("HDOP: ")),0);
  m_G12HdopDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),_T("G12 HDOP"),
					     _T("min"),_T("hdop"),_T("hdop"),_T("g12_pos"));
  g12infoSizer->Add(m_G12HdopDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("VDOP: ")),0);
  m_G12VdopDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),_T("G12 VDOP"),
					     _T("min"),_T("vdop"),_T("vdop"),_T("g12_pos"));
  g12infoSizer->Add(m_G12VdopDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("TDOP: ")),0);
  m_G12TdopDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),_T("G12 TDOP"),
					     _T("min"),_T("tdop"),_T("tdop"),_T("g12_pos"));
  g12infoSizer->Add(m_G12TdopDisplay,0,wxEXPAND);

  gpsSizer->Add(g12Sizer,1,wxEXPAND);
  
  wxStaticBox *patbox=new wxStaticBox(this,wxID_ANY,_("ADU5 Pat"));
  wxStaticBoxSizer *patSizer=new wxStaticBoxSizer(patbox,wxHORIZONTAL);
  wxBoxSizer *patlabelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *patinfoSizer=new wxBoxSizer(wxVERTICAL);
  patSizer->Add(patlabelSizer,0,wxEXPAND);
  patSizer->Add(patinfoSizer,1,wxEXPAND);

  patlabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("UTC Time: ")),0);
  m_ADU5PatTimeDisplay = new wxStaticText(this,wxID_ANY,_T(""),wxDefaultPosition,
					  wxSize(160,-1));
  patinfoSizer->Add(m_ADU5PatTimeDisplay,0,wxEXPAND);

  patlabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Microsec: ")),0);
  m_ADU5PatTimeUSDisplay = new wxStaticText(this,wxID_ANY,_T("")); 
  patinfoSizer->Add(m_ADU5PatTimeUSDisplay,0,wxEXPAND);

  patlabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("TimeOfDay: ")),0);
  m_ADU5TodDisplay = new wxStaticText(this,wxID_ANY,_T("")); 
  patinfoSizer->Add(m_ADU5TodDisplay,0,wxEXPAND);

  wxString xUnitsOrient[3]={_T("min"),_T("min"),_T("min")};
  wxString yUnitsOrient[3]={_T("Heading [deg]"),_T("Pitch [deg]"),_T("Roll [deg]")};
  wxString dbVarOrient[3]={_T("heading"),_T("pitch"),_T("roll")};
  patlabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Orientation: ")),0);
  m_ADU5OrientationDisplay = new wxHistoryStaticText(this,wxID_ANY,3,_T("%+5.1f"),
						     _T(":"),wxDefaultPosition,
						     wxDefaultSize,0,
						     _T("Orientation"),
						     xUnitsOrient,yUnitsOrient,
						     dbVarOrient,_T("adu5_pat"));
  patinfoSizer->Add(m_ADU5OrientationDisplay,0,wxEXPAND);

  wxString xUnitsRMS[3]={_T("min"),_T("min")};
  wxString yUnitsRMS[3]={_T("mrms"),_T("brms")};
  wxString dbVarRMS[3]={_T("mrms"),_T("brms")};
  patlabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("mrms/brms: ")),0);
  m_ADU5RmsDisplay = new wxHistoryStaticText(this,wxID_ANY,2,_T("%.3f"),_T(":"),
					     wxDefaultPosition,wxDefaultSize,0,
					     _T("MRMS/BRMS"),xUnitsRMS,yUnitsRMS,
					     dbVarRMS,_T("adu5_pat"));
  patinfoSizer->Add(m_ADU5RmsDisplay,0,wxEXPAND);

  patlabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Location: ")),0);
  m_ADU5LocationDisplay = new wxHistoryStaticText(this,wxID_ANY,3,_T("%+8.3f"),_T(":"),
						  wxDefaultPosition,
						  wxDefaultSize,0,
						  _T("ADU5 Location"),
						  xUnitsLoc,yUnitsLoc,
						  dbVarLoc,_T("adu5_pat"));
  patinfoSizer->Add(m_ADU5LocationDisplay,0,wxEXPAND);

  patlabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Unit/Att. Flag: ")),0);
  m_ADU5AttFlagDisplay = new wxStaticText(this,wxID_ANY,_T("")); 
  patinfoSizer->Add(m_ADU5AttFlagDisplay,0,wxEXPAND);

  wxStaticBox *vtgbox=new wxStaticBox(this,wxID_ANY,_("ADU5 Vtg"));
  wxStaticBoxSizer *vtgSizer=new wxStaticBoxSizer(vtgbox,wxHORIZONTAL);
  wxBoxSizer *vtglabelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *vtginfoSizer=new wxBoxSizer(wxVERTICAL);
  vtgSizer->Add(vtglabelSizer,0,wxEXPAND);
  vtgSizer->Add(vtginfoSizer,1,wxEXPAND);

  vtglabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("UTC Time: ")),0);
  m_ADU5VtgTimeDisplay = new wxStaticText(this,wxID_ANY,_T("")); 
  vtginfoSizer->Add(m_ADU5VtgTimeDisplay,0,wxEXPAND);

  vtglabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Microsec: ")),0);
  m_ADU5VtgTimeUSDisplay = new wxStaticText(this,wxID_ANY,_T("")); 
  vtginfoSizer->Add(m_ADU5VtgTimeUSDisplay,0,wxEXPAND);

  vtglabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("True course: ")),0);
  m_ADU5TrueCourseDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						    _T("True course"),_T("min"),_T("deg"),
						    _T("course"),_T("adu5_vtg")); 
  vtginfoSizer->Add(m_ADU5TrueCourseDisplay,0,wxEXPAND);

  vtglabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Mag. course: ")),0);
  m_ADU5MagCourseDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						   _T("Magnetic course"),_T("min"),
						   _T("deg"),_T("mcourse"),_T("adu5_vtg"));
  vtginfoSizer->Add(m_ADU5MagCourseDisplay,0,wxEXPAND);

  vtglabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Speed [kt]: ")),0);
  m_ADU5SpeedKtDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						 _T("Speed/Knots"),_T("min"),
						 _T("kt"),_T("vkt"),_T("adu5_vtg"));
  vtginfoSizer->Add(m_ADU5SpeedKtDisplay,0,wxEXPAND);

  vtglabelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Speed [kph]: ")),0);
  m_ADU5SpeedKphDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						  _T("Speed/KPH"),_T("min"),
						  _T("kph"),_T("vkph"),_T("adu5_vtg"));
  vtginfoSizer->Add(m_ADU5SpeedKphDisplay,0,wxEXPAND);
  
  wxStaticBox *patbox_u2=new wxStaticBox(this,wxID_ANY,_("ADU5 Pat  Unit 2"));
  wxStaticBoxSizer *patSizer2=new wxStaticBoxSizer(patbox_u2,wxHORIZONTAL);
  wxBoxSizer *patlabelSizer2=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *patinfoSizer2=new wxBoxSizer(wxVERTICAL);
  patSizer2->Add(patlabelSizer2,0,wxEXPAND);
  patSizer2->Add(patinfoSizer2,1,wxEXPAND);

  patlabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("UTC Time: ")),0);
  m_ADU5PatTimeDisplay2 = new wxStaticText(this,wxID_ANY,_T(""),wxDefaultPosition,
					  wxSize(160,-1));
  patinfoSizer2->Add(m_ADU5PatTimeDisplay2,0,wxEXPAND);

  patlabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Microsec: ")),0);
  m_ADU5PatTimeUSDisplay2 = new wxStaticText(this,wxID_ANY,_T("")); 
  patinfoSizer2->Add(m_ADU5PatTimeUSDisplay2,0,wxEXPAND);

  patlabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("TimeOfDay: ")),0);
  m_ADU5TodDisplay2 = new wxStaticText(this,wxID_ANY,_T("")); 
  patinfoSizer2->Add(m_ADU5TodDisplay2,0,wxEXPAND);

  patlabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Orientation: ")),0);
  m_ADU5OrientationDisplay2 = new wxHistoryStaticText(this,wxID_ANY,3,_T("%+5.1f"),
						     _T(":"),wxDefaultPosition,
						     wxDefaultSize,0,
						     _T("Orientation"),
						     xUnitsOrient,yUnitsOrient,
						     dbVarOrient,_T("adu5_pat"));
  patinfoSizer2->Add(m_ADU5OrientationDisplay2,0,wxEXPAND);

  patlabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("mrms/brms: ")),0);
  m_ADU5RmsDisplay2 = new wxHistoryStaticText(this,wxID_ANY,2,_T("%.3f"),_T(":"),
					     wxDefaultPosition,wxDefaultSize,0,
					     _T("MRMS/BRMS"),xUnitsRMS,yUnitsRMS,
					     dbVarRMS,_T("adu5_pat"));
  patinfoSizer2->Add(m_ADU5RmsDisplay2,0,wxEXPAND);

  patlabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Location: ")),0);
  m_ADU5LocationDisplay2 = new wxHistoryStaticText(this,wxID_ANY,3,_T("%+8.3f"),_T(":"),
						  wxDefaultPosition,
						  wxDefaultSize,0,
						  _T("ADU5 Location"),
						  xUnitsLoc,yUnitsLoc,
						  dbVarLoc,_T("adu5_pat"));
  patinfoSizer2->Add(m_ADU5LocationDisplay2,0,wxEXPAND);

  patlabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Unit/Att. Flag: ")),0);
  m_ADU5AttFlagDisplay2 = new wxStaticText(this,wxID_ANY,_T("")); 
  patinfoSizer2->Add(m_ADU5AttFlagDisplay2,0,wxEXPAND);

  wxStaticBox *vtgbox_u2=new wxStaticBox(this,wxID_ANY,_("ADU5 Vtg  Unit 2"));
  wxStaticBoxSizer *vtgSizer2=new wxStaticBoxSizer(vtgbox_u2,wxHORIZONTAL);
  wxBoxSizer *vtglabelSizer2=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *vtginfoSizer2=new wxBoxSizer(wxVERTICAL);
  vtgSizer2->Add(vtglabelSizer2,0,wxEXPAND);
  vtgSizer2->Add(vtginfoSizer2,1,wxEXPAND);

  vtglabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("UTC Time: ")),0);
  m_ADU5VtgTimeDisplay2 = new wxStaticText(this,wxID_ANY,_T("")); 
  vtginfoSizer2->Add(m_ADU5VtgTimeDisplay2,0,wxEXPAND);

  vtglabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Microsec: ")),0);
  m_ADU5VtgTimeUSDisplay2 = new wxStaticText(this,wxID_ANY,_T("")); 
  vtginfoSizer2->Add(m_ADU5VtgTimeUSDisplay2,0,wxEXPAND);

  vtglabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("True course: ")),0);
  m_ADU5TrueCourseDisplay2 = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						    _T("True course"),_T("min"),_T("deg"),
						    _T("course"),_T("adu5_vtg")); 
  vtginfoSizer2->Add(m_ADU5TrueCourseDisplay2,0,wxEXPAND);

  vtglabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Mag. course: ")),0);
  m_ADU5MagCourseDisplay2 = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						   _T("Magnetic course"),_T("min"),
						   _T("deg"),_T("mcourse"),_T("adu5_vtg"));
  vtginfoSizer2->Add(m_ADU5MagCourseDisplay2,0,wxEXPAND);

  vtglabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Speed [kt]: ")),0);
  m_ADU5SpeedKtDisplay2 = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						 _T("Speed/Knots"),_T("min"),
						 _T("kt"),_T("vkt"),_T("adu5_vtg"));
  vtginfoSizer2->Add(m_ADU5SpeedKtDisplay2,0,wxEXPAND);

  vtglabelSizer2->Add(new wxStaticText(this,wxID_ANY,_T("Speed [kph]: ")),0);
  m_ADU5SpeedKphDisplay2 = new wxHistoryStaticText(this,wxID_ANY,_T("%.3f"),
						  _T("Speed/KPH"),_T("min"),
						  _T("kph"),_T("vkph"),_T("adu5_vtg"));
  vtginfoSizer2->Add(m_ADU5SpeedKphDisplay2,0,wxEXPAND);
  
  adu5Sizer->Add(patSizer,0,wxEXPAND);
  adu5Sizer->Add(vtgSizer,1,wxEXPAND);
  adu5Sizer2->Add(patSizer2,0,wxEXPAND);
  adu5Sizer2->Add(vtgSizer2,1,wxEXPAND);

  gpsSizer->Add(adu5Sizer,1,wxEXPAND);
  gpsSizer->Add(adu5Sizer2,1,wxEXPAND);

  sizer->Add(gpsSizer,1,wxEXPAND);
  sizer->Add(buttons,0,wxEXPAND);

  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
  
  return;
}

/*
 * Destructor
 */
GPSDataInfo::~GPSDataInfo(){}

void GPSDataInfo::SetG12Data(const GpsG12PosStruct_t &gps){
  wxString tmp;
  wxDateTime time((time_t)gps.unixTime);
  tmp=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_G12TimeDisplay->SetLabel(tmp);

  tmp.Printf(_T("%d"),gps.unixTimeUs);
  m_G12TimeUSDisplay->SetLabel(tmp);

  tmp.Printf(_T("%d"),gps.timeOfDay);
  m_G12TodDisplay->SetLabel(tmp);
  
  m_G12NumSatellitesDisplay->SetLabel((double)gps.numSats);
  double loc[3]={gps.latitude,gps.longitude,gps.altitude};
  m_G12LocationDisplay->SetLabel(loc);
  m_G12CourseDisplay->SetLabel(gps.trueCourse);
  m_G12VerticalVelocityDisplay->SetLabel(gps.verticalVelocity);
  m_G12SpeedDisplay->SetLabel(gps.speedInKnots);
  m_G12PdopDisplay->SetLabel(gps.pdop);
  m_G12HdopDisplay->SetLabel(gps.hdop);
  m_G12VdopDisplay->SetLabel(gps.vdop);
  m_G12TdopDisplay->SetLabel(gps.tdop);
  
  return;
}

void GPSDataInfo::SetADU5PatData(const GpsAdu5PatStruct_t &gps){
  wxString tmp;
  wxDateTime time((time_t)gps.unixTime);
  tmp=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_ADU5PatTimeDisplay->SetLabel(tmp);

  tmp.Printf(_T("%ld"),gps.unixTimeUs);
  m_ADU5PatTimeUSDisplay->SetLabel(tmp);

  tmp.Printf(_T("%ld"),gps.timeOfDay);
  m_ADU5TodDisplay->SetLabel(tmp);
    
  double orient[3]={gps.heading,gps.pitch,gps.roll};
  m_ADU5OrientationDisplay->SetLabel(orient);
  double rms[2]={gps.mrms,gps.brms};
  m_ADU5RmsDisplay->SetLabel(rms);
  double loc[3]={gps.latitude,gps.longitude,gps.altitude};
  m_ADU5LocationDisplay->SetLabel(loc);

  tmp.Printf(_T("%d %ld"),(gps.gHdr.code&0x60000)>>17, gps.attFlag);
  m_ADU5AttFlagDisplay->SetLabel(tmp);

  return;
}

void GPSDataInfo::SetADU5PatData_U2(const GpsAdu5PatStruct_t &gps){
  wxString tmp;
  wxDateTime time((time_t)gps.unixTime);
  tmp=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_ADU5PatTimeDisplay2->SetLabel(tmp);

  tmp.Printf(_T("%ld"),gps.unixTimeUs);
  m_ADU5PatTimeUSDisplay2->SetLabel(tmp);

  tmp.Printf(_T("%ld"),gps.timeOfDay);
  m_ADU5TodDisplay2->SetLabel(tmp);
    
  double orient[3]={gps.heading,gps.pitch,gps.roll};
  m_ADU5OrientationDisplay2->SetLabel(orient);
  double rms[2]={gps.mrms,gps.brms};
  m_ADU5RmsDisplay2->SetLabel(rms);
  double loc[3]={gps.latitude,gps.longitude,gps.altitude};
  m_ADU5LocationDisplay2->SetLabel(loc);

  tmp.Printf(_T("%d %ld"),(gps.gHdr.code&0x60000)>>17, gps.attFlag);
  m_ADU5AttFlagDisplay2->SetLabel(tmp);

  return;
}

void GPSDataInfo::SetADU5VtgData(const GpsAdu5VtgStruct_t &gps){
  wxString tmp;
  wxDateTime time((time_t)gps.unixTime);
  tmp=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_ADU5VtgTimeDisplay->SetLabel(tmp);

  tmp.Printf(_T("%d"),gps.unixTimeUs);
  m_ADU5VtgTimeUSDisplay->SetLabel(tmp);

  m_ADU5TrueCourseDisplay->SetLabel(gps.trueCourse);
  m_ADU5MagCourseDisplay->SetLabel(gps.magneticCourse);
  m_ADU5SpeedKtDisplay->SetLabel(gps.speedInKnots);
  m_ADU5SpeedKphDisplay->SetLabel(gps.speedInKPH);

  return;
}

void GPSDataInfo::SetADU5VtgData_U2(const GpsAdu5VtgStruct_t &gps){
  wxString tmp;
  wxDateTime time((time_t)gps.unixTime);
  tmp=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_ADU5VtgTimeDisplay2->SetLabel(tmp);

  tmp.Printf(_T("%d"),gps.unixTimeUs);
  m_ADU5VtgTimeUSDisplay2->SetLabel(tmp);

  m_ADU5TrueCourseDisplay2->SetLabel(gps.trueCourse);
  m_ADU5MagCourseDisplay2->SetLabel(gps.magneticCourse);
  m_ADU5SpeedKtDisplay2->SetLabel(gps.speedInKnots);
  m_ADU5SpeedKphDisplay2->SetLabel(gps.speedInKPH);

  return;
}
