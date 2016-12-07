/* 
 * options.h
 */

#ifndef _OPTIONSH_
#define _OPTIONSH_

#include "aviewdefs.h"
#include "range.h"

namespace AviewOpts{
  enum RFOverviewType {SurfView,newPhiView, PayloadView,PhiView};
  enum PayloadStyleType {Mesh, Solid};
  enum RFReferenceType {RMS, Absolute};
  enum RFTabType {VoltageTab, FFTTab, PowerTab, ALFATab, ALFAFFTTab};
  enum RFViewType {VoltageView=0x01, FFTView=0x02, PowerView=0x04, ALFAView=0x08, ALFAFFTView=0x16};
  enum GPSType {ADU5,G12};

  extern wxRangeSpec **TempSpec;
  extern double **TempRange;
  extern ScaleType *TempScaleType;
  extern unsigned int *TempAverage;
  extern wxString *TempLabels;
  extern wxString *TempCaptions;
  extern wxString *TempVars;
  extern wxString *TempTable;

  extern wxRangeSpec **DiskSpec;
  extern double **DiskRange;
  extern ScaleType *DiskScaleType;
  extern unsigned int *DiskAverage;
  extern wxString *DiskLabels;
  extern wxString *DiskRefLabels;
  extern wxString *DiskCaptions;
  extern wxString *DiskVars;
  extern wxString *DiskTable;

  extern wxRangeSpec **QueueSpec;
  extern double **QueueRange;
  extern ScaleType *QueueScaleType;
  extern unsigned int *QueueAverage;
  extern wxString *QueueLabels;
  extern wxString *QueueRefLabels;
  extern wxString *QueueCaptions;
  extern wxString *QueueVars;
  extern wxString *QueueTable;

  extern wxRangeSpec **CurrentSpec;
  extern wxString *CurrentLabels;
  extern wxString *CurrentCaptions;
  extern wxString *CurrentVars;
  extern wxString *CurrentTable;

  extern wxRangeSpec **VoltageSpec;
  extern wxString *VoltageLabels;
  extern wxString *VoltageCaptions;
  extern wxString *VoltageVars;
  extern wxString *VoltageTable;

  extern wxRangeSpec **PowerSpec;
  extern wxString *PowerLabels;
  extern wxString *PowerCaptions;
  extern wxString *PowerVars;
  extern wxString *PowerTable;

  extern wxRangeSpec **AuxSpec;
  extern wxString *AuxLabels;
  extern wxString *AuxCaptions;
  extern wxString *AuxVars;
  extern wxString *AuxTable;

  extern wxRangeSpec **RFSpecRMS;
  extern wxRangeSpec **RFSpecAbsolute;
  extern wxString *RFRefLabels;
  extern wxString *RFLabels;

  extern wxRangeSpec *EventRateSpec;
  extern wxString EventRateLabels;
  extern double *EventRateRange;
  extern ScaleType EventRateScaleType;
  extern unsigned int EventRateAverage;
  extern unsigned int EventRateStep;
  
  extern RFOverviewType RFOverview;
  extern RFViewType RFView;
  extern RFTabType RFTab;
  extern PayloadStyleType PayloadStyle; 
  extern RFReferenceType RFReference;

  extern wxColour L1MaskColour;
  extern wxColour L1PhiMaskColour;
  extern wxColour PhiMaskColour;
  extern wxColour PhiHMaskColour;
  extern wxColour L3TrigColour;
  extern wxColour L3HTrigColour;

  extern bool Compass;
  extern bool ClockRF;
  extern int tabScaleMin;
  extern int tabScaleMax;
  extern int clockTabScaleMin;
  extern int clockTabScaleMax;

  extern GPSType DefaultGPS;
  extern int readDelay;
  extern bool sqlDump;
  extern bool crcCheck;
  extern bool fetchRunNumber;

  extern wxString dbHost;
  extern wxString dbUser;
  extern wxString dbName;
  extern wxString dbPwd;
};

extern wxColour *wxYELLOW;

#endif // _OPTIONSH_
