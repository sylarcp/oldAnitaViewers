/*
 * TrigMonOptions.cpp
 *
 * Definitions for dialog that handles TrigMon's user controled options.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TrigMonOptions.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "TrigMonOptions.h"

IMPLEMENT_CLASS( TrigMonOptions, wxDialog )

BEGIN_EVENT_TABLE( TrigMonOptions, wxDialog )
  EVT_BUTTON(wxID_APPLY, TrigMonOptions::OnApply)
END_EVENT_TABLE()

/*
 * Constructor
 */
TrigMonOptions::TrigMonOptions( wxWindow* parent ){
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
  
  // Editing of specs
  wxString labels[N_BARINFO_OPTIONS]={_T("L1 Rates"),_T("L2 Rates"),_T("L3 Rates"),_T("RF Power"),_T("Scalers"),
				      _T("Thresholds")};
  m_SpecChoice = new wxRadioBox(this,wxID_ANY,_T("Select display"),
				wxDefaultPosition,wxDefaultSize,
				N_BARINFO_OPTIONS,labels,N_BARINFO_OPTIONS/2,wxRA_SPECIFY_ROWS);
  sizer->Add(m_SpecChoice,0,wxEXPAND);

  // Initialize subdialogs
  for(int i=0;i<N_BARINFO_OPTIONS;++i){
    m_SpecDialog[i]=new wxRangeDialog(this,wxID_ANY,labels[i],MAX_SPECS);

    m_Limits[i][0]=new wxSpinCtrl(m_SpecDialog[i],wxID_ANY);
    m_Limits[i][0]->SetRange(-10000,1000000000);//changed from 1*10^7 to 1*10^9
    m_Limits[i][1]=new wxSpinCtrl(m_SpecDialog[i],wxID_ANY);//changed from 1*10^7 to 1*10^9
    m_Limits[i][1]->SetRange(-10000,1000000000);
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

  wxBoxSizer *turfDelaySizer =new wxBoxSizer(wxHORIZONTAL);
  wxStaticText *turfDelayText = new wxStaticText(this,wxID_ANY,_T("TURF autoload delay [s] "));
  m_turfReadDelayChoice = new wxSpinCtrl(this,wxID_ANY);
  m_turfReadDelayChoice->SetRange(0,1000);

  turfDelaySizer->Add(turfDelayText,1,wxEXPAND);
  turfDelaySizer->Add(m_turfReadDelayChoice,1,wxEXPAND);

  wxBoxSizer *surfDelaySizer =new wxBoxSizer(wxHORIZONTAL);
  wxStaticText *surfDelayText = new wxStaticText(this,wxID_ANY,_T("SURF autoload delay [s] "));
  m_surfReadDelayChoice = new wxSpinCtrl(this,wxID_ANY);
  m_surfReadDelayChoice->SetRange(0,1000);

  surfDelaySizer->Add(surfDelayText,1,wxEXPAND);
  surfDelaySizer->Add(m_surfReadDelayChoice,1,wxEXPAND);

  sizer->Add(turfDelaySizer,0,wxEXPAND);
  sizer->Add(surfDelaySizer,0,wxEXPAND);

  m_lockstepChoice=new wxCheckBox(this,wxID_ANY,_T("SURF/TURF Lockstep"));
  m_dynamicTargetChoice=new wxCheckBox(this,wxID_ANY,_T("Scaler/Threshold dynamic range"));
  m_sqlDumpChoice=new wxCheckBox(this,wxID_ANY,_T("Show SQL queries"));
  m_crcCheckChoice=new wxCheckBox(this,wxID_ANY,_T("Enable checksum check"));

  sizer->Add(m_lockstepChoice,0,wxEXPAND);
  sizer->Add(m_dynamicTargetChoice,0,wxEXPAND);
  sizer->Add(m_sqlDumpChoice,0,wxEXPAND);
  sizer->Add(m_crcCheckChoice,0,wxEXPAND);

  sizer->Add(buttons,0,wxEXPAND);

  ImportOptions();
  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
  
  return;
}

/*
 * Destructor
 */
TrigMonOptions::~TrigMonOptions(){}

// This actually activates editing subdialogs
void TrigMonOptions::OnApply(wxCommandEvent& event){
  // Any changes stored in dialog's internal rangeSpec array
  m_SpecDialog[m_SpecChoice->GetSelection()]->ShowModal(); 
}

void TrigMonOptions::ImportOptions(){
  using namespace TrigMonOpts;
  m_SpecDialog[0]->SetRangeSpec(L1Spec[0]);
  m_Limits[0][0]->SetValue((int)L1Range[0][0]);
  m_Limits[0][1]->SetValue((int)L1Range[0][1]);
  m_Scale[0]->SetSelection(L1ScaleType[0]);
  m_Average[0]->SetValue(L1Average[0]);
  
  m_SpecDialog[1]->SetRangeSpec(L2Spec[0]);
  m_Limits[1][0]->SetValue((int)L2Range[0][0]);
  m_Limits[1][1]->SetValue((int)L2Range[0][1]);
  m_Scale[1]->SetSelection(L2ScaleType[0]); 
  m_Average[1]->SetValue(L2Average[0]);
 
  m_SpecDialog[2]->SetRangeSpec(L3Spec[0]);
  m_Limits[2][0]->SetValue((int)L3Range[0][0]);
  m_Limits[2][1]->SetValue((int)L3Range[0][1]);
  m_Scale[2]->SetSelection(L3ScaleType[0]);
  m_Average[2]->SetValue(L3Average[0]);
  
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

  m_SpecDialog[5]->SetRangeSpec(ThresholdSpec[0]);
  m_Limits[5][0]->SetValue((int)ThresholdRange[0][0]);
  m_Limits[5][1]->SetValue((int)ThresholdRange[0][1]);
  m_Scale[5]->SetSelection(ThresholdScaleType[0]);
  m_Average[5]->SetValue(ThresholdAverage[0]);

  m_rfPowerTypeChoice->SetSelection(rfPowerType);
  m_turfReadDelayChoice->SetValue(turfReadDelay);
  m_surfReadDelayChoice->SetValue(surfReadDelay);

  m_lockstepChoice->SetValue(Lockstep);
  m_dynamicTargetChoice->SetValue(DynamicTarget);
  m_sqlDumpChoice->SetValue(sqlDump);
  m_crcCheckChoice->SetValue(crcCheck);

  return;
}

void TrigMonOptions::ExportOptions(){
  using namespace TrigMonOpts;

  for(int i=0;i<N_L1;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L1Spec[i][j]=m_SpecDialog[0]->GetRangeSpec()[j];
    L1Range[i][0]=m_Limits[0][0]->GetValue();
    L1Range[i][1]=m_Limits[0][1]->GetValue();
    L1ScaleType[i]=static_cast<ScaleType>(m_Scale[0]->GetSelection());
    L1Average[i]=m_Average[0]->GetValue();
  }
  L1Scale.Printf(_T("%g,%g"),(double)m_Limits[0][0]->GetValue(),
		 (double)m_Limits[0][1]->GetValue());

  for(int i=0;i<N_L2;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L2Spec[i][j]=m_SpecDialog[1]->GetRangeSpec()[j];
    L2Range[i][0]=m_Limits[1][0]->GetValue();
    L2Range[i][1]=m_Limits[1][1]->GetValue();
    L2ScaleType[i]=static_cast<ScaleType>(m_Scale[1]->GetSelection());
    L2Average[i]=m_Average[1]->GetValue();
  }
  L2Scale.Printf(_T("%g,%g"),(double)m_Limits[1][0]->GetValue(),
		 (double)m_Limits[1][1]->GetValue());
  
  for(int i=0;i<N_L3;++i){
    for(int j=0;j<MAX_SPECS;++j)
      L3Spec[i][j]=m_SpecDialog[2]->GetRangeSpec()[j];
    L3Range[i][0]=m_Limits[2][0]->GetValue();
    L3Range[i][1]=m_Limits[2][1]->GetValue();
    L3ScaleType[i]=static_cast<ScaleType>(m_Scale[2]->GetSelection());
    L3Average[i]=m_Average[2]->GetValue();
  }
  L3Scale.Printf(_T("%g,%g"),(double)m_Limits[2][0]->GetValue(),
		 (double)m_Limits[2][1]->GetValue());

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


  for(int i=0;i<N_THRESHOLD;++i){
    for(int j=0;j<MAX_SPECS;++j)
      ThresholdSpec[i][j]=m_SpecDialog[5]->GetRangeSpec()[j];
    
    ThresholdRange[i][0]=m_Limits[5][0]->GetValue();
    ThresholdRange[i][1]=m_Limits[5][1]->GetValue();
    ThresholdScaleType[i]=static_cast<ScaleType>(m_Scale[5]->GetSelection());
    ThresholdAverage[i]=m_Average[5]->GetValue();
  }
  ThresholdScale.Printf(_T("%g,%g"),(double)m_Limits[5][0]->GetValue(),
			(double)m_Limits[5][1]->GetValue());

  rfPowerType = static_cast<RFPowerType>(m_rfPowerTypeChoice->GetSelection()); 

  turfReadDelay = m_turfReadDelayChoice->GetValue();
  surfReadDelay = m_surfReadDelayChoice->GetValue();

  Lockstep = m_lockstepChoice->IsChecked();
  DynamicTarget = m_dynamicTargetChoice->IsChecked();
  sqlDump = m_sqlDumpChoice->IsChecked();
  crcCheck = m_crcCheckChoice->IsChecked();

  return;
}
    
