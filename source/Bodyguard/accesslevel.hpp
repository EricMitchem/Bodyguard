#pragma once

#include <optional>
#include <string>

enum class AccessLevel
{
    Owner,
    Tribe,
    Alliance,
    All
};

std::string GetStringFromAccessLevel(AccessLevel access_level);
std::optional<AccessLevel> GetAccessLevelFromString(const std::string& ascii_string);