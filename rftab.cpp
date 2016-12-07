/*
 * rftab.cpp
 *
 * Definitions for class that displays RF information in small tab windows.
 * It has a hook that calls on full RF display.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "rftab.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/tooltip.h>
#include <wx/dcbuffer.h>
#include "rftab.h"

IMPLEMENT_CLASS( wxRFTab, wxWindow )

BEGIN_EVENT_TABLE( wxRFTab, wxWindow )

  EVT_SIZE( wxRFTab::OnSize )
  EVT_PAINT( wxRFTab::OnPaint )
  EVT_LEFT_DOWN( wxRFTab::OnMouseClick )

END_EVENT_TABLE()

/*
 * Constructor
 */
wxRFTab::wxRFTab( wxWindow* parent, const wxString &label, wxRFWindow *full, 
		  wxWindowID id, const wxPoint& pos, 
		  const wxSize& size, long style )
  :wxWindow(parent,id,pos,size,style),m_scaleMin(-1),m_scaleMax(1),
   m_colour(wxColour(255,255,255)), m_bgcolour(wxColour(0,0,0)),fullData(NULL),fullDisplay(full){
    
  //SetBackgroundColour( m_bgcolour );
  SetMinSize(size);

  SetToolTip(label);
  GetToolTip()->SetDelay(1000); // CONFIG

  tabLength = size.GetWidth()-2;
  //double x[2]={0.,1.};
  //double y[2]={0.,0.};
  tabData = NULL;//new wxGraphCurve(x,y,2);
  DrawCurve();

  return;
}

/*
 * Destructor
 */
wxRFTab::~wxRFTab(){
  if(tabData) delete tabData;
}

/*
 * wxEVT_IDLE event handler 
 */
void wxRFTab::OnSize( wxSizeEvent& event ){
  tabLength = GetSize().GetWidth()-2;
  MakeTabData();
  DrawCurve();
  event.Skip();
}

/*
 * wxEVT_PAINT event handler 
 */
void wxRFTab::OnPaint( wxPaintEvent &WXUNUSED(event) ){
  DrawCurve();
}

void wxRFTab::SetScale(int min,int max){
  m_scaleMin=min;
  m_scaleMax=max;
  DrawCurve();
}

void wxRFTab::DrawCurve(){
  // Draw Tab RF here  
  wxPaintDC dc(this);

  dc.SetPen(wxPen(m_colour));
  
  wxRect frameBox(wxPoint(0,0),GetSize());
  dc.SetBrush(wxBrush(m_bgcolour));
  //dc.SetBrush(wxBrush(GetBackgroundColour()));
  dc.DrawRectangle(frameBox);

  if(!tabData) return;

  int client_width,client_height;
  GetClientSize(&client_width,&client_height);

  double maxY=wxMax(m_scaleMax,tabData->GetMaxY());
  double minY=wxMin(m_scaleMin,tabData->GetMinY());
  double dy=(maxY-minY)/(double)client_height;
  double dx=(tabData->GetMaxX()-tabData->GetMinX())/(double)(client_width-2);
  for(unsigned int i=1;i<tabData->GetLength();++i){
    int y1=(int)((maxY-(*tabData)[i-1].y)/dy)+1;
    int y2=(int)((maxY-(*tabData)[i].y)/dy)+1;
    int x1=(int)(((*tabData)[i-1].x-tabData->GetMinX())/dx)+1;
    int x2=(int)(((*tabData)[i].x-tabData->GetMinX())/dx)+1;
    dc.DrawLine(x1,y1,x2,y2);
  }

}

void wxRFTab::OnMouseClick( wxMouseEvent& event ){
  // Here we initiate opening of full RF window
  if(fullDisplay) fullDisplay->Show();
}

void wxRFTab::SetData(wxGraphCurve *data){
  fullData = data;
  MakeTabData();
}
 
void wxRFTab::MakeTabData(){
  // Clear old data
  if(tabData) delete tabData;

  if(fullData){
    int N=fullData->GetLength();
    if(tabLength>=N){ // No compression needed...just draw everything
      double x[N];
      double y[N];
      for(int i=0;i<N;++i){
	x[i]=i;
	y[i]=(*fullData)[i].y;
      }
      tabData = new wxGraphCurve(x,y,N);
    }else{
      double x[tabLength];
      double y[tabLength];
      double d=(double)tabLength/N;
      for(int i=0;i<tabLength;++i){
	x[i]=i;
	y[i]=0;
      }
      for(int j=0;j<N;++j){
	int i=(int)(j*d);
	if(fabs((*fullData)[j].y)>fabs(y[i])) y[i]=(*fullData)[j].y;
      }
      tabData = new wxGraphCurve(x,y,tabLength);
    }
  }else
    tabData = NULL;
}
  
