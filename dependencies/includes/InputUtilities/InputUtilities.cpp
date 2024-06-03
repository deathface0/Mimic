#include "InputUtilities.h"

InputUtilitiesCore::~InputUtilitiesCore()
{
    reset();
}

bool InputUtilitiesCore::SetCursorPos(int x, int y)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.time = 0;
    input.mi.dx = x * (65536 / GetSystemMetrics(SM_CXSCREEN));
    input.mi.dy = y * (65536 / GetSystemMetrics(SM_CYSCREEN));
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    return SendInput(1, &input, sizeof(INPUT));
}

bool InputUtilitiesCore::MouseEvent(WORD m_event)
{ 
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = m_event;

    Event c_event{ m_event, -1, -1 };

    if (m_event == MOUSEEVENTF_LEFTDOWN || m_event == MOUSEEVENTF_RIGHTDOWN || m_event == MOUSEEVENTF_MIDDLEDOWN) {
        bool success = SendInput(1, &input, sizeof(INPUT));
        this->runningInputs.insert({ m_event, c_event });

        return success;
    }
    else if (m_event == MOUSEEVENTF_LEFTUP || m_event == MOUSEEVENTF_RIGHTUP || m_event == MOUSEEVENTF_MIDDLEUP) {
        bool success = SendInput(1, &input, sizeof(INPUT));
        this->runningInputs.erase(m_event);

        return success;
    }

    return false;
}

bool InputUtilitiesCore::ExtraClickDown(int button)
{
    Event c_event{ MOUSEEVENTF_XDOWN, -1, -1 };

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = XBUTTON1 + (button - 1); // Se calcula el número de botón extra en función del parámetro
    input.mi.dwFlags = MOUSEEVENTF_XDOWN;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    try {
        this->runningInputs.insert({ button, c_event });
        return SendInput(1, &input, sizeof(INPUT));
    }
    catch (std::exception& e)
    {
        return false;
    }
}

bool InputUtilitiesCore::ExtraClickUp(int button)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = XBUTTON1 + (button - 1); // Se calcula el número de botón extra en función del parámetro
    input.mi.dwFlags = MOUSEEVENTF_XUP;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    try {
        return SendInput(1, &input, sizeof(INPUT));
    }
    catch (std::exception& e)
    {
        return false;
    }
}

bool InputUtilitiesCore::MouseWheelRoll(int scrolls, int delta)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = scrolls * delta * WHEEL_DELTA;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;
    return SendInput(1, &input, sizeof(INPUT));
}

bool InputUtilitiesCore::vkKeyDown(WORD vkCode)
{
    Event c_event{ -1, vkCode, -1 };

    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = 0;
    bool success = SendInput(1, &input, sizeof(INPUT));

    if (success)
        this->runningInputs.insert({ vkCode, c_event });

    return success;
}

bool InputUtilitiesCore::vkKeyUp(WORD vkCode)
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(1, &input, sizeof(INPUT));
}

bool InputUtilitiesCore::KeyDown(char key)
{
    Event c_event{ -1, -1, key };

    INPUT input;
    memset(&input, 0, sizeof(INPUT));
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wScan =
        static_cast<WORD>(MapVirtualKeyEx(VkKeyScanA(key), MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    bool success = SendInput(1, &input, sizeof(INPUT));

    if (success)
        this->runningInputs.insert({ key, c_event });

    return success;
}

bool InputUtilitiesCore::KeyUp(char key)
{
    INPUT input;
    memset(&input, 0, sizeof(INPUT));
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wScan =
        static_cast<WORD>(MapVirtualKeyEx(VkKeyScanA(key), MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    return SendInput(1, &input, sizeof(INPUT));
}

bool InputUtilitiesCore::vkMultiKeyDown(const std::vector<WORD>& vkCodes)
{
    int flag = 0;
    INPUT input;

    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    for (const auto& vkCode : vkCodes) {
        Event c_event{ -1, vkCode, -1 };

        input.ki.wVk = vkCode;
        input.ki.dwFlags = 0;
        bool success = SendInput(1, &input, sizeof(INPUT));

        if (success)
        {
            this->runningInputs.insert({ vkCode, c_event });
            flag++;
        }
    }

    return (flag == vkCodes.size()) ? true : false;
}

bool InputUtilitiesCore::vkMultiKeyUp(const std::vector<WORD>& vkCodes)
{
    int flag = 0;
    INPUT input;

    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    for (const auto& vkCode : vkCodes) {
        input.ki.wVk = vkCode;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        flag += SendInput(1, &input, sizeof(INPUT));
    }

    return (flag == vkCodes.size()) ? true : false;
}

void InputUtilitiesCore::reset()
{
    for (const auto& input : runningInputs)
    {
        if (input.second.vk != -1) 
            vkKeyUp(input.second.vk);

        if (input.second.key != -1)
            KeyUp(input.second.key);

        if (input.second.mouse != -1)
            MouseEvent(input.second.mouse << 1); //Bit shift left to obtain UP equivalence
    }

    this->runningInputs.clear();
}










bool InputUtilities::leftClick(time_t ms_hold)
{
    bool success = MouseEvent(MOUSEEVENTF_LEFTDOWN);
    Sleep(ms_hold);
    success &= MouseEvent(MOUSEEVENTF_LEFTUP);

    return success;
}

bool InputUtilities::rightClick(time_t ms_hold)
{
    bool success = MouseEvent(MOUSEEVENTF_RIGHTDOWN);
    Sleep(ms_hold);
    success &= MouseEvent(MOUSEEVENTF_RIGHTUP);

    return success;
}

bool InputUtilities::middleClick(time_t ms_hold)
{
    bool success = MouseEvent(MOUSEEVENTF_MIDDLEDOWN);
    Sleep(ms_hold);
    success &= MouseEvent(MOUSEEVENTF_MIDDLEUP);

    return success;
}

bool InputUtilities::extraClick(int button, time_t ms_hold)
{
    bool success = ExtraClickDown(button);
    Sleep(ms_hold);
    success &= ExtraClickUp(button);

    return success;
}

bool InputUtilities::vkKey(WORD vkCode)
{
    int flag = 0;

    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = 0;
    flag += SendInput(1, &input, sizeof(INPUT));
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    flag += SendInput(1, &input, sizeof(INPUT));

    return (flag == 2) ? true : false;
}

bool InputUtilities::directKey(char key)
{
    int flag = 0;

    INPUT input;
    memset(&input, 0, sizeof(INPUT));
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wScan =
        static_cast<WORD>(MapVirtualKeyEx(VkKeyScanA(key), MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    flag += SendInput(1, &input, sizeof(INPUT));

    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    flag += SendInput(1, &input, sizeof(INPUT));

    return (flag == 2) ? true : false;
}

bool InputUtilities::vkMultiKey(const std::vector<WORD>& vkCodes)
{
    int flag = 0;
    INPUT input;

    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    for (const auto& vkCode : vkCodes) {
        input.ki.wVk = vkCode;
        input.ki.dwFlags = 0;
        flag += SendInput(1, &input, sizeof(INPUT));
    }

    for (const auto& vkCode : vkCodes) {
        input.ki.wVk = vkCode;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        flag += SendInput(1, &input, sizeof(INPUT));
    }

    return (flag == vkCodes.size() * 2) ? true : false;
}
