#include "Hook.h"

time_t Hook::lastEventTimestamp = 0;

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
        time_t mm_end = TimeUtils::getUnixTimestamp();
        time_t duration = TimeUtils::TimeElapsed(Hook::lastEventTimestamp, mm_end);
        Hook::lastEventTimestamp = mm_end;

        Instruction* e = new Instruction({ EVENT_TYPE::SLEEP, {std::to_string(duration)} });
        Global::recordBuf.emplace_back(e);

        //std::cout << duration << std::endl;

        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            Instruction* e = new Instruction({ EVENT_TYPE::KEYDOWN, {std::to_string(p->vkCode)} });
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
            Instruction* e = new Instruction({ EVENT_TYPE::KEYUP, {std::to_string(p->vkCode)} });
            Global::recordBuf.emplace_back(e);
            Global::pressedKeys.erase(p->vkCode);
            std::cout << "KEYUP: " << p->vkCode << std::endl;
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
        time_t mm_end = TimeUtils::getUnixTimestamp();
        time_t duration = TimeUtils::TimeElapsed(Hook::lastEventTimestamp, mm_end);
        Hook::lastEventTimestamp = mm_end;

        Instruction* e = new Instruction({ EVENT_TYPE::SLEEP, {std::to_string(duration)} });
        Global::recordBuf.emplace_back(e);

        //std::cout << duration << std::endl;

        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

        switch (wParam)
        {
        case WM_LBUTTONDOWN:
        {
            Instruction* e = new Instruction({ EVENT_TYPE::LCLICKDOWN, {} });
            Global::recordBuf.emplace_back(e);

            std::cout << "LBDOWN: " << std::endl;
            break;
        }
        case WM_LBUTTONUP:
        {
            Instruction* e = new Instruction({ EVENT_TYPE::LCLICKUP, {} });
            Global::recordBuf.emplace_back(e);

            std::cout << "LBUP: " << std::endl;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            Instruction* e = new Instruction({ EVENT_TYPE::RCLICKDOWN, {} });
            Global::recordBuf.emplace_back(e);
            std::cout << "RBDOWN: " << std::endl;
            break;
        }
        case WM_RBUTTONUP:
        {
            Instruction* e = new Instruction({ EVENT_TYPE::RCLICKUP, {} });
            Global::recordBuf.emplace_back(e);

            std::cout << "RBUP: " << std::endl;
            break;
        }
        case WM_MOUSEMOVE:
        {
            time_t mm_end = TimeUtils::getUnixTimestamp();
            time_t duration = TimeUtils::TimeElapsed(Hook::lastEventTimestamp, mm_end);
            Hook::lastEventTimestamp = mm_end;

            Instruction* e1 = new Instruction({ EVENT_TYPE::SLEEP, {std::to_string(duration)} });
            Global::recordBuf.emplace_back(e1);

            Instruction* e2 = new Instruction({ EVENT_TYPE::MOVE, {std::to_string(pMouseStruct->pt.x), std::to_string(pMouseStruct->pt.y)} });
            Global::recordBuf.emplace_back(e2);

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
    time_t mm_stimestamp, mm_etimestamp;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}