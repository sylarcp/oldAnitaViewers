/*
 * AviewXMLHandler.cpp
 * 
 * This objects parses AnitaViewer user defined options file. 
 */

#include <wx/log.h>
#include "options.h"
#include "AviewXMLHandler.h"

AviewXMLHandler::AviewXMLHandler(){
  Reset();
}

AviewXMLHandler::~AviewXMLHandler(){}

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

void AviewXMLHandler::xmlAviewStart(void *data, const char *el, const char **attr){
  using namespace AviewOpts;
  AviewXMLHandler *xml_handler=reinterpret_cast<AviewXMLHandler*>(data);
  long n;

  // Parse elements
  wxString element(_U(el));
  if(element.Lower()==_T("temp")){
    xml_handler->ReadBlockHead(attr,&n);
    if(n>N_TEMP){
      wxLogError(_T("[xml_start_hndl] Too many temperature elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Mark that we are now parsing temperature block
    xml_handler->SetBarItemBlock(TempSpec,TempRange,TempLabels,TempScaleType,TempAverage);
  }else if(element.Lower()==_T("disk")){
    xml_handler->ReadBlockHead(attr,&n);
    if(n>N_DISK){
      wxLogError(_T("[xml_start_hndl] Too many disk elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Mark that we are now parsing disk block
    xml_handler->SetBarItemBlock(DiskSpec,DiskRange,DiskLabels,DiskScaleType,DiskAverage);
  }else if(element.Lower()==_T("queue")){
    xml_handler->ReadBlockHead(attr,&n);
    if(n>N_QUEUE){
      wxLogError(_T("[xml_start_hndl] Too many queue elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Mark that we are now parsing queue block
    xml_handler->SetBarItemBlock(QueueSpec,QueueRange,QueueLabels,QueueScaleType,QueueAverage);
  }else if(element.Lower()==_T("current")){
    xml_handler->ReadBlockHead(attr,&n);
    if(n>N_CURRENT){
      wxLogError(_T("[xml_start_hndl] Too many current elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Mark that we are now parsing current block
    xml_handler->SetTextItemBlock(CurrentSpec,CurrentLabels);
  }else if(element.Lower()==_T("voltage")){
    xml_handler->ReadBlockHead(attr,&n);
    if(n>N_VOLTAGE){
      wxLogError(_T("[xml_start_hndl] Too many voltage elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Mark that we are now parsing voltage block
    xml_handler->SetTextItemBlock(VoltageSpec,VoltageLabels);
  }else if(element.Lower()==_T("power")){
    xml_handler->ReadBlockHead(attr,&n);
    if(n>N_POWER){
      wxLogError(_T("[xml_start_hndl] Too many power elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Mark that we are now parsing Power block
    xml_handler->SetTextItemBlock(PowerSpec,PowerLabels);
  }else if(element.Lower()==_T("rf")){
    // Prepare extra tags for rf line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[16];
    extra[0]= new xmlTagValuePair(_T("overview"));
    extra[1]= new xmlTagValuePair(_T("style"));
    extra[2]= new xmlTagValuePair(_T("reference"));
    extra[3]=new xmlTagValuePair(_T("voltage"));
    extra[4]=new xmlTagValuePair(_T("fft"));
    extra[5]=new xmlTagValuePair(_T("power"));
    extra[6]=new xmlTagValuePair(_T("clockrf"));
    extra[7]=new xmlTagValuePair(_T("tab"));
    extra[8]=new xmlTagValuePair(_T("tabscale"));
    extra[9]=new xmlTagValuePair(_T("clkscale"));
    extra[10]=new xmlTagValuePair(_T("delay"));
    extra[11]=new xmlTagValuePair(_T("phimaskcolour_v"));
    extra[12]=new xmlTagValuePair(_T("phimaskcolour_h"));
    extra[13]=new xmlTagValuePair(_T("l3colour_v"));
    extra[14]=new xmlTagValuePair(_T("l3colour_h"));
    extra[15]=new xmlTagValuePair(_T("compass"));
    extra[16]=NULL;
    xml_handler->ReadBlockHead(attr,&n,extra);
    if(n>N_RFANT){
      wxLogError(_T("[xml_start_hndl] Too many RF elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Extract extra values
    try{
      long n2;
      unsigned long red,green,blue;
      if(extra[0]->value.Lower()==_T("surf")) RFOverview=SurfView;
      else if(extra[0]->value.Lower()==_T("phi")) RFOverview=PhiView;
      else if(extra[0]->value.Lower()==_T("newphi")) RFOverview=newPhiView;
      else if(extra[0]->value.Lower()==_T("payload")) RFOverview=PayloadView;
      else throw *(extra[0]);

      if(extra[1]->value.Lower()==_T("mesh")) PayloadStyle=Mesh;
      else if(extra[1]->value.Lower()==_T("solid")) PayloadStyle=Solid;
      else throw *(extra[1]);

      if(extra[2]->value.Lower()==_T("rms")) RFReference=RMS;
      else if(extra[2]->value.Lower()==_T("absolute")) RFReference=Absolute;
      else throw *(extra[2]);

      if(extra[3]->value.Lower()==_T("on")) RFView=static_cast<RFViewType>((int)RFView|VoltageView);
      else if(extra[3]->value.Lower()==_T("off")) RFView=static_cast<RFViewType>((int)RFView&(~VoltageView));
      else throw *(extra[3]);

      if(extra[4]->value.Lower()==_T("on")) RFView=static_cast<RFViewType>((int)RFView|FFTView);
      else if(extra[4]->value.Lower()==_T("off")) RFView=static_cast<RFViewType>((int)RFView&(~FFTView));
      else throw *(extra[4]);

      if(extra[5]->value.Lower()==_T("on")) RFView=static_cast<RFViewType>((int)RFView|PowerView);
      else if(extra[5]->value.Lower()==_T("off")) RFView=static_cast<RFViewType>((int)RFView&(~PowerView));
      else throw *(extra[5]);
      
      if(extra[6]->value.Lower()==_T("on")) ClockRF=true;
      else if(extra[6]->value.Lower()==_T("off")) ClockRF=false;
      else throw *(extra[6]);
      
      if(extra[7]->value.Lower()==_T("voltage")) RFTab=VoltageTab;
      else if(extra[7]->value.Lower()==_T("fft")) RFTab=FFTTab;
      else if(extra[7]->value.Lower()==_T("power")) RFTab=PowerTab;
      else throw *(extra[7]);      
      
      if(extra[8]->value.BeforeFirst(':').ToLong(&n) &&
	 extra[8]->value.AfterFirst(':').ToLong(&n2)){
	tabScaleMin=n;
	tabScaleMax=n2;
      }else throw *(extra[8]);

      if(extra[9]->value.BeforeFirst(':').ToLong(&n) &&
	 extra[9]->value.AfterFirst(':').ToLong(&n2)){
	clockTabScaleMin=n;
	clockTabScaleMax=n2;
      }else throw *(extra[9]);

      if(extra[10]->value.ToLong(&n)) readDelay=n;
      else throw *(extra[10]);

      if(extra[11]->value.Mid(1,2).ToULong(&red,16) &&
	 extra[11]->value.Mid(3,2).ToULong(&green,16) && 
	 extra[11]->value.Mid(5,2).ToULong(&blue,16)) PhiMaskColour=wxColour(red,green,blue);
      else throw *(extra[11]);

      if(extra[12]->value.Mid(1,2).ToULong(&red,16) &&
	 extra[12]->value.Mid(3,2).ToULong(&green,16) && 
	 extra[12]->value.Mid(5,2).ToULong(&blue,16)) PhiHMaskColour=wxColour(red,green,blue);
      else throw *(extra[12]);

      if(extra[13]->value.Mid(1,2).ToULong(&red,16) &&
	 extra[13]->value.Mid(3,2).ToULong(&green,16) && 
	 extra[13]->value.Mid(5,2).ToULong(&blue,16)) L3TrigColour=wxColour(red,green,blue);
      else throw *(extra[13]);

      if(extra[14]->value.Mid(1,2).ToULong(&red,16) &&
	 extra[14]->value.Mid(3,2).ToULong(&green,16) && 
	 extra[14]->value.Mid(5,2).ToULong(&blue,16)) L3HTrigColour=wxColour(red,green,blue);
      else throw *(extra[13]);

      if(extra[15]->value.Lower()==_T("on")) Compass=true;
      else if(extra[15]->value.Lower()==_T("off")) Compass=false;
      else throw *(extra[15]);
      
    }
    catch(xmlTagValuePair &e){
      wxLogError(_T("[xml_start_hndl] Unknown value '%s' for tag '%s'"),e.value.c_str(),e.tag.c_str());
      xml_handler->error=true;
    }
    // Clean up
    for(int i=0;i<14;++i) delete extra[i];  
    delete[] extra;
    // Mark that we are now parsing RF block
    xml_handler->SetTextItemBlock(RFSpecRMS,RFLabels);
  }else if(element.Lower()==_T("rfabs")){
    xml_handler->ReadBlockHead(attr,&n);
    if(n>N_RFANT){
      wxLogError(_T("[xml_start_hndl] Too many RFabs elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Mark that we are now parsing voltage block
    xml_handler->SetTextItemBlock(RFSpecAbsolute,RFLabels);
  }else if(element.Lower()==_T("aux")){
    // Prepare extra tags for aux line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[2];
    extra[0]= new xmlTagValuePair(_T("gps"));
    extra[1]=NULL;
    xml_handler->ReadBlockHead(attr,&n,extra);
    if(n>N_AUX){
      wxLogError(_T("[xml_start_hndl] Too many auxillary elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Extract extra values
    try{
      if(extra[0]->value.Lower()==_T("adu5")) DefaultGPS=ADU5;
      else if(extra[0]->value.Lower()==_T("g12")) DefaultGPS=G12;
      else throw *(extra[0]);
    }
    catch(xmlTagValuePair &e){
      wxLogError(_T("[xml_start_hndl] Unknown value '%s' for tag '%s'"),e.value.c_str(),e.tag.c_str());
      xml_handler->error=true;
    }
    // Clean up
    delete extra[0];  
    delete[] extra;
    // Mark that we are now parsing Aux block
    xml_handler->SetTextItemBlock(AuxSpec,AuxLabels);
  }else if(element.Lower()==_T("rate")){
    // Prepare extra tags for aux line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[2];
    extra[0]= new xmlTagValuePair(_T("step"));
    extra[1]=NULL;
    xml_handler->ReadBlockHead(attr,&n,extra);
    if(n>1){
      wxLogError(_T("[xml_start_hndl] Too many event rate elements (%ld) declared in options file"),n);
      xml_handler->error=true;
    }
    // Extract extra values
    try{
      if(extra[0]->value.ToLong(&n)) EventRateStep=n;
      else throw *(extra[0]);
    }
    catch(xmlTagValuePair &e){
      wxLogError(_T("[xml_start_hndl] Unknown value '%s' for tag '%s'"),e.value.c_str(),e.tag.c_str());
      xml_handler->error=true;
    }
    // Clean up
    delete extra[0];  
    delete[] extra;
    // Mark that we are now parsing event rate block
    xml_handler->SetBarItemBlock(&EventRateSpec,&EventRateRange,&EventRateLabels,&EventRateScaleType,&EventRateAverage);
  }else if(element.Lower()==_T("db")){
    // Prepare extra tags for db line
    xmlTagValuePair **extra;
    extra = new xmlTagValuePair*[7];
    extra[0]= new xmlTagValuePair(_T("host"));
    extra[1]= new xmlTagValuePair(_T("user"));
    extra[2]= new xmlTagValuePair(_T("name"));
    extra[3]= new xmlTagValuePair(_T("sql"));
    extra[4]= new xmlTagValuePair(_T("crc"));
    extra[5]= new xmlTagValuePair(_T("run"));
    extra[6]=NULL;
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
    if(extra[5]->value.Lower()==_T("on")) fetchRunNumber=true;
    else if(extra[5]->value.Lower()==_T("off")) fetchRunNumber=false;
    else fetchRunNumber=true;
    // Clean up
    for(int i=0;i<6;++i) delete extra[i];  
    delete[] extra;
  }else if(element.Lower()==_T("baritem")){
    xml_handler->ReadBarItem(attr);
    xml_handler->StartRangeSpec();
  }else if(element.Lower()==_T("textitem")){
    xml_handler->ReadTextItem(attr);
    xml_handler->StartRangeSpec();
  }else if(element.Lower()==_T("range")){
    xml_handler->ReadRangeSpec(attr);
  }else if(element.Lower()==_T("aview")){
    // We ignore this one
  }else{
    wxLogError(_T("[xml_start_hndl] Unknown element '%s'"),element.c_str());
    xml_handler->error=true;
  }

  return;
}  

void AviewXMLHandler::xmlAviewEnd(void *data, const char *el) {
  using namespace AviewOpts;
  AviewXMLHandler *xml_handler=reinterpret_cast<AviewXMLHandler*>(data);

  // Parse elements
  wxString element(_U(el));
  if(element.Lower()==_T("temp") ||
     element.Lower()==_T("disk") ||
     element.Lower()==_T("queue") ||
     element.Lower()==_T("rate")){
    xml_handler->ResetBarItemBlock();
  }else if(element.Lower()==_T("current") || 
	   element.Lower()==_T("voltage") ||
	   element.Lower()==_T("power") ||
	   element.Lower()==_T("rf")|| 
	   element.Lower()==_T("rfabs")|| 
	   element.Lower()==_T("aux")){
    xml_handler->ResetTextItemBlock();
  }else if(element.Lower()==_T("baritem")){
    // Terminate last range spec and increment counter
    xml_handler->TerminateRangeSpec();
    xml_handler->IncrementBarItemCount();
  }else if(element.Lower()==_T("textitem")){
    // Terminate last range spec and increment to next item
    xml_handler->TerminateRangeSpec();
    xml_handler->IncrementTextItemCount();
  }else if(element.Lower()==_T("range")){
    // Increment spec pointer
    xml_handler->IncrementRangeSpec();
  }else if(element.Lower()==_T("aview") || 
	   element.Lower()==_T("db")){
    // We ignore these one
  }else{
    wxLogError(_T("[xml_end_hndl] Unknown end element '%s' end"),element.c_str());
    xml_handler->error=true;
  }

  return;
}  
