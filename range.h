/*
 * range.h
 */

#ifndef _RANGEH_
#define _RANGEH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "range.cpp"
#endif

#include <wx/dialog.h>
#include <wx/colour.h>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/button.h>

#define ADD_BUTTON_ID      0x01
#define REMOVE_BUTTON_ID   0x02
#define EDIT_BUTTON_ID     0x03 
#define COLOUR_BUTTON_ID   0x04

#define colourBoxSize  16

struct wxRangeSpec{							
  wxRangeSpec():low(0),high(0),colour(wxColour()){}
  wxRangeSpec(double l,double h, wxColour c):low(l),high(h),colour(c){}
  wxRangeSpec(double l,double h, wxColour *c):low(l),high(h),colour(*c){}
  double low;
  double high;
  wxColour colour;
};

#define N_SCALETYPES 2
enum ScaleType {Linear, Log};

class wxRangeDialog: public wxDialog{
 public:
  wxRangeDialog();
  wxRangeDialog(wxWindow *parent,
		wxWindowID id,
		const wxString &caption,
		int max,
		wxRangeSpec *rangeSpec=NULL,
		const wxPoint &pos=wxDefaultPosition,
		const wxSize &size=wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE,
		const wxString &name=_T("rangedialog"));
  
  ~wxRangeDialog();

  void AddExtraValControl(wxWindow *valControl,const wxString &label);

  void SetRangeSpec(wxRangeSpec *rangeSpec);
  const wxRangeSpec *GetRangeSpec() {return m_rangeSpec;}

  /* Event handlers */
  void OnAdd( wxCommandEvent &event );
  void OnRemove( wxCommandEvent &event );
  void OnEdit( wxCommandEvent &event );

 protected:
  DECLARE_EVENT_TABLE()
  void initList();    

 private:
  wxRangeSpec *m_rangeSpec;
  int maxSpec;
  wxListCtrl *m_specList;
  wxImageList *m_colourBitmaps;

  DECLARE_DYNAMIC_CLASS( wxRangeDialog )
};

class wxRangeEditDialog: public wxDialog{
 public:
  wxRangeEditDialog();
  wxRangeEditDialog(wxWindow *parent, 
		    wxWindowID id,
		    wxRangeSpec *rangeSpec=NULL,
		    const wxPoint &pos=wxDefaultPosition,
		    const wxSize &size=wxDefaultSize,
		    long style = wxDEFAULT_DIALOG_STYLE,
		    const wxString &name=_T("rangeeditdialog"));
  ~wxRangeEditDialog();

  void SetRangeSpec(wxRangeSpec *rangeSpec);
  void SetLow(double low);
  void SetHigh(double high);
  void SetColour(wxColour col);
  
  /* Event handlers */
  void OnColourEdit( wxCommandEvent &event );
  void OnOK( wxCommandEvent &event );

  const wxRangeSpec *GetRangeSpec() {return &m_rangeSpec;}
 protected:
  DECLARE_EVENT_TABLE()

 private:
  wxRangeSpec m_rangeSpec;
  wxTextCtrl *m_lowText;
  wxTextCtrl *m_highText;
  wxButton *m_colourButton;

  DECLARE_DYNAMIC_CLASS( wxRangeEditDialog )
};

  
#endif // _RANGEH_
