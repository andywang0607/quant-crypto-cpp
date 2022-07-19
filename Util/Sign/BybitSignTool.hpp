#ifndef __BYBITSIGNTOOL_H__
#define __BYBITSIGNTOOL_H__

#include <map>
#include <string>

#include "hmac.h"
#include "sha256.h"

namespace Util::Sign {

class BybitSignTool
{
public:
    static std::string signHttpReq(std::map<std::string, std::string> &param, const std::string &secret)
    {
        std::string signQuery;
        for (const auto &pair : param) {
            signQuery += pair.first + "=" + pair.second + "&";
        }
        auto sign = hmac<SHA256>(signQuery.substr(0, signQuery.length() - 1), secret);
        signQuery += "sign=" + sign;

        return signQuery;
    }
};
} // namespace Util::Sign

#endif // __BYBITSIGNTOOL_H__