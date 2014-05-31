#pragma once
#include <string>
#include <libtcod.hpp>
#include <vector>
#include "Ship.h"
#include <map>

std::string rightAlignNumber(const int& input, const int& size = -1);
std::string rightAlignNumber(const double& input, const int& precision = 1);
std::string rightAlign(const std::string& input, const size_t& size);
std::string leftAlign(const std::string& input, const size_t& size);
std::string changeToDecimal(const std::string& input);

void swapLineColors(TCODConsole* con, const int& line);
void drawPageDots(TCODConsole* console, const int& X, const int& Y, const int& picked, const size_t& length); // if picked < 0 color all circles.
void invertLine(TCODConsole* console, const int& line);
int printEquipment(TCODConsole* console, Ship& ship, int line); // line to start drawing from.

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

// used for equipParts and printEquipment() to help draw the stats.
namespace vectornumbers 
{
    extern std::vector<int> zero;
    extern std::vector<int> one;
    extern std::vector<int> two;
    extern std::vector<int> three ;
    extern std::vector<int> four;
    extern std::vector<int> five;
    extern std::vector<std::vector<int>> indexes;
}
