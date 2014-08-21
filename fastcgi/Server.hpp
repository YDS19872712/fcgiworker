#ifndef FASTCGI_SERVER_INCLUDED
#define FASTCGI_SERVER_INCLUDED

#include <stdexcept>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <fcgiapp.h>
#include "utils.hpp"
#include "Request.hpp"

namespace fastcgi {

    class Server
    {
        DELETE_COPY(Server);
        DELETE_MOVE(Server);

    public:
    
        typedef std::function<void (Request&, unsigned int)> RequestHandler;
        
        typedef std::function<bool (int, unsigned int)> FailureHandler;

        Server(const char* path, int backlog,
            RequestHandler requestHandler,
            FailureHandler failureHandler = nullptr);

        virtual ~Server();

        bool start(unsigned int spawn = 1);

        void stop();

    private:

        RequestHandler _respond;
        
        FailureHandler _fail;

        std::atomic_bool _active;

        int _socket;

        std::mutex _accept;

        std::vector<std::thread> _threads;

        void _loop(unsigned int threadId);
    };
}

#endif // FASTCGI_SERVER_INCLUDED
