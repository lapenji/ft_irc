#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
#include <time.h>
#include "Client.hpp"
#include "utils.hpp"

#include <arpa/inet.h>

extern bool run;

class Server
{
private:
    int socket_fd;
    int opt;
    int port;
    std::string password;
    std::vector<pollfd> poll_vec;
    std::map<int, Client *> connected_clients;
    std::map<std::string, Channel *> channels;
    std::vector<std::string> insulti;
    int nrInsulti;

    void ft_print_motd(int client_fd);
    void serverReplyMessage(const char *response, int client_fd);
    int handle_client_request(int client_fd);
    void setSocket();
    void ft_manage_nick(const std::string &tmp, int client_fd, Client *client);
    bool ft_manage_user(const std::string &tmp, int client_fd, Client *client);
    bool ft_manage_pass(const std::string &tmp);
    void ft_manage_mode(const std::string &tmp, int client_fd);
    void ft_manage_join(const std::string &tmp, int client_fd, Client *client);
    void ft_manage_ping(const std::string &tmp, int client_fd);
    void ft_manage_part(const std::string &tmp, Client *client);
    void ft_manage_privmsg(const std::string &tmp, int client_fd);
    void ft_manage_invite(const std::string &tmp, int client_fd);
    void ft_manage_topic(const std::string &tmp, int client_fd);
    void ft_create_map_user(std::vector<std::string> result, int client_fd);
    void ft_delete_client(int client_fd);
    void ft_print_welcome_msg(int client_fd, Client *client);
    void ft_refresh_chan(std::string channel, std::string joiner);
    void ft_manage_kick(const std::string &tmp, int client_fd);
    int find_client(const std::string &name);
    bool isNickInUse(const std::string &nick);
    void ft_manage_i(const std::string &resp, int client_fd, Channel *chan, char c);
    void ft_manage_o(const std::string &resp, int client_fd, Channel *chan, std::vector<std::string> tmp_splitted);
    void ft_manage_t(const std::string &resp, int client_fd, Channel *chan, char c);
    void ft_manage_l(const std::string &first_part, int client_fd, Channel *chan, std::vector<std::string> tmp_splitted);
    void ft_manage_k(const std::string &resp, int client_fd, Channel *chan, std::vector<std::string> tmp_splitted);
    std::string ft_resp_at(const std::string arg, int client_fd);                 ////////////
    void ft_reply(const std::string &num, const std::string &arg, int client_fd); //////////
    void ft_print_topic(Channel *chan, const std::string &channel, int client_fd);
    void ft_manage_quit(const std::string &tmp, int client_fd);
    void ft_manage_who(const std::string &tmp, int client_fd, const std::string &nick);
    void ft_manage_userhost(const std::string &tmp, int client_fd, Client *client);
    void ft_manage_b(const std::string chan, int client_fd);
    void ft_initialize_bot();
    void ft_bot_insult_someone(const std::string &nick, const std::string &chan, int client_fd);
    std::string ft_bot_get_insult();
    void ft_bot_no_user_in_chan(const std::string &nick, const std::string &chan, int client_fd);
    void ft_bot_insult_bot(const std::string &nick, const std::string &chan, int client_fd);
    void ft_bot_op(int client_fd, const std::string &chan, const std::string &nick);
    void ft_multiple_join(std::vector<std::string> chans, int client_fd, Client *client);
    void ft_print_banner();

public:
    Server(int port, const std::string &password);
    ~Server();
    void startServer();
};

#endif
