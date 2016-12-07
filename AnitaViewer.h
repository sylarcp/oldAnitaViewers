/*
 * AnitaViewer.h
 */

#ifndef _ANITAVIEWER_H_
#define _ANITAVIEWER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AnitaViewer.cpp"
#endif

#include <wx/image.h>

/*
 * AnitaViewerApp class declaration
 */
class AnitaViewerApp: public wxApp{    
 public:
  // Constructor
  AnitaViewerApp();
  
  // Initialises the application
  virtual bool OnInit();
  
  // Called on exit
  virtual int OnExit();
};

/*
 * Application instance declaration 
 */

DECLARE_APP(AnitaViewerApp)

#endif
