#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
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

    inline const std::unordered_map<std::string, EVENT_TYPE> cmdMap = {
            {"SLEEP", EVENT_TYPE::SLEEP},
            {"NSSLEEP", EVENT_TYPE::NSSLEEP},
            {"MOVE", EVENT_TYPE::MOVE},
            {"LCLICKDOWN", EVENT_TYPE::LCLICKDOWN},
            {"LCLICKUP", EVENT_TYPE::LCLICKUP},
            {"LCLICK", EVENT_TYPE::LCLICK},
            {"RCLICKDOWN", EVENT_TYPE::RCLICKDOWN},
            {"RCLICKUP", EVENT_TYPE::RCLICKUP},
            {"RCLICK", EVENT_TYPE::RCLICK},
            {"MCLICKDOWN", EVENT_TYPE::MCLICKDOWN},
            {"MCLICKUP", EVENT_TYPE::MCLICKUP},
            {"MCLICK", EVENT_TYPE::MCLICK},
            {"MWHEELDOWN", EVENT_TYPE::MWHEELDOWN},
            {"MWHEELUP", EVENT_TYPE::MWHEELUP},
            {"EXTRACLICKDOWN", EVENT_TYPE::EXTRACLICKDOWN},
            {"EXTRACLICKUP", EVENT_TYPE::EXTRACLICKUP},
            {"EXTRACLICK", EVENT_TYPE::EXTRACLICK},
            {"SCKEYDOWN", EVENT_TYPE::SCKEYDOWN},
            {"SCKEYUP", EVENT_TYPE::SCKEYUP},
            {"SCKEY", EVENT_TYPE::SCKEY},
            {"VKEYDOWN", EVENT_TYPE::VKEYDOWN},
            {"VKEYUP", EVENT_TYPE::VKEYUP},
            {"VKEY", EVENT_TYPE::VKEY},
            {"UCKEYDOWN", EVENT_TYPE::UCKEYDOWN},
            {"UCKEYUP", EVENT_TYPE::UCKEYUP},
            {"UCKEY", EVENT_TYPE::UCKEY},
            {"MULTIVKDOWN", EVENT_TYPE::MULTIVKDOWN},
            {"MULTIVKUP", EVENT_TYPE::MULTIVKUP},
            {"MULTIVK", EVENT_TYPE::MULTIVK},
            {"MULTISCDOWN", EVENT_TYPE::MULTISCDOWN},
            {"MULTISCUP", EVENT_TYPE::MULTISCUP},
            {"MULTISC", EVENT_TYPE::MULTISC},
            {"MULTIUCDOWN", EVENT_TYPE::MULTIUCDOWN},
            {"MULTIUCUP", EVENT_TYPE::MULTIUCUP},
            {"MULTIUC", EVENT_TYPE::MULTIUC},
            {"VKTYPESTRING", EVENT_TYPE::VKTYPESTRING},
            {"SCTYPESTRING", EVENT_TYPE::SCTYPESTRING}
    };
}