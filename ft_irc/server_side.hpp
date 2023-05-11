#ifndef SERVER_SIDE_HPP
#define SERVER_SIDE_HPP
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <strings.h>
#include <vector>
#include <sys/poll.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>


class Server {
private:
    int socket_fd;
    int opt;
    std::string port;
    std::string password;
    void setSocket();
    std::vector<pollfd> poll_vec;

public:
    Server(const std::string& port, const std::string& password);
    ~Server();
    void startServer();
};


#endif