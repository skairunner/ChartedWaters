#pragma once
#include <string>
#include <random>
#include <vector>

class NameFactory
  {
  public:
    NameFactory(const unsigned long int& seed);
    virtual std::string getName();
    

  protected:
    virtual void setVars() {}
    virtual void abstractify() = 0;

    enum lists {LEADINGCON=  1, VOWELS = 2, TRAILINGCON = 3};
    std::mt19937 gen;
    virtual std::string genSyllable();
    virtual std::string getLetter(const int& listType);
    void readFromFile(const std::string filename);
    std::vector<std::string> leadingCon;
    std::vector<std::string> vowels;
    std::vector<std::string> trailingCon;

    int minLength, maxLength;
    int spaceChance;
    double leadingChance, vowelChance, trailingChance;
  };

class KoreanNameFactory : public NameFactory
  {
  public: 
    KoreanNameFactory(const unsigned long int& seed);
    void setVars();
    void abstractify(){}
  };

class EnglishNameFactory : public NameFactory
  {
  public:
    EnglishNameFactory(const unsigned long int & seed);
    void setVars();
    void abstractify(){}
   // std::string getName();
  };

extern KoreanNameFactory nameFactory;