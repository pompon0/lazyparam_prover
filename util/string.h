#ifndef UTIL_STRING_H_
#define UTIL_STRING_H_

#include <sstream>
#include "util/types.h"

namespace util
{
  inline str to_str(const Bytes &d){ return str(d.begin(),d.end()); }
  inline Bytes to_bytes(const str &s){ return Bytes(s.begin(),s.end()); }
  
  inline bool has(char c, const str &s)
  { for(char x : s) if(x==c) return 1; return 0; }
  
  inline arr<str> split(const str &s, const str &sep = " \t\n\r\v")
  {
    arr<str> out;
    size_t i=0;
    for(size_t j=0; j<s.size(); ++j) if(has(s[j],sep))
    { out.push_back(s.substr(i,j-i)); i = j+1; }
    out.push_back(s.substr(i));
    return out;
  }

  inline arr<str> strip(arr<str> v)
  {
    size_t i = 0;
    for(str &s : v) if(s.size()) v[i++].swap(s);
    v.resize(i);
    return v;
  }

  inline str join(str sep, const arr<str> &A)
  {
    if(!A.size()) return "";
    str r = A[0];
    for(size_t i=1; i<A.size(); ++i) r += sep+A[i];
    return r;
  }

  template<typename T> inline str to_str(const T &v)
  { std::stringstream ss; ss << v; return ss.str(); }

  template<typename ...Args> inline str fmt(const str &format_str, Args ...args)
  {
    arr<str> vs{to_str(args)...};
    std::stringstream ss; size_t i = 0;
    for(char c : format_str) if(c!='%') ss << c; else ss << vs.at(i++);
    return ss.str();
  }
}

#endif  // UTIL_STRING_H_
