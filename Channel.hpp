#pragma once
#include "Client.hpp"
#include <map>

class Channel {
private:
    std::string             password;
    std::map<int, Client *> admins;
    std::string             name;
    std::string             topic;
public:
    std::map<int, Client *> clients;
    Channel(Client * client, std::string name);
    ~Channel();
    std::string ft_353_366(Client* client, const char* str);
    void    addClient(Client* client);
    void    printChanUsers();
    std::string getUsers();
    std::string getTopic();
    void    addAdmin(Client* client);
    void    removeClient(Client* client);
    void    changeTopic(const std::string& topic, int changer);
    bool    checkIfAdmin(int user);
    void    removeFromChan(int user);
    bool    isUserInChan(int user);
};
