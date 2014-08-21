#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "fastcgi/Server.hpp"
#include "fastcgi/Request.hpp"
#include "fastcgi/Response.hpp"
#include "logging.hpp"

int main(int argc, char **argv)
{
    try
    {
        fastcgi::Server s( "/tmp/worker.sock", 500, 
            [](fastcgi::Request& request, unsigned int id)
            {
                using namespace fastcgi;

                Response response(request);

                response.setValue("title", "Hello!");
                response.setValue("content", request.GET["content"]);

                if (request.POST["c"] == "on")
                    response.setCookie("cv", request.POST["cv"], 10);

                response.setValue("cv", request.COOKIE["cv"]);
                LOGF_INFO("cookie = %s", request.COOKIE["cv"].c_str());

                response.renderTemplate(
                    "<html>"
                    "<head><title>{{ title }}</title></head>"
                    "<body>"
                    "<font color=\"darkred\">{{ content }}</font><br>"
                    "<form action=\".\" method=\"post\">"
                    "<input type=\"checkbox\" name=\"c\">Set cookie?<br>"
                    "Value: <input type=\"text\" name=\"cv\"><br>"
                    "<input type=\"submit\" value=\"submit\">"
                    "</form>"
                    "</body>"
                    "</html>"
                );
            },
            [](int error, unsigned int threadId) -> bool
            {
                LOGF_ERROR(
                    "Server error, "
                    "code %i thread %d\n", error, threadId);
                return true;
            });

        s.start(30);

        std::string st;
        std::cin >> st; // input anything

        s.stop();
    }
    catch (std::runtime_error& err)
    {
        std::cout << err.what() << std::endl;
    }

    return 0;
}
