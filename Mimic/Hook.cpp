#include "Hook.h"

Hook::Hook()
{
    std::thread(&Hook::MsgLoop, this).detach();
}

Hook::~Hook()
{
}

tstring Hook::VirtualKeyCodeToString(UCHAR virtualKey)
{
    UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

    bool extended = false;
    bool capsLockOn = (GetKeyState(VK_CAPITAL) & 1);
    bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000);

    TCHAR szName[128];
    int result = 0;
    switch (virtualKey)
    {
    case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
    case VK_RCONTROL: case VK_RMENU:
    case VK_LWIN: case VK_RWIN: case VK_APPS:
    case VK_PRIOR: case VK_NEXT:
    case VK_END: case VK_HOME:
    case VK_INSERT: case VK_DELETE:
    case VK_DIVIDE:
    case VK_NUMLOCK:
        scanCode |= KF_EXTENDED;
        extended = true;
        result = GetKeyNameText(scanCode << 16, szName, 128);
        break;
    case VK_LCONTROL: case VK_LMENU: case VK_CAPITAL: case VK_LSHIFT: case VK_RSHIFT: case VK_TAB: case VK_ESCAPE: case VK_BACK: case VK_RETURN: case VK_SPACE:
    case 44: case 145: case 106: case 107: case 109: case 110: case 12: case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105:
    case VK_F1: case VK_F2: case VK_F3: case VK_F4: case VK_F5: case VK_F6: case VK_F7: case VK_F8: case VK_F9: case VK_F10: case VK_F11: case VK_F12:
        extended = true;
        result = GetKeyNameText(scanCode << 16, szName, 128);
        break;
    default:
        result = GetKeyNameText(scanCode << 16, szName, 128);
    }

    //CHECK WHEN FN PRESSED
    //CHECK KEY COMBINATIONS

    if (((capsLockOn == 0 && shiftPressed == 0) || (capsLockOn && shiftPressed)) && !extended)
        CharLowerW(szName);

    if (extended)
    {
        std::wstring str = L"{";
        str += szName;
        str += L"}";
        wcscpy_s(szName, str.c_str());
    }

    /*if (result == 0)
        throw std::system_error(std::error_code(GetLastError(), std::system_category()),
            "WinAPI Error occured.");*/
    
    return szName;
}

LRESULT Hook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && Global::recording)
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            MM_Event* e = new MM_Event({ TimeUtils::getUnixTimestamp(), 0, EVENT_TYPE::KEYDOWN, p->vkCode });
            if (Global::pressedKeys.find(p->vkCode) == Global::pressedKeys.end())
            {
                Global::recordBuf.emplace_back(e);
                Global::pressedKeys.insert(p->vkCode);
                std::cout << "KEYDOWN: " << p->vkCode << std::endl;
            }

            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            auto it = std::find_if(Global::recordBuf.rbegin(), Global::recordBuf.rend(),
                [&](const MM_Event* mm_event) {
                    return std::holds_alternative<WORD>(mm_event->data) && std::get<WORD>(mm_event->data) == p->vkCode;
                });

            if (it != Global::recordBuf.rend())
                (*it)->end = TimeUtils::getUnixTimestamp();

            Global::pressedKeys.erase(p->vkCode);
            std::cout << "KEYUP: " << p->vkCode << " - (start)" << (*it)->start << " | (end)" << (*it)->end << std::endl;
            break;
        }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT Hook::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && Global::recording)
    {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

        switch (wParam)
        {
        case WM_LBUTTONDOWN:
        {
            MM_Event* e = new MM_Event({ TimeUtils::getUnixTimestamp(), 0, EVENT_TYPE::LCLICKDOWN, pMouseStruct->pt });
            Global::recordBuf.emplace_back(e);
            std::cout << "LBDOWN: " << std::endl;
            break;
        }
        case WM_LBUTTONUP:
        {
            auto it = std::find_if(Global::recordBuf.rbegin(), Global::recordBuf.rend(),
                [&](const MM_Event* mm_event) {
                    return mm_event->type == EVENT_TYPE::LCLICKDOWN;
                });

            if (it != Global::recordBuf.rend())
                (*it)->end = TimeUtils::getUnixTimestamp();

            std::cout << "LBUP: " << std::endl;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            MM_Event* e = new MM_Event({ TimeUtils::getUnixTimestamp(), 0 , EVENT_TYPE::RCLICKDOWN, pMouseStruct->pt });
            Global::recordBuf.emplace_back(e);
            std::cout << "RBDOWN: " << std::endl;
            break;
        }
        case WM_RBUTTONUP:
        {
            auto it = std::find_if(Global::recordBuf.rbegin(), Global::recordBuf.rend(),
                [&](const MM_Event* mm_event) {
                    return mm_event->type == EVENT_TYPE::RCLICKDOWN;
                });

            if (it != Global::recordBuf.rend())
                (*it)->end = TimeUtils::getUnixTimestamp();

            std::cout << "RBUP: " << std::endl;
            break;
        }
        case WM_MOUSEMOVE:
        {
            MM_Event* e = new MM_Event({ 0, 0, EVENT_TYPE::MOVE, pMouseStruct->pt });
            Global::recordBuf.emplace_back(e);

            break;
        }
        }
    }

    // Pasar el evento al siguiente hook en la cadena
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Hook::MsgLoop()
{
    // Install the low-level keyboard & mouse hooks
    HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

    // Keep this app running until we're told to stop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {    //this while loop keeps the hook
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}