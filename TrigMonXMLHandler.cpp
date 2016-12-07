/* 
 * TrigMonXMLHandler.cpp
 *
 * Class that parses user controled options file for TrigMon
 */


#include <wx/log.h>
#include "trigmon_options.h"
#include "TrigMonXMLHandler.h"

TrigMonXMLHandler::TrigMonXMLHandler(){
  Reset();
}

TrigMonXMLHandler::~TrigMonXMLHandler(){}

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

void TrigMonXMLHandler::xmlTrigMonStart(void *data, const char *el, 
					const char **attr){
  using namespace TrigMonOpts;
  TrigMonXMLHandler *xml_handler=reinterpret_cast<TrigMonXMLHandler*>(data);
  long n;
  xmlTagValuePair **scale;
  scale = new xmlTagValuePair*[2];
  scale[0]= new xmlTagValuePair(_T("scale"));
  scale[1]=NULL;

  // Parse elements
  wxString element(_U(el));
  if(element.Lower()==_T("l1")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_L1){
      wxLogError(_T("[xml_start_hndl] Too many L1 elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    L1Scale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(L1Spec,L1Range,L1Labels,L1ScaleType,L1Average);
  }else if(element.Lower()==_T("l2")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_L2){
      wxLogError(_T("[xml_start_hndl] Too many L2 elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    L2Scale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(L2Spec,L2Range,L2Labels,L2ScaleType,L2Average);
  }else if(element.Lower()==_T("l3")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_L3){
      wxLogError(_T("[xml_start_hndl] Too many L3 elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    L3Scale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(L3Spec,L3Range,L3Labels,L3ScaleType,L3Average);
// }else if(element.Lower()=="veto"){
//     xml_handler->ReadBlockHead(attr,&n);
//     if(n>N_VETO){
//       wxLogError("[xml_start_hndl] Too many veto elements (%ld) declared in options file",n);
//       xml_handler->error=true;
//     }
//     // Mark that we are now parsing temperature block
//    xml_handler->SetBarItemBlock(VetoSpec,VetoRange,VetoLabels);
  }else if(element.Lower()==_T("rfpower")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_RFPOWER){
      wxLogError(_T("[xml_start_hndl] Too many RF power elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    RFPowerScale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(RFPowerSpec,RFPowerRange,RFPowerLabels,RFPowerScaleType,RFPowerAverage);
  }else if(element.Lower()==_T("scaler")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_SCALER){
      wxLogError(_T("[xml_start_hndl] Too many scaler elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    ScalerScale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(ScalerSpec,ScalerRange,ScalerLabels,ScalerScaleType,ScalerAverage);

  }else if(element.Lower()==_T("threshold")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_THRESHOLD){
      wxLogError(_T("[xml_start_hndl] Too many threshold elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    ThresholdScale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(ThresholdSpec,ThresholdRange,ThresholdLabels,ThresholdScaleType,ThresholdAverage);
  }else if(element.Lower()==_T("db")){
    // Prepare extra tags for db line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[6];
    extra[0]= new xmlTagValuePair(_T("host"));
    extra[1]= new xmlTagValuePair(_T("user"));
    extra[2]= new xmlTagValuePair(_T("name"));
    extra[3]= new xmlTagValuePair(_T("sql"));
    extra[4]= new xmlTagValuePair(_T("crc"));
    extra[5]=NULL;
    xml_handler->ReadBlockHead(attr,NULL,extra);
    // Extract extra values
    dbHost = extra[0]->value;
    dbUser = extra[1]->value;
    dbName = extra[2]->value;
    if(extra[3]->value.Lower()==_T("on")) sqlDump=true;
    else if(extra[3]->value.Lower()==_T("off")) sqlDump=false;
    else sqlDump=false;
    if(extra[4]->value.Lower()==_T("on")) crcCheck=true;
    else if(extra[4]->value.Lower()==_T("off")) crcCheck=false;
    else crcCheck=true;
    // Clean up
    for(int i=0;i<5;++i) delete extra[i];  
    delete[] extra;
  }else if(element.Lower()==_T("baritem")){
    xml_handler->ReadBarItem(attr);
    xml_handler->StartRangeSpec();
  }else if(element.Lower()==_T("range")){
    xml_handler->ReadRangeSpec(attr);
  }else if(element.Lower()==_T("surf")){
    // Prepare extra tags for surf line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[4];
    extra[0] = new xmlTagValuePair(_T("lock"));
    extra[1] = new xmlTagValuePair(_T("dynamic"));
    extra[2] = new xmlTagValuePair(_T("rf"));
    extra[3] = NULL;
    xml_handler->ReadBlockHead(attr,NULL,extra);
    // Extract extra values
    try{
      if(extra[0]->value.Lower()==_T("on")) Lockstep=true;
      else if(extra[0]->value.Lower()==_T("off")) Lockstep=false;
      else throw *(extra[0]);

      if(extra[1]->value.Lower()==_T("on")) DynamicTarget=true;
      else if(extra[1]->value.Lower()==_T("off")) DynamicTarget=false;
      else throw *(extra[1]);

      if(extra[2]->value.Lower()==_T("adc")) rfPowerType=ADC;
      else if(extra[2]->value.Lower()==_T("dbm")) rfPowerType=dBm;
      else if(extra[2]->value.Lower()==_T("t")) rfPowerType=T;
      else throw *(extra[2]);
    }
    catch(xmlTagValuePair &e){
      wxLogError(_T("[xml_start_hndl] Unknown value '%s' for tag '%s'"),e.value.c_str(),e.tag.c_str());
      xml_handler->error=true;
    }
    // Clean up
    for(int i=0;i<2;++i) delete extra[i];  
    delete[] extra;
  }else if(element.Lower()==_T("trigmon") || element.Lower()==_T("turf")){
    // We ignore these
  }else{
    wxLogError(_T("[xml_start_hndl] Unknown element '%s'"),element.c_str());
    xml_handler->error=true;
  }

  return;
}  

void TrigMonXMLHandler::xmlTrigMonEnd(void *data, const char *el) {
  using namespace TrigMonOpts;
  TrigMonXMLHandler *xml_handler=reinterpret_cast<TrigMonXMLHandler*>(data);

  // Parse elements
  wxString element(_U(el));
  if(element.Lower()==_T("l1") ||
     element.Lower()==_T("l2") ||
     element.Lower()==_T("l3") ||
     //     element.Lower()==_T("veto") ||
     element.Lower()==_T("rfpower") ||
     element.Lower()==_T("scaler") ||
     element.Lower()==_T("threshold")){
     xml_handler->ResetBarItemBlock();
  }else if(element.Lower()==_T("baritem")){
    // Terminate last range spec and increment counter
    xml_handler->TerminateRangeSpec();
    xml_handler->IncrementBarItemCount();
  }else if(element.Lower()==_T("range")){
    // Increment spec pointer
     xml_handler->IncrementRangeSpec();
  }else if(element.Lower()==_T("trigmon") || element.Lower()==_T("turf") ||
	   element.Lower()==_T("surf") || element.Lower()==_T("db")){
    // We ignore these){
  }else{
    wxLogError(_T("[xml_start_hndl] Unknown end element '%s' end"),element.c_str());
    xml_handler->error=true;
  }

  return;
}  
