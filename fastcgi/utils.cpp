#include "utils.hpp"

std::string fastcgi::utils::escape(const std::string& value)
{
    char encoded_char[4] = {'%', 0, 0, 0};
    
    std::string result;
    
    for (auto cursor = value.begin(); value.end() != cursor; ++cursor)
    {
        if ((*cursor >= 'A' && *cursor <= 'Z') ||
            (*cursor >= 'a' && *cursor <= 'z') ||
            (*cursor >= '0' && *cursor <= '9') ||
            '-' == *cursor || '_' == *cursor ||
            '.' == *cursor || '~' == *cursor)
        {
            result += *cursor;
        }
        else if (' ' == *cursor)
        {
            result += '+';
        }
        else
        {
            const char* HEX_DIGIT = "0123456789ABCDEF";
            
            encoded_char[1] = HEX_DIGIT[(*cursor & 0xf0) >> 4];
            encoded_char[2] = HEX_DIGIT[(*cursor & 0x0f)];

            result.append(encoded_char);
        }
    }

    return result;
}

std::string fastcgi::utils::unescape(const std::string& value)
{
    std::string result;
    
    for (auto cursor = value.begin(); value.end() != cursor; ++cursor)
    {
        if ('%' == *cursor &&
            value.end() != cursor + 1 &&
            value.end() != cursor + 2)
        {
            auto convert = [](char ch) -> char
            {
                if ((ch >= '0') && (ch <= '9'))
                    return ch - '0';

                if ((ch >= 'A') && (ch <= 'F'))
                    return ch - 'A' + 10;

                if ((ch >= 'a') && (ch <= 'f'))
                    return ch - 'a' + 10;

                return 0;
            };
            
            result += convert(*(cursor + 1)) << 4 | convert(*(cursor + 2));
            cursor += 2;
        }
        else if ('+' == *cursor)
        {
            result += ' ';
        }
        else
        {
            result += *cursor;
        }
    }
    
    return result;
}
