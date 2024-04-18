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
    else if (cmd == "VKEYDOWN") return Command::VKEYDOWN;
    else if (cmd == "VKEYUP") return Command::VKEYUP;
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
    if (instruction.cmd == Command::SLEEP) Sleep(stoi(instruction.args[0]));
    else if (instruction.cmd == Command::MOVE) inputUtils.SetCursorPos(stoi(instruction.args[0]), stoi(instruction.args[1]));
    else if (instruction.cmd == Command::LCLICKDOWN) inputUtils.MouseEvent(MOUSEEVENTF_LEFTDOWN);
    else if (instruction.cmd == Command::LCLICKUP) inputUtils.MouseEvent(MOUSEEVENTF_LEFTUP);
    else if (instruction.cmd == Command::LCLICK) {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        inputUtils.leftClick(ms_hold);
    }
    else if (instruction.cmd == Command::RCLICKDOWN) inputUtils.MouseEvent(MOUSEEVENTF_RIGHTDOWN);
    else if (instruction.cmd == Command::RCLICKUP) inputUtils.MouseEvent(MOUSEEVENTF_RIGHTUP);
    else if (instruction.cmd == Command::RCLICK) {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        inputUtils.rightClick(ms_hold);
    }
    else if (instruction.cmd == Command::MCLICKDOWN) inputUtils.MouseEvent(MOUSEEVENTF_MIDDLEDOWN);
    else if (instruction.cmd == Command::MCLICKUP) inputUtils.MouseEvent(MOUSEEVENTF_MIDDLEUP);
    else if (instruction.cmd == Command::MCLICK) {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        inputUtils.middleClick(ms_hold);
    }
    else if (instruction.cmd == Command::EXTRACLICKDOWN) inputUtils.ExtraClickDown(stoi(instruction.args[0]));
    else if (instruction.cmd == Command::EXTRACLICKUP) inputUtils.ExtraClickUp(stoi(instruction.args[0]));
    else if (instruction.cmd == Command::EXTRACLICK) {
        time_t ms_hold = instruction.args.empty() ? 0 : stoi(instruction.args[0]);
        inputUtils.extraClick(ms_hold);
    }
    else if (instruction.cmd == Command::KEYDOWN) {
        char key = std::tolower(instruction.args[0].front());
        inputUtils.KeyDown(key);
    }
    else if (instruction.cmd == Command::KEYUP) {
        char key = std::tolower(instruction.args[0].front());
        inputUtils.KeyUp(key);
    }
    else if (instruction.cmd == Command::VKEYDOWN) {
        char key = std::toupper(instruction.args[0].front());
        inputUtils.vkKeyDown(key);
    }
    else if (instruction.cmd == Command::VKEYUP) {
        char key = std::toupper(instruction.args[0].front());
        inputUtils.vkKeyUp(key);
    }
    //else if (instruction.cmd == Command::MULTIKEYPRESSDOWN) inputUtils.vkMultiKey(instruction.args);
    //else if (instruction.cmd == Command::MULTIKEYPRESSUP) inputUtils.KeyUp((char)instruction.args[0].c_str());
    return 0;
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
