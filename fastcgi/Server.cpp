#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <fcgio.h>
#include "Server.hpp"

using fastcgi::Server;

Server::Server(const char* path, int backlog,
    RequestHandler requestHandler,
    FailureHandler failureHandler
)
    : _respond(requestHandler)
    , _fail(failureHandler)
    , _active(false)
{
    if (!requestHandler)
        throw std::runtime_error("invalid request handler");

    if (0 != FCGX_Init())
        throw std::runtime_error("failed to initialize libfcgi");

    _socket = FCGX_OpenSocket(path, backlog);

    if (_socket < 0)
        throw std::runtime_error("failed to create a socket");
}

Server::~Server()
{
    stop();
}

bool Server::start(unsigned int spawn)
{
    if ((0 == spawn) && !_threads.empty() && _active)
        return false;

    _threads.resize(spawn);
    _active = true;

    for (unsigned int i = 0; i < spawn; i++)
        _threads[i] = std::thread(&Server::_loop, this, i);

    return true;
}

void Server::stop()
{
    if (_threads.empty() && !_active)
        return;

    _active = false;

    for (auto& t : _threads)
        t.join();

    _threads.clear();    
}

void Server::_loop(unsigned int threadId)
{
    Request request(_socket, FCGI_FAIL_ACCEPT_ON_INTR);

    for (;;)
    {
        int result = -1;

        _accept.lock();

         if (_active)
         {
             result = FCGX_Accept_r(&request._request);
         }

        _accept.unlock();

        if (0 != result)
        {
            if (!_active)
                break;

            if (EINTR != errno && _fail && !_fail(result, threadId))
                break;

            continue;
        }   

        fcgi_streambuf in_buf (request._request.in);
        fcgi_streambuf out_buf(request._request.out);
        fcgi_streambuf err_buf(request._request.err);

        std::istream in (&in_buf);
        std::ostream out(&out_buf);
        std::ostream err(&err_buf);

        request._inPtr  = &in;
        request._outPtr = &out;
        request._errPtr = &err;

        request._update();

        _respond(request, threadId);

        FCGX_Finish_r(&request._request);
    }
}
