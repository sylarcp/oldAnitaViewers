/* 
 * slowmo_defaults.h
 */

#ifndef _SLOWMO_DEFAULTSH_
#define _SLOWMO_DEFAULTSH_

#include "slowmo_options.h"

namespace SlowMoOpts{
  wxRangeSpec **EventRateSpec;
  double **EventRateRange;
  ScaleType *EventRateScaleType;
  unsigned int *EventRateAverage;
  wxString *EventRateLabels;
  wxString *EventRateCaptions;
  wxString *EventRateVars;
  wxString *EventRateTable;
  wxString EventRateScale;

  wxRangeSpec **TempSpec;
  double **TempRange;
  ScaleType *TempScaleType;
  unsigned int *TempAverage;
  wxString *TempLabels;
  wxString *TempCaptions;
  wxString *TempVars;
  wxString *TempTable;
  wxString TempScale;

  wxRangeSpec **PowerSpec;
  double **PowerRange;
  ScaleType *PowerScaleType;
  unsigned int *PowerAverage;
  wxString *PowerLabels;
  wxString *PowerCaptions;
  wxString *PowerVars;
  wxString *PowerTable;
  wxString PowerScale;

  wxRangeSpec **L1Spec;
  double **L1Range;
  ScaleType *L1ScaleType;
  unsigned int *L1Average;
  wxString *L1Labels;
  wxString *L1Captions;
  wxString *L1Vars;
  wxString *L1Table;
  wxString L1Scale;

  wxRangeSpec **L2Spec;
  double **L2Range;
  ScaleType *L2ScaleType;
  unsigned int *L2Average;
  wxString *L2Labels;
  wxString *L2Captions;
  wxString *L2Vars;
  wxString *L2Table;
  wxString L2Scale;

  wxRangeSpec **L3Spec;
  double **L3Range;
  ScaleType *L3ScaleType;
  unsigned int *L3Average;
  wxString *L3Labels;
  wxString *L3Captions;
  wxString *L3Vars;
  wxString *L3Table;
  wxString L3Scale;

  wxRangeSpec **RFPowerSpec;
  double **RFPowerRange;
  ScaleType *RFPowerScaleType;
  unsigned int *RFPowerAverage;
  wxString *RFPowerLabels;
  wxString *RFPowerCaptions;
  wxString *RFPowerVars;
  wxString *RFPowerTable;
  wxString RFPowerScale;

  wxRangeSpec **ScalerSpec;
  double **ScalerRange;
  ScaleType *ScalerScaleType;
  unsigned int *ScalerAverage;
  wxString *ScalerLabels;
  wxString *ScalerCaptions;
  wxString *ScalerVars;
  wxString *ScalerTable;
  wxString ScalerScale;

  wxRangeSpec **ScalerRMSSpec;
  double **ScalerRMSRange;
  ScaleType *ScalerRMSScaleType;
  unsigned int *ScalerRMSAverage;
  wxString *ScalerRMSLabels;
  wxString *ScalerRMSCaptions;
  wxString *ScalerRMSVars;
  wxString *ScalerRMSTable;
  wxString ScalerRMSScale;

  int readDelay=2;  
  RFPowerType rfPowerType=ADC;
   
  wxString dbHost=_T("128.175.112.125");
  wxString dbUser=_T("gui");
  wxString dbName=_T("anita_0902d");
  wxString dbPwd=_T("AniTa08");

  bool sqlDump=false;
};

wxColour *wxYELLOW=new wxColour(218,165,32);

#endif // _SLOWMO_DEFAULTSH_
