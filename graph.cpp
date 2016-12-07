/////////////////////////////////////////////////////////////////////////////
// Name:        graph.cpp
// Purpose:     wxGraphWindow
// Author:      Predrag Miocinovic
// Modified by:
// Created:     03/30/2006
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Predrag Miocinovic
// Licence:     wxWindows license
// Note:        This is a hack of wxPlotWindow, adding and removing functionality
/////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#ifdef __GNUG__
#pragma implementation "graph.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/object.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dcbuffer.h"
#include "wx/stattext.h"
#include "wx/module.h"
#include "wx/tooltip.h"
#endif

#include "graph.h"

#include <math.h>

//----------------------------------------------------------------------------
// event types
//----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_GRAPH_DATA_CHANGED);

//-----------------------------------------------------------------------------
// wxRealRect
//-----------------------------------------------------------------------------
wxRealRect::wxRealRect(const wxRealPoint& point1, const wxRealPoint& point2){
    x = point1.x;
    y = point1.y;
    width = point2.x - point1.x;
    height = point1.y - point2.y;

    if (width < 0)
    {
        width = -width;
        x = point2.x;
    }

    if (height < 0)
    {
        height = -height;
        y = point2.y;
    }
}

bool wxRealRect::operator==(const wxRealRect& rect) const
{
    return ((x == rect.x) &&
            (y == rect.y) &&
            (width == rect.width) &&
            (height == rect.height));
}

wxRealRect wxRealRect::operator+(const wxRealRect& rect) const
{
    double x1 = wxMin(this->x, rect.x);
    double y1 = wxMin(this->y, rect.y);
    double y2 = wxMax(y+height, rect.height+rect.y);
    double x2 = wxMax(x+width, rect.width+rect.x);
    return wxRealRect(x1, y1, x2-x1, y2-y1);
}

//-----------------------------------------------------------------------------
// wxGraphEvent
//-----------------------------------------------------------------------------

wxGraphEvent::wxGraphEvent( wxEventType commandType, int id )
    : wxNotifyEvent( commandType, id )
{
    m_curve = (wxGraphCurve*) NULL;
    m_zoom = wxRealRect();
    m_position = 0;
}

//-----------------------------------------------------------------------------
// wxGraphCurve
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGraphCurve, wxObject)

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayOfPoints);

wxGraphCurve::wxGraphCurve( double *x,double *y,int N)
  :minX(1e30),maxX(-1e30),minY(1e30),maxY(-1e30),m_colour(wxColour(255,255,255)){

  m_pts.Alloc(N);
  for(int i=0;i<N;++i){
    m_pts.Add(wxRealPoint(x[i],y[i]));
    if(minX>x[i]) minX=x[i];
    if(maxX<x[i]) maxX=x[i];
    if(minY>y[i]) minY=y[i];
    if(maxY<y[i]) maxY=y[i];
  }
  // Add 10% to min and max Y for nicer range
  if(minY==maxY){
    minY-=1;
    maxY+=1;
  }else{
    minY-=(maxY-minY)*0.1;
    maxY+=(maxY-minY)*0.1;
  }
  // Add 2% to min and max X for nicer range
  if(minX==maxX){
    minX-=1;
    maxX+=1;
  }else{
    minX-=(maxX-minX)*0.02;
    maxX+=(maxX-minX)*0.02;
  }

}

//-----------------------------------------------------------------------------
// wxGraphArea
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGraphArea, wxWindow)

// Here we implement varous zooms.....
BEGIN_EVENT_TABLE(wxGraphArea, wxWindow)
    EVT_PAINT(        wxGraphArea::OnPaint)
    EVT_MOUSE_EVENTS(    wxGraphArea::OnMouse)
END_EVENT_TABLE()

wxGraphArea::wxGraphArea( wxGraphWindow *parent, const wxSize &size )
    : wxWindow( parent, wxID_ANY, wxDefaultPosition, size , wxSIMPLE_BORDER, _T("grapharea") )
{
    m_owner = parent;
    xZoomStart = -1;
    yZoomStart = -1;
    zoomTrack = -1;

    SetBackgroundColour( *wxBLACK );
}

// Zooming support here
void wxGraphArea::OnMouse( wxMouseEvent &event ){
    // Check which mouse event brought us here and dispach accordingly
    if (event.LeftDown() && yZoomStart==-1) StartXZoom(event);
    else if (event.LeftUp() && xZoomStart!=-1) EndXZoom(event);
    else if (event.RightDown() && xZoomStart==-1) StartYZoom(event);
    else if (event.RightUp() && yZoomStart!=-1) EndYZoom(event);
    else if (event.MiddleDown()){ // Maybe another combo for two button mice
      m_owner->ResetZoom(); 
      m_lookupPoint=wxPoint();
    }else if (event.Dragging() && (yZoomStart!=-1 ||  xZoomStart!=-1)) TrackZoom(event);
    else if (event.Moving() && event.ShiftDown()) GetCoords(event);
    else if (event.Moving()) m_lookupPoint=wxPoint();
    else{
      event.Skip();
      return;
    }

    m_owner->RedrawEverything();
}

void wxGraphArea::StartXZoom( wxMouseEvent &event ){
  xZoomStart = event.GetX();
}

void wxGraphArea::EndXZoom( wxMouseEvent &event ){
  wxCoord xZoomEnd = event.GetX();

  if(xZoomEnd!=xZoomStart){

    // Transform graph coordinates into curve scale
    wxRealRect zoomBox = m_owner->GetZoomBox();
    
    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    
    double lower = zoomBox.GetX();
    double rangeX = zoomBox.GetWidth()/(double)client_width;

    if(xZoomEnd>client_width) xZoomEnd=client_width;
    if(xZoomEnd<0) xZoomEnd=0;

    if(xZoomStart>xZoomEnd){
      wxCoord tmp=xZoomStart;
      xZoomStart=xZoomEnd;
      xZoomEnd=tmp;
    }
    
    double newLower = xZoomStart*rangeX+lower;
    double newUpper = xZoomEnd*rangeX+lower;
    
    zoomBox.SetX(newLower);
    zoomBox.SetWidth(newUpper-newLower);
    
    m_owner->SetZoomBox(zoomBox);
  }

  xZoomStart=zoomTrack=-1;

  return;
}


void wxGraphArea::StartYZoom( wxMouseEvent &event ){
  yZoomStart = event.GetY();
}

void wxGraphArea::EndYZoom( wxMouseEvent &event ){
  wxCoord yZoomEnd = event.GetY();

  if(yZoomEnd!=yZoomStart){
    // Transform graph coordinates into curve scale
    wxRealRect zoomBox = m_owner->GetZoomBox();

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);
    
    double upper = zoomBox.GetY();
    double rangeY = zoomBox.GetHeight()/(double)client_height;
    
    if(yZoomEnd>client_height) yZoomEnd=client_height;
    if(yZoomEnd<0) yZoomEnd=0;

    if(yZoomStart<yZoomEnd){
      wxCoord tmp=yZoomStart;
      yZoomStart=yZoomEnd;
      yZoomEnd=tmp;
    }
    
    double newLower = upper-yZoomStart*rangeY;
    double newUpper = upper-yZoomEnd*rangeY;

    zoomBox.SetY(newUpper);
    zoomBox.SetHeight(newUpper-newLower);
  
    m_owner->SetZoomBox(zoomBox);
  }
  
  yZoomStart=zoomTrack=-1;

  return;
}

void wxGraphArea::TrackZoom( wxMouseEvent &event ){
  if(xZoomStart>=0) zoomTrack=event.GetX();
  else if(yZoomStart>=0) zoomTrack=event.GetY();
}

void wxGraphArea::GetCoords( wxMouseEvent &event ){
  m_lookupPoint=event.GetPosition();
}

void wxGraphArea::DrawCurve( wxDC *dc, wxGraphCurve *curve){
  int client_width;
  int client_height;
  GetClientSize( &client_width, &client_height);
  
  wxRealRect zoom = m_owner->GetZoomBox();
  
  double double_client_width = (double)client_width;
  double double_client_height = (double)client_height;
  double rangeX = (zoom.GetWidth())/double_client_width;
  double rangeY = (zoom.GetHeight())/double_client_height;
  double startX = zoom.GetX();
  double endY = zoom.GetY();
  
  wxArrayOfPoints pts=curve->GetPoints();
  for (unsigned int i = 0; i < pts.GetCount()-1; ++i){
    wxCoord x1=(wxCoord)((pts.Item(i).x-startX)/rangeX);
    wxCoord y1=(wxCoord)((endY-pts.Item(i).y)/rangeY);
    wxCoord x2=(wxCoord)((pts.Item(i+1).x-startX)/rangeX);
    wxCoord y2=(wxCoord)((endY-pts.Item(i+1).y)/rangeY);
    
    dc->DrawLine( x1, y1, x2, y2 );
  }
  
  // Draw zoom lines if zoom under way
  if(xZoomStart!=-1){  // X zoom under way
    dc->DrawLine( xZoomStart, 0, xZoomStart, client_height);
    if(zoomTrack!=-1)
      dc->DrawLine( zoomTrack, 0, zoomTrack, client_height);
  }
  if(yZoomStart!=-1){ // Y zoom under way
    dc->DrawLine( 0, yZoomStart, client_width, yZoomStart );
    if(zoomTrack!=-1)
      dc->DrawLine( 0, zoomTrack, client_width, zoomTrack );
  }

  // Draw cross-hairs and readout value if lookup underway
  if(m_lookupPoint.x>0 && m_lookupPoint.y>0){
    dc->DrawLine( m_lookupPoint.x, 0, m_lookupPoint.x, client_height);
    dc->DrawLine( 0, m_lookupPoint.y, client_width, m_lookupPoint.y );

    double lower = zoom.GetX();
    double rangeX = zoom.GetWidth()/(double)client_width;
    double xCoord = m_lookupPoint.x*rangeX+lower;

    double upper = zoom.GetY();
    double rangeY = zoom.GetHeight()/(double)client_height;
    double yCoord = upper-m_lookupPoint.y*rangeY;

    wxString tmp;
    tmp.Printf(_T("(%.4g,%.4g)"),xCoord,yCoord);
    wxCoord w,h;
    dc->GetTextExtent(tmp,&w,&h);
    dc->DrawText( tmp,client_width-w-1 , 1 );
  }
}

void wxGraphArea::OnPaint( wxPaintEvent &WXUNUSED(event) ){
    wxPaintDC dc( this );

    wxGraphCurve *curve = (wxGraphCurve*)  m_owner->GetCurve();
    if(curve){
      dc.SetPen(wxPen(curve->GetColour()));
      dc.SetTextForeground(*wxWHITE);
      DrawCurve( &dc, curve);
    }
}

//-----------------------------------------------------------------------------
// wxGraphXAxisArea
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGraphXAxisArea, wxWindow)

BEGIN_EVENT_TABLE(wxGraphXAxisArea, wxWindow)
  EVT_PAINT(        wxGraphXAxisArea::OnPaint)
END_EVENT_TABLE()

wxGraphXAxisArea::wxGraphXAxisArea( wxGraphWindow *parent, const wxString &label)
        : wxWindow( parent, wxID_ANY, wxDefaultPosition, wxSize(wxDefaultCoord,20), 0, _T("graphxaxisarea") ){
    m_owner = parent;

    SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    SetBackgroundColour( *wxBLACK );
    if(label.Length()){
      SetToolTip(label);
      GetToolTip()->SetDelay(500); // CONFIG
    }
}

void wxGraphXAxisArea::OnPaint( wxPaintEvent &WXUNUSED(event) ){

    wxPaintDC dc( this );

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    dc.SetFont(GetFont());
    dc.SetPen( *wxWHITE_PEN );
    dc.SetTextForeground(*wxWHITE);
    dc.DrawLine( 0, 5, client_width, 5 );

    wxGraphCurve *curve = m_owner->GetCurve();

    if (!curve) return;

    wxRealRect zoomBox = m_owner->GetZoomBox();

    double lower = zoomBox.GetX();
    double range = zoomBox.GetWidth();
    double upper = lower+range;

    double ideal = (upper-lower)/7.;
    double log = log10(ideal);
    double power = floor(log);
    double fraction = log-power;
    double factor = 1.;
    double error = fraction;
    
#define N_MULT 2
    double multiples[N_MULT]={2.,5.};
    for(int i=0;i<N_MULT;++i){
      double f=multiples[i];
      double lf=log10(f);
      double e=fabs(fraction-lf);
      if (e < error){
	error = e;
	factor = f;
      }
    }
#undef N_MULT
    double grid = factor * pow(10.,power);
    
    wxString format;
    if (power > 3 || power < -3){
      format =  _T("%+7.1e");
    }else if (power >= 0){
      int digits = wxMax(1, int(power));
      format.Printf(_T("%%%d.0f"),digits);
    }else{
      int digits = -int(power);
      format.Printf(_T("%%%d.%df"),digits+2,digits);
    }
    
    double current = -grid*floor(-lower/grid);
    while (current <= upper){
        int x = (int)ceil((current-lower) / range * (double)client_width) - 1;
        if ((x > 4) && (x < client_width-4)){
            dc.DrawLine( x, 0, x, 5 );
            wxString label;
	    label.Printf(format.c_str(), current );
	    wxCoord w,h;
	    dc.GetTextExtent(label,&w,&h);
            dc.DrawText( label, x-w/2, client_height-h-1 );
        }

        current += grid;
    }

}

//-----------------------------------------------------------------------------
// wxGraphYAxisArea
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGraphYAxisArea, wxWindow)

BEGIN_EVENT_TABLE(wxGraphYAxisArea, wxWindow)
  EVT_PAINT(        wxGraphYAxisArea::OnPaint)
END_EVENT_TABLE()

wxGraphYAxisArea::wxGraphYAxisArea( wxGraphWindow *parent,const wxString &label)
  : wxWindow( parent, wxID_ANY, wxDefaultPosition, wxSize(Y_LABEL_SP,wxDefaultCoord), 0, _T("graphyaxisarea") ){
    m_owner = parent;

    SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    SetBackgroundColour( *wxBLACK );
    if(label.Length()){
      SetToolTip(label);
      GetToolTip()->SetDelay(500); // CONFIG
    }
}

void wxGraphYAxisArea::OnPaint( wxPaintEvent &WXUNUSED(event) ){
    wxPaintDC dc( this );

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    dc.SetFont(GetFont());
    dc.SetPen( *wxWHITE_PEN );
    dc.SetTextForeground(*wxWHITE);
    dc.DrawLine( client_width-5, 0, client_width-5, client_height );

    wxGraphCurve *curve = m_owner->GetCurve();

    if (!curve) return;

    wxRealRect zoomBox = m_owner->GetZoomBox();

    double upper = zoomBox.GetY();
    double range = zoomBox.GetHeight();
    double lower = upper-range;

    double ideal = (upper-lower)/7.;
    double log = log10(ideal);
    double power = floor(log);
    double fraction = log-power;
    double factor = 1.;
    double error = fraction;
    
#define N_MULT 2
    double multiples[N_MULT]={2.,5.};
    for(int i=0;i<N_MULT;++i){
      double f=multiples[i];
      double lf=log10(f);
      double e=fabs(fraction-lf);
      if (e < error){
	error = e;
	factor = f;
      }
    }
#undef N_MULT
    double grid = factor * pow(10.,power);
    
    wxString format;
    if (power > 3 || power < -3){
      format =  _T("%6.1e");
    }else if (power >= 0){
      int digits = wxMax(1, int(power));
      format.Printf(_T("%%%d.0f"),digits);
    }else{
      int digits = -int(power);
      format.Printf(_T("%%%d.%df"),digits+2,digits);
    }

    double current = -grid*floor(-lower/grid);
    while (current <= upper){
        int y = (int)((upper-current) / range * (double)client_height) - 1;

        if ((y > 4) && (y < client_height-7)){
            dc.DrawLine( client_width-5, y, client_width, y );
            wxString label;
	    label.Printf( format.c_str(), current );
	    wxCoord w,h;
	    dc.GetTextExtent(label,&w,&h);
	    dc.DrawText( label,client_width-6-w , y-h/2 );
        }

        current += grid;
    }

}

//-----------------------------------------------------------------------------
// wxGraphAxisGap
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGraphAxisGap, wxWindow)

BEGIN_EVENT_TABLE(wxGraphAxisGap, wxWindow)
  EVT_PAINT(        wxGraphAxisGap::OnPaint)
END_EVENT_TABLE()

wxGraphAxisGap::wxGraphAxisGap( wxGraphWindow *parent )
  : wxWindow( parent, wxID_ANY, wxDefaultPosition, wxSize(Y_LABEL_SP,20), 0, _T("graphaxisgap") )
{
    m_owner = parent;

    SetBackgroundColour( *wxBLACK );
}

void wxGraphAxisGap::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );

    int client_width;
    int client_height;
    GetClientSize( &client_width, &client_height);

    dc.SetPen( *wxWHITE_PEN );
    dc.SetTextForeground(*wxWHITE);

    dc.DrawLine( client_width-5, 5, client_width, 5);
    dc.DrawLine( client_width-5, 0, client_width-5, 5 );
}

//-----------------------------------------------------------------------------
// wxGraphWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGraphWindow, wxPanel)

BEGIN_EVENT_TABLE(wxGraphWindow, wxPanel)
  EVT_SIZE( wxGraphWindow::OnSize )
END_EVENT_TABLE()

wxGraphWindow::wxGraphWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, const wxString &xLabel, const wxString &yLabel, int flag )
  : wxPanel( parent, id, pos, size, flag, _T("graphcanvas") ){

    m_boundingBox = wxRealRect();
    m_zoomBox = wxRealRect();

    m_area = new wxGraphArea( this , (wxSize)size-wxSize(Y_LABEL_SP,20) );
    wxBoxSizer *graphsizer = new wxBoxSizer( wxHORIZONTAL );
    if ((GetWindowStyleFlag() & wxGRAPH_Y_AXIS) != 0){
      m_yaxis = new wxGraphYAxisArea( this, yLabel );

      wxBoxSizer *vert1 = new wxBoxSizer( wxVERTICAL );
      vert1->Add( m_yaxis, 1);
      if ((GetWindowStyleFlag() & wxGRAPH_X_AXIS) != 0)
	vert1->Add( new wxGraphAxisGap( this), 0 );
      graphsizer->Add( vert1, 0, wxEXPAND);
    }else{
        m_yaxis = (wxGraphYAxisArea*) NULL;
    }

    if ((GetWindowStyleFlag() & wxGRAPH_X_AXIS) != 0)
    {
      m_xaxis = new wxGraphXAxisArea( this, xLabel );

      wxBoxSizer *vert2 = new wxBoxSizer( wxVERTICAL );
      graphsizer->Add( vert2, 5, wxEXPAND);
      vert2->Add( m_area, 1, wxEXPAND );
      vert2->Add( m_xaxis, 0, wxEXPAND );
    }else{
        graphsizer->Add( m_area, 1, wxEXPAND );
        m_xaxis = (wxGraphXAxisArea*) NULL;
    }

    SetSizer( graphsizer );
    graphsizer->SetSizeHints(this);

    SetBackgroundColour( *wxWHITE );

    m_curve = (wxGraphCurve*) NULL;
}

wxGraphWindow::~wxGraphWindow(){}

void wxGraphWindow::SetCurve( wxGraphCurve* curve ){
    m_curve = curve;
    if(m_curve){
      m_boundingBox = wxRealRect(wxRealPoint(m_curve->GetMinX(),m_curve->GetMaxY()),
				 wxRealPoint(m_curve->GetMaxX(),m_curve->GetMinY()));
      m_zoomBox = m_boundingBox;
    }else{
      m_boundingBox = wxRealRect();
      m_zoomBox = wxRealRect();
    }
    
    RedrawEverything();

    wxGraphEvent event( wxEVT_GRAPH_DATA_CHANGED, GetId() );
    event.SetEventObject( this );
    event.SetZoom( GetZoomBox() );
    event.SetCurve( m_curve );
    GetEventHandler()->ProcessEvent( event );
}

void wxGraphWindow::SetZoomBox( wxRealRect zoomBox ){
    m_zoomBox = zoomBox;

    RedrawEverything();
}

void wxGraphWindow::RedrawXAxis(){
    if (m_xaxis)
        m_xaxis->Refresh( true );
}

void wxGraphWindow::RedrawYAxis(){
    if (m_yaxis)
       m_yaxis->Refresh( true );
}

void wxGraphWindow::RedrawEverything(){
    if (m_xaxis)
        m_xaxis->Refresh( true );
    if (m_yaxis)
        m_yaxis->Refresh( true );
    m_area->Refresh( true );

}

void wxGraphWindow::OnSize( wxSizeEvent& event ){
  RedrawEverything();
  event.Skip();
}
