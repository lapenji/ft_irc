#include "Client.hpp"
#include <map>

class Channel {
private:
    std::map<int, Client *> users;
    std::string             password;
    std::map<int, Client *> admins;
};