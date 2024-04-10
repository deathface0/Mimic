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
            this->m_instructions.push(instruction);
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
    else if(cmd == "LCLICKDOWN") return Command::LCLICKDOWN;
    else if(cmd == "LCLICKUP") return Command::LCLICKUP;
    else if(cmd == "RCLICKDOWN") return Command::RCLICKDOWN;
    else if(cmd == "RCLICKUP") return Command::RCLICKUP;
    else if(cmd == "MCLICKDOWN") return Command::MCLICKDOWN;
    else if(cmd == "MCLICKUP") return Command::MCLICKUP;
    else if(cmd == "EXTRACLICKDOWN") return Command::EXTRACLICKDOWN;
    else if(cmd == "EXTRACLICKUP") return Command::EXTRACLICKUP;
    else if(cmd == "KEYDOWN") return Command::KEYDOWN;
    else if(cmd == "KEYUP") return Command::KEYDOWN;
    else if(cmd == "MULTIKEYPRESSDOWN") return Command::MULTIKEYPRESSDOWN;
    else if(cmd == "MULTIKEYPRESSUP") return Command::MULTIKEYPRESSUP;

    return Command::NONE;
}

void MimicEngine::resetInstructions()
{
    std::queue<Instruction>().swap(this->m_instructions);
}
