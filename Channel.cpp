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

void sendMessage(const char* response, int client_fd) {
    if(send(client_fd, response, strlen(response), 0) == -1) {
        std::cerr << "->>\tError sending response to client!" << std::endl;
    }
}

void    Channel::printChanUsers() { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "STAMPO IL MAP DEL CHAN" << std::endl;
    for(std::map<int, Client*>::const_iterator it = this->clients.begin();
    it != this->clients.end(); it++)
    {
        std::cout << it->first << ": " << it->second->getNick() << " - " << it->second->getFull() << std::endl;
    }
}

Channel::Channel(Client * client, std::string name) {
    this->name = name;
    this->clients.insert(std::make_pair(client->getFd(), client));
}


void    Channel::addClient(Client* client) {
    this->clients.insert(std::make_pair(client->getFd(), client));
    std::map<int, Client *>::iterator it = this->clients.begin();
    while (it != this->clients.end()) {
        if (it->first != client->getFd()) {
            std::string resp = ":" + client->getNick() + "!" + client->getUser() + " JOIN :" + this->name  + "\n";
            sendMessage(resp.c_str(), it->first);
        }
        it++;
    }
}

void    Channel::removeClient(Client* client) {
    std::map<int, Client *>::iterator it = this->clients.begin();
    while (it != this->clients.end()) {
        std::string resp = ":" + client->getNick() + "!" + client->getUser() + " PART " + this->name + "\n";
        sendMessage(resp.c_str(), it->first);
        it++;
    }
    this->clients.erase(client->getFd());
}

std::string Channel::getUsers() {
    std::string res = "";
    std::map<int, Client*>::const_iterator it = this->clients.begin();
    while (it != this->clients.end()) {
        res += it->second->getNick();
        res += " ";
        it++;
    }
    if (res[0] != '\0')
        res.erase(res.end() - 1);
    std::cout << "res = " << res << "|" << std::endl;
    return res;
}



Channel::~Channel() {}
