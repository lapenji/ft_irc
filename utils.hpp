#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include "Client.hpp"
#include <csignal>
#include <cstdlib>

bool isStringNumeric(const std::string& str);
std::vector<std::string> ft_splitString(const std::string& str);
std::vector<std::string> ft_splitBuffer(std::string tmp);
void    printMap(std::map<int, Client*> myMap); //DA TOGLIERE ALLA FINE

#endif