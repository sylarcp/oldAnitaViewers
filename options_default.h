/* 
 * options_default.h
 */

#ifndef _OPTIONS_DEFAULTH_
#define _OPTIONS_DEFAULTH_

#include "options.h"

namespace AviewOpts{

  wxRangeSpec **TempSpec;
  double **TempRange;
  ScaleType *TempScaleType;
  unsigned int *TempAverage;
  wxString *TempLabels;
  wxString *TempCaptions;
  wxString *TempVars;
  wxString *TempTable;

  wxRangeSpec **DiskSpec;
  double **DiskRange;
  ScaleType *DiskScaleType;
  unsigned int *DiskAverage;
  wxString *DiskLabels;
  wxString *DiskRefLabels;
  wxString *DiskCaptions;
  wxString *DiskVars;
  wxString *DiskTable;

  wxRangeSpec **QueueSpec;
  double **QueueRange;
  ScaleType *QueueScaleType;
  unsigned int *QueueAverage;
  wxString *QueueLabels;
  wxString *QueueRefLabels;
  wxString *QueueCaptions;
  wxString *QueueVars;
  wxString *QueueTable;

  wxRangeSpec **CurrentSpec;
  wxString *CurrentLabels;
  wxString *CurrentCaptions;
  wxString *CurrentVars;
  wxString *CurrentTable;

  wxRangeSpec **VoltageSpec;
  wxString *VoltageLabels;
  wxString *VoltageCaptions;
  wxString *VoltageVars;
  wxString *VoltageTable;
  
  wxRangeSpec **PowerSpec;
  wxString *PowerLabels;
  wxString *PowerCaptions;
  wxString *PowerVars;
  wxString *PowerTable;

  wxRangeSpec **AuxSpec;
  wxString *AuxLabels;
  wxString *AuxCaptions;
  wxString *AuxVars;
  wxString *AuxTable;

  wxRangeSpec **RFSpecRMS;
  wxRangeSpec **RFSpecAbsolute;
  wxString *RFLabels;
  wxString *RFRefLabels;

  wxRangeSpec *EventRateSpec;
  wxString EventRateLabels;
  double *EventRateRange;
  ScaleType EventRateScaleType;
  unsigned int EventRateAverage;
  unsigned int EventRateStep;

  RFOverviewType RFOverview=newPhiView;
  RFViewType RFView=VoltageView;
  RFTabType RFTab=VoltageTab;
  PayloadStyleType PayloadStyle=Mesh;
  RFReferenceType RFReference=Absolute;

  wxColour L1MaskColour=wxColour(0,0,120);
  wxColour L1PhiMaskColour=wxColour(84,84,144);
  wxColour PhiMaskColour=wxColour(84,84,84);
  wxColour PhiHMaskColour=wxColour(84,84,84);
  wxColour L3TrigColour=wxColour(255,0,0);
  wxColour L3HTrigColour=wxColour(0,255,0);

  bool Compass=true;
  bool ClockRF=true;
  int tabScaleMin=-200;
  int tabScaleMax=200;
  int clockTabScaleMin=-350;
  int clockTabScaleMax=350;

  GPSType DefaultGPS=ADU5;
  int readDelay=2;
  bool sqlDump=false;
  bool crcCheck=true;
  bool fetchRunNumber=true;
  
  //  wxString dbHost=_T("157.132.95.142");
  wxString dbHost=_T("128.175.14.242");
  wxString dbUser=_T("gui");
  wxString dbName=_T("anita_MMDDx");
  wxString dbPwd=_T("AniTa08");
};

wxColour *wxYELLOW = new wxColour(218,165,32);

#endif // _OPTIONS_DEFAULTH_
