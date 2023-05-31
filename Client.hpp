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
    bool printed;
    bool cap;
    bool aut;

    public:
    Client(int fd, const std::string& hostname, int port);
    ~Client();
    void    setPassword(const std::string& password);
    void    setNickname(const std::string& nick);
    void    setFullName(const std::string& full);
    void    setUserName(const std::string& user);
    void    setHostname(const std::string& host);
    void    setServerName(const std::string& servername);
    void    setPrinted(bool printed);   //////TEST
    void    setCap(bool cap);
    const std::string&  getNick();
    const std::string&  getFull();
    const std::string&  getUser();
    const std::string&  getPass();
    bool    getPrinted();   ////TEST
    bool    getCap();
    int     getFd();
    bool    getAut();
    void    setAut(bool aut);
};

#endif