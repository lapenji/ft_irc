#include "Server.hpp"

int main() {
    Socket    obj;
    Server      serv(obj.getMasterSock(), obj.getAddress());
    serv.runServer();
}