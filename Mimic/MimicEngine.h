#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "InputUtilities/InputUtilities.h"
#include "Global.h"

class MimicEngine
{
public:
	//MimicEngine();

	int readFile(std::string filepath);
	void run();

private:
	EVENT_TYPE strToEventType(std::string cmd);
	void resetInstructions();

	int processCmd(Instruction instruction);

	std::vector<WORD> str2wordVec(std::vector<std::string> vec);

private:
	std::vector<Instruction> m_instructions;

	InputUtilities inputUtils;
};

