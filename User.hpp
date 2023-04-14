#include <iostream>

class User {
    private:
    int id;
    std::string name;
    std::string fullname;
    public:
    User(std::string name, std::string fullname, int id);
    std::string getName();
    int         getId();
    std::string getFullName();
};

bool operator<(const User &c1, const User &c2);
std::ostream& operator<<(std::ostream &os, const User &c);