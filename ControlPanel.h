/* 
 * ControlPanel.h
 */

#ifndef _CONTROLPANELH_
#define _CONTROLPANELH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ControlPanel.cpp"
#endif

#include <wx/dialog.h>
#include "BaseControlItem.h"

class ControlPanel:public wxDialog{
  DECLARE_CLASS(ControlPanel)
 public:
  ControlPanel(wxWindow *parent,char *itemClassName, int n,const wxString &caption=wxT("Control Panel"));
  ~ControlPanel();

  void SetIndex(int type,int n);
  void SetIndex(int type,void *data);
  void SetTime(int type,long time,bool mark=false);
  time_t GetTime(int type);
  void SetList(int type,void *list,int n);
  void SetLabel(int type,const wxString &label);
  void Next(int type,wxCommandEvent &event);
  void Prev(int type,wxCommandEvent &event);
  void Last(int type,wxCommandEvent &event);
  void AdjustTimer(int type, int delay);
  void InverseTimers();
  void StartTimers(int type=-1);
  void StopTimers(int type=-1);
  void MarkIndex(int type);

 private:
  int nItem;
  BaseControlItem **m_ItemList;
};

#endif // _CONTROLPANELH_
