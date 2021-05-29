#pragma once

#include <string>


const std::string whiteSpace = " \t\r\n\v\f";

namespace std
{
    inline string& TrimLeft(string& str, string& chars)
    {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    inline string& TrimRight(string& str, string& chars)
    {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    inline string& TrimLeftWhitespace(string& str)
    {
        str.erase(0, str.find_first_not_of(whiteSpace));
        return str;
    }

    inline string& TrimRightWhitespace(string& str)
    {
        str.erase(str.find_last_not_of(whiteSpace) + 1);
        return str;
    }
}

