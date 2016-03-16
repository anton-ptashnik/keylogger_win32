#pragma once
#include "resource.h"
#include <locale>
#include <map>
#define MAX_LOADSTRING 100
#define ID_NI WM_USER + 1
#define WM_NIM WM_USER + 2
#define ID_NICM_ABOUT WM_USER + 3
#define ID_NICM_EXIT WM_USER + 4
#define HM_KEYBOARD WM_USER + 5
const wchar_t BOM{ 0xfeff };
typedef std::basic_string<TCHAR> tString;
typedef std::basic_ofstream<TCHAR> tOfstream;