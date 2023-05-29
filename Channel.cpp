#include "Channel.hpp"

void    Channel::printChanUsers() { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "STAMPO IL MAP DEL CHAN" << std::endl;
    for(std::map<int, Client*>::const_iterator it = this->clients.begin();
    it != this->clients.end(); it++)
    {
        std::cout << it->first << ": " << it->second->getNick() << " - " << it->second->getFull() << std::endl;
    }
}

Channel::Channel(Client * client) {
    this->clients.insert(std::make_pair(client->getFd(), client));
    printChanUsers();
}

void    Channel::addClient(Client* client) {
    this->clients.insert(std::make_pair(client->getFd(), client));
    printChanUsers();
}

Channel::~Channel() {}
