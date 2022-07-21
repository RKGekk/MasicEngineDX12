#pragma once

#include <Windows.h>

struct OSMessageData {
	HWND hwnd;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
};