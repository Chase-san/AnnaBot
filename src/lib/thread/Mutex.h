#ifndef _Mutex_
#define _Mutex_

#include <errno.h>

#ifdef WIN32
  #include "win32/Mutex.h"
#else
  #include "posix/Mutex.h"
#endif

#endif // !_Mutex_
