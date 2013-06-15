#pragma once
#include <string>
#include <random>
#include <vector>

class NameFactory
  {
  public:
    NameFactory(const unsigned long int& seed);
    std::string getName();

  private:
    enum lists {LEADINGCON=  1, VOWELS = 2, TRAILINGCON = 3};
    std::mt19937 gen;
    std::string genSyllable();
    std::string getLetter(const int& listType);
    std::vector<std::string> leadingCon;
    std::vector<std::string> vowels;
    std::vector<std::string> trailingCon;
  };