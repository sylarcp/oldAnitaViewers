/*
 * ADU5VtgViewer.h
 */

#ifndef _ADU5VTGVIEWERH_
#define _ADU5VTGVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ADU5VtgViewer.cpp"
#endif

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/file.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include "utcdatectrl.h"
#include <includes/anitaStructures.h>

//#include "BaseDBHandler.h"

//#define PREV_BUTTON_ID   10
//#define NEXT_BUTTON_ID   11
#define TERM5_CLEAR_BUTTON_ID   100005
#define SAVE_FILE5_ID  110005
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class ADU5VtgViewer:public wxDialog{
 public:
  ADU5VtgViewer(wxWindow *parent);
  ~ADU5VtgViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event);
void ADU5VtgData(const GpsAdu5VtgStruct_t &vtg);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(ADU5VtgViewer)
};

#endif // _ADU5VTGVIEWERH_







