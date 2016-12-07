/*
 * rfwindow.h
 *
 * Defines full RF display window
 */

#ifndef _RFWINDOW_H_
#define _RFWINDOW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "rfwindow.cpp"
#endif
#include <wx/log.h>
#include <wx/dialog.h>
#include "graph.h"

#define VOLTAGE_ON_CHECKBOX 10001
#define FFT_ON_CHECKBOX 10002
#define POWER_ON_CHECKBOX 10003
#define ALFA_ON_CHECKBOX 10004
#define ALFAFFT_ON_CHECKBOX 10005

#define NO_DBM_REF -999
/*
 * wxRFWindow class declaration
 */
class wxRFWindow: public wxDialog{    
  
    DECLARE_CLASS( wxRFWindow )

public:
    // Constructors
  wxRFWindow( wxWindow* parent, wxWindowID id, const wxString &caption, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, bool alfa = false, long style = wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU|wxFULL_REPAINT_ON_RESIZE);

  // Destructor 
  ~wxRFWindow();

  bool Show(bool show=true);

  // Event handlers
  void InsertVoltage(wxCommandEvent& event);
  void InsertFFT(wxCommandEvent& event);
  void InsertPower(wxCommandEvent& event);
  void InsertALFA(wxCommandEvent& event);
  void InsertALFAFFT(wxCommandEvent& event);
  void NewZoom(wxCommandEvent& event);
  //  void OnKey(wxKeyEvent &event);

  // Sets data content
  void SetData(wxGraphCurve *data);

  // Create data average, with corresponding FFT, the resulting wfm average
  // is stored in return pointer provided
  void SetAverage(wxGraphCurve **data,int n,wxGraphCurve **avg);

  // Set reference dBm level
  void SetDBMRef(double val){dBmRef=val;}

  // Clear mean power 
  void ClearDBMRef(){powerRef=0;powerN=0;}

  // Get data pointer
  wxGraphCurve *GetVoltage() const {return m_voltageData;}
  wxGraphCurve *GetFFT() const {return m_fftData;}
  wxGraphCurve *GetPower() const {return m_powerData;}
  wxGraphCurve *GetALFA() const {return m_alfaData;}
  wxGraphCurve *GetALFAFFT() const {return m_alfafftData;}

 protected:
  DECLARE_EVENT_TABLE()
  void processData();    
  void GetRMS();
  void GetdBm(double mean);
 private:
  wxGraphCurve *m_voltageData;
  wxGraphCurve *m_fftData;
  wxGraphCurve *m_powerData;
  wxGraphCurve *m_alfaData;
  wxGraphCurve *m_alfafftData;

  wxGraphWindow *m_voltageGraph; 
  wxGraphWindow *m_fftGraph;
  wxGraphWindow *m_powerGraph;
  wxGraphWindow *m_alfaGraph;
  wxGraphWindow *m_alfafftGraph;

  wxStaticText *m_rmsValue;
  wxStaticText *m_dBmValue;

  bool fftOn;
  bool powerOn;
  bool voltageOn;
  bool alfaOn;
  bool alfafftOn;

  double dBmRef;
  double powerRef;
  int powerN;
};

#endif // _RFWINDOW_H_
