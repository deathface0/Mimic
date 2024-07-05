#pragma once

#include <vector>
#include <unordered_set>
#include <string>
#include <Windows.h>
#include <variant>

typedef std::basic_string<TCHAR> tstring;

enum EVENT_TYPE
{
	NONE = -1,

	SLEEP = 0,

	LCLICKDOWN, LCLICKUP, LCLICK,
	RCLICKDOWN, RCLICKUP, RCLICK,
	MCLICKDOWN, MCLICKUP, MCLICK,
	MWHEELDOWN, MWHEELUP,
	EXTRACLICKDOWN, EXTRACLICKUP, EXTRACLICK,

	VKEYDOWN, VKEYUP, VKEY,
	KEYDOWN, KEYUP, KEY,
	MULTIKEYPRESSDOWN, MULTIKEYPRESSUP, MULTIKEYPRESS,
	VKTYPESTRING, TYPESTRING,
	MOVE,
};

struct Instruction
{
	Instruction(EVENT_TYPE cmd, const std::vector<std::string>& args)
		: cmd(cmd), args(args)
	{}

	EVENT_TYPE cmd;
	std::vector<std::string> args;
};

namespace Global 
{
	inline bool recording = false;

	inline std::vector<Instruction*> recordBuf;
	inline std::unordered_set<WORD> pressedKeys;
}