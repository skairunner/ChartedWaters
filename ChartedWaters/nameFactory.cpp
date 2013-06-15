#include "nameFactory.h"

using namespace std;

NameFactory::NameFactory(const unsigned long int& seed)
  {
  gen.seed(seed);
  leadingCon.push_back(string("g"));
  leadingCon.push_back(string("n"));
  leadingCon.push_back(string("d"));
  leadingCon.push_back(string("r"));
  leadingCon.push_back(string("m"));
  leadingCon.push_back(string("b"));
  leadingCon.push_back(string("s"));
  leadingCon.push_back(string("j"));
  leadingCon.push_back(string("ch"));
  leadingCon.push_back(string("k"));
  leadingCon.push_back(string("t"));
  leadingCon.push_back(string("p"));
  leadingCon.push_back(string("h"));
  
  trailingCon.push_back(string("k"));
  trailingCon.push_back(string("n"));
  trailingCon.push_back(string("t"));
  trailingCon.push_back(string("l"));
  trailingCon.push_back(string("m"));
  trailingCon.push_back(string("p"));
  trailingCon.push_back(string("ng"));

  vowels.push_back(string("a"));
  //vowels.push_back(string("eo"));
  vowels.push_back(string("o"));
  vowels.push_back(string("u"));
  vowels.push_back(string("i"));
 // vowels.push_back(string("ae"));
  vowels.push_back(string("e"));
 // vowels.push_back(string("oe"));
 /* vowels.push_back(string("wi"));
  vowels.push_back(string("ya"));
  vowels.push_back(string("yeo"));
  vowels.push_back(string("yo"));
  vowels.push_back(string("yu"));
  vowels.push_back(string("yae"));
  vowels.push_back(string("ye"));
  vowels.push_back(string("wa"));
  vowels.push_back(string("wae"));
  vowels.push_back(string("wo"));
  vowels.push_back(string("we"));*/
  vowels.push_back(string("ui"));
  }

string NameFactory::getName()
  {
  std::uniform_int_distribution<> dist(2, 4);
  std::string result;
  int length = dist(gen);
  for (int counter = 0; counter < length; counter++)
    result.append(genSyllable());
  return result;
  };

string NameFactory::genSyllable()
  {
  string result;
  std::uniform_int_distribution<> dist(0, 9); //
  if (dist(gen) <= 8)
    result.append(getLetter(LEADINGCON));
  result.append(getLetter(VOWELS));
  if (dist(gen) <= 4)
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