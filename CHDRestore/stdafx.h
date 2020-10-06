// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>


// Config data
// Macros you want across all files

#define ERROR_FINDFILE_THREAD_TIMEDOUT		ERROR_SERVICE_REQUEST_TIMEOUT

// Max time in milliseconds a thread can take before it is terminated
#define MAX_THREAD_TIME				10000
#define MAX_FINDFILE_THREAD_TIME	5000
#define FINDFILE_THREAD_SLEEP		5

#if (defined(MAX_FINDFILE_THREAD_TIME) && MAX_FINDFILE_THREAD_TIME>MAX_THREAD_TIME)
#undef MAX_FINDFILE_THREAD_TIME
#define MAX_FINDFILE_THREAD_TIME MAX_THREAD_TIME
#endif



// TODO: reference additional headers your program requires here
