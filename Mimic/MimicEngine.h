#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "InputUtilities/InputUtilities.h"

enum Command
{
	NONE = -1,

	SLEEP = 0,

	MOVE,
	LCLICKDOWN, LCLICKUP, LCLICK,
	RCLICKDOWN, RCLICKUP, RCLICK,
	MCLICKDOWN, MCLICKUP, MCLICK,
	MWHEELDOWN, MWHEELUP,
	EXTRACLICKDOWN, EXTRACLICKUP, EXTRACLICK,

	VKEYDOWN, VKEYUP,
	KEYDOWN, KEYUP,
	MULTIKEYPRESSDOWN, MULTIKEYPRESSUP
};

struct Instruction
{
	Command cmd;
	std::vector<std::string> args;
};

class MimicEngine
{
public:
	//MimicEngine();

	int readFile(std::string filepath);
	void run();

private:
	Command strToCommand(std::string cmd);
	void resetInstructions();

	int processCmd(Instruction instruction);

private:
	std::vector<Instruction> m_instructions;

	InputUtilities inputUtils;
};

