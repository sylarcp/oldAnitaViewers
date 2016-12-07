/* 
 * trigmon_options.h
 */

#ifndef _TRIGMON_OPTIONSH_
#define _TRIGMON_OPTIONSH_

#include "trigmondefs.h"
#include "range.h"

namespace TrigMonOpts{
  enum RFPowerType {ADC,dBm,T};

  extern wxRangeSpec **L1MaskSpec;
  extern double **L1MaskRange;
  extern ScaleType *L1MaskScaleType;
  extern unsigned int *L1MaskAverage;
  extern wxString *L1MaskLabels;
  extern wxString *L1MaskCaptions;
  extern wxString *L1MaskVars;
  extern wxString *L1MaskTable;
  extern wxString L1MaskScale;

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
  extern wxString *ScalerRefLabels;
  extern wxString *ScalerCaptions;
  extern wxString *ScalerVars;
  extern wxString *ScalerTable;
  extern wxString ScalerScale;

  extern wxRangeSpec **ThresholdSpec;
  extern double **ThresholdRange;
  extern ScaleType *ThresholdScaleType;
  extern unsigned int *ThresholdAverage;
  extern wxString *ThresholdLabels;
  extern wxString *ThresholdRefLabels;
  extern wxString *ThresholdCaptions;
  extern wxString *ThresholdVars;
  extern wxString *ThresholdTable;
  extern wxString ThresholdScale;

  extern int turfReadDelay;
  extern int surfReadDelay;

  extern wxString dbHost;
  extern wxString dbUser;
  extern wxString dbName;
  extern wxString dbPwd;

  extern bool Lockstep;
  extern bool DynamicTarget;
  extern bool sqlDump;
  extern bool crcCheck;

  extern RFPowerType rfPowerType;
};

extern wxColour *wxYELLOW;

#endif // _TRIGMON_OPTIONSH_
