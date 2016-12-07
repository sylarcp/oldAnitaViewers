/*
 * GPSSatInfo.cpp
 *
 * Definitions for display of GPS satellite info in a separate window.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "GPSSatInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/datetime.h>
#include "GPSSatInfo.h"

IMPLEMENT_CLASS(GPSSatInfo, wxDialog )

/*
 * Constructor
 */
GPSSatInfo::GPSSatInfo( wxWindow* parent , int max):maxSat(max){
  Create(parent,wxID_ANY,_T("GPS Satellites"),wxDefaultPosition,
	 wxSize(350,300),wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxCLOSE_BOX|wxSYSTEM_MENU);
  
  SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));

  wxStdDialogButtonSizer *buttons=new wxStdDialogButtonSizer();
  buttons->AddButton(new wxButton(this,wxID_OK,_("OK")));
  buttons->Realize();

  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
  wxNotebook *m_notebook = new wxNotebook(this,wxID_ANY);
  sizer->Add(m_notebook,1,wxEXPAND);
  sizer->Add(buttons,0,wxEXPAND);

  // Add G12 page
  wxPanel *g12Page = new wxPanel(m_notebook,wxID_ANY,wxDefaultPosition,wxSize(350,250));
  wxBoxSizer *g12Sizer=new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *g12TextSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *g12labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *g12infoSizer=new wxBoxSizer(wxVERTICAL);
  g12TextSizer->Add(g12labelSizer,0,wxEXPAND);
  g12TextSizer->Add(g12infoSizer,1,wxEXPAND);
  
  g12labelSizer->Add(new wxStaticText(g12Page,wxID_ANY,_T("UTC Time: ")),0);
  m_G12TimeDisplay = new wxStaticText(g12Page,wxID_ANY,_T("                      ")); 
  g12infoSizer->Add(m_G12TimeDisplay,0,wxEXPAND);

  g12labelSizer->Add(new wxStaticText(g12Page,wxID_ANY,_T("# satellites: ")),0);
  m_G12NumSatellitesDisplay = new wxHistoryStaticText(g12Page,wxID_ANY,_T("%.0f"),
						      _T("G12 Sat #"),_T("min"),
						      _T("sat #"),_T("numsats"),
						      _T("g12_sat"));
  g12infoSizer->Add(m_G12NumSatellitesDisplay,0,wxEXPAND);

  g12Sizer->Add(g12TextSizer,0,wxEXPAND);

  m_G12SatList = new wxListCtrl(g12Page,wxID_ANY,wxDefaultPosition,wxSize(250,300),
				wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_HRULES|
				wxLC_VRULES|wxLC_SINGLE_SEL|wxSUNKEN_BORDER);
  // Set column labels, and blank items for entries
  wxString labels[5]={_T("PRN"),_T("elevation"),_T("SNR"),
		      _T("flag"),_T("azimuth")};
  for(int i=0;i<5;++i){
    wxListItem item;
    item.SetText(labels[i]);
    m_G12SatList->InsertColumn(i,item);
    for(int j=0;j<maxSat;++j){
      wxListItem tmp;
      tmp.SetId(j);
      tmp.SetColumn(i);
      m_G12SatList->InsertItem(tmp);
    }
#ifdef LINUX   
    m_G12SatList->SetColumnWidth(i,wxLIST_AUTOSIZE_USEHEADER);
#else
    m_G12SatList->SetColumnWidth(i,70);
#endif
  }

  g12Sizer->Add(m_G12SatList,1,wxEXPAND);
  g12Page->SetSizerAndFit(g12Sizer);

  m_notebook->AddPage(g12Page,_("G12"),true);
  
  // Add ADU5 pages
  wxString caption;
  wxString dbVar;
  for(int p=0;p<4;++p){
    wxPanel *adu5Page = new wxPanel(m_notebook,wxID_ANY);
    wxBoxSizer *adu5Sizer=new wxBoxSizer(wxVERTICAL);
    
    wxBoxSizer *adu5TextSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *adu5labelSizer=new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *adu5infoSizer=new wxBoxSizer(wxVERTICAL);
    adu5TextSizer->Add(adu5labelSizer,0,wxEXPAND);
    adu5TextSizer->Add(adu5infoSizer,1,wxEXPAND);
  
    adu5labelSizer->Add(new wxStaticText(adu5Page,wxID_ANY,_T("UTC Time: ")),0);
    m_ADU5TimeDisplay[p] = new wxStaticText(adu5Page,wxID_ANY,_T("                      ")); 
    adu5infoSizer->Add(m_ADU5TimeDisplay[p],0,wxEXPAND);

    adu5labelSizer->Add(new wxStaticText(adu5Page,wxID_ANY,_T("# satellites: ")),0);
    caption.Printf(_T("ADU5[%d] Sat #"),p);
    dbVar.Printf(_T("numsats[%d]"),p+1);
    m_ADU5NumSatellitesDisplay[p] = new wxHistoryStaticText(adu5Page,wxID_ANY,
							    _T("%.0f"),caption,
							    _T("min"),_T("sat #"),
							    dbVar,_T("adu5_sat"));
    adu5infoSizer->Add(m_ADU5NumSatellitesDisplay[p],0,wxEXPAND);

    adu5Sizer->Add(adu5TextSizer,0,wxEXPAND);

    m_ADU5SatList[p] = new wxListCtrl(adu5Page,wxID_ANY,wxDefaultPosition,wxDefaultSize,
				      wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_HRULES|
				      wxLC_VRULES|wxLC_SINGLE_SEL|wxSUNKEN_BORDER);
    for(int i=0;i<5;++i){
      wxListItem item;
      item.SetText(labels[i]);
      m_ADU5SatList[p]->InsertColumn(i,item);
      for(int j=0;j<maxSat;++j){
	wxListItem tmp;
	tmp.SetId(j);
	tmp.SetColumn(i);
	m_ADU5SatList[p]->InsertItem(tmp);
      }
#ifdef LINUX   
      m_ADU5SatList[p]->SetColumnWidth(i,wxLIST_AUTOSIZE_USEHEADER);
#else
      m_ADU5SatList[p]->SetColumnWidth(i,70);
#endif
    }

    adu5Sizer->Add(m_ADU5SatList[p],1,wxEXPAND);
    adu5Page->SetSizerAndFit(adu5Sizer);

    wxString str;
    str << _T("ADU5 [") << p << _T("]");
    m_notebook->AddPage(adu5Page,str,false);
 
  }

  // for ADU5 unit #2
  for(int p=0;p<4;++p){
    wxPanel *adu5Page_2 = new wxPanel(m_notebook,wxID_ANY);
    wxBoxSizer *adu5Sizer_2=new wxBoxSizer(wxVERTICAL);
    
    wxBoxSizer *adu5TextSizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *adu5labelSizer_2=new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *adu5infoSizer_2=new wxBoxSizer(wxVERTICAL);
    adu5TextSizer_2->Add(adu5labelSizer_2,0,wxEXPAND);
    adu5TextSizer_2->Add(adu5infoSizer_2,1,wxEXPAND);
  
    adu5labelSizer_2->Add(new wxStaticText(adu5Page_2,wxID_ANY,_T("UTC Time: ")),0);
    m_ADU5TimeDisplay2[p] = new wxStaticText(adu5Page_2,wxID_ANY,_T("                      ")); 
    adu5infoSizer_2->Add(m_ADU5TimeDisplay2[p],0,wxEXPAND);

    adu5labelSizer_2->Add(new wxStaticText(adu5Page_2,wxID_ANY,_T("# satellites: ")),0);
    caption.Printf(_T("ADU5-2[%d] Sat #"),p);
    dbVar.Printf(_T("numsats[%d]"),p+1);
    m_ADU5NumSatellitesDisplay2[p] = new wxHistoryStaticText(adu5Page_2,wxID_ANY,
							    _T("%.0f"),caption,
							    _T("min"),_T("sat #"),
							    dbVar,_T("adu5_sat"));
    adu5infoSizer_2->Add(m_ADU5NumSatellitesDisplay2[p],0,wxEXPAND);

    adu5Sizer_2->Add(adu5TextSizer_2,0,wxEXPAND);

    m_ADU5SatList2[p] = new wxListCtrl(adu5Page_2,wxID_ANY,wxDefaultPosition,wxDefaultSize,
				      wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_HRULES|
				      wxLC_VRULES|wxLC_SINGLE_SEL|wxSUNKEN_BORDER);
    for(int i=0;i<5;++i){
      wxListItem item;
      item.SetText(labels[i]);
      m_ADU5SatList2[p]->InsertColumn(i,item);
      for(int j=0;j<maxSat;++j){
	wxListItem tmp;
	tmp.SetId(j);
	tmp.SetColumn(i);
	m_ADU5SatList2[p]->InsertItem(tmp);
      }
#ifdef LINUX   
      m_ADU5SatList2[p]->SetColumnWidth(i,wxLIST_AUTOSIZE_USEHEADER);
#else
      m_ADU5SatList2[p]->SetColumnWidth(i,70);
#endif
    }

    adu5Sizer_2->Add(m_ADU5SatList2[p],1,wxEXPAND);
    adu5Page_2->SetSizerAndFit(adu5Sizer_2);

    wxString str;
    str << _T("ADU5-2 [") << p << _T("]");
    m_notebook->AddPage(adu5Page_2,str,false);
 
  }

  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);
  
  return;
}

/*
 * Destructor
 */
GPSSatInfo::~GPSSatInfo(){}


void GPSSatInfo::SetG12Sat(const GpsG12SatStruct_t &gps){
  wxString tmp;
  wxDateTime time((time_t)gps.unixTime);
  tmp=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_G12TimeDisplay->SetLabel(tmp);

  m_G12NumSatellitesDisplay->SetLabel((double)(gps.numSats&0xff));
  
  SetList(m_G12SatList,gps.sat,gps.numSats&0xff); // Hack
  
  return;
}

void GPSSatInfo::SetADU5Sat(const GpsAdu5SatStruct_t &gps){
  wxString tmp,timestr;
  wxDateTime time((time_t)gps.unixTime);
  timestr=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));

  for(int p=0;p<4;++p){
    m_ADU5TimeDisplay[p]->SetLabel(timestr);

    m_ADU5NumSatellitesDisplay[p]->SetLabel((double)gps.numSats[p]);
  
    SetList(m_ADU5SatList[p],gps.sat[p],gps.numSats[p]);
  }

  return;
}

void GPSSatInfo::SetADU5Sat_U2(const GpsAdu5SatStruct_t &gps){
  wxString tmp,timestr;
  wxDateTime time((time_t)gps.unixTime);
  timestr=time.ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));

  for(int p=0;p<4;++p){
    m_ADU5TimeDisplay2[p]->SetLabel(timestr);

    m_ADU5NumSatellitesDisplay2[p]->SetLabel((double)gps.numSats[p]);
  
    SetList(m_ADU5SatList2[p],gps.sat[p],gps.numSats[p]);
  }

  return;
}

void GPSSatInfo::SetList(wxListCtrl *list, const GpsSatInfo_t sat[],long nSat){
  wxString tmp;
  if(nSat>maxSat) nSat=maxSat;
  for(int i=0;i<nSat;++i){
    wxListItem item;
    item.SetId(i);
    
    for(int j=0;j<5;++j){
      item.SetColumn(j);
      switch(j){
      case 0: tmp.Printf(_T("%u"),sat[i].prn); break;
      case 1: tmp.Printf(_T("%u"),sat[i].elevation); break;
      case 2: tmp.Printf(_T("%u"),sat[i].snr); break;
      case 3: tmp.Printf(_T("%u"),sat[i].flag); break;
      case 4: tmp.Printf(_T("%d"),sat[i].azimuth); break;
      default: tmp.Clear(); break;
      }
      item.SetText(tmp);    
      list->SetItem(item);
    }
  }
}
