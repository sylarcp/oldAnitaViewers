/* 
 * slowmo_options.h
 */

#ifndef _SLOWMO_OPTIONSH_
#define _SLOWMO_OPTIONSH_

#include "range.h"
#include "slowmodefs.h"

namespace SlowMoOpts{
  enum RFPowerType {ADC,dBm,T};

  extern wxRangeSpec **EventRateSpec;
  extern double **EventRateRange;
  extern ScaleType *EventRateScaleType;
  extern unsigned int *EventRateAverage;
  extern wxString *EventRateLabels;
  extern wxString *EventRateCaptions;
  extern wxString *EventRateVars;
  extern wxString *EventRateTable;
  extern wxString EventRateScale;

  extern wxRangeSpec **TempSpec;
  extern double **TempRange;
  extern ScaleType *TempScaleType;
  extern unsigned int *TempAverage;
  extern wxString *TempLabels;
  extern wxString *TempCaptions;
  extern wxString *TempVars;
  extern wxString *TempTable;
  extern wxString TempScale;

  extern wxRangeSpec **PowerSpec;
  extern double **PowerRange;
  extern ScaleType *PowerScaleType;
  extern unsigned int *PowerAverage;
  extern wxString *PowerLabels;
  extern wxString *PowerCaptions;
  extern wxString *PowerVars;
  extern wxString *PowerTable;
  extern wxString PowerScale;

  extern wxRangeSpec **L1Spec;
  extern double **L1Range;
  extern ScaleType *L1ScaleType;
  extern unsigned int *L1Average;
  extern wxString *L1Labels;
  extern wxString *L1Captions;
  extern wxString *L1Vars;
  extern wxString *L1Table;
  extern wxString L1Scale;

  extern wxRangeSpec **L2Spec;
  extern double **L2Range;
  extern ScaleType *L2ScaleType;
  extern unsigned int *L2Average;
  extern wxString *L2Labels;
  extern wxString *L2Captions;
  extern wxString *L2Vars;
  extern wxString *L2Table;
  extern wxString L2Scale;

  extern wxRangeSpec **L3Spec;
  extern double **L3Range;
  extern ScaleType *L3ScaleType;
  extern unsigned int *L3Average;
  extern wxString *L3Labels;
  extern wxString *L3Captions;
  extern wxString *L3Vars;
  extern wxString *L3Table;
  extern wxString L3Scale;

  extern wxRangeSpec **RFPowerSpec;
  extern double **RFPowerRange;
  extern ScaleType *RFPowerScaleType;
  extern unsigned int *RFPowerAverage;
  extern wxString *RFPowerLabels;
  extern wxString *RFPowerCaptions;
  extern wxString *RFPowerVars;
  extern wxString *RFPowerTable;
  extern wxString RFPowerScale;

  extern wxRangeSpec **ScalerSpec;
  extern double **ScalerRange;
  extern ScaleType *ScalerScaleType;
  extern unsigned int *ScalerAverage;
  extern wxString *ScalerLabels;
  extern wxString *ScalerCaptions;
  extern wxString *ScalerVars;
  extern wxString *ScalerTable;
  extern wxString ScalerScale;

  extern wxRangeSpec **ScalerRMSSpec;
  extern double **ScalerRMSRange;
  extern ScaleType *ScalerRMSScaleType;
  extern unsigned int *ScalerRMSAverage;
  extern wxString *ScalerRMSLabels;
  extern wxString *ScalerRMSCaptions;
  extern wxString *ScalerRMSVars;
  extern wxString *ScalerRMSTable;
  extern wxString ScalerRMSScale;

  extern RFPowerType rfPowerType;
  extern int readDelay;

  extern wxString dbHost;
  extern wxString dbUser;
  extern wxString dbName;
  extern wxString dbPwd;

  extern bool sqlDump;
};

extern wxColour *wxYELLOW;

#endif // _SLOWMO_OPTIONSH_
