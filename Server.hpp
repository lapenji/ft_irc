#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
//#include "Channel.hpp"
#include "Client.hpp"
#include "utils.hpp"

class Server {
private:
    int socket_fd;
    int opt;
    int port;
    std::string password;
    std::vector<pollfd> poll_vec;
    std::map<int, Client *> connected_clients;
    std::map<std::string, Channel *> channels;
    
    void    serverReplyMessage(const char* response, int client_fd);
    int     handle_client_request(int client_fd);
    void    setSocket();
    void    ft_manage_nick(const std::string& tmp, int client_fd/* , std::string& resp */);
    bool    ft_manage_user(const std::string& tmp, int client_fd/* , std::string& resp */);
    bool    ft_manage_pass(const std::string& tmp);
    void    ft_manage_mode(const std::string& tmp, int client_fd);
    void    ft_manage_join(const std::string& tmp, int client_fd);
    void    ft_manage_ping(const std::string& tmp, int client_fd);
    void    ft_manage_part(const std::string& tmp, int client_fd);
    void    ft_manage_privmsg(const std::string& tmp, int client_fd);
    void    ft_manage_invite(const std::string& tmp, int client_fd);
    void    ft_manage_topic(const std::string& tmp, int client_fd);
    void    ft_create_map_user(std::vector<std::string> result, int client_fd);
    bool    sendAll(const char* resp);
    void    ft_delete_client(int client_fd);
    void    ft_print_welcome_msg(int client_fd);
    void    ft_refresh_chan(std::string channel, std::string joiner);
    void    ft_manage_kick(const std::string& tmp, int client_fd);
    int     find_client(const std::string& name);

public:
    Server(int port, const std::string& password);
    ~Server();
    void    startServer();
};

std::string ft_joinStr(std::vector<std::string> result, int i);

#endif