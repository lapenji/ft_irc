#include "Server.hpp"

bool run = true;

void    ft_signal_ctrl_c(int sig) {
    (void)sig;
    std::cout << "\nexiting for ctrl+c..." << std::endl;
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
