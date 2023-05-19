#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <iostream>

class Client {
    private:
    int         fd;
    std::string password;   //creata per fare un check in caso di doppio inserimento del comando PASS
    std::string nickname;
    std::string fullName;
    std::string hostname;
    std::string username;
    std::string serverName;
    int         port;

    public:
    Client(int fd, const std::string& hostname, int port);
    ~Client();
    void    setPassword(const std::string& password);
    void    setNickname(const std::string& nick);
    void    setFullName(const std::string& full);
    void    setUserName(const std::string& user);
    void    setHostname(const std::string& host);
    void    setServerName(const std::string& servername);
    const std::string&  getNick();
    const std::string&  getFull();
    const std::string&  getUser();
    const std::string&  getPass();
};

#endif