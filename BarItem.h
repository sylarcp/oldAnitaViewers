/*
 * BarItem.h
 */ 

#ifndef _BARITEM_H_
#define _BARITEM_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BarItem.h"
#endif

#include <wx/panel.h>
#include <wx/stattext.h>
#include "rangedgauge.h"

class BarItem: public wxPanel{
  DECLARE_CLASS(BarItem)
 public:
  BarItem();
  BarItem(wxWindow* parent,wxWindowID id,const wxString label, 
	  const wxString caption,const wxRangeSpec *rangeSpec, double *range, 
	  const ScaleType type=Linear,
	  const unsigned int avg=1,
	  const wxSize &size=wxDefaultSize,
	  const wxString &xUnits=wxT(""),const wxString &yUnits=wxT(""),
	  const wxString &dbVar=wxT(""), const wxString &dbTable=wxT(""),
	  const wxFont *font=wxSMALL_FONT);

  ~BarItem();

  // Various functions to set data fields
  void SetDisplayValue(double val);
  void SetDisplayRange(double low, double high);
  void SetDisplayRangeSpec(const wxRangeSpec *rangeSpec);
  void SetLabel(const wxString &label);
  void SetScaleType(ScaleType type);
  void SetAverage(unsigned int avg);
  void SetBackground(const wxColour &colour);

 private:
  wxRangedGauge *m_gauge;
  wxStaticText *m_label;
};

#endif // _BARITEM_H_
