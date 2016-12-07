/*
 * utcdatectrl.cpp
 *
 * Class that provides parsing of human readable UTC dates into UNIX timestamps.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "utcdatectrl.h"
#endif

#include <wx/msgdlg.h>
#include "utcdatectrl.h"

wxUTCDateCtrl::wxUTCDateCtrl(){}

wxUTCDateCtrl::wxUTCDateCtrl(wxWindow* parent, wxWindowID id, 
			     const wxString& value, 
			     const wxPoint& pos, 
			     const wxSize& size, 
			     long style, 
			     const wxValidator& validator, 
			     const wxString& name){
  Create(parent,id,value,pos,size,style,validator,name);
}

time_t wxUTCDateCtrl::GetTicks(int line){
  wxDateTime refTime;
  if(!refTime.ParseFormat(GetLineText(line),_T("%Y-%m-%d %H:%M:%S"))){
    wxMessageBox(_T("Unable to parse reference time. Please use YYYY-MM-DD HH:MM:SS"),_T("UTC time parse error"),wxOK|wxICON_ERROR);
    return -1;
  }else
    return refTime.GetTicks()-tzOffset+3600*refTime.IsDST();
}
 
time_t wxUTCDateCtrl::tzOffset=wxDateTime(1, wxDateTime::Jan, 1970).GetTicks();
