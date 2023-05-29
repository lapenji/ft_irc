#include "Client.hpp"
#include <map>

class Channel {
private:
    std::string             password;
    std::map<int, Client *> admins;

public:
    std::map<int, Client *> clients;
    Channel(Client * client);
    ~Channel();
    void    addClient(Client* client);
    void    printChanUsers();
};
