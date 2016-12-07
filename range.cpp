/* 
 * range.cpp
 *
 * Definitions for basic range element used in color coding variables displayed
 * as text or gauges. Also definitions for dialog windows used to edit ranges. 
 * 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "range.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dcmemory.h>
#include <wx/stattext.h>
#include <wx/colordlg.h>
#include "range.h"

IMPLEMENT_DYNAMIC_CLASS(wxRangeDialog, wxDialog)

BEGIN_EVENT_TABLE( wxRangeDialog, wxDialog )

  EVT_BUTTON(ADD_BUTTON_ID, wxRangeDialog::OnAdd)
  EVT_BUTTON(REMOVE_BUTTON_ID, wxRangeDialog::OnRemove)
  EVT_BUTTON(EDIT_BUTTON_ID, wxRangeDialog::OnEdit)

END_EVENT_TABLE()

wxRangeDialog::wxRangeDialog(){}

wxRangeDialog::wxRangeDialog(wxWindow *parent, wxWindowID id,
			     const wxString &caption, int max,
			     wxRangeSpec *rangeSpec,
			     const wxPoint &pos,
			     const wxSize &size,
			     long style,
			     const wxString &name):maxSpec(max){
  Create(parent,id,caption,pos,size,style,name);

  wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *specSizer=new wxBoxSizer(wxHORIZONTAL);
  topSizer->Add(specSizer,1,wxEXPAND);

  wxBoxSizer *buttonSizer=new wxBoxSizer(wxVERTICAL);
  wxButton *addButton=new wxButton(this,ADD_BUTTON_ID,_("Add"));
  wxButton *removeButton=new wxButton(this,REMOVE_BUTTON_ID,_("Remove"));
  wxButton *editButton=new wxButton(this,EDIT_BUTTON_ID,_("Edit"));
  wxButton *okButton=new wxButton(this,wxID_OK,_("OK"));
  wxButton *cancelButton=new wxButton(this,wxID_CANCEL,_("Cancel"));
  buttonSizer->Add(addButton,0,wxALL,5);
  buttonSizer->Add(removeButton,0,wxALL,5);
  buttonSizer->Add(editButton,0,wxALL,5);
  buttonSizer->Add(10,50);
  buttonSizer->Add(okButton,0,wxALL,5);
  buttonSizer->Add(cancelButton,0,wxALL,5);
  
  m_specList = new wxListCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,
			      wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_HRULES|
			      wxLC_SINGLE_SEL|wxSUNKEN_BORDER);
  wxListItem topLabel;
  topLabel.SetText(_T("Ranges"));
  topLabel.SetImage(-1);
  m_specList->InsertColumn(0,topLabel);

  specSizer->Add(m_specList,1,wxEXPAND);
  specSizer->Add(buttonSizer,0);

  m_colourBitmaps = new wxImageList(colourBoxSize,colourBoxSize,true);
  m_rangeSpec=new wxRangeSpec[maxSpec+1];
  if(rangeSpec){
    for(int i=0;i<=maxSpec;++i) m_rangeSpec[i]=rangeSpec[i];
    initList();
  }

  SetSizerAndFit(topSizer);
  GetSizer()->SetSizeHints(this);

  return;
}

wxRangeDialog::~wxRangeDialog(){
  //  delete m_colourBitmaps;
}

void wxRangeDialog::initList(){
  m_colourBitmaps->RemoveAll();
  // Create bitmap and add to image list
  for(int i=0;m_rangeSpec[i].colour.Ok();++i){
    wxMemoryDC *dc=new wxMemoryDC;
    wxBitmap bitmap(colourBoxSize,colourBoxSize);
    dc->SelectObject(bitmap);
    dc->SetPen(*wxBLACK);
    dc->SetBrush(wxBrush(wxColour(m_rangeSpec[i].colour)));
    dc->DrawRectangle(0,0,colourBoxSize,colourBoxSize);
    delete dc;
    if(i!=m_colourBitmaps->Add(bitmap))
      wxLogError(_T("[wxRangeDialog::initList] Color bitmap list index insertion mismatch"));
  }
  m_specList->DeleteAllItems();
  m_specList->SetImageList(m_colourBitmaps,wxIMAGE_LIST_SMALL);
  // Set item text
  wxString label;
  for(int i=0;m_rangeSpec[i].colour.Ok();++i){
    wxListItem item;
    item.SetColumn(0);
    item.SetId(i);
    label.Printf(_T("[%5g,%5g)"),m_rangeSpec[i].low,m_rangeSpec[i].high);
    item.SetText(label);
    item.SetImage(i);
    item.SetData(&m_rangeSpec[i]);
    item.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_IMAGE|wxLIST_MASK_DATA);
    if(i!=m_specList->InsertItem(item))
      wxLogError(_T("[wxRangeDialog::initList] List item index insertion mismatch"));
  }
  m_specList->SetColumnWidth(0,wxLIST_AUTOSIZE);
}

void wxRangeDialog::AddExtraValControl(wxWindow *valControl,const wxString &label){
  wxBoxSizer *box=new wxBoxSizer(wxHORIZONTAL);
  if(label.Length()){
    wxStaticText *text = new wxStaticText(this,wxID_ANY,label);
    box->Add(text,1,wxEXPAND);
  }
  box->Add(valControl,1,wxEXPAND);
  GetSizer()->Add(box,0,wxEXPAND);
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();
}

void wxRangeDialog::SetRangeSpec(wxRangeSpec *rangeSpec){
  if(rangeSpec){
    for(int i=0;i<=maxSpec;++i) m_rangeSpec[i]=rangeSpec[i];
    initList();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    GetSizer()->Layout();
  }
}

void wxRangeDialog::OnAdd( wxCommandEvent& WXUNUSED(event) ){
  // Check that there is enough room
  int n=m_specList->GetItemCount();
  if(n>=maxSpec){
    wxMessageBox(_T("Maximum number of ranges already specified"),
		 _T("Range Spec Limit"), wxOK|wxICON_ERROR);
    return;
  }
  // Move terminating entry
  m_rangeSpec[n+1]=m_rangeSpec[n];
  wxRangeEditDialog *dlg=new wxRangeEditDialog(this,wxID_ANY);
  if(dlg->ShowModal()==wxOK){
    m_rangeSpec[n]=*dlg->GetRangeSpec();
    initList();
  }
  delete dlg;
}

void wxRangeDialog::OnRemove( wxCommandEvent& WXUNUSED(event) ){
  long id = -1;
  for ( ;; ){
    id = m_specList->GetNextItem(id,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
    if ( id == -1 ) break; // Non selected so ignore event

    // Move specs up
    for(int i=id;i<m_specList->GetItemCount();++i)
      m_rangeSpec[i]=m_rangeSpec[i+1];

    initList();
  }   
}

void wxRangeDialog::OnEdit( wxCommandEvent& WXUNUSED(event) ){
  long id = -1;
  for ( ;; ){
    id = m_specList->GetNextItem(id,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
    if ( id == -1 ) break; // Non selected so ignore event
    
    wxRangeSpec *spec=(wxRangeSpec*)m_specList->GetItemData(id);
    wxRangeEditDialog *dlg=new wxRangeEditDialog(this,wxID_ANY,spec);
    if(dlg->ShowModal()==wxOK){
      *spec=*dlg->GetRangeSpec();
      initList();
    }
    delete dlg;
  }   
}


/*
 * wxRangeEditDialog functions
 */ 
IMPLEMENT_DYNAMIC_CLASS(wxRangeEditDialog, wxDialog)

BEGIN_EVENT_TABLE( wxRangeEditDialog, wxDialog )

  EVT_BUTTON(COLOUR_BUTTON_ID, wxRangeEditDialog::OnColourEdit)
  EVT_BUTTON(wxID_OK, wxRangeEditDialog::OnOK)

END_EVENT_TABLE()

wxRangeEditDialog::wxRangeEditDialog(){}

wxRangeEditDialog::wxRangeEditDialog(wxWindow *parent, wxWindowID id,
				     wxRangeSpec *rangeSpec,
				     const wxPoint &pos,
				     const wxSize &size,
				     long style,
				     const wxString &name){
  Create(parent,id,_("Range Edit"),pos,size,style,name);

  if(rangeSpec) m_rangeSpec=*rangeSpec;
  else m_rangeSpec=wxRangeSpec(0,0,wxBLACK);

  wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *editSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);

  editSizer->Add(labelSizer,1,wxEXPAND);
  editSizer->Add(infoSizer,1,wxEXPAND);

  topSizer->Add(editSizer,1,wxEXPAND);

  wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
  buttons->AddButton(new wxButton(this,wxID_OK,_("OK")));
  buttons->AddButton(new wxButton(this,wxID_CANCEL,_("Cancel")));
  buttons->Realize();
  topSizer->Add(buttons,0,wxEXPAND|wxTOP,5);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_("Low range")),1);
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_("High range")),1);
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_("Color")),1);

  m_lowText = new wxTextCtrl(this,wxID_ANY);
  m_lowText->SetMaxLength(10);
  *m_lowText << m_rangeSpec.low;
  infoSizer->Add(m_lowText,0,wxEXPAND);

  m_highText = new wxTextCtrl(this,wxID_ANY);
  m_highText->SetMaxLength(10);
  *m_highText << m_rangeSpec.high;
  infoSizer->Add(m_highText,0,wxEXPAND);

  m_colourButton = new wxButton(this,COLOUR_BUTTON_ID);
  m_colourButton->SetBackgroundColour(m_rangeSpec.colour);
  infoSizer->Add(m_colourButton,0,wxEXPAND);

  SetSizerAndFit(topSizer);
  GetSizer()->SetSizeHints(this);

  return;
}

wxRangeEditDialog::~wxRangeEditDialog(){}

void wxRangeEditDialog::SetRangeSpec(wxRangeSpec *rangeSpec){
  if(rangeSpec){
    SetLow(rangeSpec->low);
    SetHigh(rangeSpec->high);
    SetColour(rangeSpec->colour);
  }
}

void wxRangeEditDialog::SetLow(double low){
  m_rangeSpec.low=low;
  m_lowText->Clear();
  *m_lowText << low;
}    

void wxRangeEditDialog::SetHigh(double high){
  m_rangeSpec.high=high;
  m_highText->Clear();
  *m_highText << high;
}    

void wxRangeEditDialog::SetColour(wxColour col){
  m_rangeSpec.colour=col;
  m_colourButton->SetBackgroundColour(col);
}
  
/* Event handlers */
void wxRangeEditDialog::OnColourEdit( wxCommandEvent &event ){
  SetColour(wxGetColourFromUser(this,m_colourButton->GetBackgroundColour()));
}

void wxRangeEditDialog::OnOK( wxCommandEvent &event ){
  double low,high;
  if(m_lowText->GetLineText(0).ToDouble(&low)) m_rangeSpec.low=low;
  if(m_highText->GetLineText(0).ToDouble(&high)) m_rangeSpec.high=high;
  m_rangeSpec.colour = m_colourButton->GetBackgroundColour();
  
  EndModal(wxOK);
}
