#include "decls.h"
extern HINSTANCE hInst;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason_for_call, LPVOID reserved)
{
	if (reason_for_call == DLL_PROCESS_ATTACH)
		hInst = hModule;
	return TRUE;
}