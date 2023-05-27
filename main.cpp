#include "Server.hpp"

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