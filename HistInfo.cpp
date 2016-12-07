/*
 * HistInfo.cpp
 *
 * Definitions for class used for histogramming. It is used for 
 * display of cumulative trigger patterns.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "HistInfo.h"
#endif

#include "HistInfo.h"

IMPLEMENT_CLASS( HistInfo, BarInfo )

HistInfo::HistInfo(){}
// Constructor
HistInfo::HistInfo(wxWindow* parent,int nItem, int itemPerRow,
		   const wxString &boxLabel, const wxString &units,
		   const wxSize &size)
  :m_nItem(nItem),m_scaleMin(0),m_scaleMax(1){
  
  // Prepare variables that are handled internally 
  wxString *labels=new wxString[nItem];
  wxString *captions=new wxString[nItem];
  wxString *dbVar=new wxString[nItem];
  wxString *dbTable=new wxString[nItem];
  wxRangeSpec **rangeSpecs=new wxRangeSpec*[nItem];
  double **ranges=new double*[nItem];
  unsigned int *avgs=new unsigned int[nItem];
  ScaleType *scaleTypes=new ScaleType[nItem];
  wxString yScale;

  for(int i=0;i<nItem;++i){
    labels[i].Printf(_T("%d"),i+1);
    captions[i].Printf(_T("%s %d"),boxLabel.c_str(),i+1);
    rangeSpecs[i]=new wxRangeSpec[MAX_SPECS+1];
    rangeSpecs[i][0]=wxRangeSpec(-HUGE_VAL,HUGE_VAL,wxWHITE);
    rangeSpecs[i][1]=wxRangeSpec();
    ranges[i]=new double[2];
    ranges[i][0]=m_scaleMin;ranges[i][1]=m_scaleMax;
    avgs[i]=1;
    scaleTypes[i]=Linear;
    yScale.Printf(_T("%.2g,%.2g"),m_scaleMin,m_scaleMax);
  }

  BarInfo::Create(parent,nItem,itemPerRow,boxLabel,units,labels,labels,captions,
		  dbVar,dbTable,rangeSpecs,ranges,avgs,scaleTypes,size,yScale);
  
  m_value=new double[nItem];
  for(int i=0;i<m_nItem;++i) m_value[i]=0;
}

HistInfo::~HistInfo(){
  delete[] m_value;
}

void HistInfo::Clear(){
  wxString tmp;
  m_scaleMin=0;
  m_scaleMax=1;

  for(int i=0;i<m_nItem;++i){
    tmp.Printf(_T("%d"),i+1);
    SetDisplayRange(tmp,m_scaleMin,m_scaleMax);
    SetDisplayValue(tmp,0);
  }
}

void HistInfo::SetValues(double *val){
  for(int i=0;i<m_nItem;++i) m_value[i]=val[i];
  // Find min and max and change scales if needed
  bool newScale=false;
  for(int i=0;i<m_nItem;++i){
    if(m_value[i]<m_scaleMin){
      m_scaleMin=floor(m_value[i]);
      newScale=true;
    }else if(m_value[i]>m_scaleMax){
      m_scaleMax=ceil(m_value[i]+(fabs(m_value[i])*0.1)); 
      newScale=true;
    }
  }
  wxString tmp;
  if(newScale){
    wxString yScale;
    yScale.Printf(_T("%.2g,%.2g"),m_scaleMin,m_scaleMax);
    SetScale(yScale);
  }

  for(int i=0;i<m_nItem;++i){
    tmp.Printf(_T("%d"),i+1);
    if(newScale) SetDisplayRange(tmp,m_scaleMin,m_scaleMax);
    SetDisplayValue(tmp,m_value[i]);
  }
}

void HistInfo::IncrementValues(double *val){
  for(int i=0;i<m_nItem;++i) m_value[i]+=val[i];
  SetValues(m_value);
}


