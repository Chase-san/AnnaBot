#ifndef _Thread_
#define _Thread_

#include <errno.h>

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

#ifdef WIN32
  #include "win32/Thread.h"
#else
  #include "posix/Thread.h"
#endif

#endif // !_Thread_
