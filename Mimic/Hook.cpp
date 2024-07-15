#include "Hook.h"

std::chrono::steady_clock::time_point Hook::lastEventTimestamp;

Hook::Hook()
{
    std::thread(&Hook::MsgLoop, this).detach();
}

Hook::~Hook()
{
}

LRESULT Hook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && Global::recording)
    {
        std::chrono::steady_clock::time_point mm_event_start = std::chrono::steady_clock::now();
        auto btw_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_start - lastEventTimestamp);
        Hook::lastEventTimestamp = mm_event_start;

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
        std::chrono::steady_clock::time_point mm_event_end = std::chrono::steady_clock::now();

        auto in_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_end - mm_event_start);
        auto elapsed = btw_elapsed - in_elapsed;
        //std::cout << "ns: " << btw_elapsed.count() << " -- " << in_elapsed.count() << std::endl;

        Instruction* e = new Instruction({ EVENT_TYPE::NSSLEEP, {std::to_string(elapsed.count())} });
        Global::recordBuf.emplace_back(e);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT Hook::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && Global::recording)
    {
        std::chrono::steady_clock::time_point mm_event_start = std::chrono::steady_clock::now();
        auto btw_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_start - lastEventTimestamp);
        Hook::lastEventTimestamp = mm_event_start;

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
        case WM_XBUTTONDOWN:
        {
            DWORD button = pMouseStruct->mouseData >> 16;
            Instruction* e = new Instruction({ EVENT_TYPE::EXTRACLICKDOWN, {std::to_string(button)} });
            Global::recordBuf.emplace_back(e);

            std::cout << "EXTRADOWN: " << std::endl;
            break;
        }
        case WM_XBUTTONUP:
        {
            DWORD button = pMouseStruct->mouseData >> 16;
            Instruction* e = new Instruction({ EVENT_TYPE::EXTRACLICKUP, {std::to_string(button)} });
            Global::recordBuf.emplace_back(e);

            std::cout << "EXTRAUP: " << std::endl;
            break;
        }
        case WM_MOUSEMOVE:
        {
            Instruction* e2 = new Instruction({ EVENT_TYPE::MOVE, {std::to_string(pMouseStruct->pt.x), std::to_string(pMouseStruct->pt.y)} });
            Global::recordBuf.emplace_back(e2);
            
            break;
        }
        }
        std::chrono::steady_clock::time_point mm_event_end = std::chrono::steady_clock::now();

        auto in_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_end - mm_event_start);
        auto elapsed = btw_elapsed - in_elapsed;
        //std::cout << "ns: " << btw_elapsed.count() << " -- " << in_elapsed.count() << std::endl;

        Instruction* e = new Instruction({ EVENT_TYPE::NSSLEEP, {std::to_string(elapsed.count())} });
        Global::recordBuf.emplace_back(e);
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Hook::MsgLoop()
{
    HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

    MSG msg;
    time_t mm_stimestamp, mm_etimestamp;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyboardHook);
    UnhookWindowsHookEx(hMouseHook);
}