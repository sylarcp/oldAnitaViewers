/*
 * OptionsDialog.cpp
 *
 * Definitions for user controled options dialog for AnitaViewer.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "OptionsDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/colordlg.h>
#include "OptionsDialog.h"

IMPLEMENT_CLASS( OptionsDialog, wxDialog )

BEGIN_EVENT_TABLE( OptionsDialog, wxDialog )
  EVT_BUTTON(wxID_APPLY, OptionsDialog::OnApply)
  EVT_COMMAND_RANGE(PHIMASK_BUTTON,L3HTRIG_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,
		    OptionsDialog::OnTrigColourEdit)
  EVT_BUTTON(EVENTRATE_BUTTON, OptionsDialog::OnEventRateEdit)
END_EVENT_TABLE()

/*
 * Constructor
 */
OptionsDialog::OptionsDialog( wxWindow* parent ){
  Create(parent,wxID_ANY,_T("Aview Options"),wxDefaultPosition,wxDefaultSize,
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
  
  m_notebook = new wxNotebook(this,wxID_ANY);
  sizer->Add(m_notebook,1,wxEXPAND);
  sizer->Add(buttons,0,wxEXPAND);

  wxPanel *m_TempPage = new wxPanel(m_notebook,wxID_ANY);
  m_TempChoice = new wxRadioBox(m_TempPage,wxID_ANY,
				_T("Select temperature display"),
				wxDefaultPosition,
				wxDefaultSize,
				N_TEMP,
				AviewOpts::TempLabels,
				TEMP_PER_ROW,
				wxRA_SPECIFY_ROWS);
  wxBoxSizer *tempSizer=new wxBoxSizer(wxVERTICAL);
  tempSizer->Add(m_TempChoice,1,wxEXPAND);
  m_TempPage->SetSizerAndFit(tempSizer);
 
  m_notebook->InsertPage(TEMP_PAGE_ID,m_TempPage,_("Temperatures"),true);

  // Initialize temperature subdialogs
  for(int i=0;i<N_TEMP;++i){
    m_TempSpecDialog[i]=new wxRangeDialog(m_TempPage,wxID_ANY,
					  AviewOpts::TempLabels[i],
					  MAX_SPECS);

    m_TempLimits[i][0]=new wxSpinCtrl(m_TempSpecDialog[i],wxID_ANY);
    m_TempLimits[i][0]->SetRange(-273,500);
    m_TempLimits[i][1]=new wxSpinCtrl(m_TempSpecDialog[i],wxID_ANY);
    m_TempLimits[i][1]->SetRange(-273,500);
    m_TempScale[i]=new wxRadioBox(m_TempSpecDialog[i],wxID_ANY,_("Scaling"),wxDefaultPosition, 
				  wxDefaultSize,N_SCALETYPES,ScaleTypeLabels,1,
				  wxRA_SPECIFY_ROWS);
    m_TempAverage[i]=new wxSpinCtrl(m_TempSpecDialog[i],wxID_ANY);
    m_TempAverage[i]->SetRange(1,wxDEFAULT_HISTORY_BUFFER);

    m_TempSpecDialog[i]->AddExtraValControl(m_TempLimits[i][0],_("Low"));
    m_TempSpecDialog[i]->AddExtraValControl(m_TempLimits[i][1],_("High"));
    m_TempSpecDialog[i]->AddExtraValControl(m_TempScale[i],_T(""));
    m_TempSpecDialog[i]->AddExtraValControl(m_TempAverage[i],_("Running Average"));
  }

  // Current page
  wxPanel *m_CurrentPage = new wxPanel(m_notebook,wxID_ANY);
  m_CurrentChoice = new wxRadioBox(m_CurrentPage,wxID_ANY,
				   _T("Select current display"),
				   wxDefaultPosition,
				   wxDefaultSize,
				   N_CURRENT,
				   AviewOpts::CurrentLabels,
				   CURRENT_PER_COL,
				   wxRA_SPECIFY_ROWS);
  wxBoxSizer *currentSizer=new wxBoxSizer(wxVERTICAL);
  currentSizer->Add(m_CurrentChoice,1,wxEXPAND);
  m_CurrentPage->SetSizerAndFit(currentSizer);
  
  m_notebook->InsertPage(CURRENT_PAGE_ID,m_CurrentPage,_("Currents"),false);

  // Initialize current subdialogs
  for(int i=0;i<N_CURRENT;++i)
    m_CurrentSpecDialog[i]=new wxRangeDialog(m_CurrentPage,wxID_ANY,
					     AviewOpts::CurrentLabels[i],
					     MAX_SPECS);

  // Voltage page
  wxPanel *m_VoltagePage = new wxPanel(m_notebook,wxID_ANY);
  m_VoltageChoice = new wxRadioBox(m_VoltagePage,wxID_ANY,
				   _T("Select voltage display"),
				   wxDefaultPosition,
				   wxDefaultSize,
				   N_VOLTAGE,
				   AviewOpts::VoltageLabels,
				   VOLTAGE_PER_COL,
				   wxRA_SPECIFY_ROWS);
  wxBoxSizer *voltageSizer=new wxBoxSizer(wxVERTICAL);
  voltageSizer->Add(m_VoltageChoice,1,wxEXPAND);
  m_VoltagePage->SetSizerAndFit(voltageSizer);
  
  m_notebook->InsertPage(VOLTAGE_PAGE_ID,m_VoltagePage,_("Voltages"),false);

  // Initialize voltage subdialogs
  for(int i=0;i<N_VOLTAGE;++i)
    m_VoltageSpecDialog[i]=new wxRangeDialog(m_VoltagePage,wxID_ANY,
					     AviewOpts::VoltageLabels[i],
					     MAX_SPECS);

  // Power page
  wxPanel *m_PowerPage = new wxPanel(m_notebook,wxID_ANY);
  m_PowerChoice = new wxRadioBox(m_PowerPage,wxID_ANY,
				   _T("Select power display"),
				   wxDefaultPosition,
				   wxDefaultSize,
				   N_POWER,
				   AviewOpts::PowerLabels,
				   POWER_PER_COL,
				   wxRA_SPECIFY_ROWS);
  wxBoxSizer *powerSizer=new wxBoxSizer(wxVERTICAL);
  powerSizer->Add(m_PowerChoice,1,wxEXPAND);
  m_PowerPage->SetSizerAndFit(powerSizer);
  
  m_notebook->InsertPage(VOLTAGE_PAGE_ID,m_PowerPage,_("Powers"),false);

  // Initialize power subdialogs
  for(int i=0;i<N_POWER;++i)
    m_PowerSpecDialog[i]=new wxRangeDialog(m_PowerPage,wxID_ANY,
					     AviewOpts::PowerLabels[i],
					     MAX_SPECS);

  // RF page
  wxPanel *m_RFPage = new wxPanel(m_notebook,wxID_ANY);
  wxString RFOverviewLabels[]={_T("Surf View"),_T("Phi View"), _T("Payload View")};
  //wxString RFOverviewLabels[]={_T("Surf View"),_T("Phi View"), _T("Payload View"),_T("old PHi view")};
  m_RFOverviewChoice = new wxRadioBox(m_RFPage,wxID_ANY,
				      _T("RF Overview Display"),
				      wxDefaultPosition,
				      wxDefaultSize,
				      3,RFOverviewLabels,    
				      3,wxRA_SPECIFY_ROWS);

  wxString PayloadStyleLabels[]={_T("Mesh"),_T("Solid")};
  m_PayloadStyleChoice = new wxRadioBox(m_RFPage,wxID_ANY,
					_T("Payload Style"),
					wxDefaultPosition,
					wxDefaultSize,
					2,PayloadStyleLabels,    
					2,wxRA_SPECIFY_ROWS);

  wxString RFReferenceLabels[]={_T("RMS"),_T("Absolute")};
  m_RFReferenceChoice = new wxRadioBox(m_RFPage,wxID_ANY,
					_T("RF Color Reference"),
					wxDefaultPosition,
					wxDefaultSize,
					2,RFReferenceLabels,    
					2,wxRA_SPECIFY_ROWS);


  wxStaticBox *checkBox = new wxStaticBox(m_RFPage,wxID_ANY,_T("RF View Defaults"));
  wxStaticBoxSizer *checkBoxSizer=new wxStaticBoxSizer(checkBox,wxVERTICAL);
  m_RFViewChoice[0]=new wxCheckBox(m_RFPage,wxID_ANY,_T("Voltage"));
  m_RFViewChoice[1]=new wxCheckBox(m_RFPage,wxID_ANY,_T("FFT"));
  m_RFViewChoice[2]=new wxCheckBox(m_RFPage,wxID_ANY,_T("Power"));
  for(int i=0;i<3;++i)
    checkBoxSizer->Add(m_RFViewChoice[i],0,wxEXPAND);

  m_ClockRFDisplayChoice=new wxCheckBox(m_RFPage,wxID_ANY,_T("Display RF clock"));
  m_CompassDisplayChoice=new wxCheckBox(m_RFPage,wxID_ANY,_T("Display compass"));

  wxString RFTabLabels[]={_T("Voltage"),_T("FFT"),_T("Power")};
  m_RFTabChoice = new wxRadioBox(m_RFPage,wxID_ANY,
				      _T("RF Tab View"),
				      wxDefaultPosition,
				      wxDefaultSize,
				      3,RFTabLabels,    
				      3,wxRA_SPECIFY_ROWS);

  wxBoxSizer *rftabSizer =new wxBoxSizer(wxHORIZONTAL);
  rftabSizer->Add(new wxStaticText(m_RFPage,wxID_ANY,_T("RF tab scale min/max, (set 0/0 for auto scale) ")),2,
		  wxEXPAND);
  m_RFTabScaleMinChoice = new wxSpinCtrl(m_RFPage,wxID_ANY);
  m_RFTabScaleMinChoice->SetRange(-4000,4000);
  m_RFTabScaleMaxChoice = new wxSpinCtrl(m_RFPage,wxID_ANY);
  m_RFTabScaleMaxChoice->SetRange(-4000,4000);
  rftabSizer->Add(m_RFTabScaleMinChoice,1,wxEXPAND);
  rftabSizer->Add(m_RFTabScaleMaxChoice,1,wxEXPAND);

  wxBoxSizer *clockrftabSizer =new wxBoxSizer(wxHORIZONTAL);
  clockrftabSizer->Add(new wxStaticText(m_RFPage,wxID_ANY,
					_T("Clock RF tab scale min/max ")),
		       2,wxEXPAND);
  m_ClockRFTabScaleMinChoice = new wxSpinCtrl(m_RFPage,wxID_ANY);
  m_ClockRFTabScaleMinChoice->SetRange(-4000,4000);
  m_ClockRFTabScaleMaxChoice = new wxSpinCtrl(m_RFPage,wxID_ANY);
  m_ClockRFTabScaleMaxChoice->SetRange(-4000,4000);
  clockrftabSizer->Add(m_ClockRFTabScaleMinChoice,1,wxEXPAND);
  clockrftabSizer->Add(m_ClockRFTabScaleMaxChoice,1,wxEXPAND);

  m_PhiMaskColourButton=new wxButton(m_RFPage,PHIMASK_BUTTON,_T("Phi V Mask"));
  m_PhiHMaskColourButton=new wxButton(m_RFPage,PHIHMASK_BUTTON,_T("Phi H Mask"));
  m_L3TrigColourButton=new wxButton(m_RFPage,L3TRIG_BUTTON,_T("L3 V Trigger"));
  m_L3HTrigColourButton=new wxButton(m_RFPage,L3HTRIG_BUTTON,_T("L3 H Trigger"));
  wxStaticBox *trigBox = new wxStaticBox(m_RFPage,wxID_ANY,_T("Trigger Colour"));
  wxStaticBoxSizer *trigColourSizer=new wxStaticBoxSizer(trigBox,wxVERTICAL);
  trigColourSizer->Add(m_PhiMaskColourButton,0,wxEXPAND|wxLEFT|wxRIGHT,5);
  trigColourSizer->Add(m_PhiHMaskColourButton,0,wxEXPAND|wxLEFT|wxRIGHT,5);
  trigColourSizer->Add(m_L3TrigColourButton,0,wxEXPAND|wxLEFT|wxRIGHT,5);
  trigColourSizer->Add(m_L3HTrigColourButton,0,wxEXPAND|wxLEFT|wxRIGHT,5);

  wxBoxSizer *RFSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *firstRowSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *secondRowSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *thirdRowSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *thirdRowMiscSizer=new wxBoxSizer(wxVERTICAL);

  firstRowSizer->Add(m_RFOverviewChoice,1,wxEXPAND);
  firstRowSizer->Add(m_PayloadStyleChoice,1,wxEXPAND);
  secondRowSizer->Add(m_RFReferenceChoice,1,wxEXPAND);
  secondRowSizer->Add(checkBoxSizer,1,wxEXPAND);
  secondRowSizer->Add(m_RFTabChoice,1,wxEXPAND);
  thirdRowSizer->Add(thirdRowMiscSizer,1,wxEXPAND);
  thirdRowSizer->Add(trigColourSizer,0,wxEXPAND|wxLEFT,10);

  thirdRowMiscSizer->Add(rftabSizer,0,wxEXPAND);
  thirdRowMiscSizer->Add(clockrftabSizer,0,wxEXPAND);
  thirdRowMiscSizer->Add(m_ClockRFDisplayChoice,0,wxEXPAND);
  thirdRowMiscSizer->Add(m_CompassDisplayChoice,0,wxEXPAND);

  RFSizer->Add(firstRowSizer,0,wxEXPAND);
  RFSizer->Add(secondRowSizer,0,wxEXPAND);
  RFSizer->Add(thirdRowSizer,0,wxEXPAND);

  m_RFPage->SetSizerAndFit(RFSizer);
  
  m_notebook->InsertPage(RF_PAGE_ID,m_RFPage,_("RF View"),false);

  // RF RMS spec page
  wxPanel *m_RFSpecRMSPage = new wxPanel(m_notebook,wxID_ANY);
  m_RFSpecRMSChoice = new wxRadioBox(m_RFSpecRMSPage,wxID_ANY,
				     _T("Select RF channel"),
				     wxDefaultPosition,
				     wxDefaultSize,
				     N_RFANT,
				     AviewOpts::RFLabels,
				     N_CHAN-1,
				     wxRA_SPECIFY_ROWS);
  wxBoxSizer *rfspecRMSSizer=new wxBoxSizer(wxVERTICAL);
  rfspecRMSSizer->Add(m_RFSpecRMSChoice,1,wxEXPAND);
  m_RFSpecRMSPage->SetSizerAndFit(rfspecRMSSizer);
  
  m_notebook->InsertPage(RFSPECRMS_PAGE_ID,m_RFSpecRMSPage,_("RF RMS"),false);

  // Initialize RF subdialogs
  for(int i=0;i<N_RFANT;++i)
    m_RFSpecRMSDialog[i]=new wxRangeDialog(m_RFSpecRMSPage,wxID_ANY,
					   AviewOpts::RFLabels[i],
					   MAX_SPECS);

  // RMS absolute spec page
  wxPanel *m_RFSpecAbsPage = new wxPanel(m_notebook,wxID_ANY);
  m_RFSpecAbsChoice = new wxRadioBox(m_RFSpecAbsPage,wxID_ANY,
				     _T("Select RF channel"),
				     wxDefaultPosition,
				     wxDefaultSize,
				     N_RFANT,
				     AviewOpts::RFLabels,
				     N_CHAN-1,
				     wxRA_SPECIFY_ROWS);
  wxBoxSizer *rfspecAbsSizer=new wxBoxSizer(wxVERTICAL);
  rfspecAbsSizer->Add(m_RFSpecAbsChoice,1,wxEXPAND);
  m_RFSpecAbsPage->SetSizerAndFit(rfspecAbsSizer);
  
  m_notebook->InsertPage(RFSPECABS_PAGE_ID,m_RFSpecAbsPage,_("RF Absolute"),false);

  // Initialize RF subdialogs
  for(int i=0;i<N_RFANT;++i)
    m_RFSpecAbsDialog[i]=new wxRangeDialog(m_RFSpecAbsPage,wxID_ANY,
					   AviewOpts::RFLabels[i],
					   MAX_SPECS);

  // Disk page
  wxPanel *m_DiskPage = new wxPanel(m_notebook,wxID_ANY);
  m_DiskChoice = new wxRadioBox(m_DiskPage,wxID_ANY,
				_T("Select disk"),
				wxDefaultPosition,
				wxDefaultSize,
				N_DISK,
				AviewOpts::DiskLabels,
				N_DISK/2,
				wxRA_SPECIFY_ROWS);
  wxBoxSizer *diskSizer=new wxBoxSizer(wxVERTICAL);
  diskSizer->Add(m_DiskChoice,1,wxEXPAND);
  m_DiskPage->SetSizerAndFit(diskSizer);
  
  m_notebook->InsertPage(DISK_PAGE_ID,m_DiskPage,_("Disks"),false);
  
  // Initialize disk subdialogs
  for(int i=0;i<N_DISK;++i){
    m_DiskSpecDialog[i]=new wxRangeDialog(m_DiskPage,wxID_ANY,
					  AviewOpts::DiskLabels[i],
					  MAX_SPECS);
    m_DiskLimits[i][0]=new wxSpinCtrl(m_DiskSpecDialog[i],wxID_ANY);
    m_DiskLimits[i][0]->SetRange(0,100000);
    m_DiskLimits[i][1]=new wxSpinCtrl(m_DiskSpecDialog[i],wxID_ANY);
    m_DiskLimits[i][1]->SetRange(0,100000);
    m_DiskScale[i]=new wxRadioBox(m_DiskSpecDialog[i],wxID_ANY,_("Scaling"),wxDefaultPosition, 
				  wxDefaultSize,N_SCALETYPES,ScaleTypeLabels,1,
				  wxRA_SPECIFY_ROWS);
    m_DiskAverage[i]=new wxSpinCtrl(m_DiskSpecDialog[i],wxID_ANY);
    m_DiskAverage[i]->SetRange(1,wxDEFAULT_HISTORY_BUFFER);

    m_DiskSpecDialog[i]->AddExtraValControl(m_DiskLimits[i][0],_("Low"));
    m_DiskSpecDialog[i]->AddExtraValControl(m_DiskLimits[i][1],_("High"));
    m_DiskSpecDialog[i]->AddExtraValControl(m_DiskScale[i],_T(""));
    m_DiskSpecDialog[i]->AddExtraValControl(m_DiskAverage[i],_("Running Average"));
  }


  // Queue page
  wxPanel *m_QueuePage = new wxPanel(m_notebook,wxID_ANY);
  m_QueueChoice = new wxRadioBox(m_QueuePage,wxID_ANY,
				 _T("Select packet queue"),
				 wxDefaultPosition,
				 wxDefaultSize,
				 N_QUEUE,
				 AviewOpts::QueueLabels,
				 N_QUEUE/2,
				 wxRA_SPECIFY_ROWS);
  wxBoxSizer *queueSizer=new wxBoxSizer(wxVERTICAL);
  queueSizer->Add(m_QueueChoice,1,wxEXPAND);
  m_QueuePage->SetSizerAndFit(queueSizer);
  
  m_notebook->InsertPage(QUEUE_PAGE_ID,m_QueuePage,_("Queue"),false);
  
  // Initialize queue subdialogs
  for(int i=0;i<N_QUEUE;++i){
    m_QueueSpecDialog[i]=new wxRangeDialog(m_QueuePage,wxID_ANY,
				       AviewOpts::QueueLabels[i],
				       MAX_SPECS);
    m_QueueLimits[i][0]=new wxSpinCtrl(m_QueueSpecDialog[i],wxID_ANY);
    m_QueueLimits[i][0]->SetRange(0,10000);
    m_QueueLimits[i][1]=new wxSpinCtrl(m_QueueSpecDialog[i],wxID_ANY);
    m_QueueLimits[i][1]->SetRange(0,10000);
    m_QueueScale[i]=new wxRadioBox(m_QueueSpecDialog[i],wxID_ANY,_("Scaling"),wxDefaultPosition, 
				   wxDefaultSize,N_SCALETYPES,ScaleTypeLabels,1,
				   wxRA_SPECIFY_ROWS);
    m_QueueAverage[i]=new wxSpinCtrl(m_QueueSpecDialog[i],wxID_ANY);
    m_QueueAverage[i]->SetRange(1,wxDEFAULT_HISTORY_BUFFER);

    m_QueueSpecDialog[i]->AddExtraValControl(m_QueueLimits[i][0],_("Low"));
    m_QueueSpecDialog[i]->AddExtraValControl(m_QueueLimits[i][1],_("High"));
    m_QueueSpecDialog[i]->AddExtraValControl(m_QueueScale[i],_T(""));
    m_QueueSpecDialog[i]->AddExtraValControl(m_QueueAverage[i],_("Running Average"));
  }
  
  // Miscellaneous page
  wxPanel *m_MiscPage = new wxPanel(m_notebook,wxID_ANY);
  m_AuxChoice = new wxRadioBox(m_MiscPage,wxID_ANY,
			       _T("Select variable"),
			       wxDefaultPosition,
			       wxDefaultSize,
			       N_AUX,
			       AviewOpts::AuxLabels,
			       1,
			       wxRA_SPECIFY_ROWS);
  wxBoxSizer *miscSizer=new wxBoxSizer(wxVERTICAL);
  miscSizer->Add(m_AuxChoice,0,wxEXPAND);
  wxString GPSLabels[]={_T("ADU5"),_T("G12")};
  m_GPSChoice = new wxRadioBox(m_MiscPage,wxID_ANY,
			       _T("Default GPS"),
			       wxDefaultPosition,
			       wxDefaultSize,
			       2,GPSLabels,    
			       2,wxRA_SPECIFY_ROWS);
  miscSizer->Add(m_GPSChoice,0,wxEXPAND);

  wxBoxSizer *delaySizer =new wxBoxSizer(wxHORIZONTAL);
  delaySizer->Add(new wxStaticText(m_MiscPage,wxID_ANY,_T("Autoload delay [s] ")),0,
		  wxEXPAND);
  m_readDelayChoice = new wxSpinCtrl(m_MiscPage,wxID_ANY);
  m_readDelayChoice->SetRange(0,1000);
  delaySizer->Add(m_readDelayChoice,0,wxEXPAND|wxLEFT,45);

  wxBoxSizer *eventRateSizer =new wxBoxSizer(wxHORIZONTAL);
  eventRateSizer->Add(new wxStaticText(m_MiscPage,wxID_ANY,_T("Event rate update step [s] ")),0,
		      wxEXPAND);
  m_EventRateStepChoice = new wxSpinCtrl(m_MiscPage,wxID_ANY);
  m_EventRateStepChoice->SetRange(1,10000);
  eventRateSizer->Add(m_EventRateStepChoice,0,wxEXPAND);
  
  wxButton *editEventRate=new wxButton(m_MiscPage,EVENTRATE_BUTTON,_T("Edit event rate"));
  eventRateSizer->Add(editEventRate,0,wxLEFT,10);

  m_sqlDumpChoice=new wxCheckBox(m_MiscPage,wxID_ANY,_T("Show SQL queries"));
  m_crcCheckChoice=new wxCheckBox(m_MiscPage,wxID_ANY,_T("Enable checksum check"));
  m_fetchRunNumberChoice=new wxCheckBox(m_MiscPage,wxID_ANY,_T("Fetch run numbers"));

  miscSizer->Add(delaySizer,0,wxEXPAND);
  miscSizer->Add(eventRateSizer,0,wxEXPAND);
  miscSizer->Add(m_sqlDumpChoice,0,wxEXPAND);
  miscSizer->Add(m_crcCheckChoice,0,wxEXPAND);
  miscSizer->Add(m_fetchRunNumberChoice,0,wxEXPAND);

  m_MiscPage->SetSizerAndFit(miscSizer);  
  m_notebook->InsertPage(MISC_PAGE_ID,m_MiscPage,_("Misc"),false);
  
  // Initialize auxillary subdialogs
  for(int i=0;i<N_AUX;++i)
    m_AuxSpecDialog[i]=new wxRangeDialog(m_MiscPage,wxID_ANY,
					 AviewOpts::AuxLabels[i],
					 MAX_SPECS);
  // Initialize event rate subdialog
  m_EventRateSpecDialog=new wxRangeDialog(m_MiscPage,wxID_ANY,
					  _T("Event Rate"),MAX_SPECS);
  
  m_EventRateLimits[0]=new wxSpinCtrl(m_EventRateSpecDialog,wxID_ANY);
  m_EventRateLimits[0]->SetRange(0,10000);
  m_EventRateLimits[1]=new wxSpinCtrl(m_EventRateSpecDialog,wxID_ANY);
  m_EventRateLimits[1]->SetRange(0,10000);
  m_EventRateScale=new wxRadioBox(m_EventRateSpecDialog,wxID_ANY,_("Scaling"),wxDefaultPosition, 
				  wxDefaultSize,N_SCALETYPES,ScaleTypeLabels,1,
				  wxRA_SPECIFY_ROWS);
  m_EventRateAverage=new wxSpinCtrl(m_EventRateSpecDialog,wxID_ANY);
  m_EventRateAverage->SetRange(1,wxDEFAULT_HISTORY_BUFFER);
  m_EventRateSpecDialog->AddExtraValControl(m_EventRateLimits[0],_("Low"));
  m_EventRateSpecDialog->AddExtraValControl(m_EventRateLimits[1],_("High"));
  m_EventRateSpecDialog->AddExtraValControl(m_EventRateScale,_T(""));
  m_EventRateSpecDialog->AddExtraValControl(m_EventRateAverage,_("Running Average"));

  ImportOptions();
  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
  
  return;
}

/*
 * Destructor
 */
OptionsDialog::~OptionsDialog(){}

// This actually activates editing subdialogs
void OptionsDialog::OnApply(wxCommandEvent& event){
  // First identify which notebook page is opened
  switch(m_notebook->GetSelection()){
  case TEMP_PAGE_ID:
    // Launch subdialog
    m_TempSpecDialog[m_TempChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  case CURRENT_PAGE_ID:
    // Launch subdialog
    m_CurrentSpecDialog[m_CurrentChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  case VOLTAGE_PAGE_ID:
    // Launch subdialog
    m_VoltageSpecDialog[m_VoltageChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  case RFSPECRMS_PAGE_ID:
    // Launch subdialog
    m_RFSpecRMSDialog[m_RFSpecRMSChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  case RFSPECABS_PAGE_ID:
    // Launch subdialog
    m_RFSpecAbsDialog[m_RFSpecAbsChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  case DISK_PAGE_ID:
    // Launch subdialog
    m_DiskSpecDialog[m_DiskChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  case QUEUE_PAGE_ID:
    // Launch subdialog
    m_QueueSpecDialog[m_QueueChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  case MISC_PAGE_ID:
    // Launch subdialog
    m_AuxSpecDialog[m_AuxChoice->GetSelection()]->ShowModal(); // Any changes stored in dialog's internal rangeSpec array
    break;
  default:
    break;
  }
}

void OptionsDialog::OnEventRateEdit( wxCommandEvent &event ){
  m_EventRateSpecDialog->ShowModal();
}

void OptionsDialog::OnTrigColourEdit( wxCommandEvent &event ){
  switch(event.GetId()){
  case PHIMASK_BUTTON: 
    m_PhiMaskColourButton->SetBackgroundColour(wxGetColourFromUser(this,m_PhiMaskColourButton->GetBackgroundColour()));
    break;
  case PHIHMASK_BUTTON: 
    m_PhiHMaskColourButton->SetBackgroundColour(wxGetColourFromUser(this,m_PhiHMaskColourButton->GetBackgroundColour()));
    break;
  case L3TRIG_BUTTON: 
    m_L3TrigColourButton->SetBackgroundColour(wxGetColourFromUser(this,m_L3TrigColourButton->GetBackgroundColour()));
    break;
  case L3HTRIG_BUTTON: 
    m_L3HTrigColourButton->SetBackgroundColour(wxGetColourFromUser(this,m_L3HTrigColourButton->GetBackgroundColour()));
    break;
  default:
    break;
  }
}

void OptionsDialog::ExportOptions(){
  using namespace AviewOpts;
  for(int i=0;i<N_TEMP;++i){
    for(int j=0;j<MAX_SPECS;++j)
      TempSpec[i][j]=m_TempSpecDialog[i]->GetRangeSpec()[j];
    TempRange[i][0]=m_TempLimits[i][0]->GetValue();
    TempRange[i][1]=m_TempLimits[i][1]->GetValue();
    TempScaleType[i]=static_cast<ScaleType>(m_TempScale[i]->GetSelection());
    TempAverage[i]=m_TempAverage[i]->GetValue();
  }

  for(int i=0;i<N_DISK;++i){
    for(int j=0;j<MAX_SPECS;++j)
      DiskSpec[i][j]=m_DiskSpecDialog[i]->GetRangeSpec()[j];
    DiskRange[i][0]=m_DiskLimits[i][0]->GetValue();
    DiskRange[i][1]=m_DiskLimits[i][1]->GetValue();
    DiskScaleType[i]=static_cast<ScaleType>(m_DiskScale[i]->GetSelection());
    DiskAverage[i]=m_DiskAverage[i]->GetValue();
  }

  for(int i=0;i<N_QUEUE;++i){
    for(int j=0;j<MAX_SPECS;++j)
      QueueSpec[i][j]=m_QueueSpecDialog[i]->GetRangeSpec()[j];
    QueueRange[i][0]=m_QueueLimits[i][0]->GetValue();
    QueueRange[i][1]=m_QueueLimits[i][1]->GetValue();
    QueueScaleType[i]=static_cast<ScaleType>(m_QueueScale[i]->GetSelection());
    QueueAverage[i]=m_QueueAverage[i]->GetValue();
  }

  for(int i=0;i<N_CURRENT;++i)
    for(int j=0;j<MAX_SPECS;++j)
      CurrentSpec[i][j]=m_CurrentSpecDialog[i]->GetRangeSpec()[j];

  for(int i=0;i<N_VOLTAGE;++i)
    for(int j=0;j<MAX_SPECS;++j)
      VoltageSpec[i][j]=m_VoltageSpecDialog[i]->GetRangeSpec()[j];

  for(int i=0;i<N_POWER;++i)
    for(int j=0;j<MAX_SPECS;++j)
      PowerSpec[i][j]=m_PowerSpecDialog[i]->GetRangeSpec()[j];

  for(int i=0;i<N_RFANT;++i)
    for(int j=0;j<MAX_SPECS;++j){
      RFSpecRMS[i][j]=m_RFSpecRMSDialog[i]->GetRangeSpec()[j];
      RFSpecAbsolute[i][j]=m_RFSpecAbsDialog[i]->GetRangeSpec()[j];
    }
  
  RFOverview=static_cast<RFOverviewType>(m_RFOverviewChoice->GetSelection());
  PayloadStyle=static_cast<PayloadStyleType>(m_PayloadStyleChoice->GetSelection());
  RFReference=static_cast<RFReferenceType>(m_RFReferenceChoice->GetSelection());
  RFView=static_cast<RFViewType>(VoltageView*m_RFViewChoice[0]->IsChecked()+
				 FFTView*m_RFViewChoice[1]->IsChecked()+
				 PowerView*m_RFViewChoice[2]->IsChecked());
  RFTab=static_cast<RFTabType>(m_RFTabChoice->GetSelection());
  ClockRF=m_ClockRFDisplayChoice->IsChecked();
  Compass=m_CompassDisplayChoice->IsChecked();
  tabScaleMin=m_RFTabScaleMinChoice->GetValue();
  tabScaleMax=m_RFTabScaleMaxChoice->GetValue();
  clockTabScaleMin=m_ClockRFTabScaleMinChoice->GetValue();
  clockTabScaleMax=m_ClockRFTabScaleMaxChoice->GetValue();

  PhiMaskColour=m_PhiMaskColourButton->GetBackgroundColour();
  PhiHMaskColour=m_PhiHMaskColourButton->GetBackgroundColour();
  L3TrigColour=m_L3TrigColourButton->GetBackgroundColour();
  L3HTrigColour=m_L3HTrigColourButton->GetBackgroundColour();

  readDelay = m_readDelayChoice->GetValue();

  for(int i=0;i<N_AUX;++i)
    for(int j=0;j<MAX_SPECS;++j)
      AuxSpec[i][j]=m_AuxSpecDialog[i]->GetRangeSpec()[j];

  DefaultGPS=static_cast<GPSType>(m_GPSChoice->GetSelection());

  for(int j=0;j<MAX_SPECS;++j)
    EventRateSpec[j]=m_EventRateSpecDialog->GetRangeSpec()[j];
  EventRateRange[0]=m_EventRateLimits[0]->GetValue();
  EventRateRange[1]=m_EventRateLimits[1]->GetValue();
  EventRateScaleType=static_cast<ScaleType>(m_EventRateScale->GetSelection());
  EventRateAverage=m_EventRateAverage->GetValue();
  EventRateStep=m_EventRateStepChoice->GetValue();

  sqlDump=m_sqlDumpChoice->IsChecked();
  crcCheck=m_crcCheckChoice->IsChecked();
  fetchRunNumber=m_fetchRunNumberChoice->IsChecked();

  return;
}

void OptionsDialog::ImportOptions(){
  using namespace AviewOpts;

  for(int i=0;i<N_TEMP;++i){
    m_TempSpecDialog[i]->SetRangeSpec(TempSpec[i]);
    m_TempLimits[i][0]->SetValue((int)TempRange[i][0]);
    m_TempLimits[i][1]->SetValue((int)TempRange[i][1]);
    m_TempScale[i]->SetSelection(TempScaleType[i]);
    m_TempAverage[i]->SetValue(TempAverage[i]);
  }

  for(int i=0;i<N_DISK;++i){
    m_DiskSpecDialog[i]->SetRangeSpec(DiskSpec[i]);
    m_DiskLimits[i][0]->SetValue((int)DiskRange[i][0]);
    m_DiskLimits[i][1]->SetValue((int)DiskRange[i][1]);
    m_DiskScale[i]->SetSelection(DiskScaleType[i]);
    m_DiskAverage[i]->SetValue(DiskAverage[i]);
  }

  for(int i=0;i<N_QUEUE;++i){
    m_QueueSpecDialog[i]->SetRangeSpec(QueueSpec[i]);
    m_QueueLimits[i][0]->SetValue((int)QueueRange[i][0]);
    m_QueueLimits[i][1]->SetValue((int)QueueRange[i][1]);
    m_QueueScale[i]->SetSelection(QueueScaleType[i]);
    m_QueueAverage[i]->SetValue(QueueAverage[i]);
  }

  for(int i=0;i<N_CURRENT;++i)
    m_CurrentSpecDialog[i]->SetRangeSpec(CurrentSpec[i]);

  for(int i=0;i<N_VOLTAGE;++i)
    m_VoltageSpecDialog[i]->SetRangeSpec(VoltageSpec[i]);

  for(int i=0;i<N_POWER;++i)
    m_PowerSpecDialog[i]->SetRangeSpec(PowerSpec[i]);

  for(int i=0;i<N_RFANT;++i){
    m_RFSpecRMSDialog[i]->SetRangeSpec(RFSpecRMS[i]);
    m_RFSpecAbsDialog[i]->SetRangeSpec(RFSpecAbsolute[i]);
  }

  m_RFOverviewChoice->SetSelection(RFOverview);
  m_PayloadStyleChoice->SetSelection(PayloadStyle);
  m_RFReferenceChoice->SetSelection(RFReference);
  for(int i=0;i<3;++i)
    m_RFViewChoice[i]->SetValue(RFView&(0x1<<i));
  m_RFTabChoice->SetSelection(RFTab);
  m_ClockRFDisplayChoice->SetValue(ClockRF);
  m_CompassDisplayChoice->SetValue(Compass);
  m_RFTabScaleMinChoice->SetValue(tabScaleMin);
  m_RFTabScaleMaxChoice->SetValue(tabScaleMax);
  m_ClockRFTabScaleMinChoice->SetValue(clockTabScaleMin);
  m_ClockRFTabScaleMaxChoice->SetValue(clockTabScaleMax);

  m_PhiMaskColourButton->SetBackgroundColour(PhiMaskColour);
  m_PhiHMaskColourButton->SetBackgroundColour(PhiHMaskColour);
  m_L3TrigColourButton->SetBackgroundColour(L3TrigColour);
  m_L3HTrigColourButton->SetBackgroundColour(L3HTrigColour);

  m_readDelayChoice->SetValue(readDelay);

  for(int i=0;i<N_AUX;++i)
    m_AuxSpecDialog[i]->SetRangeSpec(AuxSpec[i]);
  m_GPSChoice->SetSelection(DefaultGPS);

  m_EventRateStepChoice->SetValue(EventRateStep);
  m_EventRateSpecDialog->SetRangeSpec(EventRateSpec);
  m_EventRateLimits[0]->SetValue((int)EventRateRange[0]);
  m_EventRateLimits[1]->SetValue((int)EventRateRange[1]);
  m_EventRateScale->SetSelection(EventRateScaleType);
  m_EventRateAverage->SetValue(EventRateAverage);

  m_sqlDumpChoice->SetValue(sqlDump);
  m_crcCheckChoice->SetValue(crcCheck);
  m_fetchRunNumberChoice->SetValue(fetchRunNumber);

  return;
}
