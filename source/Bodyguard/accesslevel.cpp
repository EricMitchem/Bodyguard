// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include <algorithm>
#include <cctype>
#include <map>
#include "accesslevel.hpp"

std::string GetStringFromAccessLevel(AccessLevel access_level)
{
    static const std::map<AccessLevel, std::string> Impl = {
        {AccessLevel::Owner, "owner"},
        {AccessLevel::Tribe, "tribe"},
        {AccessLevel::Alliance, "alliance"},
        {AccessLevel::All, "all"}
    };

    return Impl.find(access_level)->second;
}

std::optional<AccessLevel> GetAccessLevelFromString(const std::string& ascii_str)
{
    static const std::map<std::string, AccessLevel> Impl = {
        {"owner", AccessLevel::Owner},
        {"tribe", AccessLevel::Tribe},
        {"alliance", AccessLevel::Alliance},
        {"all", AccessLevel::All}
    };

    std::string lowercase_str;

    lowercase_str.resize(ascii_str.size());
    std::transform(ascii_str.cbegin(), ascii_str.cend(), lowercase_str.begin(), ::tolower);

    if(const auto it = Impl.find(lowercase_str); it != Impl.cend()) {
        return it->second;
    }

    return std::nullopt;
}