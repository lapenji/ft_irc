#include "Client.hpp"

Client::Client(int fd, int port) : fd(fd), password(""), port(port), printed(false), cap(false), aut(false), nickOk (false) {

}

Client::~Client() {}

void    Client::setPassword(const std::string& password) {
    this->password = password;
}

void    Client::setNickname(const std::string& nick) {
    this->nickname = nick;
}

void    Client::setFullName(const std::string& full) {
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

bool    Client::getCap() {
    return this->cap;
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

void    Client::setCap(bool cap) {
    this->cap = cap;
}

int     Client::getFd() {
    return this->fd;
}

bool    Client::getAut() {
    return this->aut;
}

void    Client::setAut(bool aut) {
    this->aut = aut;
}

void    Client::setIp(char ipAddress[INET_ADDRSTRLEN]) {
    this->ip = ipAddress;
}
const std::string& Client::getIp() {
    return this->ip;
}