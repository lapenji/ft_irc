#include "Client.hpp"
#include <map>

class Channel {
private:
    std::string             password;
    std::map<int, Client *> admins;
    std::string             name;

public:
    std::map<int, Client *> clients;
    Channel(Client * client, std::string name);
    ~Channel();
    std::string ft_353_366(Client* client, const char* str);
    void    addClient(Client* client);
    void    printChanUsers();
    std::string getUsers();
    void    removeClient(Client* client);
};
