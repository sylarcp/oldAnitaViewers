/* 
 * historydisplay.cpp
 *
 * Base class for all displays that keep history of past values displayed. 
 * It also provides definitions for hooks used to pull given variable data 
 * from SQL database.
 */


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "historydisplay.h"
#endif

#include "historydisplay.h"

wxHistoryDisplay::wxHistoryDisplay(int dim,const wxString &name,
			       const wxString *xUnits,
			       const wxString *yUnits,
			       const wxString *dbVar,
			       const wxString &dbTable,
			       unsigned int buffer)
  :wrap(false),m_dim(dim),m_nBuffer(buffer),m_iBuffer(0),m_name(name),
   m_xUnits(NULL),m_yUnits(NULL),m_dbVar(NULL),m_graph(NULL){

  m_buffer = new double*[m_dim];
  dataCurve=new wxGraphCurve*[m_dim];
  for(unsigned int i=0;i<m_dim;++i){
    dataCurve[i]=NULL;
    m_buffer[i]=new double[m_nBuffer];
    for(unsigned int j=0;j<m_nBuffer;++j) m_buffer[i][j]=0;
  }
  if(xUnits){
    m_xUnits = new wxString[m_dim];
    for(unsigned int i=0;i<m_dim;++i) m_xUnits[i] = xUnits[i];
  }
  if(yUnits){
    m_yUnits = new wxString[m_dim];
    for(unsigned int i=0;i<m_dim;++i) m_yUnits[i] = yUnits[i];
  }
  if(dbVar){
    m_dbVar = new wxString[m_dim];
    for(unsigned int i=0;i<m_dim;++i) m_dbVar[i] = dbVar[i];
    m_dbTable=dbTable;
  }

}
 
wxHistoryDisplay::~wxHistoryDisplay(){
  for(unsigned int i=0;i<m_dim;++i) delete[] m_buffer[i];
  delete[] m_buffer;
  for(unsigned int i=0;i<m_dim;++i) delete dataCurve[i];
  delete[] dataCurve;
  if(m_xUnits) delete[] m_xUnits;
  if(m_yUnits) delete[] m_yUnits;
  if(m_dbVar) delete[] m_dbVar;
}

void wxHistoryDisplay::SetValues(double *val){
  for(unsigned int i=0;i<m_dim;++i) m_buffer[i][m_iBuffer]=val[i];
  
  // Advance buffer pointer and update curves if visible
  if(++m_iBuffer==m_nBuffer){
    m_iBuffer=0;
    wrap=true;
  }
  if(m_graph) _NewCurve();
}

double wxHistoryDisplay::GetValue(unsigned int dim,unsigned int navg){
  double val=0;
  if(m_iBuffer<navg){
    if(wrap){
      for(unsigned int i=m_nBuffer+m_iBuffer-navg;i<m_nBuffer;++i) val+=m_buffer[dim][i];
      for(unsigned int i=0;i<m_iBuffer;++i) val+=m_buffer[dim][i];
    }else{
      for(unsigned int i=0;i<m_iBuffer;++i) val+=m_buffer[dim][i];
      navg=m_iBuffer;
    }
  }else
    for(unsigned int i=m_iBuffer-navg;i<m_iBuffer;++i) val+=m_buffer[dim][i];
  return val/navg;
}

void wxHistoryDisplay::_NewCurve(){
  for(unsigned int i=0;i<m_dim;++i){
    if(dataCurve[i]) delete dataCurve[i];
    double x[m_nBuffer];
    for(unsigned int j=0;j<m_nBuffer;++j) x[j]=j;
    double y[m_nBuffer];
    int iy=0;
    for(unsigned int j=m_iBuffer;j<m_nBuffer;++j) y[iy++]=m_buffer[i][j];
    for(unsigned int j=0;j<m_iBuffer;++j) y[iy++]=m_buffer[i][j];
    dataCurve[i] = new wxGraphCurve(x,y,m_nBuffer);
    m_graph->SetData(dataCurve[i],i);
  }
}
