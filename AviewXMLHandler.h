/* 
 * AviewXMLHandler.h
 */

#ifndef _AVIEWXMLHANDLERH_
#define _AVIEWXMLHANDLERH_

#include "BaseXMLHandler.h"

class AviewXMLHandler:public BaseXMLHandler{
 public:
  AviewXMLHandler();
  ~AviewXMLHandler();

  static void xmlAviewStart(void *data, const char *el, const char **attr);
  static void xmlAviewEnd(void *data, const char *el);
};

#endif // _AVIEWXMLHANDLERH_
