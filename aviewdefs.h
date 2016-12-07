/*
 * aviewdefs.h
 */

#include <includes/anitaStructures.h>

#ifndef _AVIEWDEFSH_
#define _AVIEWDEFSH_

#include <cmath>

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#define TITLE _T("AVIEW - Anita Viewer III")
#define DEFAULT_OPTIONS_FILE _T("default.arc")
#define SPLASH_FILE _T("aview_splash.png")

#define N_TEMP 50  //46
#define TEMP_PER_ROW 10

#define N_CURRENT 8
#define CURRENT_PER_COL 4

#define N_VOLTAGE 10
#define VOLTAGE_PER_COL 5

#define N_POWER 1
#define POWER_PER_COL 1

#define N_QUEUE 19
#define QUEUE_PER_ROW 10

#define N_DISK 8

#define N_AUX 2

#define N_SURF ACTIVE_SURFS // defined in anitaFlight.h
#define N_CHIP 4
#define N_CHAN 9
#define N_SCA 260
#define N_HITBUS 8

#define N_RFANT (N_SURF*(N_CHAN-1))
#define N_QRH 48
#define N_PHI 16

#define MAX_SPECS 10

// Some convenience templates 
#define ZERO 1e-6

/** Function returns the smaller of two parameters  */ 
template <class T> T Min(const T x, const T y) {return x<y?x:y;}
/** Function returns the larger of two parameters  */
template <class T> T Max(const T x, const T y) {return x>y?x:y;}
/** Function returns an angle converted into radians  */
template <class T> T Radian(const T x) {return x*M_PI/180.;}
/** Function returns an angle converted into degrees  */
template <class T> T Degree(const T x) {return x*180./M_PI;}
/** Function returns the square of a parameter  */
template <class T> T sqr(const T x) {return x*x;}  

// Structure for antenna to SURF channel mapping
struct SurfMapType{
  int h_surf;
  int h_chan;
  int v_surf;
  int v_chan;
  int phi;
};

#endif // _AVIEWDEFSH_
