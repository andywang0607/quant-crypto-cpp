#ifndef __UTIL_RESTREQUESTER_H__
#define __UTIL_RESTREQUESTER_H__

#include <string>

#include "Logger.hpp"

#include <restclient-cpp/restclient.h>

namespace Util::Requester {

class RestRequester
{
public:
    template <typename GenUrlFunc, typename IsSuccess>
    static bool get(GenUrlFunc &&genUrl, IsSuccess &&isSuccess)
    {
        const std::string url = genUrl();

        const auto response = RestClient::get(url);
        const auto success = isSuccess(response);
        if (!success) {
            logger.warn("Get req failed, url={}, code={}, body={}", url, response.code, response.body);
        }

        return success;
    }

    template <typename GenUrlFunc, typename GenContentTypeFunc, typename GenDataFunc, typename IsSuccess>
    static bool post(GenUrlFunc &&genUrl, GenContentTypeFunc &&genContentType, GenDataFunc &&genData, IsSuccess &&isSuccess)
    {
        const std::string url = genUrl();
        const std::string contentType = genContentType();
        const std::string data = genData();

        const auto response = RestClient::post(url, contentType, data);
        const auto success = isSuccess(response);
        if (!success) {
            logger.warn("Post req failed, url={}, contentType={}, data={}, code={}, body={}", url, contentType, data, response.code, response.body);
        }

        return success;
    }

    template <typename GenUrlFunc, typename GenContentTypeFunc, typename GenDataFunc, typename IsSuccess>
    static bool put(GenUrlFunc &&genUrl, GenContentTypeFunc &&genContentType, GenDataFunc &&genData, IsSuccess &&isSuccess)
    {
        const std::string url = genUrl();
        const std::string contentType = genContentType();
        const std::string data = genData();

        const auto response = RestClient::put(url, contentType, data);
        const auto success = isSuccess(response);
        if (!success) {
            logger.warn("Put req failed, url={}, contentType={}, data={}, code={}, body={}", url, contentType, data, response.code, response.body);
        }

        return success;
    }

    template <typename GenUrlFunc, typename IsSuccess>
    static bool del(GenUrlFunc &&genUrl, IsSuccess &&isSuccess)
    {
        const std::string url = genUrl();

        const auto response = RestClient::del(url);
        const auto success = isSuccess(response);
        if (!success) {
            logger.warn("Del req failed, url={}, code={}, body={}", url, response.code, response.body);
        }

        return success;
    }

private:
    inline static Util::Log::Logger logger{"RestRequester"};
};
} // namespace Util::Requester

#endif // __UTIL_RESTREQUESTER_H__