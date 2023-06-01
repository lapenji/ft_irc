#include "Server.hpp"

void Server::ft_manage_i(const std::string& first_part, int client_fd, Channel* chan, char c) {
    std::string resp = first_part + "\n";
    this->serverReplyMessage(resp.c_str(), client_fd);
    chan->sendToAllusersExcept(resp.c_str(), client_fd);
    if (c == '-')
        chan->setInviteOnly(false);
    else {
        chan->setInviteOnly(true);
    }
}

void Server::ft_manage_o(const std::string& first_part, int client_fd, Channel* chan, std::vector<std::string> tmp_splitted) {
    std::string resp = first_part + " " + tmp_splitted[3] + "\n";
    if (tmp_splitted[2][0] == '-') {
        if (this->channels.at(tmp_splitted[1])->isUserAdmin(find_client(tmp_splitted[3])) == true) {
            this->channels.at(tmp_splitted[1])->removeFromAdmin(find_client(tmp_splitted[3]));
            this->serverReplyMessage(resp.c_str(), client_fd);
            chan->sendToAllusersExcept(resp.c_str(), client_fd);
        }
    }
    else if (tmp_splitted[2][0] == '+') {
        if (this->channels.at(tmp_splitted[1])->isUserAdmin(find_client(tmp_splitted[3])) == false) {
            this->channels.at(tmp_splitted[1])->addAdmin(this->connected_clients.at(find_client(tmp_splitted[3])));
            this->serverReplyMessage(resp.c_str(), client_fd);
            chan->sendToAllusersExcept(resp.c_str(), client_fd);
        }
    }
}

void Server::ft_manage_t(const std::string& first_part, int client_fd, Channel* chan, char c) {
    std::string resp = first_part + "\n";
    this->serverReplyMessage(resp.c_str(), client_fd);
    chan->sendToAllusersExcept(resp.c_str(), client_fd);
    if (c == '-')
        chan->setFreeTopic(true);
    else {
        chan->setFreeTopic(false);
    }
}

void Server::ft_manage_l(const std::string& first_part, int client_fd, Channel* chan, std::vector<std::string> tmp_splitted) {
    if (tmp_splitted[2][0] == '+') {
        if (tmp_splitted.size() == 4) {
            if (isStringNumeric(tmp_splitted[3]) == true) {
            chan->setUserNrLimited(true);
            chan->setMaxUsers(std::atoi(tmp_splitted[3].c_str()));
            std::string resp = first_part + " " + tmp_splitted[3] + "\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
            chan->sendToAllusersExcept(resp.c_str(), client_fd);
            }
        }
    }
    else if (tmp_splitted[2][0] == '-') {
        std::string resp = first_part + "\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        chan->sendToAllusersExcept(resp.c_str(), client_fd);
        chan->setUserNrLimited(false);
    }
}

void Server::ft_manage_k(const std::string& first_part, int client_fd, Channel* chan, std::vector<std::string> tmp_splitted) {
    if (tmp_splitted[2][0] == '+') {
        std::string resp = first_part + " " + tmp_splitted[3] + "\n";
        chan->setNeedPassword(true, tmp_splitted[3]);
        this->serverReplyMessage(resp.c_str(), client_fd);
        chan->sendToAllusersExcept(resp.c_str(), client_fd);
    }
    else {
        if (tmp_splitted[3] == chan->getPassword()) {
            chan->setNeedPassword(false, "");
            std::string resp = first_part + " " + tmp_splitted[3] + "\n";
        }
        else {
            return;
        }
    }
}

void    Server::ft_manage_mode(const std::string& tmp, int client_fd, const std::string& nick, const std::string& user) {
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (this->channels.find(tmp_splitted[1]) != this->channels.end()) {
        Channel* chan = this->channels.at(tmp_splitted[1]);
        if (tmp_splitted.size() == 3 && tmp_splitted[2][1] == 'b') {
            return;
        }
        if (chan->isUserAdmin(client_fd) && tmp_splitted.size() > 2) {
            std::string first_part = ":" + nick + "!" + user + " MODE " + tmp_splitted[1] + " " + tmp_splitted[2];
            if (tmp_splitted.size() > 2) {
                if (tmp_splitted[2][1] == 'i') {
                    ft_manage_i(first_part, client_fd, chan, tmp_splitted[2][0]);
                }
                else if (tmp_splitted[2][1] == 'o') {
                    ft_manage_o(first_part, client_fd, chan, tmp_splitted);   
                }
                else if (tmp_splitted[2][1] == 't') {
                    ft_manage_t(first_part, client_fd, chan, tmp_splitted[2][0]); 
                }
                else if (tmp_splitted[2][1] == 'l') {
                    ft_manage_l(first_part, client_fd, chan, tmp_splitted);
                }
                else if (tmp_splitted[2][1] == 'k') {
                    ft_manage_k(first_part, client_fd, chan, tmp_splitted);
                }
            }
        }
        else if (chan->isUserAdmin(client_fd) == false && tmp_splitted.size() > 2) {
            std::string resp = ":SovietServer 482 " + nick + " " + tmp_splitted[1] + " :You must have channel op access or above to set channel mode "
                + tmp_splitted[2] + "\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
        if (tmp_splitted.size() == 2) {
            std::string resp = ":SovietServ 324 " + nick + " " + tmp_splitted[1] + " +n";
            if (chan->getNeedPassword() == true) {
                resp += "k";
            }
            if (chan->getInviteOnly() == true) {
                resp += "i";
            }
            if (chan->getFreeTopic() == false) {
                resp += "t";
            }
            resp += "\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
    }
}
