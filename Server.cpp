#include "Server.hpp"

Server::Server(int port, const std::string& password): opt(1), port(port), password(password) {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    setSocket();
}

Server::~Server() {
    std::cout << "distruttore in azione" << std::endl;
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    while (it != this->connected_clients.end()) {
        delete it->second;
        close (it->first);
        it++;
    }
    std::map<std::string, Channel *>::iterator ita = this->channels.begin();
    while (ita != this->channels.end()) {
        delete ita->second;
        ita++;
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
    std::cout << "\033[1;33m\n->>\tSovietServer listening on port " << this->port << "\033[0m\n" << std:: endl;
}

void    Server::ft_manage_topic(const std::string& tmp, int client_fd) {
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string msg = ft_joinStr(tmp_splitted, 2);
    Channel* chan = this->channels.at(tmp_splitted[1]);
    chan->changeTopic(msg, client_fd);
}

void    Server::ft_manage_kick(const std::string& tmp, int client_fd, const std::string& nick, const std::string& user) { 
    //std::string nick = this->connected_clients.at(client_fd)->getNick();
    //std::string user = this->connected_clients.at(client_fd)->getUser();
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (this->channels.find(tmp_splitted[1]) != this->channels.end()) {
        if (this->channels.at(tmp_splitted[1])->isUserAdmin(client_fd) == true) {
            std::string reason = "";
            reason += ft_joinStr(tmp_splitted, 3);
            std::string resp = ":" + nick + "!" + user + " KICK " + tmp_splitted[1]
                + " " + tmp_splitted[2] + " " + reason + "\n";
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
            std::string resp = ":SovietServer 482 " + nick + " " + tmp_splitted[1] + " :You must be a channel operator\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
    }
    else {
        std::string resp = ":SovietServer 401 " + nick + " " + tmp_splitted[1] + " :No such channel!\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
    }
}

void    Server::ft_manage_invite(const std::string& tmp, int client_fd, const std::string& nick, const std::string& user) {
    bool success = false;
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string resp;
    //std::string nick = this->connected_clients.at(client_fd)->getNick();
    //std::string user = this->connected_clients.at(client_fd)->getUser();
    std::string resp2 = ":" + nick + "!" + user + " INVITE " + tmp_splitted[1] + " :" + tmp_splitted[2] + "\n";
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    if (this->channels.at(tmp_splitted[2])->checkIfAdmin(client_fd) == false) {
        resp = ":SovietServer 482 " + nick + " " + tmp_splitted[2] + " :You must be admin to invite someone!\n";
    }
    else {
        this->channels.at(tmp_splitted[2])->addToInvited(tmp_splitted[1]);
        while (it != this->connected_clients.end()) {
            std::cout << it->second->getNick();
            if (it->second->getNick() == tmp_splitted[1]) {
                this->serverReplyMessage(resp2.c_str(), it->first);
                success = true;
            }
            it++;
        }
        if (success == true) {
            resp = ":SovietServer 341 " + nick + " " + tmp_splitted[1] + " " + tmp_splitted[2] + "\n";
        }
        else {
            resp = ":SovietServer 401 " + nick + " " + tmp_splitted[1] + " :No such nick/channel\n";
        }
    }
    std::cout << "STO PER MANDARE RISP " << resp << std::endl;
    this->serverReplyMessage(resp.c_str(), client_fd);

}

void    Server::ft_manage_privmsg(const std::string& tmp, int client_fd, const std::string& nick) {
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    Channel* chan = this->channels.at(tmp_splitted[1]);
    //std::string nick = this->connected_clients.at(client_fd)->getNick();
    std::string msg = ft_joinStr(tmp_splitted, 2);
    if (tmp_splitted[1][0] == '#') {
        if (this->channels.find(tmp_splitted[1]) != this->channels.end()) {
            std::map<int, Client*>::iterator it = chan->clients.begin();
            chan->printChanUsers();
            if (chan->isUserInChan(client_fd) == true) {
                while (it != chan->clients.end()) {
                    if (it->first != client_fd) {
                        std::string resp = ":" + nick + " PRIVMSG " + tmp_splitted[1] + " " + msg + "\n";
                        this->serverReplyMessage(resp.c_str(), it->first);
                    }
                    it++;
                }
            }
            else {
                std::string resp = ":SovietServ 404 " + nick + " " + tmp_splitted[1] + " :Cannot send to channel (no external messages)\n";
                this->serverReplyMessage(resp.c_str(), client_fd);
            }
        }
    }
    else {
        if (this->find_client(tmp_splitted[1]) == -1) {
            std::string resp = ":SovietServ 401 " + nick + " " + tmp_splitted[1] + " :No such nick/channel\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
        else {
            std::string resp = ":" + nick + "!" + this->connected_clients.at(client_fd)->getUser() + " PRIVMSG " + tmp_splitted[1] + " " + tmp_splitted[2] + "\n";
            this->serverReplyMessage(resp.c_str(), this->find_client(tmp_splitted[1]));
        }
    }

}

void    Server::ft_manage_part(const std::string& tmp/* , int client_fd */, Client* client) {
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    Channel* chan = this->channels.at(tmp_splitted[1]);
    //Client* client = this->connected_clients.at(client_fd);
    if (tmp_splitted.size() > 2) {
        chan->removeClient(client, ft_joinStr(tmp_splitted, 2));
    }
    else {
        chan->removeClient(client, "");
    }
    if (chan->isEmpty() == true) {
        this->channels.erase(tmp_splitted[1]);
    }
}

void    Server::ft_manage_join(const std::string& tmp, int client_fd, Client* client, const std::string& nick, const std::string& user) {

    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (this->channels.find(tmp_splitted[1]) == this->channels.end())  {
        this->channels.insert(std::make_pair(tmp_splitted[1], new Channel(client, tmp_splitted[1])));
        Channel* chan = this->channels.at(tmp_splitted[1]);
        chan->addAdmin(client);
    }
    Channel* chan = this->channels.at(tmp_splitted[1]);
    if (chan->getInviteOnly() == true && chan->isInvited(nick) == false) {
        std::string resp = ":SovietServ 473 " + nick + " " + tmp_splitted[1] + " :Cannot join channel (Invite only)\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (chan->getUserNrLimited() == true && (int)chan->clients.size() >= chan->getMaxUsers()) {
        std::string resp = ":SovietServ 471 " + nick + " " + tmp_splitted[1] + " :Cannot join channel (Channel is full)\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (chan->getNeedPassword() == true) {
        if (tmp_splitted.size() < 3 || tmp_splitted[2] != chan->getPassword()) {
            std::string resp = ":SovietServ 475 " + nick + " " + tmp_splitted[1] + " :Cannot join channel (Incorrect channel key)\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
            return;
        }
    }
    chan->addClient(client);
    if (chan->isInvited(nick) == true) {
        chan->removeToInvited(nick);
    }
    std::cout << "\nIL TOPIC ADESSO " << chan->getTopic() << std::endl;
    std::string resp = ":" + nick + "!" + user
        + " JOIN " + tmp_splitted[1] + "\n"
        + ":SovietServer 332 " + nick + " " + tmp_splitted[1] +  " " + chan->getTopic() + "\n"
        + ":SovietServer 353 " + user + " = " + tmp_splitted[1] + " :" + chan->getUsers() + "\n"
        + ":SovietServer 366 " + user + " " + tmp_splitted[1] + " :End of /NAMES list\n";
    this->serverReplyMessage(resp.c_str(), client_fd);

    printMap(this->channels);
}

void    Server::ft_manage_ping(const std::string& tmp, int client_fd) {
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (tmp_splitted.size() == 2) {
        this->serverReplyMessage(":SovietServer PONG SovietServer :SovietServer\r\n", client_fd);
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

bool    Server::ft_manage_user(const std::string& tmp, int client_fd, Client* client) {
    //Client* client = this->connected_clients.at(client_fd);
    std::vector<std::string> result = ft_splitString(tmp);
    if (client->getFull() != "") {
        std::string resp = ":SovietServer 462 " + client->getNick() + " :You may not reregister\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        return true;
    }
    if (client->getAut() == true) {
        if (result.size() >= 5) {
        ft_create_map_user(result, client_fd);
    }
        ft_print_welcome_msg(client_fd, client);
        return true;
    }
    else {
        std::cout << "ENTRO" << std::endl;
        std::string resp = ":SovietServ 464 " + client->getNick() + " :Password incorrect or missing!\n";
        std::cout << "invio resp " << resp << std::endl;
        this->serverReplyMessage(resp.c_str(), client_fd);
        return false;
    }
}

void    Server::ft_manage_nick(const std::string& tmp, int client_fd, Client* client) {
    if (isNickInUse(tmp.substr(tmp.find(" ") + 1)) == true) {
        std::string resp = ":SovietServer 433 " + tmp.substr(tmp.find(" ") + 1) + " :Nickname already in use\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (client->getNick() != "") {
        std::string oldNick = client->getNick();
        client->setNickname(tmp.substr(tmp.find(" ") + 1)); 
        std::map<std::string, Channel *>::iterator it = this->channels.begin();
        std::string resp = ":" + oldNick + "!" + client->getUser() + " NICK " + client->getNick() + "\n";
        while (it != this->channels.end()) {
            if (it->second->isUserInChan(client_fd) == true) {
                it->second->sendToAllusersExcept(resp, client_fd);
            }
            it++;
        }
        this->serverReplyMessage(resp.c_str(), client_fd);
    }
    else if (tmp.length() > 5) {
        client->setNickname(tmp.substr(tmp.find(" ") + 1));
    }
}

int Server::handle_client_request(int client_fd) {
   
    Client* client = this->connected_clients.at(client_fd);
    std::string nick = this->connected_clients.at(client_fd)->getNick();
    std::string user = this->connected_clients.at(client_fd)->getUser();
    char buffer[1024];
    bzero(buffer, 1024);
    int num_bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (num_bytes == -1) {
        std::cerr << "->>\tError handling client!" << std::endl;
        return -1;
    }
    else if (num_bytes == 0) {
        std::cout << "->>\tConnessione chiusa sulla porta " << this->port << std:: endl;
        return -1;
    }
    else {
        std::cout << "\033[1;31m\n--->>> RICEVUTO QUESTO MESSAGGIO DAL CLIENT: " << client_fd << " <<<---\n\n\033[0m" << buffer << std::endl;
        std::string tmp = buffer;
        std::vector<std::string> buffer_splitted = ft_splitBuffer(tmp);
        if (client->getCap() == false) {
            this->serverReplyMessage("IRC CAP REQ :none\n", client_fd);
            client->setCap(true);
        }
        for (size_t i = 0; i < buffer_splitted.size(); ++i) {
            if (buffer_splitted[i].find("NICK") == 0) {
               ft_manage_nick(buffer_splitted[i], client_fd, client);
            }
            else if (buffer_splitted[i].find("USER") == 0 && buffer_splitted[i].find("USERHOST") != 0) {
               if (ft_manage_user(buffer_splitted[i], client_fd, client) == false)
                return -1;
            }
            else if (buffer_splitted[i].find("PASS") == 0) {
                if (ft_manage_pass(buffer_splitted[i]) == false) {
                    std::string resp;
                    client->setAut(false);
                }
                else {
                    client->setAut(true);
                }
            }
            else if (buffer_splitted[i].find("MODE") == 0) {
               ft_manage_mode(buffer_splitted[i], client_fd, nick, user);
            }
            else if (buffer_splitted[i].find("PING") == 0) {
                ft_manage_ping(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("JOIN") == 0) {
                ft_manage_join(buffer_splitted[i], client_fd, client, nick, user);
            }
            else if (buffer_splitted[i].find("PART") == 0) {
                ft_manage_part(buffer_splitted[i], client);
            }
            else if (buffer_splitted[i].find("PRIVMSG") == 0) {
                ft_manage_privmsg(buffer_splitted[i], client_fd, nick);
            }
            else if (buffer_splitted[i].find("TOPIC") == 0) {
                ft_manage_topic(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("INVITE") == 0) {
                ft_manage_invite(buffer_splitted[i], client_fd, nick, user);
            }
            else if (buffer_splitted[i].find("KICK") == 0) {
                ft_manage_kick(buffer_splitted[i], client_fd, nick, user);
            }
            else if (buffer_splitted[i].find("QUIT") == 0) {
                return -1;
            }
        }
        if (client->getNick().size() > 0 && client->getFull().size() > 0 && client->getPrinted() == false ) {
            ft_print_welcome_msg(client_fd, client);
        }
    }
    return 0;
}

void Server::startServer() {

    sockaddr_in s_address = {};
    pollfd new_pollfd = {socket_fd, POLLIN, 0};
    this->poll_vec.push_back(new_pollfd);
    while(true) {
        signal(SIGINT, ft_signal_ctrl_c);
        if (poll(this->poll_vec.data(), this->poll_vec.size(), -1) == -1) {
            std::cerr << "Poll error, exit..." << std::endl;
            return;
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
                Client* tmp = new Client(new_pollfd.fd, ntohs(s_address.sin_port));
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
                        ft_delete_client(it->fd);
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