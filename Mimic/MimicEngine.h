#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>

#include "InputUtilities/InputUtilities.h"

enum Command
{
	NONE = -1,

	SLEEP = 0,

	LCLICKDOWN, LCLICKUP,
	RCLICKDOWN, RCLICKUP,
	MCLICKDOWN, MCLICKUP,
	MWHEELDOWN, MWHEELUP,
	EXTRACLICKDOWN, EXTRACLICKUP,

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

private:
	Command strToCommand(std::string cmd);
	void resetInstructions();

private:
	std::queue<Instruction> m_instructions;
};

