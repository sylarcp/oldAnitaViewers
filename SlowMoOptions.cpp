/*
 * SlowMoOptions.cpp
 *
 * Definitions for dialog that handles SlowMo's user controled options.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SlowMoOptions.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SlowMoOptions.h"

IMPLEMENT_CLASS( SlowMoOptions, wxDialog )

BEGIN_EVENT_TABLE( SlowMoOptions, wxDialog )
  EVT_BUTTON(wxID_APPLY, SlowMoOptions::OnApply)
END_EVENT_TABLE()

/*
 * Constructor
 */
SlowMoOptions::SlowMoOptions( wxWindow* parent ){
  Create(parent,wxID_ANY,_T("Surf Options"),wxDefaultPosition,wxDefaultSize,
	 wxDEFAULT_DIALOG_STYLE);
  
  SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
  buttons->AddButton(new wxButton(this,wxID_OK,_("OK")));
  buttons->AddButton(new wxButton(this,wxID_CANCEL,_("Cancel")));
  buttons->AddButton(new wxButton(this,wxID_APPLY,_("&Edit")));
  //buttons->AddButton(editButton);
  buttons->Realize();

  wxString ScaleTypeLabels[N_SCALETYPES]={_T("Linear"),_T("Log")};

  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
  
  wxString labels[N_BARINFO_OPTIONS]={_T("Event Rates"),_T("Temperatures"),_T("Powers"),
				      _T("RF Power"),_T("Scalers")};
				     // _T("L1 Rates"),_T("L2 Rates"),_T("L3 Rates"),
				     // _T("RF Power"),_T("Scalers"),_T("Scaler RMS")};
  m_SpecChoice = new wxRadioBox(this,wxID_ANY,_T("Select display"),
				wxDefaultPosition,wxDefaultSize,
				N_BARINFO_OPTIONS,labels,N_BARINFO_OPTIONS/3,
				wxRA_SPECIFY_ROWS);
  sizer->Add(m_SpecChoice,0,wxEXPAND);

  // Initialize subdialogs
  for(int i=0;i<N_BARINFO_OPTIONS;++i){
    m_SpecDialog[i]=new wxRangeDialog(this,wxID_ANY,labels[i],MAX_SPECS);

    m_Limits[i][0]=new wxSpinCtrl(m_SpecDialog[i],wxID_ANY);
    m_Limits[i][0]->SetRange(-10000,10000000);
    m_Limits[i][1]=new wxSpinCtrl(m_SpecDialog[i],wxID_ANY);
    m_Limits[i][1]->SetRange(-10000,10000000);
    m_Scale[i]=new wxRadioBox(m_SpecDialog[i],wxID_ANY,_("Scaling"),wxDefaultPosition, 
			      wxDefaultSize,N_SCALETYPES,ScaleTypeLabels,1,
			      wxRA_SPECIFY_ROWS);
    m_Average[i]=new wxSpinCtrl(m_SpecDialog[i],wxID_ANY);
    m_Average[i]->SetRange(1,wxDEFAULT_HISTORY_BUFFER);

    m_SpecDialog[i]->AddExtraValControl(m_Limits[i][0],_("Low"));
    m_SpecDialog[i]->AddExtraValControl(m_Limits[i][1],_("High"));
    m_SpecDialog[i]->AddExtraValControl(m_Scale[i],_T(""));
    m_SpecDialog[i]->AddExtraValControl(m_Average[i],_("Running Average"));
  }

  wxString rflabels[3]={_T("ADC"),_T("dBm"),_T("T")};
  m_rfPowerTypeChoice = new wxRadioBox(this,wxID_ANY,_T("Select RF power display type"),
				       wxDefaultPosition,wxDefaultSize,
				       3,rflabels,1,wxRA_SPECIFY_ROWS);
  sizer->Add(m_rfPowerTypeChoice,0,wxEXPAND);

  wxBoxSizer *delaySizer =new wxBoxSizer(wxHORIZONTAL);
  wxStaticText *delayText = new wxStaticText(this,wxID_ANY,_T("Autoload delay [s] "));
  m_readDelayChoice = new wxSpinCtrl(this,wxID_ANY);
  m_readDelayChoice->SetRange(0,1000);
  
  delaySizer->Add(delayText,1,wxEXPAND);
  delaySizer->Add(m_readDelayChoice,1,wxEXPAND);

  sizer->Add(delaySizer,0,wxEXPAND);

  m_sqlDumpChoice=new wxCheckBox(this,wxID_ANY,_T("Show SQL queries"));
  sizer->Add(m_sqlDumpChoice,0,wxEXPAND);

  sizer->Add(buttons,0,wxEXPAND);

  ImportOptions();
  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
  
  return;
}

/*
 * Destructor
 */
SlowMoOptions::~SlowMoOptions(){}

// This actually activates editing subdialogs
void SlowMoOptions::OnApply(wxCommandEvent& event){
  // Any changes stored in dialog's internal rangeSpec array
  m_SpecDialog[m_SpecChoice->GetSelection()]->ShowModal(); 
}

void SlowMoOptions::ImportOptions(){
  using namespace SlowMoOpts;
  m_SpecDialog[0]->SetRangeSpec(EventRateSpec[0]);
  m_Limits[0][0]->SetValue((int)EventRateRange[0][0]);
  m_Limits[0][1]->SetValue((int)EventRateRange[0][1]);
  m_Scale[0]->SetSelection(EventRateScaleType[0]);
  m_Average[0]->SetValue(EventRateAverage[0]);
  
  m_SpecDialog[1]->SetRangeSpec(TempSpec[0]);
  m_Limits[1][0]->SetValue((int)TempRange[0][0]);
  m_Limits[1][1]->SetValue((int)TempRange[0][1]);
  m_Scale[1]->SetSelection(TempScaleType[0]);
  m_Average[1]->SetValue(TempAverage[0]);
  
  m_SpecDialog[2]->SetRangeSpec(PowerSpec[0]);
  m_Limits[2][0]->SetValue((int)PowerRange[0][0]);
  m_Limits[2][1]->SetValue((int)PowerRange[0][1]);
  m_Scale[2]->SetSelection(PowerScaleType[0]);
  m_Average[2]->SetValue(PowerAverage[0]);
 /* 
  
  m_SpecDialog[3]->SetRangeSpec(L1Spec[0]);
  m_Limits[3][0]->SetValue((int)L1Range[0][0]);
  m_Limits[3][1]->SetValue((int)L1Range[0][1]);
  m_Scale[3]->SetSelection(L1ScaleType[0]);
  m_Average[3]->SetValue(L1Average[0]);

  m_SpecDialog[4]->SetRangeSpec(L1Spec[0]);
  m_Limits[4][0]->SetValue((int)L1Range[0][0]);
  m_Limits[4][1]->SetValue((int)L1Range[0][1]);
  m_Scale[4]->SetSelection(L1ScaleType[0]);
  m_Average[4]->SetValue(L1Average[0]);

  m_SpecDialog[4]->SetRangeSpec(L2Spec[0]);
  m_Limits[4][0]->SetValue((int)L2Range[0][0]);
  m_Limits[4][1]->SetValue((int)L2Range[0][1]);
  m_Scale[4]->SetSelection(L2ScaleType[0]); 
  m_Average[4]->SetValue(L2Average[0]);
 
  m_SpecDialog[5]->SetRangeSpec(L3Spec[0]);
  m_Limits[5][0]->SetValue((int)L3Range[0][0]);
  m_Limits[5][1]->SetValue((int)L3Range[0][1]);
  m_Scale[5]->SetSelection(L3ScaleType[0]);
  m_Average[5]->SetValue(L3Average[0]);

  m_SpecDialog[6]->SetRangeSpec(L3Spec[0]);
  m_Limits[6][0]->SetValue((int)L3Range[0][0]);
  m_Limits[6][1]->SetValue((int)L3Range[0][1]);
  m_Scale[6]->SetSelection(L3ScaleType[0]);
  m_Average[6]->SetValue(L3Average[0]);
*/
  
  m_SpecDialog[3]->SetRangeSpec(RFPowerSpec[0]);
  m_Limits[3][0]->SetValue((int)RFPowerRange[0][0]);
  m_Limits[3][1]->SetValue((int)RFPowerRange[0][1]);
  m_Scale[3]->SetSelection(RFPowerScaleType[0]);
  m_Average[3]->SetValue(RFPowerAverage[0]);

  m_SpecDialog[4]->SetRangeSpec(ScalerSpec[0]);
  m_Limits[4][0]->SetValue((int)ScalerRange[0][0]);
  m_Limits[4][1]->SetValue((int)ScalerRange[0][1]);
  m_Scale[4]->SetSelection(ScalerScaleType[0]);
  m_Average[4]->SetValue(ScalerAverage[0]);
/*
  m_SpecDialog[8]->SetRangeSpec(ScalerRMSSpec[0]);
  m_Limits[8][0]->SetValue((int)ScalerRMSRange[0][0]);
  m_Limits[8][1]->SetValue((int)ScalerRMSRange[0][1]);
  m_Scale[8]->SetSelection(ScalerRMSScaleType[0]);
  m_Average[8]->SetValue(ScalerRMSAverage[0]);
*/
  m_rfPowerTypeChoice->SetSelection(rfPowerType);
  m_readDelayChoice->SetValue(readDelay);
  m_sqlDumpChoice->SetValue(sqlDump);

  return;
}

void SlowMoOptions::ExportOptions(){
  using namespace SlowMoOpts;

  for(int i=0;i<2;++i){
    for(int j=0;j<MAX_SPECS;++j)
      EventRateSpec[i][j]=m_SpecDialog[0]->GetRangeSpec()[j];
    EventRateRange[i][0]=m_Limits[0][0]->GetValue();
    EventRateRange[i][1]=m_Limits[0][1]->GetValue();
    EventRateScaleType[i]=static_cast<ScaleType>(m_Scale[0]->GetSelection());
    EventRateAverage[i]=m_Average[0]->GetValue();
  }
  EventRateScale.Printf(_T("%g,%g"),(double)m_Limits[0][0]->GetValue(),
		 (double)m_Limits[0][1]->GetValue());

  for(int i=0;i<N_TEMP;++i){
    for(int j=0;j<MAX_SPECS;++j)
      TempSpec[i][j]=m_SpecDialog[1]->GetRangeSpec()[j];
    TempRange[i][0]=m_Limits[1][0]->GetValue();
    TempRange[i][1]=m_Limits[1][1]->GetValue();
    TempScaleType[i]=static_cast<ScaleType>(m_Scale[1]->GetSelection());
    TempAverage[i]=m_Average[1]->GetValue();
  }
  TempScale.Printf(_T("%g,%g"),(double)m_Limits[1][0]->GetValue(),
		 (double)m_Limits[1][1]->GetValue());

  for(int i=0;i<N_POWER;++i){
    for(int j=0;j<MAX_SPECS;++j)
      PowerSpec[i][j]=m_SpecDialog[2]->GetRangeSpec()[j];
    PowerRange[i][0]=m_Limits[2][0]->GetValue();
    PowerRange[i][1]=m_Limits[2][1]->GetValue();
    PowerScaleType[i]=static_cast<ScaleType>(m_Scale[2]->GetSelection());
    PowerAverage[i]=m_Average[2]->GetValue();
  }
  PowerScale.Printf(_T("%g,%g"),(double)m_Limits[2][0]->GetValue(),
		 (double)m_Limits[2][1]->GetValue());
/*
  for(int i=0;i<N_L1;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L1Spec[i][j]=m_SpecDialog[3]->GetRangeSpec()[j];
    L1Range[i][0]=m_Limits[3][0]->GetValue();
    L1Range[i][1]=m_Limits[3][1]->GetValue();
    L1ScaleType[i]=static_cast<ScaleType>(m_Scale[3]->GetSelection());
    L1Average[i]=m_Average[3]->GetValue();
  }
  L1Scale.Printf(_T("%g,%g"),(double)m_Limits[3][0]->GetValue(),
		 (double)m_Limits[3][1]->GetValue());
  for(int i=0;i<N_L1;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L1Spec[i][j]=m_SpecDialog[4]->GetRangeSpec()[j];
    L1Range[i][0]=m_Limits[4][0]->GetValue();
    L1Range[i][1]=m_Limits[4][1]->GetValue();
    L1ScaleType[i]=static_cast<ScaleType>(m_Scale[4]->GetSelection());
    L1Average[i]=m_Average[4]->GetValue();
  }
  L1Scale.Printf(_T("%g,%g"),(double)m_Limits[3][0]->GetValue(),
		 (double)m_Limits[3][1]->GetValue());

  for(int i=0;i<N_L2;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L2Spec[i][j]=m_SpecDialog[4]->GetRangeSpec()[j];
    L2Range[i][0]=m_Limits[4][0]->GetValue();
    L2Range[i][1]=m_Limits[4][1]->GetValue();
    L2ScaleType[i]=static_cast<ScaleType>(m_Scale[4]->GetSelection());
    L2Average[i]=m_Average[4]->GetValue();
  }
  L2Scale.Printf(_T("%g,%g"),(double)m_Limits[4][0]->GetValue(),
		 (double)m_Limits[4][1]->GetValue());
  for(int i=0;i<N_L3;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L3Spec[i][j]=m_SpecDialog[5]->GetRangeSpec()[j];
    L3Range[i][0]=m_Limits[5][0]->GetValue();
    L3Range[i][1]=m_Limits[5][1]->GetValue();
    L3ScaleType[i]=static_cast<ScaleType>(m_Scale[5]->GetSelection());
    L3Average[i]=m_Average[5]->GetValue();
  }
  L3Scale.Printf(_T("%g,%g"),(double)m_Limits[5][0]->GetValue(),
		 (double)m_Limits[5][1]->GetValue());

  for(int i=0;i<N_L3;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L3Spec[i][j]=m_SpecDialog[6]->GetRangeSpec()[j];
    L3Range[i][0]=m_Limits[6][0]->GetValue();
    L3Range[i][1]=m_Limits[6][1]->GetValue();
    L3ScaleType[i]=static_cast<ScaleType>(m_Scale[6]->GetSelection());
    L3Average[i]=m_Average[6]->GetValue();
  }
  L3Scale.Printf(_T("%g,%g"),(double)m_Limits[6][0]->GetValue(),
		 (double)m_Limits[6][1]->GetValue());
  */

  for(int i=0;i<N_RFPOWER;++i){
    for(int j=0;j<MAX_SPECS;++j)
      RFPowerSpec[i][j]=m_SpecDialog[3]->GetRangeSpec()[j];
    RFPowerRange[i][0]=m_Limits[3][0]->GetValue();
    RFPowerRange[i][1]=m_Limits[3][1]->GetValue();
    RFPowerScaleType[i]=static_cast<ScaleType>(m_Scale[3]->GetSelection());
    RFPowerAverage[i]=m_Average[3]->GetValue();
  }
  RFPowerScale.Printf(_T("%g,%g"),(double)m_Limits[3][0]->GetValue(),
		      (double)m_Limits[3][1]->GetValue());

  for(int i=0;i<N_SCALER;++i){
    for(int j=0;j<MAX_SPECS;++j)
      ScalerSpec[i][j]=m_SpecDialog[4]->GetRangeSpec()[j];     
    ScalerRange[i][0]=m_Limits[4][0]->GetValue();
    ScalerRange[i][1]=m_Limits[4][1]->GetValue();
    ScalerScaleType[i]=static_cast<ScaleType>(m_Scale[4]->GetSelection());
    ScalerAverage[i]=m_Average[4]->GetValue();
  }
  ScalerScale.Printf(_T("%g,%g"),(double)m_Limits[4][0]->GetValue(),
		     (double)m_Limits[4][1]->GetValue());


/*
  for(int i=0;i<N_SCALERRMS;++i){
    for(int j=0;j<MAX_SPECS;++j)
      ScalerRMSSpec[i][j]=m_SpecDialog[8]->GetRangeSpec()[j];
    
    ScalerRMSRange[i][0]=m_Limits[8][0]->GetValue();
    ScalerRMSRange[i][1]=m_Limits[8][1]->GetValue();
    ScalerRMSScaleType[i]=static_cast<ScaleType>(m_Scale[8]->GetSelection());
    ScalerRMSAverage[i]=m_Average[8]->GetValue();
  }
  ScalerRMSScale.Printf(_T("%g,%g"),(double)m_Limits[8][0]->GetValue(),
			(double)m_Limits[8][1]->GetValue());
*/
  rfPowerType = static_cast<RFPowerType>(m_rfPowerTypeChoice->GetSelection());
  readDelay = m_readDelayChoice->GetValue();
  sqlDump = m_sqlDumpChoice->IsChecked();

  return;
}
    
