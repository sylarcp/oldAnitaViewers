/*
 * BarItem.cpp
 *
 * Object that displays one variable with bar-like gauge and provides its label
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BarItem.h"
#endif

#include <wx/sizer.h>
#include "BarItem.h"

IMPLEMENT_CLASS( BarItem, wxPanel )

BarItem::BarItem(){}
// Constructor
BarItem::BarItem(wxWindow* parent,wxWindowID id,const wxString label, 
		 const wxString caption,const wxRangeSpec *rangeSpec, 
		 double *range,ScaleType type,const unsigned int avg,const wxSize &size, 
		 const wxString &xUnits,const wxString &yUnits,
		 const wxString &dbVar, const wxString &dbTable,
		 const wxFont *font)
  :wxPanel(parent,id){
  
  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
  
  m_gauge=new wxRangedGauge(this,wxID_ANY,range[0],range[1],
   			    rangeSpec,type,avg,wxDefaultPosition,size,
  			    wxVERTICAL,caption,xUnits,yUnits,
  			    dbVar,dbTable);
  sizer->Add(m_gauge,1,wxEXPAND);
  m_label=new wxStaticText(this,wxID_ANY,label,wxDefaultPosition,
  			   wxDefaultSize,wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
  m_label->SetFont(*font);
  sizer->Add(m_label,0,wxEXPAND);

  sizer->CalcMin();
  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
}

BarItem::~BarItem(){}

void BarItem::SetDisplayValue(double val){
  m_gauge->SetGauge(val);
}

void BarItem::SetDisplayRange(double low, double high){
  m_gauge->SetRange(low,high);
}

void BarItem::SetDisplayRangeSpec(const wxRangeSpec *rangeSpec){
  m_gauge->SetRangeSpec(rangeSpec);
}

void BarItem::SetLabel(const wxString &label){
  m_label->SetLabel(label);
}

void BarItem::SetScaleType(ScaleType type){
  m_gauge->SetScaleType(type);
}

void BarItem::SetAverage(unsigned int avg){
  m_gauge->SetAverage(avg);
}

void BarItem::SetBackground(const wxColour &colour){
  m_gauge->SetBackgroundColour(colour);
}
