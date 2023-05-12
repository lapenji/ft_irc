#include "server_side.hpp"

Server::Server(const std::string& port, const std::string& password): opt(1), port(port), password(password) {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    setSocket();
}

Server::~Server() {}

void    printMap(std::map<int, Client*> myMap) { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "STAMPO IL MAP" << std::endl;
    for(std::map<int, Client*>::const_iterator it = myMap.begin();
    it != myMap.end(); ++it)
    {
        std::cout << it->first << ": " << it->second->getNick() << " - " << it->second->getFull() << std::endl;
    }
}



void    Server::ft_manage_nick(const std::string& tmp, int client_fd, std::string& resp) {
    std::string buffer;
    buffer = tmp.substr(0, tmp.length() -1);
    if (buffer.length() > 5) {
        this->connected_clients.at(client_fd)->setNickname(buffer.substr(buffer.find(" ") + 1));
        resp = "welcome to the server\n";
        return;
    }
    resp = "not enough parameters, kicked!\n";
}

void    Server::ft_create_map_user(std::vector<std::string> result, int client_fd) {
    this->connected_clients.at(client_fd)->setUserName(result[1]);
    this->connected_clients.at(client_fd)->setHostname(result[2]);
    this->connected_clients.at(client_fd)->setServerName(result[3]);
    if (result.size() > 5) {
        std::string tmp = "";
        std::vector<std::string>::iterator it = result.begin() + 4;
        while (it != result.end()) {
            std::cout << "entro e stampo " << *it << std::endl;
            tmp += *it;
            if (it + 1 != result.end()) {
                tmp += " ";
            }
            it++;
        }
    this->connected_clients.at(client_fd)->setFullName(tmp);   
    }
    else
        this->connected_clients.at(client_fd)->setFullName(result[4]);
}

void    Server::ft_manage_user(const std::string& tmp, int client_fd, std::string& resp) {
    std::string buffer;
    buffer = tmp.substr(0, tmp.length() -1);
    std::vector<std::string> result;
    std::stringstream ss(buffer);
    std::string word;
    while (ss >> word) {
        result.push_back(word);
    }
    if (result.size() >= 5) {
        ft_create_map_user(result, client_fd);
        resp = "welcome to server!\n";
        return;
    }
    
    resp = "not enough parameters\n";
}

void Server::setSocket() {
    if (this->socket_fd < 0) {
        std::cerr << "Error creating socket, exit...";
        exit(-1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Error in socket options, exit..." << std::endl;
        exit(-1);
    }

    sockaddr_in address = {};
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

int Server::handle_client_request(int client_fd) {
    char buffer[1024];
    bzero(buffer, 1024);
    int num_bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (num_bytes == -1) {
        std::cerr << "->>\tError handling client!" << std::endl;
        return -1;
    }
    else if (num_bytes == 0) {
        // Il client ha chiuso la connessione
        return -1;
    }
    else {
        std::string tmp = buffer;
        std::string resp;
        resp = "Message received from server\n";
        std::cout << tmp << std::endl;
        if ((tmp.substr(0, 4) == "NICK" || tmp.substr(0, 4) == "nick"))
            ft_manage_nick(buffer, client_fd, resp);
        if (tmp.substr(0, 4) == "USER" || tmp.substr(0, 4) == "user")
            ft_manage_user(buffer, client_fd, resp);
        
        printMap(this->connected_clients);
        std::cout << "->>\tMessaggio del client " << client_fd << ": " << buffer << std::flush;
        const char* response = resp.c_str();
        int num_sent = send(client_fd, response, strlen(response), 0);
        if (num_sent == -1) {
            std::cerr << "->>\tError sending response to client!" << std::endl;
            return -1;
        }
    }

    return 0;
}

void Server::startServer() {

    sockaddr_in s_address = {};

    pollfd new_pollfd = {socket_fd, POLLIN, 0};
    this->poll_vec.push_back(new_pollfd);
    while(true) {
        if (poll(this->poll_vec.data(), this->poll_vec.size(), -1) == -1) {
            std::cerr << "Poll error, exit..." << std::endl;
            exit (-1);
        }
        if (this->poll_vec[0].revents & POLLIN) {
            sockaddr_in client_address = {};
            socklen_t client_address_len = sizeof(client_address);
            int client_fd = accept(this->socket_fd, (sockaddr*) &client_address, &client_address_len);
            if (client_fd < 0) {
                std::cerr << "->>\tNew connection refused" << std::endl;
            }
            else {
                std::cout << "->>\tNew connection accepted" << std::endl;
                pollfd new_pollfd = {client_fd, POLLIN, 0};
                this->poll_vec.push_back(new_pollfd);
                char hostname[NI_MAXHOST];
                getnameinfo((struct sockaddr *) &s_address, sizeof(s_address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
                Client* tmp = new Client(new_pollfd.fd, hostname, ntohs(s_address.sin_port));
                this->connected_clients.insert(std::make_pair(client_fd, tmp));
                std::cout << "->>\t valore:" << new_pollfd.fd << std::endl;
                printMap(connected_clients);
            }
        }
        std::vector<pollfd>::iterator it = this->poll_vec.begin();
        it++;
        while (it != this->poll_vec.end()) {
            if (it->revents & POLLIN) {
                // Un client ha inviato dei dati
                if (handle_client_request(it->fd) == -1) {
                    // Il client si è disconnesso, rimuovi il socket dalla lista
                    if (connected_clients.find(it->fd) != connected_clients.end()) {
                        connected_clients.erase(it->fd);
                    }
                    std::cout << it->fd << " ->>\tDisconnected" << std::endl;
                    close(it->fd);
                    this->poll_vec.erase(it);
                    it--;
                }
            }
            it++;
        }
    }

}



