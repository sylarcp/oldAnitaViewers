/*
 * TextInfo.cpp
 *
 * Container class for multiple text based variable displays, like currents 
 * and voltages.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TextInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "TextInfo.h"

IMPLEMENT_CLASS( TextInfo, wxPanel )

TextInfo::TextInfo(){}
// Constructor
TextInfo::TextInfo( wxWindow* parent,int nItem, int itemPerCol, 
		    const wxString &boxLabel, const wxString &units,
		    const wxString *labels, const wxString *captions,
		    const wxString *dbVar, const wxString *dbTable, 
		    wxRangeSpec **rangeSpecs, const wxString &format) 
  :wxPanel(parent),m_format(format){

  // Create and arrange all children
  wxString unitLabel=boxLabel;
  if(units.Len()) unitLabel<< _T(" [")<<units<<']';
  wxStaticBox *textBox = new wxStaticBox(this,wxID_ANY,unitLabel);
  wxStaticBoxSizer *topSizer=new wxStaticBoxSizer(textBox,wxHORIZONTAL);
  wxBoxSizer *infoSizer;
  wxBoxSizer *labelSizer;
  m_Display = new wxRangedStaticText*[nItem];
  for(int i=0;i<nItem;++i){
    m_labels.Add(labels[i].Strip(wxString::both));
    if((i%itemPerCol)==0){
      labelSizer=new wxBoxSizer(wxVERTICAL);
      infoSizer=new wxBoxSizer(wxVERTICAL);
      topSizer->Add(labelSizer,0,wxEXPAND);
      topSizer->Add(infoSizer,1,wxEXPAND);
    }
  
    labelSizer->Add(new wxStaticText(this,wxID_ANY,labels[i]+_T(": "),
				     wxDefaultPosition,wxDefaultSize,
				     wxALIGN_LEFT), 0, wxRIGHT, 5);
    m_Display[i] = new wxRangedStaticText(this,wxID_ANY,rangeSpecs[i],m_format,
					  wxDefaultPosition,wxDefaultSize,0,
					  captions[i],_T("min"),units,dbVar[i],
					  dbTable[i]); 
    infoSizer->Add(m_Display[i],0,wxEXPAND);
  }

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);

}

TextInfo::~TextInfo(){}

void TextInfo::SetDisplayValue(const wxString &label, double val){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetLabel(val);
  else
    wxLogError(_T("[TextInfo::SetDisplayValue] Attempt to set value of unknown label '%s'"),label.c_str());
}

void TextInfo::SetDisplayRangeSpec(const wxString &label, const wxRangeSpec *rangeSpec){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetRangeSpec(rangeSpec);
  else
    wxLogError(_T("[TextInfo::SetDisplayRangeSpec] Attempt to set range on unknown label '%s'"),label.c_str());
}

