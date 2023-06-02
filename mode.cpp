#include "Server.hpp"

void Server::ft_manage_i(const std::string &resp, int client_fd, Channel *chan, char c)
{
    this->serverReplyMessage(resp.c_str(), client_fd);
    chan->sendToAllusersExcept(resp.c_str(), client_fd);
    if (c == '-')
        chan->setInviteOnly(false);
    else
    {
        chan->setInviteOnly(true);
    }
}

void Server::ft_manage_o(const std::string &resp, int client_fd, Channel *chan, std::vector<std::string> tmp_splitted)
{
    if (tmp_splitted[2][0] == '-')
    {
        if (chan->isUserAdmin(find_client(tmp_splitted[3])) == true)
        {
            chan->removeFromAdmin(find_client(tmp_splitted[3]));
            this->serverReplyMessage(resp.c_str(), client_fd);
            chan->sendToAllusersExcept(resp.c_str(), client_fd);
        }
    }
    else if (tmp_splitted[2][0] == '+')
    {
        if (chan->isUserAdmin(find_client(tmp_splitted[3])) == false)
        {
            chan->addAdmin(this->connected_clients.at(find_client(tmp_splitted[3])));
            this->serverReplyMessage(resp.c_str(), client_fd);
            chan->sendToAllusersExcept(resp.c_str(), client_fd);
        }
    }
}

// void Server::ft_reply(const std::string &num, const std::string &arg, int client_fd)

void Server::ft_manage_b(const std::string chan, int client_fd)
{
    //":SovietServer 368 lapenji #cacca :End of channel ban list"
    std::string resp = ":SovietServer 368 " + this->connected_clients.at(client_fd)->getNick() + " " + chan + " :End of channel ban list\n";
    this->serverReplyMessage(resp.c_str(), client_fd);
}

void Server::ft_manage_t(const std::string &resp, int client_fd, Channel *chan, char c)
{
    this->serverReplyMessage(resp.c_str(), client_fd);
    chan->sendToAllusersExcept(resp.c_str(), client_fd);
    if (c == '-')
        chan->setFreeTopic(true);
    else
    {
        chan->setFreeTopic(false);
    }
}

void Server::ft_manage_l(const std::string &first_part, int client_fd, Channel *chan, std::vector<std::string> tmp_splitted)
{
    if (tmp_splitted[2][0] == '+')
    {
        if (tmp_splitted.size() == 4)
        {
            if (isStringNumeric(tmp_splitted[3]) == true)
            {
                chan->setUserNrLimited(true);
                chan->setMaxUsers(std::atoi(tmp_splitted[3].c_str()));
                std::string resp = first_part + " " + tmp_splitted[3] + "\n";
                this->serverReplyMessage(resp.c_str(), client_fd);
                chan->sendToAllusersExcept(resp.c_str(), client_fd);
            }
        }
    }
    else if (tmp_splitted[2][0] == '-')
    {
        std::string resp = first_part + "\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        chan->sendToAllusersExcept(resp.c_str(), client_fd);
        chan->setUserNrLimited(false);
    }
}

void Server::ft_manage_k(const std::string &resp, int client_fd, Channel *chan, std::vector<std::string> tmp_splitted)
{
    if (tmp_splitted[2][0] == '+')
    {
        chan->setNeedPassword(true, tmp_splitted[3]);
        this->serverReplyMessage(resp.c_str(), client_fd);
        chan->sendToAllusersExcept(resp.c_str(), client_fd);
    }
    else
    {
        if (tmp_splitted[3] == chan->getPassword())
        {
            chan->setNeedPassword(false, "");
            this->serverReplyMessage(resp.c_str(), client_fd);
            chan->sendToAllusersExcept(resp.c_str(), client_fd);
        }
        else
        {
            return;
        }
    }
}

void Server::ft_manage_mode(const std::string &tmp, int client_fd)
{
    std::string nick = this->connected_clients.at(client_fd)->getNick();
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (this->channels.find(tmp_splitted[1]) != this->channels.end())
    {
        Channel *chan = this->channels.at(tmp_splitted[1]);
        if (tmp_splitted.size() == 3 && tmp_splitted[2][1] == 'b')
        {
            ft_manage_b(tmp_splitted[1], client_fd);
            return;
        }
        if (chan->isUserAdmin(client_fd) && tmp_splitted.size() > 2)
        {
            if (tmp_splitted.size() > 2)
            {
                if (tmp_splitted[2][1] == 'i')
                {
                    ft_manage_i(ft_resp_at((" MODE " + tmp_splitted[1] + " " + tmp_splitted[2] + "\n"), client_fd), client_fd, chan, tmp_splitted[2][0]);
                }
                else if (tmp_splitted[2][1] == 'o')
                {
                    ft_manage_o(ft_resp_at((" MODE " + tmp_splitted[1] + " " + tmp_splitted[2] + " " + tmp_splitted[3] + "\n"), client_fd), client_fd, chan, tmp_splitted);
                }
                else if (tmp_splitted[2][1] == 't')
                {
                    ft_manage_t(ft_resp_at((" MODE " + tmp_splitted[1] + " " + tmp_splitted[2] + "\n"), client_fd), client_fd, chan, tmp_splitted[2][0]);
                }
                else if (tmp_splitted[2][1] == 'l')
                {
                    std::string first_part = ":" + nick + "!" + this->connected_clients.at(client_fd)->getUser() + "@" + this->connected_clients.at(client_fd)->getIp() + " MODE " + tmp_splitted[1] + " " + tmp_splitted[2];
                    ft_manage_l(first_part, client_fd, chan, tmp_splitted);
                }
                else if (tmp_splitted[2][1] == 'k')
                {
                    ft_manage_k(ft_resp_at((" MODE " + tmp_splitted[1] + " " + tmp_splitted[2] + " " + tmp_splitted[3] + "\n"), client_fd), client_fd, chan, tmp_splitted);
                }
            }
        }
        else if (chan->isUserAdmin(client_fd) == false && tmp_splitted.size() > 2)
        {
            ft_reply("482 ", (tmp_splitted[1] + " :You must have channel op access or above to set channel mode " + tmp_splitted[2] + "\n"), client_fd);
        }
        if (tmp_splitted.size() == 2)
        {
            std::string resp = ":SovietServ 324 " + nick + " " + tmp_splitted[1] + " +n";
            if (chan->getNeedPassword() == true)
            {
                resp += "k";
            }
            if (chan->getInviteOnly() == true)
            {
                resp += "i";
            }
            if (chan->getFreeTopic() == false)
            {
                resp += "t";
            }
            resp += "\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
        }
    }
}
