/*
 * TriggerInfo.cpp
 *
 * Definitions for display of RF trigger information.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TriggerInfo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "aviewdefs.h"
#include "TriggerInfo.h"
#include <includes/anitaStructures.h>

// Constructor
TriggerInfo::TriggerInfo( wxWindow* parent ):wxPanel(parent){

  // Create and arrange all children
  wxStaticBox *triggerBox = new wxStaticBox(this,wxID_ANY,_("Trigger Condition"));
  wxStaticBoxSizer *topSizer=new wxStaticBoxSizer(triggerBox,wxHORIZONTAL);
  wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
  topSizer->Add(labelSizer,0,wxEXPAND);
  topSizer->Add(infoSizer,1,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Priority: "),
				   wxDefaultPosition,wxDefaultSize,
				   wxALIGN_LEFT), 0);
  m_PriorityDisplay = new wxHistoryStaticText(this,wxID_ANY, _T("%0.f"), 
					      _T("Priority"),
					      _T("0:1:2:3:4:5:6:7:8:9:10:"),
					      _T("priority"),
					      _T("priority"),_T("hd"));
  infoSizer->Add(m_PriorityDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Type: ")), 0);
  m_TypeDisplay = new wxCodedStaticText(this,wxID_ANY,
					_T("1=Trig_RF|2=PPS1|4=PPS2|8=Soft/Ext|16=L3Type1"),
					_T("Trigger Type"),
					_T("0:1:2:4:8:16:32:"),
					_T("trig type"),_T("trigtype"),_T("hd"));
  infoSizer->Add(m_TypeDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Number: ")), 0);
  m_NumberDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
					    _T("Trigger Number"),_T("min"),_T("TrigNum"),
					    _T("trignum"),_T("hd"));
  infoSizer->Add(m_NumberDisplay,0,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("L3 count: ")), 0);
  m_L3NumberDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),
					      _T("L3 count"),_T("min"),_T("L3#"),
					      _T("l3cnt"),_T("hd"));
  infoSizer->Add(m_L3NumberDisplay,0,wxEXPAND);
  
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Time : ")), 0);
  /*  m_TimeNSDisplay = new wxHistoryStaticText(this,wxID_ANY,"%.10f","Trig Time",
					    "0:0.1:0.2:0.3:0.4:0.5:0.6:0.7:0.8:0.9:1:",
					    "Trig Time","trigtime","hd");
  */
  m_TimeNSDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_TimeNSDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("PPS: ")), 0);
  m_PPSDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.0f"),_T("PPS"),
					 _T("0:1e4:2e4:3e4:4e4:5e4:6e4:7e4:"),
					 _T("PPS"),_T("pps"),_T("hd"));
  infoSizer->Add(m_PPSDisplay,0,wxEXPAND);

/*
  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Deadtime: ")), 0);
  m_DeadtimeDisplay = new wxHistoryStaticText(this,wxID_ANY,_T("%.6f"),
					      _T("Deadtime Fraction"),_T("min"),
					      _T("deadtime"),_T("deadtime"),_T("hd"));
  infoSizer->Add(m_DeadtimeDisplay,0,wxEXPAND);
*/

//   labelSizer->Add(new wxStaticText(this,wxID_ANY,"C3PO: "), 0);
//   m_C3PODisplay = new wxHistoryStaticText(this,wxID_ANY,"%.0f",
// 					  "C3PO",
// 					  "0:1e8:2e8:3e8:4e8:5e8:6e8:7e8:9e8:1e9:", // Should change this binning once target range is known
// 					  "C3PO","c3po","hd");
//   infoSizer->Add(m_C3PODisplay,0,wxEXPAND);

//   labelSizer->Add(new wxStaticText(this,wxID_ANY,"Delta t: "), 0);
//   m_IntervalDisplay = new wxHistoryStaticText(this,wxID_ANY,"%.0f",
// 					      "Trigger Interval"
// 					      "0:1e8:2e8:3e8:4e8:5e8:6e8:7e8:9e8:1e9:",
// 					      "dt","trigint","hd");
//   infoSizer->Add(m_IntervalDisplay,0,wxEXPAND);

  //  labelSizer->Add(new wxStaticText(this,wxID_ANY,"Threshold: "), 0);
  //  m_ThresholdDisplay = new wxStaticText(this,wxID_ANY,"");
  //  infoSizer->Add(m_ThresholdDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("TURF monitor: ")), 0);
  m_TurfMonitorDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_TurfMonitorDisplay,0,wxEXPAND);

// surfmask no longer associated with even HD, commented out Aug-08
//   labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("SURF mask: ")), 0);
//   m_SurfMaskDisplay = new wxStaticText(this,wxID_ANY,_T(""));
//   infoSizer->Add(m_SurfMaskDisplay,0,wxEXPAND);

  labelSizer->Add(new wxStaticText(this,wxID_ANY,_T("Calibration: ")), 0);
  m_CalibDisplay = new wxStaticText(this,wxID_ANY,_T(""));
  infoSizer->Add(m_CalibDisplay,0,wxEXPAND);

  topSizer->CalcMin();
  SetSizerAndFit(topSizer);
  topSizer->SetSizeHints(this);

}

TriggerInfo::~TriggerInfo(){}

void TriggerInfo::SetPriority(unsigned short priority){
  wxString tmp;
  tmp.Printf(_T("%d -- (Jim %d)"),priority&0xf,(priority&0xf0)>>4);
  m_PriorityDisplay->SetLabel(tmp);
}

void TriggerInfo::SetTriggerType(unsigned char type){
  m_TypeDisplay->SetLabel(type);
}

void TriggerInfo::SetTriggerNumber(unsigned short trnum, unsigned char otherFlag2){
    wxString tmp;
    tmp.Printf(_T("%d  %s"),trnum,(otherFlag2&0x1?"Lost Sync":""));
    m_NumberDisplay->SetLabel(tmp);
}

void TriggerInfo::SetL3Number(unsigned char l3cnt){
  m_L3NumberDisplay->SetLabel((double)l3cnt);
}

void TriggerInfo::SetTriggerTimeNS(unsigned long ns,unsigned long c3po){
  double trigtime;
  if(c3po>1e6)
    trigtime=(double)ns/(double)c3po;
  else
    trigtime=(double)ns/133e6;
  wxString tmp;
  if (trigtime > 1.) 
    tmp.Printf(_T("  n/a")) ;
  else 
    tmp.Printf(_T("0.%03d,%03d,%03d"),(unsigned int)(trigtime*1e3),
  	     ((unsigned int)(trigtime*1e6)%1000),((unsigned int)(trigtime*1e9)%1000));
  //tmp.Printf("%g",trigtime);
  m_TimeNSDisplay->SetLabel(tmp);
}

void TriggerInfo::SetPPS(unsigned short pps){
  m_PPSDisplay->SetLabel((double)pps);
}
/*
void TriggerInfo::SetDeadtime(double deadtime){
  m_DeadtimeDisplay->SetLabel(deadtime);
}
*/
//void TriggerInfo::SetC3PO(unsigned long c3po ){
//   m_C3PODisplay->SetLabel((double)c3po);
//}

// void TriggerInfo::SetInterval(unsigned long dt ){
//   m_IntervalDisplay->SetLabel((double)dt);
// }

void TriggerInfo::SetCalibStatus(unsigned short calib){
// peng cao: TODO
// commented out because RFSWITCH_MASK and RFSWITCH_SHIFT and ATTEN_MASK and ATTEN_SHIFT are no longer  there in the new flightsoft
//  wxString tmp;
//  int tempBic=((calib&RFSWITCH_MASK)>>RFSWITCH_SHIFT);
//  int bic=0;
//  switch(tempBic) {
//      case 1:
//	  bic=4;
//	  break;
//      case 2:
//	  bic=3;
//	  break;
//      case 4:
//	  bic=2;
//	  break;
//      case 8:
//	  bic=1;
//	  break;
//      default:
//	  bic=0;
//	  break;
//  }
//  int tempAttn=((calib&ATTEN_MASK)>>ATTEN_SHIFT);
//  tmp << (calib&CAL_PULSER_MASK?_T("on"):_T("off")) 
//      << _T(" ") << bic << _T(" attn: ") << tempAttn;
//  m_CalibDisplay->SetLabel(tmp);
}

// void TriggerInfo::SetGlobalThreshold(unsigned short thresh ){
//   wxString tmp;
//   tmp << thresh;
//   m_ThresholdDisplay->SetLabel(tmp);
// }

void TriggerInfo::SetSurfMask(unsigned short mask[] ){
  wxString tmp=_T("") ;
  for(int i=N_SURF-1;i>=0;--i) tmp.Printf(_T("%s %01x"),tmp.c_str(),mask[i]&0xf);
    // tmp << (mask[i]&0xf) << " " ;
  m_SurfMaskDisplay->SetLabel(tmp);
}

void TriggerInfo::SetTurfMonitor(unsigned char word){
  wxString tmp;
  for(int i=7;i>=0;--i) tmp << ((word&(0x1<<i))?1:0);
  m_TurfMonitorDisplay->SetLabel(tmp);
}






