/*
 * GpsPosViewer.h
 */

#ifndef _GPSPOSVIEWERH_
#define _GPSPOSVIEWERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "GpsPosViewer.cpp"
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
#define TERM_CLEAR_BUTTON_ID   100000
#define SAVE_FILE_ID  110000
//#define FILE_SELECT_ID1   111

//#define TIME_DISPLAY_ID  1001

//#define N_CONFIG_FILE_TYPES1  9

class GpsPosViewer:public wxDialog{
 public:
  GpsPosViewer(wxWindow *parent);
  ~GpsPosViewer();

 void OnClear(wxCommandEvent &event); 
 void SaveFile(wxCommandEvent &event); 
void GpsPosData(const GpsG12PosStruct_t &gps);
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxTextCtrl *m_display;
  wxTextCtrl *m1_display;
  //wxChoice *m_fileList;
 // wxUint8 *wxUint;
// wxString m_fileNames[N_CONFIG_FILE_TYPES1];

 DECLARE_CLASS(GpsPosViewer)
};

#endif // _GPSPOSVIEWERH_


