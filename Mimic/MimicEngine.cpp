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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }

    // Read file
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string jsonString = buffer.str();

    // Parse the JSON string
    Json::CharReaderBuilder readerBuilder;
    Json::Value root;
    std::string errs;

    std::istringstream s(jsonString);
    if (!Json::parseFromStream(readerBuilder, s, &root, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return -1;
    }

    const Json::Value commands = root["commands"];
    if (!commands.isArray()) {
        std::cerr << "Error: 'commands' is not an array." << std::endl;
        return -1;
    }

    for (const auto& command : commands) {
        const Json::Value args = command["args"];
        if (!args.isObject()) {
            std::cerr << "Error: 'args' is not an object." << std::endl;
            return -1;
        }

        std::string type = command["type"].asString();
        EVENT_TYPE cmd = strToEventType(type);

        Instruction* instruction = new Instruction(cmd, args);
        this->m_instructions.push_back(instruction);        
    }
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
    using namespace Global;

    auto it = cmdMap.find(cmd);
    if (it != cmdMap.end())
        return it->second;

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
        if (instruction->args.isMember("ms") && instruction->args["ms"].isInt64()) {
            time_t ms_sleep = instruction->args["ms"].asInt64();
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep));
            return 1;
        }

        return -1;
    }
    case EVENT_TYPE::NSSLEEP:
    {
        if (instruction->args.isMember("ns") && instruction->args["ns"].isInt64()) {
            time_t ns_sleep = instruction->args["ns"].asInt64();
            auto astart = std::chrono::steady_clock::now();
            auto target = astart + std::chrono::nanoseconds(ns_sleep);
            while (std::chrono::steady_clock::now() < target) {} // Busy-wait loop
            return 1;
        }

        return -1;
    }
    case EVENT_TYPE::MOVE:
    {
        if (instruction->args.isMember("x") && instruction->args.isMember("y") &&
            instruction->args["x"].isInt() && instruction->args["y"].isInt()) {
            int x = instruction->args["x"].asInt();
            int y = instruction->args["y"].asInt();
            return inputUtils.SetCursorPos(x, y);
        }

        return -1;
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
        if (instruction->args.isMember("ms") && instruction->args["ms"].isInt64()) {
            time_t ms_hold = instruction->args["ms"].asInt64();
            return inputUtils.leftClick(ms_hold);
        }
        
        return -1;
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
        if (instruction->args.isMember("ms") && instruction->args["ms"].isInt64()) {
            time_t ms_hold = instruction->args["ms"].asInt64();
            return inputUtils.rightClick(ms_hold);
        }

        return -1;
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
        if (instruction->args.isMember("ms") && instruction->args["ms"].isInt64()) {
            time_t ms_hold = instruction->args["ms"].asInt64();
            return inputUtils.middleClick(ms_hold);
        }

        return -1;
    }
    case EVENT_TYPE::MWHEELDOWN:
    {
        if (instruction->args.isMember("scroll_num") && instruction->args["scroll_num"].isInt()) {
            int scroll_num = instruction->args["scroll_num"].asInt();
            return inputUtils.MouseWheelRoll(scroll_num, DOWN);
        }

        return -1;
    }
    case EVENT_TYPE::MWHEELUP:
    {
        if (instruction->args.isMember("scroll_num") && instruction->args["scroll_num"].isInt()) {
            int scroll_num = instruction->args["scroll_num"].asInt();
            return inputUtils.MouseWheelRoll(scroll_num, UP);
        }

        return -1;
    }
    case EVENT_TYPE::EXTRACLICKDOWN:
    {
        if (instruction->args.isMember("id") && instruction->args["id"].isInt()) {
            int button = instruction->args["id"].asInt();
            return inputUtils.ExtraClickDown(button);
        }

        return -1;
    }
    case EVENT_TYPE::EXTRACLICKUP:
    {
        if (instruction->args.isMember("id") && instruction->args["id"].isInt()) {
            int button = instruction->args["id"].asInt();
            return inputUtils.ExtraClickUp(button);
        }

        return -1;
    }
    case EVENT_TYPE::EXTRACLICK:
    {
        if (instruction->args.isMember("id") && instruction->args.isMember("ms") &&
            instruction->args["id"].isInt() && instruction->args["ms"].isInt64()) {
            int button = instruction->args["id"].asInt();
            time_t ms_hold = instruction->args["ms"].asInt64();
            return inputUtils.extraClick(button, ms_hold);
        }

        return -1;
    }
    case EVENT_TYPE::VKEYDOWN:
    {
        if (instruction->args.isMember("key")) {
            WORD vk_code = static_cast<WORD>(instruction->args["key"].asString().front());
            return inputUtils.vKeyDown(vk_code);
        }

        return -1;
    }
    case EVENT_TYPE::VKEYUP:
    {
        if (instruction->args.isMember("key")) {
            WORD vk_code = static_cast<WORD>(instruction->args["key"].asString().front());
            return inputUtils.vKeyUp(vk_code);
        }
        
        return -1;
    }
    case EVENT_TYPE::VKEY:
    {
        if (instruction->args.isMember("key") && instruction->args.isMember("ms")
            && instruction->args["ms"].isInt64()) {
            std::string vk_str = instruction->args["key"].asString();
            auto it = vk_map.find(vk_str);
            WORD vk_code = it != vk_map.end() ? it->second : static_cast<WORD>(vk_str.front());
            time_t ms_hold = instruction->args["ms"].asInt64();
            return inputUtils.vKey(vk_code, ms_hold);
        }

        return -1;
    }
    case EVENT_TYPE::SCKEYDOWN:
    {
        if (instruction->args.isMember("key")) {
            std::string key = instruction->args["key"].asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            return inputUtils.scKeyDown(wkey.front());
        }

        return -1;
    }
    case EVENT_TYPE::SCKEYUP:
    {
        if (instruction->args.isMember("key")) {
            std::string key = instruction->args["key"].asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            return inputUtils.scKeyUp(wkey.front());
        }

        return -1;
    }
    case EVENT_TYPE::SCKEY:
    {
        if (instruction->args.isMember("key") && instruction->args.isMember("ms")
            && instruction->args["ms"].isInt64()) {
            std::string key = instruction->args["key"].asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            time_t ms_hold = instruction->args["ms"].asInt64();
            return inputUtils.scKey(wkey.front(), ms_hold);
        }

        return -1;
    }
    case EVENT_TYPE::UCKEYDOWN:
    {
        if (instruction->args.isMember("key")) {
            std::string key = instruction->args["key"].asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            return inputUtils.unicodeKeyDown(wkey.front());
        }

        return -1;
    }
    case EVENT_TYPE::UCKEYUP:
    {
        if (instruction->args.isMember("key")) {
            std::string key = instruction->args["key"].asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            return inputUtils.unicodeKeyUp(wkey.front());
        }

        return -1;
    }
    case EVENT_TYPE::UCKEY:
    {
        if (instruction->args.isMember("key") && instruction->args.isMember("ms")
            && instruction->args["ms"].isInt64()) {
            std::string key = instruction->args["key"].asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            time_t ms_hold = instruction->args["ms"].asInt64();
            return inputUtils.unicodeKey(wkey.front(), ms_hold);
        }

        return -1;
    }
    case EVENT_TYPE::MULTIVKDOWN:
    {
        std::vector<WORD> vkCodes;

        if (!instruction->args.isMember("keys") || !instruction->args["keys"].isArray())
            return -1;

        for (const auto vk : instruction->args["keys"]) {
            std::string vk_str = vk.asString();
            auto it = vk_map.find(vk_str);
            WORD vk_code = it != vk_map.end() ? it->second : static_cast<WORD>(vk.asString().front());

            vkCodes.push_back(vk_code);
        }

        return inputUtils.vkMultiKeyDown(vkCodes);
    }
    case EVENT_TYPE::MULTIVKUP:
    {
        std::vector<WORD> vkCodes;

        if (!instruction->args.isMember("keys") || !instruction->args["keys"].isArray())
            return -1;

        for (const auto vk : instruction->args["keys"]) {
            std::string vk_str = vk.asString();
            auto it = vk_map.find(vk_str);
            WORD vk_code = it != vk_map.end() ? it->second : static_cast<WORD>(vk.asString().front());

            vkCodes.push_back(vk_code);
        }

        return inputUtils.vkMultiKeyUp(vkCodes);
    }
    case EVENT_TYPE::MULTIVK:
    {
        std::vector<WORD> vkCodes;

        if (!instruction->args.isMember("keys") || !instruction->args.isMember("ms") || !instruction->args["keys"].isArray() || !instruction->args["ms"].isInt64())
            return -1;

        time_t ms_sleep = instruction->args["ms"].asInt64();

        for (const auto vk : instruction->args["keys"]) {
            std::string vk_str = vk.asString();
            auto it = vk_map.find(vk_str);
            WORD vk_code = it != vk_map.end() ? it->second : static_cast<WORD>(vk_str.front());

            vkCodes.push_back(vk_code);
        }

        return inputUtils.vkMultiKey(vkCodes, ms_sleep);
    }
    case EVENT_TYPE::MULTISCDOWN:
    {
        std::vector<wchar_t> keys;

        if (!instruction->args.isMember("keys") || !instruction->args["keys"].isArray())
            return -1;

        for (const auto vk : instruction->args["keys"]) {
            std::string key = vk.asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            keys.push_back(wkey.front());
        }

        return inputUtils.scMultiKeyDown(keys);
    }
    case EVENT_TYPE::MULTISCUP:
    {
        std::vector<wchar_t> keys;

        if (!instruction->args.isMember("keys") || !instruction->args["keys"].isArray())
            return -1;

        for (const auto vk : instruction->args["keys"]) {
            std::string key = vk.asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            keys.push_back(wkey.front());
        }

        return inputUtils.scMultiKeyUp(keys);
    }
    case EVENT_TYPE::MULTISC:
    {
        std::vector<wchar_t> keys;

        if (!instruction->args.isMember("keys") || !instruction->args.isMember("ms") || !instruction->args["keys"].isArray() || !instruction->args["ms"].isInt64())
            return -1;

        time_t ms_sleep = instruction->args["ms"].asInt64();

        for (const auto vk : instruction->args["keys"]) {
            std::string key = vk.asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            keys.push_back(wkey.front());
        }
        
        return inputUtils.scMultiKey(keys, ms_sleep);
    }
    case EVENT_TYPE::MULTIUCDOWN:
    {
        std::vector<wchar_t> keys;

        if (!instruction->args.isMember("keys") || !instruction->args["keys"].isArray())
            return -1;

        for (const auto vk : instruction->args["keys"]) {
            std::string key = vk.asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            keys.push_back(wkey.front());
        }

        return inputUtils.unicodeMultiKeyDown(keys);
    }
    case EVENT_TYPE::MULTIUCUP:
    {
        std::vector<wchar_t> keys;

        if (!instruction->args.isMember("keys") || !instruction->args["keys"].isArray())
            return -1;

        for (const auto vk : instruction->args["keys"]) {
            std::string key = vk.asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            keys.push_back(wkey.front());
        }

        return inputUtils.unicodeMultiKeyUp(keys);
    }
    case EVENT_TYPE::MULTIUC:
    {
        std::vector<wchar_t> keys;

        if (!instruction->args.isMember("keys") || !instruction->args.isMember("ms") || !instruction->args["keys"].isArray() || !instruction->args["ms"].isInt64())
            return -1;

        time_t ms_sleep = instruction->args["ms"].asInt64();

        for (const auto vk : instruction->args["keys"]) {
            std::string key = vk.asString();
            std::wstring wkey = inputUtils.get_utf16(key);
            keys.push_back(wkey.front());
        }

        return inputUtils.unicodeMultiKey(keys, ms_sleep);
    }
    case EVENT_TYPE::VKTYPESTRING:
    {
        if (instruction->args.isMember("str") && instruction->args["str"].isString()) {
            std::string str = instruction->args["str"].asString();
            std::wstring wstr = inputUtils.get_utf16(str);
            return inputUtils.typeStr(wstr);
        }

        return -1;
    }
    case EVENT_TYPE::SCTYPESTRING:
    {
        if (instruction->args.isMember("str") && instruction->args["str"].isString()) {
            std::string str = instruction->args["str"].asString();
            std::wstring wstr = inputUtils.get_utf16(str);
            return inputUtils.scTypeStr(wstr);
        }

        return -1;
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
        std::cout << instruction->cmd << " -- " << processCmd(instruction) << std::endl;
    }
    counter /= 2;
    std::cout << "COUNT: " << counter << std::endl;
}
