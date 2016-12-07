/*
 * DB_CmdViewer.cpp
 *
 * Class for the display of monitor data from database
 * 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "DB_CmdViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "DB_CmdViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>

IMPLEMENT_CLASS(DB_CmdViewer,wxDialog)
BEGIN_EVENT_TABLE(DB_CmdViewer,wxDialog)
 EVT_BUTTON(TERM16_CLEAR_BUTTON_ID, DB_CmdViewer::OnClear)
 EVT_BUTTON(SAVE_FILE16_ID, DB_CmdViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
DB_CmdViewer::DB_CmdViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("DB Command Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
 /////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT("GpsG12PosStruct_t"),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT("time  crc  numCmdBytes  flag"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("cmd[MAX_CMD_LENGTH]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM16_CLEAR_BUTTON_ID,_("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE16_ID,_("Save File"));
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
// boxSizer->Add(staticText3, 0, wxALIGN_LEFT|wxALL, 5);
// boxSizer->Add(staticText4, 0, wxALIGN_LEFT|wxALL, 5);
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
		 
DB_CmdViewer::~DB_CmdViewer(){}

void DB_CmdViewer::DB_CmdData(const DBCmd_t  &dbcmd){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<"\n";
//////////////////////////////// july 1, 2008 9:15 am/////////////////////////////////////////
 (*m1_display)<<dbcmd.time<<_T(' ')<<dbcmd.crc<<_T(' ')<<dbcmd.numCmdBytes<<_T(' ')<<dbcmd.flag<<_T("\n");
  for(int i=0;i<20;i++){
  	 (*m1_display)<<int(dbcmd.cmdLong[i])<<_T(' ');
  	}
  (*m1_display)<<_T("\n");
  (*m1_display)<<_T("----------------------------------------------------------------")<<_T("\n");
/////////////////////////////////////////////////////////////////////////////////////////
  }
void DB_CmdViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}
void DB_CmdViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("DBCmdData.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}








