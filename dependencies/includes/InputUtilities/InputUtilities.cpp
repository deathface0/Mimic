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
    input.mi.dx = static_cast<LONG>(x * 65536.0 / GetSystemMetrics(SM_CXSCREEN));
    input.mi.dy = static_cast<LONG>(y * 65536.0 / GetSystemMetrics(SM_CYSCREEN));
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
        this->runningInputs.insert({ "m_event_" + std::to_string(m_event) , c_event});

        return success;
    }
    else if (m_event == MOUSEEVENTF_LEFTUP || m_event == MOUSEEVENTF_RIGHTUP || m_event == MOUSEEVENTF_MIDDLEUP) {
        bool success = SendInput(1, &input, sizeof(INPUT));
        this->runningInputs.erase("m_event_" + std::to_string(m_event));

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
        this->runningInputs.insert({ "extra_btn_" + std::to_string(button), c_event});
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
        this->runningInputs.erase("extra_btn_" + std::to_string(button));
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

bool InputUtilitiesCore::vkKeyDown(WORD vkCode, bool upper)
{
    Event c_event{ -1, vkCode, -1 };

    // Check the state of the Shift and Caps Lock keys
    bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool isCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    bool isUpper = isShiftPressed ^ isCapsLockOn;

    INPUT input = { 0 };
    if (isUpper ^ upper)
    {
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_SCANCODE;
        input.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
        SendInput(1, &input, sizeof(input));
    }

    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = 0;
    bool success = SendInput(1, &input, sizeof(INPUT));

    if (isUpper ^ upper)
    {
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        input.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
        SendInput(1, &input, sizeof(input));
    }

    if (success)
        this->runningInputs.insert({ "vk_" + std::to_string(vkCode), c_event});

    return success;
}

bool InputUtilitiesCore::vkKeyUp(WORD vkCode)
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = KEYEVENTF_KEYUP;

    this->runningInputs.erase("vk_" + std::to_string(vkCode));

    return SendInput(1, &input, sizeof(INPUT));
}

//bool InputUtilitiesCore::KeyDown(char key)
//{
//    WORD vk_key = static_cast<WORD>(MapVirtualKeyEx(VkKeyScanA(key), MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
//
//    Event c_event{ -1, -1, key };
//
//    INPUT input;
//    memset(&input, 0, sizeof(INPUT));
//    input.type = INPUT_KEYBOARD;
//    input.ki.dwExtraInfo = GetMessageExtraInfo();
//    input.ki.wScan = vk_key;
//    input.ki.dwFlags = KEYEVENTF_SCANCODE;
//    bool success = SendInput(1, &input, sizeof(INPUT));
//
//    if (success)
//        this->runningInputs.insert({ "dk_" + std::to_string(key), c_event });
//
//    return success;
//}

bool InputUtilitiesCore::KeyDown(DWORD key, bool upper)
{
    Event c_event{ -1, -1, static_cast<char>(key) };

    WORD vk_key = static_cast<WORD>(MapVirtualKeyEx(key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));

    // Check the state of the Shift and Caps Lock keys
    bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool isCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    bool isUpper = isShiftPressed ^ isCapsLockOn;

    INPUT input;
    memset(&input, 0, sizeof(INPUT));
    if (isUpper ^ upper)
    {
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_SCANCODE;
        input.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
        SendInput(1, &input, sizeof(input));
    }

    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wScan = vk_key;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    bool success = SendInput(1, &input, sizeof(INPUT));

    if (isUpper ^ upper)
    {
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        input.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
        SendInput(1, &input, sizeof(input));
    }

    if (success)
        this->runningInputs.insert({ "dk_" + std::to_string(static_cast<char>(key)), c_event });


    /*WORD vk_key = static_cast<WORD>(MapVirtualKeyEx(key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));

    Event c_event{ -1, -1, static_cast<char>(key) };

    INPUT input;
    memset(&input, 0, sizeof(INPUT));
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wScan = vk_key;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    bool success = SendInput(1, &input, sizeof(INPUT));

    if (success)
        this->runningInputs.insert({ "dk_" + std::to_string(static_cast<char>(key)), c_event });*/

    return success;
}

//bool InputUtilitiesCore::KeyUp(char key)
//{
//    INPUT input;
//    memset(&input, 0, sizeof(INPUT));
//    input.type = INPUT_KEYBOARD;
//    input.ki.dwExtraInfo = GetMessageExtraInfo();
//    input.ki.wScan =
//        static_cast<WORD>(MapVirtualKeyEx(VkKeyScanA(key), MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
//    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
//
//    this->runningInputs.erase("dk_" + std::to_string(key));
//
//    return SendInput(1, &input, sizeof(INPUT));
//}

bool InputUtilitiesCore::KeyUp(DWORD key)
{
    WORD vk_key = static_cast<WORD>(MapVirtualKeyEx(key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));

    INPUT input;
    memset(&input, 0, sizeof(INPUT));
    input.type = INPUT_KEYBOARD;
    input.ki.dwExtraInfo = GetMessageExtraInfo();
    input.ki.wScan = vk_key;
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

    this->runningInputs.erase("dk_" + std::to_string(static_cast<char>(key)));

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
            this->runningInputs.insert({ "mvk_" + std::to_string(vkCode), c_event });
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

        this->runningInputs.erase("mvk_" + std::to_string(vkCode));
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
    return success &= MouseEvent(MOUSEEVENTF_LEFTUP);
}

bool InputUtilities::rightClick(time_t ms_hold)
{
    bool success = MouseEvent(MOUSEEVENTF_RIGHTDOWN);
    Sleep(ms_hold);
    return success &= MouseEvent(MOUSEEVENTF_RIGHTUP);
}

bool InputUtilities::middleClick(time_t ms_hold)
{
    bool success = MouseEvent(MOUSEEVENTF_MIDDLEDOWN);
    Sleep(ms_hold);
    return success &= MouseEvent(MOUSEEVENTF_MIDDLEUP);
}

bool InputUtilities::extraClick(int button, time_t ms_hold)
{
    bool success = ExtraClickDown(button);
    Sleep(ms_hold);
    return success &= ExtraClickUp(button);
}

bool InputUtilities::vkKey(WORD vkCode, bool upper, time_t ms_hold)
{
    int flag = vkKeyDown(vkCode, upper);
    Sleep(ms_hold);
    return flag &= vkKeyUp(vkCode);
}

//bool InputUtilities::directKey(char key, bool upper, time_t ms_hold)
//{
//    int flag = KeyDown(key, upper);
//    Sleep(ms_hold);
//    return flag &= KeyUp(key);
//}

bool InputUtilities::directKey(DWORD key, bool upper, time_t ms_hold)
{
    int flag = KeyDown(key, upper);
    Sleep(ms_hold);
    return flag &= KeyUp(key);
}

bool InputUtilities::vkMultiKey(const std::vector<WORD>& vkCodes, time_t ms_hold)
{
    int flag = vkMultiKeyDown(vkCodes);
    Sleep(ms_hold);
    return flag &= vkMultiKeyUp(vkCodes);
}

void InputUtilities::vkTypeString(std::string str)
{
    for (char cChar : str) {
        short vk = VkKeyScan(cChar);
        if ((vk >> 8) & 1) {
            keybd_event(VK_LSHIFT, 0, 0, 0);
        }
        keybd_event(static_cast<unsigned char>(vk), 0, 0, 0);
        keybd_event(static_cast<unsigned char>(vk), 0, KEYEVENTF_KEYUP, 0);
        if ((vk >> 8) & 1) {
            keybd_event(VK_LSHIFT, 0, KEYEVENTF_KEYUP, 0);
        }
    }
}

void InputUtilities::directTypeString(std::string str)
{

}
