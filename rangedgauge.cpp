/* 
 * rangedgauge.cpp
 *
 * Definitions for class that displayes variable as color coded gauge.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "rangedgauge.h"
#endif

#include "rangedgauge.h"
#include <wx/tooltip.h>
#include <wx/dcbuffer.h>
#include <wx/log.h>

IMPLEMENT_DYNAMIC_CLASS(wxRangedGauge, wxWindow)

BEGIN_EVENT_TABLE( wxRangedGauge, wxWindow )

  EVT_SIZE( wxRangedGauge::OnSize )
  EVT_PAINT( wxRangedGauge::OnPaint )
  EVT_LEFT_DCLICK( wxRangedGauge::OpenGraph)

END_EVENT_TABLE()

wxRangedGauge::wxRangedGauge(wxWindow *parent,
			     wxWindowID id,
			     double low,
			     double high,
			     const wxRangeSpec *rangeSpec,
			     ScaleType type,
			     unsigned int avg,
			     const wxPoint &pos,
			     const wxSize &size,
			     long style,
			     const wxString &name,
			     const wxString &xUnits,
			     const wxString &yUnits,
			     const wxString &dbVar,
			     const wxString &dbTable,
			     unsigned int buffer)
  :wxHistoryDisplay(1,name,&xUnits,&yUnits,&dbVar,dbTable,buffer),
   m_low(low),m_high(high),m_rangeSpec(rangeSpec),m_scale(type),m_avg(avg){

  Create(parent,id,pos,size,style,name);

  SetForegroundColour(m_OutOfRange_Colour);
  SetBackgroundColour(*wxBLACK);
  SetToolTip(wxString(_T("?")));
  GetToolTip()->SetDelay(500);
  SetValue(m_low);
  if(m_avg>wxDEFAULT_HISTORY_BUFFER) m_avg=wxDEFAULT_HISTORY_BUFFER;
}

wxRangedGauge::~wxRangedGauge(){}

void wxRangedGauge::GetRange(double *low,double *high){
  *low=m_low;
  *high=m_high;
}

void wxRangedGauge::SetRangeSpec(const wxRangeSpec *rangeSpec){
  m_rangeSpec=rangeSpec;
  _SetGauge(LocalValue());
}

void wxRangedGauge::SetRange(double low,double high){
  m_low=low;
  m_high=high;
  DrawBar(LocalValue());
}

void wxRangedGauge::SetScaleType(ScaleType type){
  m_scale=type;
  DrawBar(LocalValue());
}

void wxRangedGauge::SetAverage(unsigned int avg){
  m_avg=wxMax(wxMin(avg,wxDEFAULT_HISTORY_BUFFER),1);
  double val=LocalValue();
  _SetTip(val);
  _SetGauge(val);
}

void wxRangedGauge::SetGauge(double val){
  SetValue(val);
  double local_val=LocalValue();
  _SetTip(local_val);
  _SetGauge(local_val);
}

double wxRangedGauge::LocalValue(){
  double val=GetValue(0,m_avg);
  if(m_scale==Log) val=(val>0?log10(val):-99);
  return val;
}

void wxRangedGauge::_SetGauge(double val){
  int i;
  for(i=0;m_rangeSpec[i].colour.Ok();++i)
    if(val>=m_rangeSpec[i].low && val<m_rangeSpec[i].high) break;
  if(m_rangeSpec[i].colour.Ok()) SetForegroundColour(m_rangeSpec[i].colour);
  else SetForegroundColour(m_OutOfRange_Colour);

  DrawBar(val);
}

void wxRangedGauge::_SetTip(double val){
  wxString tmp;
  if(val==(long)val)
    tmp.Printf(_T("%ld"),(long)val);
  else{
    wxString fmt;
    fmt.Printf(_T("%%.%dg"),wxMin(m_avg+2,6));
    tmp.Printf(fmt,val);
  }
  SetToolTip(tmp);
}

void wxRangedGauge::OnPaint( wxPaintEvent& WXUNUSED(event) ){
  if(GetForegroundColour().Ok()) DrawBar(LocalValue());
}

void wxRangedGauge::OnSize( wxSizeEvent& WXUNUSED(event) ){
  if(GetForegroundColour().Ok()) DrawBar(LocalValue());
}

void wxRangedGauge::OpenGraph( wxMouseEvent& WXUNUSED(event) ){
  // Generate and open graph window
  if(!m_graph){
    m_graph=new wxMultiGraphDialog(this,wxID_ANY,m_name,m_dim,
				   m_xUnits,m_yUnits,m_dbVar,m_dbTable,
				   wxDefaultPosition,wxSize(250,100*m_dim));
    _NewCurve();
  }
  m_graph->Show();
}

void wxRangedGauge::DrawBar(double val){
  int client_width,client_height;
  GetClientSize(&client_width,&client_height);

  wxPaintDC dc(this);
  // Erase old display
  dc.SetPen(GetBackgroundColour());
  dc.SetBrush(GetBackgroundColour());
  dc.DrawRectangle(wxRect(0,0,client_width,client_height));
  
  dc.SetPen(GetForegroundColour());
  dc.SetBrush(GetForegroundColour());
  
  double fraction=(val-m_low)/(m_high-m_low);

  wxRect Bar;
  if(GetWindowStyle() & wxVERTICAL){ // Vertical bar
    Bar.SetTopLeft(wxPoint(1,1+(int)((1-fraction)*(client_height-2))));
    Bar.SetBottomRight(wxPoint(client_width-1,client_height-1));
  }else{ // Horizontal bar
    Bar.SetTopLeft(wxPoint(1,1));
    Bar.SetBottomRight(wxPoint(1+(int)(fraction*(client_width-2)),client_height-1));
  }

  dc.DrawRectangle(Bar);

}

wxColour wxRangedGauge::m_OutOfRange_Colour=wxColour(255,0,0);
