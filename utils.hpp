#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <csignal>
#include <cstdlib>
#include "Client.hpp"
#include "Channel.hpp"

bool isStringNumeric(const std::string& str);
std::vector<std::string> ft_splitString(const std::string& str);
std::vector<std::string> ft_splitBuffer(std::string tmp);
void    printMap(std::map<std::string, Channel*> myMap  /* std::map<int, Client*> myMap */); //DA TOGLIERE ALLA FINE

#endif