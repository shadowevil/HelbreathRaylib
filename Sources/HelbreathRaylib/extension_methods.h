#pragma once
#include <string>
#include <algorithm>

namespace ext {
    inline std::string to_lower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return s;
    }
}