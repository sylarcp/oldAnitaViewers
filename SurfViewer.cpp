/*
 * TurfViewer.cpp
 *
 * Class that is used to display configuration and log files stored in the 
 * database. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SurfViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "SurfViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>

IMPLEMENT_CLASS(SurfViewer,wxDialog)
BEGIN_EVENT_TABLE(SurfViewer,wxDialog)
 EVT_BUTTON(TERM11_CLEAR_BUTTON_ID, SurfViewer::OnClear)
 EVT_BUTTON(SAVE_FILE11_ID, SurfViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
SurfViewer::SurfViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("Surf Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 /////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT("GpsG12PosStruct_t"),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT("checksum  code  feByte  nmBytes  packetNumber  verId)"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("unixTime  unixTimeUs  globalThreshold  errorFlag"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText3 = new wxStaticText(this, wxID_STATIC,wxT("upperWords[ACTIVE_SURFS] "),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText4 = new wxStaticText(this, wxID_STATIC,wxT("surfTrigBandMask[ACTIVE_SURFS]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText5 = new wxStaticText(this, wxID_STATIC,wxT("scalerGoals[BANDS_PER_ANT]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText6 = new wxStaticText(this, wxID_STATIC,wxT("scaler[ACTIVE_SURFS][SCALERS_PER_SURF]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText7 = new wxStaticText(this, wxID_STATIC,wxT("threshold[ACTIVE_SURFS][SCALERS_PER_SURF]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText8 = new wxStaticText(this, wxID_STATIC,wxT("setthreshold[ACTIVE_SURFS][SCALERS_PER_SURF]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText9 = new wxStaticText(this, wxID_STATIC,wxT("rfPower[ACTIVE_SURFS][RFCHAN_PER_SURF];"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM11_CLEAR_BUTTON_ID,_T("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE11_ID,_T("Save File"));
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
		 
SurfViewer::~SurfViewer(){}

void SurfViewer::SurfHKData(const FullSurfHkStruct_t  &surf){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<"\n";
//////////////////////////////// july 1, 2008 9:15 am/////////////////////////////////////////
  (*m1_display)<<double(surf.gHdr.checksum)<<_T(' ')<<int(surf.gHdr.code)<<_T(' ')<<int(surf.gHdr.feByte)<<_T(' ')<<int(surf.gHdr.numBytes)<<_T(' ')<<double(surf.gHdr.packetNumber)<<_T(' ')<<int(surf.gHdr.verId)<<_T("\n");
  (*m1_display)<<double(surf.unixTime)<<_T(' ')<<double(surf.unixTimeUs)<<_T(' ')<<int(surf.globalThreshold)<<_T(' ')<<int(surf.errorFlag)<<_T("\n");
  for(int i=0;i<ACTIVE_SURFS;i++){
  (*m1_display)<<int(surf.upperWords[i])<<_T(' ');
  	}
  (*m1_display)<<_T("\n");
  for(int j=0;j<ACTIVE_SURFS;j++){
  (*m1_display)<<int(surf.surfTrigBandMask[j])<<_T(' ');
  	}
  (*m1_display)<<_T("\n");
  for(int k=0;k<BANDS_PER_ANT;k++){
  (*m1_display)<<int(surf.scalerGoals[k])<<_T(' ');
  	}
  (*m1_display)<<_T("\n");
  for(int ii=0;ii<SCALERS_PER_SURF;ii++){
  for(int iii=0;iii<ACTIVE_SURFS;iii++){
  (*m1_display)<<int(surf.scaler[iii][ii])<<_T(' ');
  		}
  (*m1_display)<<_T("\n");
  	}
  (*m1_display)<<_T("\n");
  for(int jj=0;jj<SCALERS_PER_SURF;jj++){
  for(int jjj=0;jjj<ACTIVE_SURFS;jjj++){
  (*m1_display)<<int(surf.threshold[jjj][jj])<<_T(' ');
  		}
  (*m1_display)<<_T("\n");
  	}
  (*m1_display)<<_T("\n");
  for(int kk=0;kk<SCALERS_PER_SURF;kk++){
  for(int kkk=0;kkk<ACTIVE_SURFS;kkk++){
  (*m1_display)<<int(surf.setThreshold[kkk][kk])<<_T(' ');
  		}
  (*m1_display)<<_T("\n");
  	}
  (*m1_display)<<_T("\n");
    for(int n=0;n<RFCHAN_PER_SURF;n++){
  for(int m=0;m<ACTIVE_SURFS;m++){
  (*m1_display)<<int(surf.rfPower[m][n])<<_T(' ');
  		}
  (*m1_display)<<_T("\n");
  	}
  (*m1_display)<<_T("\n");
/////////////////////////////////////////////////////////////////////////////////////////
  }
void SurfViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}
void SurfViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("SurfData.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}






