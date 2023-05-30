#include "utils.hpp"

bool isStringNumeric(const std::string& str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> ft_splitString(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string word;
    while (ss >> word) {
        result.push_back(word);
    }
    return result;
}

std::vector<std::string> ft_splitBuffer(std::string tmp) {
    size_t pos = tmp.find("\n");
    std::vector<std::string> buffer_splitted;
    while (pos != std::string::npos) {
        buffer_splitted.push_back(tmp.substr(0, pos));
        tmp = tmp.substr(pos + 1);
        pos = tmp.find("\n");
    }
    // Stampa le stringhe separate
   /*  std::cout << "STAMPO" << std::endl;
    for (size_t i = 0; i < buffer_splitted.size(); ++i) {
    std::cout << buffer_splitted[i] << std::endl;
    } */
    return buffer_splitted;
}

void    printMap(std::map<std::string, Channel*> myMap /* std::map<int, Client*> myMap */) { //FUNZIONE PER STAMPARE IL CONTENUTO DEL MAP (PER CONTROLLARLO)
    std::cout << "STAMPO IL MAP" << std::endl;
    //for(std::map<int, Client*>::const_iterator it = myMap.begin();
    for(std::map<std::string, Channel*>::const_iterator it = myMap.begin(); it != myMap.end(); ++it)
    {
        std::cout << it->first << std::endl;
    }
}