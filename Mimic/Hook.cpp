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
        bool isVK = it != vk_map_rev.end();

        Json::Value command;
        Json::CharReaderBuilder readerBuilder;
        std::string errors;

        std::string key_str = isVK ? it->second : std::string(1, key);

        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            command["type"] = isVK ? "VKEYDOWN" : "SCKEYDOWN";
            command["args"]["key"] = key_str;

            if (Global::pressedKeys.find(p->vkCode) == Global::pressedKeys.end())
            {
                Global::pressedKeys.insert(p->vkCode);
            }

            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            command["type"] = isVK ? "VKEYUP" : "SCKEYUP";
            command["args"]["key"] = key_str;

            Global::pressedKeys.erase(p->vkCode);
            break;
        }
        }
        std::chrono::high_resolution_clock::time_point mm_event_end = std::chrono::high_resolution_clock::now();

        auto in_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_end - mm_event_start);
        auto elapsed = btw_elapsed - in_elapsed;

        // Append the kb event
        Global::recordedJson["commands"].append(command);

        // Create and append the sleep event
        command.clear();
        command["type"] = "NSSLEEP";
        command["args"]["ns"] = Json::Value::UInt64(elapsed.count());
        Global::recordedJson["commands"].append(command);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT Hook::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && Global::recording || true)
    {
        std::chrono::high_resolution_clock::time_point mm_event_start = std::chrono::high_resolution_clock::now();
        auto btw_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_start - lastEventTimestamp);
        Hook::lastEventTimestamp = mm_event_start;
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

        Json::Value command;

        switch (wParam)
        {
        case WM_LBUTTONDOWN:
            command["type"] = "LCLICKDOWN";
            command["args"] = Json::Value(Json::objectValue);
            break;

        case WM_LBUTTONUP:
            command["type"] = "LCLICKUP";
            command["args"] = Json::Value(Json::objectValue);
            break;

        case WM_RBUTTONDOWN:
            command["type"] = "RCLICKDOWN";
            command["args"] = Json::Value(Json::objectValue);
            break;

        case WM_RBUTTONUP:
            command["type"] = "RCLICKUP";
            command["args"] = Json::Value(Json::objectValue);
            break;
        case WM_XBUTTONDOWN:
        {
            DWORD button = HIWORD(pMouseStruct->mouseData);
            command["type"] = "EXTRACLICKDOWN";
            command["args"]["id"] = Json::Value::Int64(button);
            break;
        }
        break;
        case WM_XBUTTONUP:
        {
            DWORD button = HIWORD(pMouseStruct->mouseData);
            command["type"] = "EXTRACLICKUP";
            command["args"]["id"] = Json::Value::Int64(button);
            break;

        }
        case WM_MOUSEMOVE:
            command["type"] = "MOVE";
            command["args"]["x"] = Json::Value::Int(pMouseStruct->pt.x);
            command["args"]["y"] = Json::Value::Int(pMouseStruct->pt.y);
            break;
        }

        std::chrono::high_resolution_clock::time_point mm_event_end = std::chrono::high_resolution_clock::now();
        auto in_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(mm_event_end - mm_event_start);
        auto elapsed = btw_elapsed - in_elapsed;

        // Append the mouse event
        Global::recordedJson["commands"].append(command);

        // Create and append the sleep event
        command.clear();
        command["type"] = "NSSLEEP";
        command["args"]["ns"] = Json::Value::UInt64(elapsed.count());
        Global::recordedJson["commands"].append(command);
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