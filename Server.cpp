#include "Server.hpp"

void    printMap(std::multimap<std::string, std::string> myMap) { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "STAMPO IL MAP" << std::endl;
    for(std::multimap<std::string, std::string>::const_iterator it = myMap.begin();
    it != myMap.end(); ++it)
    {
    std::cout << it->first << ": " << it->second << std::endl;
    }
}


std::string	extractType(std::string& fileName) {
	size_t pos;

	pos = fileName.find_last_of(".");
	if (pos == std::string::npos) { //SE NON C'E ESTENSIONE STAMPO ERRORE E CHIUDO (CREDO NON CI ENTRI MAI)
		return "";
	}
	std::string type = fileName.substr(pos + 1);
	if (type == "js")
		type = "javascript";
	return type;
}

Server::Server(int masterSock, struct sockaddr_in  address) : masterSock(masterSock), address(address), addrlen(sizeof(address)) {
}

Server::~Server() {
	std::cout << "DISTRUZIONE" << std::endl;
}

void    Server::runServer() {
	int ret = 1;
	//INIZIALIZZO ARRAY DI SOCKET A 0
	for (int i = 0; i < MAX_CLIENTS; i++)
		clientSocket[i] = 0;

	while (1) {
		//inizializza il socket set a 0
		FD_ZERO(&readfds);

		//aggiungi master socket al set
		FD_SET(masterSock, &readfds);
		maxSd = masterSock;

		//aggiungo socket all'array
		for (int i = 0; i < MAX_CLIENTS; i++) {
			sd = clientSocket[i];
			if (sd > 0)
				FD_SET(sd, &readfds);
			//MAXSD SERVE PER SELECT!
			if (sd > maxSd)
				maxSd = sd;
		}
		
		ret = select(maxSd + 1, &readfds, NULL, NULL, NULL);
		std::cout << "RET = " << ret << std::endl;
		if ((ret < 0) && (errno != EINTR)) {
			std::cout << "SELECT ERROR VERO!" << std::endl;
		}

		ret = FD_ISSET(masterSock, &readfds);
		if (ret != 0) {
			newSock = accept(masterSock, (struct sockaddr *)&address, (socklen_t *)&addrlen);
			if (newSock < 0) {
				perror("accept error!");
				exit(EXIT_FAILURE);
			}
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newSock , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

			//AGGIUNGO NUOVO SOCKET ALL'ARRAY
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (clientSocket[i] == 0) {
					clientSocket[i] = newSock;
					printf("Adding to list of sockets as %d\n" , i);
					break;
				}
			}
		}
		for (int i = 0; i < MAX_CLIENTS; i++) {
			sd = clientSocket[i];

			if (FD_ISSET(sd, &readfds)) {
				valread = read (sd, buffer, 1024);
				if (valread == 0) {
					getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
					close (sd);
					clientSocket[i] = 0;
				}
				else {
					buffer[valread] = '\0';
					std::string tmp = buffer;
					std::string cmd = tmp.substr(0, tmp.find(" "));
					std::string arg = tmp.substr(tmp.find(" ") + 1);
					std::cout << "ECCO CMD" << std::endl;
					std::cout << cmd << std::endl;
					std::cout << "ECCO ARG" << std::endl;
					std::cout << arg << std::endl;
					if (cmd == "NICK") {
						std::pair<std::string, std::string>	tmp(cmd, arg);
						users.insert(tmp);
						printMap(users)
					}
				}
			}
		}
	}
}