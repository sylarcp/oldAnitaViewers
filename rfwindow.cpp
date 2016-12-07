/*
 * rfwindow.cpp
 *
 * Definitions for class that displays RF infomration in separate window. 
 * Also, all calculations of FFT, power envelope, and average RF signals is
 * performed here. 
 */

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "rfwindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/checkbox.h>
#include <wx/valgen.h>
#include <fftw3.h>
#include "rfwindow.h"
#include "options.h"


IMPLEMENT_CLASS( wxRFWindow, wxDialog )

BEGIN_EVENT_TABLE( wxRFWindow, wxDialog )

  EVT_CHECKBOX(VOLTAGE_ON_CHECKBOX, wxRFWindow::InsertVoltage)
  EVT_CHECKBOX(FFT_ON_CHECKBOX, wxRFWindow::InsertFFT)
  EVT_CHECKBOX(POWER_ON_CHECKBOX, wxRFWindow::InsertPower)
  EVT_CHECKBOX(ALFA_ON_CHECKBOX, wxRFWindow::InsertALFA)
  EVT_CHECKBOX(ALFAFFT_ON_CHECKBOX, wxRFWindow::InsertALFAFFT)
//  EVT_KEY_DOWN(wxRFWindow::OnKey)
//  EVT_CHAR(wxRFWindow::OnKey)

END_EVENT_TABLE()

/*
 * Constructor
 */
  wxRFWindow::wxRFWindow( wxWindow* parent, wxWindowID id, const wxString &caption, const wxPoint& pos, const wxSize& size, bool alfa, long style ):wxDialog(parent,id,caption,pos,size,style),m_voltageData(NULL),m_fftData(NULL),m_powerData(NULL),m_alfaData(NULL),m_alfafftData(NULL),fftOn(false),powerOn(false),voltageOn(false),alfaOn(false), alfafftOn(false),dBmRef(NO_DBM_REF),powerRef(0),powerN(0){
    
  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);

  m_voltageGraph=new wxGraphWindow(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(),size.GetHeight()/3), _T("t [ns]"), _T("V [mV]"), wxGRAPH_X_AXIS | wxGRAPH_Y_AXIS);
  m_fftGraph=new wxGraphWindow(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(),size.GetHeight()/3), _T("f [GHz]"), _T("P [dBm/MHz]"),  wxGRAPH_X_AXIS | wxGRAPH_Y_AXIS);
  m_powerGraph=new wxGraphWindow(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(),size.GetHeight()/3), _T("t [ns]"), _T("P [sigma]"),  wxGRAPH_X_AXIS | wxGRAPH_Y_AXIS);
  m_alfaGraph=new wxGraphWindow(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(),size.GetHeight()/3), _T("t [ns]"), _T("alfa_V [mV]"), wxGRAPH_X_AXIS | wxGRAPH_Y_AXIS);
  m_alfafftGraph=new wxGraphWindow(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(),size.GetHeight()/3), _T("alfa_f [GHz]"), _T("P [dBm/MHz]"),  wxGRAPH_X_AXIS | wxGRAPH_Y_AXIS);

  wxBoxSizer *controlBox = new wxBoxSizer(wxHORIZONTAL);
  controlBox->Add(new wxCheckBox(this,VOLTAGE_ON_CHECKBOX,_T("Voltage"),wxDefaultPosition, wxDefaultSize,
				 wxCHK_2STATE,wxGenericValidator(&voltageOn),_T("VoltageCheckBox")),0);
  controlBox->Add(new wxCheckBox(this,FFT_ON_CHECKBOX,_T("FFT"),wxDefaultPosition, wxDefaultSize,
				 wxCHK_2STATE,wxGenericValidator(&fftOn),_T("FFTCheckBox")),0);
  controlBox->Add(new wxCheckBox(this,POWER_ON_CHECKBOX,_T("Power"),wxDefaultPosition, wxDefaultSize,
				 wxCHK_2STATE,wxGenericValidator(&powerOn),_T("PowerCheckBox")),0);
  if (alfa){
    controlBox->Add(new wxCheckBox(this,ALFA_ON_CHECKBOX,_T("ALFA"),wxDefaultPosition, wxDefaultSize,
				 wxCHK_2STATE,wxGenericValidator(&alfaOn),_T("ALFACheckBox")),0);
    controlBox->Add(new wxCheckBox(this,ALFAFFT_ON_CHECKBOX,_T("ALFAFFT"),wxDefaultPosition, wxDefaultSize,
				 wxCHK_2STATE,wxGenericValidator(&alfafftOn),_T("ALFAFFTCheckBox")),0);
  }
  m_rmsValue=new wxStaticText( this , wxID_STATIC, _(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
  m_dBmValue=new wxStaticText( this , wxID_STATIC, _(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );

  controlBox->AddStretchSpacer();
  controlBox->Add(m_rmsValue,1);
  controlBox->AddStretchSpacer();
  controlBox->Add(m_dBmValue,1);

  sizer->Add(controlBox,0,wxEXPAND);
  sizer->Add(m_voltageGraph,1,wxEXPAND|wxBOTTOM, 5);
  sizer->Add(m_fftGraph,1,wxEXPAND|wxBOTTOM, 5);
  sizer->Add(m_powerGraph,1,wxEXPAND|wxBOTTOM, 5);
  sizer->Add(m_alfaGraph,1,wxEXPAND|wxBOTTOM, 5);
  sizer->Add(m_alfafftGraph,1,wxEXPAND);

  if(!voltageOn) sizer->Hide(m_voltageGraph);
  if(!fftOn) sizer->Hide(m_fftGraph);
  if(!powerOn) sizer->Hide(m_powerGraph);
  if(!alfaOn) sizer->Hide(m_alfaGraph);
  if(!alfafftOn) sizer->Hide(m_alfafftGraph);

  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);

  return;
}

/*
 * Destructor
 */
wxRFWindow::~wxRFWindow(){
  if(m_fftData) delete m_fftData;
  if(m_powerData) delete m_powerData;
}

bool wxRFWindow::Show(bool show){
  if(show){
    voltageOn=AviewOpts::RFView&AviewOpts::VoltageView;
    fftOn=AviewOpts::RFView&AviewOpts::FFTView;
    powerOn=AviewOpts::RFView&AviewOpts::PowerView;
    alfaOn=AviewOpts::RFView&AviewOpts::ALFAView;
    alfafftOn=AviewOpts::RFView&AviewOpts::ALFAFFTView;
    TransferDataToWindow();
    
    GetSizer()->Show(m_voltageGraph,voltageOn);
    GetSizer()->Show(m_fftGraph,fftOn);
    GetSizer()->Show(m_powerGraph,powerOn);
    GetSizer()->Show(m_alfaGraph,alfaOn);
    GetSizer()->Show(m_alfafftGraph,alfafftOn);
    
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    GetSizer()->Layout();
  }

  return wxDialog::Show(show);
}

// // Pass key press events to parent, i.e. main Aview Frame
// void wxRFWindow::OnKey(wxKeyEvent &event){  
//   wxLogMessage("In [wxRFWindow::OnKey]");
//   wxWindow *ancestor = GetParent();
//   while (ancestor){
//     int command = ancestor->GetAcceleratorTable()->GetCommand( event );
//     if (command != -1){
//       wxCommandEvent command_event( wxEVT_COMMAND_MENU_SELECTED, command );
//       wxLogMessage("[wxRFWindow::OnKey] sending cmd %d",command);
//       ancestor->GetEventHandler()->ProcessEvent( command_event );
//       break;
//     }
//     if (ancestor->IsTopLevel()) break;
//     ancestor = ancestor->GetParent();
//   }
// }

void wxRFWindow::InsertVoltage(wxCommandEvent& event){
  if(Validate() && TransferDataFromWindow()){
    if(voltageOn) GetSizer()->Show(m_voltageGraph);
    else GetSizer()->Hide(m_voltageGraph);
    if(GetSizer()->GetSize().GetWidth()<GetSizer()->GetMinSize().GetWidth()
       || GetSizer()->GetSize().GetHeight()<GetSizer()->GetMinSize().GetHeight()){
      GetSizer()->Fit(this);
      GetSizer()->SetSizeHints(this);
    }
    GetSizer()->Layout();
  }
}

void wxRFWindow::InsertFFT(wxCommandEvent& event){
  if(Validate() && TransferDataFromWindow()){
    if(fftOn) GetSizer()->Show(m_fftGraph);
    else GetSizer()->Hide(m_fftGraph);
    if(GetSizer()->GetSize().GetWidth()<GetSizer()->GetMinSize().GetWidth()
       || GetSizer()->GetSize().GetHeight()<GetSizer()->GetMinSize().GetHeight()){
      GetSizer()->Fit(this);
      GetSizer()->SetSizeHints(this);
    }
    GetSizer()->Layout();
  }
}

void wxRFWindow::InsertPower(wxCommandEvent& event){
  if(Validate() && TransferDataFromWindow()){
    if(powerOn) GetSizer()->Show(m_powerGraph);
    else GetSizer()->Hide(m_powerGraph);
    if(GetSizer()->GetSize().GetWidth()<GetSizer()->GetMinSize().GetWidth()
       || GetSizer()->GetSize().GetHeight()<GetSizer()->GetMinSize().GetHeight()){
      GetSizer()->Fit(this);
      GetSizer()->SetSizeHints(this);
    }
    GetSizer()->Layout();
  }
}

void wxRFWindow::InsertALFA(wxCommandEvent& event){
  if(Validate() && TransferDataFromWindow()){
    if(alfaOn) GetSizer()->Show(m_voltageGraph);
    else GetSizer()->Hide(m_voltageGraph);
    if(GetSizer()->GetSize().GetWidth()<GetSizer()->GetMinSize().GetWidth()
       || GetSizer()->GetSize().GetHeight()<GetSizer()->GetMinSize().GetHeight()){
      GetSizer()->Fit(this);
      GetSizer()->SetSizeHints(this);
    }
    GetSizer()->Layout();
  }
}

void wxRFWindow::InsertALFAFFT(wxCommandEvent& event){
  if(Validate() && TransferDataFromWindow()){
    if(alfafftOn) GetSizer()->Show(m_fftGraph);
    else GetSizer()->Hide(m_fftGraph);
    if(GetSizer()->GetSize().GetWidth()<GetSizer()->GetMinSize().GetWidth()
       || GetSizer()->GetSize().GetHeight()<GetSizer()->GetMinSize().GetHeight()){
      GetSizer()->Fit(this);
      GetSizer()->SetSizeHints(this);
    }
    GetSizer()->Layout();
  }
}
void wxRFWindow::SetData(wxGraphCurve *data){
  m_voltageData = data;
  processData();
}

void wxRFWindow::SetAverage(wxGraphCurve **data,int n,wxGraphCurve **avg){
  if (n<1) return;
  int N=260;
  //int N=data[0]->GetLength();
  // Make FFT plan
  fftw_complex *fft_in=(fftw_complex *)fftw_malloc(N*sizeof(fftw_complex));
  fftw_complex *fft_out=(fftw_complex *)fftw_malloc(N*sizeof(fftw_complex));
  fftw_plan plan,iplan;
  plan = fftw_plan_dft_1d(N, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE);
  iplan = fftw_plan_dft_1d(N, fft_out, fft_in, FFTW_BACKWARD, FFTW_ESTIMATE);
  double vavg[N],pavg[N];
  for(int i=0;i<N;++i) vavg[i]=pavg[i]=0;
  double fftpow[N/2+1];
  for(int i=0;i<N/2+1;++i) fftpow[i]=0;
  // Loop over events
  for(int i=0;i<n;++i){
    for(int j=0;j<N;++j){
      vavg[j]+=(*data[i])[j].y;
      fft_in[j][0]=(*data[i])[j].y;
      fft_in[j][1]=0;
    }
    fftw_execute(plan);
    // Calculate and add FFT power
    for(int j=0;j<N/2+1;++j){
      double ffty=fft_out[j][0]*fft_out[j][0]+fft_out[j][1]*fft_out[j][1];
      if(j>0 && j<N/2) ffty*=2; // Factor of 2 for positive and neagative frequencies
      fftpow[j]+=ffty;
    }
    // Run Hilber transform 
    for(int j=1;j<N/2;++j){
      fft_out[j][0]*=2;
      fft_out[j][1]*=2;
    }
    for(int j=N/2+2;j<N;++j){
      fft_out[j][0]=0;
      fft_out[j][1]=0;
    }
    fftw_execute(iplan);
    for(int j=0;j<N;++j){
      pavg[j]+=(fft_in[j][0]*fft_in[j][0]+fft_in[j][1]*fft_in[j][1])/(N*N);  // in mV^2
    }
  }
  fftw_destroy_plan(plan);
  fftw_destroy_plan(iplan);

  // Average voltage and export
  double x[N];
  for(int j=0;j<N;++j){
    vavg[j]/=n;
    x[j]=(*data[0])[j].x;
  }
  *avg=new wxGraphCurve(x,vavg,N);
  m_voltageData=*avg;
  GetRMS();

  // Average and normalize FFT
  double dt=fabs((*m_voltageData)[1].x-(*m_voltageData)[0].x);
  double df=1./(dt*N); 
  double fftx[N/2+1];
  for(int j=0;j<N/2+1;++j){
    fftx[j]=j*df;
    fftpow[j]/=n;
    // Normalize FFT
    fftpow[j]*=dt*dt*df; // in mV^2/GHz
    fftpow[j]/=1e9;      // in V^2/MHz
    if(fftpow[j]>0)
      fftpow[j]=10*log10(fftpow[j]/50*1e3); // in dBm/MHz, assuming 50 ohm load
    else
      fftpow[j]=-100;
  }
  if(m_fftData) delete m_fftData;
  m_fftData = new wxGraphCurve(&fftx[1],&fftpow[1],N/2); // Skip DC bin

  // Finally, average and normalize power
  for(int j=0;j<N;++j){
    pavg[j]/=n;
    pavg[j]/=1e3*50; // in mW, assuming 50 ohm load
  }

  if(m_powerData) delete m_powerData;
  m_powerData = new wxGraphCurve(x,pavg,N);

  fftw_free(fft_in);
  fftw_free(fft_out);

  m_voltageGraph->SetCurve(m_voltageData);
  m_fftGraph->SetCurve(m_fftData);
  m_powerGraph->SetCurve(m_powerData);
  
  return;
}

// Calculates FFT and signal power envelope from input data
void wxRFWindow::processData(){
  // Clear old data
  if(m_powerData) delete m_powerData;
  if(m_fftData) delete m_fftData;

  if(m_voltageData){
    // Dummy for now 
    int N=m_voltageData->GetLength();

    if(N>1){
      
      GetRMS();
      
      // Get FFT 
      fftw_complex *fft_in=(fftw_complex *)fftw_malloc(N*sizeof(fftw_complex));
      fftw_complex *fft_out=(fftw_complex *)fftw_malloc(N*sizeof(fftw_complex));
      fftw_plan plan;
      plan = fftw_plan_dft_1d(N, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE);
      for(int i=0;i<N;++i){
	fft_in[i][0]=(*m_voltageData)[i].y;
	fft_in[i][1]=0;
      }
      fftw_execute(plan);
      fftw_destroy_plan(plan);
      
      double dt=fabs((*m_voltageData)[1].x-(*m_voltageData)[0].x);
      double df=1./(dt*N); 
      
      double fftx[N/2+1];
      double ffty[N/2+1];
      for(int i=0;i<N/2+1;++i){
	fftx[i]=i*df;
	ffty[i]=fft_out[i][0]*fft_out[i][0]+fft_out[i][1]*fft_out[i][1];
	if(i>0 && i<N/2) ffty[i]*=2; // Factor of 2 for positive and negative frequencies
	// Normalize FFT
	ffty[i]*=dt*dt*df; // in mV^2/GHz
	ffty[i]/=1e9;      // in V^2/MHz
	if(ffty[i]>0)
	  ffty[i]=10*log10(ffty[i]/50*1e3); // in dBm/MHz, assuming 50 ohm load
	else
	  ffty[i]=-100;
      }
      
      m_fftData = new wxGraphCurve(&fftx[1],&ffty[1],N/2); // Skip DC bin
      
      // Calculate power of signal envelope, by running a Hilbert xform
      plan =  fftw_plan_dft_1d(N, fft_out, fft_in, FFTW_BACKWARD, FFTW_ESTIMATE);
      for(int i=1;i<N/2;++i){
	fft_out[i][0]*=2;
	fft_out[i][1]*=2;
      }
      for(int i=N/2+2;i<N;++i){
	fft_out[i][0]=0;
	fft_out[i][1]=0;
      }
      fftw_execute(plan);  
      fftw_destroy_plan(plan);
      
      double y[N];
      double x[N];
      for(int i=0;i<N;++i){
	x[i]=(*m_voltageData)[i].x;
	y[i]=(fft_in[i][0]*fft_in[i][0]+fft_in[i][1]*fft_in[i][1])/(N*N);  // in mV^2
	y[i]/=1e3*50; // in mW, assuming 50 ohm load
      }
      
      // Calculate mean power from first part of the pulse
      if(N>3){
#define FRAC 0.3333
	double mean=0;
	for(int i=0;i<N*FRAC;++i)  mean+=y[i];
	mean/=(int)(N*FRAC);
#undef FRAC
	powerRef+=mean;
	++powerN;
	for(int i=0;i<N;++i) y[i]/=mean;
	GetdBm(mean);
      }
      
      m_powerData = new wxGraphCurve(x,y,N);
      
      fftw_free(fft_in);
      fftw_free(fft_out);
    }
  }else{
    m_fftData=NULL;
    m_powerData=NULL;
  }

  m_voltageGraph->SetCurve(m_voltageData);
  m_fftGraph->SetCurve(m_fftData);
  m_powerGraph->SetCurve(m_powerData);
  
  return;
}

void wxRFWindow::GetRMS(){
  double rms=0,rms2=0;
  int rmsN=0;
  
  for(unsigned int i=0;i<m_voltageData->GetLength();++i){
    rms += (*m_voltageData)[i].y ;
    rms2 += (*m_voltageData)[i].y*(*m_voltageData)[i].y;
    ++rmsN;
  }

  rms/= rmsN ; rms2/= rmsN ;
  rms = rms2 - rms*rms;
  if (rms> 0.) rms= sqrt(rms) ;
  else rms = -1 ;
  
  wxString tmp;
  tmp.Printf(_T("RMS: %.2f mV"), rms);
  m_rmsValue->SetLabel(tmp.c_str());
}

void wxRFWindow::GetdBm(double mean){
  double ref;
  if(dBmRef!=NO_DBM_REF)
    ref=pow(10.,(dBmRef-14+6)/10);  // ~14 dBm difference between trigger and signal paths, but +6 dBm to account for about x2 difference in average of envelope and abs value of waveform 
  else if(powerN>0)
    ref=powerRef/powerN;
  else return; // No reference available
  wxString tmp;
  tmp.Printf(_T("NNR: %.2f"),mean/ref); 
  m_dBmValue->SetLabel(tmp.c_str());
}
  

