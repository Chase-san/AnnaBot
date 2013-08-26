#ifndef _Semaphore_
#define _Semaphore_

#include <errno.h>

#ifdef WIN32
  #include "win32/Semaphore.h"
#else
  #include "posix/Semaphore.h"
#endif

#endif // !_Semaphore_
