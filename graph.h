/////////////////////////////////////////////////////////////////////////////
// Name:        graph.h
// Purpose:     wxGraphWindow
// Author:      Predrag Miocinovic
// Modified by:
// Created:     03/30/2006
// Copyright:   (c) Robert Roebling, Predrag Miocinovic
// RCS-ID:      $Id$
// Licence:     wxWindows licence
// Note:        This is a hack of wxPlotWindow, adding and removing functionality
/////////////////////////////////////////////////////////////////////////////

/* This file contains definitions for classes that combine into single 
 * graph display, i.e. axes, plot area, curve, and all mouse controls */

#ifndef _WX_GRAPH_H_
#define _WX_GRAPH_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "graph.h"
#endif

#include "wx/panel.h"
#include "wx/defs.h"
#include "wx/event.h"
#include "wx/dynarray.h"

#ifdef WXMAKINGDLL_GRAPH
    #define WXDLLIMPEXP_GRAPH WXEXPORT
    #define WXDLLIMPEXP_DATA_GRAPH(type) WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_GRAPH WXIMPORT
    #define WXDLLIMPEXP_DATA_GRAPH(type) WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_GRAPH
    #define WXDLLIMPEXP_DATA_GRAPH(type) type
#endif

#define Y_LABEL_SP 60 // this should be changed if font size changed
//#define Y_LABEL_SP 48 // this should be changed if font size changed
//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_GRAPH wxGraphEvent;
class WXDLLIMPEXP_GRAPH wxGraphCurve;
class WXDLLIMPEXP_GRAPH wxGraphArea;
class WXDLLIMPEXP_GRAPH wxGraphXAxisArea;
class WXDLLIMPEXP_GRAPH wxGraphYAxisArea;
class WXDLLIMPEXP_GRAPH wxGraphWindow;

//-----------------------------------------------------------------------------
// consts
//-----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_GRAPH, wxEVT_GRAPH_DATA_CHANGED, 9921)
END_DECLARE_EVENT_TYPES()

#define wxGRAPH_X_AXIS          0x0004
#define wxGRAPH_Y_AXIS          0x0008
#define wxGRAPH_DEFAULT         (wxGRAPH_X_AXIS|wxGRAPH_Y_AXIS)


//-----------------------------------------------------------------------------
// wxRealRect
//-----------------------------------------------------------------------------
class WXDLLIMPEXP_GRAPH wxRealRect{
public:
    wxRealRect()
        : x(0), y(0), width(0), height(0)
        { }
    wxRealRect(double xx, double yy, double ww, double hh)
        : x(xx), y(yy), width(ww), height(hh)
        { }
    wxRealRect(const wxRealPoint& topLeft, const wxRealPoint& bottomRight);

    double GetX() const { return x; }
    void SetX(double xx) { x = xx; }

    double GetY() const { return y; }
    void SetY(double yy) { y = yy; }

    double GetWidth() const { return width; }
    void SetWidth(double w) { width = w; }

    double GetHeight() const { return height; }
    void SetHeight(double h) { height = h; }

    wxRealPoint GetPosition() const { return wxRealPoint(x, y); }
    void SetPosition( const wxRealPoint &p ) { x = p.x; y = p.y; }

    bool IsEmpty() const { return (width <= 0) || (height <= 0); }

    wxRealPoint GetTopLeft() const { return GetPosition(); }
    wxRealPoint GetLeftTop() const { return GetTopLeft(); }
    void SetTopLeft(const wxRealPoint &p) { SetPosition(p); }
    void SetLeftTop(const wxRealPoint &p) { SetTopLeft(p); }

    wxRealPoint GetBottomRight() const { return wxRealPoint(GetRight(), GetBottom()); }
    wxRealPoint GetRightBottom() const { return GetBottomRight(); }
    void SetBottomRight(const wxRealPoint &p) { SetRight(p.x); SetBottom(p.y); }
    void SetRightBottom(const wxRealPoint &p) { SetBottomRight(p); }

    double GetLeft()   const { return x; }
    double GetTop()    const { return y; }
    double GetBottom() const { return y + height - 1; }
    double GetRight()  const { return x + width - 1; }

    void SetLeft(double left) { x = left; }
    void SetRight(double right) { width = right - x + 1; }
    void SetTop(double top) { y = top; }
    void SetBottom(double bottom) { height = bottom - y + 1; }

    // compare rectangles
    bool operator==(const wxRealRect& rect) const;
    bool operator!=(const wxRealRect& rect) const { return !(*this == rect); }

    // Add rectangles
    wxRealRect operator+(const wxRealRect& rect) const;
    wxRealRect& operator+=(const wxRealRect& rect)
    {
        *this = *this + rect;
        return *this;
    }


public:
    double x, y, width, height;
};


//-----------------------------------------------------------------------------
// wxGraphEvent
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_GRAPH wxGraphEvent: public wxNotifyEvent{
public:
    wxGraphEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );

    wxGraphCurve *GetCurve()
        { return m_curve; }
    void SetCurve( wxGraphCurve *curve )
        { m_curve = curve; }

    wxRealRect GetZoomBox()
        { return m_zoom; }
    void SetZoom( wxRealRect zoom )
        { m_zoom = zoom; }

    wxInt32 GetPosition()
        { return m_position; }
    void SetPosition( wxInt32 pos )
        { m_position = pos; }

private:
    wxGraphCurve   *m_curve;
    wxRealRect          m_zoom;
    wxInt32         m_position;
};

//-----------------------------------------------------------------------------
// wxGraphCurve
//-----------------------------------------------------------------------------
WX_DECLARE_OBJARRAY(wxRealPoint, wxArrayOfPoints);

class WXDLLIMPEXP_GRAPH wxGraphCurve: public wxObject{
 public:
  wxGraphCurve(double *x,double *y,int N);

  void SetColour(const wxColour &colour){m_colour=colour;}
  wxColour GetColour(){return m_colour;}

  inline double GetMinX() const{return minX;}
  inline double GetMaxX() const{return maxX;}
  inline double GetMinY() const{return minY;}
  inline double GetMaxY() const{return maxY;}

  inline wxArrayOfPoints GetPoints() {return m_pts;}
  inline size_t GetLength(){return m_pts.GetCount();}
  inline wxRealPoint operator[](int i) {return m_pts.Item(i);}

 private:
  wxArrayOfPoints m_pts;
  double minX;
  double maxX;
  double minY;
  double maxY;
   
  wxColour m_colour;

  DECLARE_ABSTRACT_CLASS(wxGraphCurve)
};


//-----------------------------------------------------------------------------
// wxGraphArea
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_GRAPH wxGraphArea: public wxWindow{
public:
    wxGraphArea() {}
    wxGraphArea( wxGraphWindow *parent , const wxSize &size=wxDefaultSize );

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    
    void DrawCurve( wxDC *dc, wxGraphCurve *curve);
    
private:
    wxGraphWindow     *m_owner;

    void StartXZoom( wxMouseEvent &event );
    void EndXZoom( wxMouseEvent &event );
    void StartYZoom( wxMouseEvent &event );
    void EndYZoom( wxMouseEvent &event );
    void TrackZoom( wxMouseEvent &event );  
    void GetCoords( wxMouseEvent &event );
    void ResetZoom();

    wxCoord xZoomStart;
    wxCoord yZoomStart;
    wxCoord zoomTrack;
    wxPoint m_lookupPoint;

    DECLARE_CLASS(wxGraphArea)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxGraphXAxisArea
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_GRAPH wxGraphXAxisArea: public wxWindow
{
public:
    wxGraphXAxisArea() {}
    wxGraphXAxisArea( wxGraphWindow *parent , const wxString &label=_T(""));

    void OnPaint( wxPaintEvent &event );

private:
    wxGraphWindow   *m_owner;

    DECLARE_CLASS(wxGraphXAxisArea)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxGraphYAxisArea
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_GRAPH wxGraphYAxisArea: public wxWindow
{
public:
    wxGraphYAxisArea() {}
    wxGraphYAxisArea( wxGraphWindow *parent, const wxString &label=_T("") );

    void OnPaint( wxPaintEvent &event );

private:
    wxGraphWindow   *m_owner;

    DECLARE_CLASS(wxGraphYAxisArea)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxGraphAxisGap
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_GRAPH wxGraphAxisGap: public wxWindow{
public:
    wxGraphAxisGap() {}
    wxGraphAxisGap( wxGraphWindow *parent );

    void OnPaint( wxPaintEvent &event );

private:
    wxGraphWindow   *m_owner;

    DECLARE_CLASS(wxGraphAxisGap)
    DECLARE_EVENT_TABLE()
};


//-----------------------------------------------------------------------------
// wxGraphWindow
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_GRAPH wxGraphWindow: public wxPanel{
public:
    wxGraphWindow() {}
    wxGraphWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,  const wxString &xLabel=_T(""), const wxString &yLabel=_T(""), int flags = wxGRAPH_DEFAULT );
    ~wxGraphWindow();

    // curve accessors
    // ---------------

    void SetCurve( wxGraphCurve *curve );
    wxGraphCurve *GetCurve() {return m_curve;}

    // Zoom representation
    // -------------------------

    void SetZoomBox( wxRealRect zoomBox );
    wxRealRect GetZoomBox(){ return m_zoomBox; }
    void ResetZoom(){m_zoomBox = m_boundingBox;}

    // events (may be overridden)
    // --------------------------

    void OnSize( wxSizeEvent& event );

    // utilities
    // ---------

    void RedrawEverything();
    void RedrawXAxis();
    void RedrawYAxis();

private:
    friend class wxGraphArea;
    friend class wxGraphXAxisArea;
    friend class wxGraphYAxisArea;
    friend class wxGraphAxisGap;

    wxRealRect          m_boundingBox;
    wxRealRect          m_zoomBox;    

    wxGraphArea        *m_area;
    wxGraphXAxisArea   *m_xaxis;
    wxGraphYAxisArea   *m_yaxis;
    wxGraphCurve       *m_curve;

    DECLARE_CLASS(wxGraphWindow)
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// graph event macros
// ----------------------------------------------------------------------------

typedef void (wxEvtHandler::*wxGraphEventFunction)(wxGraphEvent&);

#if WXWIN_COMPATIBILITY_EVENT_TYPES

#define EVT_GRAPH_DATA_CHANGED(id, fn) { wxEVT_GRAPH_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxGraphEventFunction) & fn, (wxObject *) NULL },

#else

#define EVT_GRAPH_DATA_CHANGED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_GRAPH_SEL_CHANGED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxGraphEventFunction, & fn ), (wxObject *) NULL ),

#endif

#endif
   // _WX_GRAPH_H_
