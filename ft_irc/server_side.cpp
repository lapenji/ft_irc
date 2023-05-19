#include "server_side.hpp"

Server::Server(int port, const std::string& password): opt(1), port(port), password(password) {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    setSocket();
}

Server::~Server() {}

//PER GESTIRE QUIT?

// void    Server::ft_manage_quit(const std::string& tmp, int fd) {
//     (void)tmp;
//     delete this->connected_clients.at(fd);
//     this->connected_clients.erase(fd);
//     std::vector<pollfd>::iterator it = this->poll_vec.begin();
//     while (it->fd != fd) {
//         it ++;
//     }
//     close(it->fd);
//     this->poll_vec.erase(it);
// }

bool    Server::sendAll(const char *resp) {
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
}

void    Server::ft_manage_pass(const std::string& tmp, int client_fd, std::string& resp) {
   /*  if (this->connected_clients.at(client_fd)->getPass().size() > 0) {
        resp = "\nYou already inserted password, skip...\n";
        return ;
    } */
    std::string buffer;
    buffer = tmp.substr(0, tmp.length() -1);
    if (buffer.length() > 5) {
        if (buffer.substr(buffer.find(" ") + 1) == this->password) {
        this->connected_clients.at(client_fd)->setPassword(buffer.substr(buffer.find(" ") + 1));
        resp = "\nPassword accepted\r\n";
        return;
        }    
    }
    resp = "\nWrong password\r\n";
}

void    Server::ft_manage_nick(const std::string& tmp, int client_fd, std::string& resp) {
    if (this->connected_clients.at(client_fd)->getNick() == "") {
        std::string buffer;
        buffer = tmp.substr(0, tmp.length() -1);
        if (buffer.length() > 5) {
            this->connected_clients.at(client_fd)->setNickname(buffer.substr(buffer.find(" ") + 1));
            
            std::cout << "\nNick accepted" << std::endl;
            //resp = "\nNick accepted\r\n";
        }
        else {
            std::cout << "\nYou already inserted nick, skip..." << std::endl;
        }
        //resp = "\nYou already inserted nick, skip...\n";
    }
    else {
        std::cout << "\nnot enough parameters, kicked!" << std::endl;
        //resp = "not enough parameters, kicked!\r\n";
        resp = "";
    }
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
    /* if (this->connected_clients.at(client_fd)->getFull().size() > 0) {
        resp = "\nYou already inserted user prarams, skip...\n";
        return ;
    } */
    std::string buffer;
    buffer = tmp.substr(0, tmp.length() -1);
    std::vector<std::string> result;
    std::stringstream ss(buffer);
    std::string word;
    while (ss >> word) {
        result.push_back(word);
    }
    if (result.size() >= 5) {
        if (this->connected_clients.at(client_fd)->getFull() != "") {
            resp = "you already register\n";
            return;
        }
        ft_create_map_user(result, client_fd);
        resp = "welcome to SovietServer!\r\n";
        return;
    }
    resp = "not enough parameters\r\n";
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
        return -1;
    }
    else {
        //buffer[num_bytes] = '\0';   //// VERIFICARE, LO DICE CHATGPT
        std::string tmp = buffer;
        std::string resp;
        //resp = "Message received from server\n";
        ///parte di test
        std::vector<std::string> buffer_splitted = ft_splitBuffer(tmp);

        for (size_t i = 0; i < buffer_splitted.size(); ++i) {
            if (buffer_splitted[i].find("NICK") == 0) {
               ft_manage_nick(buffer_splitted[i], client_fd, resp);
            }
        }
        //buffer_splitted.push_back(tmp);

        /* std::istringstream iss(tmp); //FUNZIONA MA PROBABILMENTE NON E' LA SEPARAZIONE GIUSTA DA FARE
        std::string line;
        while (std::getline(iss, line)) {
            if (line.find("PASS") == 0) {
                this->connected_clients.at(client_fd)->setPassword(line.substr(5));
            } else if (line.find("NICK") == 0) {
                this->connected_clients.at(client_fd)->setNickname(line.substr(5));
            } else if (line.find("USER") == 0) {
                this->connected_clients.at(client_fd)->setFullName(line.substr(5));
            }
        } */
        /* std::cout << "NICK = " << this->connected_clients.at(client_fd)->getNick() << std::endl;
        std::cout << "USER = " << this->connected_clients.at(client_fd)->getFull() << std::endl;
        std::cout << "PASS = " << this->connected_clients.at(client_fd)->getPass() << std::endl; */
        std::string extract_name_from_user;
        size_t colonPosition = this->connected_clients.at(client_fd)->getFull().find(":");
    
        if (colonPosition != std::string::npos) {
            extract_name_from_user = this->connected_clients.at(client_fd)->getFull().substr(colonPosition + 1);
        }
        
        if (this->connected_clients.at(client_fd)->getNick().size() > 0) {
        resp = ":server 001 " + this->connected_clients.at(client_fd)->getNick() + " Welcome to the Soviet Network, " + this->connected_clients.at(client_fd)->getNick() + "!\r\n";
        resp += ":server 311 " + this->connected_clients.at(client_fd)->getNick() + " " + this->connected_clients.at(client_fd)->getNick() + " Soviet server * " + extract_name_from_user + "\r\n";
        resp += ":server 312 " + this->connected_clients.at(client_fd)->getNick() + " " + this->connected_clients.at(client_fd)->getNick() + " Soviet server :Server Description\r\n";
        resp += ":server 318 " + this->connected_clients.at(client_fd)->getNick() + " " + this->connected_clients.at(client_fd)->getNick() + " :End of WHOIS list\r\n";}

        //std::cout << resp << std::endl;
        //std::cout << tmp << std::endl;


        //////DA RIABILITARE ASSOLUTAMENTE A PROBLEMA RISOLTO

       /*  if ((tmp.substr(0, 4) == "PASS" || tmp.substr(0, 4) == "pass"))
            ft_manage_pass(buffer, client_fd, resp);
        if ((tmp.substr(0, 4) == "NICK" || tmp.substr(0, 4) == "nick"))
            ft_manage_nick(buffer, client_fd, resp);
        if (tmp.substr(0, 4) == "USER" || tmp.substr(0, 4) == "user")
            ft_manage_user(buffer, client_fd, resp);
        if (tmp.substr(0, 4) == "QUIT" || tmp.substr(0, 4) == "quit") {
            return -1;
        }*/

        ////DECOMMENTARE QUANDO IL CLIENT IMMETTE PASSWORD NICK E USER CORRETTI
        /* const char* response = "001 Welcome to Soviet Server!\n";
        int num_sent = send(client_fd, response, strlen(response), 0);
        if (num_sent == -1) {
            std::cerr << "->>\tError sending response to client!" << std::endl;
            return -1;
        } */
        
        //printMap(this->connected_clients);
        
        
        
        std::cout << "->>\tMessaggio del client " << client_fd << ": " << buffer  << std::flush;
        const char* response = resp.c_str();
        this->serverReplyMessage(response, client_fd);
        /* response = resp2.c_str();
        this->serverReplyMessage(response, client_fd);
        response = resp3.c_str();
        this->serverReplyMessage(response, client_fd);
        response = resp4.c_str();
        this->serverReplyMessage(response, client_fd); */
       /*  num_sent = send(client_fd, response, strlen(response), 0);
        if (num_sent == -1) {
            std::cerr << "->>\tError sending response to client!" << std::endl;
            return -1;
        } */
    }
    return 0;
}

void Server::serverReplyMessage(const char* response, int client_fd) {
    int num_sent = send(client_fd, response, strlen(response), 0);
        if (num_sent == -1) {
            std::cerr << "->>\tError sending response to client!" << std::endl;
            return ;
        }
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
                /* const char* response = "this is a private server that require a password.\nPlease type PASS <password>\n";
                this->serverReplyMessage(response, client_fd); */
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



