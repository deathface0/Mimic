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

            Command cmd = strToCommand(args.front());
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

Command MimicEngine::strToCommand(std::string cmd)
{
    if (cmd == "SLEEP") return Command::SLEEP; 
    else if (cmd == "MOVE") return Command::MOVE;
    else if (cmd == "LCLICKDOWN") return Command::LCLICKDOWN;
    else if (cmd == "LCLICKUP") return Command::LCLICKUP;
    else if (cmd == "LCLICK") return Command::LCLICK;
    else if (cmd == "RCLICKDOWN") return Command::RCLICKDOWN;
    else if (cmd == "RCLICKUP") return Command::RCLICKUP;
    else if (cmd == "RCLICK") return Command::RCLICK;
    else if (cmd == "MCLICKDOWN") return Command::MCLICKDOWN;
    else if (cmd == "MCLICKUP") return Command::MCLICKUP;
    else if (cmd == "MCLICK") return Command::MCLICK;
    else if (cmd == "MWHEELDOWN") return Command::MWHEELDOWN;
    else if (cmd == "MWHEELUP") return Command::MWHEELUP;
    else if (cmd == "EXTRACLICKDOWN") return Command::EXTRACLICKDOWN;
    else if (cmd == "EXTRACLICKUP") return Command::EXTRACLICKUP;
    else if (cmd == "EXTRACLICK") return Command::EXTRACLICK;
    else if (cmd == "KEYDOWN") return Command::KEYDOWN;
    else if (cmd == "KEYUP") return Command::KEYUP;
    else if (cmd == "KEY") return Command::KEY;
    else if (cmd == "VKEYDOWN") return Command::VKEYDOWN;
    else if (cmd == "VKEYUP") return Command::VKEYUP;
    else if (cmd == "VKEY") return Command::VKEY;
    else if (cmd == "MULTIKEYPRESSDOWN") return Command::MULTIKEYPRESSDOWN;
    else if (cmd == "MULTIKEYPRESSUP") return Command::MULTIKEYPRESSUP;

    return Command::NONE;
}

void MimicEngine::resetInstructions()
{
    m_instructions.clear();
}

int MimicEngine::processCmd(Instruction instruction)
{
    switch (instruction.cmd) {
    case Command::SLEEP:
    {
        time_t ms_sleep = stoi(instruction.args[0]);
        Sleep(ms_sleep);
        break;
    }
    case Command::MOVE:
    {
        int x = stoi(instruction.args[0]), y = stoi(instruction.args[1]);
        return inputUtils.SetCursorPos(x, y);
    }
    case Command::LCLICKDOWN:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_LEFTDOWN);
    }
    case Command::LCLICKUP:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_LEFTUP);
    }
    case Command::LCLICK:
    {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        return inputUtils.leftClick(ms_hold);
    }
    case Command::RCLICKDOWN:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_RIGHTDOWN);
    }
    case Command::RCLICKUP:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_RIGHTUP);
    }
    case Command::RCLICK:
    {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        return inputUtils.rightClick(ms_hold);
    }
    case Command::MCLICKDOWN:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_MIDDLEDOWN);
    }
    case Command::MCLICKUP:
    {
        return inputUtils.MouseEvent(MOUSEEVENTF_MIDDLEUP);
    }
    case Command::MCLICK:
    {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        return inputUtils.middleClick(ms_hold);
    }
    case Command::MWHEELDOWN:
    {
        int scroll_num = stoi(instruction.args[0]);
        return inputUtils.MouseWheelRoll(scroll_num, DOWN);
    }
    case Command::MWHEELUP:
    {
        int scroll_num = stoi(instruction.args[0]);
        return inputUtils.MouseWheelRoll(scroll_num, UP);
    }
    case Command::EXTRACLICKDOWN:
    {
        int button = stoi(instruction.args[0]);
        return inputUtils.ExtraClickDown(button);
    }
    case Command::EXTRACLICKUP:
    {
        int button = stoi(instruction.args[0]);
        return inputUtils.ExtraClickUp(button);
    }
    case Command::EXTRACLICK:
    {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        return inputUtils.extraClick(ms_hold);
    }
    case Command::VKEYDOWN:
    {
        char key = std::toupper(instruction.args[0].front());
        return inputUtils.vkKeyDown(key);
    }
    case Command::VKEYUP:
    {
        char key = std::toupper(instruction.args[0].front());
        return inputUtils.vkKeyUp(key);
    }
    case Command::VKEY:
    {
        char key = std::toupper(instruction.args[0].front());
        time_t ms_hold = instruction.args.size() == 1 ? 0 : stoi(instruction.args[1]);
        return inputUtils.vkKey(key , ms_hold);
    }
    case Command::KEYDOWN:
    {
        char key = std::tolower(instruction.args[0].front());
        return inputUtils.KeyDown(key);
    }
    case Command::KEYUP:
    {
        char key = std::tolower(instruction.args[0].front());
        return inputUtils.KeyUp(key);
    }
    case Command::KEY:
    {
        char key = std::tolower(instruction.args[0].front());
        time_t ms_hold = instruction.args.size() == 1 ? 0 : stoi(instruction.args[1]);
        return inputUtils.directKey(key, ms_hold);
    }
    case Command::MULTIKEYPRESSDOWN:
    {
        break;
    }
    case Command::MULTIKEYPRESSUP:
    {
        break;
    }
    case Command::MULTIKEYPRESS:
    {
        break;
    }
    default:
        break;
    }
    
    return 0;
}

std::vector<WORD> MimicEngine::str2wordVec(std::vector<std::string> vec)
{
    std::vector<WORD> word_vec;

    for (const auto& it : vec)
    {
        //word_vec.push_back()
    }

    return word_vec;
}

void MimicEngine::run()
{
    for (const auto& instruction : m_instructions)
    {
        std::cout << instruction.cmd;
        for (const auto& arg : instruction.args)
            std::cout << " " << arg << " ";

        std::cout << "\n";
        processCmd(instruction);
    }
}
