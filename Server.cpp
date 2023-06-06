#include "Server.hpp"

Server::Server(int port, const std::string &password) : opt(1), port(port), password(password)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    this->nrInsulti = 0;
    this->setSocket();
    ft_initialize_bot();
}

Server::~Server()
{
    std::cout << "Server destructor called...\nBye!" << std::endl;
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    while (it != this->connected_clients.end())
    {
        delete it->second;
        close(it->first);
        it++;
    }
    std::map<std::string, Channel *>::iterator it2 = this->channels.begin();
    while (it2 != this->channels.end())
    {
        delete it2->second;
        it2++;
    }
}

void Server::setSocket()
{
    if (this->socket_fd < 0)
    {
        std::cerr << "Error creating socket, exit...";
        exit(-1);
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "Error in socket options, exit..." << std::endl;
        exit(-1);
    }
    sockaddr_in address = {};
    bzero((char *)&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port);

    if (bind(socket_fd, (sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Binding error, exit..." << std::endl;
        exit(-1);
    }
    if (listen(socket_fd, 10) < 0)
    {
        std::cerr << "Socket listening error, exit..." << std::endl;
        exit(-1);
    }
    ft_print_banner();
    std::cout << "\033[1;33m\n->>\tlistening on port " << this->port << "\033[0m\n"
              << std::endl;
}

void Server::ft_print_banner()
{
    std::cout << "\x1b[31;1m   _____  ______      _______ ______ _______ " << std::endl;
    std::cout << "  / ____|/ __ \\ \\    / /_   _|  ____|__   __|" << std::endl;
    std::cout << " | (___ | |  | \\ \\  / /  | | | |__     | |   " << std::endl;
    std::cout << "  \\___ \\| |  | |\\ \\/ /   | | |  __|    | |" << std::endl;
    std::cout << "  ____) | |__| | \\  /   _| |_| |____   | |  " << std::endl;
    std::cout << " |_____/ \\____/ __\\/___|_____|______| _|_|_  " << std::endl;
    std::cout << "  / ____|  ____|  __ \\ \\    / /  ____|  __ \\ " << std::endl;
    std::cout << " | (___ | |__  | |__) \\ \\  / /| |__  | |__) |" << std::endl;
    std::cout << "  \\___ \\|  __| |  _  / \\ \\/ / |  __| |  _  /  " << std::endl;
    std::cout << "  ____) | |____| | \\ \\  \\  /  | |____| | \\ \\ " << std::endl;
    std::cout << " |_____/|______|_|  \\_\\  \\/   |______|_|  \\_\\\x1b[0m" << std::endl;

}

void Server::ft_print_motd(int client_fd)
{

    std::ifstream motd("motd.txt");

    if (motd.good())
    {
        ft_reply("375 ", " :SovietServer message of the day\n", client_fd);
        std::string linea;
        while (std::getline(motd, linea))
        {
            ft_reply("372 ", " :" + linea + "\n", client_fd);
        }
        ft_reply("376 ", " :End of message of the day.\n", client_fd);
        motd.close();
        //: acb10abd8c2c.example.com 376 lapenji :End of message of the day.
    }
    else
    {
        ft_reply("422 ", " :MOTD FILE MISSING\n", client_fd);
    }
}

void Server::ft_manage_who(const std::string &tmp, int client_fd, const std::string &nick)
{
    if (tmp.substr(tmp.find(" ") + 1) == "bot")
    {
        ft_reply("352 ", "bot bot 666.666.666.666 SovietServer stalin H :0 stalin\n", client_fd);
        ft_reply("315 ", "bot :End of /WHO list.\n", client_fd);
        return;
    }
    if (tmp.find("#") != std::string::npos && this->channels.find(tmp.substr(tmp.find(" ") + 1)) != this->channels.end())
    {
        Channel *chan = this->channels.at(tmp.substr(tmp.find(" ") + 1));
        std::string userList = chan->getUsersAsEq();
        std::vector<std::string> usersSplitted = ft_splitString(userList);
        std::vector<std::string>::iterator its = usersSplitted.begin();
        while (its != usersSplitted.end())
        {
            Client *client = this->connected_clients.at(this->find_client(*its));
            std::string op = "";
            if (chan->isUserAdmin(client->getFd()) == true)
            {
                op = "@";
            }
            std::string resp = ":SovietServer 352 " + nick + " " + tmp.substr(tmp.find(" ") + 1) + " " + client->getUser() + " " + client->getIp() + " SovietServer " + client->getNick() + " H" + op + " :0 " + client->getFull() + "\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
            its++;
        }
        if (chan->getIsBotInside() == true)
        {
            ft_reply("352 ", "" + tmp.substr(tmp.find(" ") + 1) + " bot 666.666.666.666 SovietServer stalin H :0 stalin\n", client_fd);
        }
    }
    else if (this->connected_clients.find(this->find_client(tmp.substr(tmp.find(" ") + 1))) != this->connected_clients.end())
    {
        Client *client = this->connected_clients.at(this->find_client(tmp.substr(tmp.find(" ") + 1)));
        std::string resp = ":SovietServer 352 " + nick + " * " + client->getUser() + " " + client->getIp() + " SovietServer " + client->getNick() + " H :0 " + client->getFull() + "\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
    }
    std::string resp2 = ":SovietServer 315 " + nick + " " + tmp.substr(tmp.find(" ") + 1) + " :End of /WHO list.\n";
    this->serverReplyMessage(resp2.c_str(), client_fd);
}

void Server::ft_manage_topic(const std::string &tmp, int client_fd)
{
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string msg = ft_joinStr(tmp_splitted, 2);
    Channel *chan = this->channels.at(tmp_splitted[1]);
    chan->changeTopic(msg, client_fd);
}

void Server::ft_manage_kick(const std::string &tmp, int client_fd)
{
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (this->channels.find(tmp_splitted[1]) != this->channels.end())
    {
        if (this->channels.at(tmp_splitted[1])->isUserInChan(this->find_client(tmp_splitted[2])) == false && this->channels.at(tmp_splitted[1])->getIsBotInside() == false)
        {
            ft_reply("441 ", (tmp_splitted[2] + " " + tmp_splitted[1] + " :They aren't on that channel\n"), client_fd);
            return;
        }
        // if (tmp_splitted[2] == "bot" && this->channels.at(tmp_splitted[1])->getIsBotInside() == false)
        // {
        //     ft_reply("441 ", (tmp_splitted[2] + " " + tmp_splitted[1] + " :They aren't on that channel\n"), client_fd);
        //     return;
        // }
        if (this->channels.at(tmp_splitted[1])->isUserAdmin(client_fd) == true)
        {
            std::string reason = "";
            reason += ft_joinStr(tmp_splitted, 3);
            std::string resp = ft_resp_at((" KICK " + tmp_splitted[1] + " " + tmp_splitted[2] + " " + reason), client_fd);
            if (tmp_splitted[2] == "bot")
            {
                this->channels.at(tmp_splitted[1])->setIsBotInside(false);
                this->channels.at(tmp_splitted[1])->sendToAllusersExcept(resp, client_fd);
                this->serverReplyMessage(resp.c_str(), client_fd);
                return;
            }
            this->serverReplyMessage(resp.c_str(), client_fd);
            std::map<int, Client *>::iterator it = this->connected_clients.begin();
            while (it != this->connected_clients.end())
            {
                if (it->second->getNick() == tmp_splitted[2])
                {
                    this->channels.at(tmp_splitted[1])->removeFromChan(it->second->getFd());
                }
                this->serverReplyMessage(resp.c_str(), it->second->getFd());
                it++;
            }
        }
        else
        {
            ft_reply("482 ", (tmp_splitted[1] + " :You must be a channel operator\n"), client_fd);
        }
    }
    else
    {
        ft_reply("401 ", (tmp_splitted[1] + " :No such channel!\n"), client_fd);
    }
}

void Server::ft_manage_invite(const std::string &tmp, int client_fd)
{
    bool success = false;
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string resp;
    std::string resp2 = ft_resp_at((" INVITE " + tmp_splitted[1] + " :" + tmp_splitted[2]), client_fd);
    std::map<int, Client *>::iterator it = this->connected_clients.begin();
    if (this->channels.find(tmp_splitted[2]) == this->channels.end())
    {
        ft_reply("401 ", (tmp_splitted[2] + " :No such channel\n"), client_fd);
        return;
    }
    if (this->channels.at(tmp_splitted[2])->checkIfAdmin(client_fd) == false)
    {
        ft_reply("482 ", (tmp_splitted[2] + " :You must be admin to invite someone!\n"), client_fd);
    }
    else
    {
        if (tmp_splitted[1] == "bot")
        {
            if (this->channels.find(tmp_splitted[2]) != this->channels.end())
            {
                if (this->channels.at(tmp_splitted[2])->getIsBotInside() == true)
                {
                    ft_reply("443 ", (tmp_splitted[1] + " " + tmp_splitted[2] + " :is already on channel!\n"), client_fd);
                    return;
                }
                if (this->channels.at(tmp_splitted[2])->getUserNrLimited() == true && this->channels.at(tmp_splitted[2])->getHowManyUsers() >= this->channels.at(tmp_splitted[2])->getMaxUsers())
                {
                    std::string resp = ":bot!stalin@666.666.666.666 PRIVMSG " + this->connected_clients.at(client_fd)->getNick() + " :Non posso entrare nel canale " + tmp_splitted[2] + " perchè è pieno!\n";
                    this->serverReplyMessage(resp.c_str(), client_fd);
                    return;
                }
                this->channels.at(tmp_splitted[2])->setIsBotInside(true);
                std::string resp = ":bot!stalin@666.666.666.666 JOIN " + tmp_splitted[2] + "\n";
                this->channels.at(tmp_splitted[2])->sendToAllusersExcept(resp.c_str(), client_fd);
                this->serverReplyMessage(resp.c_str(), client_fd);
                std::string resp2 = ":bot PRIVMSG " + tmp_splitted[2] + " :Ciao merde, sono qua per insultarvi!\n";
                this->channels.at(tmp_splitted[2])->sendToAllusersExcept(resp2.c_str(), client_fd);
                this->serverReplyMessage(resp2.c_str(), client_fd);
                return;
            }
            else
            {
                ft_reply("401 ", (tmp_splitted[2] + " :No such channel\n"), client_fd);
                return;
            }
        }
        if (this->channels.at(tmp_splitted[2])->isUserInChan(this->find_client(tmp_splitted[1])) == true)
        {
            ft_reply("443 ", (tmp_splitted[1] + " " + tmp_splitted[2] + " :is already on channel!\n"), client_fd);
            return;
        }
        this->channels.at(tmp_splitted[2])->addToInvited(tmp_splitted[1]);
        while (it != this->connected_clients.end())
        {
            if (it->second->getNick() == tmp_splitted[1])
            {
                this->serverReplyMessage(resp2.c_str(), it->first);
                success = true;
            }
            it++;
        }
        if (success == true)
        {
            ft_reply("341 ", (tmp_splitted[1] + " " + tmp_splitted[2] + "\n"), client_fd);
        }
        else
        {
            ft_reply("401 ", (tmp_splitted[1] + " :No such nick/channel\n"), client_fd);
        }
    }
}

void Server::ft_manage_privmsg(const std::string &tmp, int client_fd)
{
    std::string nick = this->connected_clients.at(client_fd)->getNick();
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::string msg = ft_joinStr(tmp_splitted, 2);
    if (tmp_splitted[1] == "bot")
    {
        std::string resp = ":bot!stalin@666.666.666.666 PRIVMSG " + nick + " :Non rispondo ai messaggi privati!\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (tmp_splitted[1][0] == '#')
    {
        if (this->channels.find(tmp_splitted[1]) == this->channels.end())
        {
            ft_reply("401 ", (tmp_splitted[1] + " :No such nick/channel\n"), client_fd);
            return;
        }
        Channel *chan = this->channels.at(tmp_splitted[1]);
        if (this->channels.find(tmp_splitted[1]) != this->channels.end())
        {
            std::map<int, Client *>::iterator it = chan->clients.begin();
            if (chan->isUserInChan(client_fd) == true)
            {
                while (it != chan->clients.end())
                {
                    if (it->first != client_fd)
                    {
                        // SAREBBE PIU BELLO CON HOST ECCETERA
                        std::string resp = ":" + nick + " PRIVMSG " + tmp_splitted[1] + " " + msg + "\n";
                        this->serverReplyMessage(resp.c_str(), it->first);
                    }
                    it++;
                }
                if (msg.substr(1, 4) == "bot " && chan->getIsBotInside() == true)
                {
                    std::string insultato = msg.substr(msg.find("insulta") + 8);
                    insultato = insultato.substr(0, insultato.find(" "));
                    if (insultato == "bot")
                    {
                        ft_bot_insult_bot(nick, chan->getName(), client_fd);
                        return;
                    }
                    if (this->find_client(insultato) != -1 && chan->isUserInChan(find_client(insultato)))
                    {
                        ft_bot_insult_someone(insultato, chan->getName(), client_fd);
                    }
                    else
                    {
                        ft_bot_no_user_in_chan(nick, chan->getName(), client_fd);
                    }
                }
            }
            else
            {
                ft_reply("404 ", (tmp_splitted[1] + " :Cannot send to channel (no external messages)\n"), client_fd);
            }
        }
    }
    else

    {
        if (this->find_client(tmp_splitted[1]) == -1)
        {
            ft_reply("401 ", (tmp_splitted[1] + " :No such nick/channel\n"), client_fd);
        }
        else
        {
            std::string resp = ft_resp_at((" PRIVMSG " + tmp_splitted[1] + " " + ft_joinStr(tmp_splitted, 2)), client_fd);
            this->serverReplyMessage(resp.c_str(), this->find_client(tmp_splitted[1]));
        }
    }
}

void Server::ft_manage_quit(const std::string &tmp, int client_fd)
{
    std::vector<int> alreadyComunicated;
    alreadyComunicated.clear();
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    std::map<std::string, Channel *>::iterator it = this->channels.begin();
    while (it != this->channels.end())
    {
        if (it->second->isUserInChan(client_fd) == true)
        {
            it->second->removeFromChan(client_fd);
            std::string resp = ft_resp_at((" QUIT " + ft_joinStr(tmp_splitted, 1)), client_fd);
            std::map<int, Client *>::iterator it2 = it->second->clients.begin();
            while (it2 != it->second->clients.end())
            {
                if (is_fd_in_vector(it2->first, alreadyComunicated) == false)
                {
                    this->serverReplyMessage(resp.c_str(), it2->first);
                    alreadyComunicated.push_back(it2->first);
                }
                it2++;
            }
        }
        it++;
    }
}

void Server::ft_manage_part(const std::string &tmp, Client *client)
{
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    Channel *chan = this->channels.at(tmp_splitted[1]);
    if (tmp_splitted.size() > 2)
    {
        chan->removeClient(client, ft_joinStr(tmp_splitted, 2));
    }
    else
    {
        chan->removeClient(client, "");
    }
    if (chan->isEmpty() == true)
    {
        delete this->channels.at(tmp_splitted[1]);
        this->channels.erase(tmp_splitted[1]);
    }
}

void Server::ft_multiple_join(std::vector<std::string> chans, int client_fd, Client *client)
{
    std::vector<std::string>::iterator it = chans.begin();
    while (it != chans.end())
    {
        if (this->channels.find(*it) == this->channels.end())
        {
            this->channels.insert(std::make_pair(*it, new Channel(client, *it)));
            Channel *chan = this->channels.at(*it);
            chan->addAdmin(client);
        }
        Channel *chan = this->channels.at(*it);
        if (chan->getInviteOnly() == true && chan->isInvited(client->getNick()) == false)
        {
            ft_reply("473 ", (*it + " :Cannot join channel (Invite only)\n"), client_fd);
            return;
        }
        if (chan->getUserNrLimited() == true && chan->getHowManyUsers() >= chan->getMaxUsers())
        {
            ft_reply("471 ", (*it + " :Cannot join channel (Channel is full)\n"), client_fd);
            return;
        }
        if (chan->getNeedPassword() == true)
        {
            ft_reply("475 ", ":Cannot join channel (Incorrect channel key)\n", client_fd);
            return;
        }
        chan->addClient(client);
        if (chan->isInvited(client->getNick()) == true)
        {
            chan->removeToInvited(client->getNick());
        }
        ft_print_topic(chan, *it, client_fd);
        it++;
    }
}

void Server::ft_manage_join(const std::string &tmp, int client_fd, Client *client)
{
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (tmp_splitted.size() < 2)
    {
        std::string resp = build_461("JOIN", client->getNick()); //////////// da modidificare con ft_reply, forse
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (tmp_splitted[1][0] != '#')
    {
        ft_reply("403 ", (tmp_splitted[1] + " :Invalid channel name\n"), client_fd);
        return;
    }
    if (this->channels.find(tmp_splitted[1]) != this->channels.end() && this->channels.at(tmp_splitted[1])->isUserInChan(client_fd) == true)
    {
        return;
    }
    if (tmp_splitted[1].find(",") != std::string::npos)
    {
        ft_multiple_join(ft_splitChans(tmp_splitted[1]), client_fd, client);
        return;
    }
    if (this->channels.find(tmp_splitted[1]) == this->channels.end())
    {
        this->channels.insert(std::make_pair(tmp_splitted[1], new Channel(client, tmp_splitted[1])));
        Channel *chan = this->channels.at(tmp_splitted[1]);
        chan->addAdmin(client);
    }
    Channel *chan = this->channels.at(tmp_splitted[1]);
    if (chan->getInviteOnly() == true && chan->isInvited(client->getNick()) == false)
    {
        ft_reply("473 ", (tmp_splitted[1] + " :Cannot join channel (Invite only)\n"), client_fd);
        return;
    }
    if (chan->getUserNrLimited() == true && chan->getHowManyUsers() >= chan->getMaxUsers())
    {
        ft_reply("471 ", (tmp_splitted[1] + " :Cannot join channel (Channel is full)\n"), client_fd);
        return;
    }
    if (chan->getNeedPassword() == true)
    {
        if (tmp_splitted.size() < 3 || tmp_splitted[2] != chan->getPassword())
        {
            ft_reply("475 ", ":Cannot join channel (Incorrect channel key)\n", client_fd);
            return;
        }
    }
    chan->addClient(client);
    if (chan->isInvited(client->getNick()) == true)
    {
        chan->removeToInvited(client->getNick());
    }
    ft_print_topic(chan, tmp_splitted[1], client_fd);

    // printMap(this->channels);
}

void Server::ft_manage_ping(const std::string &tmp, int client_fd)
{
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (tmp_splitted.size() == 2)
    {
        if (tmp_splitted[1].find("LAG") != std::string::npos)
        {
            std::string resp = ":SovietServer PONG SovietServer " + tmp_splitted[1] + "\n";
            this->serverReplyMessage(resp.c_str(), client_fd);
            return;
        }
        this->serverReplyMessage(":SovietServer PONG SovietServer :SovietServer\r\n", client_fd);
    }
}

bool Server::ft_manage_pass(const std::string &tmp)
{
    if (tmp.length() > 5)
    {
        if (tmp.substr(tmp.find(":") + 1) == this->password)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void Server::ft_manage_userhost(const std::string &tmp, int client_fd, Client *client)
{
    std::string resp = ":SovietServer 302 " + client->getNick() + " :";
    if (tmp.substr(tmp.find(" ") + 1) == "bot")
    {
        resp += "bot=+stalin@666.666.666.666";
    }
    if (this->connected_clients.find(this->find_client(tmp.substr(tmp.find(" ") + 1))) != this->connected_clients.end())
    {
        Client *client = this->connected_clients.at(this->find_client(tmp.substr(tmp.find(" ") + 1)));
        resp += client->getNick() + "=+" + client->getUser() + "@" + client->getIp();
        //: 84c93a935673.example.com 302 ciccio :ciccio=+1@172.17.0.1
    }
    resp += "\n";
    this->serverReplyMessage(resp.c_str(), client_fd);
}

bool Server::ft_manage_user(const std::string &tmp, int client_fd, Client *client)
{
    std::vector<std::string> result = ft_splitString(tmp);
    if (client->getFull() != "")
    {
        ft_reply("462 ", ":You may not reregister\n", client_fd);
        return true;
    }
    if (client->getAut() == true)
    {
        if (result.size() >= 5)
        {
            ft_create_map_user(result, client_fd);
        }
        ft_print_welcome_msg(client_fd, client);
        return true;
    }
    else
    {
        ft_reply("464 ", ":Password incorrect or missing!\n", client_fd);
        return false;
    }
}

void Server::ft_manage_nick(const std::string &tmp, int client_fd, Client *client)
{
    std::vector<std::string> tmp_splitted = ft_splitString(tmp);
    if (tmp_splitted.size() < 2)
    {
        std::string resp = build_461("NICK", "");
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (isNickInUse(tmp.substr(tmp.find(" ") + 1)) == true)
    {
        std::string resp = ":SovietServer 433 " + tmp.substr(tmp.find(" ") + 1) + " :Nickname already in use\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (isNickValid(tmp.substr(tmp.find(" ") + 1)) == false)
    {
        std::string resp = ":SovietServer 432 " + tmp.substr(tmp.find(" ") + 1) + " :Erroneous Nickname\n";
        this->serverReplyMessage(resp.c_str(), client_fd);
        return;
    }
    if (client->getNick() != "")
    {
        std::string oldNick = client->getNick();
        client->setNickname(tmp.substr(tmp.find(" ") + 1));
        std::map<std::string, Channel *>::iterator it = this->channels.begin();
        std::string resp = ":" + oldNick + "!" + client->getUser() + "@" + client->getIp() + " NICK " + client->getNick() + "\n";
        while (it != this->channels.end())
        {
            if (it->second->isUserInChan(client_fd) == true)
            {
                it->second->sendToAllusersExcept(resp, client_fd);
            }
            it++;
        }
        this->serverReplyMessage(resp.c_str(), client_fd);
        std::cout << "\x1b[33;1m-\x1b[0m\tclient \x1b[32;1m" << client->getIp() << "\x1b[0m is now known as \x1b[35;1m" << client->getNick() << "\x1b[0m" << std::endl;
    }
    else if (tmp.length() > 5)
    {
        client->setNickname(tmp.substr(tmp.find(" ") + 1));
    }
}

int Server::handle_client_request(int client_fd)
{

    Client *client = this->connected_clients.at(client_fd);

    char buffer[1024];
    bzero(buffer, 1024);
    int num_bytes;
    num_bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    std::string tmpbuff = buffer;
    if (num_bytes != 0 && strchr(buffer, '\n') == NULL)
    {
        std::cout << "Entro\n" << std::endl;
        while(strchr(buffer, '\n') == NULL)
        {
            bzero(buffer, 1024);
            num_bytes += recv(client_fd, buffer, sizeof(buffer), 0);
            tmpbuff += buffer;
        }
    }
    else if (num_bytes == 0)
    {
        //std::cout << "->>\tConnessione chiusa sulla porta " << this->port << std::endl;
        return -1;
    }
    else
    {
        //std::cout << "\033[1;31m\n--->>> RICEVUTO QUESTO MESSAGGIO DAL CLIENT: " << client_fd << " <<<---\n\n\033[0m" << buffer << std::endl;
        std::string tmp = buffer;
        std::vector<std::string> buffer_splitted = ft_splitBuffer(tmp);
        if (client->getCap() == false)
        {
            this->serverReplyMessage("IRC CAP REQ :none\n", client_fd);
            client->setCap(true);
        }
        for (size_t i = 0; i < buffer_splitted.size(); ++i)
        {
            if (buffer_splitted[i].find("NICK") == 0)
            {
                ft_manage_nick(buffer_splitted[i], client_fd, client);
            }
            else if (buffer_splitted[i].find("USER") == 0 && buffer_splitted[i].find("USERHOST") != 0)
            {
                if (ft_manage_user(buffer_splitted[i], client_fd, client) == false)
                    return -1;
                std::cout << "\x1b[33;1m-\x1b[0m\tclient \x1b[32;1m" << client->getIp() << "\x1b[0m completed connection as \x1b[35;1m" << client->getNick() << "\x1b[0m" << std::endl;
            }
            else if (buffer_splitted[i].find("USERHOST") == 0)
            {
                ft_manage_userhost(buffer_splitted[i], client_fd, client);
            }
            else if (buffer_splitted[i].find("PASS") == 0)
            {
                if (ft_manage_pass(buffer_splitted[i]) == false)
                {
                    std::string resp;
                    client->setAut(false);
                }
                else
                {
                    client->setAut(true);
                }
            }
            else if (buffer_splitted[i].find("MODE") == 0)
            {
                ft_manage_mode(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("PING") == 0)
            {
                ft_manage_ping(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("JOIN") == 0)
            {
                ft_manage_join(buffer_splitted[i], client_fd, client);
            }
            else if (buffer_splitted[i].find("PART") == 0)
            {
                ft_manage_part(buffer_splitted[i], client);
            }
            else if (buffer_splitted[i].find("PRIVMSG") == 0)
            {
                ft_manage_privmsg(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("TOPIC") == 0)
            {
                ft_manage_topic(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("INVITE") == 0)
            {
                ft_manage_invite(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("KICK") == 0)
            {
                ft_manage_kick(buffer_splitted[i], client_fd);
            }
            else if (buffer_splitted[i].find("WHO") == 0)
            {
                ft_manage_who(buffer_splitted[i], client_fd, client->getNick());
            }
            else if (buffer_splitted[i].find("QUIT") == 0)
            {
                ft_manage_quit(buffer_splitted[i], client_fd);
                std::cout << "\x1b[31;1m-\x1b[0m\tclient \x1b[32;1m" << client->getIp() << "\x1b[0m known as \x1b[35;1m" << client->getNick() << "\x1b[0m is quitting!"  << std::endl;
                return -1;
            }
            else if (buffer_splitted[i].find("CAP") == std::string::npos)
            {
                ft_reply("421 ", (buffer_splitted[i].substr(0, buffer_splitted[i].find(" ")) + " :Unknown command\n"), client_fd);
            }
        }
        if (client->getNick().size() > 0 && client->getFull().size() > 0 && client->getPrinted() == false)
        {
            ft_print_welcome_msg(client_fd, client);
        }
    }
    return 0;
}

void Server::startServer()
{

    sockaddr_in s_address = {};
    pollfd new_pollfd = {socket_fd, POLLIN, 0};
    this->poll_vec.push_back(new_pollfd);
    while (true)
    {
        signal(SIGINT, ft_signal_ctrl_c);
        if (poll(this->poll_vec.data(), this->poll_vec.size(), -1) == -1)
        {
            // std::cerr << "Poll error, exit..." << std::endl;
            return;
        }
        if (this->poll_vec[0].revents & POLLIN)
        {
            sockaddr_in client_address = {};
            socklen_t client_address_len = sizeof(client_address);
            int client_fd = accept(this->socket_fd, (sockaddr *)&client_address, &client_address_len);
            if (client_fd < 0)
            {
                std::cerr << "->>\tNew connection refused" << std::endl;
            }
            else
            {
                //std::cout << "->>\tNew connection accepted" << std::endl;
                ///////////STAMPA IP DEL CLIENT
                char ipAddress[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(client_address.sin_addr), ipAddress, INET_ADDRSTRLEN);

                //std::cout << "Indirizzo IP del client: " << ipAddress << std::endl;
                //////////////////////
                pollfd new_pollfd = {client_fd, POLLIN, 0};
                this->poll_vec.push_back(new_pollfd);
                char hostname[NI_MAXHOST];
                getnameinfo((struct sockaddr *)&s_address, sizeof(s_address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
                Client *tmp = new Client(new_pollfd.fd, ntohs(s_address.sin_port));
                tmp->setIp(ipAddress);                             ///////aggiunto per settare ip, sarebbe meglio nel costruttore
                //std::cout << "ip = " << tmp->getIp() << std::endl; ////////
                std::cout << "\x1b[32;1m-\x1b[0m\tnew client connected with ip: \x1b[32;1m" << tmp->getIp() << "\x1b[0m" << std::endl;
                this->connected_clients.insert(std::make_pair(client_fd, tmp));
                //std::cout << "->>\tsocket:" << new_pollfd.fd << std::endl;
            }
        }
        std::vector<pollfd>::iterator it = this->poll_vec.begin();
        it++;
        while (it != this->poll_vec.end())
        {
            if (it->revents & POLLIN)
            {
                if (handle_client_request(it->fd) == -1)
                {
                    if (this->connected_clients.find(it->fd) != this->connected_clients.end())
                    {
                        ft_delete_client(it->fd);
                        this->connected_clients.erase(it->fd);
                    }
                    //std::cout << it->fd << " ->>\tDisconnected" << std::endl;
                    close(it->fd);
                    this->poll_vec.erase(it);
                    it--;
                }
            }
            it++;
        }
    }
}
