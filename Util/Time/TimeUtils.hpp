#ifndef __TIMEUTILS_H__
#define __TIMEUTILS_H__

#include <chrono>
#include <string>

namespace Util::Time {

inline long long getTime()
{
    const auto p1 = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(
               p1.time_since_epoch())
        .count();
}

inline long long toTimestamp(const std::string &dateTime)
{
    std::tm t{};
    std::istringstream ss(dateTime);

    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error{"failed to parse time string"};
    }
    std::time_t time_stamp = mktime(&t);

    return time_stamp;
}
} // namespace Util::Time

#endif // __TIMEUTILS_H__