#include "LoaderUtils.h"
#include <algorithm>
#include <cctype>
#include <string>

inline std::string trim(const std::string& s)
{
    auto start = std::find_if_not(s.begin(), s.end(), ::isspace);
    auto end = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    if (start >= end)
        return "";
    return std::string(start, end);
}
