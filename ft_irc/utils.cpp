#include "utils.hpp"

std::vector<std::string> ft_splitBuffer(std::string tmp) {
    size_t pos = tmp.find("\r\n");
    std::vector<std::string> buffer_splitted;
    while (pos != std::string::npos) {
        buffer_splitted.push_back(tmp.substr(0, pos));
        tmp = tmp.substr(pos + 2);
        pos = tmp.find("\r\n");
    }
    // Stampa le stringhe separate
    std::cout << "STAMPO" << std::endl;
    for (size_t i = 0; i < buffer_splitted.size(); ++i) {
    std::cout << buffer_splitted[i] << std::endl;
    }
    return buffer_splitted;
}

void    printMap(std::map<int, Client*> myMap) { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "STAMPO IL MAP" << std::endl;
    for(std::map<int, Client*>::const_iterator it = myMap.begin();
    it != myMap.end(); ++it)
    {
        std::cout << it->first << ": " << it->second->getNick() << " - " << it->second->getFull() << std::endl;
    }
}