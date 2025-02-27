#pragma once

#include "registries.hpp"

#include <span>

// clang-format off

namespace redfish::registries
{
struct BiosAttributeRegistry
{
static constexpr Header header = {
    "Copyright 2020 OpenBMC. All rights reserved.",
    "#MessageRegistry.v1_4_0.MessageRegistry",
    1,
    0,
    0,
    "Bios Attribute Registry",
    "en",
    "This registry defines the messages for bios attribute registry.",
    "BiosAttributeRegistry",
    "DMTF",
};

static constexpr const char* url = nullptr;

static constexpr std::span<const MessageEntry> registry =
{
};

enum class Index
{
};
}; // struct struct BiosAttributeRegistry


[[gnu::constructor]] inline void registerBiosAttributeRegistry()
{ registerRegistry<BiosAttributeRegistry>(); }

} // namespace redfish::registries