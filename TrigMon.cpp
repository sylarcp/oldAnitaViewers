/*
 * TrigMon.cpp
 *
 * Definitions of global variables for TrigMon program, and also launches
 * TrigMon.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TrigMon.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "trigmon_defaults.h"
#include "TrigMon.h"
#include "TrigMonFrame.h"

/*
 * Application instance implementation
 */
IMPLEMENT_APP( TrigMonApp )

/*
 * Constructor for TrigMonApp
 */
TrigMonApp::TrigMonApp(){}

/*
 * Initialisation for TrigMonApp
 */
bool TrigMonApp::OnInit(){    
  using namespace TrigMonOpts;
  // Initialize default options
/*
  // L1 Mask 
  L1MaskSpec = new wxRangeSpec*[N_L1];
  L1MaskRange = new double*[N_L1];
  L1MaskScaleType = new ScaleType[N_L1];
  L1MaskAverage = new unsigned int[N_L1];
  L1MaskLabels = new wxString[N_L1];
  L1MaskCaptions = new wxString[N_L1];
  L1MaskVars = new wxString[N_L1];
  L1MaskTable = new wxString[N_L1];
  L1MaskScale = _T("0,7");
  for(int i=0;i<N_L1;++i){
    L1MaskSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    L1MaskRange[i]=new double[2];
    L1MaskScaleType[i]=Log;
    L1MaskAverage[i]=1;
    
    L1MaskLabels[i].Printf( _T("%02d"),(i%L1_PER_ROW)*(N_L1/L1_PER_ROW)+(i/L1_PER_ROW+1));
    L1MaskCaptions[i].Printf( _T("Antenna %d L1 rate"),(i%L1_PER_ROW)*(N_L1/L1_PER_ROW)+(i/L1_PER_ROW+1));
    L1MaskVars[i].Printf(_T("l1[%d][%d]"),i%L1_PER_ROW+1,i/L1_PER_ROW+1);
    L1MaskTable[i]=_T("turf");
    L1MaskSpec[i][0]=wxRangeSpec(2000,7000,wxGREEN);
    L1MaskSpec[i][1]=wxRangeSpec(1000,10000,wxYELLOW);
    L1MaskSpec[i][2]=wxRangeSpec();
    L1MaskRange[i][0]=0;
    L1MaskRange[i][1]=15000;
  }
*/
 // L1 Mask
  L1MaskSpec = new wxRangeSpec*[N_L1];
  L1MaskRange = new double*[N_L1];
  L1MaskScaleType = new ScaleType[N_L1];
  L1MaskAverage = new unsigned int[N_L1];
  L1MaskLabels = new wxString[N_L1];
  L1MaskCaptions = new wxString[N_L1];
  L1MaskVars = new wxString[N_L1];
  L1MaskTable = new wxString[N_L1];
  L1MaskScale = _T("0,1");

  for(int i=0;i<N_L1;++i){
    L1MaskSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    L1MaskRange[i]=new double[2];
    L1MaskScaleType[i]=Linear;
    L1MaskAverage[i]=1;

    L1MaskLabels[i].Printf(_T("%d"),i+1);
    //L1Labels[i].Printf(_T("%02d"),i+1);
    L1MaskCaptions[i].Printf(_T("Sector %d L1 Mask"),i+1);
    L1MaskVars[i].Printf(_T("l1[%d]"),i+1);
    L1MaskTable[i]=_T("turf");
    L1MaskSpec[i][0]=wxRangeSpec(-0.5,0.5,wxGREEN);
    L1MaskSpec[i][1]=wxRangeSpec(0.5,0.5,wxYELLOW);
    L1MaskSpec[i][2]=wxRangeSpec();
    L1MaskRange[i][0]=0;
    L1MaskRange[i][1]=1;
  }
 // L1 rates
  L1Spec = new wxRangeSpec*[N_L1];
  L1Range = new double*[N_L1];
  L1ScaleType = new ScaleType[N_L1];
  L1Average = new unsigned int[N_L1];
  L1Labels = new wxString[N_L1];
  L1Captions = new wxString[N_L1];
  L1Vars = new wxString[N_L1];
  L1Table = new wxString[N_L1];
  L1Scale = _T("0,65535");

  for(int i=0;i<N_L1;++i){
    L1Spec[i]=new wxRangeSpec[MAX_SPECS+1];
    L1Range[i]=new double[2];
    L1ScaleType[i]=Linear;
    L1Average[i]=1;

    L1Labels[i].Printf(_T("%d"),i+1);
    //L1Labels[i].Printf(_T("%02d"),i+1);
    L1Captions[i].Printf(_T("Sector %d L1 rate"),i+1);
    L1Vars[i].Printf(_T("l1[%d]"),i+1);
    L1Table[i]=_T("turf");
    L1Spec[i][0]=wxRangeSpec(5000,30000,wxGREEN);
    L1Spec[i][1]=wxRangeSpec(2000,50000,wxYELLOW);
    L1Spec[i][2]=wxRangeSpec();
    L1Range[i][0]=0;
    L1Range[i][1]=65535;
  }
  // L2 rates
  L2Spec = new wxRangeSpec*[N_L2];
  L2Range = new double*[N_L2];
  L2ScaleType = new ScaleType[N_L2];
  L2Average = new unsigned int[N_L2];
  L2Labels = new wxString[N_L2];
  L2Captions = new wxString[N_L2];
  L2Vars = new wxString[N_L2];
  L2Table = new wxString[N_L2];
  //L2Scale = _T("0,3");
   L2Scale = _T("0,4.5");
  for(int i=0;i<(N_L2);i++){
  //L2Labels[i].Printf(_T("%d"),i+1);
  if(i<N_L2-16){
  L2Labels[i].Printf(_T("%c%02d"),i<(N_L2-16)/2?'u':'l',i%N_PHI_SECTORS+1);//amir's commentation
  	}
  if(i>=(N_L2-16) & i<(N_L2-8)){
  	 L2Labels[i].Printf(_T("%c%02d"),'N',i+1);//amir's commentation
  	}
  if(i>=N_L2-8){
  	L2Labels[i].Printf(_T("%c"),'E');//amir's commentation
  	}
  	}
  for(int i=0;i<N_L2;++i){
    L2Spec[i]=new wxRangeSpec[MAX_SPECS+1];
    L2Range[i]=new double[2];
    L2ScaleType[i]=Log;
    L2Average[i]=1;

   if(i<N_L2-16){
    L2Captions[i].Printf(_T("%s Phi Sector %02d L2 rate"),i<N_L2/2?"Upper":"Lower",i%N_PHI_SECTORS+1);
   	}
   if(i>=(N_L2-16) & i<(N_L2-8)){
  	 L2Captions[i].Printf(_T("Nadir %c%02d"),'N',i+1);//amir's commentation
  	}
   if(i>=N_L2-8){
  	L2Captions[i].Printf(_T("Empty"));//amir's commentation
  	}
    L2Vars[i].Printf(_T("l2[%d][%d]"),i/L2_PER_ROW+1,i%L2_PER_ROW+1);
    L2Table[i]=_T("turf");
    L2Spec[i][0]=wxRangeSpec(3,3.5,wxGREEN);
    L2Spec[i][1]=wxRangeSpec(2,4,wxYELLOW);
    L2Spec[i][2]=wxRangeSpec();
    L2Range[i][0]=0;
    L2Range[i][1]=4.5;
  }
  // L3 rates
  L3Spec = new wxRangeSpec*[N_L3];
  L3Range = new double*[N_L3];
  L3ScaleType = new ScaleType[N_L3];
  L3Average = new unsigned int[N_L3];
  L3Labels = new wxString[N_L3];
  L3Captions = new wxString[N_L3];
  L3Vars = new wxString[N_L3];
  L3Table = new wxString[N_L3];
  L3Scale = _T("0,255");
  for(int i=0;i<N_L3;++i){
    L3Spec[i]=new wxRangeSpec[MAX_SPECS+1];
    L3Range[i]=new double[2];
    L3ScaleType[i]=Linear;
    L3Average[i]=1;

    L3Labels[i].Printf(_T("%d"),i+1);
    L3Captions[i].Printf(_T("Sector %d L3 rate"),i+1);
    L3Vars[i].Printf(_T("l3[%d]"),i+1);
    L3Table[i]=_T("turf");
    L3Spec[i][0]=wxRangeSpec(4,125,wxGREEN);
    L3Spec[i][1]=wxRangeSpec(2,170,wxYELLOW);
    L3Spec[i][2]=wxRangeSpec();
    L3Range[i][0]=0;
    L3Range[i][1]=255;
  }

  // RF power 
  RFPowerSpec = new wxRangeSpec*[N_RFPOWER];
  RFPowerRange = new double*[N_RFPOWER];
  RFPowerScaleType = new ScaleType[N_RFPOWER];
  RFPowerAverage = new unsigned int[N_RFPOWER];
  RFPowerLabels = new wxString[N_RFPOWER];
  RFPowerCaptions = new wxString[N_RFPOWER];
  RFPowerVars = new wxString[N_RFPOWER];
  RFPowerTable = new wxString[N_RFPOWER];
  RFPowerScale = _T("-100,600 ,1200");
  for(int i=0;i<N_RFPOWER;++i){
    RFPowerSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    RFPowerRange[i]=new double[2];
    RFPowerScaleType[i]=Linear;
    RFPowerAverage[i]=1;

    RFPowerVars[i].Printf(_T("rfpow[%d][%d]"),i/RFPOWER_PER_ROW+1,
				       i%RFPOWER_PER_ROW+1);
    RFPowerTable[i]=_T("hk_surf");
    RFPowerSpec[i][0]=wxRangeSpec(0,400,wxBLUE);
    RFPowerSpec[i][1]=wxRangeSpec(400,800,wxGREEN);
    RFPowerSpec[i][2]=wxRangeSpec(800,1200,wxYELLOW);
    RFPowerSpec[i][3]=wxRangeSpec();
     RFPowerRange[i][0]=-100;
    RFPowerRange[i][1]=1200;

  }
//   RFPowerLabels[0]=_T("9V");
//   RFPowerLabels[1]=_T("13V");
//   RFPowerLabels[2]=_T("17V");
//   RFPowerLabels[3]=_T("25V");
//   RFPowerLabels[4]=_T("9H");
//   RFPowerLabels[5]=_T("13H");
//   RFPowerLabels[6]=_T("17H");
//   RFPowerLabels[7]=_T("25H");
//   ////////////////////////////////////////////
//   RFPowerLabels[8]=_T("4V");
//   RFPowerLabels[9]=_T("8V");
//   RFPowerLabels[10]=_T("24V");
//   RFPowerLabels[11]=_T("32V");
//   RFPowerLabels[12]=_T("4H");
//   RFPowerLabels[13]=_T("8H");
//   RFPowerLabels[14]=_T("24H");
//   RFPowerLabels[15]=_T("32H");
//   ////////////////////////////////////////////////
//   RFPowerLabels[16]=_T("10V");
//   RFPowerLabels[17]=_T("14V");
//   RFPowerLabels[18]=_T("19V");
//   RFPowerLabels[19]=_T("27V");
//   RFPowerLabels[20]=_T("10H");
//   RFPowerLabels[21]=_T("14H");
//   RFPowerLabels[22]=_T("19H");
//   RFPowerLabels[23]=_T("27H");
//   ///////////////////////////////////////////////
//   RFPowerLabels[24]=_T("11V");
//   RFPowerLabels[25]=_T("15V");
//   RFPowerLabels[26]=_T("21V");
//   RFPowerLabels[27]=_T("29V");
//   RFPowerLabels[28]=_T("11H");
//   RFPowerLabels[29]=_T("15H");
//   RFPowerLabels[30]=_T("21H");
//   RFPowerLabels[31]=_T("29H");
//   ///////////////////////////////////////////////
//   RFPowerLabels[32]=_T("12V");
//   RFPowerLabels[33]=_T("16V");
//   RFPowerLabels[34]=_T("23V");
//   RFPowerLabels[35]=_T("31V");
//   RFPowerLabels[36]=_T("12H");
//   RFPowerLabels[37]=_T("16H");
//   RFPowerLabels[38]=_T("23H");
//   RFPowerLabels[39]=_T("31H");
//   ////////////////////////////////////////////////
//   RFPowerLabels[40]=_T("1V");
//   RFPowerLabels[41]=_T("5V");
//   RFPowerLabels[42]=_T("18V");
//   RFPowerLabels[43]=_T("26V");
//   RFPowerLabels[44]=_T("1H");
//   RFPowerLabels[45]=_T("5H");
//   RFPowerLabels[46]=_T("18H");
//   RFPowerLabels[47]=_T("26H");
//   ///////////////////////////////////////////
//   RFPowerLabels[48]=_T("3V");
//   RFPowerLabels[49]=_T("7V");
//   RFPowerLabels[50]=_T("22V");
//   RFPowerLabels[51]=_T("30V");
//   RFPowerLabels[52]=_T("3H");
//   RFPowerLabels[53]=_T("7H");
//   RFPowerLabels[54]=_T("22H");
//   RFPowerLabels[55]=_T("30H");
//   /////////////////////////////////////////
//   RFPowerLabels[56]=_T("2V");
//   RFPowerLabels[57]=_T("6V");
//   RFPowerLabels[58]=_T("20V");
//   RFPowerLabels[59]=_T("28V");
//   RFPowerLabels[60]=_T("2H");
//   RFPowerLabels[61]=_T("6H");
//   RFPowerLabels[62]=_T("20H");
//   RFPowerLabels[63]=_T("28H");
//   /////////////////////////////////////////
//   RFPowerLabels[64]=_T("N1V");
//   RFPowerLabels[65]=_T("N3V");
//   RFPowerLabels[66]=_T("N5V");
//   RFPowerLabels[67]=_T("N7V");
//   RFPowerLabels[68]=_T("N1H");
//   RFPowerLabels[69]=_T("N3H");
//   RFPowerLabels[70]=_T("N5H");
//   RFPowerLabels[71]=_T("N7H");
//   /////////////////////////////////////////
//   RFPowerLabels[72]=_T("N2V");
//   RFPowerLabels[73]=_T("N4V");
//   RFPowerLabels[74]=_T("N6V");
//   RFPowerLabels[75]=_T("N8V");
//   RFPowerLabels[76]=_T("N2H");
//   RFPowerLabels[77]=_T("N4H");
//   RFPowerLabels[78]=_T("N6H");
//   RFPowerLabels[79]=_T("N8H");
  
  ///////////// SURF 1 ///////////////////////////////
  RFPowerLabels[0]=_T("10BV");
  RFPowerLabels[1]=_T("2BV");
  RFPowerLabels[2]=_T("16BV");
  RFPowerLabels[3]=_T("8BV");
  RFPowerLabels[4]=_T("10BH");
  RFPowerLabels[5]=_T("2BH");
  RFPowerLabels[6]=_T("16BH");
  RFPowerLabels[7]=_T("8BH");
  ///////////// SURF 2 ///////////////////////////////
  RFPowerLabels[8]=_T("12BV");
  RFPowerLabels[9]=_T("4BV");
  RFPowerLabels[10]=_T("14BV");
  RFPowerLabels[11]=_T("6BV");
  RFPowerLabels[12]=_T("12BH");
  RFPowerLabels[13]=_T("4BH");
  RFPowerLabels[14]=_T("14BH");
  RFPowerLabels[15]=_T("6BH");
  ///////////// SURF 3 ///////////////////////////////////
  RFPowerLabels[16]=_T("16MV");
  RFPowerLabels[17]=_T("8MV");
  RFPowerLabels[18]=_T("12MV");
  RFPowerLabels[19]=_T("4MV");
  RFPowerLabels[20]=_T("16MH");
  RFPowerLabels[21]=_T("8MH");
  RFPowerLabels[22]=_T("12MH");
  RFPowerLabels[23]=_T("4MH");
  ///////////// SURF 4 //////////////////////////////////
  RFPowerLabels[24]=_T("14MV");
  RFPowerLabels[25]=_T("6MV");
  RFPowerLabels[26]=_T("10MV");
  RFPowerLabels[27]=_T("2MV");
  RFPowerLabels[28]=_T("14MH");
  RFPowerLabels[29]=_T("6MH");
  RFPowerLabels[30]=_T("10MH");
  RFPowerLabels[31]=_T("2MH");
  ///////////// SURF 5 //////////////////////////////////
  RFPowerLabels[32]=_T("12TV");
  RFPowerLabels[33]=_T("4TV");
  RFPowerLabels[34]=_T("14TV");
  RFPowerLabels[35]=_T("6TV");
  RFPowerLabels[36]=_T("12TH");
  RFPowerLabels[37]=_T("4TH");
  RFPowerLabels[38]=_T("14TH");
  RFPowerLabels[39]=_T("6TH");
  ///////////// SURF 6 ///////////////////////////////////
  RFPowerLabels[40]=_T("10TV");
  RFPowerLabels[41]=_T("2TV");
  RFPowerLabels[42]=_T("16TV");
  RFPowerLabels[43]=_T("8TV");
  RFPowerLabels[44]=_T("10TH");
  RFPowerLabels[45]=_T("2TH");
  RFPowerLabels[46]=_T("16TH");
  RFPowerLabels[47]=_T("8TH");
  ///////////// SURF 7 //////////////////////////////
  RFPowerLabels[48]=_T("13BV");
  RFPowerLabels[49]=_T("5BV");
  RFPowerLabels[50]=_T("9BV");
  RFPowerLabels[51]=_T("1BV");
  RFPowerLabels[52]=_T("13BH");
  RFPowerLabels[53]=_T("5BH");
  RFPowerLabels[54]=_T("9BH");
  RFPowerLabels[55]=_T("1BH");
  ///////////// SURF 8 ////////////////////////////
  RFPowerLabels[56]=_T("15BV");
  RFPowerLabels[57]=_T("7BV");
  RFPowerLabels[58]=_T("11BV");
  RFPowerLabels[59]=_T("3BV");
  RFPowerLabels[60]=_T("15BH");
  RFPowerLabels[61]=_T("7BH");
  RFPowerLabels[62]=_T("11BH");
  RFPowerLabels[63]=_T("3BH");
  ///////////// SURF 9 ////////////////////////////
  RFPowerLabels[64]=_T("11MV");
  RFPowerLabels[65]=_T("3MV");
  RFPowerLabels[66]=_T("13MV");
  RFPowerLabels[67]=_T("5MV");
  RFPowerLabels[68]=_T("11MH");
  RFPowerLabels[69]=_T("3MH");
  RFPowerLabels[70]=_T("13MH");
  RFPowerLabels[71]=_T("5MH");
  ///////////// SURF 10 ////////////////////////////
  RFPowerLabels[72]=_T("9MV");
  RFPowerLabels[73]=_T("1MV");
  RFPowerLabels[74]=_T("15MV");
  RFPowerLabels[75]=_T("7MV");
  RFPowerLabels[76]=_T("9MH");
  RFPowerLabels[77]=_T("1MH");
  RFPowerLabels[78]=_T("15MH");
  RFPowerLabels[79]=_T("7MH");
  ///////////// SURF 11 ////////////////////////////
  RFPowerLabels[80]=_T("15TV");
  RFPowerLabels[81]=_T("7TV");
  RFPowerLabels[82]=_T("11TV");
  RFPowerLabels[83]=_T("3TV");
  RFPowerLabels[84]=_T("15TH");
  RFPowerLabels[85]=_T("7TH");
  RFPowerLabels[86]=_T("11TH");
  RFPowerLabels[87]=_T("3TH");
  ///////////// SURF 12 ////////////////////////////
  RFPowerLabels[88]=_T("13TV");
  RFPowerLabels[89]=_T("5TV");
  RFPowerLabels[90]=_T("9TV");
  RFPowerLabels[91]=_T("1TV");
  RFPowerLabels[92]=_T("13TH");
  RFPowerLabels[93]=_T("5TH");
  RFPowerLabels[94]=_T("9TH");
  RFPowerLabels[95]=_T("1TH");

  for(int i=0;i<N_RFPOWER;++i)
    RFPowerCaptions[i] << _T("RF power in channel ") << RFPowerLabels[i];

  // Scalers
  ScalerSpec = new wxRangeSpec*[N_SCALER];
  ScalerRange = new double*[N_SCALER];
  ScalerScaleType = new ScaleType[N_SCALER];
  ScalerAverage = new unsigned int[N_SCALER];
  ScalerLabels = new wxString[N_SCALER];
  ScalerRefLabels = new wxString[N_SCALER];
  ScalerCaptions = new wxString[N_SCALER];
  ScalerVars = new wxString[N_SCALER];
  ScalerTable = new wxString[N_SCALER];
  ScalerScale = _T("0,8");
 // const char bandLabel[]={'l','m','h','f'};
  /////////////////////////////////////////////////////////////////////
  /* 
  ScalerLabels[0].Printf(_T("1L"));
  ScalerLabels[1].Printf(_T("1M"));
  ScalerLabels[2].Printf(_T("1H"));
  ScalerLabels[3].Printf(_T("1F"));

  ScalerLabels[4].Printf(_T("9L"));
  ScalerLabels[5].Printf(_T("9M"));
  ScalerLabels[6].Printf(_T("9H"));
  ScalerLabels[7].Printf(_T("9F"));

  ScalerLabels[8].Printf(_T("17L"));
  ScalerLabels[9].Printf(_T("17M"));
  ScalerLabels[10].Printf(_T("17H"));
  ScalerLabels[11].Printf(_T("17F"));

  ScalerLabels[12].Printf(_T("18L"));
  ScalerLabels[13].Printf(_T("18M"));
  ScalerLabels[14].Printf(_T("18H"));
  ScalerLabels[15].Printf(_T("18F"));
  */
  //////////////////////////////////////////////////////////////////////
  /*
  ScalerLabels[16].Printf(_T("2L"));
  ScalerLabels[17].Printf(_T("2M"));
  ScalerLabels[18].Printf(_T("2H"));
  ScalerLabels[19].Printf(_T("2F"));

  ScalerLabels[20].Printf(_T("10L"));
  ScalerLabels[21].Printf(_T("10M"));
  ScalerLabels[22].Printf(_T("10H"));
  ScalerLabels[23].Printf(_T("10F"));

  ScalerLabels[24].Printf(_T("19L"));
  ScalerLabels[25].Printf(_T("19M"));
  ScalerLabels[26].Printf(_T("19H"));
  ScalerLabels[27].Printf(_T("19F"));

  ScalerLabels[28].Printf(_T("20L"));
  ScalerLabels[29].Printf(_T("20M"));
  ScalerLabels[30].Printf(_T("20H"));
  ScalerLabels[31].Printf(_T("20F"));
  */
////////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[32].Printf(_T("3L"));
  ScalerLabels[33].Printf(_T("3M"));
  ScalerLabels[34].Printf(_T("3H"));
  ScalerLabels[35].Printf(_T("3F"));

  ScalerLabels[36].Printf(_T("11L"));
  ScalerLabels[37].Printf(_T("11M"));
  ScalerLabels[38].Printf(_T("11H"));
  ScalerLabels[39].Printf(_T("11F"));

  ScalerLabels[40].Printf(_T("21L"));
  ScalerLabels[41].Printf(_T("21M"));
  ScalerLabels[42].Printf(_T("21H"));
  ScalerLabels[43].Printf(_T("21F"));

  ScalerLabels[44].Printf(_T("22L"));
  ScalerLabels[45].Printf(_T("22M"));
  ScalerLabels[46].Printf(_T("22H"));
  ScalerLabels[47].Printf(_T("22F"));
  */
//////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[48].Printf(_T("4L"));
  ScalerLabels[49].Printf(_T("4M"));
  ScalerLabels[50].Printf(_T("4H"));
  ScalerLabels[51].Printf(_T("4F"));

  ScalerLabels[52].Printf(_T("12L"));
  ScalerLabels[53].Printf(_T("12M"));
  ScalerLabels[54].Printf(_T("12H"));
  ScalerLabels[55].Printf(_T("12F"));

  ScalerLabels[56].Printf(_T("23L"));
  ScalerLabels[57].Printf(_T("23M"));
  ScalerLabels[58].Printf(_T("23H"));
  ScalerLabels[59].Printf(_T("23F"));

  ScalerLabels[60].Printf(_T("24L"));
  ScalerLabels[61].Printf(_T("24M"));
  ScalerLabels[62].Printf(_T("24H"));
  ScalerLabels[63].Printf(_T("24F"));
  */
//////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[64].Printf(_T("5L"));
  ScalerLabels[65].Printf(_T("5M"));
  ScalerLabels[66].Printf(_T("5H"));
  ScalerLabels[67].Printf(_T("5F"));

  ScalerLabels[68].Printf(_T("13L"));
  ScalerLabels[69].Printf(_T("13M"));
  ScalerLabels[70].Printf(_T("13H"));
  ScalerLabels[71].Printf(_T("13F"));

  ScalerLabels[72].Printf(_T("25L"));
  ScalerLabels[73].Printf(_T("25M"));
  ScalerLabels[74].Printf(_T("25H"));
  ScalerLabels[75].Printf(_T("25F"));

  ScalerLabels[76].Printf(_T("26L"));
  ScalerLabels[77].Printf(_T("26M"));
  ScalerLabels[78].Printf(_T("26H"));
  ScalerLabels[79].Printf(_T("26F"));
  */
///////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[80].Printf(_T("6L"));
  ScalerLabels[81].Printf(_T("6M"));
  ScalerLabels[82].Printf(_T("6H"));
  ScalerLabels[83].Printf(_T("6F"));

  ScalerLabels[84].Printf(_T("14L"));
  ScalerLabels[85].Printf(_T("14M"));
  ScalerLabels[86].Printf(_T("14H"));
  ScalerLabels[87].Printf(_T("14F"));

  ScalerLabels[88].Printf(_T("27L"));
  ScalerLabels[89].Printf(_T("27M"));
  ScalerLabels[90].Printf(_T("27H"));
  ScalerLabels[91].Printf(_T("27F"));

  ScalerLabels[92].Printf(_T("28L"));
  ScalerLabels[93].Printf(_T("28M"));
  ScalerLabels[94].Printf(_T("28H"));
  ScalerLabels[95].Printf(_T("28F"));
  */
////////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[96].Printf(_T("7L"));
  ScalerLabels[97].Printf(_T("7M"));
  ScalerLabels[98].Printf(_T("7H"));
  ScalerLabels[99].Printf(_T("7F"));

  ScalerLabels[100].Printf(_T("15L"));
  ScalerLabels[101].Printf(_T("15M"));
  ScalerLabels[102].Printf(_T("15H"));
  ScalerLabels[103].Printf(_T("15F"));

  ScalerLabels[104].Printf(_T("29L"));
  ScalerLabels[105].Printf(_T("29M"));
  ScalerLabels[106].Printf(_T("29H"));
  ScalerLabels[107].Printf(_T("29F"));

  ScalerLabels[108].Printf(_T("30L"));
  ScalerLabels[109].Printf(_T("30M"));
  ScalerLabels[110].Printf(_T("30H"));
  ScalerLabels[111].Printf(_T("30F"));
  */
////////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[112].Printf(_T("8L"));
  ScalerLabels[113].Printf(_T("8M"));
  ScalerLabels[114].Printf(_T("8H"));
  ScalerLabels[115].Printf(_T("8F"));

  ScalerLabels[116].Printf(_T("16L"));
  ScalerLabels[117].Printf(_T("16M"));
  ScalerLabels[118].Printf(_T("16H"));
  ScalerLabels[119].Printf(_T("16F"));

  ScalerLabels[120].Printf(_T("31L"));
  ScalerLabels[121].Printf(_T("31M"));
  ScalerLabels[122].Printf(_T("31H"));
  ScalerLabels[123].Printf(_T("31F"));

  ScalerLabels[124].Printf(_T("32L"));
  ScalerLabels[125].Printf(_T("32M"));
  ScalerLabels[126].Printf(_T("32H"));
  ScalerLabels[127].Printf(_T("32F"));
  */
/////////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[128].Printf(_T("33L"));
  ScalerLabels[129].Printf(_T("33M"));
  ScalerLabels[130].Printf(_T("33H"));
  ScalerLabels[131].Printf(_T("33F"));

  ScalerLabels[132].Printf(_T("34L"));
  ScalerLabels[133].Printf(_T("34M"));
  ScalerLabels[134].Printf(_T("34H"));
  ScalerLabels[135].Printf(_T("34F"));

  ScalerLabels[136].Printf(_T("35L"));
  ScalerLabels[137].Printf(_T("35M"));
  ScalerLabels[138].Printf(_T("35H"));
  ScalerLabels[139].Printf(_T("35F"));

  ScalerLabels[140].Printf(_T("36L"));
  ScalerLabels[141].Printf(_T("36M"));
  ScalerLabels[142].Printf(_T("36H"));
  ScalerLabels[143].Printf(_T("36F"));
  */
//////////////////////////////////////////////////////////////////////////////////////
/*
  ScalerLabels[144].Printf(_T("37L"));
  ScalerLabels[145].Printf(_T("37M"));
  ScalerLabels[146].Printf(_T("37H"));
  ScalerLabels[147].Printf(_T("37F"));

  ScalerLabels[148].Printf(_T("38L"));
  ScalerLabels[149].Printf(_T("38M"));
  ScalerLabels[150].Printf(_T("38H"));
  ScalerLabels[151].Printf(_T("38F"));

  ScalerLabels[152].Printf(_T("39L"));
  ScalerLabels[153].Printf(_T("39M"));
  ScalerLabels[154].Printf(_T("39H"));
  ScalerLabels[155].Printf(_T("39F"));

  ScalerLabels[156].Printf(_T("40L"));
  ScalerLabels[157].Printf(_T("40M"));
  ScalerLabels[158].Printf(_T("40H"));
  ScalerLabels[159].Printf(_T("40F"));
  */
  //////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
  
  ScalerLabels[0].Printf(_T("nu01"));
  ScalerLabels[1].Printf(_T("nu02"));
  ScalerLabels[2].Printf(_T("nu03"));
  ScalerLabels[3].Printf(_T("nu04"));

  ScalerLabels[4].Printf(_T("nu05"));
  ScalerLabels[5].Printf(_T("nu06"));
  ScalerLabels[6].Printf(_T("nu07"));
  ScalerLabels[7].Printf(_T("nu08"));

  ScalerLabels[8].Printf(_T("nu09"));
  ScalerLabels[9].Printf(_T("nu10"));
  ScalerLabels[10].Printf(_T("nu11"));
  ScalerLabels[11].Printf(_T("nu12"));

///// 1 //////////////////////////////////////////////////////////////

  ScalerLabels[12].Printf(_T("nu13"));
  ScalerLabels[13].Printf(_T("nu14"));
  ScalerLabels[14].Printf(_T("nu15"));
  ScalerLabels[15].Printf(_T("nu16"));

  ScalerLabels[16].Printf(_T("nu17"));
  ScalerLabels[17].Printf(_T("nu18"));
  ScalerLabels[18].Printf(_T("nu19"));
  ScalerLabels[19].Printf(_T("nu20"));

  ScalerLabels[20].Printf(_T("nu21"));
  ScalerLabels[21].Printf(_T("nu22"));
  ScalerLabels[22].Printf(_T("nu23"));
  ScalerLabels[23].Printf(_T("nu24"));

//// 2 //////////////////////////////////////////////////////////

  ScalerLabels[24].Printf(_T("1TV"));
  ScalerLabels[25].Printf(_T("1TH"));
  ScalerLabels[26].Printf(_T("1MV"));
  ScalerLabels[27].Printf(_T("1MH"));
  ScalerLabels[28].Printf(_T("1BV"));
  ScalerLabels[29].Printf(_T("1BH"));
			      				  
  ScalerLabels[30].Printf(_T("5TV"));
  ScalerLabels[31].Printf(_T("5TH"));
  ScalerLabels[32].Printf(_T("5MV"));
  ScalerLabels[33].Printf(_T("5MH"));
  ScalerLabels[34].Printf(_T("5BV"));
  ScalerLabels[35].Printf(_T("5BH"));
//// 3 //////////////////////////////////////////////////////////////////

  ScalerLabels[36].Printf(_T("3TV"));
  ScalerLabels[37].Printf(_T("3TH"));
  ScalerLabels[38].Printf(_T("3MV"));
  ScalerLabels[39].Printf(_T("3MH"));
  ScalerLabels[40].Printf(_T("3BV"));
  ScalerLabels[41].Printf(_T("3BH"));
			      	  			  
  ScalerLabels[42].Printf(_T("7TV"));
  ScalerLabels[43].Printf(_T("7TH"));
  ScalerLabels[44].Printf(_T("7MV"));
  ScalerLabels[45].Printf(_T("7MH"));
  ScalerLabels[46].Printf(_T("7BV"));
  ScalerLabels[47].Printf(_T("7BH"));
//// 4 ///////////////////////////////////////////////////////////////

  ScalerLabels[48].Printf(_T("2TV"));
  ScalerLabels[49].Printf(_T("2TH"));
  ScalerLabels[50].Printf(_T("2MV"));
  ScalerLabels[51].Printf(_T("2MH"));
  ScalerLabels[52].Printf(_T("2BV"));
  ScalerLabels[53].Printf(_T("2BH"));
			      	  
  ScalerLabels[54].Printf(_T("6TV"));
  ScalerLabels[55].Printf(_T("6TH"));
  ScalerLabels[56].Printf(_T("6MV"));
  ScalerLabels[57].Printf(_T("6MH"));
  ScalerLabels[58].Printf(_T("6BV"));
  ScalerLabels[59].Printf(_T("6BH"));
//// 5 //////////////////////////////////////////////////////////////////

  ScalerLabels[60].Printf(_T("4TV"));
  ScalerLabels[61].Printf(_T("4TH"));
  ScalerLabels[62].Printf(_T("4MV"));
  ScalerLabels[63].Printf(_T("4MH"));
  ScalerLabels[64].Printf(_T("4BV"));
  ScalerLabels[65].Printf(_T("4 BH"));
			      	  
  ScalerLabels[66].Printf(_T("8TV"));
  ScalerLabels[67].Printf(_T("8TH"));
  ScalerLabels[68].Printf(_T("8MV"));
  ScalerLabels[69].Printf(_T("8MH"));
  ScalerLabels[70].Printf(_T("8BV"));
  ScalerLabels[71].Printf(_T("8BH"));
///// 6 ////////////////////////////////////////////////////////////////

  ScalerLabels[72].Printf(_T("16TV"));
  ScalerLabels[73].Printf(_T("16TH"));
  ScalerLabels[74].Printf(_T("16MV"));
  ScalerLabels[75].Printf(_T("16MH"));
  ScalerLabels[76].Printf(_T("16BV"));
  ScalerLabels[77].Printf(_T("16BH"));
			      	   
  ScalerLabels[78].Printf(_T("12TV"));
  ScalerLabels[79].Printf(_T("12TH"));
  ScalerLabels[80].Printf(_T("12MV"));
  ScalerLabels[81].Printf(_T("12MH"));
  ScalerLabels[82].Printf(_T("12BV"));
  ScalerLabels[83].Printf(_T("12BH"));
//// 7 /////////////////////////////////////////////////////////////////

  ScalerLabels[84].Printf(_T("14TV"));
  ScalerLabels[85].Printf(_T("14TH"));
  ScalerLabels[86].Printf(_T("14MV"));
  ScalerLabels[87].Printf(_T("14MH"));
  ScalerLabels[88].Printf(_T("14BV"));
  ScalerLabels[89].Printf(_T("14BH"));
			      	   
  ScalerLabels[90].Printf(_T("10TV"));
  ScalerLabels[91].Printf(_T("10TH"));
  ScalerLabels[92].Printf(_T("10MV"));
  ScalerLabels[93].Printf(_T("10MH"));
  ScalerLabels[94].Printf(_T("10BV"));
  ScalerLabels[95].Printf(_T("10BH"));
//// 8 ///////////////////////////////////////////////////////////////

  ScalerLabels[96].Printf(_T("15TV"));
  ScalerLabels[97].Printf(_T("15TH"));
  ScalerLabels[98].Printf(_T("15MV"));
  ScalerLabels[99].Printf(_T("15MH"));
  ScalerLabels[100].Printf(_T("15BV"));
  ScalerLabels[101].Printf(_T("15BH"));
			       	   
  ScalerLabels[102].Printf(_T("11TV"));
  ScalerLabels[103].Printf(_T("11TH"));
  ScalerLabels[104].Printf(_T("11MV"));
  ScalerLabels[105].Printf(_T("11MH"));
  ScalerLabels[106].Printf(_T("11BV"));
  ScalerLabels[107].Printf(_T("11BH"));
//// 9 ////////////////////////////////////////////////////////////////////

  ScalerLabels[108].Printf(_T("13TV"));
  ScalerLabels[109].Printf(_T("13TH"));
  ScalerLabels[110].Printf(_T("13MV"));
  ScalerLabels[111].Printf(_T("13MH"));
  ScalerLabels[112].Printf(_T("13BV"));
  ScalerLabels[113].Printf(_T("13BH"));
			       	   
  ScalerLabels[114].Printf(_T("9TV"));
  ScalerLabels[115].Printf(_T("9TH"));
  ScalerLabels[116].Printf(_T("9MV"));
  ScalerLabels[117].Printf(_T("9MH"));
  ScalerLabels[118].Printf(_T("9BV"));
  ScalerLabels[119].Printf(_T("9BH"));
//// 10 ////////////////////////////////////////////////////////////////

  ScalerLabels[120].Printf(_T("nu25"));
  ScalerLabels[121].Printf(_T("nu26"));
  ScalerLabels[122].Printf(_T("nu27"));
  ScalerLabels[123].Printf(_T("nu28"));
  ScalerLabels[124].Printf(_T("nu29"));
  ScalerLabels[125].Printf(_T("nu30"));
			       
  ScalerLabels[126].Printf(_T("nu31"));
  ScalerLabels[127].Printf(_T("nu32"));
  ScalerLabels[128].Printf(_T("nu33"));
  ScalerLabels[129].Printf(_T("nu34"));
  ScalerLabels[130].Printf(_T("nu35"));
  ScalerLabels[131].Printf(_T("nu36"));
//// 11 ////////////////////////////////////////////////////////////

  ScalerLabels[132].Printf(_T("nu37"));
  ScalerLabels[133].Printf(_T("nu38"));
  ScalerLabels[134].Printf(_T("nu39"));
  ScalerLabels[135].Printf(_T("nu40"));
  ScalerLabels[136].Printf(_T("nu41"));
  ScalerLabels[137].Printf(_T("nu42"));

  ScalerLabels[138].Printf(_T("nu43"));
  ScalerLabels[139].Printf(_T("nu44"));
  ScalerLabels[140].Printf(_T("nu45"));
  ScalerLabels[141].Printf(_T("nu46"));
  ScalerLabels[142].Printf(_T("nu47"));
  ScalerLabels[143].Printf(_T("nu48"));
//// 12 ////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////////////////////////////
  for(int i=0;i<N_SCALER;++i){
    ScalerSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    ScalerRange[i]=new double[2];
    ScalerScaleType[i]=Log;
    ScalerAverage[i]=1;

   ScalerRefLabels[i]=ScalerLabels[i];
    ScalerCaptions[i] << _T("Scaler rate of A") << ScalerRefLabels[i];
    ScalerVars[i].Printf(_T("scaler[%d][%d]"),i/SCALER_PER_ROW+1,
					i%SCALER_PER_ROW+1);
    ScalerTable[i]=_T("hk_surf");
    ScalerSpec[i][0]=wxRangeSpec(5,6.5,wxGREEN);
    ScalerSpec[i][1]=wxRangeSpec(4,7.5,wxYELLOW);
    ScalerSpec[i][2]=wxRangeSpec();
    ScalerRange[i][0]=0;
    ScalerRange[i][1]=8;
  }

  // Thresholds
  ThresholdSpec = new wxRangeSpec*[N_THRESHOLD];
  ThresholdRange = new double*[N_THRESHOLD];
  ThresholdScaleType = new ScaleType[N_THRESHOLD];
  ThresholdAverage = new unsigned int[N_THRESHOLD];
  ThresholdLabels = new wxString[N_THRESHOLD];
  ThresholdRefLabels = new wxString[N_THRESHOLD];
  ThresholdCaptions = new wxString[N_THRESHOLD];
  ThresholdVars = new wxString[N_THRESHOLD];
  ThresholdTable = new wxString[N_THRESHOLD];
  ThresholdScale = _T("0,4096");
  for(int i=0;i<N_THRESHOLD;++i){
    ThresholdSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    ThresholdRange[i]=new double[2];
    ThresholdScaleType[i]=Linear;
    ThresholdAverage[i]=1;

	ThresholdRefLabels[i]=ScalerLabels[i];
    ThresholdLabels[i]=ScalerLabels[i];
    ThresholdCaptions[i] << _T("Threshold for A") << ThresholdRefLabels[i];
    ThresholdVars[i].Printf(_T("thresh[%d][%d]"),i/THRESHOLD_PER_ROW+1,i%THRESHOLD_PER_ROW+1);
    ThresholdTable[i]=_T("hk_surf");
    ThresholdSpec[i][0]=wxRangeSpec(1,2,wxGREEN);
    ThresholdSpec[i][1]=wxRangeSpec(0.5,2.4,wxYELLOW);
    ThresholdSpec[i][2]=wxRangeSpec();
    ThresholdRange[i][0]=0;
    ThresholdRange[i][1]=4096;
  }
  
  TrigMonFrame* trigmonWindow = new TrigMonFrame( (wxWindow*) NULL, this->argc,(char **)this->argv );
  trigmonWindow->Show(true);
  return true;
}

/*
 * Cleanup for TrigMonApp
 */
int TrigMonApp::OnExit(){    
  return wxApp::OnExit();
}
