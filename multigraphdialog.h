/*
 * multigraphdialog.h
 */

#ifndef _MULTIGRAPHDIALOGH_
#define _MULTIGRAPHDIALOGH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "multigraphdialog.cpp"
#endif

#include <wx/log.h>
#include <wx/dialog.h>
#include "BaseDBHandler.h"
#include "graph.h"
#include "utcdatectrl.h"

#define REQUEST_BUTTON_ID 10
#define NOW_BUTTON_ID     11

#define TIME_DISPLAY     1001
#define LENGTH_DISPLAY   1002

/*
 * wxMultiGraphDialog class declaration
 */
BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxNO_RESULTS,-1)
END_DECLARE_EVENT_TYPES()

class wxMultiGraphDialog: public wxDialog, public BaseDBHandler{    
  
  DECLARE_CLASS( wxMultiGraphDialog )

public:
  // Constructors
  wxMultiGraphDialog( wxWindow* parent, wxWindowID id, const wxString &caption, 
		    int dim=1,
		    const wxString *xUnits=NULL,const wxString *yUnits=NULL,
		      const wxString *dbVar=NULL,const wxString &dbTable=wxT(""),
		    const wxPoint& pos = wxDefaultPosition, 
		    const wxSize& size = wxDefaultSize, 
		    long style = wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|
		    wxMINIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU|
		    wxFULL_REPAINT_ON_RESIZE|wxGRAPH_DEFAULT);

  // Destructor 
  ~wxMultiGraphDialog();

  // Event handlers
  void InsertGraph(wxCommandEvent& event);
  void OnRequest(wxCommandEvent &event);
  void OnNow(wxCommandEvent &event);

  // Sets data content
  void SetData(wxGraphCurve *data,int n=0){
    if(n<m_dim && n>=0){
      m_data[n] = data;
      m_graphWindow[n]->SetCurve(m_data[n]);
    }
  }

  // Get data pointer
  wxGraphCurve *GetData(int n=0) const {return (n>=0&&n<m_dim)?m_data[n]:NULL;}

 protected:
  DECLARE_EVENT_TABLE()
  void sqlParser(PGresult *result);  

 private:
  int m_dim;
  bool *windowOn;
  wxGraphWindow **m_graphWindow; 
  wxGraphCurve **m_data;

  wxUTCDateCtrl *m_refTimeEntry;
  wxTextCtrl *m_lengthEntry;
  long m_refTick;
  wxString *m_sqlVariable;
  wxString m_sqlTable;
  wxString m_caption;
  wxGraphCurve **m_sqlDataCurve;
  double **m_histBound;
  int *m_histLength;
 
};

#endif // _MULTIGRAPHDIALOGH_
