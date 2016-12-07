/*
 * ADU5PatViewer.cpp
 *
 * Class that is used to display configuration and log files stored in the 
 * database. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ADU5PatViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "ADU5PatViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>

IMPLEMENT_CLASS(ADU5PatViewer,wxDialog)
BEGIN_EVENT_TABLE(ADU5PatViewer,wxDialog)
 EVT_BUTTON(TERM3_CLEAR_BUTTON_ID, ADU5PatViewer::OnClear)
 EVT_BUTTON(SAVE_FILE3_ID, ADU5PatViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
ADU5PatViewer::ADU5PatViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("ADU5 Pat Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 /////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT("GpsG12PosStruct_t"),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT("gpstype code  checksum  feByte  numBytes  verId"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("unixTime  unixTimeUs  timeOfDay  attFlag  brms"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText3 = new wxStaticText(this, wxID_STATIC,wxT("altitude  heading  latitude  longitude  mrms "),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText4 = new wxStaticText(this, wxID_STATIC,wxT("pitch  roll"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM3_CLEAR_BUTTON_ID,_("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE3_ID,_("Save File"));
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
		 
ADU5PatViewer::~ADU5PatViewer(){}

void ADU5PatViewer::ADU5PatData(const GpsAdu5PatStruct_t  &pat){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<"\n";
//////////////////////////////// july 1, 2008 9:15 am/////////////////////////////////////////
  (*m1_display)<<int(pat.gHdr.code>>16)<<_T(' ')<<double(pat.gHdr.code&0xffff)<<_T(' ')<<int(pat.gHdr.checksum)<<_T(' ')<<int(pat.gHdr.feByte)<<_T(' ')<<int(pat.gHdr.numBytes)<<_T(' ')<<double(pat.gHdr.packetNumber)<<_T(' ')<<int(pat.gHdr.verId)<<_T("\n");
  (*m1_display)<<double(pat.unixTime)<<_T(' ')<<double(pat.unixTimeUs)<<_T(' ')<<double(pat.timeOfDay)<<_T(' ')<<double(pat.attFlag)<<_T(' ')<<pat.brms <<_T("\n");
  (*m1_display)<<pat.altitude<<_T(' ')<<pat.heading<<_T(' ')<<pat.latitude<<_T(' ')<<pat.longitude<<_T(' ')<<pat.mrms<<_T(' ')<<_T("\n");
  (*m1_display)<<pat.pitch<<_T(' ')<<pat.roll<<_T("\n");
  (*m1_display)<<_T("\n");
   (*m1_display)<<_T("---------------------------------------------------")<<_T("\n");
/////////////////////////////////////////////////////////////////////////////////////////
  }
void ADU5PatViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}
void ADU5PatViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("ADU5PatData.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}




