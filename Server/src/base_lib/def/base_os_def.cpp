#include "def/base_os_def.h"
#if defined(_WIN32) || defined(_WIN64)
#include <WinBase.h>
#include <synchapi.h>
#include <handleapi.h>

void usleep(unsigned long usec)
{
	Sleep(usec / 1000);
}
#endif