#include "time_utils.h"

time_t TimeUtils::getUnixTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch().count();
    return value;
}

std::string TimeUtils::currentDateTime(std::string params) //"%Y-%m-%d %H:%M:%S" (24h)  /  "%Y-%m-%d %I:%M:%S %p" (12h)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm time_info;
#ifdef _WIN32
    localtime_s(&time_info, &now_c);
#else
    localtime_r(&timestamp, &time_info);
#endif
    std::stringstream ss;
    ss << std::put_time(&time_info, params.c_str());
    return ss.str();
}

time_t TimeUtils::TimeElapsed(time_t start, time_t end)
{
    return end - start;
}

std::string TimeUtils::timestampToDateTime(time_t timestamp, std::string params)
{
    time_t m_timestamp = timestamp / 1000;

    std::tm time_info;
#ifdef _WIN32
    localtime_s(&time_info, &m_timestamp);
#else
    localtime_r(&m_timestamp, &time_info);
#endif
    std::stringstream ss;
    ss << std::put_time(&time_info, params.c_str());
    return ss.str();
}

Time TimeUtils::secondsToHMS(size_t seconds)
{
    Time time;

    time.hours = seconds / 3600;
    time.minutes = (seconds % 3600) / 60;
    time.seconds = seconds % 60;

    return time;
}