/*
 * sort.cpp
 *
 * Functions for sorting file names and DB entries.
 */

#include "sort.h"
#include "DBControlItem.h"

// Sort function definitions
int extensionSortFunction(const wxString &first,const wxString &second){
  long int val1,val2;
  int dot1=first.Find('.',true);
  int dot2=second.Find('.',true);
  if(first.Mid(dot1+1).ToLong(&val1) && second.Mid(dot2+1).ToLong(&val2)){
    if(val1<val2) return -1;
    else if(val1>val2) return 1;
    else return 0;
  }
  return 0; // On unsuccessfull number extraction, return equality
}

int singleIndexSortFunction(const wxString &first,const wxString &second){
  long int val1,val2;
  int bar1=first.Find('_',true);
  int bar2=second.Find('_',true);
  int dot1=first.Mid(bar1+1).Find('.',false);
  int dot2=second.Mid(bar2+1).Find('.',false);
  if(first.Mid(bar1+1,dot1).ToLong(&val1) && 
     second.Mid(bar2+1,dot2).ToLong(&val2)){
    if(val1<val2) return -1;
    else if(val1>val2) return 1;
    else return 0;
  }
  return 0; // On unsuccessfull number extraction, return equality
}

int doubleIndexSortFunction(const wxString &first,const wxString &second){
  long int val1,val2;
  int sbar1=first.Find('_',true);
  int sbar2=second.Find('_',true);
  int fbar1=first.Mid(0,sbar1-1).Find('_',true);
  int fbar2=second.Mid(0,sbar2-1).Find('_',true);
  if(first.Mid(fbar1+1,sbar1-fbar1-1).ToLong(&val1) && 
     second.Mid(fbar2+1,sbar2-fbar2-1).ToLong(&val2)){
    if(val1<val2) return -1;
    else if(val1>val2) return 1;
    else{
      int dot1=first.Mid(sbar1+1).Find('.',false);
      int dot2=second.Mid(sbar2+1).Find('.',false);
      if(first.Mid(sbar1+1,dot1).ToLong(&val1) && 
	 second.Mid(sbar2+1,dot2).ToLong(&val2)){
	if(val1<val2) return -1;
	else if(val1>val2) return 1;
	else return 0;
      }
    }
  }
  return 0; // On unsuccessfull number extraction, return equality
}

int dbRefComp(const void *first,const void *second){
  dbRef a=*((dbRef*)first);  
  dbRef b=*((dbRef*)second);
  if(a.time<b.time) return -1;
  else if(a.time>b.time) return 1;
  else{
    if(a.us<b.us) return -1;
    else if(a.us>b.us) return 1;
    else return 0;
  }
}

int dbRefSearch(const void *first,const void *second){
  long key=*((long*)first);  
  dbRef loval=*((dbRef*)second);
  dbRef hival=*((dbRef*)(second)+1);
  
  if(key <= loval.time && key < hival.time) return -1;
  else if(key>loval.time && key>hival.time) return 1;
  else return 0;
}
