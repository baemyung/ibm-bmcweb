#pragma once

#include "bmcweb_config.h"

#include <bit>
#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <source_location>
#include <string_view>
#include <system_error>
#include <thread>

// NOLINTBEGIN(readability-convert-member-functions-to-static, cert-dcl58-cpp)
template <>
struct std::formatter<void*>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }
    auto format(const void*& ptr, auto& ctx) const
    {
        return std::format_to(ctx.out(), "{}",
                              std::to_string(std::bit_cast<size_t>(ptr)));
    }
};
// NOLINTEND(readability-convert-member-functions-to-static, cert-dcl58-cpp)

namespace crow
{
enum class LogLevel
{
    Disabled = 0,
    Critical,
    Error,
    Warning,
    Info,
    Debug,
    Enabled,
};

// Mapping of the external loglvl name to internal loglvl
constexpr std::array<std::string_view, 7> mapLogLevelFromName{
    "DISABLED", "CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG", "ENABLED"};

constexpr crow::LogLevel getLogLevelFromName(std::string_view name)
{
    const auto* iter = std::ranges::find(mapLogLevelFromName, name);
    if (iter != mapLogLevelFromName.end())
    {
        return static_cast<LogLevel>(iter - mapLogLevelFromName.begin());
    }
    return crow::LogLevel::Disabled;
}

// configured bmcweb LogLevel
constexpr crow::LogLevel bmcwebCurrentLoggingLevel =
    getLogLevelFromName(BMCWEB_LOGGING_LEVEL);

template <typename T>
const void* logPtr(T p)
{
    static_assert(std::is_pointer<T>::value,
                  "Can't use logPtr without pointer");
    return std::bit_cast<const void*>(p);
}

template <LogLevel level, typename... Args>
inline void vlog(std::format_string<Args...>&& format, Args&&... args,
                 const std::source_location& loc) noexcept
{
    if constexpr (bmcwebCurrentLoggingLevel < level)
    {
        return;
    }
    constexpr size_t stringIndex = static_cast<size_t>(level);
    static_assert(stringIndex < mapLogLevelFromName.size(),
                  "Missing string for level");
    constexpr std::string_view levelString = mapLogLevelFromName[stringIndex];
    std::string_view filename = loc.file_name();
    filename = filename.substr(filename.rfind('/'));
    if (!filename.empty())
    {
        filename.remove_prefix(1);
    }
    std::string logLocation;
    try
    {
        // TODO, multiple static analysis tools flag that this could potentially
        // throw Based on the documentation, it shouldn't throw, so long as none
        // of the formatters throw, so unclear at this point why this try/catch
        // is required, but add it to silence the static analysis tools.
        logLocation =
            std::format("[{} {}:{}] ", levelString, filename, loc.line());
        logLocation +=
            std::format(std::move(format), std::forward<Args>(args)...);
    }
    catch (const std::format_error& /*error*/)
    {
        logLocation += "Failed to format";
        // Nothing more we can do here if logging is broken.
    }
    logLocation += '\n';
    // Intentionally ignore error return.
    fwrite(logLocation.data(), sizeof(std::string::value_type),
           logLocation.size(), stdout);
    fflush(stdout);
}
} // namespace crow

template <typename... Args>
struct BMCWEB_LOG_CRITICAL
{
    // NOLINTNEXTLINE(google-explicit-constructor)
    BMCWEB_LOG_CRITICAL(std::format_string<Args...> format, Args&&... args,
                        const std::source_location& loc =
                            std::source_location::current()) noexcept
    {
        crow::vlog<crow::LogLevel::Critical, Args...>(
            std::move(format), std::forward<Args>(args)..., loc);
    }
};

template <typename... Args>
struct BMCWEB_LOG_ERROR
{
    // NOLINTNEXTLINE(google-explicit-constructor)
    BMCWEB_LOG_ERROR(std::format_string<Args...> format, Args&&... args,
                     const std::source_location& loc =
                         std::source_location::current()) noexcept
    {
        crow::vlog<crow::LogLevel::Error, Args...>(
            std::move(format), std::forward<Args>(args)..., loc);
    }
};

template <typename... Args>
struct BMCWEB_LOG_WARNING
{
    // NOLINTNEXTLINE(google-explicit-constructor)
    BMCWEB_LOG_WARNING(std::format_string<Args...> format, Args&&... args,
                       const std::source_location& loc =
                           std::source_location::current()) noexcept
    {
        crow::vlog<crow::LogLevel::Warning, Args...>(
            std::move(format), std::forward<Args>(args)..., loc);
    }
};

template <typename... Args>
struct BMCWEB_LOG_INFO
{
    // NOLINTNEXTLINE(google-explicit-constructor)
    BMCWEB_LOG_INFO(std::format_string<Args...> format, Args&&... args,
                    const std::source_location& loc =
                        std::source_location::current()) noexcept
    {
        crow::vlog<crow::LogLevel::Info, Args...>(
            std::move(format), std::forward<Args>(args)..., loc);
    }
};

template <typename... Args>
struct BMCWEB_LOG_DEBUG
{
    // NOLINTNEXTLINE(google-explicit-constructor)
    BMCWEB_LOG_DEBUG(std::format_string<Args...> format, Args&&... args,
                     const std::source_location& loc =
                         std::source_location::current()) noexcept
    {
        crow::vlog<crow::LogLevel::Debug, Args...>(
            std::move(format), std::forward<Args>(args)..., loc);
    }
};

template <typename... Args>
BMCWEB_LOG_CRITICAL(std::format_string<Args...>, Args&&...)
    -> BMCWEB_LOG_CRITICAL<Args...>;

template <typename... Args>
BMCWEB_LOG_ERROR(std::format_string<Args...>, Args&&...)
    -> BMCWEB_LOG_ERROR<Args...>;

template <typename... Args>
BMCWEB_LOG_WARNING(std::format_string<Args...>, Args&&...)
    -> BMCWEB_LOG_WARNING<Args...>;

template <typename... Args>
BMCWEB_LOG_INFO(std::format_string<Args...>, Args&&...)
    -> BMCWEB_LOG_INFO<Args...>;

template <typename... Args>
BMCWEB_LOG_DEBUG(std::format_string<Args...>, Args&&...)
    -> BMCWEB_LOG_DEBUG<Args...>;

// NOTE 1: if /tmp/wait exists, it goes to wait loop
// NOTE 2: if /tmp/skip exists, it exits the loop and remove /tmp/skip

inline void waitIfNeeded(
    const std::string& title,
    const std::source_location& loc = std::source_location::current())
{
    std::error_code ec1;
    bool fileExists = std::filesystem::exists("/tmp/wait", ec1);
    if (ec1 || !fileExists)
    {
        return;
    }

    std::string_view filename = loc.file_name();
    filename = filename.substr(filename.rfind('/'));
    if (!filename.empty())
    {
        filename.remove_prefix(1);
    }
    std::string logLocation = std::format("[{}:{}] ", filename, loc.line());

    BMCWEB_LOG_ERROR(
        "TEST:waitIfNeeded:{}, loc:{} - /tmp/wait exists .... wait-loop START",
        title, logLocation);
    bool done = false;
    while (!done)
    {
        std::chrono::seconds timespan(1); // or whatever
        std::this_thread::sleep_for(timespan);

        fileExists = std::filesystem::exists("/tmp/wait", ec1);
        if (ec1 || !fileExists)
        {
            break;
        }

        fileExists = std::filesystem::exists("/tmp/skip", ec1);
        if (!ec1 && fileExists)
        {
            std::filesystem::remove("/tmp/skip");
            break;
        }
    }

    BMCWEB_LOG_ERROR(
        "TEST:waitIfNeeded:{}, loc:{} - /tmp/wait exists .... wait-loop END",
        title, logLocation);
}
