#include "stdafx.h"
#include "angel_wait.h"


#if defined(_WIN32)||(_WIN64)

#ifdef __cplusplus
extern "C"
{
#endif
	pid_t waitpid(pid_t a_iPid, int* a_piStatus, int a_iOptions)
	{
		DWORD ret = 0;
		HWND h = ::GetTopWindow(0);
		while (h)
		{
			DWORD pid;
			DWORD dwTheardId = ::GetWindowThreadProcessId(h, &pid);
			if (pid == a_iPid)
			{
				return WaitForSingleObject(h, INFINITE);
			}
			h = ::GetNextWindow(h, GW_HWNDNEXT);
		}
		return 0;
	}
#ifdef __cplusplus

}
#endif
#endif

























