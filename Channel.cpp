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

void sendMessage(const char* response, int client_fd) {
    if(send(client_fd, response, strlen(response), 0) == -1) {
        std::cerr << "->>\tError sending response to client!" << std::endl;
    }
}

void    Channel::addAdmin(Client* client) {
    this->admins.insert(std::make_pair(client->getFd(), client));
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
    this->topic = ":Welcome to " + name;
    this->freeTopic = false;
    this->needPassword = false;
    this->inviteOnly = false;
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
            std::string resp = ":" + client->getNick() + "!" + client->getUser() + " JOIN :" + this->name  + "\n";
            sendMessage(resp.c_str(), it->first);
        }
        it++;
    }
    printChanUsers();
}

void    Channel::removeClient(Client* client) {
    std::map<int, Client *>::iterator it = this->clients.begin();
    while (it != this->clients.end()) {
        std::string resp = ":" + client->getNick() + "!" + client->getUser() + " PART " + this->name + "\n";
        sendMessage(resp.c_str(), it->first);
        it++;
    }
    removeFromChan(client->getFd());
}

void    Channel::changeTopic(const std::string& topic, int changer) {
    std::string resp;
    Client* conn_client = this->clients.at(changer);
    if (this->admins.find(changer) == this->admins.end()) {
        resp = ":SovietServ 482 " + conn_client->getNick() + " " + this->name + " :You do not have access to change the topic on this channel\n";
        sendMessage(resp.c_str(), changer);  
    }
    else {
        resp = ":" + conn_client->getNick() + "!" + conn_client->getUser() + " TOPIC " + this->name + " " + topic + "\n";
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
    std::cout << "ESCO DAL WHILE" << std::endl;
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
    std::cout << "STAMPO DOPO LEAVE" << std::endl;
    this->printChanUsers();
    std::cout << "FINE STAMPA DOPO IL LEAVE" << std::endl;
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

Channel::~Channel() {}
