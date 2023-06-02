#include "Channel.hpp"
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
#include "Server.hpp"

Channel::~Channel() {
    // std::cout << "distruttore in azione" << std::endl;
    // std::map<int, Client *>::iterator it = this->admins.begin();
    // while (it != this->admins.end()) {
    //     delete it->second;
    //     it++;
    // }
    // std::map<int, Client *>::iterator ita = this->clients.begin();
    // while (ita != this->clients.end()) {
    //     delete ita->second;
    //     ita++;
    // }
}

void sendMessage(const char* response, int client_fd) {
    if(send(client_fd, response, strlen(response), 0) == -1) {
        std::cerr << "->>\tError sending response to client!" << std::endl;
    }
}

void    Channel::addAdmin(Client* client) {
    this->admins.insert(std::make_pair(client->getFd(), client));
}

void    Channel::sendToAllusersExcept(const std::string& message, int client) {
    std::map<int, Client *>::iterator it = this->clients.begin();
    while (it != this->clients.end()) {
        if (it->first != client) {
            sendMessage(message.c_str(), it->first);
        }
        it++;
    }
}


void    Channel::printChanUsers() { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "STAMPO IL MAP DEL CHAN" << std::endl;
    for(std::map<int, Client*>::const_iterator it = this->clients.begin();
    it != this->clients.end(); it++)
    {
        std::cout << it->first << ": " << it->second->getNick() << " - " << it->second->getFull() << std::endl;
    }
    for(std::map<int, Client*>::const_iterator it = this->admins.begin();
    it != this->admins.end(); it++)
    {
        std::cout << "admin" << it->first << ": " << it->second->getNick() << " - " << it->second->getFull() << std::endl;
    }
}

Channel::Channel(Client * client, std::string name) {
    this->name = name;
    this->clients.insert(std::make_pair(client->getFd(), client));
    this->topic = ":Welcome to " + this->name;
    this->freeTopic = false;
    this->needPassword = false;
    this->inviteOnly = false;
    this->userNrLimited = false;
    if (this->admins.empty()) {
        this->addAdmin(client);
    }
}


void    Channel::addClient(Client* client) {
    std::map<int, Client *>::iterator it = this->clients.begin();
    if (this->admins.empty() == true && this->clients.empty() == true) {
        this->addAdmin(client);
    }
    this->clients.insert(std::make_pair(client->getFd(), client));
    while (it != this->clients.end()) {
        if (it->first != client->getFd()) {
            std::string resp = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getIp() + " JOIN :" + this->name  + "\n";
            sendMessage(resp.c_str(), it->first);
        }
        it++;
    }
}

void    Channel::removeClient(Client* client, const std::string& message) {
    std::map<int, Client *>::iterator it = this->clients.begin();
    while (it != this->clients.end()) {
        std::string resp = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getIp() + " PART " + this->name + " " + message + "\n";
        sendMessage(resp.c_str(), it->first);
        it++;
    }
    removeFromChan(client->getFd());
}

void    Channel::changeTopic(const std::string& topic, int changer) {
    std::string resp;
    Client* conn_client = this->clients.at(changer);
    if (this->admins.find(changer) == this->admins.end() && this->freeTopic == false) {
        resp = ":SovietServ 482 " + conn_client->getNick() + " " + this->name + " :You do not have access to change the topic on this channel\n";
        sendMessage(resp.c_str(), changer);  
    }
    else {
        resp = ":" + conn_client->getNick() + "!" + conn_client->getUser() + "@" + conn_client->getIp() + " TOPIC " + this->name + " " + topic + "\n";
        std::map<int, Client *>::iterator it = this->clients.begin();
        this->topic = topic;
        while (it != this->clients.end()) {
            sendMessage(resp.c_str(), it->first);
            it++;
        }
    }
}

bool     Channel::checkIfAdmin(int user) {
    if (this->admins.find(user) == this->admins.end())
        return false;
    return true;
}

std::string Channel::getUsers() {
    std::string res = "";
    std::map<int, Client*>::const_iterator ita = this->admins.begin();
    while (ita != this->admins.end()) {
        res += "@";
        res += ita->second->getNick();
        res += " ";
        ita++;
    }
    std::map<int, Client*>::const_iterator it = this->clients.begin();
        while (it != this->clients.end()) {
            if (this->admins.find(it->first) == this->admins.end()) {
                res += it->second->getNick();
                res += " ";
            }
            it++;
        }
    if (res[0] != '\0')
        res.erase(res.end() - 1);
    return res;
}

std::string Channel::getTopic() {
    return this->topic;
}

void    Channel::removeFromChan(int user) {
    if (this->isUserAdmin(user) == true) {

        this->admins.erase(user);
    }
    this->clients.erase(user);
}

bool    Channel::isUserInChan(int user) {
    if (this->clients.find(user) != this->clients.end()) {
        return true;
    }
    return false;
}

bool    Channel::isUserAdmin(int user) {
    if (this->admins.find(user) != this->admins.end()) {
        return true;
    }
    return false;
}

bool    Channel::isEmpty() {
    if (this->clients.empty() == true) {
        return true;
    }
    return false;
}

bool    Channel::getFreeTopic() {
    return this->freeTopic;
}

bool    Channel::getInviteOnly() {
    return this->inviteOnly;
}

bool    Channel::getNeedPassword() {
    return this->needPassword;
}

void    Channel::setFreeTopic(bool arg) {
    this->freeTopic = arg;
}

void    Channel::setInviteOnly(bool arg) {
    this->inviteOnly = arg;
}

void    Channel::setNeedPassword(bool arg, const std::string& password) {
    if (arg == true) {
        this->needPassword = true;
        this->password = password;
    }
    else {
        this->needPassword = false;
        this->password = "";
    }
}

void    Channel::addToInvited(const std::string& nick) {
    this->invited.push_back(nick);
}

void    Channel::removeToInvited(const std::string& nick) {
    std::vector<std::string>::iterator it = this->invited.begin();
    while (it != this->invited.end()) {
        if (*it == nick) {
            it->erase();
        }
        it++;
    }
}

bool    Channel::isInvited(const std::string& nick) {
    std::vector<std::string>::iterator it = this->invited.begin();
    while (it != this->invited.end()) {
        if (*it == nick) {
           return true;
        }
        it++;
    }
    return false;
}

void    Channel::setPassword(const std::string& password) {
    this->password = password;
}

std::string&    Channel::getPassword() {
    return this->password;
}

void    Channel::removeFromAdmin(int client) {
    this->admins.erase(client);
}

int             Channel::getMaxUsers() {
    return this->maxUsers;
}

bool            Channel::getUserNrLimited() {
    return this->userNrLimited;
}

void            Channel::setMaxUsers(int nr) {
    this->maxUsers = nr;
}
void            Channel::setUserNrLimited(bool arg) {
    this->userNrLimited = arg;
}

