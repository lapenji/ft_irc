#include "Socket.hpp"
#include <map>
#include <algorithm>

class Server {
    private:

    int                 newSock;
    int                 activity;
    int                 valread;
    int                 sd;
    int                 maxSd;
    char                buffer[1025];
    int                 clientSocket[MAX_CLIENTS];
    int                 masterSock;
    struct sockaddr_in  address;
    fd_set              readfds;
    int                 addrlen;
    std::map<std::string, std::string>  users;
    Server();
    public:
    Server(int masterSock, struct sockaddr_in  address);
    ~Server();
    void    runServer();

};