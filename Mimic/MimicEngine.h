#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

#include "time_utils/time_utils.h"
#include "InputUtilities/InputUtilities.h"
#include "Global.h"

class MimicEngine
{
public:
	//MimicEngine();

	int readFile(std::string filepath);
	void eventToInstruction(const std::vector<MM_Event*>& events);
	void run();

private:
	EVENT_TYPE strToEventType(std::string cmd);
	void resetInstructions();

	int processCmd(Instruction instruction);


private:
	std::vector<Instruction> m_instructions;

	InputUtilities inputUtils;
};

