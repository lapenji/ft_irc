#include "server_side.hpp"

Server::Server(const std::string& password): opt(1), password(password) {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    setSocket();
}

Server::~Server() {}

void Server::setSocket() {
    if (this->socket_fd < 0) {
        std::cerr << "Error creating socket, exit...";
        exit(-1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Error in socket options, exit..." << std::endl;
        exit(-1);
    }

    struct sockaddr_in address;
    bzero((char *) &address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8000);

    if(bind(socket_fd, (sockaddr*) &address, sizeof(address)) < 0) {
        std::cerr << "Binding error, exit..." << std::endl;
        exit(-1);
    }

    if(listen(socket_fd, 10) < 0) {
        std::cerr << "Socket listening error, exit..." << std::endl;
        exit(-1);
    }

    std::cout << "->>\tListening on port 8000..." << std:: endl;
}

void Server::startServer() {
    pollfd new_pollfd = {socket_fd, POLLIN, 0};
    this->poll_vec.push_back(new_pollfd);
    while(true) {
        if (poll(this->poll_vec.data(), this->poll_vec.size(), -1) == -1) {
            std::cerr << "Poll error, exit..." << std::endl;
            exit (-1);
        }
        std::vector<pollfd>::iterator it = this->poll_vec.begin();
        while (it != this->poll_vec.end()) {
            if (it->revents & POLLIN) {
                if (it->fd == this->socket_fd) {
                    sockaddr_in client_address = {};
                    socklen_t client_address_len = sizeof(client_address);
                    int client_fd = accept(this->socket_fd, (sockaddr*) &client_address, &client_address_len);
                    if (client_fd < 0) {
                        std::cerr << "->>\tNew connection refused" << std::endl;
                    }
                    else {
                        pollfd new_pollfd = {socket_fd, POLLIN, 0};
                        this->poll_vec.push_back(new_pollfd);
                    }
                }
            }
        }
    }
}