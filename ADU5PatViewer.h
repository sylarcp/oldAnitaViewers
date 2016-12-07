/*
 * ADU5PatViewer.h
 */

#ifndef _ADU5PATVIEWERH_
#define _ADU5PATVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ADU5PatViewer.cpp"
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
#define TERM3_CLEAR_BUTTON_ID   100003
#define SAVE_FILE3_ID  110003
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class ADU5PatViewer:public wxDialog{
 public:
  ADU5PatViewer(wxWindow *parent);
  ~ADU5PatViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event); 
void ADU5PatData(const GpsAdu5PatStruct_t &pat);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(ADU5PatViewer)
};

#endif // _ADU5PATVIEWERH_





