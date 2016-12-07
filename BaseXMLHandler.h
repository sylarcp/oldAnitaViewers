/* 
 * BaseXMLHandler.h
 */

#ifndef _BASEXMLHANDLERH_
#define _BASEXMLHANDLERH_

#include "range.h"
#include <wx/file.h>

struct xmlTagValuePair{
  xmlTagValuePair(const wxString &t=wxT(""),const wxString &v=wxT("")):tag(t),value(v){}
  wxString tag;
  wxString value;
};

class BaseXMLHandler{
 public:
  BaseXMLHandler();
  ~BaseXMLHandler();

  void Reset();

  void WriteHead(wxFile &file,const wxString &tag);
  void OpenTag(wxFile &file,const wxString &tag,xmlTagValuePair **extra=NULL,bool close=false);
  void CloseTag(wxFile &file,const wxString &tag);
  void WriteTextItemBlock(wxFile &file,const wxString &tag,int nItem, wxString *labels,wxRangeSpec **specs,xmlTagValuePair **extra=NULL);
  void WriteBarItemBlock(wxFile &file,const wxString &tag,int nItem, wxString *labels,wxRangeSpec **specs,double **ranges,ScaleType *type,unsigned int *avgs,xmlTagValuePair **extra=NULL);

  inline bool Ok(){return !error;}
  void SetError();
  void StartRangeSpec();
  void IncrementRangeSpec(){++m_spec;}
  void TerminateRangeSpec();

  void SetBarItemBlock(wxRangeSpec **specList,double **range,wxString *label,ScaleType *type,unsigned int *avgs);
  void SetTextItemBlock(wxRangeSpec **specList,wxString *label);

  void IncrementBarItemCount();
  void IncrementTextItemCount();

  void ResetBarItemBlock();
  void ResetTextItemBlock();

  void ReadBlockHead(const char **attr,long *n,xmlTagValuePair **extra=NULL);
  void ReadTextItem(const char **attr);
  void ReadBarItem(const char **attr);
  void ReadRangeSpec(const char **attr);

 protected:
  void WriteBlockHead(wxFile &file,const wxString &tag,int n,xmlTagValuePair **extra);
  void WriteBlockTail(wxFile &file,const wxString &tag);
  void WriteTextItem(wxFile &file,const wxString &label,wxRangeSpec *spec);
  void WriteBarItem(wxFile &file,const wxString &label,wxRangeSpec *spec,double *range,const ScaleType &type,const unsigned int avg);
  void WriteRangeSpec(wxFile &file,wxRangeSpec *spec);

  wxRangeSpec *m_spec;
  wxRangeSpec **m_specList;
  double **m_rangeList;
  wxString *m_labelList;
  ScaleType *m_scaleTypeList;
  unsigned int *m_avgList;
  bool error;
};


#endif //  _BASEXMLHANDLERH_

