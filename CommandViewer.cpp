/*
 * CommandViewer.cpp
 *
 * Class that is used to display configuration and log files stored in the 
 * database. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "CommandViewer.h"
#endif
#include <wx/datstrm.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "CommandViewer.h"
#include <wx/ffile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filedlg.h>
#include "wx/checkbox.h"

IMPLEMENT_CLASS(CommandViewer,wxDialog)
BEGIN_EVENT_TABLE(CommandViewer,wxDialog)
 EVT_BUTTON(TERM6_CLEAR_BUTTON_ID, CommandViewer::OnClear)
 EVT_BUTTON(SAVE_FILE6_ID, CommandViewer::SaveFile)
 //EVT_CHOICE(wxID_ANY,GpsPosViewer::FileSelect)
END_EVENT_TABLE()

// Constructor
CommandViewer::CommandViewer(wxWindow *parent){
 Create(parent,wxID_ANY,_T("Command Viewer"),wxDefaultPosition,wxSize(400,300),
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|
	 wxSYSTEM_MENU);
 wxFont ttfont=wxFont(8, wxFONTFAMILY_TELETYPE,wxNORMAL, wxNORMAL);
  //////////////////////////////////////////////////////////////////////
//wxCheckBox* checkbox = new wxCheckBox(this, ID_CHECKBOX,
//						wxT("&Check me"), wxDefaultPosition, wxDefaultSize);
//  checkbox->SetValue(false);
 /////////////////////////////////////////////////////////////////////
 //wxStaticText* staticText = new wxStaticText(this, wxID_STATIC,wxT("GpsG12PosStruct_t"),
 // wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText1 = new wxStaticText(this, wxID_STATIC,wxT(" checksum code numBytes feByte packetNumber verId"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  wxStaticText* staticText2 = new wxStaticText(this, wxID_STATIC,wxT("unixTime  goodFlag  numCmdBytes"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
   wxStaticText* staticText3 = new wxStaticText(this, wxID_STATIC,wxT("cmd[MAX_CMD_LENGTH]"),
 					     wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
  
 /////////////////////////////////////////////////////////////
 wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
 wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
 wxBoxSizer *boxSizer=new wxBoxSizer(wxVERTICAL);
 wxButton *clearButton=new wxButton(this,TERM6_CLEAR_BUTTON_ID,_("Clear"));
 wxButton *saveFileButton=new wxButton(this,SAVE_FILE6_ID,_("Save File"));
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
   //  buttonSizer->Add(checkbox);
     //  buttons->Realize();
 ////////////////////////////////////////////////////////////////

 m1_display = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,wxSize(400,250),
			    wxTE_MULTILINE|wxTE_READONLY);
 m1_display->SetFont(ttfont);

 boxSizer->Add(staticText1, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText2, 0, wxALIGN_LEFT|wxALL, 5);
 boxSizer->Add(staticText3, 0, wxALIGN_LEFT|wxALL, 5);
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
		 
CommandViewer::~CommandViewer(){}

void CommandViewer::CommandData(const CommandEcho_t  &cmd){
  //(*m1_display).Clear();
  //(*m1_display)<<"This is a data viewer."<<"\n";
//////////////////////////////// july 1, 2008 9:15 am/////////////////////////////////////////
//bool value=checkbox->GetValue();
 // if(value==true){
 (*m1_display)<<double(cmd.gHdr.checksum)<<_T(' ')<<int(cmd.gHdr.code)<<int(cmd.gHdr.numBytes)<<_T(' ')<<int(cmd.gHdr.feByte)<<_T(' ')<<double(cmd.gHdr.packetNumber)<<_T(' ')<<int(cmd.gHdr.verId)<<_T("\n");
  (*m1_display)<<double(cmd.unixTime)<<_T(' ')<<int(cmd.goodFlag)<<_T(' ')<<int(cmd.numCmdBytes)<<_T("\n");
  for(int j=0;j<MAX_CMD_LENGTH;j++){
     (*m1_display)<<int(cmd.cmd[j])<<_T(' ');}
(*m1_display)<<_T("---------------------------------------------------")<<_T("\n");
   (*m1_display)<<_T("\n");
  // }
//////////////////////////////////////////////////////////////////////////////////////////
  }
void CommandViewer::OnClear(wxCommandEvent &WXUNUSED(event)){
  m1_display->Clear();
}
void CommandViewer::SaveFile( wxCommandEvent &WXUNUSED(event) ){  
	wxFileDialog *dlg = new wxFileDialog(this,_T("Save options file"),_T(""),_T("CommandEcho.txt"),_T("Aview options(*.txt)|*.txt"),wxSAVE);  
	if (dlg->ShowModal() == wxID_OK) m1_display->SaveFile(dlg->GetPath());  
	dlg->Destroy();
	}







