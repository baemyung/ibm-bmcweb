// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors
#pragma once

#include "http_request.hpp"
#include "http_response.hpp"
#include "parsing.hpp"
#include "utils/json_utils.hpp"

#include <boost/beast/http/verb.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <string_view>

namespace redfish
{

class SubRequest
{
  public:
    explicit SubRequest(const crow::Request& req) :
        url_(req.url().encoded_path()), method_(req.method())
    {
        BMCWEB_LOG_ERROR("TEST: SubRequest - CTOR. START.  method={}",
                         req.methodString());
        // Extract Oem/OpenBmc payload if present
        if (req.method() == boost::beast::http::verb::patch ||
            req.method() == boost::beast::http::verb::post)
        {
            BMCWEB_LOG_ERROR("TEST: SubRequest - CTOR");
            nlohmann::json reqJson;
            if (parseRequestAsJson(req, reqJson) != JsonParseResult::Success)
            {
                BMCWEB_LOG_ERROR("TEST: SubRequest - CTOR PARSE ERROR");
                return;
            }

            BMCWEB_LOG_ERROR("TEST: SubRequest - CTOR FINDING");

#if 0

            crow::Response nullRes;  // It is to ignore the unknown fields
            std::optional<nlohmann::json::object_t> openBmcObj;
            if (!redfish::json_util::readJson(reqJson, nullRes,
                                              "Oem/OpenBmc", openBmcObj))
            {
                BMCWEB_LOG_ERROR("TEST: SubRequest - CTOR readJson ERROR - SKIP");
                return;
            }

            if (openBmcObj.has_value() && !openBmcObj->empty())
            {
                payload_ = *openBmcObj;

                nlohmann::json tPayload = payload_;
                BMCWEB_LOG_ERROR("TEST: SubRequest - CTOR. FOUND. payload={}",
                                 tPayload.dump());
            }
            else
            {
                BMCWEB_LOG_ERROR(
                    "TEST: SubRequest - CTOR. NOT FOUND     **** OPENBMC SKIP");
            }

#else

            auto oemIt = reqJson.find("Oem");
            if (oemIt != reqJson.end())
            {
                const nlohmann::json::object_t* oemObj =
                    oemIt->get_ptr<const nlohmann::json::object_t*>();
                if (oemObj != nullptr && !oemObj->empty())
                {
                    payload_ = *oemObj;

                    nlohmann::json tPayload = payload_;
                    BMCWEB_LOG_ERROR(
                        "TEST: SubRequest222 - CTOR. FOUND. payload={}",
                        tPayload.dump());
                }
            }
            else
            {
                BMCWEB_LOG_ERROR(
                    "TEST: SubRequest - CTOR. NOT FOUND     **** OEM SKIP");
            }

#endif
        }
    }

    std::string_view url() const
    {
        return url_;
    }

    boost::beast::http::verb method() const
    {
        return method_;
    }

    const nlohmann::json::object_t& payload() const
    {
        return payload_;
    }

    bool needHandling() const
    {
        if (method_ == boost::beast::http::verb::get)
        {
            return true;
        }

        if ((method_ == boost::beast::http::verb::patch ||
             method_ == boost::beast::http::verb::post) &&
            !payload_.empty())
        {
            return true;
        }

        return false;
    }

  private:
    std::string url_;
    boost::beast::http::verb method_;
    nlohmann::json::object_t payload_;
};

} // namespace redfish
