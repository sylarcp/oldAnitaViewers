/*
 * FileHandler.h
 */

#ifndef _FILEHANDLERH_
#define _FILEHANDLERH_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "FileHandler.cpp"
#endif

#include <wx/filename.h>
#include <wx/zstream.h>
#include <wx/file.h>
#include <zlib.h>

class FileHandler{
 public:
  FileHandler();
  ~FileHandler();

  bool MultiRead(wxFile &file,void *target, int size,int retries=1);
  bool ReadFromFile(wxFile &file,void *target, unsigned int size, int n=0);
  bool OpenGZ(wxFile &file);
  void CloseGZ();
  bool Eof();
  bool ForwardGZ(int n);
  bool ReadGZ(void *target,int size);

  // Deprecated
  bool ReadFlatStream(wxFile &file,void *target, unsigned int size);
  bool ReadZippedStream(wxZlibInputStream &iz,void *target, unsigned int size);

 private:
  gzFile m_gz;
};

#endif // _FILEHANDLERH_
