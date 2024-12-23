
#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <cctype>


namespace ClvLib {

// http://www.gotw.ca/gotw/029.htm
struct ci_char_traits : std::char_traits<char>
{
    static bool eq(char c1, char c2)
    {
        return toupper(c1) == toupper(c2);
    }

    static bool ne(char c1, char c2)
    {
        return toupper(c1) != toupper(c2);
    }

    static bool lt(char c1, char c2)
    {
        return toupper(c1) < toupper(c2);
    }

    static int compare(const char *s1,
                       const char *s2,
                       size_t n)
    {
        while (n-- && s1 && s2 && eq(*s1++, *s2++));            
        return *s1 - *s2;
    }

    static const char *
    find(const char *s, int n, char a)
    {
        while (n-- > 0 && ne(*s, a))
        {
            ++s;
        }
        return s;
    }
};

// Case insensitive but case-preserving string types

using ci_string = std::basic_string<char, ci_char_traits>;
using ci_string_view = std::basic_string_view<char, ci_char_traits>;

// Conversions
std::string to_string(ci_string_view str)
{
    return std::string(str.data(), str.length());
}

ci_string to_ci_str(std::string_view str)
{
    return ci_string(str.data(), str.length());
}

} // namespace ClvLib

namespace std {
template <>
struct hash<ClvLib::ci_string>
{
    size_t operator()(const ClvLib::ci_string &str) const
    {
        std::string up(str.length(), '\0');
        std::transform(str.begin(), str.end(), up.begin(), [](auto c)
                       { return std::toupper(c); });
        return std::hash<std::string>()(up);
    }
};
template <>
struct hash<ClvLib::ci_string_view>
{
    size_t operator()(const ClvLib::ci_string_view &str) const
    {
        std::string up(str.length(), '\0');
        std::transform(str.begin(), str.end(), up.begin(), [](auto c)
                       { return std::toupper(c); });
        return std::hash<std::string>()(up);
    }
};

} // namespace std
