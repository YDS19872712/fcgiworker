#ifndef FASTCGI_REQUEST_INCLUDED
#define FASTCGI_REQUEST_INCLUDED

#include <cassert>
#include <stdexcept>
#include <map>
#include <string>
#include <iostream>
#include <fcgiapp.h>
#include "utils.hpp"

namespace fastcgi {
    
    class Server;
    
    class Request
    {
        friend class Server;

        DELETE_COPY(Request);
        DELETE_MOVE(Request);

    public:
    
        typedef std::map<std::string, std::string> Variables;
        
        enum class Method { GET, HEAD, POST };
        
        Variables GET;

        Variables POST;

        Variables COOKIE;

        Method method() const;
        
        const char* httpParameter(const char* name) const
        {
            return FCGX_GetParam(name, _request.envp);
        }

        std::istream& in()
        {
            assert(_inPtr);
            return *_inPtr;
        }
        
        std::ostream& out()
        {
            assert(_outPtr);
            return *_outPtr;
        }
        
        std::ostream& error()
        {
            assert(_errPtr);
            return *_errPtr;
        }
    
    private:
    
        std::istream* _inPtr;
        
        std::ostream* _outPtr;
        
        std::ostream* _errPtr;
        
        FCGX_Request _request;

        Request(int socket, int flags = 0)
        {
            FCGX_InitRequest(&_request, socket, flags);
        }
        
        ~Request()
        {
            FCGX_Finish_r(&_request);
        }
        
        void _decodeUrlVariables(const char* encoded, Variables& variables);

        void _decodeCookies(const char* encoded, Variables& variables);

        void _update();
    };
}

#endif // FASTCGI_REQUEST_INCLUDED
