/* 
 * trigmon_defaults.h
 */

#ifndef _TRIGMON_DEFAULTSH_
#define _TRIGMON_DEFAULTSH_

#include "trigmon_options.h"

namespace TrigMonOpts{

   wxRangeSpec **L1MaskSpec;
   double **L1MaskRange;
   ScaleType *L1MaskScaleType;
   unsigned int *L1MaskAverage;
   wxString *L1MaskLabels;
   wxString *L1MaskCaptions;
   wxString *L1MaskVars;
   wxString *L1MaskTable;
   wxString L1MaskScale;

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
   wxString *ScalerRefLabels;
   wxString *ScalerCaptions;
   wxString *ScalerVars;
   wxString *ScalerTable;
   wxString ScalerScale;

   wxRangeSpec **ThresholdSpec;
   double **ThresholdRange;
   ScaleType *ThresholdScaleType;
   unsigned int *ThresholdAverage;
   wxString *ThresholdLabels;
   wxString *ThresholdRefLabels;
   wxString *ThresholdCaptions;
   wxString *ThresholdVars;
   wxString *ThresholdTable;
   wxString ThresholdScale;

   int turfReadDelay=2;  
   int surfReadDelay=2;
   
   wxString dbHost=_T("128.175.14.242");
   wxString dbUser=_T("gui");
   wxString dbName=_T("anita_MMDDx");
   wxString dbPwd=_T("AniTa08");

   bool Lockstep=false;
   bool DynamicTarget=true;
   bool sqlDump=false;
   bool crcCheck=true;

   RFPowerType rfPowerType=dBm;
};

wxColour *wxYELLOW=new wxColour(218,165,32);

#endif // _TRIGMON_DEFAULTSH_
