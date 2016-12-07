/*
 * sort.h
 */

#include <wx/string.h>

int extensionSortFunction(const wxString &first,const wxString &second);
int singleIndexSortFunction(const wxString &first,const wxString &second);
int doubleIndexSortFunction(const wxString &first,const wxString &second);
int dbRefComp(const void *first,const void *second);
int dbRefSearch(const void *first,const void *second);
