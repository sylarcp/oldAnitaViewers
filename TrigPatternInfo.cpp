/*
 * TrigPatternInfo.cpp
 *
 * Definitions for window that displays cumulative trigger pattern histograms.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TrigPatternInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "TrigPatternInfo.h"
extern struct SurfMapType surfMap[];

IMPLEMENT_CLASS(TrigPatternInfo, wxDialog )

BEGIN_EVENT_TABLE( TrigPatternInfo, wxDialog )
  EVT_BUTTON(REQUEST_BUTTON_ID,TrigPatternInfo::OnRequest)
  EVT_BUTTON(NOW_BUTTON_ID,TrigPatternInfo::OnNow)
  EVT_BUTTON(HISTCLEAR_BUTTON_ID,TrigPatternInfo::OnClear)
  EVT_TEXT_ENTER(TIME_DISPLAY,TrigPatternInfo::OnRequest)
  EVT_TEXT_ENTER(LENGTH_DISPLAY,TrigPatternInfo::OnRequest)
  EVT_TEXT_ENTER(FILTER_DISPLAY,TrigPatternInfo::OnRequest)
END_EVENT_TABLE()

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

/*
 * Constructor
 */
TrigPatternInfo::TrigPatternInfo( wxWindow* parent ){
  Create(parent,wxID_ANY,_T("Trigger Pattern"),wxDefaultPosition,wxDefaultSize,
	 wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU|wxFULL_REPAINT_ON_RESIZE);
  
  //SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);

  m_l3=new HistInfo(this,N_PHI,N_PHI,_T("L3V"));
  m_l3H=new HistInfo(this,N_PHI,N_PHI,_T("L3H"));

  sizer->Add(m_l3,1,wxEXPAND);
  sizer->Add(m_l3H,1,wxEXPAND);

  wxBoxSizer *sqlSizer=new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  sqlSizer->Add(labelSizer,0,wxEXPAND);
  sqlSizer->Add(infoSizer,1,wxEXPAND);
  sizer->Add(sqlSizer,0,wxEXPAND);
  
  wxBoxSizer *buttons=new wxBoxSizer(wxHORIZONTAL);
  buttons->Add(new wxButton(this,REQUEST_BUTTON_ID,_("Request")),0,wxRIGHT,5);
  buttons->Add(new wxButton(this,NOW_BUTTON_ID,_("UTC Now")),0,wxRIGHT|wxLEFT,5);
  buttons->Add(new wxButton(this,HISTCLEAR_BUTTON_ID,_("Clear")),0,wxRIGHT|wxLEFT,5);
  buttons->Add(new wxButton(this,wxID_OK,_("OK")),0,wxLEFT,5);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Reference UTC: ")), 1);
  m_refTimeEntry = new wxUTCDateCtrl(this,TIME_DISPLAY,
				     wxDateTime::Now().ToUTC().Format(_T("%Y-%m-%d %H:%M:%S")),
				     wxDefaultPosition,wxDefaultSize,
				     wxTE_PROCESS_ENTER); 
  infoSizer->Add(m_refTimeEntry,0,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Length [s]: ")), 1);
  m_lengthEntry = new wxTextCtrl(this,LENGTH_DISPLAY,_T("-100"),
				 wxDefaultPosition,wxDefaultSize,
				 wxTE_PROCESS_ENTER); 
  infoSizer->Add(m_lengthEntry,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("SQL filter: ")), 1);
  m_sqlFilter = new wxTextCtrl(this,LENGTH_DISPLAY,_T(""),
			       wxDefaultPosition,wxDefaultSize,
			       wxTE_PROCESS_ENTER); 
  infoSizer->Add(m_sqlFilter,0,wxEXPAND);

  sizer->Add(buttons,0,wxEXPAND);

  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
  
  return;
}

/*
 * Destructor
 */
TrigPatternInfo::~TrigPatternInfo(){}

void TrigPatternInfo::SetPattern(unsigned short l3, unsigned short l3H){

  double val[N_PHI];
  for(int i=0;i<N_PHI;++i) val[i]=((l3&(0x1<<i))!=0);
  m_l3->IncrementValues(val);

  double valH[N_PHI];
  for(int i=0;i<N_PHI;++i) valH[i]=((l3H&(0x1<<i))!=0);
  m_l3H->IncrementValues(valH);
}

void TrigPatternInfo::OnNow(wxCommandEvent &WXUNUSED(event)){
  wxString now=wxDateTime::Now().ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_refTimeEntry->Clear();
  (*m_refTimeEntry) << now;
}

void TrigPatternInfo::OnClear(wxCommandEvent &WXUNUSED(event)){
  m_l3->Clear();
  m_l3H->Clear();
}

void TrigPatternInfo::OnRequest(wxCommandEvent &WXUNUSED(event)){
  m_refTick=m_refTimeEntry->GetTicks();

  long length;
  if(!m_lengthEntry->GetLineText(0).ToLong(&length)){
    wxMessageBox(_T("Unable to parse record length. Please use valid integer value."),_T("SQL Request Error"),wxOK|wxICON_ERROR);
    return;
  }

  wxString sqlFilter=m_sqlFilter->GetLineText(0);
  if(sqlFilter.Length()) sqlFilter.Prepend(_T(" AND "));

  wxString cmd;
  unsigned int startTime,endTime;
  if(length>0){
    startTime=m_refTick;
    endTime=startTime+length;
  }else{
    endTime=m_refTick;
    startTime=endTime+length;
  }

  cmd.Printf(_T("SELECT l1trigpat,l2trigpat,l3trigpat FROM hd WHERE time>=%u AND time<=%u %s;"),startTime,endTime,sqlFilter.c_str());

  // Prepare SQL request
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  // Send it
  GetParent()->GetEventHandler()->ProcessEvent( sqlEvent );
}

void TrigPatternInfo::sqlParser(PGresult *result){ 
  // Clear displays 
  m_l3->Clear();
  m_l3H->Clear();
  ExecStatusType status=PQresultStatus(result);
  if(status!=PGRES_TUPLES_OK){
    wxLogError(_U(PQresStatus(status)));
    wxLogError(_U(PQresultErrorMessage(result)));
  }else{
    int n=PQntuples(result);
    int coll3=PQfnumber(result,"l3trigpat");
    int coll3H=PQfnumber(result,"l3trigpath");

    if(coll3>=0){
      double l3[N_PHI];
      for(int i=0;i<N_PHI;++i)
	l3[i]=0;
      for(int i=0;i<n;++i){
	unsigned short l3TrigPattern=atol(PQgetvalue(result,i,coll3));
	for(int i=0;i<N_PHI;++i){
	  l3[i]+=((l3TrigPattern&(0x1<<i))!=0);
	}
      }
      m_l3->SetValues(l3);
    }else
      wxLogError(_T("[TrigPatternInfo::sqlParser] Bad SQL columns (%d,%d,%d)."),coll3);

    if(coll3H>=0){
      double l3H[N_PHI];
      for(int i=0;i<N_PHI;++i)
	l3H[i]=0;
      for(int i=0;i<n;++i){
	unsigned short l3TrigPatternH=atol(PQgetvalue(result,i,coll3H));
	for(int i=0;i<N_PHI;++i){
	  l3H[i]+=((l3TrigPatternH&(0x1<<i))!=0);
	}
      }

      m_l3H->SetValues(l3H);
    }else
      wxLogError(_T("[TrigPatternInfo::sqlParser] Bad SQL columns (%d,%d,%d)."),coll3);
  }

  PQclear(result);
}
