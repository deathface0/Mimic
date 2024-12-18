#pragma once

#include <vector>
#include <unordered_set>
#include <string>
#include <Windows.h>
#include <variant>
#include "json/json.h"

typedef std::basic_string<TCHAR> tstring;

enum EVENT_TYPE
{
	NONE = -1,

	SLEEP = 0, NSSLEEP,

	MOVE,

	LCLICKDOWN, LCLICKUP,
	RCLICKDOWN, RCLICKUP,
	MCLICKDOWN, MCLICKUP,
	MWHEELDOWN, MWHEELUP,
	EXTRACLICKDOWN, EXTRACLICKUP,

	VKEYDOWN, VKEYUP,
	SCKEYDOWN, SCKEYUP,
	UCKEYDOWN, UCKEYUP,

	MULTIVKDOWN, MULTIVKUP,
	MULTISCDOWN, MULTISCUP,
	MULTIUCDOWN, MULTIUCUP,

	LCLICK, RCLICK, MCLICK, EXTRACLICK,
	VKEY, SCKEY, UCKEY,
	MULTIVK, MULTISC, MULTIUC,

	VKTYPESTRING, SCTYPESTRING,
};

struct Instruction
{
	Instruction(EVENT_TYPE cmd, const Json::Value& args)
		: cmd(cmd), args(args)
	{}

	Instruction(const Instruction& other)
		: cmd(other.cmd), args(other.args)
	{}

	EVENT_TYPE cmd;
	Json::Value args;
};

namespace Global 
{
	inline bool recording = false;

	inline std::vector<Instruction*> recordBuf;
	inline std::unordered_set<WORD> pressedKeys;
}