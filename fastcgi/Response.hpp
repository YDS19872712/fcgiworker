#ifndef FASTCGI_RESPONSE_INCLUDED
#define FASTCGI_RESPONSE_INCLUDED

#include <cassert>
#include <cstdint>
#include <ctime>
#include <map>
#include <string>
#include <sstream>
#include "utils.hpp"
#include "Request.hpp"

namespace fastcgi {
    
    class Response
    {
    public:

        static const char* MIME_HTML;

        static const char* MIME_JSON;

        static const char* CHARSET_UTF8;

        static const char* CHARSET_CP1251;

        explicit Response(Request& request)
            : _rendered(false)
            , _request(request)
            , _contentType(MIME_HTML)
            , _charset(CHARSET_UTF8) {};

        void setContentType(const char* type)
        {
            assert(MIME_HTML == type || MIME_JSON == type);
            _contentType = type;
        }
        
        void setCharset(const char* charset)
        {
            assert(CHARSET_UTF8 == charset || CHARSET_CP1251 == charset);
            _charset = charset;
        }
        
        void setCookie(
            const std::string& name,
            const std::string& value,
            std::time_t maxAge = 0,
            const std::string& domain = "",
            const std::string& path = "",
            bool secure = false,
            bool httpOnly = false);

        void setValue(const char* name, std::int64_t value)
        {
            _integers[name] = value;
        }

        void setValue(const char* name, const std::string& value)
        {
            _strings[name] = value;
        }

        bool renderTemplate(const std::string& outTemplate);

    private:

        bool _rendered;

        Request& _request;

        const char* _contentType;
        
        const char* _charset;
        
        std::ostringstream _cookies;

        std::map<std::string, std::int64_t> _integers;

        std::map<std::string, std::string> _strings;
    };
}

#endif // FASTCGI_RESPONSE_INCLUDED
