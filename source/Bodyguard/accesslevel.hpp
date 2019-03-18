// Copyright (C) 2019 Eric Mitchem <ericmitchem@gmail.com>. All Rights Reserved.

#pragma once

#include <string>

enum class AccessLevel
{
    Owner,
    Tribe,
    Alliance,
    All
};

std::string AccessLevelToString(AccessLevel access_level);
bool StringToAccessLevel(const std::string& ascii_string, AccessLevel& access_level);