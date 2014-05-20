#pragma once
#include <string>
#include <libtcod.hpp>
#include <vector>
#include <map>

std::string rightAlignNumber(const int& input, const int& size = -1);
std::string rightAlignNumber(const double& input, const int& precision = 1);
std::string rightAlign(const std::string& input, const size_t& size);
std::string leftAlign(const std::string& input, const size_t& size);
std::string changeToDecimal(const std::string& input);

template<typename T> void removeInPlace(const int& i, std::vector<T>& v);
template<typename K, typename T> std::vector<K> getKeys(std::map<K, T>& mp);

#include "utility.tpp"

int stringToDecimal(const std::string& input);
typedef std::pair<int, int> coord;

extern const int WIDTH;
extern const int HEIGHT;
extern const int screenwidth;
extern const int screenheight;
extern TCODColor MabinogiBrown;
extern TCODColor DarkMabinogiBrown;