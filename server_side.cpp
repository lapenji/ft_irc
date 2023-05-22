#include "server_side.hpp"

Server::Server(int port, const std::string& password): opt(1), port(port), password(password), printed(false) {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    setSocket();
}

Server::~Server() {}


void Server::ft_print_welcome_msg(const std::string& extract_name_from_user, int client_fd) {
    std::string resp;
    resp = ":server 001 " + this->connected_clients.at(client_fd)->getNick() + " Welcome to the Soviet Network, " + this->connected_clients.at(client_fd)->getNick() + "!\r\n";
    resp += ":server 311 " + this->connected_clients.at(client_fd)->getNick() + " " + this->connected_clients.at(client_fd)->getNick() + " Soviet server * " + extract_name_from_user + "\r\n";
    resp += ":server 312 " + this->connected_clients.at(client_fd)->getNick() + " " + this->connected_clients.at(client_fd)->getNick() + " Soviet server :Server Description\r\n";
    resp += ":server 318 " + this->connected_clients.at(client_fd)->getNick() + " " + this->connected_clients.at(client_fd)->getNick() + " :End of WHOIS list\r\n";
    this->serverReplyMessage(resp.c_str(), client_fd);
}

void Server::serverReplyMessage(const char* response, int client_fd) {
    if(send(client_fd, response, strlen(response), 0) == -1) {
        std::cerr << "->>\tError sending response to client!" << std::endl;
    }
}

//PER GESTIRE QUIT?

void Server::ft_manage_quit() {
    std::map<int, Client *>::iterator map_it = this->connected_clients.begin();
    while (map_it != this->connected_clients.end()) {
        delete map_it->second;
        map_it++;
    }

    std::vector<pollfd>::iterator it = this->poll_vec.begin();
    it++;
    while (it != this->poll_vec.end()) {
        close(it->fd);
        this->poll_vec.erase(it);
    }
    std::cout << "-->> QUITTATO TUTTO" << std::endl;
}

/* bool    Server::sendAll(const char *resp) {      /////INUTILIZZATA PER ORA
    std::map<int, Client*>::iterator    it = this->connected_clients.begin();
    while (it != this->connected_clients.end())
    {
        int num_sent = send(it->first, resp, strlen(resp), 0);
        if (num_sent == -1) {
            std::cerr << "->>\tError sending response to client!" << std::endl;
            return false;
        }
        it++;
    }
    return true;
} */

void    Server::ft_manage_ping(const std::string& tmp, int client_fd) {
    std::string resp;
    std::vector<std::string> tmp_splitted = ft_splitBuffer(tmp);
    if (tmp_splitted.size() == 2) {
        resp = ":server PONG " + tmp_splitted[1];   /// QUI NON VA MESSO \R\N PERCHE' E' DENTRO LA STRINGA, CREDO
        this->serverReplyMessage(resp.c_str(), client_fd);
    }
}

void    Server::ft_manage_mode(const std::string& tmp, int client_fd) {
    std::string resp;
    std::vector<std::string> tmp_splitted = ft_splitBuffer(tmp);
    if (tmp_splitted.size() == 3 && tmp_splitted[1] == this->connected_clients.at(client_fd)->getUser()) {
        if (tmp_splitted[2][0] == '+' && tmp_splitted[2][1] == 'i') {
            resp = ":server 324 " + this->connected_clients.at(client_fd)->getUser() + " +i\r\n";   /// NON SI SA SE FUNZIONA BOH
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
    }
}

void    Server::ft_manage_pass(const std::string& tmp, int client_fd, std::string& resp) {
   if (this->connected_clients.at(client_fd)->getPass() == "") {
        std::string buffer;
        buffer = tmp.substr(0, tmp.length());
        if (buffer.length() > 5) {
            if (buffer.substr(buffer.find(" ") + 1) == this->password) {
            this->connected_clients.at(client_fd)->setPassword(buffer.substr(buffer.find(" ") + 1));
            std::cout << "pass ok" << std::endl;        ////SOLO PER DEBUG
            //resp = "\nPassword accepted\r\n";
            return;
            }    
        }
        resp = "\nWrong password\r\n";
    }
    else {
        std::cout << "\nYou already inserted password, skip..." << std::endl;
    }
}

void    Server::ft_manage_nick(const std::string& tmp, int client_fd, std::string& resp) {
    if (this->connected_clients.at(client_fd)->getNick() == "") {
        std::string buffer;
        buffer = tmp.substr(0, tmp.length()); //tolto -1 alla fine, se non va rimettere
        if (buffer.length() > 5) {
            this->connected_clients.at(client_fd)->setNickname(buffer.substr(buffer.find(" ") + 1));

            std::cout << "\nNick accepted" << std::endl;    ////SOLO PER DEBUG
            return;
        }
        else {
            std::cout << "\nnot enough parameters, kicked!" << std::endl;   ////SOLO PER DEBUG
        }
    }
    else {
        std::cout << "\nYou already inserted nick, skip..." << std::endl;   ////SOLO PER DEBUG
        resp = "";
    }
}

void    Server::ft_create_map_user(std::vector<std::string> result, int client_fd) {
    this->connected_clients.at(client_fd)->setUserName(result[1]);
    this->connected_clients.at(client_fd)->setHostname(result[2]);
    this->connected_clients.at(client_fd)->setServerName(result[3]);
    if (result.size() > 5) {
        std::string tmp = "";
        std::vector<std::string>::iterator it = result.begin() + 1; // era + 4 ma ho messo + 1 cosi' abbiamo tutto il campo USER mandato dal client
        while (it != result.end()) {
            //std::cout << "entro e stampo " << *it << std::endl;
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
    if (this->connected_clients.at(client_fd)->getFull() == "") {
        //resp = "\nYou already inserted user prarams, skip...\n";
        std::string buffer;
        buffer = tmp.substr(0, tmp.length());
        std::vector<std::string> result = ft_splitString(buffer);
        
       /*  std::vector<std::string> result;
        std::stringstream ss(buffer);
        std::string word;
        while (ss >> word) {
            result.push_back(word);
        } */
        if (result.size() >= 5) {
            ft_create_map_user(result, client_fd);
            resp = "welcome to SovietServer!\r\n";
            return;
        }
        else {
            resp = "not enough parameters\r\n";
        }
    }
    else {
        resp = "you already register\r\n";
    }
    
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
    address.sin_port = htons(this->port);

    if(bind(socket_fd, (sockaddr*) &address, sizeof(address)) < 0) {
        std::cerr << "Binding error, exit..." << std::endl;
        exit(-1);
    }

    if(listen(socket_fd, 10) < 0) {
        std::cerr << "Socket listening error, exit..." << std::endl;
        exit(-1);
    }

    std::cout << "->>\tListening on port " << this->port << std:: endl;
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
        std::cout << "->>\tConnessione chiusa " << this->port << std:: endl;
        return -1;
    }
    else {
        //buffer[num_bytes] = '\0';   //// VERIFICARE, LO DICE CHATGPT
        std::string tmp = buffer;
        std::string resp;
        std::vector<std::string> buffer_splitted = ft_splitBuffer(tmp);

        for (size_t i = 0; i < buffer_splitted.size(); ++i) {
            if (buffer_splitted[i].find("NICK") == 0) {
               ft_manage_nick(buffer_splitted[i], client_fd, resp);
            }
            else if (buffer_splitted[i].find("USER") == 0) {
               ft_manage_user(buffer_splitted[i], client_fd, resp);
            }
            else if (buffer_splitted[i].find("PASS") == 0) {
               ft_manage_pass(buffer_splitted[i], client_fd, resp);
            }
            else if (buffer_splitted[i].find("MODE") == 0) {
               ft_manage_mode(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("PING") == 0) {
               ft_manage_ping(buffer_splitted[i], client_fd);
            }
            /* else if (buffer_splitted[i].find("QUIT") == 0) {
               ft_manage_quit();
            } */
        }
        /* std::cout << "NICK = " << this->connected_clients.at(client_fd)->getNick() << std::endl;
        std::cout << "USER = " << this->connected_clients.at(client_fd)->getFull() << std::endl;
        std::cout << "PASS = " << this->connected_clients.at(client_fd)->getPass() << std::endl; */
        
        
        std::string extract_name_from_user;
        size_t colonPosition = this->connected_clients.at(client_fd)->getFull().find(":");
    
        if (colonPosition != std::string::npos) {
            extract_name_from_user = this->connected_clients.at(client_fd)->getFull().substr(colonPosition + 1);
        }
        
        
        //std::cout << resp << std::endl;
        std::cout << tmp << std::endl;
        
        //printMap(this->connected_clients);
        
        std::cout << "->>\tMessaggio del client " << client_fd << ": \n" << buffer << std::endl;
        //const char* response = resp.c_str();
        if (this->connected_clients.at(client_fd)->getNick().size() > 0 && this->printed == false ) {
            this->printed = true;
            ft_print_welcome_msg(extract_name_from_user, client_fd);
        }
       /*  else
            this->serverReplyMessage(response, client_fd); */
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
                std::cout << "->>\tsocket:" << new_pollfd.fd << std::endl;
                //printMap(connected_clients);
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



