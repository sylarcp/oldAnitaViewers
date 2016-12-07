/* 
 * rangedstattext.cpp
 *
 * Definitions for class that displays variable as color coded text.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "rangedstattext.h"
#endif

#include "rangedstattext.h"

IMPLEMENT_DYNAMIC_CLASS(wxRangedStaticText, wxHistoryStaticText)

wxRangedStaticText::wxRangedStaticText(wxWindow *parent,
				       wxWindowID id,
				       const wxRangeSpec *rangeSpec,
				       const wxString &format,
				       const wxPoint &pos,
				       const wxSize &size,
				       long style,
				       const wxString &name,
				       const wxString &xUnits,
				       const wxString &yUnits,
				       const wxString &dbVar,
				       const wxString &dbTable,
				       unsigned int buffer)
  :wxHistoryStaticText(parent,id,1,format,_T(""),pos,size,style,name,&xUnits,
		       &yUnits,&dbVar,dbTable,buffer),m_rangeSpec(rangeSpec){}

wxRangedStaticText::~wxRangedStaticText(){}

void wxRangedStaticText::SetLabel(double val){
  SetValue(val);

  int i;
  for(i=0;m_rangeSpec[i].colour.Ok();++i)
    if(val>=m_rangeSpec[i].low && val<m_rangeSpec[i].high) break;
  if(m_rangeSpec[i].colour.Ok()) SetForegroundColour(m_rangeSpec[i].colour);
  else SetForegroundColour(m_OutOfRange_Colour);
  
  wxString tmp;
  tmp.Printf(m_format.c_str(),val);
  wxStaticText::SetLabel(tmp);
}

void wxRangedStaticText::SetRangeSpec(const wxRangeSpec *rangeSpec){
  m_rangeSpec=rangeSpec;
  SetLabel(GetValue(0));
}

wxColour wxRangedStaticText::m_OutOfRange_Colour=wxColour(255,0,0);


