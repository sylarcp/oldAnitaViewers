/* 
 * OptionsDialog.h
 */

#ifndef _OPTIONSDIALOGH_
#define _OPTIONSDIALOGH_


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "OptionsDialog.cpp"
#endif


#include <wx/dialog.h>
#include <wx/notebook.h>

#include "options.h"
#include "range.h"
#include "historydisplay.h"

#define TEMP_PAGE_ID         0x00
#define CURRENT_PAGE_ID      0x01
#define VOLTAGE_PAGE_ID      0x02
#define RF_PAGE_ID           0x03
#define RFSPECRMS_PAGE_ID    0x04
#define RFSPECABS_PAGE_ID    0x05
#define DISK_PAGE_ID         0x06
#define QUEUE_PAGE_ID        0x07
#define MISC_PAGE_ID         0x08

#define PHIMASK_BUTTON        0x101
#define PHIHMASK_BUTTON        0x102
#define L3TRIG_BUTTON        0x103
#define L3HTRIG_BUTTON       0x104
#define EVENTRATE_BUTTON     0x105

class OptionsDialog:public wxDialog{
 public:
  OptionsDialog(wxWindow *parent);
  ~OptionsDialog();

  void OnApply( wxCommandEvent &event );
  void OnTrigColourEdit( wxCommandEvent &event );
  void OnEventRateEdit( wxCommandEvent &event );

  void ImportOptions();
  void ExportOptions();

 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxPanel *m_panel;
  wxNotebook *m_notebook;

  wxRadioBox *m_TempChoice;
  wxRadioBox *m_DiskChoice;
  wxRadioBox *m_QueueChoice;
  wxRadioBox *m_CurrentChoice;
  wxRadioBox *m_VoltageChoice;
  wxRadioBox *m_PowerChoice;
  wxRadioBox *m_RFSpecRMSChoice;
  wxRadioBox *m_RFSpecAbsChoice;
  wxRadioBox *m_RFOverviewChoice;
  wxRadioBox *m_RFTabChoice;
  wxCheckBox *m_RFViewChoice[5];
  wxRadioBox *m_PayloadStyleChoice;
  wxRadioBox *m_RFReferenceChoice;
  wxCheckBox *m_ClockRFDisplayChoice;
  wxCheckBox *m_CompassDisplayChoice;
  wxButton *m_PhiMaskColourButton;
  wxButton *m_PhiHMaskColourButton;
  wxButton *m_L3TrigColourButton;
  wxButton *m_L3HTrigColourButton;
  wxSpinCtrl *m_RFTabScaleMinChoice;
  wxSpinCtrl *m_RFTabScaleMaxChoice;
  wxSpinCtrl *m_ClockRFTabScaleMinChoice;
  wxSpinCtrl *m_ClockRFTabScaleMaxChoice;
  wxSpinCtrl *m_readDelayChoice;
  wxRadioBox *m_AuxChoice;
  wxRadioBox *m_GPSChoice;
  wxCheckBox *m_sqlDumpChoice;
  wxCheckBox *m_crcCheckChoice;
  wxCheckBox *m_fetchRunNumberChoice;
  wxSpinCtrl *m_EventRateStepChoice;
 
  wxRangeDialog *m_TempSpecDialog[N_TEMP];
  wxSpinCtrl *m_TempLimits[N_TEMP][2];
  wxRadioBox *m_TempScale[N_TEMP];
  wxSpinCtrl *m_TempAverage[N_TEMP];
  wxRangeDialog *m_DiskSpecDialog[N_DISK];
  wxSpinCtrl *m_DiskLimits[N_DISK][2];
  wxRadioBox *m_DiskScale[N_DISK];
  wxSpinCtrl *m_DiskAverage[N_TEMP];
  wxRangeDialog *m_QueueSpecDialog[N_QUEUE];
  wxSpinCtrl *m_QueueLimits[N_QUEUE][2];
  wxRadioBox *m_QueueScale[N_QUEUE];
  wxSpinCtrl *m_QueueAverage[N_TEMP];
  wxRangeDialog *m_CurrentSpecDialog[N_CURRENT];
  wxRangeDialog *m_VoltageSpecDialog[N_VOLTAGE];
  wxRangeDialog *m_PowerSpecDialog[N_POWER];
  wxRangeDialog *m_RFSpecRMSDialog[N_RFANT];
  wxRangeDialog *m_RFSpecAbsDialog[N_RFANT];
  wxRangeDialog *m_AuxSpecDialog[N_AUX];
  wxRangeDialog *m_EventRateSpecDialog;
  wxSpinCtrl *m_EventRateLimits[2];
  wxRadioBox *m_EventRateScale;
  wxSpinCtrl *m_EventRateAverage;

  DECLARE_CLASS( OptionsDialog )
};

#endif // _OPTIONSDIALOGH_
