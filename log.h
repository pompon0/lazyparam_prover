#ifndef LOG_H_
#define LOG_H_

#include "types.h"
#include "util/string.h"
#include <ctime>
#include <unistd.h>
#include <iostream>

struct log {

  inline static str now(str tf = "%F %T ")
  {
    char B[25]; time_t t; time(&t);
    strftime(B,sizeof B,tf.c_str(),localtime(&t));
    return B;
  }

  template<typename ...Args> inline static void info(const str &s, Args ...args)
  { std::cerr << now() << util::fmt(s,args...) << std::endl; }

  template<typename ...Args> inline static void error(const str &s, Args ...args)
  {
    std::cerr << now() << "ERROR: " << util::fmt(s,args...) << std::endl;
    _exit(EXIT_FAILURE);
  }
};

#endif // LOG_H_
