/* 
 * payload.cpp
 *
 * Definitions for all classes involved in display of Anita payload view. 
 * This includes canvas with mouse controls, zooming, etc., and different 
 * antenna classes. 
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "payload.h"
#endif

#include <wx/log.h>
#include <wx/dcmemory.h>
#include "options.h"
#include "payload.h"
#include "rotationMatrix.h"

extern struct SurfMapType surfMap[];

/*
 *  PayloadPanel
 */
DEFINE_EVENT_TYPE(wxANTENNA_SELECTED);

wxAntennaEvent::wxAntennaEvent( wxEventType commandType, int id )
  : wxNotifyEvent( commandType, id ),m_surf(-1),m_chan(-1){}

IMPLEMENT_CLASS(PayloadPanel,wxPanel)
BEGIN_EVENT_TABLE(PayloadPanel, wxWindow)
    EVT_SIZE( PayloadPanel::OnSize)
    EVT_PAINT( PayloadPanel::OnPaint)
    EVT_MOUSE_EVENTS( PayloadPanel::OnMouse)
    EVT_ERASE_BACKGROUND( PayloadPanel::OnEraseBackground)
END_EVENT_TABLE()

PayloadPanel::PayloadPanel(wxWindow *parent, wxWindowID id, wxString *labels)
  :Nant(0),Nqrh(0),Nbic(0),Ndis(0),Ncomp(0),Nnad(0){
  Create(parent,id , wxDefaultPosition, wxSize(375,400),wxCLIP_CHILDREN);
  SetBackgroundColour( *wxBLACK );

  phi=ini_phi;theta=ini_theta; // Initial view angles
  zoom = ini_zoom;             // Initial zoom factor
  trX=ini_trX;trY=ini_trY;     // Initial translation
  dragging=0;
  CalcProjection();

  // Now generate antennas
  
  for(int i=0;i<N_RFANT;++i) m_antennaLabels.Add(labels[i]);

#define NLayer 3
#define NBicone   4
#define NDiscone  4
//#define NNadir  8
#define NNadir  16

  float tilt=10;
  // original with redicurous accuracy
  //  double r_layer[NLayer]={0.9210802,0.7553198,2.0645374}; //radius for each layer
  //  double z_layer[NLayer]={0,-0.9670034,-3.730752}; //height for each layer
  //double r_layer[]={0.921,0.755,2.065,1.690}; //radius for each layer
  //double z_layer[]={0,-0.967,-3.731,-5.377}; //height for each layer
  double r_layer[]={0.921,0.755,2.065,2.065}; //radius for each layer
  double z_layer[]={0,-0.967,-3.731,-4.698}; //height for each layer
  int n_qrh[]={8,8,16}; //number of antennas for each layer

  double r_discone=1;
  double r_bicone=1.9;
  double z_discone=-1.6;
  double z_bicone=-3;
  
  //  if(0){
  for (int i_layer=0;i_layer<NLayer;i_layer++) Nqrh+=n_qrh[i_layer];

  if(N_SURF>8){
    if (N_SURF>9) {
      Nnad=NNadir ;
    }
    else {
      Nbic=NBicone;
      Ndis=NDiscone;
    }
  }

  Nant=Nqrh+Nbic+Ndis+Nnad;
  m_antennaList=new Antenna*[Nant];
  
  // Generate QRH's
  for (int i_layer=0;i_layer<NLayer;++i_layer)
    for (int i_ant=0;i_ant<n_qrh[i_layer];++i_ant){
      int i=i_ant+(i_layer>0?(((i_layer>1)?n_qrh[1]:0)+n_qrh[0]):0);
      double angle_ant;
      if (i_layer==0) angle_ant=Radian((360./n_qrh[i_layer])*(i_ant));//peng change from +0.5 to 0
      else if (i_layer==1) angle_ant=Radian((360./n_qrh[i_layer])*(i_ant+0.5));//peng change from +1 to +0.5
      else angle_ant=Radian((360./n_qrh[i_layer])*i_ant);
      double x=cos(angle_ant)*r_layer[i_layer]*100;
      double y=sin(angle_ant)*r_layer[i_layer]*100;
      double z=(z_layer[i_layer]-z_layer[NLayer-1]/2)*100;
      Vector feed(x,y,z);
      double nx=cos(angle_ant)*sin(Radian(90+tilt));
      double ny=sin(angle_ant)*sin(Radian(90+tilt));
      double nz=cos(Radian(90+tilt));
      Vector n(nx,ny,nz);
      double fx=-sin(angle_ant);
      double fy=cos(angle_ant);
      double fz=0;
      Vector f(fx,fy,fz);

      m_antennaList[i]=static_cast<Antenna*>(new QRH(this,feed,n,f));
    }  
  
  // Generate Nadir's
  for (int i=0;i<Nnad;++i){
    int i_layer=3 ;
    double angle_ant;
    angle_ant=Radian((360./Nnad)*i);
    double x=cos(angle_ant)*r_layer[i_layer]*100;
    double y=sin(angle_ant)*r_layer[i_layer]*100;
    double z=(z_layer[i_layer]-z_layer[NLayer-1]/2)*100;
    Vector feed(x,y,z);
    double nx=cos(angle_ant)*sin(Radian(90+tilt));
    double ny=sin(angle_ant)*sin(Radian(90+tilt));
    double nz=cos(Radian(90+tilt));
    Vector n(nx,ny,nz);
    double fx=-sin(angle_ant);
    double fy=cos(angle_ant);
    double fz=0;
    Vector f(fx,fy,fz);

    m_antennaList[i+Nqrh]=static_cast<Antenna*>(new QRH(this,feed,n,f));
  }  
  
  // Generate vetos, these are bicones
  for (int i=0;i<Nbic;++i){
    double angle_ant=Radian((360./Nbic)*i+360./16);
    double x=cos(angle_ant)*r_bicone*100;
    double y=sin(angle_ant)*r_bicone*100;
    double z=(z_bicone-z_layer[NLayer-1]/2)*100;
    Vector feed(x,y,z);
    double nx=cos(angle_ant);
    double ny=sin(angle_ant);
    double nz=0;
    Vector n(nx,ny,nz);
    double fx=-sin(angle_ant);
    double fy=cos(angle_ant);
    double fz=0;
    Vector f(fx,fy,fz);
    m_antennaList[i+Nqrh]=static_cast<Antenna*>(new Bicone(this,feed,n,f));
  }

  // Generate vetos, these are discones
  for (int i=0;i<Ndis;++i){
    double angle_ant=Radian((360./Ndis)*i-360./16);
    double x=cos(angle_ant)*r_discone*100;
    double y=sin(angle_ant)*r_discone*100;
    double z=(z_discone-z_layer[NLayer-1]/2)*100;
    Vector feed(x,y,z);
    Vector n(0,0,-1); // Always pointing down
    double fx=-sin(angle_ant);
    double fy=cos(angle_ant);
    double fz=0;
    Vector f(fx,fy,fz);
    m_antennaList[i+Nqrh+Nbic]=static_cast<Antenna*>(new Discone(this,feed,n,f));
  }


//   }else{
//   // Testing with one horn
//     Nant=1;Nbic=1;
//     m_antennaList=new Antenna*[Nant];
//     m_antennaList[0]=static_cast<Antenna*>(new Bicone(this));
//   }

  SetRangeSpecs();
  
  // Generate compass
  Ncomp=12; 
  double da=360./Ncomp;
  Compass = new CompassItem*[Ncomp];
  for(int i=0;i<Ncomp;++i)
    Compass[i] = new CompassItem(this,da*i);
}

PayloadPanel::~PayloadPanel(){
  int i;
  for(i=0;i<Nqrh;++i) delete static_cast<QRH*>(m_antennaList[i]);
  for(;i<Nqrh+Nnad;++i) delete static_cast<QRH*>(m_antennaList[i]);
  for(;i<Nqrh+Nnad+Nbic;++i) delete static_cast<Bicone*>(m_antennaList[i]);
  for(;i<Nant;++i) delete static_cast<Discone*>(m_antennaList[i]);
  delete[] m_antennaList;
  for(i=0;i<Ncomp;++i) delete Compass[i];
  delete[] Compass;
}

void PayloadPanel::OnSize(wxSizeEvent &event){
  CalcProjection();
  event.Skip();
}

void PayloadPanel::OnPaint(wxPaintEvent &WXUNUSED(event)){
  wxPaintDC dc(this);  
#ifdef WIN32
  int width,height;
  GetClientSize(&width,&height);
  wxMemoryDC memdc;
  wxBitmap bitmap(width,height);
  memdc.SelectObject(bitmap);
  Draw(&memdc);
  dc.Blit(0,0,width,height,&memdc,0,0);
#else
  Draw(&dc);
#endif
}

void PayloadPanel::OnEraseBackground(wxEraseEvent &event){
#ifndef WIN32
  event.Skip();
#endif
}

void PayloadPanel::OnMouse(wxMouseEvent &event){
  int winsizex,winsizey,winsize;
  GetClientSize(&winsizex,&winsizey);
  winsize=Min(winsizex,winsizey);

  // Check which mouse event brought us here and act accordingly
  if (event.ButtonDown()){ // Record start location
    origx=lastx=event.GetX();
    origy=lasty=event.GetY();
  }else if(event.ButtonUp()){
    if(dragging){
      dragging=false;
    }else if(event.ShiftDown()){  // Antenna selected
      // Locate closest feed point (or other reference point for vetos, etc)
      int iMin=-1;
      double dMin=1e30;
      for(int i=0;i<Nant;++i){
	wxPoint c=Project(m_antennaList[i]->GetFeedPoint());
	double dx=sqrt(sqr(c.x-origx)+sqr(c.y-origy));
	if(dx<dMin){ iMin=i; dMin=dx;}
      }
      if(iMin<0) return; // Something goofy happened
      wxAntennaEvent e( wxANTENNA_SELECTED, GetId() );
      e.SetEventObject( this );
      // Must remove one due to 1-based counting of surfs & channels in surfMap
      if(event.LeftUp()){
	e.SetRFChannel(surfMap[iMin].h_surf-1,surfMap[iMin].h_chan-1);
      }else if(event.RightUp()){
	e.SetRFChannel(surfMap[iMin].v_surf-1,surfMap[iMin].v_chan-1);
      }else{
	return;
      }
      GetEventHandler()->ProcessEvent( e );
    }else{
      if(event.LeftUp()){ // Zoom in and translate
	zoom*=1.4142;
	trX-=(double)(event.GetX()-winsizex/2)/zoom/winsize;
	trY-=(double)(event.GetY()-winsizey/2)/zoom/winsize;
      }else if(event.MiddleUp()){ // Reset 
	zoom=ini_zoom;
	phi=ini_phi;
	theta=ini_theta;
	trX=ini_trX;
	trY=ini_trY;
      }else if(event.RightUp()){ // Zoom out 
	zoom/=1.4142;
      }
      CalcProjection();
      Refresh(true);
    }
  }else if(event.Dragging()){ // Active change
    int dx,dy;
    int posx=event.GetX();
    int posy=event.GetY();
    
    /* Don't allow change which is too small*/
    if (abs(posx-origx)+abs(posy-origy)<2) return;

    /* Calculate motion vector */
    dx=posx-lastx;
    dy=posy-lasty;
   
    if(event.LeftIsDown()){ // Active rotation
      theta-=180.*(double)dy/(double)winsizey;
      if (theta<0) theta=0;
      if (theta>180) theta=180;
      phi-=180.*(double)dx/(double)winsizex;
    }else if (event.MiddleIsDown()) { // Active zoom
      double z;
      z=5*(double)(dx-dy)/winsize;
      zoom*=z+1;
      trX-=(double)(origx-winsizex/2)/winsize*z/zoom;
      trY-=(double)(origy-winsizey/2)/winsize*z/zoom;
    }else if(event.RightIsDown()){ // Active translation
      trX+=(double)(dx)/zoom/winsize;
      trY+=(double)(dy)/zoom/winsize;
    }

    dragging=true;

    lastx=posx;
    lasty=posy;
    CalcProjection();
    Refresh(true);
  }
}

int CompOrder(const void *a,const void *b){
  const struct Order *A=reinterpret_cast<const struct Order *>(a);
  const struct Order *B=reinterpret_cast<const struct Order *>(b);
  if(A->distance<B->distance)
    return -1;
  else if(A->distance>B->distance)
    return 1;
  else
    return 0;
}

void PayloadPanel::Draw(wxDC *dc){
#ifdef DEBUG
  static Line Xaxis(Vector(0,0,0),Vector(1000,0,0));
  static Line Yaxis(Vector(0,0,0),Vector(0,1000,0));
  static Line Zaxis(Vector(0,0,0),Vector(0,0,1000));
  DrawLine(dc,&Xaxis,*wxRED_PEN);
  DrawLine(dc,&Yaxis,*wxGREEN_PEN);
  DrawLine(dc,&Zaxis,*wxCYAN_PEN);
#endif
  
  // Draw arrow in direction of phi sector 2 (direction of ADU fore)
  static Arrow phi2Marker(this,Vector(0,0,-150),
			  Vector(100*cos(22.5*M_PI/180),
				 100*sin(22.5*M_PI/180),
				 -150),
			  Vector(75*cos(22.5*M_PI/180)-20*sin(22.5*M_PI/180),
				 75*sin(22.5*M_PI/180)+20*cos(22.5*M_PI/180),
				 -150),
			  Vector(75*cos(22.5*M_PI/180)+20*sin(22.5*M_PI/180),
				 75*sin(22.5*M_PI/180)-20*cos(22.5*M_PI/180),
				 -150));

  if(AviewOpts::PayloadStyle==AviewOpts::Mesh){  // Draw wire mesh view
    for(int i=0;i<Nant;++i) m_antennaList[i]->Draw(dc);
    phi2Marker.Draw(dc);
    for(int i=0;i<Ncomp;++i) Compass[i]->Draw(dc);
  }else{   // Filled view
    // Decide in which order to draw objects
    struct Order object_order[Nant+Ncomp+1];
    for(int i=0;i<Nant;++i){
      object_order[i].index.p=static_cast<Drawable*>(m_antennaList[i]);
      object_order[i].distance=m_antennaList[i]->GetFeedPoint()*eyevec;
    }
    for(int i=0;i<Ncomp;++i){
      object_order[Nant+i].index.p=static_cast<Drawable*>(Compass[i]);
      object_order[Nant+i].distance=Compass[i]->GetPosition()*eyevec;
    }

    object_order[Nant+Ncomp].index.p=static_cast<Drawable*>(&phi2Marker);
    object_order[Nant+Ncomp].distance=phi2Marker.GetStart()*eyevec;

    qsort(object_order,Nant+Ncomp+1,sizeof(struct Order),CompOrder);
    
    for(int i=0;i<Nant+Ncomp+1;++i) object_order[i].index.p->Draw(dc);
  }
}

/* calculate the projection matrix etc. */
void PayloadPanel::CalcProjection(){
   double ph,th;
   double scale;
   int winsizex,winsizey,winsize;
   
   GetClientSize(&winsizex,&winsizey);
   winsize=Min(winsizey,winsizex);

   scale=winsize*zoom/1000.;
   ph=phi*(M_PI/180.);
   th=theta*(M_PI/180.);
   Xproj.set(-scale*sin(ph),scale*cos(ph),0);
   Yproj.set(scale*cos(ph)*cos(th),scale*sin(ph)*cos(th), -scale*sin(th));
   Xtr = 0.5 + trX*zoom*winsize + 0.5*winsizex;
   Ytr = 0.5 + trY*zoom*winsize + 0.5*winsizey;
   eyevec.set(sin(th)*cos(ph),sin(th)*sin(ph),cos(th));
   Zproj.set(sin(th)*cos(ph),sin(th)*sin(ph),cos(th));
}

// Projects 3D point into 2D canvas location
wxPoint PayloadPanel::Project(const Vector &v){
  int x = (int)(Xtr + Xproj*v);
  int y = (int)(Ytr + Yproj*v);
  return wxPoint(x,y);
}

void PayloadPanel::DrawText(wxDC *dc,const wxString &text,const Vector &v,const wxColour &colour){
  wxPoint c;
  c=Project(v);
  dc->SetTextForeground(colour);
  dc->DrawText(text,c.x,c.y);
}

void PayloadPanel::DrawLine(wxDC *dc,Line *l,const wxPen &pen){
  wxPoint c[2];
  for(int i=0;i<2;++i) c[i]=Project(l->p[i]);
  dc->SetPen(pen);
  dc->DrawLine(c[0].x,c[0].y,c[1].x,c[1].y);
}

void PayloadPanel::DrawSurface(wxDC *dc,Surface *s,const wxPen &pen){
  wxPoint ca=Project(s->pa);
  wxPoint cc=Project(s->pc);
  wxPoint c[4];
  for(int i=0;i<4;++i) c[i]=Project(s->p[i]);

  dc->SetPen(pen);
  dc->DrawPolygon(4,c);
}

void PayloadPanel::SetTriggerPatternColour(int i,const wxColour &colour){
  static_cast<QRH*>(m_antennaList[i])->SetColour(colour);
  Refresh();
}
      
void PayloadPanel::SetOrientation(double orient){
  for(int i=0;i<Ncomp;++i) Compass[i]->SetOrientation(orient);
}

// Transfers range specification from global namespace to appropriate 
// antenna elements
void PayloadPanel::SetRangeSpecs(){
  using namespace AviewOpts;
  wxRangeSpec **RFSpec=(RFReference==RMS?RFSpecRMS:RFSpecAbsolute);
  for(int i=0;i<Nqrh;++i){
    static_cast<QRH*>(m_antennaList[i])->SetRangeSpec(RFSpec[2*i],0);   // Horizontal
    static_cast<QRH*>(m_antennaList[i])->SetRangeSpec(RFSpec[2*i+1],1); // Vertical
  }
  // nadir antennas
  for(int i=0;i<Nnad;++i){
    static_cast<QRH*>(m_antennaList[i])->SetRangeSpec(RFSpec[2*(Nqrh+i)],0);   // Horizontal
    static_cast<QRH*>(m_antennaList[i])->SetRangeSpec(RFSpec[2*(Nqrh+i)+1],1); // Vertical
  }
  for(int i=0;i<Nbic;++i)
    static_cast<Bicone*>(m_antennaList[Nqrh+i])->SetRangeSpec(RFSpec[2*Nqrh+i]);
  for(int i=0;i<Ndis;++i)
    static_cast<Discone*>(m_antennaList[Nqrh+Nbic+i])->SetRangeSpec(RFSpec[2*Nqrh+Nbic+i]);
}

// Set color coding for an antenna
void PayloadPanel::SetValue(wxGraphCurve *rfCurve,int i,int j){
  if(i<0 || i>N_SURF){
    wxLogError(_T("[PayloadPanel::SetValue] Attempt to set value for SURF %d > N_SURF %d"),i+1,N_SURF);
  }else if(j<0 || j>N_CHAN){
    wxLogError(_T("[PayloadPanel::SetValue] Attempt to set value for CHAN %d > N_CHAN-1 %d"),i,N_CHAN-1);
  }else{
    double vMax=0;
    if(rfCurve){
      // Find mean and max peak
      double mean=0;
      int Nsamp=rfCurve->GetLength();
    
      for(int n=0;n<Nsamp;++n) mean += (*rfCurve)[n].y;
      mean/= Nsamp ; 
      vMax=1e-30;
      for(unsigned int n=0;n<rfCurve->GetLength();++n)
	vMax=Max(fabs((*rfCurve)[n].y-mean),vMax);
      
      if(AviewOpts::RFReference==AviewOpts::RMS){
	//RMS of first 60 samples
#define N_RMS 60
	double rms=0,rms2=0;
	int rmsN=wxMin(N_RMS,rfCurve->GetLength());
	for(int n=0;n<rmsN;++n){  
	  rms+=(*rfCurve)[n].y;
	  rms2 += sqr((*rfCurve)[n].y);
	}
	if(rmsN>0){
	  rms/=rmsN;
	  rms2/=rmsN;
	  rms = rms2 - sqr(rms);
	  if (rms> 0.){
	    rms= sqrt(rms);
	    vMax/=rms;
	  }else{
	    vMax=0;
	  }
	}
      }
    }      

    // Find which antenna to store this value in
    wxString label;
    if(i<N_SURF-1 || N_SURF>9){ // QRH channels
      label.Printf(_T("%d:%d"),i+1,j+1);
      int n=m_antennaLabels.Index(label);
      if(n!=wxNOT_FOUND)
	static_cast<QRH*>(m_antennaList[n/2])->SetValue(vMax,n%2);
      else
	wxLogError(_T("[PayloadPanel::SetValue] Attempt to set value of unknown label '%s'"),label.c_str());
    }else if(i<N_SURF){ // Veto channels
      label.Printf(_T("veto%d"),j+1);
      int n=m_antennaLabels.Index(label);
      if(n!=wxNOT_FOUND)
	static_cast<Bicone*>(m_antennaList[n-Nqrh])->SetValue(vMax);
      else
	wxLogError(_T("[PayloadPanel::SetValue] Attempt to set value of unknown label '%s'"),label.c_str());
    }else{     // Nadir channels
      label.Printf(_T("nad%d"),j+1);
      int n=m_antennaLabels.Index(label);
      if(n!=wxNOT_FOUND)
	static_cast<Discone*>(m_antennaList[n-Nqrh])->SetValue(vMax);
      else
	wxLogError(_T("[PayloadPanel::SetValue] Attempt to set value of unknown label '%s'"),label.c_str());
    }
  }
  Refresh(true);
}


/* Drawable */

Drawable::Drawable(PayloadPanel *parent)
  :m_owner(parent){}

Drawable::~Drawable(){}

/* Arrow */
Arrow::Arrow(PayloadPanel *parent,const Vector &start,const Vector &end,const Vector &arm1,const Vector &arm2)
  :Drawable(parent){
  element[0]=Line(start,end);
  element[1]=Line(end,arm1);
  element[2]=Line(end,arm2);
}

Arrow::~Arrow(){}

void Arrow::Draw(wxDC *dc){
  for(int i=0;i<3;++i) m_owner->DrawLine(dc,&element[i],*wxWHITE_PEN);
}

/*
 * Antenna
 */
Antenna::Antenna(PayloadPanel *parent)
  :Drawable(parent),feedPoint(Vector()),dir(Vector(1,0,0)),face(Vector(0,1,0)){}

Antenna::~Antenna(){}

// No translation done in base class
void Antenna::SetFeedPoint(const Vector &feed){ feedPoint = feed;}
// No rotation done in base class
void Antenna::SetDirection(const Vector &n,const Vector &f){
  dir=n;
  face=f;
}
// No drawing done in base class
void Antenna::Draw(wxDC *dc){}

/* 
 * QRH
 */ 

#define BOX_H   10
#define FEED_L  30
#define PARA_H  50
#define MOUTH_L 90

// Default constructor, generates antenna at origin, facing along X axis
// which is then translated and rotated per input vectors
QRH::QRH(PayloadPanel *parent,const Vector &feed,const Vector &n,const Vector &f)
  :Antenna(parent),m_colour(wxColour(192,192,192)){

  // Create feedbox
  feedBox[1]=Surface(Vector(-BOX_H,-FEED_L/2, FEED_L/2), 
		     Vector(-BOX_H, FEED_L/2, FEED_L/2),
		     Vector(     0, FEED_L/2, FEED_L/2),
		     Vector(     0,-FEED_L/2, FEED_L/2));
  feedBox[0]=Surface(Vector(-BOX_H, FEED_L/2,-FEED_L/2),
		     Vector(     0, FEED_L/2,-FEED_L/2),
		     Vector(     0, FEED_L/2, FEED_L/2),
		     Vector(-BOX_H, FEED_L/2, FEED_L/2));
  feedBox[3]=Surface(Vector(     0,-FEED_L/2,-FEED_L/2),
		     Vector(     0, FEED_L/2,-FEED_L/2),
		     Vector(-BOX_H, FEED_L/2,-FEED_L/2),
		     Vector(-BOX_H,-FEED_L/2,-FEED_L/2));
  feedBox[2]=Surface(Vector(-BOX_H,-FEED_L/2, FEED_L/2),
		     Vector(     0,-FEED_L/2, FEED_L/2),
		     Vector(     0,-FEED_L/2,-FEED_L/2),
		     Vector(-BOX_H,-FEED_L/2,-FEED_L/2));
  // Back side (needed for opaque view)
  feedBox[4]=Surface(Vector(-BOX_H,-FEED_L/2,-FEED_L/2),
		     Vector(-BOX_H, FEED_L/2,-FEED_L/2),
		     Vector(-BOX_H, FEED_L/2, FEED_L/2),
		     Vector(-BOX_H,-FEED_L/2, FEED_L/2));

  // Create horn mouth sides
  mouth[1]=Surface(Vector(PARA_H,-MOUTH_L/2,-MOUTH_L/2),
		   Vector(PARA_H, MOUTH_L/2,-MOUTH_L/2),
		   Vector(     0, FEED_L/2,-FEED_L/2),
		   Vector(     0,-FEED_L/2,-FEED_L/2));
  mouth[0]=Surface(Vector(     0, FEED_L/2,-FEED_L/2),
		   Vector(PARA_H, MOUTH_L/2,-MOUTH_L/2),
		   Vector(PARA_H, MOUTH_L/2, MOUTH_L/2),
		   Vector(     0, FEED_L/2, FEED_L/2));
  mouth[3]=Surface(Vector(     0,-FEED_L/2, FEED_L/2),
		   Vector(     0, FEED_L/2, FEED_L/2),
		   Vector(PARA_H, MOUTH_L/2, MOUTH_L/2),
		   Vector(PARA_H,-MOUTH_L/2, MOUTH_L/2));
  mouth[2]=Surface(Vector(     0,-FEED_L/2, FEED_L/2),
		   Vector(PARA_H,-MOUTH_L/2, MOUTH_L/2),
		   Vector(PARA_H,-MOUTH_L/2,-MOUTH_L/2),
		   Vector(     0,-FEED_L/2,-FEED_L/2));

  // Create ridges
  for(int i=0;i<4;++i) ridge[i]=Ridge(i);

  // Rotate and move as needed
  SetDirection(n,f);
  SetFeedPoint(feed);
}

QRH::~QRH(){}

void QRH::SetFeedPoint(const Vector &feed){
  Vector dx=feed-feedPoint;
  for(int i=0;i<5;++i) feedBox[i].translate(dx);
  for(int i=0;i<4;++i){
    mouth[i].translate(dx);
    for(int j=0;j<5;++j)
      ridge[i].side[j].translate(dx);
  }

  feedPoint=feed;
}

void QRH::SetDirection(const Vector &n,const Vector &f){
  RotationMatrix matrix(dir,face,n,f);
  for(int i=0;i<5;++i) feedBox[i].rotate(matrix);
  for(int i=0;i<4;++i){
    mouth[i].rotate(matrix);
    for(int j=0;j<5;++j)
      ridge[i].side[j].rotate(matrix);
  }
  dir=n;
  face=f;
}

void QRH::Draw(wxDC *dc){
  if(AviewOpts::PayloadStyle==AviewOpts::Mesh){  // Draw wire mesh view
    dc->SetBrush(*wxTRANSPARENT_BRUSH);
    wxPen pen;
    if(m_colour==*wxLIGHT_GREY) pen=*wxWHITE_PEN;
    else pen=wxPen(m_colour);
    for(int s=0;s<4;++s){
      m_owner->DrawSurface(dc,&feedBox[s],pen);
      m_owner->DrawSurface(dc,&mouth[s],pen);
      for(int j=0;j<5;++j)
	m_owner->DrawSurface(dc,&(ridge[s].side[j]),ridge[s].colour);
    }
  }else{ // Filled view
    // Decide ordering of antenna sides
    struct Order side_order[4];
    for(int i=0;i<4;++i){
      side_order[i].index.i=i;
      side_order[i].distance=mouth[i].pc*m_owner->GetEyevec();
    }
    qsort(side_order,4,sizeof(struct Order),CompOrder);
      
    if(feedBox[4].pa*m_owner->GetEyevec()<0){ // Feed box in front of mouth
      // Draw mouth, then feedbox
      for(int s=0;s<4;++s){
	for(int j=0;j<5;++j)
	  // Draw ridge side if visible
	  if(ridge[side_order[s].index.i].side[j].pa*m_owner->GetEyevec()<0){ 
	    dc->SetBrush(ridge[side_order[s].index.i].colour);
	    m_owner->DrawSurface(dc,&(ridge[side_order[s].index.i].side[j]),*wxWHITE_PEN);
	  }
	// Draw mouth side
	dc->SetBrush(wxBrush(m_colour));
	m_owner->DrawSurface(dc,&mouth[side_order[s].index.i],*wxWHITE_PEN);
      }
      // Draw feedbox sides, then edge plate
      dc->SetBrush(wxBrush(m_colour));
      for(int s=0;s<4;++s)
	m_owner->DrawSurface(dc,&feedBox[side_order[s].index.i],*wxWHITE_PEN);
      m_owner->DrawSurface(dc,&feedBox[4],*wxWHITE_PEN); 
    }else{   // Mouth in front of the feed
      // Draw feedbox edge plate, then sides, then mouth
      dc->SetBrush(wxBrush(m_colour));
      m_owner->DrawSurface(dc,&feedBox[4],*wxWHITE_PEN); 
      for(int s=0;s<4;++s)
	m_owner->DrawSurface(dc,&feedBox[side_order[s].index.i],*wxWHITE_PEN);
      for(int s=0;s<4;++s){
	// If face behind ridge, draw it first
	if(mouth[side_order[s].index.i].pa*m_owner->GetEyevec()>=0){ 
	  dc->SetBrush(wxBrush(m_colour));
	  m_owner->DrawSurface(dc,&mouth[side_order[s].index.i],*wxWHITE_PEN);
	}
	for(int j=0;j<5;++j)
	  // Draw ridge side if visible
	  if(ridge[side_order[s].index.i].side[j].pa*m_owner->GetEyevec()<0){
	    dc->SetBrush(ridge[side_order[s].index.i].colour);
	    m_owner->DrawSurface(dc,&ridge[side_order[s].index.i].side[j],*wxWHITE_PEN);
	  }
	// If face infront of ridge, draw it now
	if(mouth[side_order[s].index.i].pa*m_owner->GetEyevec()<0){ 
	  dc->SetBrush(wxBrush(m_colour));
	  m_owner->DrawSurface(dc,&mouth[side_order[s].index.i],*wxWHITE_PEN);
	}
      }
    }
  }
}

// Ridge size definitions
#define RIDGE_W1 20
#define RIDGE_W2 5
#define RIDGE_H  10 
#define DL_ML_HALF  ((MOUTH_L-FEED_L)/2.)
#define RIDGE_ANGLE Degree(atan2(DL_ML_HALF,PARA_H))
#define RIDGE_L     sqrt(sqr(PARA_H)+sqr(DL_ML_HALF))
#define STEP1 0.65
#define STEP2 0.85

// Ridge constructed at origin and rotated and trasplated per input side
// argument
Ridge::Ridge(int iSide){
  // Top sides
  side[0]=Surface(Vector(    0, -RIDGE_W2/2, 0),
		  Vector(    0,  RIDGE_W2/2, 0),
		  Vector(RIDGE_L*STEP1, ((RIDGE_W1-RIDGE_W2)*STEP1+RIDGE_W2)/2, RIDGE_H),
		  Vector(RIDGE_L*STEP1,-((RIDGE_W1-RIDGE_W2)*STEP1+RIDGE_W2)/2, RIDGE_H));
  side[1]=Surface(Vector(RIDGE_L*STEP1,-((RIDGE_W1-RIDGE_W2)*STEP1+RIDGE_W2)/2, RIDGE_H),
		  Vector(RIDGE_L*STEP1, ((RIDGE_W1-RIDGE_W2)*STEP1+RIDGE_W2)/2, RIDGE_H),
		  Vector(RIDGE_L*STEP2, ((RIDGE_W1-RIDGE_W2)*STEP2+RIDGE_W2)/2, RIDGE_H),
		  Vector(RIDGE_L*STEP2,-((RIDGE_W1-RIDGE_W2)*STEP2+RIDGE_W2)/2, RIDGE_H));
  
  side[2]=Surface(Vector(RIDGE_L*STEP2,-((RIDGE_W1-RIDGE_W2)*STEP2+RIDGE_W2)/2, RIDGE_H),
		  Vector(RIDGE_L*STEP2, ((RIDGE_W1-RIDGE_W2)*STEP2+RIDGE_W2)/2, RIDGE_H),
		  Vector( RIDGE_L, RIDGE_W1/2, 0),
		  Vector( RIDGE_L,-RIDGE_W1/2, 0));

  // Edge sides
  side[3]=Surface(Vector(    0,  RIDGE_W2/2, 0),
		  Vector( RIDGE_L, RIDGE_W1/2, 0),
		  Vector(RIDGE_L*STEP2, ((RIDGE_W1-RIDGE_W2)*STEP2+RIDGE_W2)/2, RIDGE_H),
		  Vector(RIDGE_L*STEP1, ((RIDGE_W1-RIDGE_W2)*STEP1+RIDGE_W2)/2, RIDGE_H));
  
  side[4]=Surface(Vector( RIDGE_L,-RIDGE_W1/2, 0),
		  Vector(    0, -RIDGE_W2/2, 0),
		  Vector(RIDGE_L*STEP1,-((RIDGE_W1-RIDGE_W2)*STEP1+RIDGE_W2)/2, RIDGE_H),
		  Vector(RIDGE_L*STEP2,-((RIDGE_W1-RIDGE_W2)*STEP2+RIDGE_W2)/2, RIDGE_H));
  
  
  Vector start_dir(1,0,0);
  Vector start_face(0,1,0);
  Vector end_dir;
  Vector end_face;
  Vector dx;
  
  switch(iSide){
  case 1:  // Lower vertical
    end_dir.set(sin(Radian(90+RIDGE_ANGLE)),0,cos(Radian(90+RIDGE_ANGLE)));
    end_face.set(0,1,0);
    dx=Vector(0,0,-FEED_L/2);
    break;
  case 0: // Left horizontal (facing out of the horn)
    end_dir.set(cos(Radian(RIDGE_ANGLE)),sin(Radian(RIDGE_ANGLE)),0);
    end_face.set(0,0,1);
    dx=Vector(0,FEED_L/2,0);
    break;
  case 3: // Upper vertical
    end_dir.set(sin(Radian(90-RIDGE_ANGLE)),0,cos(Radian(90-RIDGE_ANGLE)));
    end_face.set(0,-1,0);
    dx=Vector(0,0,FEED_L/2);
    break;
  case 2: // Right horizontal (facing out of the horn)
    end_dir.set(cos(Radian(-RIDGE_ANGLE)),sin(Radian(-RIDGE_ANGLE)),0);
    end_face.set(0,0,-1);
    dx=Vector(0,-FEED_L/2,0);
    break;
  default:  // Shouldn't happen, so set no rotation
    end_dir=start_dir;
    end_face=start_face;
    break;
  }

  // Rotate and translate ridge into place
  RotationMatrix matrix(start_dir,start_face,end_dir,end_face);
  for(int i=0;i<5;++i){
    side[i].rotate(matrix);
    side[i].translate(dx);
  }
  
}

AntennaResponse::AntennaResponse()
  :m_value(-1),m_rangeSpec(NULL),colour(m_Off_Colour){}

void AntennaResponse::SetValue(double val){
  m_value=val;
  if(!m_rangeSpec || m_value<0) return;
  int i;
  for(i=0;m_rangeSpec[i].colour.Ok();++i)
    if(val>=m_rangeSpec[i].low && val<m_rangeSpec[i].high) break;
  if(m_rangeSpec[i].colour.Ok()) colour=m_rangeSpec[i].colour;
  else colour=m_OutOfRange_Colour;
}

void AntennaResponse::SetRangeSpec(wxRangeSpec *rangeSpec){
  m_rangeSpec=rangeSpec;
  SetValue(m_value);
}

wxColour AntennaResponse::m_OutOfRange_Colour=wxColour(255,0,0);
wxColour AntennaResponse::m_Off_Colour=wxColour(0xc0,0xc0,0xc0);

/*
 * Discone
 */
#define DISC_R 20
#define DISC_STEP 0.7
#define DISC_L 40
#define DISC_FLARE 5

Discone::Discone(PayloadPanel *parent,const Vector &feed,const Vector &n,
		 const Vector &f)
  :Antenna(parent){
  for(int i=0;i<6;++i){
    double th=Radian(i*60.);
    // Disc wire
    wire[i]=Line(Vector(),Vector(-DISC_FLARE,DISC_R*sin(th),DISC_R*cos(th))); 
    // Cone wire
    wire[i+6]=Line(Vector(),Vector(DISC_L*DISC_STEP,DISC_R*sin(th),DISC_R*cos(th)));
    wire[i+12]=Line(Vector(DISC_L*DISC_STEP,DISC_R*sin(th),DISC_R*cos(th)),
		    Vector(DISC_L,0,0));

  }

  // Rotate and translate 
  SetDirection(n,f);
  SetFeedPoint(feed);
}

Discone::~Discone(){}

void Discone::SetFeedPoint(const Vector &feed){ 
  Vector dx=feed-feedPoint;
  for(int i=0;i<18;++i) wire[i].translate(dx);
  feedPoint=feed;
}

void Discone::SetDirection(const Vector &n,const Vector &f){
  RotationMatrix matrix(dir,face,n,f);
  for(int i=0;i<18;++i) wire[i].rotate(matrix);
  dir=n;
  face=f;
}

void Discone::Draw(wxDC *dc){
  for(int i=0;i<18;++i)
    m_owner->DrawLine(dc,&wire[i],wxPen(colour));
}

/*
 * Bicone
 */
#define BIC_R 20
#define BIC_STEP 0.7
#define BIC_L 40

Bicone::Bicone(PayloadPanel *parent,const Vector &feed,const Vector &n,
	       const Vector &f)
  :Antenna(parent){
  for(int i=0;i<6;++i){
    double th=Radian(i*60.);
    wire[i]=Line(Vector(BIC_R*sin(th),BIC_R*cos(th),BIC_L*BIC_STEP),
		 Vector(-BIC_R*sin(th),-BIC_R*cos(th),-BIC_L*BIC_STEP));
    wire[i+6]=Line(Vector(BIC_R*sin(th),BIC_R*cos(th),BIC_L*BIC_STEP),
		   Vector(0,0,BIC_L));
    wire[i+12]=Line(Vector(-BIC_R*sin(th),-BIC_R*cos(th),-BIC_L*BIC_STEP),
		    Vector(0,0,-BIC_L));
  }

  // Rotate and translate 
  SetDirection(n,f);
  SetFeedPoint(feed);
}

Bicone::~Bicone(){}

void Bicone::SetFeedPoint(const Vector &feed){ 
  Vector dx=feed-feedPoint;
  for(int i=0;i<18;++i) wire[i].translate(dx);
  feedPoint=feed;
}

void Bicone::SetDirection(const Vector &n,const Vector &f){
  RotationMatrix matrix(dir,face,n,f);
  for(int i=0;i<18;++i) wire[i].rotate(matrix);
  dir=n;
  face=f;
}

void Bicone::Draw(wxDC *dc){
  for(int i=0;i<18;++i)
    m_owner->DrawLine(dc,&wire[i],wxPen(colour));
}

/*
 * Bowtie
 */

#define BOW_W 20
#define BOW_L 20
#define BOW_STEP 0.8

Bowtie::Bowtie(PayloadPanel *parent,const Vector &feed,const Vector &n,
		 const Vector &f)
  :Antenna(parent){

  surf[0]=Surface(Vector(),
		  Vector(0, BOW_W/2,BOW_L*BOW_STEP),
		  Vector(0, 0,BOW_L),
		  Vector(0,-BOW_W/2,BOW_L*BOW_STEP));
  surf[1]=Surface(Vector(),
		  Vector(0,-BOW_W/2,-BOW_L*BOW_STEP),
		  Vector(0, 0,-BOW_L),
		  Vector(0, BOW_W/2,-BOW_L*BOW_STEP));

  // Rotate and translate 
  SetDirection(n,f);
  SetFeedPoint(feed);
}

Bowtie::~Bowtie(){}

void Bowtie::SetFeedPoint(const Vector &feed){ 
  Vector dx=feed-feedPoint;
  for(int i=0;i<2;++i) surf[i].translate(dx);
  feedPoint=feed;
}

void Bowtie::SetDirection(const Vector &n,const Vector &f){
  RotationMatrix matrix(dir,face,n,f);
  for(int i=0;i<2;++i) surf[i].rotate(matrix);
  dir=n;
  face=f;
}

void Bowtie::Draw(wxDC *dc){
  for(int i=0;i<2;++i){
    if(AviewOpts::PayloadStyle==AviewOpts::Mesh){  // Draw wire mesh view
      dc->SetBrush(*wxTRANSPARENT_BRUSH);
      m_owner->DrawSurface(dc,&surf[i],wxPen(colour));
    }else{
      dc->SetBrush(colour);
      m_owner->DrawSurface(dc,&surf[i],*wxWHITE_PEN);
    }
  }
}


/* Surface class */  

Surface::Surface(const Vector &p1,const Vector &p2,const Vector &p3,
		 const Vector &p4){
  p[0]=p1;
  p[1]=p2;
  p[2]=p3;
  p[3]=p4;

  pc=int3d(int3d(p[0],p[1]),int3d(p[2],p[3]));
  pa=out3d(0.5);
} 

void Surface::translate(const Vector &v){
  for(int i=0;i<4;++i) p[i]+=v;
  pc+=v;
}

void Surface::rotate(RotationMatrix &m){
  for(int i=0;i<4;++i) p[i]=m.rotatePoint(p[i]);
  pc=m.rotatePoint(pc);
  pa=m.rotatePoint(pa);
}

Vector Surface::int3d(Vector p0, Vector p1, double f){
  return Vector((1-f)*p0.x()+f*p1.x(),
		(1-f)*p0.y()+f*p1.y(),
		(1-f)*p0.z()+f*p1.z());
}

Vector Surface::out3d(double f){
  Vector d0=p[0]-pc;
  Vector d1=p[1]-pc;
  Vector no=d0%d1;

  f *= (d0.magnitude()+d1.magnitude())/no.magnitude();
  no*=f;

  //  return pc+no;
  return no;
}


/* Line class */  
Line::Line(const Vector &p1,const Vector &p2){
  p[0]=p1;
  p[1]=p2;
} 

void Line::translate(const Vector &v){
  for(int i=0;i<2;++i) p[i]+=v;
}

void Line::rotate(RotationMatrix &m){
  for(int i=0;i<2;++i) p[i]=m.rotatePoint(p[i]);
}

Vector Line::int3d(Vector p0, Vector p1, double f){
  return Vector((1-f)*p0.x()+f*p1.x(),
		(1-f)*p0.y()+f*p1.y(),
		(1-f)*p0.z()+f*p1.z());
}

#define COMPASS_R 300
#define COMPASS_Z 0
#define COMPASS_W 2
#define COMPASS_L 20

CompassItem::CompassItem(PayloadPanel *parent,double azimuth)
  :Drawable(parent),m_azimuth(azimuth){
  SetOrientation(0);
}

CompassItem::~CompassItem(){}

void CompassItem::SetOrientation(double orient){
  double angle=Radian(orient-m_azimuth+22.5); // ADU5 fore is at phi sector 2 

  if(m_azimuth==0)
    marker=Surface(Vector( COMPASS_L,0,0),
		   Vector( 0, COMPASS_W*3,0),
		   Vector(-COMPASS_L/2, 0,0),
		   Vector( 0,-COMPASS_W*3,0));
  else
    marker=Surface(Vector( COMPASS_L/2,-COMPASS_W/2,0),
		   Vector( COMPASS_L/2, COMPASS_W/2,0),
		   Vector(-COMPASS_L/2, COMPASS_W/2,0),
		   Vector(-COMPASS_L/2,-COMPASS_W/2,0));

  m_reference=Vector(COMPASS_R*cos(angle),COMPASS_R*sin(angle),COMPASS_Z);

  double nx=cos(angle);
  double ny=sin(angle);
  double nz=0;
  Vector n(nx,ny,nz);
  double fx=0;
  double fy=0;
  double fz=1;
  Vector f(fx,fy,fz);

  RotationMatrix matrix(Vector(1,0,0),f,n,f);
  marker.rotate(matrix);
  marker.translate(m_reference);
}

void CompassItem::Draw(wxDC *dc){
  if(AviewOpts::Compass){
    if(AviewOpts::PayloadStyle==AviewOpts::Mesh){  // Draw wire mesh view
      dc->SetBrush(*wxTRANSPARENT_BRUSH);
      m_owner->DrawSurface(dc,&marker,*wxWHITE_PEN);
    }else{
      dc->SetBrush(*wxWHITE_BRUSH);
      m_owner->DrawSurface(dc,&marker,*wxWHITE_PEN);
    }
    m_owner->DrawText(dc,wxString()<<(int)m_azimuth,m_reference,*wxWHITE);
  }
}
