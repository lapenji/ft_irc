#include "utils.hpp"

bool isStringNumeric(const std::string& str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> ft_splitString(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string word;
    while (ss >> word) {
        result.push_back(word);
    }
    return result;
}

std::vector<std::string> ft_splitBuffer(std::string tmp) {
    size_t pos = tmp.find("\n");
    std::vector<std::string> buffer_splitted;
    while (pos != std::string::npos) {
        buffer_splitted.push_back(tmp.substr(0, pos));
        tmp = tmp.substr(pos + 1);
        pos = tmp.find("\n");
    }
    // Stampa le stringhe separate
   /*  std::cout << "STAMPO" << std::endl;
    for (size_t i = 0; i < buffer_splitted.size(); ++i) {
    std::cout << buffer_splitted[i] << std::endl;
    } */
    return buffer_splitted;
}

std::string ft_joinStr(std::vector<std::string> result, int i) {
    std::string tmp = "";
    std::vector<std::string>::iterator it = result.begin() + i;
        while (it != result.end()) {
            //std::cout << "entro e stampo " << *it << std::endl;
            tmp += *it;
            if (it + 1 != result.end()) {
                tmp += " ";
            }
            it++;
        }
    return tmp;
}

void    printMap(std::map<std::string, Channel*> myMap ) { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "\nSTAMPO IL MAP" << std::endl;
    for(std::map<std::string, Channel*>::const_iterator it = myMap.begin(); it != myMap.end(); ++it)
    {
        std::cout << it->first << std::endl;
    }
}

int Server::find_client(const std::string& name) {
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    while (it != this->connected_clients.end()) {
            if (it->second->getNick() == name) {
                return it->first;
            }
            it++;
    }
    return -1;
}

bool    Server::isNickInUse(const std::string &nick) {
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    while (it != this->connected_clients.end()) {
        if (it->second->getNick() == nick) {
            return true;
        }
        it++;
    }
    return false;
}

void Server::ft_delete_client(int client_fd) {
    std::map<int, Client *>::iterator map_it = this->connected_clients.begin();
    while (map_it != this->connected_clients.end()) {
        if (map_it->first == client_fd) {
            delete map_it->second;
            this->connected_clients.erase(map_it);
            break;
        }
        map_it++;
    }
    std::cout << "\033[1;32m-->> QUITTATO IL CLIENT\033[0m" << std::endl;
}

void    Server::ft_create_map_user(std::vector<std::string> result, int client_fd) {
    Client* client = this->connected_clients.at(client_fd);
    client->setUserName(result[1]);
    client->setHostname(result[2]);
    client->setServerName(result[3]);
    if (result.size() > 5) {
        std::string tmp = ft_joinStr(result, 1);
        client->setFullName(tmp);   
    }
    else
        client->setFullName(result[4]);        ///////NON SI SA SE SERVE O NO, PER ME NO
}


void Server::serverReplyMessage(const char* response, int client_fd) {
    if(send(client_fd, response, strlen(response), 0) == -1) {
        std::cerr << "->>\tError sending response to client!" << std::endl;
    }
}

void Server::ft_print_welcome_msg(int client_fd, Client* client) {
    //Client* client = this->connected_clients.at(client_fd);
    std::string extract_name_from_user;
    size_t pos = client->getFull().find(":");
    client->setPrinted(true);
    if (pos != std::string::npos) {
        extract_name_from_user = client->getFull().substr(pos + 1);
    }
    std::string resp = " Welcome to the Soviet Network!\n";
    this->serverReplyMessage(resp.c_str(), client_fd);
}

