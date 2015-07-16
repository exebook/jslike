#include "sys/time.h"

#ifdef linux
long time1000() {
  timeval val;
  gettimeofday(&val, 0);
  val.tv_sec &= 0xffff;
  return val.tv_sec * 1000 + val.tv_usec / 1000;
}
#endif

#ifdef WINDOWS
int time1000() {
   return GetTickCount();
}
#endif
