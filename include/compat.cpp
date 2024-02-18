#include "compat.hpp"

// Windows required includes.
#ifdef TRAACKER_WINDOWS_BUILD
#include <algorithm>
#endif

/**
 * compat.cpp
 * 
 * Defines a variety of required operating-system compatibility functions.
 * Specifically windows path normalization.
 * Windows sucks.
 */

#ifdef TRAACKER_WINDOWS_BUILD
void aa::normalize_path(std::string& str)
{
    std::replace(str.begin(), str.end(), '\\', '/');
}
#else
void aa::normalize_path(std::string&) {}
#endif