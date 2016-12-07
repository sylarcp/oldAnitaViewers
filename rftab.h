/*
 * rftab.h
 *
 * Defines small dispay area for summary of RF data
 */

#ifndef _RFTAB_H_
#define _RFTAB_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "rftab.cpp"
#endif

using namespace std;
#include <wx/window.h>
#include "rfwindow.h"

//WX_DECLARE_OBJARRAY(wxRealPoint, wxArrayOfPoints);
/*
 * wxRFTab class declaration
 */
class wxRFTab: public wxWindow{    
  
    DECLARE_CLASS( wxRFTab )

public:
    // Constructors
  wxRFTab( wxWindow* parent, const wxString &label, wxRFWindow *full = NULL, wxWindowID id = wxID_ANY , const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(20,20), long style = wxNO_BORDER|wxTAB_TRAVERSAL|wxCLIP_CHILDREN );

  // Destructor 
  ~wxRFTab();

  // Event handlers
  void OnSize( wxSizeEvent& event );
  void OnPaint( wxPaintEvent& event );
  void OnMouseClick( wxMouseEvent& event );

  // Sets display scale
  void SetScale(int min,int max);

  // Sets data content
  void SetData(wxGraphCurve *data);
  void SetBGColour(const wxColour &colour){m_bgcolour=colour;Refresh();}
  inline wxColour GetBGColour(){return m_bgcolour;}
  inline void SetColour(const wxColour &colour){m_colour=colour;Refresh();}
  inline wxColour GetColour(){return m_colour;}

  // Get data pointer
  wxGraphCurve *GetData() const {return fullData;}

 protected:
  DECLARE_EVENT_TABLE()
  void DrawCurve();  
  void MakeTabData();
 private:
  int tabLength;
  int m_scaleMin;
  int m_scaleMax;
  wxColour m_colour;
  wxColour m_bgcolour;

  wxGraphCurve *fullData;
  wxGraphCurve *tabData;

  wxRFWindow *fullDisplay;

};

#endif // _RFTAB_H_
