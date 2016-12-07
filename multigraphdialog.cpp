 /*
 * multigraphdialog.cpp
 *
 * Class that displays history of any given variable(s) and has ability to 
 * query SQL database on their values with parameters supplied by user.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "multigraphdialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/log.h>
#include <wx/checkbox.h>
#include <wx/valgen.h>
#include "multigraphdialog.h"

IMPLEMENT_CLASS( wxMultiGraphDialog, wxDialog )

DEFINE_EVENT_TYPE(wxNO_RESULTS)
BEGIN_EVENT_TABLE( wxMultiGraphDialog, wxDialog )
  EVT_COMMAND_RANGE(10000,10010,wxEVT_COMMAND_CHECKBOX_CLICKED, wxMultiGraphDialog::InsertGraph)
  EVT_BUTTON(REQUEST_BUTTON_ID,wxMultiGraphDialog::OnRequest)
  EVT_BUTTON(NOW_BUTTON_ID,wxMultiGraphDialog::OnNow)
  EVT_TEXT_ENTER(TIME_DISPLAY,wxMultiGraphDialog::OnRequest)
  EVT_TEXT_ENTER(LENGTH_DISPLAY,wxMultiGraphDialog::OnRequest)
END_EVENT_TABLE()

inline wxString _U(const char String[] = "")
{
    return wxString(String, wxConvUTF8);
}

/*
 * Constructor
 */
wxMultiGraphDialog::wxMultiGraphDialog( wxWindow* parent, wxWindowID id, 
				    const wxString &caption, int dim,
				    const wxString *xUnits,
				    const wxString *yUnits,
				    const wxString *dbVar,
				    const wxString &dbTable,
				    const wxPoint& pos, 
				    const wxSize& size, 
				    long style )
  :wxDialog(parent,id,caption,pos,size,style^(wxGRAPH_DEFAULT)),
   m_dim(dim),m_refTick(0),m_sqlTable(dbTable),m_caption(caption){
  
  // Allocate all arrays we need
  windowOn=new bool[m_dim];
  m_graphWindow=new wxGraphWindow*[m_dim];
  m_data=new wxGraphCurve*[m_dim];

  m_sqlVariable=new wxString[m_dim];
  m_sqlDataCurve=new wxGraphCurve*[m_dim];
  m_histBound=new double*[m_dim];
  m_histLength=new int[m_dim];

  wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *controlBox = new wxBoxSizer(wxHORIZONTAL);
  if(m_dim>1) sizer->Add(controlBox,0,wxEXPAND);

  for(int i=0;i<m_dim;++i){
    windowOn[i]=true;
    m_data[i]=NULL;
    m_sqlDataCurve[i]=NULL;
    if(dbVar) m_sqlVariable[i]=dbVar[i];
    if((m_histLength[i]=xUnits[i].Freq(':'))<2){ // Regular plot
      m_graphWindow[i]=new wxGraphWindow(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(),size.GetHeight()/m_dim), xUnits[i], yUnits[i], style&wxGRAPH_DEFAULT);
      m_histBound[i]=NULL;
    }else{  // Histogram plot
      m_graphWindow[i]=new wxGraphWindow(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(),size.GetHeight()/m_dim), yUnits[i], _T("N"), style&wxGRAPH_DEFAULT);
      // parse xUnits[i] to get histogram boundaries
      m_histBound[i]=new double[m_histLength[i]];
      int c=0;
      wxString tmp=xUnits[i];
      while(tmp.Length() && tmp.BeforeFirst(':').ToDouble(&m_histBound[i][c])){
	tmp=tmp.AfterFirst(':');
	++c;
      }
      if(c!=m_histLength[i]){
	wxLogMessage(_T("Histogram boundary parsing error: Expected %d, found %d boundaries."),m_histLength[i],c);
	m_histLength[i]=c; // Adjust to actually extracted number of boundaries
      }
    }
    sizer->Add(m_graphWindow[i],1,wxEXPAND|wxBOTTOM, 5);
    controlBox->Add(new wxCheckBox(this,10000+i,yUnits[i],wxDefaultPosition, wxDefaultSize,wxCHK_2STATE,wxGenericValidator(&windowOn[i])),0);
  }

  if(dbTable.Length()){
    wxBoxSizer *sqlSizer=new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *labelSizer=new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *infoSizer=new wxBoxSizer(wxVERTICAL);
    sqlSizer->Add(labelSizer,0,wxEXPAND);
    sqlSizer->Add(infoSizer,1,wxEXPAND);
    sizer->Add(sqlSizer,0,wxEXPAND);

    wxBoxSizer *buttonSizer=new wxBoxSizer(wxHORIZONTAL);
    wxButton *requestButton=new wxButton(this,REQUEST_BUTTON_ID,_("Request"));
    wxButton *nowButton=new wxButton(this,NOW_BUTTON_ID,_("UTC Now"));
    buttonSizer->Add(requestButton,0,wxRIGHT,5);
    buttonSizer->Add(nowButton,0,wxLEFT,5);
    sizer->Add(buttonSizer);

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
  }

  SetSizerAndFit(sizer);
  sizer->SetSizeHints(this);

  return;
}

/*
 * Destructor
 */
wxMultiGraphDialog::~wxMultiGraphDialog(){
  delete[] windowOn;
  delete[] m_sqlVariable;
  for(int i=0;i<m_dim;++i) delete m_sqlDataCurve[i];
  delete[] m_sqlDataCurve;
  for(int i=0;i<m_dim;++i) if(m_histBound[i]) delete[] m_histBound[i];
  delete[] m_histBound;
  delete[] m_histLength;
}

void wxMultiGraphDialog::InsertGraph(wxCommandEvent& event){
  if(Validate() && TransferDataFromWindow()){
    for(int i=0;i<m_dim;++i){
      if(windowOn[i]) GetSizer()->Show(m_graphWindow[i]);
      else GetSizer()->Hide(m_graphWindow[i]);
    }
    if(GetSizer()->GetSize().GetWidth()<GetSizer()->GetMinSize().GetWidth()
       || GetSizer()->GetSize().GetHeight()<GetSizer()->GetMinSize().GetHeight()){
      GetSizer()->Fit(this);
      GetSizer()->SetSizeHints(this);
    }
    GetSizer()->Layout();
  }
}

void wxMultiGraphDialog::OnNow(wxCommandEvent &WXUNUSED(event)){
  wxString now=wxDateTime::Now().ToUTC().Format(_T("%Y-%m-%d %H:%M:%S"));
  m_refTimeEntry->Clear();
  (*m_refTimeEntry) << now;
}

void wxMultiGraphDialog::OnRequest(wxCommandEvent &WXUNUSED(event)){
  m_refTick=m_refTimeEntry->GetTicks();

  long length;
  if(!m_lengthEntry->GetLineText(0).ToLong(&length)){
    wxMessageBox(_T("Unable to parse record length. Please use valid integer value."),_T("SQL Request Error"),wxOK|wxICON_ERROR);
    return;
  }

  wxString cmd;
  unsigned int startTime,endTime;
  if(length>0){
    startTime=m_refTick;
    endTime=startTime+length;
  }else{
    endTime=m_refTick;
    startTime=endTime+length;
  }
  wxString varstr;
  for(int i=0;i<m_dim;++i){
    varstr << m_sqlVariable[i];
    if (m_sqlVariable[i]==_T("disk[5]") ||m_sqlVariable[i]==_T("disk[6]")) varstr << _T("*128 as disk") ;
    if (m_sqlVariable[i]==_T("disk[7]") ||m_sqlVariable[i]==_T("disk[8]")) varstr << _T("*4 as disk") ;
//peng
    if (m_sqlVariable[i]==_T("deadtime")) varstr << _T("/65535 as deadtime") ;
    if (m_sqlVariable[i]==_T("sbst1")) varstr << _T("*0.1 as sbst1") ;
    if (m_sqlVariable[i]==_T("sbst2")) varstr << _T("*0.1 as sbst2") ;
    if (m_sqlVariable[i]==_T("core1")) varstr << _T("*0.1 as core1") ;
    if (m_sqlVariable[i]==_T("core2")) varstr << _T("*0.1 as core2") ;
    if (m_sqlVariable[i]==_T("rate1")) varstr << _T("*0.5 as rate1") ;
    if (m_sqlVariable[i]==_T("rate10")) varstr << _T("*0.5 as rate10") ;
    if (m_sqlTable==_T("slow")&&m_sqlVariable[i]==_T("ppvv")) varstr << _T("*1.054 as ppvv") ;
    if (m_sqlTable==_T("slow")&&m_sqlVariable[i]==_T("p24I")) varstr << _T("*1.5 as p24I") ;
//peng
    if(i<m_dim-1) varstr << _T(",");
  }
  if(m_sqlTable==_T("cmd") ||
     m_sqlTable==_T("mon") || 
     m_sqlTable==_T("g12_sat") || 
     m_sqlTable==_T("turf") ||
     m_sqlTable==_T("other"))
  {  cmd.Printf(_T("SELECT time,%s FROM %s WHERE time>=%u AND time<=%u AND crc=257 ORDER BY time;"),
	       varstr.c_str(),m_sqlTable.c_str(),startTime,endTime);
  }
  else if (m_sqlTable==_T("adu5_pat"))  
  {  cmd.Printf(_T("SELECT time,%s FROM %s WHERE time>=%u AND time<=%u AND crc=257 AND %s ORDER BY time;"),
	       varstr.c_str(),m_sqlTable.c_str(),startTime,endTime,(m_caption==_T("Location A")||m_caption==_T("Orientation A"))?_T("code<200000"):_T("code>200000"));
std::cout<<m_caption<<std::endl;
std::cout<<_T(" Location A: ")<<std::endl;
  }
  else if  (m_sqlTable==_T("adu5_vtg"))  
  {  cmd.Printf(_T("SELECT time,%s FROM %s WHERE time>=%u AND time<=%u AND crc=257 AND %s ORDER BY time;"),
	       varstr.c_str(),m_sqlTable.c_str(),startTime,endTime,m_caption==_T("Speed/Course A")?_T("code<200000"):_T("code>200000"));
  }

  else if(m_sqlTable==_T("slow"))
    cmd.Printf(_T("SELECT time,%s FROM %s WHERE time>=%u AND time<=%u ORDER BY time;"),
	       varstr.c_str(),m_sqlTable.c_str(),startTime,endTime);
  else if (m_sqlTable==_T("hk")&&m_sqlVariable[0]==_T("ppvp")) 
    		cmd.Printf(_T("SELECT time, ppvv * ppvi as ppvp FROM %s WHERE time>=%u AND time<=%u ORDER BY time;"),
	       	m_sqlTable.c_str(),startTime,endTime);
		
  else
    cmd.Printf(_T("SELECT time,%s FROM %s WHERE time>=%u AND time<=%u AND crc=257 ORDER BY time,us;"),
	       varstr.c_str(),m_sqlTable.c_str(),startTime,endTime);

std::cout<<cmd<<std::endl;
  // Prepare SQL request
  wxCommandEvent sqlEvent(wxSQL_REQUEST,GetId());
  sqlEvent.SetEventObject( this );
  sqlEvent.SetString(cmd);
  sqlEvent.SetClientData((void*)parseWrapper);
  // Send it
  GetParent()->GetEventHandler()->ProcessEvent( sqlEvent );
}

void wxMultiGraphDialog::sqlParser(PGresult *result){
  double *x,*y;
  ExecStatusType status=PQresultStatus(result);
  if(status!=PGRES_TUPLES_OK){
    wxLogError(_U(PQresStatus(status)));
    wxLogError(_U(PQresultErrorMessage(result)));
  }else{
    int n=PQntuples(result);
    for(int j=0;j<m_dim;++j){
      if(m_histLength[j]<2){  // Regular curve
	if(n>1){
	  x=new double[n];
	  y=new double[n];
	  // Get x axis
	  int timecol=PQfnumber(result,"time");
	  if(timecol>=0)
	    for(int i=0;i<n;++i) 
	      x[i]=(atol(PQgetvalue(result,i,timecol))-m_refTick)/60.;  // Custom adjust to min or hours or similar...
	  // Get y axes
	  int varcol=PQfnumber(result,m_sqlVariable[j].BeforeFirst('[').mb_str());
	  if(varcol>=0)
	    for(int i=0;i<n;++i)
	      y[i]=atof(PQgetvalue(result,i,varcol));
	  
	  if(m_sqlDataCurve[j]) delete m_sqlDataCurve[j];
	  m_sqlDataCurve[j] = new wxGraphCurve(x,y,n);
	  SetData(m_sqlDataCurve[j],j);
	  delete[] x;
	  delete[] y;
	}else{
  wxCommandEvent e(wxNO_RESULTS,0x10b);
  GetParent()->GetEventHandler()->AddPendingEvent( e );
	}
      }else{
	if(n>0){
	  double hist[m_histLength[j]-1];
	  for(int i=0;i<m_histLength[j];++i) hist[i]=0;
	  int varcol=PQfnumber(result,m_sqlVariable[j].mb_str());
	  for(int i=0;i<n;++i){
	    double y=atof(PQgetvalue(result,i,varcol));
	    for(int k=0;k<m_histLength[j]-1;++k)
	      if(y>=m_histBound[j][k] && y<m_histBound[j][k+1]){
		hist[k]+=1;
		break;
	      }
	  }
	  // Histogram is built, now need to create it's staircase
	  // representations
	  x=new double[2*m_histLength[j]];
	  y=new double[2*m_histLength[j]];
	  for(int k=0;k<m_histLength[j];++k) x[2*k]=x[2*k+1]=m_histBound[j][k];
	  for(int k=1;k<m_histLength[j];++k) y[2*k-1]=y[2*k]=hist[k-1];
	  y[0]=y[2*m_histLength[j]-1]=0;
	  
	  if(m_sqlDataCurve[j]) delete m_sqlDataCurve[j];
	  m_sqlDataCurve[j] = new wxGraphCurve(x,y,2*m_histLength[j]);
	  SetData(m_sqlDataCurve[j],j);  

	  delete[] x;
	  delete[] y;
	}else{
	  wxMessageBox(_T("Fewer than 1 point resulted from the SQL request. Graph will not be updated."),_T("Insufficient SQL Result"),wxOK);
	}
      }
    }  
  }
  PQclear(result);
}
