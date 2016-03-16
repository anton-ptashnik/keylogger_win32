#pragma once
#include <Windows.h>
#ifdef DLLEXPORT
#define DLLAPI extern "C" __declspec(dllexport)
#else
#define DLLAPI extern "C" __declspec(dllimport)
#endif
DLLAPI BOOL SetHook(HWND, UINT);
DLLAPI BOOL UnsetHook();
LRESULT CALLBACK KeyboardFilter(int, WPARAM, LPARAM);