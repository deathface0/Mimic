#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include <chrono>
#include <sstream>
#include <iomanip>

struct Time { int hours, minutes, seconds; };

class TimeUtils
{
public:
	static time_t getUnixTimestamp();
	static std::string currentDateTime(std::string params); //"%Y-%m-%d %H:%M:%S" (24h)  /  "%Y-%m-%d %I:%M:%S %p" (12h)
	static time_t TimeElapsed(time_t start, time_t end);
	static std::string timestampToDateTime(time_t timestamp, std::string params);
	static Time secondsToHMS(size_t seconds);
};

