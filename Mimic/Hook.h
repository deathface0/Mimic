#pragma once

#include <Windows.h>
#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "time_utils/time_utils.h"
#include "Global.h"

class Hook
{
public:
	Hook();
	~Hook();

private:
	static tstring VirtualKeyCodeToString(UCHAR virtualKey);

	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	void MsgLoop();
};