/* 
 * historystattext.cpp
 *
 * Definitions for class that displays variables with memory.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "historystattext.h"
#endif

#include "historystattext.h"

IMPLEMENT_DYNAMIC_CLASS(wxHistoryStaticText, wxStaticText)

BEGIN_EVENT_TABLE( wxHistoryStaticText, wxStaticText )
  EVT_LEFT_DCLICK( wxHistoryStaticText::OpenGraph)
END_EVENT_TABLE()

wxHistoryStaticText::wxHistoryStaticText(wxWindow *parent,
					 wxWindowID id,
					 int dim,
					 const wxString &format,
					 const wxString &separator,
					 const wxPoint &pos,
					 const wxSize &size,
					 long style,
					 const wxString &name,
					 const wxString *xUnits,
					 const wxString *yUnits,
					 const wxString *dbVar,
					 const wxString &dbTable,
					 unsigned int buffer)
  :wxHistoryDisplay(dim,name,xUnits,yUnits,dbVar,dbTable,buffer),
   m_format(format),m_separator(separator){
  Create(parent,id,_T(""),pos,size,style,name);
}

wxHistoryStaticText:: wxHistoryStaticText(wxWindow *parent,
					  wxWindowID id,
					  const wxString &format,
					  const wxString &name,
					  const wxString &xUnits,
					  const wxString &yUnits,
					  const wxString &dbVar,
					  const wxString &dbTable,
					  unsigned int buffer)
  :wxHistoryDisplay(1,name,&xUnits,&yUnits,&dbVar,dbTable,buffer),
   m_format(format),m_separator(_T("")){
  Create(parent,id,_T(""),wxDefaultPosition,wxDefaultSize,wxST_NO_AUTORESIZE);
}

wxHistoryStaticText::~wxHistoryStaticText(){}

void wxHistoryStaticText::SetLabel(double *val){
  SetValues(val);
  
  wxString label,tmp;
  for(unsigned int i=0;i<m_dim;++i){
    tmp.Printf(m_format.c_str(),GetValue(i));
    label+=tmp;
    if(i<m_dim-1) label+=m_separator;
  }

  wxStaticText::SetLabel(label);
}

void wxHistoryStaticText::OpenGraph( wxMouseEvent& WXUNUSED(event) ){
  // Generate (if needed) and open graph window
  if(!m_graph){
    m_graph=new wxMultiGraphDialog(this,wxID_ANY,m_name,m_dim,
				   m_xUnits,m_yUnits,m_dbVar,m_dbTable,
				   wxDefaultPosition,wxSize(250,100*m_dim));
    _NewCurve();
  }
  m_graph->Show();
}


