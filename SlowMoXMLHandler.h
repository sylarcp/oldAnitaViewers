/* 
 * SlowMoXMLHandler.h
 */

#ifndef _SLOWMOXMLHANDLERH_
#define _SLOWMOXMLHANDLERH_

#include "BaseXMLHandler.h"

class SlowMoXMLHandler:public BaseXMLHandler{
 public:
  SlowMoXMLHandler();
  ~SlowMoXMLHandler();

  static void xmlSlowMoStart(void *data, const char *el, const char **attr);
  static void xmlSlowMoEnd(void *data, const char *el);
};


#endif //  _SLOWMOXMLHANDLERH_

