/*
 * BarInfo.cpp
 *
 * Top level class for display of bar like graphs (e.g. temperatures, trigger
 * rates, etc). It is a container for BarItem objects.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BarInfo.h"
#endif

#include <wx/statbox.h>
#include "BarInfo.h"

IMPLEMENT_CLASS( BarInfo, wxPanel )

bool BarInfo::Create( wxWindow* parent, int nItem, int itemPerRow,
		      const wxString &boxLabel, const wxString &units,
		      wxString *labels, wxString *refLabels, wxString *captions,
		      const wxString *dbVar, const wxString *dbTable,
		      wxRangeSpec **rangeSpecs, double **ranges,
		      unsigned int *avgs,ScaleType *scaleTypes,
		      const wxSize &size,const wxString yScale){

  if(!wxPanel::Create(parent,wxID_ANY)) return false;

  wxFont labelFont=wxFont(7,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,0,_T(""),wxFONTENCODING_SYSTEM);

  // Create and arrange all children
  wxString unitLabel=boxLabel;
  if(units.Len()) unitLabel<< _T(" [")<<units<<_T("]");
  wxStaticBox *barBox = new wxStaticBox(this,wxID_ANY,unitLabel);
  wxStaticBoxSizer *topSizer=new wxStaticBoxSizer(barBox,wxVERTICAL);

  wxBoxSizer *infoSizer=NULL;
  wxString label;
  m_Display = new BarItem*[nItem];
  // ddm 2014: this does not work is nItem is not a multiple of itemPerRow
  // for example 54/10 produces a nRow equal to 5 and the for loop below
  // will write out of the array bounds
  // nRows=nItem/itemPerRow;
  nRows=(nItem+itemPerRow-1)/itemPerRow;

  m_scale=new wxBoxSizer*[nRows];
  for(int i=0;i<nItem;++i){
    m_labels.Add(refLabels[i].Strip(wxString::both));
    if((i%itemPerRow)==0){
      infoSizer=new wxBoxSizer(wxHORIZONTAL);
      topSizer->Add(infoSizer,1,wxEXPAND);
      label.Clear();
      m_scale[i/itemPerRow]=new wxBoxSizer(wxVERTICAL);
      infoSizer->Add(m_scale[i/itemPerRow],0,wxEXPAND);
    }
    m_Display[i]=new BarItem(this,wxID_ANY,labels[i],captions[i],rangeSpecs[i],
			     ranges[i],scaleTypes[i],avgs[i],size,_T("min"),units,dbVar[i],
			     dbTable[i],&labelFont);

    infoSizer->Add(m_Display[i],1,wxEXPAND);

  }

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);
  SetScale(yScale);

  return true;
}

BarInfo::~BarInfo(){}

void BarInfo::SetDisplayValue(const wxString &label, double val){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetDisplayValue(val);
  else
    wxLogError(_T("[BarInfo::SetDisplayValue] Attempt to set value of unknown label '%s'"),label.c_str());
}

void BarInfo::SetDisplayRange(const wxString &label, double low, double high){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetDisplayRange(low,high);
  else
    wxLogError(_T("[BarInfo::SetDisplayRange] Attempt to set range on unknown label '%s'"),label.c_str());
}

void BarInfo::SetDisplayRangeSpec(const wxString &label,const wxRangeSpec *rangeSpec){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetDisplayRangeSpec(rangeSpec);
  else
    wxLogError(_T("[BarInfo::SetDisplayRangeSpec] Attempt to set range color spec on unknown label '%s'"),label.c_str());
}

void BarInfo::SetDisplayScaleType(const wxString &label, ScaleType type){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetScaleType(type);
  else
    wxLogError(_T("[BarInfo::SetScaleType] Attempt to set scaling on unknown label '%s'"),label.c_str());
}

void BarInfo::SetDisplayAverage(const wxString &label, unsigned int avg){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetAverage(avg);
  else
    wxLogError(_T("[BarInfo::SetScaleType] Attempt to set average on unknown label '%s'"),label.c_str());
}

void BarInfo::SetDisplayBackground(const wxString &label,const wxColour &colour){
  int n=m_labels.Index(label.Strip(wxString::both));
  if(n!=wxNOT_FOUND)
    m_Display[n]->SetBackground(colour);
  else
    wxLogError(_T("[BarInfo::SetScaleType] Attempt to set background on unknown label '%s'"),label.c_str());
}

void BarInfo::SetScale(const wxString &yScale){
  // Erase current scale objects
  for(int i=0;i<nRows;++i){
    int n=m_scale[i]->GetChildren().GetCount();
    for(int j=0;j<n;++j){
      wxSizerItem *item=m_scale[i]->GetItem((size_t)0);
      m_scale[i]->Detach(0);
      wxWindow *win;
      if((win=item->GetWindow())) delete win;
    }
  }
  if(!yScale.Length()) return;

  // Parse yScale info
  bool scaleOn=false;
  wxString *scale=NULL;
  int scaleLength=0;
  if(yScale.Length()){
    wxString str=yScale;
    if((scaleLength=str.Freq(',')+1)>1){  // There is meaningfull scale
      scale=new wxString[scaleLength];
      int c=0;
      while(str.Length()){
	scale[c]=str.BeforeFirst(',') << ' ';
	str=str.AfterFirst(',');
	++c;
      }
      if(c!=scaleLength) scaleLength=c;
    }
    if(scaleLength>1) scaleOn=true;
  }
  if(!scaleOn) return;

  if(scaleOn){
    // Estimate spacer size we'll need
    int w,h;
    wxFont scaleFont=wxFont(6,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,0,_T(""),wxFONTENCODING_SYSTEM);
    GetTextExtent(_T("T"),&w,&h,NULL,NULL,&scaleFont);
    for(int i=0;i<nRows;++i){
      for(int j=scaleLength-1;j>=0;--j){
	wxStaticText *scaleText=new wxStaticText(this,wxID_ANY,scale[j],
						 wxDefaultPosition,
						 wxDefaultSize,
						 wxALIGN_RIGHT);
	scaleText->SetFont(scaleFont);
	m_scale[i]->Add(scaleText,0,wxEXPAND);
	if(j>0) m_scale[i]->AddStretchSpacer();
      }
      // Add spacer
      m_scale[i]->Add(0,h);
    }
  }
  GetSizer()->Layout();
}
