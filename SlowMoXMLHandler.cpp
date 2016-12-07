/* 
 * SlowMoXMLHandler.cpp
 *
 * Class that handles parsing of user controled options for SlowMo program.
 */


#include <wx/log.h>
#include "slowmo_options.h"
#include "SlowMoXMLHandler.h"

SlowMoXMLHandler::SlowMoXMLHandler(){
  Reset();
}

SlowMoXMLHandler::~SlowMoXMLHandler(){}

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

void SlowMoXMLHandler::xmlSlowMoStart(void *data, const char *el, 
					const char **attr){
  using namespace SlowMoOpts;
  SlowMoXMLHandler *xml_handler=reinterpret_cast<SlowMoXMLHandler*>(data);
  long n;
  xmlTagValuePair **scale;
  scale = new xmlTagValuePair*[2];
  scale[0]= new xmlTagValuePair(_T("scale"));
  scale[1]=NULL;

  // Parse elements
  wxString element(_U(el));
  if(element.Lower()==_T("rate")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>2){
      wxLogError(_T("[xml_start_hndl] Too many event rate elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    EventRateScale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(EventRateSpec,EventRateRange,EventRateLabels,EventRateScaleType,EventRateAverage);
  }else if(element.Lower()==_T("temp")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_TEMP){
      wxLogError(_T("[xml_start_hndl] Too many temperature elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    TempScale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(TempSpec,TempRange,TempLabels,TempScaleType,TempAverage);
  }else if(element.Lower()==_T("power")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_POWER){
      wxLogError(_T("[xml_start_hndl] Too many power elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    PowerScale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(PowerSpec,PowerRange,PowerLabels,PowerScaleType,PowerAverage);
  }else if(element.Lower()==_T("l1")){
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
  }else if(element.Lower()==_T("scalerrms")){
    xml_handler->ReadBlockHead(attr,&n,scale);
    if(n>N_SCALERRMS){
      wxLogError(_T("[xml_start_hndl] Too many scaler RMS elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    ScalerRMSScale=scale[0]->value;
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(ScalerRMSSpec,ScalerRMSRange,ScalerRMSLabels,ScalerRMSScaleType,ScalerRMSAverage);
  }else if(element.Lower()==_T("db")){
    // Prepare extra tags for db line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[5];
    extra[0]= new xmlTagValuePair(_T("host"));
    extra[1]= new xmlTagValuePair(_T("user"));
    extra[2]= new xmlTagValuePair(_T("name"));
    extra[3]= new xmlTagValuePair(_T("sql"));
    extra[4]=NULL;
    xml_handler->ReadBlockHead(attr,NULL,extra);
    // Extract extra values
    dbHost = extra[0]->value;
    dbUser = extra[1]->value;
    dbName = extra[2]->value;
    if(extra[3]->value.Lower()==_T("on")) sqlDump=true;
    else if(extra[3]->value.Lower()==_T("off")) sqlDump=false;
    else sqlDump=false;
    // Clean up
    for(int i=0;i<4;++i) delete extra[i];  
    delete[] extra;
  }else if(element.Lower()==_T("baritem")){
    xml_handler->ReadBarItem(attr);
    xml_handler->StartRangeSpec();
  }else if(element.Lower()==_T("range")){
    xml_handler->ReadRangeSpec(attr);
  }else if(element.Lower()==_T("slowmo")){
    // Prepare extra tags for surf line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[3];
    extra[0] = new xmlTagValuePair(_T("delay"));
    extra[1] = new xmlTagValuePair(_T("rf"));
    extra[2] = NULL;
    xml_handler->ReadBlockHead(attr,NULL,extra);
    // Extract extra values
    try{
      long n;
      if(extra[0]->value.ToLong(&n)) readDelay=n;
      else throw *(extra[0]);
      
      if(extra[1]->value.Lower()==_T("adc")) rfPowerType=ADC;
      else if(extra[1]->value.Lower()==_T("dbm")) rfPowerType=dBm;
      else if(extra[1]->value.Lower()==_T("t")) rfPowerType=T;
      else throw *(extra[1]);

    }
    catch(xmlTagValuePair &e){
      wxLogError(_T("[xml_start_hndl] Unknown value '%s' for tag '%s'"),e.value.c_str(),e.tag.c_str());
      xml_handler->error=true;
    }
    // Clean up
    delete extra[0];  
    delete extra[1];  
    delete[] extra;
  }else{
    wxLogError(_T("[xml_start_hndl] Unknown element '%s'"),element.c_str());
    xml_handler->error=true;
  }

  return;
}  

void SlowMoXMLHandler::xmlSlowMoEnd(void *data, const char *el) {
  using namespace SlowMoOpts;
  SlowMoXMLHandler *xml_handler=reinterpret_cast<SlowMoXMLHandler*>(data);

  // Parse elements
  wxString element(_U(el));
  if(element.Lower()==_T("rate") ||
     element.Lower()==_T("temp") ||
     element.Lower()==_T("power") ||
     element.Lower()==_T("l1") ||
     element.Lower()==_T("l2") ||
     element.Lower()==_T("l3") ||
     element.Lower()==_T("rfpower") ||
     element.Lower()==_T("scaler") ||
     element.Lower()==_T("scalerrms")){
     xml_handler->ResetBarItemBlock();
  }else if(element.Lower()==_T("baritem")){
    // Terminate last range spec and increment counter
    xml_handler->TerminateRangeSpec();
    xml_handler->IncrementBarItemCount();
  }else if(element.Lower()==_T("range")){
    // Increment spec pointer
     xml_handler->IncrementRangeSpec();
  }else if(element.Lower()==_T("slowmo") || element.Lower()==_T("db")){
    // We ignore these){
  }else{
    wxLogError(_T("[xml_start_hndl] Unknown end element '%s' end"),element.c_str());
    xml_handler->error=true;
  }

  return;
}  
