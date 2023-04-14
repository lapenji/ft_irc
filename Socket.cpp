#include "Socket.hpp"

Socket::Socket() {
    //inizializzo array clientsocks
    int ret;
    opt = 1;
    masterSock = socket(AF_INET, SOCK_STREAM, 0);
    if (masterSock == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    ret = setsockopt(masterSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    if (ret < 0) {
        perror("setsoskopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
    ret = bind(masterSock, (struct sockaddr *)&address, sizeof(address));
    if (ret < 0) {
        perror("bind fail");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening on port " << PORT << std::endl;
    ret = listen(masterSock, 15);
    if (ret < 0) {
        perror("bind fail");
        exit(EXIT_FAILURE);
    }
    addrLen = sizeof(address);
    std::cout << "socket ready and waiting for connections" << std::endl;
}

int Socket::getMasterSock() {
    return masterSock;
}

struct sockaddr_in  Socket::getAddress() {
    return address;
}
