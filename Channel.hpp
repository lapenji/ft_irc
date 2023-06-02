#pragma once
#include "Client.hpp"
#include <map>
#include <vector>

class Channel
{
private:
    std::string password;
    std::map<int, Client *> admins;
    std::string name;
    std::string topic;
    bool inviteOnly;
    bool freeTopic;
    bool needPassword;
    std::vector<std::string> invited;
    int maxUsers;
    bool userNrLimited;

public:
    std::map<int, Client *> clients; // DA RIMETTERE PRIVATO
    Channel(Client *client, std::string name);
    ~Channel();
    std::string ft_353_366(Client *client, const char *str);
    void addClient(Client *client);
    void printChanUsers();
    std::string getUsers();
    std::string getTopic();
    void addAdmin(Client *client);
    void removeClient(Client *client, const std::string &message);
    void changeTopic(const std::string &topic, int changer);
    bool checkIfAdmin(int user);
    void removeFromChan(int user);
    bool isUserInChan(int user);
    bool isUserAdmin(int user);
    bool isEmpty();
    void sendToAllusersExcept(const std::string &message, int client);
    bool getFreeTopic();
    bool getInviteOnly();
    bool getNeedPassword();
    void setFreeTopic(bool arg);
    void setInviteOnly(bool arg);
    void setNeedPassword(bool arg, const std::string &password);
    void addToInvited(const std::string &nick);
    void removeToInvited(const std::string &nick);
    bool isInvited(const std::string &nick);
    void setPassword(const std::string &password);
    void removeFromAdmin(int client);
    std::string &getPassword();
    int getMaxUsers();
    bool getUserNrLimited();
    void setMaxUsers(int nr);
    void setUserNrLimited(bool arg);
    std::string getUsersAsEq();
};
