/*
 * payload.h 
 * Payload display panel
 */

#ifndef _PAYLOADH_
#define _PAYLOADH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "payload.cpp"
#endif

#include <wx/panel.h>
#include <wx/dcclient.h>
#include "vector.h"
#include "rotationMatrix.h"
#include "graph.h"

// Projection and zoom variables; taken from xnecview.
static const double ini_phi=30;
static const double ini_theta=45;
static const double ini_zoom=1;
static const double ini_trX=0;
static const double ini_trY=0;

// Need to declare due to cross dependency of classes
class PayloadPanel;

// Class handling manipulation of line elements
class Line{
 public:
  Line(const Vector &p1=Vector(),const Vector &p2=Vector());
  Vector p[2];        /* ends */
  void translate(const Vector &v);
  void rotate(RotationMatrix &m);
 protected:
  /* calculate intermediate point (3D) between two ends; f=0..1 -> p0..p1 */
  Vector int3d(Vector p0, Vector p1, double f=0.5); 
};

// Class handling manipulation of 4-sided surfaces elements
class Surface{
 public:
  Surface(const Vector &p1=Vector(),
	  const Vector &p2=Vector(),
	  const Vector &p3=Vector(),
	  const Vector &p4=Vector());
  Vector p[4];        /* corners */
  Vector pc;          /* center */
  Vector pa;          /* tip of outward normal arrow */
  void translate(const Vector &v);
  void rotate(RotationMatrix &m);
 protected:
  /* calculate intermediate point (3D) between two corners; f=0..1 -> p0..p1 */
  Vector int3d(Vector p0, Vector p1, double f=0.5); 
  /* calculates surface normal vector, with f determining the length */
  Vector out3d(double f=1.);
};

// Abstract class handling antenna response display
class AntennaResponse{
 public:
  AntennaResponse();

  void SetValue(double val); // Colour set by value
  void SetRangeSpec(wxRangeSpec *rangeSpec);

 protected:
  double m_value;
  wxRangeSpec *m_rangeSpec;
  wxColour colour;
  static wxColour m_OutOfRange_Colour;
  static wxColour m_Off_Colour;
};

// Class handling QRH ridge element
class Ridge:AntennaResponse{
 public:
  Ridge(int iSide=0);
 private:
  friend class QRH;
  Surface side[5]; // 5 sides on each ridge
};

// Base class for all drawables
class Drawable{
 public:
  Drawable(PayloadPanel *parent);
  virtual ~Drawable();
  virtual void Draw(wxDC *dc)=0;
  
 protected:
  PayloadPanel *m_owner;
};

class Arrow:public Drawable{
 public:
  Arrow(PayloadPanel *parent,const Vector &start,const Vector &end,const Vector &arm1,const Vector &arm2);
  virtual ~Arrow();
  void Draw(wxDC *dc);
  Vector GetStart(){return element[0].p[0];}
  Vector GetEnd(){return element[0].p[1];}
  
 protected:
  Line element[3];
};

class CompassItem:public Drawable{
 public:
  CompassItem(PayloadPanel *parent,double azimuth); // Azimuth is relative to N
  virtual ~CompassItem();
  void SetOrientation(double orient);

  void Draw(wxDC *dc);
  Vector GetPosition(){return m_reference;}

 protected:
  double m_azimuth;
  Vector m_reference;
  Surface marker;
};

// Base class for all antennas
class Antenna:public Drawable{
 public:
  Antenna(PayloadPanel *parent);
  virtual ~Antenna();

  virtual void SetFeedPoint(const Vector &feed);
  virtual void SetDirection(const Vector &n,const Vector &f);

  inline Vector GetFeedPoint() const{return feedPoint;}
  inline Vector GetDirection() const{return dir;}
  inline Vector GetFacing() const{return face;}

  virtual void Draw(wxDC *dc);

 protected:
  Vector feedPoint; // Location of feedpoint
  Vector dir;       // Normal vector from feetpoint to mouth center, i.e. antenna direction vector
  Vector face;       // Normal vector from feetpoint to left relative to antenna , i.e. antenna facing vector
};

// Class handling Quad-Ridged Horn antennas
class QRH: public Antenna{
 public:
  QRH(PayloadPanel *parent,const Vector &feed=Vector(0,0,0),
      const Vector &n=Vector(1,0,0),const Vector &f=Vector(0,1,0));
  ~QRH();

  void SetFeedPoint(const Vector &feed);
  void SetDirection(const Vector &n,const Vector &f);

  inline void SetColour(const wxColour &colour){m_colour=colour;}
  inline void SetValue(double val,bool hv){//hv==0, horizontal; hv==1, vertical
    ridge[0+(int)hv].SetValue(val);
    ridge[2+(int)hv].SetValue(val);
  }
  inline void SetRangeSpec(wxRangeSpec *rangeSpec, bool hv){
    ridge[0+(int)hv].SetRangeSpec(rangeSpec);
    ridge[2+(int)hv].SetRangeSpec(rangeSpec);
  }
  void Draw(wxDC *dc);

 private:
  Surface feedBox[5]; // 5 sides of feed box
  Surface mouth[4];   // 4 sides of antenna mouth
  Ridge ridge[4];     // 4 ridges (0 & 2 are horizontal, 1 & 3 are vertical)
  wxColour m_colour;
};

// Class handling discone antennas
class Discone: public Antenna, public AntennaResponse{
 public:
  Discone(PayloadPanel *parent,const Vector &feed=Vector(0,0,0),
	  const Vector &n=Vector(1,0,0),const Vector &f=Vector(0,1,0));
  virtual ~Discone();

  void SetFeedPoint(const Vector &feed);
  void SetDirection(const Vector &n,const Vector &f);

  void Draw(wxDC *dc);

 private:
  Line wire[18]; // 18 line elements representing discone
};

// Class handling bicone antennas
class Bicone: public Antenna, public AntennaResponse{
 public:
  Bicone(PayloadPanel *parent,const Vector &feed=Vector(0,0,0),
	  const Vector &n=Vector(1,0,0),const Vector &f=Vector(0,1,0));
  virtual ~Bicone();

  void SetFeedPoint(const Vector &feed);
  void SetDirection(const Vector &n,const Vector &f);

  void Draw(wxDC *dc);

 private:
  Line wire[18]; // 18 line elements representing bicone
};

// Class handling bpwtie antennas
class Bowtie: public Antenna, public AntennaResponse{
 public:
  Bowtie(PayloadPanel *parent,const Vector &feed=Vector(0,0,0),
	 const Vector &n=Vector(1,0,0),const Vector &f=Vector(0,1,0));
  virtual ~Bowtie();

  void SetFeedPoint(const Vector &feed);
  void SetDirection(const Vector &n,const Vector &f);

  void Draw(wxDC *dc);

 private:
  Surface surf[2]; // 2 surfaces representing bowtie
};


// Event used in selection antenna for RF display 
BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxANTENNA_SELECTED,-1)
END_DECLARE_EVENT_TYPES()

class wxAntennaEvent: public wxNotifyEvent{
public:
  wxAntennaEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );
  
  inline void GetRFChannel(int *surf,int *chan){*surf = m_surf; *chan = m_chan;}
  inline void SetRFChannel(int surf,int chan ){ m_surf = surf; m_chan = chan; }

private:
  int m_surf;
  int m_chan;
};

// Panel will act as an observer
class PayloadPanel: public wxPanel{
 public:
  PayloadPanel(wxWindow* parent, wxWindowID id, wxString *labels);
  ~PayloadPanel();

  // Mouse event handler
  void OnSize( wxSizeEvent &event );
  void OnPaint( wxPaintEvent &event );
  void OnMouse( wxMouseEvent &event );
  void OnEraseBackground( wxEraseEvent &event );
  
  inline Vector GetEyevec() const{return eyevec;}

  void DrawText(wxDC *dc,const wxString &text,const Vector &v,const wxColour &colour); 
  void DrawLine(wxDC *dc,Line *l,const wxPen &pen); 
  void DrawSurface(wxDC *dc,Surface *s,const wxPen &pen); 

  // Functions initiating updates
  void SetRangeSpecs();
  void SetValue(wxGraphCurve *rfCurve, int i, int j);
  void SetTriggerPatternColour(int i, const wxColour &colour);
  void SetOrientation(double orient);

 protected:
  DECLARE_EVENT_TABLE()
  void Draw(wxDC *dc);
  wxPoint Project(const Vector &v); 
  void CalcProjection();
 private:
  wxArrayString m_antennaLabels;
  Antenna **m_antennaList;
  int Nant;
  int Nqrh,Nbic,Ndis;
  int Ncomp,Nnad;
  CompassItem **Compass;

  double phi, theta; // Angle defining observer direction
  double zoom;       // Zoom factor
  double trX,trY; // 2d translation, as fraction of window size 
  Vector Xproj;  // Projection matrix components
  Vector Yproj;
  Vector Zproj;  // Weight factors for calculating distance to observer
  double Xtr,Ytr; // Translation in pixels
  Vector eyevec; // tip of arrow pointing toward observer 

  // Mouse tracking variables; taken from xnecview
  bool dragging;
  int origx,origy; 
  int lastx,lasty; 

  DECLARE_CLASS(PayloadPanel);
};

struct Order{
  union Content{
    Drawable *p;
    int i;
  };
  Content index;
  double distance;
};

// Event declaration stuff
  
typedef void (wxEvtHandler::*wxAntennaEventFunction)(wxAntennaEvent&);

#if WXWIN_COMPATIBILITY_EVENT_TYPES
#define EVT_ANTENNA_SELECTED(id, fn) { wxANTENNA_SELECTED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) (wxAntennaEventFunction) & fn, (wxObject *) NULL },
#else
#define EVT_ANTENNA_SELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxANTENNA_SELECTED, id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction)  wxStaticCastEvent( wxAntennaEventFunction, & fn ), (wxObject *) NULL ),
#endif

#endif // _PAYLOADH_
