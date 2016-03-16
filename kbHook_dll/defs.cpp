#define DLLEXPORT
#include "decls.h"
#pragma data_seg(".sdata")
HHOOK hHook = NULL;
HWND hParentWnd = NULL;
UINT notification = 0;
DWORD thrId = 0;
DWORD arr[2]{0, 0};
HWND fWnd = 0;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.sdata,RWS")
HINSTANCE hInst;

DLLAPI BOOL SetHook(HWND hWnd, UINT nMsg)
{
	if (hWnd == 0)
		return FALSE;
	hParentWnd = hWnd;
	notification = nMsg;
	//hHook = SetWindowsHookEx(WH_GETMESSAGE, KeyboardFilter, hInst, NULL);
	hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardFilter, hInst, NULL);
	return hHook == 0 ? FALSE : TRUE;
}
DLLAPI BOOL UnsetHook()
{
	return UnhookWindowsHookEx(hHook);
}
LRESULT CALLBACK KeyboardFilter(int code, WPARAM wParam, LPARAM lParam)
{
	if (code < 0)
		return CallNextHookEx(hHook, code, wParam, lParam);
	//if (HIWORD(lParam) & KF_UP) {
	//	switch (wParam) {
	//	case VK_SHIFT:
	//		upperSym = false;
	//	case VK_CAPITAL:
	//		break;
	//	}
	//	;
	//}
	//else {
	//	switch (wParam) {
	//	case VK_SHIFT:
	//		if (!upperSym)
	//			upperSym = true;
	//	case VK_CAPITAL:
	//		break;
	//	default:
	//		PostMessage(hParentWnd, notification, wParam, GetCurrentThreadId());
	//		//PostMessage(hParentWnd, notification, wParam, lParam);
	//	}
	//}
	
	/*MSG* msg{ reinterpret_cast<MSG*>(lParam) };
	if (lParam && (msg->message == WM_CHAR || msg->message == WM_UNICHAR)) {

		PostMessage(hParentWnd, notification, msg->wParam, 0);
	}*/
	if (!(HIWORD(lParam) & KF_UP)) {
		switch (wParam)
		{
		case VK_CAPITAL:
			PostMessage(hParentWnd, notification, wParam, NULL);
		case VK_CONTROL:
		case VK_SCROLL:
		case VK_PAUSE:
		case VK_SHIFT:
		case VK_MENU:
			break;
		default:
			arr[1] = GetCurrentThreadId();
			arr[0] = lParam;
			PostMessage(hParentWnd, notification, wParam, LPARAM(arr));
		}
	}
	return CallNextHookEx(hHook, code, wParam, lParam);
}