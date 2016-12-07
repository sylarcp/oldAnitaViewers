/* 
 * TrigMonXMLHandler.h
 */

#ifndef _TRIGMONXMLHANDLERH_
#define _TRIGMONXMLHANDLERH_

#include "BaseXMLHandler.h"

class TrigMonXMLHandler:public BaseXMLHandler{
 public:
  TrigMonXMLHandler();
  ~TrigMonXMLHandler();

  static void xmlTrigMonStart(void *data, const char *el, const char **attr);
  static void xmlTrigMonEnd(void *data, const char *el);
};


#endif //  _TRIGMONXMLHANDLERH_

