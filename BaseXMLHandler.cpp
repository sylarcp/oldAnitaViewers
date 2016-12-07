/* 
 * BaseXMLHandler.cpp
 * 
 * Definitions for parsing of user options files which are not program 
 * specific.
 */


#include <wx/log.h>
#include "BaseXMLHandler.h"

BaseXMLHandler::BaseXMLHandler(){
  Reset();
}

BaseXMLHandler::~BaseXMLHandler(){}

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

void BaseXMLHandler::Reset(){
  m_spec=NULL;
  m_specList=NULL;
  m_rangeList=NULL;
  m_labelList=NULL;
  error=false;
}

void BaseXMLHandler::SetError(){error=true;}
void BaseXMLHandler::StartRangeSpec(){m_spec=*m_specList;}
void BaseXMLHandler::TerminateRangeSpec(){
  if(m_spec) *m_spec=wxRangeSpec();
  else{
    wxLogError(_T("[BaseXMLHandler::TerminateRangeSpec] Unexpected request to terminate RangeSpec list'"));
    error=true;
  }
  m_spec=NULL;
}

void BaseXMLHandler::SetBarItemBlock(wxRangeSpec **specList,double **range,
				     wxString *label,ScaleType *type,
				     unsigned int *avg){
  m_specList=specList;
  m_rangeList=range;
  m_labelList=label;
  m_scaleTypeList=type;
  m_avgList=avg;
}

void BaseXMLHandler::SetTextItemBlock(wxRangeSpec **specList,
				      wxString *label){
  m_specList=specList;
  m_labelList=label;
}

void BaseXMLHandler::IncrementBarItemCount(){
  ++m_specList;
  ++m_rangeList;
  ++m_labelList;
  ++m_scaleTypeList;
  ++m_avgList;
}
			   
void BaseXMLHandler::IncrementTextItemCount(){
  ++m_specList;
  ++m_labelList;
}

void BaseXMLHandler::ResetBarItemBlock(){
  m_specList=NULL;
  m_rangeList=NULL;
  m_labelList=NULL;
  m_scaleTypeList=NULL;
}

void BaseXMLHandler::ResetTextItemBlock(){
  m_specList=NULL;
  m_labelList=NULL;
}

void BaseXMLHandler::WriteHead(wxFile &file, const wxString &tag){
  wxString tmp=_T("<?xml version=\"1.0\" ?>\n");
  file.Write(tmp);
  OpenTag(file,tag);
}

void BaseXMLHandler::OpenTag(wxFile &file, const wxString &tag,
			      xmlTagValuePair **extra,bool close){
  wxString tmp;
  tmp << '<' << tag << ' ';
  while(extra && *extra){
    tmp << (*extra)->tag << _T("=\"") << (*extra)->value << _T("\" ");
    ++extra;
  }
  if(close) tmp << '/';
  tmp << _T(">\n");
  file.Write(tmp);
}

void BaseXMLHandler::CloseTag(wxFile &file, const wxString &tag){
  wxString tmp;
  tmp << _T("</") << tag << _T(">\n");
  file.Write(tmp);
}

void BaseXMLHandler::WriteTextItemBlock(wxFile &file,const wxString &tag,
					 int nItem, wxString *labels,
					 wxRangeSpec **specs,
					 xmlTagValuePair **extra){
  if(tag.Length()) WriteBlockHead(file,tag,nItem,extra);
  for(int i=0;i<nItem;++i) WriteTextItem(file,labels[i],specs[i]);
  if(tag.Length()) WriteBlockTail(file,tag);
}

void BaseXMLHandler::WriteBarItemBlock(wxFile &file,const wxString &tag,
				       int nItem, wxString *labels,
				       wxRangeSpec **specs,double **ranges,
				       ScaleType *type,unsigned int *avgs,
				       xmlTagValuePair **extra){
  if(tag.Length()) WriteBlockHead(file,tag,nItem,extra);
  for(int i=0;i<nItem;++i) WriteBarItem(file,labels[i],specs[i],ranges[i],type[i],avgs[i]);
  if(tag.Length()) WriteBlockTail(file,tag);
}

void BaseXMLHandler::WriteBlockHead(wxFile &file,const wxString &tag,int n,
				     xmlTagValuePair **extra){
  wxString tmp;
  tmp.Printf(_T("<%s "),tag.c_str());
  while(extra && *extra){
    tmp << (*extra)->tag << _T("=\"") << (*extra)->value << _T("\" ");
    ++extra;
  }
  tmp << _T("length=\"") << n << _T("\">\n");
  file.Write(tmp);
}

void BaseXMLHandler::WriteBlockTail(wxFile &file,const wxString &tag){
  wxString tmp;
  tmp.Printf(_T("</%s>\n"),tag.c_str());
  file.Write(tmp);
}

void BaseXMLHandler::WriteTextItem(wxFile &file,const wxString &label,
				    wxRangeSpec *spec){
  wxString tmp;
  tmp.Printf(_T("\t<textitem label=\"%s\">\n"),label.c_str());
  file.Write(tmp);
  for(int j=0;spec[j].colour.Ok();++j) WriteRangeSpec(file,&spec[j]);
	     tmp.Printf(_T("\t</textitem>\n"));
  file.Write(tmp); 
}

void BaseXMLHandler::WriteBarItem(wxFile &file,const wxString &label,
				  wxRangeSpec *spec,double *range,
				  const ScaleType &type,const unsigned int avg){

  wxString tmp;
  tmp.Printf(_T("\t<baritem label=\"%s\" bottom=\"%5g\" top=\"%5g\" scale=\"%s\" avg=\"%u\">\n"),
	     label.c_str(),range[0],range[1],type==Linear?"lin":"log",avg);
  file.Write(tmp);
  for(int j=0;spec[j].colour.Ok();++j) WriteRangeSpec(file,&spec[j]);
  tmp.Printf(_T("\t</baritem>\n"));
  file.Write(tmp);
}

void BaseXMLHandler::WriteRangeSpec(wxFile &file,wxRangeSpec *spec){
  wxString tmp;
  tmp.Printf(_T("\t\t<range start=\"%5g\" end=\"%5g\" colour=\"#%02X%02X%02X\" />\n"),
	     spec->low,spec->high,
	     spec->colour.Red(),
	     spec->colour.Green(),
	     spec->colour.Blue());
  file.Write(tmp);
}

void BaseXMLHandler::ReadBlockHead(const char **attr,long *n,
				    xmlTagValuePair **extra){
  int i=0;
  while(attr[i]){
    wxString tag=_U(attr[i++]);
    wxString value=_U(attr[i++]);
    if(tag.Lower()==_T("length")){
      if(n && !value.ToLong(n)){
	wxLogError(_T("[BaseXMLHandler::ReadBlockHead] Unable to extract integer from value '%s'"),value.c_str());
	error=true;
      }
    }else{
      bool found=false;
      while(extra && *extra){
	if(tag.Lower()==(*extra)->tag){
	  (*extra)->value=value;
	  found=true;
	  break;
	}
	++extra;
      }
      if(!found){
	wxLogError(_T("[BaseXMLHandler::ReadBlockHead] Unknown tag '%s' with value '%s'"),tag.c_str(),value.c_str());
	error=true;
      }
    }
  }
}

void BaseXMLHandler::ReadTextItem(const char **attr){
  int i=0;
  while(attr[i]){
    wxString tag=_U(attr[i++]);
    wxString value=_U(attr[i++]);
    if(tag.Lower()==_T("label")){
      if(m_labelList) *m_labelList=value;
      else{
	wxLogError(_T("[BaseXMLHandler::ReadTextItem] Unexpected occurence of <textitem> with label '%s'."),value.c_str());
	error=true;
      }
    }else{
      wxLogError(_T("[BaseXMLHandler::ReadTextItem] Unknown tag '%s' with value '%s'"),tag.c_str(),value.c_str());
      error=true;
    }
  }  
}

void BaseXMLHandler::ReadBarItem(const char **attr){
  int i=0;
  while(attr[i]){
    wxString tag=_U(attr[i++]);
    wxString value=_U(attr[i++]);
    if(tag.Lower()==_T("label")){
      if(m_labelList) *m_labelList=value;
      else{
	wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unexpected occurence of <baritem> with label '%s'."),value.c_str());
	error=true;
      }
    }else if(tag.Lower()==_T("bottom")){
      if(m_rangeList){
	if(!value.ToDouble(&(*m_rangeList)[0])){
	  wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unable to extract double from value '%s'"),value.c_str());
	error=true;
	}
      }else{
	wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unexpected occurence of <baritem> with bottom value '%s'."),value.c_str());
	error=true;
      }
    }else if(tag.Lower()==_T("top")){
      if(m_rangeList){
	if(!value.ToDouble(&(*m_rangeList)[1])){
	  wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unable to extract double from value '%s'"),value.c_str());
	  error=true;
	}
      }else{
	wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unexpected occurence of <baritem> with top value '%s'."),value.c_str());
	error=true;
      }
    }else if(tag.Lower()==_T("scale")){
      if(m_scaleTypeList){
	if(value.Lower()==_T("lin")) (*m_scaleTypeList)=Linear;
	else if(value.Lower()==_T("log")) (*m_scaleTypeList)=Log;
	else{
	  wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unknown scaling value '%s'"),value.c_str());
	  error=true;
	}
      }else{
	wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unexpected occurence of <baritem> with scale value '%s'."),value.c_str());
	error=true;
      }
    }else if(tag.Lower()==_T("avg")){
      if(m_avgList){
	long avg;
	if(!value.ToLong(&avg)){
	  wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unable to extract long from value '%s'"),value.c_str());
	  error=true;
	}else
	  (*m_avgList)=avg;
      }else{
	wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unexpected occurence of <baritem> with top value '%s'."),value.c_str());
	error=true;
      }
    }else{
      wxLogError(_T("[BaseXMLHandler::ReadBarItem] Unknown tag '%s' with value '%s'"),tag.c_str(),value.c_str());
      error=true;
    }
  }
}

void BaseXMLHandler::ReadRangeSpec(const char **attr){
  double low,high;
  unsigned long red,green,blue;
  int i=0;
  while(attr[i]){
    wxString tag=_U(attr[i++]);
    wxString value=_U(attr[i++]);
    if(tag.Lower()==_T("start")){
      if(!value.ToDouble(&low)){
	wxLogError(_T("[BaseXMLHandler::ReadRangeSpec] Unable to extract double from value '%s'"),value.c_str());
	error=true;
      }
    }else if(tag.Lower()==_T("end")){
      if(!value.ToDouble(&high)){
	wxLogError(_T("[BaseXMLHandler::ReadRangeSpec] Unable to extract double from value '%s'"),value.c_str());
	error=true;
      }
    }else if(tag.Lower()==_T("colour")){
      if(!value.Mid(1,2).ToULong(&red,16)){
	wxLogError(_T("[BaseXMLHandler::ReadRangeSpec] Unable to extract red colour intensity from value '%s'"),value.c_str());
	error=true;
      }
      if(!value.Mid(3,2).ToULong(&green,16)){
	wxLogError(_T("[BaseXMLHandler::ReadRangeSpec] Unable to extract green colour intensity from value '%s'"),value.c_str());
	error=true;
      }
      if(!value.Mid(5,2).ToULong(&blue,16)){
	wxLogError(_T("[BaseXMLHandler::ReadRangeSpec] Unable to extract blue colour intensity from value '%s'"),value.c_str());
	error=true;
      }
    }else{
      wxLogError(_T("[BaseXMLHandler::ReadRangeSpec] Unknown tag '%s' with value '%s'"),tag.c_str(),value.c_str());
      error=true;
    }
  }
  if(m_spec) *m_spec=wxRangeSpec(low,high,wxColour(red,green,blue));
  else{
    wxLogError(_T("[BaseXMLHandler::ReadRangeSpec] Unexpected occurence of '<range>'"));
    error=true;
  }
}

