/* 
 * FileHandler.cpp
 *
 * Class that provides function for opening and reading from compressed files.
 * It also handles uncompressed variety.
 */

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "FileHandler.h"
#endif

#include <wx/wfstream.h>
#include <wx/utils.h>
#include "FileHandler.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

FileHandler::FileHandler(){}
FileHandler::~FileHandler(){}

bool FileHandler::MultiRead(wxFile &file,void *target,int size,int retries){
  int n=0;
  while((file.Read(target,size) != size) && n<=retries){
    file.Seek( 0, wxFromStart ) ;
    if(++n>retries) return false;
    /* in case of reading an incomplete file, read again after sleep. */
    wxMilliSleep(20);  /* wait a while */
  }
  return true;
}

bool FileHandler::ReadFromFile(wxFile &file,void *target,unsigned int size,int n){
  gzFile gz=gzdopen(dup(file.fd()),"rb");
  if(!gz) return false;
  // Adjust position
  unsigned int seekloc;
  if((seekloc=gzseek(gz,size*n,SEEK_SET))!=size*n){
    gzclose(gz);
    return false;
  }
  // Read data
  int r;
  if((r=gzread(gz,target,size))!=(int)size){
    gzclose(gz);
    return false;
  }
  gzclose(gz);
  return true;
}

bool FileHandler::OpenGZ(wxFile &file){
  m_gz=gzdopen(dup(file.fd()),"rb");
  if(!m_gz) return false;
  else return true;
}

void FileHandler::CloseGZ(){
  gzclose(m_gz);
}

bool FileHandler::Eof(){
  return gzeof(m_gz);
}

bool FileHandler::ForwardGZ(int n){
  int newloc=gztell(m_gz)+n;
  int seekloc=gzseek(m_gz,newloc,SEEK_SET);
  return seekloc==newloc;
}

bool FileHandler::ReadGZ(void *target,int size){
  return gzread(m_gz,target,size)==size;
}

bool FileHandler::ReadFlatStream(wxFile &file,void *target,unsigned int size){
  if(file.Read(target,size)==(int)size) return true;
  else return false;
}

bool FileHandler::ReadZippedStream(wxZlibInputStream &iz,void *target,unsigned int size){
  if(iz.IsOk() && iz.Read(target,size).LastRead()==size) return true;
  else return false;
}
