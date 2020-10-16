
/*
 * Description: Holds System utilities such as registering a signal_callback_handler for catching ctrl-C
 */

#include "stdafx.h"

#include "SystemUtilities.h"

#include <iostream>
#include <cstdlib>
#include <signal.h>

int g_signum = 0;

void signal_callback_handler(int signum) {
	_tprintf(_T("Caught signal %d \n"), signum);
	// Terminate program
	g_signum = signum;
}

int is_process_terminated()
{
	return g_signum;
}