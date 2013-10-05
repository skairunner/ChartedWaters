#include "nameFactory.h"
#include <fstream>

using namespace std;

NameFactory::NameFactory(const unsigned long int& seed)
  : minLength(2), maxLength(5), spaceChance(15), leadingChance(0.5f), vowelChance(1.0f), trailingChance(0.5f)
  {
  gen.seed(seed);
  setVars();
  }

string NameFactory::getName()
  {
  std::uniform_int_distribution<> dist(minLength, maxLength);
  std::string result;
  int length = dist(gen);
  for (int counter = 0; counter < length; counter++)
    {
    if (result.size() > 0 && result.back() == ' ')
        {
        string str = genSyllable();
        str.front() -= 32;
        result.append(str);
        }
    else if (counter != length-1) // can't be at the beginning or end of a word, nor after a hypen/space
      {
      int maxdist = spaceChance > maxLength +1 ? spaceChance : maxLength+1;
      std::uniform_int_distribution<> dist2(0, maxdist);
      if (dist2(gen) == 0) // The longer the word, the higher the chance of a space.
        {
        result.append(" ");
        }
      else 
        result.append(genSyllable());
      }
    else result.append(genSyllable());
    }
  
  return result;
  }

string NameFactory::genSyllable()
  {
  string result;
  std::uniform_real_distribution<> dist(0, 1); //
  if (dist(gen) < leadingChance)
    result.append(getLetter(LEADINGCON));
  if (dist(gen) < vowelChance)
    result.append(getLetter(VOWELS));
  if (dist(gen) < trailingChance)
    result.append(getLetter(TRAILINGCON));
  return result;
  }

string NameFactory::getLetter(const int& listType)
  {
  vector<string>* list;
  if (listType == LEADINGCON)
    list = &leadingCon;
  else if (listType == VOWELS)
    list = &vowels;
  else if (listType == TRAILINGCON)
    list = &trailingCon;

  int size = list->size();
  std::uniform_real_distribution<> dist(0, size-1);
  return list->at(dist(gen));
  }

void NameFactory::readFromFile(const std::string filename)
  {
  fstream file;
  file.open(filename.c_str(), fstream::in | fstream::app);
  vector<string> contents;
  while (!file.eof())
    {
    string buffer;
    getline(file, buffer);
    contents.push_back(buffer);
    }
  contents.pop_back(); // remove extra line

  int status = 0;
  for each (string letter in contents)
    {
    // scan file until first #
    if (status == 0)
      {
      if (letter.front() != '#')
        continue;
      else
        status++;
      }
    else if (status == 1)
      // Stuff everything into leading consonants until #
      {
      if (letter.front() != '#')
        leadingCon.push_back(letter);
      else
        status++;
      }
    else if (status == 2)
      // all into vowels
      {
      if (letter.front() != '#')
        vowels.push_back(letter);
      else
        status++;
      }
    else if (status == 3)
      //next into trailing cons
      {
      if (letter.front() != '#')
        trailingCon.push_back(letter);
      else
        status++;
      }
    // Anything beyond that is ignored.
    }
  }

void KoreanNameFactory::setVars()
  {
  minLength = 2;
  maxLength = 5;
  spaceChance = 15;
  leadingChance = 0.5f;
  vowelChance = 1.0f;
  trailingChance = 0.5f;
  }

KoreanNameFactory::KoreanNameFactory(const unsigned long int & seed)
  :NameFactory(seed)
  {
  readFromFile("resources/namegen_korean.lang");
  }

void EnglishNameFactory::setVars()
  {
  minLength = 2;
  maxLength = 4;
  leadingChance = 0.7f;
  trailingChance = 0.6f;
  vowelChance = 0.95f;
  spaceChance = 50;
  }

EnglishNameFactory::EnglishNameFactory(const unsigned long int& seed)
  : NameFactory(seed)
  {
  readFromFile("resources/namegen_english.lang");
  }

