#ifndef __UTIL_THREADUTIL_H__
#define __UTIL_THREADUTIL_H__

#include <thread>

namespace Util::Thread::ThreadUtil {
static inline void setThreadName(const std::string &name)
{
#if defined(__linux__)
    pthread_setname_np(pthread_self(), name.c_str());
#endif
}

static inline std::string getThreadName()
{
    static constexpr size_t NameSize = 16;
    char name[NameSize] = {'\0'};
#if defined(__linux__)
    pthread_getname_np(pthread_self(), name, NameSize);
#endif
    return name;
}

} // namespace Util::Thread::ThreadUtil

#endif // __UTIL_THREADUTIL_H__