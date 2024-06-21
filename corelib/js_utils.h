#pragma once

/// tools to split/join long json string to array

#include "json/value.h"
Json::Value SplitToStringArray(const std::string& buf);
std::string JoinArrayToString(const Json::Value &v);

