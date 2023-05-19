#include "server_side.hpp"

bool isStringNumeric(const std::string& str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}



int main (int argc, char **argv) {
    
    if (argc != 3) {
        std::cerr << "Wrong number of params, exit..." << std::endl;
        return (-1);
    }
    if (isStringNumeric(argv[1]) == false) {
        std::cerr << "Wrong port number, exit..." << std::endl;
        return (-1);
    }
    int port = atoi(argv[1]);
    Server server(port, argv[2]);
    server.startServer();
    return (0);
}