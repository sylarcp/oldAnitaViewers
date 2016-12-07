/* 
 * SlowMoOptions.h
 */

#ifndef _SLOWMOOPTIONSH_
#define _SLOWMOOPTIONSH_


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SlowMoOptions.cpp"
#endif


#include <wx/dialog.h>
#include <wx/notebook.h>

#include "slowmo_options.h"
#include "range.h"
#include "historydisplay.h"

#define N_BARINFO_OPTIONS 5

class SlowMoOptions:public wxDialog{
 public:
   SlowMoOptions(wxWindow *parent);
   ~SlowMoOptions();
   
   void OnApply( wxCommandEvent &event );
   void ImportOptions();
   void ExportOptions();

 protected:
   DECLARE_EVENT_TABLE()
     
 private:
   wxPanel *m_panel;

   wxRadioBox *m_SpecChoice;
   wxRadioBox *m_rfPowerTypeChoice;
   wxSpinCtrl *m_readDelayChoice;
   wxCheckBox *m_sqlDumpChoice;
   
   wxRangeDialog *m_SpecDialog[N_BARINFO_OPTIONS];
   wxSpinCtrl *m_Limits[N_BARINFO_OPTIONS][2];
   wxRadioBox *m_Scale[N_BARINFO_OPTIONS];
   wxSpinCtrl *m_Average[N_BARINFO_OPTIONS];

   DECLARE_CLASS( SlowMoOptions )
};

#endif // _SLOWMOOPTIONSH_
