#include "Client.hpp"

Client::Client(int fd, const std::string& hostname, int port) : fd(fd), password(""), hostname(hostname), port(port), printed(false) {}

Client::~Client() {}

void    Client::setPassword(const std::string& password) {
    this->password = password;
}

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

const std::string&  Client::getUser() {
    return this->username;
}

const std::string&  Client::getPass() {
    return this->password;
}

bool    Client::getPrinted() {
    return this->printed;
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

void    Client::setPrinted(bool printed) {
    this->printed = printed;
}