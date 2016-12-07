/*
 * SlowMo.cpp
 *
 * Definitions of global variables for SlowMo program. Also launches it.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SlowMo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "slowmo_defaults.h"
#include "SlowMo.h"
#include "SlowMoFrame.h"

/*
 * Application instance implementation
 */
IMPLEMENT_APP( SlowMoApp )

/*
 * Constructor for SlowMoApp
 */
SlowMoApp::SlowMoApp(){}

/*
 * Initialisation for SlowMoApp
 */
bool SlowMoApp::OnInit(){    
  using namespace SlowMoOpts;
  // Initialize default options
  // Event Rates
  EventRateSpec = new wxRangeSpec*[2];
  EventRateRange = new double*[2];
  EventRateScaleType = new ScaleType[2];
  EventRateAverage = new unsigned int[2];
  EventRateLabels = new wxString[2];
  EventRateCaptions = new wxString[2];
  EventRateVars = new wxString[2];
  EventRateTable = new wxString[2];
  // EventRateScale = _T("0,10");
  EventRateScale = _T("0,100");
  for(int i=0;i<2;++i){
    EventRateSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    EventRateRange[i]=new double[2];
    EventRateScaleType[i]=Linear;
    EventRateAverage[i]=1;
    
    EventRateTable[i]=_T("slow");
    // EventRateSpec[i][0]=wxRangeSpec(3,6,wxGREEN);
    // EventRateSpec[i][1]=wxRangeSpec(2,7,wxYELLOW);
    EventRateSpec[i][0]=wxRangeSpec(30,60,wxGREEN);
    EventRateSpec[i][1]=wxRangeSpec(20,70,wxYELLOW);
    EventRateSpec[i][2]=wxRangeSpec();
    // EventRateRange[i][0]=0;
    //EventRateRange[i][1]=10;
     EventRateRange[i][0]=0;
    EventRateRange[i][1]=100;
  }
  EventRateLabels[0]=_T("1 min");
  EventRateLabels[1]=_T("10 min");
  EventRateCaptions[0]=_T("Event rate over 1 min");
  EventRateCaptions[1]=_T("Event rate over 10 min");
  EventRateVars[0]=_T("rate1");
  EventRateVars[1]=_T("rate10");

  // Temperatures
  TempSpec = new wxRangeSpec*[N_TEMP];
  TempRange = new double*[N_TEMP];
  TempScaleType = new ScaleType[N_TEMP];
  TempAverage = new unsigned int[N_TEMP];
  TempLabels = new wxString[N_TEMP];
  TempCaptions = new wxString[N_TEMP];
  TempVars = new wxString[N_TEMP];
  TempTable = new wxString[N_TEMP];
  TempScale = _T("-80,80");
  for(int i=0;i<N_TEMP;++i){
    TempSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    TempRange[i]=new double[2];
    TempScaleType[i]=Linear;
    TempAverage[i]=1;
    TempTable[i]=_T("slow");
    TempVars[i].Printf(_T("temp[%d]"),i+1);
    TempSpec[i][0]=wxRangeSpec(-50,0,wxBLUE);
    TempSpec[i][1]=wxRangeSpec(0,50,wxGREEN);
    TempSpec[i][2]=wxRangeSpec(50,70,wxYELLOW);
    TempSpec[i][3]=wxRangeSpec();
    TempRange[i][0]=-80;
    TempRange[i][1]=80;    
  }
  // Temps are CPU,SURF, SHORT, Radiator Plate, RFCM1, RFCM5, RFCM 12 and RFCM 15
  TempLabels[0].Printf( _T("sbs"));
  TempCaptions[0].Printf( _T("CPU temperature"));
  TempLabels[1].Printf( _T("surf"));
  TempCaptions[1].Printf( _T("SURF temperature"));
  TempLabels[2].Printf( _T("turf"));
  TempCaptions[2].Printf( _T("Short temperature"));
  TempLabels[3].Printf( _T("rad"));
  TempCaptions[3].Printf( _T("Radiator plate temperature"));
/*
  TempLabels[4].Printf( _T("rfcm1"));
  TempCaptions[4].Printf( _T("RFCM1 temperature"));
  TempLabels[5].Printf( _T("rfcm5"));
  TempCaptions[5].Printf( _T("RFCM5 temperature"));
  TempLabels[6].Printf( _T("rfcm12"));
  TempCaptions[6].Printf( _T("RFCM12 temperature"));
  TempLabels[7].Printf( _T("rfcm15"));
  TempCaptions[7].Printf( _T("RFCM15 temperature"));
*/
  
  // Powers
  PowerSpec = new wxRangeSpec*[N_POWER];
  PowerRange = new double*[N_POWER];
  PowerScaleType = new ScaleType[N_POWER];
  PowerAverage = new unsigned int[N_POWER];
  PowerLabels = new wxString[N_POWER];
  PowerCaptions = new wxString[N_POWER];
  PowerVars = new wxString[N_POWER];
  PowerTable = new wxString[N_POWER];
  PowerScale = _T("");
  for(int i=0;i<N_POWER;++i){
    PowerSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    PowerRange[i]=new double[2];
    PowerScaleType[i]=Linear;
    PowerAverage[i]=1;
    PowerTable[i]=_T("slow");
    PowerSpec[i][1]=wxRangeSpec();
  }

  // Powers are PV V, 24V, Batt I, 24 I
  PowerLabels[0].Printf( _T("PV"));
  PowerCaptions[0].Printf( _T("PV voltage"));
  PowerVars[0].Printf(_T("ppvv"));
  PowerSpec[0][0]=wxRangeSpec(47,61,wxGREEN);
  PowerRange[0][0]=0;
  PowerRange[0][1]=75;

  PowerLabels[1].Printf( _T("24V"));
  PowerCaptions[1].Printf( _T("+24V voltage"));
  PowerVars[1].Printf(_T("p24v"));
  PowerSpec[1][0]=wxRangeSpec(23.6,30.6,wxGREEN);
  PowerRange[1][0]=0;
  PowerRange[1][1]=75;

  PowerLabels[2].Printf( _T("PV_I"));
  PowerCaptions[2].Printf( _T("PV current"));
  PowerVars[2].Printf(_T("bati"));
  PowerSpec[2][0]=wxRangeSpec(7,11,wxGREEN);
  PowerRange[2][0]=0;
  PowerRange[2][1]=30;
  
  PowerLabels[3].Printf( _T("24I"));
  PowerCaptions[3].Printf( _T("+24V current"));
  PowerVars[3].Printf(_T("p24I"));
  PowerSpec[3][0]=wxRangeSpec(13.4,19,wxGREEN);
  PowerRange[3][0]=0;
  PowerRange[3][1]=30;
  

  // L1 rates
  L1Spec = new wxRangeSpec*[N_L1];
  L1Range = new double*[N_L1];
  L1ScaleType = new ScaleType[N_L1];
  L1Average = new unsigned int[N_L1];
  L1Labels = new wxString[N_L1];
  L1Captions = new wxString[N_L1];
  L1Vars = new wxString[N_L1];
  L1Table = new wxString[N_L1];
  L1Scale = _T("0,7");
  for(int i=0;i<N_L1;++i){
    L1Spec[i]=new wxRangeSpec[MAX_SPECS+1];
    L1Range[i]=new double[2];
    L1ScaleType[i]=Log;
    L1Average[i]=1;
    
    L1Labels[i].Printf( _T("%02d"),i+1);
    L1Captions[i].Printf( _T("Surf %d average L1 rate"),i+1);
    L1Vars[i].Printf(_T("l1[%d]"),i+1);
    L1Table[i]=_T("slow");
    //L1Spec[i][0]=wxRangeSpec(6,6.3,wxGREEN);
    // L1Spec[i][1]=wxRangeSpec(5.7,6.4,wxYELLOW);
    L1Spec[i][0]=wxRangeSpec(5,6,wxGREEN);
    L1Spec[i][1]=wxRangeSpec(4,6.5,wxYELLOW);
    L1Spec[i][2]=wxRangeSpec();
    L1Range[i][0]=0;
    L1Range[i][1]=7;
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
  // L2Scale = _T("0,3");
  L2Scale = _T("0,4.5");
  for(int i=0;i<N_L2;++i){
    L2Spec[i]=new wxRangeSpec[MAX_SPECS+1];
    L2Range[i]=new double[2];
    L2ScaleType[i]=Log;
    L2Average[i]=1;
//original code 
   // L2Labels[i].Printf(_T("%c%02d"),i<N_L2/2?'u':'l',i%N_PHI_SECTORS+1);
    //L2Captions[i].Printf(_T("%s sector %02d average L2 rate"),i<N_L2/2?"Upper":"Lower",i%N_PHI_SECTORS+1);
    //L2Vars[i].Printf(_T("l2[%d][%d]"),i/L2_PER_ROW+1,i%L2_PER_ROW+1);
    ///////////////////////////////////////////////////////////////////////////////amir's addition august08
   L2Labels[i].Printf( _T("%02d"),i+1);
    L2Captions[i].Printf( _T("Phi %d average L2 rate"),i+1);
    L2Vars[i].Printf(_T("l2[%d]"),i+1);
   //////////////////////////////////////////////////////////////////////////////
    L2Table[i]=_T("slow");
    //L2Spec[i][0]=wxRangeSpec(3,3.3,wxGREEN);
    // L2Spec[i][1]=wxRangeSpec(2.7,3.4,wxYELLOW);
    L2Spec[i][0]=wxRangeSpec(3,3.5,wxGREEN);
    L2Spec[i][1]=wxRangeSpec(2,4,wxYELLOW);
    L2Spec[i][2]=wxRangeSpec();
    // L2Range[i][0]=0;
    // L2Range[i][1]=3;
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
  L3Scale = _T("0,15");
  for(int i=0;i<N_L3;++i){
    L3Spec[i]=new wxRangeSpec[MAX_SPECS+1];
    L3Range[i]=new double[2];
    L3ScaleType[i]=Linear;
    L3Average[i]=1;

    L3Labels[i].Printf(_T("%02d"),i+1);
    L3Captions[i].Printf(_T("Sector %d average L3 rate"),i+1);
    L3Vars[i].Printf(_T("l3[%d]"),i+1);
    L3Table[i]=_T("slow");
    L3Spec[i][0]=wxRangeSpec(2,7,wxGREEN);
    L3Spec[i][1]=wxRangeSpec(1,10,wxYELLOW);
    L3Spec[i][2]=wxRangeSpec();
    L3Range[i][0]=0;
    L3Range[i][1]=15;
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
  RFPowerScale = _T("0,,25000");
  for(int i=0;i<N_RFPOWER;++i){
    RFPowerSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    RFPowerRange[i]=new double[2];
    RFPowerScaleType[i]=Linear;
    RFPowerAverage[i]=1;

    //    RFPowerLabels[i].Printf(" %s:%2d ",surftag[i/RFPOWER_PER_ROW],
    //				 i%RFPOWER_PER_ROW+1);
    RFPowerVars[i].Printf(_T("rfpow[%d][%d]"),i/RFPOWER_PER_ROW+1,
				       i%RFPOWER_PER_ROW+1);
    RFPowerTable[i]=_T("slow");
    RFPowerSpec[i][0]=wxRangeSpec(0,6600,wxBLUE);
    RFPowerSpec[i][1]=wxRangeSpec(6600,13300,wxGREEN);
    RFPowerSpec[i][2]=wxRangeSpec(13300,20500,wxYELLOW);
    RFPowerSpec[i][3]=wxRangeSpec();
    RFPowerRange[i][0]=0;
    RFPowerRange[i][1]=25000;
  }
/*
  RFPowerLabels[0]=_T("9V");
  RFPowerLabels[1]=_T("13V");
  RFPowerLabels[2]=_T("17V");
  RFPowerLabels[3]=_T("25V");
  RFPowerLabels[4]=_T("9H");
  RFPowerLabels[5]=_T("13H");
  RFPowerLabels[6]=_T("17H");
  RFPowerLabels[7]=_T("25H");
  ////////////////////////////////////////////
  RFPowerLabels[8]=_T("4V");
  RFPowerLabels[9]=_T("8V");
  RFPowerLabels[10]=_T("24V");
  RFPowerLabels[11]=_T("32V");
  RFPowerLabels[12]=_T("4H");
  RFPowerLabels[13]=_T("8H");
  RFPowerLabels[14]=_T("24H");
  RFPowerLabels[15]=_T("32H");
  ////////////////////////////////////////////////
  RFPowerLabels[16]=_T("10V");
  RFPowerLabels[17]=_T("14V");
  RFPowerLabels[18]=_T("19V");
  RFPowerLabels[19]=_T("27V");
  RFPowerLabels[20]=_T("10H");
  RFPowerLabels[21]=_T("14H");
  RFPowerLabels[22]=_T("19H");
  RFPowerLabels[23]=_T("27H");
  ///////////////////////////////////////////////
  RFPowerLabels[24]=_T("11V");
  RFPowerLabels[25]=_T("15V");
  RFPowerLabels[26]=_T("21V");
  RFPowerLabels[27]=_T("29V");
  RFPowerLabels[28]=_T("11H");
  RFPowerLabels[29]=_T("15H");
  RFPowerLabels[30]=_T("21H");
  RFPowerLabels[31]=_T("29H");
  ///////////////////////////////////////////////
  RFPowerLabels[32]=_T("12V");
  RFPowerLabels[33]=_T("16V");
  RFPowerLabels[34]=_T("23V");
  RFPowerLabels[35]=_T("31V");
  RFPowerLabels[36]=_T("12H");
  RFPowerLabels[37]=_T("16H");
  RFPowerLabels[38]=_T("23H");
  RFPowerLabels[39]=_T("31H");
  ////////////////////////////////////////////////
  RFPowerLabels[40]=_T("1V");
  RFPowerLabels[41]=_T("5V");
  RFPowerLabels[42]=_T("18V");
  RFPowerLabels[43]=_T("26V");
  RFPowerLabels[44]=_T("1H");
  RFPowerLabels[45]=_T("5H");
  RFPowerLabels[46]=_T("18H");
  RFPowerLabels[47]=_T("26H");
  ///////////////////////////////////////////
  RFPowerLabels[48]=_T("3V");
  RFPowerLabels[49]=_T("7V");
  RFPowerLabels[50]=_T("22V");
  RFPowerLabels[51]=_T("30V");
  RFPowerLabels[52]=_T("3H");
  RFPowerLabels[53]=_T("7H");
  RFPowerLabels[54]=_T("22H");
  RFPowerLabels[55]=_T("30H");
  /////////////////////////////////////////
  RFPowerLabels[56]=_T("2V");
  RFPowerLabels[57]=_T("6V");
  RFPowerLabels[58]=_T("20V");
  RFPowerLabels[59]=_T("28V");
  RFPowerLabels[60]=_T("2H");
  RFPowerLabels[61]=_T("6H");
  RFPowerLabels[62]=_T("20H");
  RFPowerLabels[63]=_T("28H");
  /////////////////////////////////////////
  RFPowerLabels[64]=_T("N1V");
  RFPowerLabels[65]=_T("N3V");
  RFPowerLabels[66]=_T("N5V");
  RFPowerLabels[67]=_T("N7V");
  RFPowerLabels[68]=_T("N1H");
  RFPowerLabels[69]=_T("N3H");
  RFPowerLabels[70]=_T("N5H");
  RFPowerLabels[71]=_T("N7H");
  /////////////////////////////////////////
  RFPowerLabels[72]=_T("N2V");
  RFPowerLabels[73]=_T("N4V");
  RFPowerLabels[74]=_T("N6V");
  RFPowerLabels[75]=_T("N8V");
  RFPowerLabels[76]=_T("N2H");
  RFPowerLabels[77]=_T("N4H");
  RFPowerLabels[78]=_T("N6H");
  RFPowerLabels[79]=_T("N8H");
*/

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

  ////////////////////////////////////////amir's addition july 19 08
  for(int i=0;i<N_RFPOWER;++i)
    RFPowerCaptions[i] << _T("Average RF power in channel ") << RFPowerLabels[i];

  // Scalers
  ScalerSpec = new wxRangeSpec*[N_SCALER];
  ScalerRange = new double*[N_SCALER];
  ScalerScaleType = new ScaleType[N_SCALER];
  ScalerAverage = new unsigned int[N_SCALER];
  ScalerLabels = new wxString[N_SCALER];
  ScalerCaptions = new wxString[N_SCALER];
  ScalerVars = new wxString[N_SCALER];
  ScalerTable = new wxString[N_SCALER];
  // ScalerScale = _T("0,7");
   ScalerScale = _T("0,8");
  for(int i=0;i<N_SCALER;++i){
    ScalerSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    ScalerRange[i]=new double[2];
    ScalerScaleType[i]=Log;
    ScalerAverage[i]=1;

    //ScalerLabels[i].Printf( _T("%02d"),i+1);
    ScalerLabels[i].Printf( _T("%02d"),(i%SCALER_PER_ROW)*(N_SCALER/SCALER_PER_ROW)+(i/SCALER_PER_ROW+1));

/*
//peng
///////////// SURF 1 ///////////////////////////////
  ScalerLabels[0]=_T("10BV");
  ScalerLabels[1]=_T("2BV");
  ScalerLabels[2]=_T("16BV");
  ScalerLabels[3]=_T("8BV");
  ScalerLabels[4]=_T("10BH");
  ScalerLabels[5]=_T("2BH");
  ScalerLabels[6]=_T("16BH");
  ScalerLabels[7]=_T("8BH");
  ///////////// SURF 2 ///////////////////////////////
  ScalerLabels[8]=_T("12BV");
  ScalerLabels[9]=_T("4BV");
  ScalerLabels[10]=_T("14BV");
  ScalerLabels[11]=_T("6BV");
  ScalerLabels[12]=_T("12BH");
  ScalerLabels[13]=_T("4BH");
  ScalerLabels[14]=_T("14BH");
  ScalerLabels[15]=_T("6BH");
  ///////////// SURF 3 ///////////////////////////////////
  ScalerLabels[16]=_T("16MV");
  ScalerLabels[17]=_T("8MV");
  ScalerLabels[18]=_T("12MV");
  ScalerLabels[19]=_T("4MV");
  ScalerLabels[20]=_T("16MH");
  ScalerLabels[21]=_T("8MH");
  ScalerLabels[22]=_T("12MH");
  ScalerLabels[23]=_T("4MH");
  ///////////// SURF 4 //////////////////////////////////
  ScalerLabels[24]=_T("14MV");
  ScalerLabels[25]=_T("6MV");
  ScalerLabels[26]=_T("10MV");
  ScalerLabels[27]=_T("2MV");
  ScalerLabels[28]=_T("14MH");
  ScalerLabels[29]=_T("6MH");
  ScalerLabels[30]=_T("10MH");
  ScalerLabels[31]=_T("2MH");
///////////// SURF 5 //////////////////////////////////
  ScalerLabels[32]=_T("12TV");
  ScalerLabels[33]=_T("4TV");
  ScalerLabels[34]=_T("14TV");
  ScalerLabels[35]=_T("6TV");
  ScalerLabels[36]=_T("12TH");
  ScalerLabels[37]=_T("4TH");
  ScalerLabels[38]=_T("14TH");
  ScalerLabels[39]=_T("6TH");
///////////// SURF 6 ///////////////////////////////////
  ScalerLabels[40]=_T("10TV");
  ScalerLabels[41]=_T("2TV");
  ScalerLabels[42]=_T("16TV");
  ScalerLabels[43]=_T("8TV");
  ScalerLabels[44]=_T("10TH");
  ScalerLabels[45]=_T("2TH");
  ScalerLabels[46]=_T("16TH");
  ScalerLabels[47]=_T("8TH");
  ///////////// SURF 7 //////////////////////////////
  ScalerLabels[48]=_T("13BV");
  ScalerLabels[49]=_T("5BV");
  ScalerLabels[50]=_T("9BV");
  ScalerLabels[51]=_T("1BV");
  ScalerLabels[52]=_T("13BH");
  ScalerLabels[53]=_T("5BH");
  ScalerLabels[54]=_T("9BH");
  ScalerLabels[55]=_T("1BH");
  ///////////// SURF 8 ////////////////////////////
  ScalerLabels[56]=_T("15BV");
  ScalerLabels[57]=_T("7BV");
  ScalerLabels[58]=_T("11BV");
  ScalerLabels[59]=_T("3BV");
  ScalerLabels[60]=_T("15BH");
  ScalerLabels[61]=_T("7BH");
  ScalerLabels[62]=_T("11BH");
  ScalerLabels[63]=_T("3BH");
///////////// SURF 9 ////////////////////////////
  ScalerLabels[64]=_T("11MV");
  ScalerLabels[65]=_T("3MV");
  ScalerLabels[66]=_T("13MV");
  ScalerLabels[67]=_T("5MV");
  ScalerLabels[68]=_T("11MH");
  ScalerLabels[69]=_T("3MH");
  ScalerLabels[70]=_T("13MH");
  ScalerLabels[71]=_T("5MH");
  ///////////// SURF 10 ////////////////////////////
  ScalerLabels[72]=_T("9MV");
  ScalerLabels[73]=_T("1MV");
  ScalerLabels[74]=_T("15MV");
  ScalerLabels[75]=_T("7MV");
  ScalerLabels[76]=_T("9MH");
  ScalerLabels[77]=_T("1MH");
  ScalerLabels[78]=_T("15MH");
  ScalerLabels[79]=_T("7MH");
 ///////////// SURF 11 ////////////////////////////
  ScalerLabels[80]=_T("15TV");
  ScalerLabels[81]=_T("7TV");
  ScalerLabels[82]=_T("11TV");
  ScalerLabels[83]=_T("3TV");
  ScalerLabels[84]=_T("15TH");
  ScalerLabels[85]=_T("7TH");
  ScalerLabels[86]=_T("11TH");
  ScalerLabels[87]=_T("3TH");
  ///////////// SURF 12 ////////////////////////////
  ScalerLabels[88]=_T("13TV");
  ScalerLabels[89]=_T("5TV");
  ScalerLabels[90]=_T("9TV");
  ScalerLabels[91]=_T("1TV");
  ScalerLabels[92]=_T("13TH");
  ScalerLabels[93]=_T("5TH");
  ScalerLabels[94]=_T("9TH");
  ScalerLabels[95]=_T("1TH");

*/


    ScalerCaptions[i] << _T("Average scaler rate of A") << ScalerLabels[i];
    ScalerVars[i].Printf(_T("scaler[%d][%d]"),i%SCALER_PER_ROW+1,
			 i/SCALER_PER_ROW+1);
    ScalerTable[i]=_T("slow");
    // ScalerSpec[i][0]=wxRangeSpec(6,6.3,wxGREEN);
    // ScalerSpec[i][1]=wxRangeSpec(5.7,6.4,wxYELLOW);
    ScalerSpec[i][0]=wxRangeSpec(5,6,wxGREEN);
    ScalerSpec[i][1]=wxRangeSpec(4,7.5,wxYELLOW);
    ScalerSpec[i][2]=wxRangeSpec();
    // ScalerRange[i][0]=0;
    // ScalerRange[i][1]=7;
    ScalerRange[i][0]=0;
    ScalerRange[i][1]=8;
  }
  // Scaler RMS
  ScalerRMSSpec = new wxRangeSpec*[N_SCALERRMS];
  ScalerRMSRange = new double*[N_SCALERRMS];
  ScalerRMSScaleType = new ScaleType[N_SCALERRMS];
  ScalerRMSAverage = new unsigned int[N_SCALERRMS];
  ScalerRMSLabels = new wxString[N_SCALERRMS];
  ScalerRMSCaptions = new wxString[N_SCALERRMS];
  ScalerRMSVars = new wxString[N_SCALERRMS];
  ScalerRMSTable = new wxString[N_SCALERRMS];
  // ScalerRMSScale = _T("0,1000");
  ScalerRMSScale = _T("0,8");
  for(int i=0;i<N_SCALERRMS;++i){
    ScalerRMSSpec[i]=new wxRangeSpec[MAX_SPECS+1];
    ScalerRMSRange[i]=new double[2];
    // ScalerRMSScaleType[i]=Linear;
    ScalerRMSScaleType[i]=Log;
    ScalerRMSAverage[i]=1;

  //  ScalerRMSLabels[i].Printf( _T("%02d"),(i%SCALERRMS_PER_ROW)*(N_SCALERRMS/SCALERRMS_PER_ROW)+(i/SCALERRMS_PER_ROW+1));
    ScalerRMSCaptions[i] << _T("Scaler rate RMS of A") << ScalerRMSLabels[i];
    ScalerRMSVars[i].Printf(_T("scalerrms[%d][%d]"),i%SCALERRMS_PER_ROW+1,
			    i/SCALERRMS_PER_ROW+1);
    ScalerRMSTable[i]=_T("slow");
    // ScalerRMSSpec[i][0]=wxRangeSpec(0,100,wxGREEN);
    // ScalerRMSSpec[i][1]=wxRangeSpec(100,500,wxYELLOW);
    ScalerRMSSpec[i][0]=wxRangeSpec(5,6,wxGREEN);
    ScalerRMSSpec[i][1]=wxRangeSpec(4,7.5,wxYELLOW);
    ScalerRMSSpec[i][2]=wxRangeSpec();
    ScalerRMSRange[i][0]=0;
    ScalerRMSRange[i][1]=8;
  }
  
  SlowMoFrame* slowmoWindow = new SlowMoFrame( (wxWindow*) NULL, this->argc, (char **)this->argv );
  slowmoWindow->Show(true);
  return true;
}

/*
 * Cleanup for SlowMoApp
 */
int SlowMoApp::OnExit(){    
  return wxApp::OnExit();
}
