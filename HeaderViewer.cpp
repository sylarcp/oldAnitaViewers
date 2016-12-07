/*
 * HeaderViewer.cpp
 *
 * Class that is used to display configuration and log files stored in the 
 * database. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "HeaderViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "HeaderViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>

IMPLEMENT_CLASS(HeaderViewer,wxDialog)
BEGIN_EVENT_TABLE(HeaderViewer,wxDialog)
 EVT_BUTTON(TERM2_CLEAR_BUTTON_ID, HeaderViewer::OnClear)
 EVT_BUTTON(SAVE_FILE2_ID, HeaderViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
HeaderViewer::HeaderViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("Header Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 /////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT(_T("GpsG12PosStruct_t")),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT("code  checksum  feByte  numBytes  verId"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("unixTime   unixTimeUs  calibStatus turfEventId"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText3 = new wxStaticText(this, wxID_STATIC,wxT("priority   turfUpperWord  otherFlag   errorFlag"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText4 = new wxStaticText(this, wxID_STATIC,wxT("surfSlipFlag   nadirAntTrigMask "),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText5 = new wxStaticText(this, wxID_STATIC,wxT("trigType    l3Type1Count    deadTime  trigNum    ppsNum"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText6 = new wxStaticText(this, wxID_STATIC,wxT("c3poNum  "),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText7 = new wxStaticText(this, wxID_STATIC,wxT(" bufferDepth    reserved"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM2_CLEAR_BUTTON_ID,_T("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE2_ID,_T("Save File"));
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
		 
HeaderViewer::~HeaderViewer(){}

void HeaderViewer::HeaderData(const AnitaEventHeader_t  &header){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<_T("\n");
//////////////////////////////// july 1, 2008 9:15 am/////////////////////////////////////////
   (*m1_display)<<double(header.gHdr.checksum)<<_T(' ')<<int(header.gHdr.feByte)<<_T(' ')<<int(header.gHdr.numBytes)<<_T(' ')<<int(header.gHdr.verId)<<_T(' ')<<double(header.eventNumber)<<_T("\n");
   (*m1_display)<<double(header.unixTime)<<_T(' ')<<double(header.unixTimeUs)<<_T(' ')<<int(header.calibStatus)<<_T(' ')<<double(header.turfEventId)<<_T("\n");
   (*m1_display)<<int(header.priority)<<_T(' ')<<int(header.turfUpperWord)<<_T(' ')<<int(header.otherFlag)<<_T(' ')<<int(header.errorFlag)<<_T("\n");
   (*m1_display)<<int(header.surfSlipFlag)<<_T(' ')<<int(header.phiTrigMask)<<_T("\n");
   (*m1_display)<<int(header.turfio.trigType)<<_T(' ')<<int(header.turfio.l3Type1Count)<<_T(' ')<<int(header.turfio.deadTime)<<int(header.turfio.trigNum)<<_T(' ')<<double(header.turfio.ppsNum)<<_T("\n"); 
   (*m1_display)<<double(header.turfio.c3poNum)<<_T(' ')<<_T("\n");
   (*m1_display)<<int(header.turfio.l3TrigPattern)<<_T(' ')<<int(header.turfio.bufferDepth)<< _T(' ') <<int(header.turfio.reserved)<<_T("\n");
   (*m1_display)<<_T("\n");
   (*m1_display)<<_T("----------------------------------------------------------------")<<_T("\n");
///////////////////////////////////////////////////////////////////////////////////////////
  }
void HeaderViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}

void HeaderViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("HeaderData.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}



