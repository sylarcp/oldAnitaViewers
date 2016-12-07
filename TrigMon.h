/*
 * TrigMon.h
 */

#ifndef _TRIGMON_H_
#define _TRIGMON_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TrigMon.cpp"
#endif

#include <wx/image.h>

/*
 * TrigMonApp class declaration
 */
class TrigMonApp: public wxApp{    
 public:
  // Constructor
  TrigMonApp();
  
  // Initialises the application
  virtual bool OnInit();
  
  // Called on exit
  virtual int OnExit();
};

/*
 * Application instance declaration 
 */

DECLARE_APP(TrigMonApp)

#endif //  _TRIGMON_H_
