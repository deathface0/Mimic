#include "MimicEngine.h"

MimicEngine::~MimicEngine()
{
    for (const auto instruction : m_instructions)
        delete instruction;

    for (const auto instruction : Global::recordBuf)
        delete instruction;
}

int MimicEngine::readFile(std::string filepath)
{
    resetInstructions();

    std::ifstream file(filepath);
    std::string line;
    std::vector<std::string> args(5);

    if (file.is_open()) {
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string arg;
            args.clear();

            while (iss >> arg) {
                args.push_back(arg);
            }

            EVENT_TYPE cmd = strToEventType(args.front());
            args.erase(args.begin());

            if (cmd == EVENT_TYPE::KEYDOWN || cmd == EVENT_TYPE::KEYUP || cmd == EVENT_TYPE::KEY ||
                cmd == EVENT_TYPE::VKEYDOWN || cmd == EVENT_TYPE::VKEYUP || cmd == EVENT_TYPE::VKEY)
            {
                bool upper = std::isupper(args[0].front());
                WORD vkCode = VkKeyScanEx(args[0].front(), GetKeyboardLayout(0));
                if (vkCode == -1)
                {
                    // Character cannot be mapped to a virtual key code
                    std::cout << "ERROR MAPPING KEY!!" << std::endl;
                    return -1;
                }
                args[0] = std::to_string(static_cast<UCHAR>(vkCode & 0xFF));
                if (args.size() == 1)
                {
                    args.push_back("0");
                    args.push_back(std::to_string(upper));
                }
                else if (args.size() == 2)
                {
                    args.push_back(std::to_string(upper));
                }
            }

            //Check syntax error

            Instruction* instruction = new Instruction(cmd, args);
            this->m_instructions.push_back(instruction);
        }

        file.close();
    }
    else {
        std::cout << "No se pudo abrir el archivo." << std::endl;
        return -1;
    }

    return 1;
}

void MimicEngine::importRecordBuf()
{
    resetInstructions();

    for (const auto instruction : Global::recordBuf)
    {
        Instruction* newInstruction = new Instruction(*instruction);
        this->m_instructions.push_back(newInstruction);
    }
    this->m_instructions = Global::recordBuf;

    Global::recordBuf.clear();
}

EVENT_TYPE MimicEngine::strToEventType(std::string cmd)
{
    if (cmd == "SLEEP") return EVENT_TYPE::SLEEP;
    else if (cmd == "NSSLEEP") return EVENT_TYPE::NSSLEEP;
    else if (cmd == "MOVE") return EVENT_TYPE::MOVE;
    else if (cmd == "LCLICKDOWN") return EVENT_TYPE::LCLICKDOWN;
    else if (cmd == "LCLICKUP") return EVENT_TYPE::LCLICKUP;
    else if (cmd == "LCLICK") return EVENT_TYPE::LCLICK;
    else if (cmd == "RCLICKDOWN") return EVENT_TYPE::RCLICKDOWN;
    else if (cmd == "RCLICKUP") return EVENT_TYPE::RCLICKUP;
    else if (cmd == "RCLICK") return EVENT_TYPE::RCLICK;
    else if (cmd == "MCLICKDOWN") return EVENT_TYPE::MCLICKDOWN;
    else if (cmd == "MCLICKUP") return EVENT_TYPE::MCLICKUP;
    else if (cmd == "MCLICK") return EVENT_TYPE::MCLICK;
    else if (cmd == "MWHEELDOWN") return EVENT_TYPE::MWHEELDOWN;
    else if (cmd == "MWHEELUP") return EVENT_TYPE::MWHEELUP;
    else if (cmd == "EXTRACLICKDOWN") return EVENT_TYPE::EXTRACLICKDOWN;
    else if (cmd == "EXTRACLICKUP") return EVENT_TYPE::EXTRACLICKUP;
    else if (cmd == "EXTRACLICK") return EVENT_TYPE::EXTRACLICK;
    else if (cmd == "KEYDOWN") return EVENT_TYPE::KEYDOWN;
    else if (cmd == "KEYUP") return EVENT_TYPE::KEYUP;
    else if (cmd == "KEY") return EVENT_TYPE::KEY;
    else if (cmd == "VKEYDOWN") return EVENT_TYPE::VKEYDOWN;
    else if (cmd == "VKEYUP") return EVENT_TYPE::VKEYUP;
    else if (cmd == "VKEY") return EVENT_TYPE::VKEY;
    else if (cmd == "MULTIKEYPRESSDOWN") return EVENT_TYPE::MULTIKEYPRESSDOWN;
    else if (cmd == "MULTIKEYPRESSUP") return EVENT_TYPE::MULTIKEYPRESSUP;
    else if (cmd == "MULTIKEYPRESS") return EVENT_TYPE::MULTIKEYPRESS;
    else if (cmd == "VKTYPESTRING") return EVENT_TYPE::VKTYPESTRING;
    else if (cmd == "TYPESTRING") return EVENT_TYPE::TYPESTRING;

    return EVENT_TYPE::NONE;
}

void MimicEngine::resetInstructions()
{
    m_instructions.clear();
}

int MimicEngine::processCmd(Instruction* instruction)
{
    switch (instruction->cmd) {
    case EVENT_TYPE::SLEEP:
    {
        time_t ms_sleep = stoi(instruction->args[0]);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep));
        
        break;
    }
    case EVENT_TYPE::NSSLEEP:
    {
        size_t ns_sleep = stoul(instruction->args[0]);
        auto astart = std::chrono::steady_clock::now();
        auto target = astart + std::chrono::nanoseconds(ns_sleep);
        while (std::chrono::steady_clock::now() < target) {} // Busy-wait loop

        break;
    }
    case EVENT_TYPE::MOVE:
    {
        int x = stoi(instruction->args[0]), y = stoi(instruction->args[1]);
        return inputUtils.SetCursorPos(x, y);
    }
    case EVENT_TYPE::LCLICKDOWN:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_LEFTDOWN);
    }
    case EVENT_TYPE::LCLICKUP:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_LEFTUP);
    }
    case EVENT_TYPE::LCLICK:
    {
        time_t ms_hold = instruction->args.empty() ? 0 : stoi(instruction->args[0]);
        return inputUtils.leftClick(ms_hold);
    }
    case EVENT_TYPE::RCLICKDOWN:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_RIGHTDOWN);
    }
    case EVENT_TYPE::RCLICKUP:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_RIGHTUP);
    }
    case EVENT_TYPE::RCLICK:
    {
        time_t ms_hold = instruction->args.empty() ? 0 : stoi(instruction->args[0]);
        return inputUtils.rightClick(ms_hold);
    }
    case EVENT_TYPE::MCLICKDOWN:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_MIDDLEDOWN);
    }
    case EVENT_TYPE::MCLICKUP:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_MIDDLEUP);
    }
    case EVENT_TYPE::MCLICK:
    {
        time_t ms_hold = instruction->args.empty() ? 0 : stoi(instruction->args[0]);
        return inputUtils.middleClick(ms_hold);
    }
    case EVENT_TYPE::MWHEELDOWN:
    {
        int scroll_num = stoi(instruction->args[0]);
        return inputUtils.MouseWheelRoll(scroll_num, DOWN);
    }
    case EVENT_TYPE::MWHEELUP:
    {
        int scroll_num = stoi(instruction->args[0]);
        return inputUtils.MouseWheelRoll(scroll_num, UP);
    }
    case EVENT_TYPE::EXTRACLICKDOWN:
    {
        int button = stoi(instruction->args[0]);
        return inputUtils.ExtraClickDown(button);
    }
    case EVENT_TYPE::EXTRACLICKUP:
    {
        int button = stoi(instruction->args[0]);
        return inputUtils.ExtraClickUp(button);
    }
    case EVENT_TYPE::EXTRACLICK:
    {
        time_t ms_hold = instruction->args.empty() ? 0 : stoi(instruction->args[0]);
        return inputUtils.extraClick(ms_hold);
    }
    case EVENT_TYPE::VKEYDOWN:
    {
        DWORD vk_code = static_cast<DWORD>(std::stoul(instruction->args[0], nullptr, 0));
        return inputUtils.vkKeyDown(vk_code, stoi(instruction->args[2]));
    }
    case EVENT_TYPE::VKEYUP:
    {
        DWORD vk_code = static_cast<DWORD>(std::stoul(instruction->args[0], nullptr, 0));
        return inputUtils.vkKeyUp(vk_code);
    }
    case EVENT_TYPE::VKEY:
    {
        DWORD vk_code = static_cast<DWORD>(std::stoul(instruction->args[0], nullptr, 0));
        time_t ms_hold = std::stoul(instruction->args[1], nullptr, 0);
        return inputUtils.vkKey(vk_code, stoi(instruction->args[2]), ms_hold);
    }
    case EVENT_TYPE::KEYDOWN:
    {
        DWORD vk_code = static_cast<DWORD>(std::stoul(instruction->args[0], nullptr, 0));
        return inputUtils.KeyDown(vk_code, stoi(instruction->args[2]));
    }
    case EVENT_TYPE::KEYUP:
    {
        DWORD vk_code = static_cast<DWORD>(std::stoul(instruction->args[0], nullptr, 0));
        return inputUtils.KeyUp(vk_code);
    }
    case EVENT_TYPE::KEY:
    {
        DWORD vk_code = static_cast<DWORD>(std::stoul(instruction->args[0], nullptr, 0));
        time_t ms_hold = std::stoul(instruction->args[1], nullptr, 0);
        return inputUtils.directKey(vk_code, stoi(instruction->args[2]), ms_hold);
    }
    case EVENT_TYPE::MULTIKEYPRESSDOWN:
    {
        break;
    }
    case EVENT_TYPE::MULTIKEYPRESSUP:
    {
        break;
    }
    case EVENT_TYPE::MULTIKEYPRESS:
    {
        break;
    }
    case EVENT_TYPE::VKTYPESTRING:
    {
        std::string str = instruction->args[0];
        inputUtils.vkTypeString(str);
        return 1;
    }
    case EVENT_TYPE::TYPESTRING:
    {
        std::string str = instruction->args[0];
        inputUtils.directTypeString(str);
        return 1;
    }
    default:
        break;
    }

    return -1;
}

void MimicEngine::run()
{
    int counter = 0;
    for (const auto instruction : m_instructions)
    {
        counter++;
        /*std::cout << instruction->cmd << " - ";
        for (const auto arg : instruction->args)
        {
            std::cout << arg << ", ";
        }
        std::cout << "" << std::endl;*/
        processCmd(instruction);
    }
    counter /= 2;
    std::cout << "COUNT: " << counter << std::endl;
}
