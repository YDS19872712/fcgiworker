#include "Response.hpp"

using fastcgi::Response;

const char* Response::MIME_HTML = "text/html";

const char* Response::MIME_JSON = "application/json";

const char* Response::CHARSET_UTF8 = "utf-8";

const char* Response::CHARSET_CP1251 = "windows-1251";

void Response::setCookie(
    const std::string& name,
    const std::string& value,
    std::time_t maxAge,
    const std::string& domain,
    const std::string& path,
    bool secure,
    bool httpOnly)
{
    using utils::escape;

    _cookies << "Set-Cookie: " << escape(name) << "=" << escape(value);

    if (maxAge)
        _cookies << "; Max-Age=" << maxAge;

    if (!domain.empty())
        _cookies << "; Domain=" << escape(domain);
        
    if (!path.empty())
        _cookies << "; Path=" << escape(path);
        
    if (secure)
        _cookies << "; Secure";
    
    if (httpOnly)
        _cookies << "; HttpOnly";
    
    _cookies << "\r\n";
}

bool Response::renderTemplate(const std::string& outTemplate)
{
    if (_rendered)
        return false;

    std::ostringstream out;
    
    auto begin = outTemplate.begin();
    auto end = outTemplate.end();
    
    auto find_tag = [&](decltype(begin) cursor, const char* tag)
    {
        while (cursor != end)
        {
            auto next = cursor + 1;

            if (tag[0] == *cursor && next != end && tag[1] == *next)
                break;

            cursor = next;
        }
        
        return cursor;
    };
    
    while (begin != end)
    {
        auto opening_tag = find_tag(begin, "{{");
        
        out << std::string(begin, opening_tag);

        if (opening_tag == end)
        {
            _rendered = true;
            break;
        }

        auto closing_tag = find_tag(opening_tag + 2, "}}");
        
        if (closing_tag == end)
            break;
            
        std::string name(opening_tag + 2, closing_tag);
        
        auto strval = _strings.find(name);
        
        if (_strings.end() != strval)
        {
            out << strval->second;
        }
        else
        {
            auto intval = _integers.find(name);
            
            if (_integers.end() != intval)
            {
                out << intval->second;
            }
            else
            {
                break;
            }
        }

        begin = closing_tag + 2;
    }

    _request.out() << "Content-Type: " << _contentType;
    _request.out() << "; charset=" << _charset << "\r\n";

    _request.out() << "Content-Length: " << out.str().length() << "\r\n";
    
    _request.out() << _cookies.str();

    _request.out() << "\r\n" << out.str();

    return _rendered;
}
