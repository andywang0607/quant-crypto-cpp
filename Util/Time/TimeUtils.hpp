#ifndef __TIMEUTILS_H__
#define __TIMEUTILS_H__

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"

namespace Util::Time {

inline long long getTime()
{
    const auto p1 = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(
               p1.time_since_epoch())
        .count();
}

inline std::string getDateTime()
{
    using namespace boost::posix_time;
    ptime localTime = second_clock::local_time();
    return to_iso_string(localTime);
}

inline std::string getDate()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

/**
* \brief convert ISO 8601 date time string to timestamp 
* \param[in] ISO 8601 datetime string, ex: 2020-03-30T02:21:06.000Z
* \return timestamp
*/
inline long long toTimestamp(std::string &dateTime)
{
    using namespace boost::posix_time;
    static const std::string EpochStr = "19700101T000000.000";
    static const boost::posix_time::ptime Epoch(from_iso_string(EpochStr));

    if (auto index = dateTime.rfind("Z"); index != std::string::npos) {
        dateTime.erase(dateTime.begin() + index);
    }

    boost::posix_time::ptime t(from_iso_extended_string(dateTime));
    return (t - Epoch).total_milliseconds();
}
} // namespace Util::Time

#endif // __TIMEUTILS_H__