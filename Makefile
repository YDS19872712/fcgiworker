CC = g++
CFLAGS = -g -std=c++11
LDFLAGS = -lfcgi++ -lfcgi

default: all

all: main.o Server.o Request.o Response.o utils.o
	$(CC) $(CFLAGS) -o worker main.o Server.o Request.o Response.o utils.o $(LDFLAGS)

main.o: main.cpp fastcgi/Server.cpp fastcgi/Request.cpp fastcgi/Response.cpp fastcgi/utils.cpp logging.hpp fastcgi/Server.hpp fastcgi/Request.hpp fastcgi/Response.hpp fastcgi/utils.hpp
	$(CC) $(CFLAGS) -c main.cpp

Server.o: fastcgi/Server.cpp fastcgi/Server.hpp fastcgi/Request.hpp fastcgi/utils.hpp
	$(CC) $(CFLAGS) -c fastcgi/Server.cpp

Request.o: fastcgi/Request.cpp fastcgi/Request.hpp fastcgi/utils.hpp
	$(CC) $(CFLAGS) -c fastcgi/Request.cpp

Response.o: fastcgi/Response.cpp fastcgi/Response.hpp fastcgi/Request.hpp fastcgi/utils.hpp
	$(CC) $(CFLAGS) -c fastcgi/Response.cpp

utils.o: fastcgi/utils.hpp
	$(CC) $(CFLAGS) -c fastcgi/utils.cpp

clean: 
	$(RM) worker *.o *~
