/* 
 * TrigMonOptions.h
 */

#ifndef _TRIGMONOPTIONSH_
#define _TRIGMONOPTIONSH_


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TrigMonOptions.cpp"
#endif


#include <wx/dialog.h>
#include <wx/notebook.h>

#include "trigmon_options.h"
#include "range.h"
#include "historydisplay.h"

#define N_BARINFO_OPTIONS 6

class TrigMonOptions:public wxDialog{
 public:
   TrigMonOptions(wxWindow *parent);
   ~TrigMonOptions();
   
   void OnApply( wxCommandEvent &event );
   void ImportOptions();
   void ExportOptions();

 protected:
   DECLARE_EVENT_TABLE()
     
 private:
   wxPanel *m_panel;

   wxRadioBox *m_SpecChoice;
   wxRadioBox *m_rfPowerTypeChoice;
   wxSpinCtrl *m_turfReadDelayChoice;
   wxSpinCtrl *m_surfReadDelayChoice;
   wxCheckBox *m_lockstepChoice;
   wxCheckBox *m_dynamicTargetChoice;
   wxCheckBox *m_sqlDumpChoice;
   wxCheckBox *m_crcCheckChoice;
   
   wxRangeDialog *m_SpecDialog[N_BARINFO_OPTIONS];
   wxSpinCtrl *m_Limits[N_BARINFO_OPTIONS][2];
   wxRadioBox *m_Scale[N_BARINFO_OPTIONS];
   wxSpinCtrl *m_Average[N_BARINFO_OPTIONS];

   DECLARE_CLASS( TrigMonOptions )
};

#endif // _TRIGMONOPTIONSH_
