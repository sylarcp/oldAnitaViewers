/*
 * rangedgauge.h
 */

#ifndef _RANGEDGAUGEH_
#define _RANGEDGAUGEH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "rangedgauge.cpp"
#endif

#include <wx/window.h>
#include "range.h"
#include "historydisplay.h"

class wxRangedGauge: public wxWindow, public wxHistoryDisplay{
 public:
  wxRangedGauge(){}
  wxRangedGauge(wxWindow *parent,
		wxWindowID id,
		double low,
		double high,
		const wxRangeSpec *rangeSpec,
		ScaleType type=Linear,
		unsigned int avg=1,
		const wxPoint &pos=wxDefaultPosition,
		const wxSize &size=wxDefaultSize,
		long style = wxVERTICAL,
		const wxString &name=_T("rangedgauge"),
		const wxString &xUnits=_T(""),
		const wxString &yUnits=_T(""),
		const wxString &dbVar=_T(""),
		const wxString &dbTable=_T(""),
		unsigned int buffer=wxDEFAULT_HISTORY_BUFFER);
  
  ~wxRangedGauge();

  void SetGauge(double val);
  void SetRange(double low, double high);
  void SetRangeSpec(const wxRangeSpec *rangeSpec);
  void SetScaleType(ScaleType type);
  void SetAverage(unsigned int avg);

  const wxRangeSpec *GetRangeSpec(){return m_rangeSpec;}
  void GetRange(double *low,double *high);

  /* Event handlers */
  void OnPaint( wxPaintEvent &event );
  void OnSize( wxSizeEvent& event );
  void OpenGraph( wxMouseEvent& event );

 protected:
  DECLARE_EVENT_TABLE()
  double LocalValue();
  void DrawBar(double val);    
  void _SetGauge(double val);
  void _SetTip(double val);

 private:
  double m_low;
  double m_high;
  const wxRangeSpec *m_rangeSpec;
  ScaleType m_scale;
  unsigned int m_avg;
  static wxColour m_OutOfRange_Colour;

  DECLARE_DYNAMIC_CLASS( wxRangedGauge )
};
  
#endif // _RANGEDGAUGEH_
