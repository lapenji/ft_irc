#include "Server.hpp"

Server::Server(int port, const std::string& password): opt(1), port(port), password(password) {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    setSocket();
}

Server::~Server() {
    std::cout << "distruttore in azione" << std::endl;
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

void Server::serverReplyMessage(const char* response, int client_fd) {
    if(send(client_fd, response, strlen(response), 0) == -1) {
        std::cerr << "->>\tError sending response to client!" << std::endl;
    }
}

void Server::ft_print_welcome_msg(int client_fd) {
    Client* conn_client = this->connected_clients.at(client_fd);
    std::string extract_name_from_user;
    size_t pos = conn_client->getFull().find(":");
    conn_client->setPrinted(true);
    if (pos != std::string::npos) {
        extract_name_from_user = conn_client->getFull().substr(pos + 1);
    }
    std::string resp = " Welcome to the Soviet Network!\n";
    this->serverReplyMessage(resp.c_str(), client_fd);
}

void Server::ft_delete_client(int client_fd) {
    std::map<int, Client *>::iterator map_it = this->connected_clients.begin();
    while (map_it != this->connected_clients.end()) {
        if (map_it->first == client_fd) {
            delete map_it->second;
            this->connected_clients.erase(map_it);
            break;
        }
        map_it++;
    }
    std::cout << "-->> QUITTATO IL CLIENT" << std::endl;
}

void    Server::ft_manage_topic(const std::string& tmp, int client_fd) {
    //Client* conn_client = this->connected_clients.at(client_fd);
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string msg = ft_joinStr(tmp_splitted, 2);
    this->channels.at(tmp_splitted[1])->changeTopic(msg, client_fd);
}

void    Server::ft_manage_kick(const std::string& tmp, int client_fd) {
    Client* conn_client = this->connected_clients.at(client_fd);
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (this->channels.find(tmp_splitted[1]) != this->channels.end()) {
        if (this->channels.at(tmp_splitted[1])->isUserAdmin(client_fd) == true) {

            std::string resp = ":" + conn_client->getNick() + "!" + conn_client->getUser() + " KICK " + tmp_splitted[1] + " " + tmp_splitted[2] + " :" + conn_client->getNick() + "\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
            std::map<int, Client *>::iterator it = this->connected_clients.begin();
            while (it != this->connected_clients.end()) {
                if (it->second->getNick() == tmp_splitted[2]) {
                    this->channels.at(tmp_splitted[1])->removeFromChan(it->second->getFd());
                }
                this->serverReplyMessage(resp.c_str(), it->second->getFd());
                it++;
            }
        }
        else {
            std::string resp = ":SovietServ 482 " + conn_client->getNick() + " " + tmp_splitted[1] + " :You must be a channel operator\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
    }
    else {
        std::string resp = ":SovietServ 401 " + conn_client->getNick() + " " + tmp_splitted[1] + " :No such channel!\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
    }
}

void    Server::ft_manage_invite(const std::string& tmp, int client_fd) {
    bool success = false;
    std::string resp;
    Client* conn_client = this->connected_clients.at(client_fd);
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string resp2 = ":" + conn_client->getNick() + "!" + conn_client->getUser() + " INVITE " + tmp_splitted[1] + " :" + tmp_splitted[2] + "\n";
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    if (this->channels.at(tmp_splitted[2])->checkIfAdmin(client_fd) == false) {
        resp = ":SovietServer 482 " + conn_client->getNick() + " " + tmp_splitted[2] + " :You must be admin to invite someone!\n";
    }
    else {
        while (it != this->connected_clients.end()) {
            std::cout << it->second->getNick();
            if (it->second->getNick() == tmp_splitted[1]) {
                this->serverReplyMessage(resp2.c_str(), it->first);
                success = true;
            }
            it++;
        }
        if (success == true) {
            resp = ":SovietServer 341 " + conn_client->getNick() + " " + tmp_splitted[1] + " " + tmp_splitted[2] + "\n";
        }
        else {
            resp = ":SovietServer 401 " + conn_client->getNick() + " " + tmp_splitted[1] + " :No such nick/channel\n";
        }
    }
    std::cout << "STO PER MANDARE RISP " << resp << std::endl;
    this->serverReplyMessage(resp.c_str(), client_fd);

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

std::string ft_joinStr(std::vector<std::string> result, int i) {
    std::string tmp = "";
    std::vector<std::string>::iterator it = result.begin() + i;
        while (it != result.end()) {
            //std::cout << "entro e stampo " << *it << std::endl;
            tmp += *it;
            if (it + 1 != result.end()) {
                tmp += " ";
            }
            it++;
        }
    return tmp;
}

void    Server::ft_manage_privmsg(const std::string& tmp, int client_fd) {
    std::cout << "ecco cosa mi arriva" << tmp << std::endl;
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string msg = ft_joinStr(tmp_splitted, 2);
    if (tmp_splitted[1][0] == '#') {
        if (this->channels.find(tmp_splitted[1]) != this->channels.end()) {
            std::map<int, Client*>::iterator it = this->channels.at(tmp_splitted[1])->clients.begin();
            this->channels.at(tmp_splitted[1])->printChanUsers();
            if (this->channels.at(tmp_splitted[1])->isUserInChan(client_fd) == true) {
                while (it != this->channels.at(tmp_splitted[1])->clients.end()) {
                    if (it->first != client_fd) {
                        std::string resp = ":" + this->connected_clients.at(client_fd)->getNick() + " PRIVMSG " + tmp_splitted[1] + " " + msg + "\n";
                        this->serverReplyMessage(resp.c_str(), it->first);
                    }
                    it++;
                }
            }
            else {
                std::string resp = ":SovietServ 404 " + this->connected_clients.at(client_fd)->getNick() + " " + tmp_splitted[1] + " :Cannot send to channel (no external messages)\n";
                this->serverReplyMessage(resp.c_str(), client_fd);
            }
        }
    }
    else {
        if (this->find_client(tmp_splitted[1]) == -1) {
            std::string resp = ":SovietServ 401 " + this->connected_clients.at(client_fd)->getNick() + " " + tmp_splitted[1] + " :No such nick/channel\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
        else {
            std::string resp = ":" + this->connected_clients.at(client_fd)->getNick() + "!" + this->connected_clients.at(client_fd)->getUser() + " PRIVMSG " + tmp_splitted[1] + " " + tmp_splitted[2] + "\n";
            this->serverReplyMessage(resp.c_str(), this->find_client(tmp_splitted[1]));
        }
    }

}


void    Server::ft_manage_part(const std::string& tmp, int client_fd) {
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    this->channels.at(tmp_splitted[1])->removeClient(this->connected_clients.at(client_fd));
}

void    Server::ft_manage_join(const std::string& tmp, int client_fd) {
    std::cout << "ENTRO IN MANAGE JOIN" << std::endl;
    Client* conn_client = this->connected_clients.at(client_fd);
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string nickOp = " :";
    if (this->channels.find(tmp_splitted[1]) == this->channels.end())  {
        this->channels.insert(std::make_pair(tmp_splitted[1], new Channel(conn_client, tmp_splitted[1])));
        this->channels.at(tmp_splitted[1])->addAdmin(conn_client);
        nickOp = " :@";
    }
    this->channels.at(tmp_splitted[1])->addClient(conn_client);

    std::string resp = ":" + this->connected_clients.at(client_fd)->getNick() + "!" + this->connected_clients.at(client_fd)->getUser() + " JOIN " + tmp_splitted[1] + "\n"
    + ":SovietServer 332 " + conn_client->getNick() + " " + tmp_splitted[1] +  " :" + this->channels.at(tmp_splitted[1])->getTopic() + "\n"
    + ":SovietServer 353 " + conn_client->getUser() + " = " + tmp_splitted[1] + " :" + this->channels.at(tmp_splitted[1])->getUsers() + "\n" //QUI CI VUOLE LA LISTA DI TUTTI GLI UTENTI
    + ":SovietServer 366 " + conn_client->getUser() + " " + tmp_splitted[1] + " :End of /NAMES list\n";
    std::cout << "Mando resp " << std::endl;
    this->serverReplyMessage(resp.c_str(), client_fd);

    printMap(this->channels);
}



void    Server::ft_manage_ping(const std::string& tmp, int client_fd) {
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (tmp_splitted.size() == 2) {
        this->serverReplyMessage(":SovietServer PONG SovietServer :SovietServer\r\n", client_fd);
    }
}

void    Server::ft_manage_mode(const std::string& tmp, int client_fd) {
    Client* conn_client = this->connected_clients.at(client_fd);
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (tmp_splitted.size() == 3 && tmp_splitted[1] == conn_client->getNick()) {
        if (tmp_splitted[2][0] == '+' && tmp_splitted[2][1] == 'i') {
            std::string resp = ":SovietServer 324 " + conn_client->getNick() + " +i\n";   /// NON SI SA SE FUNZIONA BOH
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
    }
}

bool    Server::ft_manage_pass(const std::string& tmp) {
    if (tmp.length() > 5) {
        if (tmp.substr(tmp.find(":") + 1) == this->password) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

void    Server::ft_create_map_user(std::vector<std::string> result, int client_fd) {
    Client* conn_client = this->connected_clients.at(client_fd);
    conn_client->setUserName(result[1]);
    conn_client->setHostname(result[2]);
    conn_client->setServerName(result[3]);
    if (result.size() > 5) {
        std::string tmp = ft_joinStr(result, 1);
        conn_client->setFullName(tmp);   
    }
    else
        conn_client->setFullName(result[4]);        ///////NON SI SA SE SERVE O NO, PER ME NO
}

void    Server::ft_manage_user(const std::string& tmp, int client_fd) {
    Client* conn_client = this->connected_clients.at(client_fd);
    if (conn_client->getFull() == "") {
        std::vector<std::string> result = ft_splitString(tmp);
        if (result.size() >= 5) {
            ft_create_map_user(result, client_fd);
        }
        //else {
        //    resp = "not enough parameters\r\n"; /////DA RIVEDERE
        //}
    }
    ft_print_welcome_msg(client_fd);
    //else {
    //    resp = "you already register\r\n";  /////DA RIVEDERE
    //}
}

void    Server::ft_manage_nick(const std::string& tmp, int client_fd) {
    Client* conn_client = this->connected_clients.at(client_fd);
    if (conn_client->getNick() != "") { /////// QUESTO IF E' UN TENTATIVO DI GESTIRE IL CAMBIO NICK, SEMBRA FUNZIONARE MA NON TROPPO, FORSE ALCUNI DI QUESTE REPLY NON SERVONO...
        conn_client->setNickname(tmp.substr(tmp.find(" ") + 1)); 
        // std::string resp = ":SovietServer 311 " + conn_client->getNick() + " " + conn_client->getNick() + " Soviet server * " + conn_client->getUser() + "\n"
        // + ":SovietServer 312 " + conn_client->getNick() + " " + conn_client->getNick() + " Soviet server :A very badass server...\n"
        // + ":SovietServer 318 " + conn_client->getNick() + " " + conn_client->getNick() + " :End of WHOIS list\r\n";
        // this->serverReplyMessage(resp.c_str(), client_fd);
    }
    //if (conn_client->getNick() == "") {   ////    TOLTO IF PERCHE' IL NICK SI PUÒ CAMBIARE SEMPRE
    else if (tmp.length() > 5) {
        conn_client->setNickname(tmp.substr(tmp.find(" ") + 1));
        std::cout << "\nNick: " << conn_client->getNick() << std::endl;    ////SOLO PER DEBUG
    }
        //else {
        //    std::cout << "\nnot enough parameters, kicked!" << std::endl;   ////SOLO PER DEBUG
        //}
    //}
    //else {
    //    std::cout << "\nYou already inserted nick, skip..." << std::endl;   ////SOLO PER DEBUG
    //    resp = "";  ////    questa messa solo per compilare, sarà da implementare
    //}
}

int Server::handle_client_request(int client_fd) {
   
    Client* conn_client = this->connected_clients.at(client_fd);
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
        std::cout << "\033[1;31m--->>> RICEVUTO QUESTO MESSAGGIO DAL CLIENT: " << client_fd << " <<<---\n\033[0m" << buffer << std::endl;
        // std::string tmps = buffer;
        // if (tmps.find("\n") != std::string::npos) {
        //     std::cout << "ABBIAMO R" << std::endl;
        // }
        std::string tmp = buffer;
        std::vector<std::string> buffer_splitted = ft_splitBuffer(tmp);
        if (conn_client->getCap() == false) {
            this->serverReplyMessage("IRC CAP REQ :none\n", client_fd);
            conn_client->setCap(true);
        }
        for (size_t i = 0; i < buffer_splitted.size(); ++i) {
            // if (buffer_splitted[i].find("PASS ") != std::string::npos) {
            //     std::cout << "entro!" << std::endl;
            // }
            if (buffer_splitted[i].find("NICK") == 0) {
               ft_manage_nick(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("USER") == 0 && buffer_splitted[i].find("USERHOST") != 0) {
               ft_manage_user(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("PASS") == 0) {
                if (ft_manage_pass(buffer_splitted[i]) == false) {
                    this->serverReplyMessage("Wrong password, disconnected!", client_fd);
                    return -1;
                }  
            }
            else if (buffer_splitted[i].find("MODE") == 0) {
               ft_manage_mode(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("PING") == 0) {
               ft_manage_ping(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("JOIN") == 0) {
                ft_manage_join(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("PART") == 0) {
                ft_manage_part(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("PRIVMSG") == 0) {
               ft_manage_privmsg(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("TOPIC") == 0) {
               ft_manage_topic(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("INVITE") == 0) {
               ft_manage_invite(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("KICK") == 0) {
               ft_manage_kick(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("QUIT") == 0) {
               return -1;
            }
        }
        if (conn_client->getNick().size() > 0 && conn_client->getFull().size() > 0 && conn_client->getPrinted() == false ) {
            ft_print_welcome_msg(client_fd);
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
                    if (this->connected_clients.find(it->fd) != this->connected_clients.end()) {
                        ft_delete_client(it->fd);        //////PROVA, DA TOGLIERE SE CRASHA
                        this->connected_clients.erase(it->fd);
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

int Server::find_client(const std::string& name) {
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    while (it != this->connected_clients.end()) {
            if (it->second->getNick() == name) {
                return it->first;
            }
            it++;
    }
    return -1;
}

