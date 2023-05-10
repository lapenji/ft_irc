#include "server_side.hpp"

int main (int argc, char **argv) {
    
    if (argc != 3) {
        std::cerr << "Wrong number of params, exit..." << std::endl;
        return (-1);
    }

    Server server(argv[2]);
    server.startServer();
    return (0);
}