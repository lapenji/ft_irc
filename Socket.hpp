#ifndef SOCKET_HPP
# define SOCKET_HPP

# define MAX_CLIENTS 30
# define PORT 8080
# include <iostream>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/time.h>
# include <stdio.h> //PERROR

class Socket {
    private :
    int                 opt;
    int                 masterSock;
    int                 addrLen;
    struct sockaddr_in  address;
    
    public :
    Socket();
    ~Socket(){};
    int                 getMasterSock();
    struct sockaddr_in  getAddress();

};

#endif