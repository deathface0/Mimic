#include "Hook.h"

std::chrono::high_resolution_clock::time_point Hook::lastEventTimestamp;

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
        std::chrono::high_resolution_clock::time_point mm_event_start = std::chrono::high_resolution_clock::now();
        auto btw_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_start - lastEventTimestamp);
        Hook::lastEventTimestamp = mm_event_start;

        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        auto it = vk_map_rev.find(p->vkCode);
        char key = static_cast<char>(MapVirtualKey(p->vkCode, MAPVK_VK_TO_CHAR));

        Json::Value args;
        Json::CharReaderBuilder readerBuilder;
        std::string errors;

        std::string jsonString = it != vk_map_rev.end() ? fmt::format(R"({{"key": "{}"}})", it->second) : fmt::format(R"({{"key": "{}"}})", key);
        std::istringstream jsonStream(jsonString);
        if (!Json::parseFromStream(readerBuilder, jsonStream, &args, &errors)) {
            std::cerr << "Error parsing JSON: " << errors << std::endl;
            return -1;
        }

        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            bool isCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
            bool upper = isShiftPressed ^ isCapsLockOn;

            Instruction* e = new Instruction({ EVENT_TYPE::VKEYDOWN, args });
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
            Instruction* e = new Instruction({ EVENT_TYPE::VKEYUP, args });
            Global::recordBuf.emplace_back(e);
            Global::pressedKeys.erase(p->vkCode);

            std::cout << "KEYUP: " << p->vkCode << std::endl;
            break;
        }
        }
        std::chrono::high_resolution_clock::time_point mm_event_end = std::chrono::high_resolution_clock::now();

        auto in_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_end - mm_event_start);
        auto elapsed = btw_elapsed - in_elapsed;
        //std::cout << "ns: " << btw_elapsed.count() << " -- " << in_elapsed.count() << std::endl;

        jsonString = fmt::format(R"({{"ns": {}}})", elapsed.count());
        std::istringstream jsonStream2(jsonString);
        if (!Json::parseFromStream(readerBuilder, jsonStream2, &args, &errors)) {
            std::cerr << "Error parsing JSON: " << errors << std::endl;
            return -1;
        }

        Instruction* e = new Instruction({ EVENT_TYPE::NSSLEEP, args });
        Global::recordBuf.emplace_back(e);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT Hook::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && Global::recording)
    {
        std::chrono::high_resolution_clock::time_point mm_event_start = std::chrono::high_resolution_clock::now();
        auto btw_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_start - lastEventTimestamp);
        Hook::lastEventTimestamp = mm_event_start;

        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

        Json::Value args;
        Json::CharReaderBuilder readerBuilder;
        std::string errors;

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

            std::string jsonString = fmt::format(R"({{"id": {}}})", button);
            std::istringstream jsonStream(jsonString);
            if (!Json::parseFromStream(readerBuilder, jsonStream, &args, &errors)) {
                std::cerr << "Error parsing JSON: " << errors << std::endl;
                return -1;
            }

            Instruction* e = new Instruction({ EVENT_TYPE::EXTRACLICKDOWN, args });
            Global::recordBuf.emplace_back(e);

            std::cout << "EXTRADOWN: " << std::endl;
            break;
        }
        case WM_XBUTTONUP:
        {
            DWORD button = pMouseStruct->mouseData >> 16;

            std::string jsonString = fmt::format(R"({{"id": {}}})", button);
            std::istringstream jsonStream(jsonString);
            if (!Json::parseFromStream(readerBuilder, jsonStream, &args, &errors)) {
                std::cerr << "Error parsing JSON: " << errors << std::endl;
                return -1;
            }

            Instruction* e = new Instruction({ EVENT_TYPE::EXTRACLICKUP, args });
            Global::recordBuf.emplace_back(e);

            std::cout << "EXTRAUP: " << std::endl;
            break;
        }
        case WM_MOUSEMOVE:
        {
            std::string jsonString = fmt::format(R"({{"x": {}, "y": {}}})", pMouseStruct->pt.x, pMouseStruct->pt.y);
            std::istringstream jsonStream(jsonString);
            if (!Json::parseFromStream(readerBuilder, jsonStream, &args, &errors)) {
                std::cerr << "Error parsing JSON: " << errors << std::endl;
                return -1;
            }

            Instruction* e2 = new Instruction({ EVENT_TYPE::MOVE, args });
            Global::recordBuf.emplace_back(e2);
            
            break;
        }
        }
        std::chrono::high_resolution_clock::time_point mm_event_end = std::chrono::high_resolution_clock::now();

        auto in_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_end - mm_event_start);
        auto elapsed = btw_elapsed - in_elapsed;

        std::string jsonString = fmt::format(R"({{"ns": {}}})", elapsed.count());
        //std::cout << jsonString << std::endl;

        std::istringstream jsonStream(jsonString);
        if (!Json::parseFromStream(readerBuilder, jsonStream, &args, &errors)) {
            std::cerr << "Error parsing JSON: " << errors << std::endl;
            return -1;
        }

        Instruction* e = new Instruction({ EVENT_TYPE::NSSLEEP, args });
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