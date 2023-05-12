#ifndef SERVER_SIDE_HPP
#define SERVER_SIDE_HPP
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <strings.h>
#include <vector>
#include <sys/poll.h>
#include <map>
#include "Client.hpp"
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sstream>


class Server {
private:
    int socket_fd;
    int opt;
    std::string port;
    std::string password;
    std::vector<pollfd> poll_vec;
    std::map<int, Client *> connected_clients;
    int     handle_client_request(int client_fd);
    void    setSocket();
    void   ft_manage_nick(const std::string& tmp, int client_fd, std::string& resp);
    void    ft_manage_user(const std::string& tmp, int client_fd, std::string& resp);
    void    ft_create_map_user(std::vector<std::string> result, int client_fd);
    bool    sendAll(const char* resp);
    void    ft_manage_quit(const std::string& tmp, int fd);
public:
    Server(const std::string& port, const std::string& password);
    ~Server();
    void    startServer();
};


#endif