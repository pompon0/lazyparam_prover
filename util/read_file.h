#ifndef READ_FILE_H_
#define READ_FILE_H_

#include <cassert>
#include <cstdio>
#include <string>
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include "util/types.h"
#include "util/string.h"
#include "util/log.h"

namespace util
{
  inline bool file_exists(str path)
  { return access(path.c_str(),F_OK)!=-1; }

  inline Bytes read_file(FILE *f)
  {
    // WARNING: in assert(expr), expr should have no side effects,
    // since it will be cut out if NDEBUG is defined
    // (cmake sets this flag)
    if(fseek(f,0,SEEK_END)) error("fseek"); //possible overflow
    long size = ftell(f); if(size==-1) error("ftell");
    if(fseek(f,0,SEEK_SET)) error("fseek");
    Bytes content(size);
    if(fread(content.data(),size,1,f)!=1) error("fread");
    return content;
  }

  inline Bytes read_file(str path)
  {
    FILE *f = fopen(path.c_str(),"r"); if(!f) error("fopen(%) = %",path,strerror(errno));
    auto s = read_file(f);
    fclose(f);
    return s;
  }

  inline void write_file(str path, const Bytes &data)
  {
    FILE *f = fopen(path.c_str(),"w"); if(!f) error("fopen(%) = %",path,strerror(errno));
    if(fwrite(data.data(),data.size(),1,f)!=1) error("fwrite");
    if(fclose(f)) error("fclose");
  }
}

#endif  // READ_FILE_H_
