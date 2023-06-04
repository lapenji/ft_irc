#include "Server.hpp"

void Server::ft_initialize_bot()
{
	std::ifstream insulti("insulti.txt");
	if (insulti.good())
	{
		std::string insultotmp;
		while (getline(insulti, insultotmp))
		{
			this->nrInsulti++;
			this->insulti.push_back(insultotmp);
		}
	}
	if (this->nrInsulti == 0)
	{
		this->nrInsulti = 1;
		this->insulti.push_back("dovrei insultarti ma non ho insulti a disposizione :(");
	}
}

std::string Server::ft_bot_get_insult()
{
	int rnd = rand() % this->nrInsulti;
	return this->insulti[rnd];
}

void Server::ft_bot_insult_someone(const std::string &nick, const std::string &chan, int client_fd)
{
	std::string resp = ":bot PRIVMSG " + chan + " :" + nick + " " + ft_bot_get_insult() + "\n";
	this->channels.at(chan)->sendToAllusersExcept(resp, client_fd);
	this->serverReplyMessage(resp.c_str(), client_fd);
}

void Server::ft_bot_no_user_in_chan(const std::string &nick, const std::string &chan, int client_fd)
{
	std::string resp = ":bot PRIVMSG " + chan + " :" + nick + " mi chiede di insultare qualcuno che non è nel canale, che idiota!\n";
	this->channels.at(chan)->sendToAllusersExcept(resp, client_fd);
	this->serverReplyMessage(resp.c_str(), client_fd);
}

void Server::ft_bot_insult_bot(const std::string &nick, const std::string &chan, int client_fd)
{
	std::string resp = ":bot PRIVMSG " + chan + " :" + nick + " mi chiede di insultare me stesso, non ci penso nemmeno!\n";
	this->channels.at(chan)->sendToAllusersExcept(resp, client_fd);
	this->serverReplyMessage(resp.c_str(), client_fd);
}

void Server::ft_bot_op(int client_fd, const std::string &chan, const std::string &nick)
{
	std::string resp = ":bot PRIVMSG " + chan + " :" + nick + " vorrebbe farmi op del canale ma io sono troppo modesto!\n";
	this->channels.at(chan)->sendToAllusersExcept(resp, client_fd);
	this->serverReplyMessage(resp.c_str(), client_fd);
}
