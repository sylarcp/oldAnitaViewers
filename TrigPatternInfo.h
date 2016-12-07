/* 
 * TrigPatternInfo.h
 */

#ifndef _TRIGPATTERNINFOH_
#define _TRIGPATTERNINFOH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TrigPatternInfo.cpp"
#endif

#include <wx/dialog.h>
#include "BaseDBHandler.h"
#include "HistInfo.h"

#define REQUEST_BUTTON_ID 10
#define NOW_BUTTON_ID     11
#define HISTCLEAR_BUTTON_ID   12

#define TIME_DISPLAY     1001
#define LENGTH_DISPLAY   1002
#define FILTER_DISPLAY   1003

class TrigPatternInfo:public wxDialog, public BaseDBHandler{
 public:
  TrigPatternInfo(wxWindow *parent);
  ~TrigPatternInfo();

  void SetPattern(unsigned short l3, unsigned short l3H);

  // Event handlers
  void OnRequest(wxCommandEvent &event);
  void OnNow(wxCommandEvent &event);
  void OnClear(wxCommandEvent &event);

 protected:
  DECLARE_EVENT_TABLE()
  void sqlParser(PGresult *result);  

 private:
  HistInfo *m_l3;
  HistInfo *m_l3H;

  wxUTCDateCtrl *m_refTimeEntry;
  wxTextCtrl *m_lengthEntry;
  wxTextCtrl *m_sqlFilter;
  long m_refTick;

  DECLARE_CLASS(TrigPatternInfo)
};

#endif // _TRIGPATTERNINFOH_
