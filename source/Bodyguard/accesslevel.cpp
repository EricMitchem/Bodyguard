// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#include <algorithm>
#include <cctype>
#include <map>
#include "accesslevel.hpp"

std::string AccessLevelToString(AccessLevel access_level)
{
    static std::map<AccessLevel, std::string> Impl = {
        {AccessLevel::Owner, "owner"},
        {AccessLevel::Tribe, "tribe"},
        {AccessLevel::Alliance, "alliance"},
        {AccessLevel::All, "all"}
    };

    return Impl.find(access_level)->second;
}

bool StringToAccessLevel(const std::string& ascii_str, AccessLevel& access_level)
{
    static std::map<std::string, AccessLevel> Impl = {
        {"owner", AccessLevel::Owner},
        {"tribe", AccessLevel::Tribe},
        {"alliance", AccessLevel::Alliance},
        {"all", AccessLevel::All}
    };

    std::string lowercase_str;

    std::transform(ascii_str.cbegin(), ascii_str.cend(), lowercase_str.begin(), ::tolower);

    auto it = Impl.find(lowercase_str);

    if(it != Impl.end()) {
        access_level = it->second;
        return true;
    }

    return false;
}