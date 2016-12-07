/*
 * SlowMo.h
 */

#ifndef _SLOWMO_H_
#define _SLOWMO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SlowMo.cpp"
#endif

#include <wx/image.h>

/*
 * SlowMoApp class declaration
 */
class SlowMoApp: public wxApp{    
 public:
  // Constructor
  SlowMoApp();
  
  // Initialises the application
  virtual bool OnInit();
  
  // Called on exit
  virtual int OnExit();
};

/*
 * Application instance declaration 
 */

DECLARE_APP(SlowMoApp)

#endif //  _SLOWMO_H_
