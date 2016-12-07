/* 
 * codedstattext.cpp
 * 
 * Definitions for class which can display textual codes on numerical input;
 * e.g. if numerical trigger flag is '5', object from this class would 
 * display 'RFTrig', if '5' was defined to be 'RFTrig' on object creation time.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "codedstattext.h"
#endif

#include "codedstattext.h"

IMPLEMENT_DYNAMIC_CLASS(wxCodedStaticText, wxHistoryStaticText)

wxCodedStaticText:: wxCodedStaticText(wxWindow *parent,
				      wxWindowID id,
				      const wxString &codeMap,
				      const wxString &name,
				      const wxString &xUnits,
				      const wxString &yUnits,
				      const wxString &dbVar,
				      const wxString &dbTable,
				      unsigned int buffer)
  :wxHistoryStaticText(parent,id,_T("%d"),name,xUnits,yUnits,dbVar,dbTable,buffer),
   nCode(0),m_code(NULL),m_codeText(NULL){
  // Parse code map
  if(nCode=(codeMap.Freq('|')+1)){
    m_code = new long[nCode];
    m_codeText = new wxString[nCode];
    int c=0;
    wxString tmp=codeMap;
    while(tmp.Length()){
      wxString map=tmp.BeforeFirst('|');
      if(map.BeforeFirst('=').ToLong(&m_code[c])){
	m_codeText[c]=map.AfterFirst('=');
	++c;
      }
      tmp=tmp.AfterFirst('|');
    }
    if(c!=nCode){
      wxLogMessage(_T("Code parsing error: Expected %d, found %d codes."),nCode,c);
      nCode=c; // Adjust to actually extracted number of codes
    }
  }
}

wxCodedStaticText::~wxCodedStaticText(){
  if(m_code) delete[] m_code;
  if(m_codeText) delete[] m_codeText;
}

void wxCodedStaticText::SetLabel(int val){
  double dblval=val;
  SetValues(&dblval);
  
  wxString label=_T("");
  for(int i=0;i<nCode;++i){
    if(val&m_code[i]){ 
      label+=m_codeText[i];
      label+=_T(" ");
    }
  }
  if(!label.Length()) label.Printf(m_format.c_str(),val); // Default 

  wxStaticText::SetLabel(label);
}
