#include "MimicEngine.h"

int MimicEngine::readFile(std::string filepath)
{
    resetInstructions();

    std::ifstream file(filepath);
    std::string line;
    std::vector<std::string> args;

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

            //Check syntax error

            Instruction instruction{ cmd, args };
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

EVENT_TYPE MimicEngine::strToEventType(std::string cmd)
{
    if (cmd == "SLEEP") return EVENT_TYPE::SLEEP; 
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

int MimicEngine::processCmd(Instruction instruction)
{
    switch (instruction.cmd) {
    case EVENT_TYPE::SLEEP:
    {
        time_t ms_sleep = stoi(instruction.args[0]);
        std::this_thread::sleep_for(std::chrono::nanoseconds(ms_sleep));
        
        break;
    }
    case EVENT_TYPE::MOVE:
    {
        int x = stoi(instruction.args[0]), y = stoi(instruction.args[1]);
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
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
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
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
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
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        return inputUtils.middleClick(ms_hold);
    }
    case EVENT_TYPE::MWHEELDOWN:
    {
        int scroll_num = stoi(instruction.args[0]);
        return inputUtils.MouseWheelRoll(scroll_num, DOWN);
    }
    case EVENT_TYPE::MWHEELUP:
    {
        int scroll_num = stoi(instruction.args[0]);
        return inputUtils.MouseWheelRoll(scroll_num, UP);
    }
    case EVENT_TYPE::EXTRACLICKDOWN:
    {
        int button = stoi(instruction.args[0]);
        return inputUtils.ExtraClickDown(button);
    }
    case EVENT_TYPE::EXTRACLICKUP:
    {
        int button = stoi(instruction.args[0]);
        return inputUtils.ExtraClickUp(button);
    }
    case EVENT_TYPE::EXTRACLICK:
    {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        return inputUtils.extraClick(ms_hold);
    }
    case EVENT_TYPE::VKEYDOWN:
    {
        char key = std::toupper(instruction.args[0].front());
        return inputUtils.vkKeyDown(key);
    }
    case EVENT_TYPE::VKEYUP:
    {
        char key = std::toupper(instruction.args[0].front());
        return inputUtils.vkKeyUp(key);
    }
    case EVENT_TYPE::VKEY:
    {
        char key = std::toupper(instruction.args[0].front());
        time_t ms_hold = instruction.args.size() == 1 ? 0 : stoi(instruction.args[1]);
        return inputUtils.vkKey(key, ms_hold);
    }
    case EVENT_TYPE::KEYDOWN:
    {
        char key = std::tolower(instruction.args[0].front());
        return inputUtils.KeyDown(key);
    }
    case EVENT_TYPE::KEYUP:
    {
        char key = std::tolower(instruction.args[0].front());
        return inputUtils.KeyUp(key);
    }
    case EVENT_TYPE::KEY:
    {
        char key = std::tolower(instruction.args[0].front());
        time_t ms_hold = instruction.args.size() == 1 ? 0 : stoi(instruction.args[1]);
        return inputUtils.directKey(key, ms_hold);
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
        std::string str = instruction.args[0];
        inputUtils.vkTypeString(str);
        return 1;
    }
    case EVENT_TYPE::TYPESTRING:
    {
        std::string str = instruction.args[0];
        inputUtils.directTypeString(str);
        return 1;
    }
    default:
        break;
    }

    return -1;
}

void MimicEngine::eventToInstruction(const std::vector<MM_Event*>& events)
{
    resetInstructions();

    for (int i = 0; i < events.size(); i++)
    {
        MM_Event* e = events[i];
        Instruction instruction;
        std::vector<std::string> args;

        switch (e->type)
        {
        case EVENT_TYPE::KEYDOWN:
        {
            WORD mm_key;
            std::visit(
                [&](auto&& arg) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, WORD>) { mm_key = arg; }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, POINT>) { std::cout << "INCORRECT EVENT_TYPE!!" << std::endl; }
                },
                e->data
            );

            std::string keyCharStr(1, static_cast<char>(mm_key));
            args.push_back(keyCharStr);
            instruction = { EVENT_TYPE::KEYDOWN, args}; //KEYDOWN

            break;
        }
        case EVENT_TYPE::LCLICKDOWN:
        {
            instruction = { EVENT_TYPE::LCLICKDOWN, {} }; //LCLICKDOWN
            break;
        }
        case EVENT_TYPE::LCLICKUP:
        {
            instruction = { EVENT_TYPE::LCLICKUP, {} }; //LCLICKDOWN
            break;
        }
        case EVENT_TYPE::RCLICKDOWN:
        {
            instruction = { EVENT_TYPE::RCLICKDOWN, {} }; //RCLICKDOWN
            break;
        }
        case EVENT_TYPE::RCLICKUP:
        {
            instruction = { EVENT_TYPE::RCLICKUP, {} }; //LCLICKDOWN
            break;
        }
        case EVENT_TYPE::MOVE:
        {
            std::visit(
                [&](auto&& arg) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, POINT>) {
                        args.push_back(std::to_string(arg.x)); 
                        args.push_back(std::to_string(arg.y));
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, WORD>) { std::cout << "INCORRECT EVENT_TYPE!!" << std::endl; }
                },
                e->data
            );
            instruction = { e->type, args };
            break;
        }
        default:
            break;
        }

        this->m_instructions.push_back(instruction);

        //DELAY BETWEEN EVENTS
        if (i == events.size() - 1)
            continue;

        int skip = 1;
        while (events[i + skip]->type == EVENT_TYPE::LCLICKUP || events[i + skip]->type == EVENT_TYPE::RCLICKUP)
            skip++;

        int ms_sleep = TimeUtils::TimeElapsed(e->end, events[i + skip]->start) * 1000 - 230; std::cout << "SLEEP: " << ms_sleep << " -- " << e->type << " | " << events[i + 1]->type << std::endl;
        //ms_sleep = 1;
        args.clear(); args.push_back(std::to_string(ms_sleep));
        instruction = { EVENT_TYPE::SLEEP, args };
        this->m_instructions.push_back(instruction);
    }
}

void MimicEngine::run()
{
    for (const auto& instruction : m_instructions)
    {
        /*std::cout << instruction.cmd;
        for (const auto& arg : instruction.args)
            std::cout << " " << arg << " ";

        std::cout << "\n";*/

        //auto start = std::chrono::high_resolution_clock::now();
        processCmd(instruction);
        //auto end = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double, std::milli> elapsed = end - start;
        //std::cout << "TBI: " << elapsed.count() << " ms" << std::endl;
    }
}
