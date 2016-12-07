/*
 * SlowMoViewer.cpp
 *
 * Class that is used to display configuration and log files stored in the 
 * database. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SlowMoViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "SlowMoViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>

IMPLEMENT_CLASS(SlowMoViewer,wxDialog)
BEGIN_EVENT_TABLE(SlowMoViewer,wxDialog)
 EVT_BUTTON(TERM12_CLEAR_BUTTON_ID, SlowMoViewer::OnClear)
 EVT_BUTTON(SAVE_FILE12_ID, SlowMoViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
SlowMoViewer::SlowMoViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("Surf Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 /////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT("GpsG12PosStruct_t"),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT("checksum  code  feByte  nmBytes  packetNumber  verId"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("unixTime)  rf.eventNumber  rf.eventRate1Min  rf.eventRate10Min"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText3 = new wxStaticText(this, wxID_STATIC,wxT("hk.altitude  hk.latitude  hk.longitude "),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText4 = new wxStaticText(this, wxID_STATIC,wxT("hk.temps[8]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText5 = new wxStaticText(this, wxID_STATIC,wxT("hk.power[4]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText6 = new wxStaticText(this, wxID_STATIC,wxT("rf.avgUpperL2Rates[PHI_SECTORS]  rf.avgLowerL2Rates[PHI_SECTORS]  rf.avgL3Rates[PHI_SECTORS]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText7 = new wxStaticText(this, wxID_STATIC,wxT("rfPwrAvg[ACTIVE_SURFS][RFCHAN_PER_SURF]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText8 = new wxStaticText(this, wxID_STATIC,wxT("avgScalerRates[TRIGGER_SURFS][ANTS_PER_SURF]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText9 = new wxStaticText(this, wxID_STATIC,wxT("rmsScalerRates[TRIGGER_SURFS][ANTS_PER_SURF]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText10 = new wxStaticText(this, wxID_STATIC,wxT("avgL1Rates[TRIGGER_SURFS]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);	
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM12_CLEAR_BUTTON_ID,_("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE12_ID,_("Save File"));
/*
 wxBoxSizer *controlSizer=new wxBoxSizer(wxHORIZONTAL);
 
 m_fileNames[0]="AnitaEventHeader_t.txt";
 m_fileNames[1]="MonitorStruct_t.txt";
 m_fileNames[2]="GpsAdu5PatStruct_t.txt";
 m_fileNames[3]="GpsAdu5VtgStruct_t.txt";
 m_fileNames[4]="GpsAdu5SatStruct_t.txt";
 m_fileNames[5]="HkDataStruct_t.txt";
 m_fileNames[6]="GpsG12SatStruct_t.txt";
 m_fileNames[7]="GpsG12PosStruct_t.txt";
 m_fileNames[8]="CommandEcho_t.txt";
 m_fileList = new wxChoice(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,
			   N_CONFIG_FILE_TYPES1,m_fileNames);
 
 controlSizer->Add(m_fileList,0,wxEXPAND|wxLEFT|wxRIGHT,5);
*/

// wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 ///////////////////////////////////// june 29 2008///////////////////////////
 // wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
     buttonSizer->Add(new wxButton(this,wxID_OK),0,wxALL,5);
     buttonSizer->Add(clearButton,0,wxALL,5);
     buttonSizer->Add(saveFileButton,0,wxALL,5);
     //  buttons->Realize();
 ////////////////////////////////////////////////////////////////

 m1_display = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,wxSize(400,250),
			    wxTE_MULTILINE|wxTE_READONLY);
 m1_display->SetFont(ttfont);
 boxSizer->Add(staticText1, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText2, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText3, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText4, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText5, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText6, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText7, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText8, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText9, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText10, 0, wxALIGN_LEFT|wxALL, 5);
 topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
 topSizer->Add(m1_display,1,wxEXPAND|wxBOTTOM,10);
// topSizer->Add(controlSizer,0,wxEXPAND);
 topSizer->Add(buttonSizer,0,wxEXPAND);
 //topSizer->Add(buttons,0,wxEXPAND|wxALL);//new additon
 // m_fileList->SetSelection(wxNOT_FOUND);

 SetSizerAndFit(topSizer);
 GetSizer()->SetSizeHints(this);

 return;
}
		 
SlowMoViewer::~SlowMoViewer(){}

void SlowMoViewer::SlowMoData(const SlowRateFull_t  &slow){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<"\n";
//////////////////////////////// july 1, 2008 9:15 am/////////////////////////////////////////
  (*m1_display)<<double(slow.gHdr.checksum)<<_T(' ')<<int(slow.gHdr.code)<<_T(' ')<<int(slow.gHdr.feByte)<<_T(' ')<<int(slow.gHdr.numBytes)<<_T(' ')<<double(slow.gHdr.packetNumber)<<_T(' ')<<int(slow.gHdr.verId)<<_T("\n");
  (*m1_display)<<double(slow.unixTime)<<_T(' ')<<double(slow.rf.eventNumber)<<_T(' ')<<int(slow.rf.eventRate1Min)<<_T(' ')<<int(slow.rf.eventRate10Min)<<_T("\n");
   (*m1_display)<<int(slow.hk.altitude)<<_T(' ')<<int(slow.hk.latitude)<<_T(' ')<<int(slow.hk.longitude)<<_T("\n");
  for(int i=0;i<8;i++){
  (*m1_display)<<int(slow.hk.temps[i])<<_T(' ');
  	}
  (*m1_display)<<_T("\n");
  for(int j=0;j<4;j++){
  (*m1_display)<<int(slow.hk.powers[j])<<_T(' ');
  	}
  (*m1_display)<<_T("\n");
  for(int k=0;k<PHI_SECTORS;k++){
    //(*m1_display)<<int(slow.rf.avgUpperL2Rates[k])<<_T(' ')<<int(slow.rf.avgLowerL2Rates[k])<<_T(' ')<<int(slow.rf.avgL3Rates[k])<<_T("\n");
  //(*m1_display)<<int(slow.rf.avgL2Rates[k])<<_T(' ')<<int(slow.rf.avgL3Rates[k])<<_T("\n");//  peng: TODO
  	}
  (*m1_display)<<_T("\n");
  for(int ii=0;ii<RFCHAN_PER_SURF;ii++){
  for(int iii=0;iii<ACTIVE_SURFS;iii++){
  (*m1_display)<<int(slow.rf.rfPwrAvg[iii][ii])<<_T(' ');
  		}
  (*m1_display)<<_T("\n");
  	}
  (*m1_display)<<_T("\n");
  for(int jj=0;jj<ANTS_PER_SURF;jj++){
  for(int jjj=0;jjj<TRIGGER_SURFS;jjj++){
  ;//(*m1_display)<<int(slow.rf.avgScalerRates[jjj][jj])<<_T(' ');
  		}
  (*m1_display)<<_T("\n");
  	}
  (*m1_display)<<_T("\n");
/*  peng TODO
  for(int kk=0;kk<ANTS_PER_SURF;kk++){
  for(int kkk=0;kkk<TRIGGER_SURFS;kkk++){
  (*m1_display)<<int(slow.rf.rmsScalerRates[kkk][kk])<<_T(' ');
  		}
  (*m1_display)<<_T("\n");
  	}
  (*m1_display)<<_T("\n");
    for(int n=0;n<TRIGGER_SURFS;n++){

  (*m1_display)<<int(slow.rf.avgL1Rates[n])<<_T(' ');

  	}
  (*m1_display)<<_T("\n");
*/
/////////////////////////////////////////////////////////////////////////////////////////
  }
void SlowMoViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}
void SlowMoViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("slowData.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}







