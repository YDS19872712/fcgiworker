#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>
#include "Request.hpp"

using fastcgi::Request;

static const std::streamsize MAX_IN = 0x100000;

Request::Method Request::method() const
{
    const char* request_method = httpParameter("REQUEST_METHOD");

    if (!std::strcmp(request_method, "GET"))
        return Method::GET;

    if (!std::strcmp(request_method, "HEAD"))
        return Method::HEAD;

    if (std::strcmp(request_method, "POST"))
        throw std::runtime_error("unsupported request method");

    return Method::POST;
}

void Request::_decodeUrlVariables(const char* encoded, Variables& variables)
{    
    enum class State
    {
        READING_KEY,
        READING_VALUE
    };

    std::pair<std::string, std::string> item;

    State state = State::READING_KEY;

    auto flush_item = [&]()
    {
        item.first  = utils::unescape(item.first);
        item.second = utils::unescape(item.second);

        variables.insert(item);

        item.first.clear();
        item.second.clear();
    };

    for (const char* p = encoded; 0 != *p; p++)
    {
        switch (*p)
        {
        default: 
            switch (state)
            {
            case State::READING_KEY:
                item.first += *p;
                break;

            case State::READING_VALUE:
                item.second += *p;
                break;
            }
            break;

        case '=':
            state = State::READING_VALUE;
            break;

        case '&':
        case ';':
            flush_item();
            state = State::READING_KEY;
            break;
        }
    }

    if (!item.first.empty())
        flush_item();
}

void Request::_decodeCookies(const char* encoded, Variables& variables)
{
    auto is_acceptable = [](char ch) -> bool
    {
        return ((ch >= '!') &&
                (ch <= '~') &&
                (ch != ',') &&
                (ch != ';') &&
                (ch != '='));
    };

    std::pair<std::string, std::string> item;

    const char* p = encoded;

    auto skip_spaces = [&]()
    {
        while (' ' == *p)
            p++;            
    };

    while (0 != *p)
    {
        skip_spaces();

        while (is_acceptable(*p))
            item.first += *(p++);

        skip_spaces();

        if ('=' == *p)
        {
            p++;

            skip_spaces();

            while (is_acceptable(*p))
                item.second += *(p++);

            skip_spaces();

            if (';' == *p)
                p++;
        }
        else
        {
            break;
        }

        item.first = utils::unescape(item.first);
        item.second = utils::unescape(item.second);

        variables.insert(item);

        item.first.clear();
        item.second.clear();
    }
}

void Request::_update()
{
    GET.clear(); POST.clear(); COOKIE.clear();

    const char* query_string = httpParameter("QUERY_STRING");

    if (query_string)
        _decodeUrlVariables(query_string, GET);

    if (Method::POST == method())
    {
        const char* content_length = httpParameter("CONTENT_LENGTH");

        const char* content_type = httpParameter("CONTENT_TYPE");

        if (content_length &&
            !std::strcmp("application/x-www-form-urlencoded", content_type))
        {
            assert(_inPtr);

            std::streamsize length = static_cast<std::streamsize>(
                std::strtol(content_length, nullptr, 10)
            );

            if (length > 0)
            {
                if (length > MAX_IN)
                    length = MAX_IN;

                std::unique_ptr<char[]> content(new char[length + 1]);

                _inPtr->read(content.get(), length);
                content[length] = 0;
            
                _decodeUrlVariables(content.get(), POST);
            }
        }
    }

    const char* http_cookie = httpParameter("HTTP_COOKIE");
    
    if (http_cookie)
        _decodeCookies(http_cookie, COOKIE);
}
