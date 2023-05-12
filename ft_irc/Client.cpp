#include "Client.hpp"

Client::Client(int fd, const std::string& hostname, int port) : fd(fd), hostname(hostname), port(port) {}

void                Client::setNickname(const std::string& nick) {
    this->nickname = nick;
}

void                Client::setFullName(const std::string& full) {
    this->fullName = full;
}

const std::string&  Client::getNick() {
    return this->nickname;
}

const std::string&  Client::getFull() {
    return this->fullName;
}

void    Client::setUserName(const std::string& user) {
    this->username = user;
}

void    Client::setHostname(const std::string& host) {
    this->hostname = host;
}

void    Client::setServerName(const std::string& servername) {
    this->serverName = servername;
}
