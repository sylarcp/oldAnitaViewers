/* 
 * DBControlItem.h
 */

#ifndef _DBCONTROLITEMH_
#define _DBCONTROLITEMH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "DBControlItem.cpp"
#endif

#include "BaseControlItem.h"

typedef struct{
  long time;
  long us;
  unsigned long nbuf;
} dbRef;

// Event used to load files off disk
BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(wxREQUEST_PACKET,-1)
END_DECLARE_EVENT_TYPES()

class DBControlItem:public BaseControlItem{

  DECLARE_DYNAMIC_CLASS( DBControlItem )

 public:
  DBControlItem();
  DBControlItem(wxWindow *parent, int type);
  ~DBControlItem();
  void Create(wxWindow *parent, int type);

  void Next(wxCommandEvent &event);
  void Prev(wxCommandEvent &event);
  void Last(wxCommandEvent &event);
  void Select(wxCommandEvent &event);
  void TimeSelect(wxCommandEvent &event);
  void SetList(void *list,int n);
  void SetIndex(void *data);
  void MarkIndex();

 protected:
  static int GetHourSelection();
  static int GetMinuteSelection();

 private:
  int nList;
  dbRef *m_list;
  dbRef m_oldIndex;

};

#endif // _DBCONTROLITEMH_
