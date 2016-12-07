/* 
 * FlightControlItem.h
 */

#ifndef _FLIGHTCONTROLITEMH_
#define _FLIGHTCONTROLITEMH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "FlightControlItem.cpp"
#endif

#include <wx/arrstr.h>
#include "BaseControlItem.h"

// Event used to load files off disk
BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxLOAD_FILE,-1)
END_DECLARE_EVENT_TYPES()

class FlightControlItem:public BaseControlItem{

  DECLARE_DYNAMIC_CLASS( FlightControlItem )

 public:
  FlightControlItem();
  FlightControlItem(wxWindow *parent, int type);
  ~FlightControlItem();

  void Create(wxWindow *parent, int type);

  // Event handlers
  void Next(wxCommandEvent &event);
  void Prev(wxCommandEvent &event);
  void Last(wxCommandEvent &event);
  void Select(wxCommandEvent &event);
  void TimeSelect(wxCommandEvent &WXUNUSED(event)){}
  
  void SetList(void *list,int n);
  void SetIndex(int n);
  void MarkIndex(){}

 private:
  int m_subIndex;
  int m_subMax;
  wxArrayString *m_list;
};

#endif // _FLIGHTCONTROLITEMH_
