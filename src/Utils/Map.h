/**
 * @file    Map.h
 * @ingroup Utils
 * @brief   Template helper functions for std::map
 *
 * Copyright (c) 2017 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <sstream>
#include <string>


namespace Utils {

/// Tells if a std::map have a specific key (similar to std::map::count())
template <typename Map>
bool hasKey(const Map& aMap, const std::string& aKey) {
    return (aMap.find(aKey) != aMap.end());
}

/// Tells if a std::map have a specific value
template <typename Map>
bool hasValue(const Map& aMap, const std::string& aKey, const std::string& aValue) {
    bool bHaveValue = false;
    const typename Map::const_iterator iPair = aMap.find(aKey);
    if (iPair != aMap.end()) {
        bHaveValue = (iPair->second == aValue);
    }
    return bHaveValue;
}

} // namespace Utils
