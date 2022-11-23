#ifndef __UTIL_STRINGUTIL_H__
#define __UTIL_STRINGUTIL_H__

#include <algorithm>
#include <string>

namespace Util::StringUtil {

void toUpperCase(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void toLowerCase(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}
}

#endif // __UTIL_STRINGUTIL_H__