#ifndef ____________DS_UTILS_H
#define ____________DS_UTILS_H
#include "json/json.h"

/// tools to split/join long json string to array

Json::Value SplitToStringArray(const std::string& buf);
std::string JoinArrayToString(const Json::Value &v);

#endif
