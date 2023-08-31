#include <windows.h>

#include "mainF.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
		LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		// DLL is being loaded into the process
		// Perform initialization here
		HANDLE hThread = CreateThread(NULL,0,fmcConsoleMainThread,NULL,0,NULL);
		if(hThread)
			CloseHandle(hThread);
		break;
	}
	return TRUE;
}
