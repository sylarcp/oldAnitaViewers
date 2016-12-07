/*
 * HKViewer.cpp
 *
 * Class for the display of House keeping data from the flight files.
 * 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DB_HKViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "DB_HKViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>

IMPLEMENT_CLASS(DB_HKViewer,wxDialog)
BEGIN_EVENT_TABLE(DB_HKViewer,wxDialog)
 EVT_BUTTON(TERM13_CLEAR_BUTTON_ID, DB_HKViewer::OnClear)
 EVT_BUTTON(SAVE_FILE13_ID, DB_HKViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
DB_HKViewer::DB_HKViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("DB HK Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 /////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT("GpsG12PosStruct_t"),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT("time  pressh  pressl  magx  magy  magz  cpu1  cpu2"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("p12v  p3_3v  p5v  p5sbv  p12v  p24v  ppvv  n5v  n12v  iprf1v  iprf2v"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText3 = new wxStaticText(this, wxID_STATIC,wxT("p12i  p3_3i  p5i  p5sbi  p12i  p24i  ppvi    n12i    bat"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText4 = new wxStaticText(this, wxID_STATIC,wxT("accx[0]  accx[1]  accy[0]  accy[1]  accz[0]  accz[1]  acct[0]  acct[1]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText5 = new wxStaticText(this, wxID_STATIC,wxT("ssel[0]  ssaz[0]  sst[0]  ssflag[0]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText6 = new wxStaticText(this, wxID_STATIC,wxT("ssel[1]  ssaz[1]  sst[1]  ssflag[1]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText7 = new wxStaticText(this, wxID_STATIC,wxT("ssel[2]  ssaz[2]  sst[2]  ssflag[2]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText8 = new wxStaticText(this, wxID_STATIC,wxT("ssel[3]  ssaz[3]  sst[3]  ssflag[3]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText9 = new wxStaticText(this, wxID_STATIC,wxT("it[25]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText10 = new wxStaticText(this, wxID_STATIC,wxT("et[25]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText11 = new wxStaticText(this, wxID_STATIC,wxT("accx[0]  accy[0]  accz[0]  acct[0]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText12 = new wxStaticText(this, wxID_STATIC,wxT("accx[1]  accy[1]  accz[1]  acct[1]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM13_CLEAR_BUTTON_ID,_("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE13_ID,_("Save File"));
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
 boxSizer->Add(staticText11, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText12, 0, wxALIGN_LEFT|wxALL, 5);
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
		 
DB_HKViewer::~DB_HKViewer(){}

void DB_HKViewer::DB_HKData(const DBHK_t  &dbhk){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<"\n";
//////////////////////////////// july 1, 2008 9:15 am/////////////////////////////////////////
 (*m1_display)<<dbhk.time<<_T(' ')<<dbhk.pressh<<_T(' ')<<dbhk.pressl<<_T(' ')<<dbhk.magx<<_T(' ')<<dbhk.magy<<dbhk.magz<<_T(' ')<<dbhk.cpu1<<_T(' ')<<dbhk.cpu2<<_T("\n");
  (*m1_display)<<dbhk.p12v<<_T(' ')<<dbhk.p3_3v<<_T(' ')<<dbhk.p5v<<_T(' ')<<dbhk.p5sbv<<_T(' ')<<dbhk.p12v<<_T(' ')<<dbhk.p24v<<_T(' ')<<dbhk.ppvv<<_T(' ')<<dbhk.n5v<<_T(' ')<<dbhk.n12v<<_T(' ')<<dbhk.iprf1v<<_T(' ')<<dbhk.iprf2v<<_T("\n");
   (*m1_display)<<dbhk.p12i<<_T(' ')<<dbhk.p3_3i<<_T(' ')<<dbhk.p5i<<_T(' ')<<dbhk.p5sbi<<_T(' ')<<dbhk.p12i<<_T(' ')<<dbhk.p24i<<_T(' ')<<dbhk.ppvi<<_T(' ')<<dbhk.n12i<<_T(' ')<<dbhk.bati<<_T("\n");
  (*m1_display)<<dbhk.accx[0]<<_T(' ')<<dbhk.accx[1]<<_T(' ')<<dbhk.accy[0]<<_T(' ')<<dbhk.accy[1]<<_T(' ')<<dbhk.accz[0]<<_T(' ')<<dbhk.accz[1]<<_T(' ')<<dbhk.acct[0]<<_T(' ')<<dbhk.acct[1]<<_T("\n");
  for(int i=0;i<4;i++){
  (*m1_display)<<dbhk.ssel[i]<<_T(' ')<<dbhk.ssaz[i]<<_T(' ')<<dbhk.sst[i]<<_T(' ')<<dbhk.ssflag[i]<<_T("\n");
  	}
   for(int j=0;j<25;j++){
  (*m1_display)<<dbhk.it[j]<<_T(' ');
  	}
   (*m1_display)<<_T("\n");
    for(int k=0;k<25;k++){
  (*m1_display)<<dbhk.et[k]<<_T(' ');
  	}
   (*m1_display)<<_T("\n");
(*m1_display)<<dbhk.accx[0]<<_T(' ')<<dbhk.accy[0]<<_T(' ')<<dbhk.accz[0]<<_T(' ')<<dbhk.acct[0]<<_T("\n");
(*m1_display)<<dbhk.accx[1]<<_T(' ')<<dbhk.accy[1]<<_T(' ')<<dbhk.accz[1]<<_T(' ')<<dbhk.acct[1]<<_T("\n");
  (*m1_display)<<_T("\n");
  (*m1_display)<<_T("----------------------------------------------------------------")<<_T("\n");
/////////////////////////////////////////////////////////////////////////////////////////
  }
void DB_HKViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}
void DB_HKViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("DBHKCalData.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}






