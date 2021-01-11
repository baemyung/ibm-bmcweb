// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright OpenBMC Authors
#pragma once

#include "app.hpp"
#include "async_resp.hpp"
#include "dbus_singleton.hpp"
#include "dbus_utility.hpp"
#include "error_messages.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "logging.hpp"
#include "query.hpp"
#include "registries/privilege_registry.hpp"
#include "utils/asset_utils.hpp"
#include "utils/chassis_utils.hpp"
#include "utils/dbus_utils.hpp"
#include "utils/json_utils.hpp"

#include <boost/beast/http/verb.hpp>
#include <sdbusplus/asio/property.hpp>
#include <sdbusplus/unpack_properties.hpp>

#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace redfish
{

/**
 * @brief Get Location code for the given assembly.
 * @param[in] asyncResp - Shared pointer for asynchronous calls.
 * @param[in] serviceName - Service in which the assembly is hosted.
 * @param[in] assembly - Assembly object.
 * @param[in] assemblyIndex - Index on the assembly object.
 * @return None.
 */
void getAssemblyLocationCode(
    const std::shared_ptr<bmcweb::AsyncResp>& asyncResp,
    const auto& serviceName, const auto& assembly, const auto& assemblyIndex)
{
    sdbusplus::asio::getProperty<std::string>(
        *crow::connections::systemBus, serviceName, assembly,
        "xyz.openbmc_project.Inventory.Decorator.LocationCode", "LocationCode",
        [asyncResp, assemblyIndex](const boost::system::error_code& ec,
                                   const std::string& value) {
            if (ec)
            {
                BMCWEB_LOG_ERROR("DBUS response error: {}", ec.value());
                messages::internalError(asyncResp->res);
                return;
            }

            nlohmann::json& assemblyArray =
                asyncResp->res.jsonValue["Assemblies"];
            nlohmann::json& assemblyData = assemblyArray.at(assemblyIndex);

            assemblyData["Location"]["PartLocation"]["ServiceLabel"] = value;
        });
}

/**
 * @brief Get properties for the assemblies associated to given chassis
 * @param[in] asyncResp - Shared pointer for asynchronous calls.
 * @param[in] chassisPath - Chassis the assemblies are associated with.
 * @param[in] assemblies - list of all the assemblies associated with the
 * chassis.
 * @return None.
 */
inline void getAssemblyProperties(
    const std::shared_ptr<bmcweb::AsyncResp>& asyncResp,
    const std::string& chassisId, const std::vector<std::string>& assemblies)
{
    BMCWEB_LOG_DEBUG("Get properties for assembly associated");

    std::size_t assemblyIndex = 0;
    for (const auto& assembly : assemblies)
    {
        nlohmann::json::object_t item;
        item["@odata.type"] = "#Assembly.v1_3_0.AssemblyData";
        item["@odata.id"] = boost::urls::format(
            "/redfish/v1/Chassis/{}/Assembly#/Assemblies/{}", chassisId,
            std::to_string(assemblyIndex));
        item["MemberId"] = std::to_string(assemblyIndex);
        item["Name"] = sdbusplus::message::object_path(assembly).filename();

        asyncResp->res.jsonValue["Assemblies"].emplace_back(item);

        nlohmann::json::json_pointer assemblyJsonPtr(
            "/Assemblies/" + std::to_string(assemblyIndex));

        dbus::utility::getDbusObject(
            assembly, chassisAssemblyInterfaces,
            [asyncResp, assemblyIndex, assembly,
             assemblyJsonPtr](const boost::system::error_code& ec,
                              const dbus::utility::MapperGetObject& object) {
                if (ec)
                {
                    BMCWEB_LOG_ERROR("DBUS response error : {}", ec.value());
                    messages::internalError(asyncResp->res);
                    return;
                }

                for (const auto& [serviceName, interfaceList] : object)
                {
                    for (const auto& interface : interfaceList)
                    {
                        if (interface ==
                            "xyz.openbmc_project.Inventory.Decorator.Asset")
                        {
                            asset_utils::getAssetInfo(asyncResp, serviceName,
                                                      assembly,
                                                      assemblyJsonPtr);
                        }
                        else if (
                            interface ==
                            "xyz.openbmc_project.Inventory.Decorator.LocationCode")
                        {
                            getAssemblyLocationCode(asyncResp, serviceName,
                                                    assembly, assemblyIndex);
                        }
                    }
                }
            });

        nlohmann::json& assemblyArray = asyncResp->res.jsonValue["Assemblies"];
        asyncResp->res.jsonValue["Assemblies@odata.count"] =
            assemblyArray.size();

        assemblyIndex++;
    }
}

inline void afterHandleChassisAssemblyGet(
    const std::shared_ptr<bmcweb::AsyncResp>& asyncResp,
    const std::string& chassisID, const boost::system::error_code& ec,
    const std::vector<std::string>& assemblyList)
{
    if (ec)
    {
        BMCWEB_LOG_WARNING("Chassis not found");
        messages::resourceNotFound(asyncResp->res, "Chassis", chassisID);
        return;
    }

    asyncResp->res.addHeader(
        boost::beast::http::field::link,
        "</redfish/v1/JsonSchemas/Assembly/Assembly.json>; rel=describedby");

    asyncResp->res.jsonValue["@odata.type"] = "#Assembly.v1_3_0.Assembly";
    asyncResp->res.jsonValue["@odata.id"] =
        boost::urls::format("/redfish/v1/Chassis/{}/Assembly", chassisID);
    asyncResp->res.jsonValue["Name"] = "Assembly Collection";
    asyncResp->res.jsonValue["Id"] = "Assembly";

    asyncResp->res.jsonValue["Assemblies"] = nlohmann::json::array();
    asyncResp->res.jsonValue["Assemblies@odata.count"] = 0;

    if (!assemblyList.empty())
    {
        getAssemblyProperties(asyncResp, chassisID, assemblyList);
    }
}

/**
 * @brief Get chassis path with given chassis ID
 * @param[in] asyncResp - Shared pointer for asynchronous calls.
 * @param[in] chassisID - Chassis to which the assemblies are
 * associated.
 *
 * @return None.
 */
inline void handleChassisAssemblyGet(
    App& app, const crow::Request& req,
    const std::shared_ptr<bmcweb::AsyncResp>& asyncResp,
    const std::string& chassisID)
{
    if (!redfish::setUpRedfishRoute(app, req, asyncResp))
    {
        return;
    }

    BMCWEB_LOG_DEBUG("Get chassis Assmbly");

    chassis_utils::getChassisAssembly(
        asyncResp, chassisID,
        std::bind_front(afterHandleChassisAssemblyGet, asyncResp, chassisID));
}

namespace assembly
{
/**
 * @brief API used to fill the Assembly id of the assembled object that
 *        assembled in the given assembly parent object path.
 *
 *        bmcweb using the sequential numeric value by sorting the
 *        assembled objects instead of the assembled object dbus id
 *        for the Redfish Assembly implementation.
 *
 * @param[in] asyncResp - The redfish response to return.
 * @param[in] assemblyParentServ - The assembly parent dbus service name.
 * @param[in] assemblyParentObjPath - The assembly parent dbus object path.
 * @param[in] assemblyParentIface - The assembly parent dbus interface name
 *                                  to valid the supports in the bmcweb.
 * @param[in] assemblyUriPropPath - The redfish property path to fill with id.
 * @param[in] assembledObjPath - The assembled object that need to fill with
 *                               its id. Used to check in the parent assembly
 *                               associations.
 * @param[in] assembledUriVal - The assembled object redfish uri value that
 *                              need to replace with its id.
 *
 * @return The redfish response with assembled object id in the given
 *         redfish property path if success else returns the error.
 */
inline void fillWithAssemblyId(
    const std::shared_ptr<bmcweb::AsyncResp>& asyncResp,
    const std::string& assemblyParentServ,
    const sdbusplus::message::object_path& assemblyParentObjPath,
    const std::string& assemblyParentIface,
    const nlohmann::json::json_pointer& assemblyUriPropPath,
    const sdbusplus::message::object_path& assembledObjPath,
    const std::string& assembledUriVal)
{
    if (assemblyParentIface != "xyz.openbmc_project.Inventory.Item.Chassis")
    {
        // Currently, bmcweb supporting only chassis assembly uri so return
        // error if unsupported assembly uri interface was given
        BMCWEB_LOG_ERROR(
            "Unsupported interface [{}] was given to fill assembly id. Please add support in the bmcweb",
            assemblyParentIface);
        messages::internalError(asyncResp->res);
        return;
    }

    using associationList =
        std::vector<std::tuple<std::string, std::string, std::string>>;

    sdbusplus::asio::getProperty<associationList>( //
        *crow::connections::systemBus, assemblyParentServ,
        assemblyParentObjPath.str,
        "xyz.openbmc_project.Association.Definitions", "Associations",
        [asyncResp, assemblyUriPropPath, assemblyParentObjPath,
         assembledObjPath,
         assembledUriVal](const boost::system::error_code& ec,
                          const associationList& associations) {
            if (ec)
            {
                BMCWEB_LOG_ERROR(
                    "DBUS response error [{}  : {}] when tried to get the Associations from [{}] to fill Assembly id of the assembled object [{}]",
                    ec.value(), ec.message(), assemblyParentObjPath.str,
                    assembledObjPath.str);
                messages::internalError(asyncResp->res);
                return;
            }

            std::vector<std::string> assemblyAssoc;
            for (const auto& association : associations)
            {
                if (std::get<0>(association) != "assembly")
                {
                    continue;
                }
                assemblyAssoc.emplace_back(std::get<2>(association));
            }

            if (assemblyAssoc.empty())
            {
                BMCWEB_LOG_ERROR(
                    "No assembly associations in the [{}] to fill Assembly id of the assembled object [{}]",
                    assemblyParentObjPath.str, assembledObjPath.str);
                messages::internalError(asyncResp->res);
                return;
            }

            // Mak sure whether the retrieved assembly associations are
            // implemented before finding the assembly id as per bmcweb Assembly
            // design.
            dbus::utility::async_method_call(
                [asyncResp, assemblyUriPropPath, assemblyParentObjPath,
                 assembledObjPath, assemblyAssoc, assembledUriVal](
                    const boost::system::error_code& ec1,
                    const dbus::utility::MapperGetSubTreeResponse& objects) {
                    if (ec1)
                    {
                        BMCWEB_LOG_ERROR(
                            "DBUS response error [{} : {}] when tried to get the subtree to check assembled objects implementation of the [{}] to find assembled object id of the [{}] to fill in the URI property",
                            ec1.value(), ec1.message(),
                            assemblyParentObjPath.str, assembledObjPath.str);
                        messages::internalError(asyncResp->res);
                        return;
                    }

                    if (objects.empty())
                    {
                        BMCWEB_LOG_ERROR(
                            "No objects in the [{}] to check assembled objects implementation to fill the assembled object [{}] id in the URI property",
                            assemblyParentObjPath.str, assembledObjPath.str);
                        messages::internalError(asyncResp->res);
                        return;
                    }

                    std::vector<std::string> implAssemblyAssocs;
                    for (const auto& object : objects)
                    {
                        auto it =
                            std::ranges::find(assemblyAssoc, object.first);
                        if (it != assemblyAssoc.end())
                        {
                            implAssemblyAssocs.emplace_back(*it);
                        }
                    }

                    if (implAssemblyAssocs.empty())
                    {
                        BMCWEB_LOG_ERROR(
                            "The assembled objects of the [{}] are not implemented so unable to fill the assembled object [{}] id in the URI property",
                            assemblyParentObjPath.str, assembledObjPath.str);
                        messages::internalError(asyncResp->res);
                        return;
                    }

                    // sort  the implemented assemply object as per bmcweb
                    // design to match with Assembly GET and PATCH handler.
                    std::ranges::sort(implAssemblyAssocs);

                    auto assembledObjectIt = std::ranges::find(
                        implAssemblyAssocs, assembledObjPath.str);

                    if (assembledObjectIt == implAssemblyAssocs.end())
                    {
                        BMCWEB_LOG_ERROR(
                            "The assembled object [{}] in the object [{}] is not implemented so unable to fill assembled object id in the URI property",
                            assembledObjPath.str, assemblyParentObjPath.str);
                        messages::internalError(asyncResp->res);
                        return;
                    }

                    auto assembledObjectId = std::distance(
                        implAssemblyAssocs.begin(), assembledObjectIt);

                    std::string::size_type assembledObjectNamePos =
                        assembledUriVal.rfind(assembledObjPath.filename());

                    if (assembledObjectNamePos == std::string::npos)
                    {
                        BMCWEB_LOG_ERROR(
                            "The assembled object name [{}] is not found in the redfish property value [{}] to replace with assembled object id [{}]",
                            assembledObjPath.filename(), assembledUriVal,
                            assembledObjectId);
                        messages::internalError(asyncResp->res);
                        return;
                    }
                    std::string uriValwithId(assembledUriVal);
                    uriValwithId.replace(assembledObjectNamePos,
                                         assembledObjPath.filename().length(),
                                         std::to_string(assembledObjectId));

                    asyncResp->res.jsonValue[assemblyUriPropPath] =
                        uriValwithId;
                },
                "xyz.openbmc_project.ObjectMapper",
                "/xyz/openbmc_project/object_mapper",
                "xyz.openbmc_project.ObjectMapper", "GetSubTree",
                "/xyz/openbmc_project/inventory", int32_t(0),
                chassisAssemblyInterfaces);
        });
}

} // namespace assembly

inline void handleChassisAssemblyHead(
    crow::App& app, const crow::Request& req,
    const std::shared_ptr<bmcweb::AsyncResp>& asyncResp,
    const std::string& chassisID)
{
    if (!redfish::setUpRedfishRoute(app, req, asyncResp))
    {
        return;
    }

    chassis_utils::getChassisAssembly(
        asyncResp, chassisID,
        [asyncResp,
         chassisID](const boost::system::error_code& ec,
                    const std::vector<std::string>& /*assemblyList*/) {
            if (ec)
            {
                BMCWEB_LOG_WARNING("Chassis not found");
                messages::resourceNotFound(asyncResp->res, "Chassis",
                                           chassisID);
                return;
            }
            asyncResp->res.addHeader(
                boost::beast::http::field::link,
                "</redfish/v1/JsonSchemas/Assembly.json>; rel=describedby");
        });
}

inline void handleChassisAssemblyPatch(
    crow::App& app, const crow::Request& req,
    const std::shared_ptr<bmcweb::AsyncResp>& asyncResp,
    const std::string& chassisID)
{
    if (!redfish::setUpRedfishRoute(app, req, asyncResp))
    {
        return;
    }

    chassis_utils::getChassisAssembly(
        asyncResp, chassisID,
        [asyncResp,
         chassisID](const boost::system::error_code& ec,
                    const std::vector<std::string>& /*assemblyList*/) {
            if (ec)
            {
                BMCWEB_LOG_WARNING("Chassis not found");
                messages::resourceNotFound(asyncResp->res, "Chassis",
                                           chassisID);
                return;
            }
            asyncResp->res.addHeader(
                boost::beast::http::field::link,
                "</redfish/v1/JsonSchemas/Assembly.json>; rel=describedby");
        });
}

/**
 * Systems derived class for delivering Assembly Schema.
 */
inline void requestRoutesAssembly(App& app)
{
    BMCWEB_ROUTE(app, "/redfish/v1/Chassis/<str>/Assembly/")
        .privileges(redfish::privileges::headAssembly)
        .methods(boost::beast::http::verb::head)(
            std::bind_front(handleChassisAssemblyHead, std::ref(app)));

    BMCWEB_ROUTE(app, "/redfish/v1/Chassis/<str>/Assembly/")
        .privileges(redfish::privileges::getAssembly)
        .methods(boost::beast::http::verb::get)(
            std::bind_front(handleChassisAssemblyGet, std::ref(app)));

    BMCWEB_ROUTE(app, "/redfish/v1/Chassis/<str>/Assembly/")
        .privileges(redfish::privileges::patchAssembly)
        .methods(boost::beast::http::verb::patch)(
            std::bind_front(handleChassisAssemblyPatch, std::ref(app)));
}

} // namespace redfish
