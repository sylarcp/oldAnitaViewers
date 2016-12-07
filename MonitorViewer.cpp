/*
 * MonitorViewer.cpp
 *
 * Class that is used to display configuration and log files stored in the 
 * database. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "MonitorViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "MonitorViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>

IMPLEMENT_CLASS(MonitorViewer,wxDialog)
BEGIN_EVENT_TABLE(MonitorViewer,wxDialog)
 EVT_BUTTON(TERM1_CLEAR_BUTTON_ID, MonitorViewer::OnClear)
 EVT_BUTTON(SAVE_FILE1_ID, MonitorViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
MonitorViewer::MonitorViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("Monitor Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 /////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT("GpsG12PosStruct_t"),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT("code  checksum  feByte  numBytes  verId"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("unixTime  ntuLabel   otherLabel  usbLabel"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
 wxStaticText* staticText3 = new wxStaticText(this, wxID_STATIC,wxT("diskSpace[8]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText4 = new wxStaticText(this, wxID_STATIC,wxT("eventLinks[NUM_PRIORITIES]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText5 = new wxStaticText(this, wxID_STATIC,wxT("hkLinks[21]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText6 = new wxStaticText(this, wxID_STATIC,wxT("utime[NUM_PROCESSES]  stime[NUM_PROCESSES]  vsize[NUM_PROCESSES]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM1_CLEAR_BUTTON_ID,_("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE1_ID,_("Save File"));
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
		 
MonitorViewer::~MonitorViewer(){}

void MonitorViewer::MonitorData(const MonitorStruct_t  &monitor){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<_T("\n");
//////////////////////////////// june 27 2008 9:15 am/////////////////////////////////////////
  (*m1_display)<<double(monitor.gHdr.checksum)<<_T(' ')<<int(monitor.gHdr.code)<<_T(' ')<<int(monitor.gHdr.numBytes)<<_T(' ')<<int(monitor.gHdr.feByte)<<_T(' ')<<double(monitor.gHdr.packetNumber)<<_T(' ')<<int(monitor.gHdr.verId)<<_T("\n");
  wxString tmp ;
  tmp.Printf(_T("%d %s %s %s\n"),monitor.unixTime,monitor.diskInfo.ntuLabel,monitor.diskInfo.otherLabel,monitor.diskInfo.usbLabel);
  (*m1_display)<<tmp.c_str() ;
		     //  (*m1_display)<<monitor.unixTime<<_T(' ')<<monitor.diskInfo.ntuLabel<<_T(' ')<<monitor.diskInfo.otherLabel<<_T(' ')<<monitor.diskInfo.usbLabel<<_T("\n");
 // (*m1_display)<<int(monitor.queueInfo.cmdLinksLOS)<<_T(' ')<<int(monitor.queueInfo.cmdLinksSIP)<<_T("\n");
 for(int m=0;m<8;m++){
 	 (*m1_display)<<int(monitor.diskInfo.diskSpace[m])<<_T(' ');
 	}
  (*m1_display)<<_T("\n");
  for(int i=0;i<NUM_PRIORITIES;i++){
    (*m1_display)<<int(monitor.queueInfo.eventLinks[i])<<_T(' ');}
  (*m1_display)<<_T("\n");
  for(int j=0;j<21;j++){
  	(*m1_display)<<int(monitor.queueInfo.hkLinks[j])<<_T(' ');
  	}
  (*m1_display)<<_T("\n");
  for(int k=0;k<NUM_PROCESSES;k++){
(*m1_display)<<double(monitor.procInfo.utime[k])<<_T(' ')<<double(monitor.procInfo.stime[k])<<_T(' ')<<double(monitor.procInfo.vsize[k])<<_T("\n");
  	}
(*m1_display)<<_T("\n");
(*m1_display)<<_T("----------------------------------------------------------------")<<_T("\n");
  //(*m1_display)<<int(monitor.queueInfo.gpsLinks)<<_T(' ')<<int(monitor.queueInfo.headLinks)<<_T(' ')<<int(monitor.queueInfo.hkLinks)<<_T(' ')<<int(monitor.queueInfo.monitorLinks)<<_T("\n");
  //(*m1_display)<<int(monitor.queueInfo.pedestalLinks)<<_T(' ')<<int(monitor.queueInfo.surfHkLinks)<<_T(' ')<<int(monitor.queueInfo.turfHkLinks)<<_T("\n");
/////////////////////////////////////////////////////////////////////////////////////////// 
  }
void MonitorViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}
void MonitorViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("MonitorData.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}


