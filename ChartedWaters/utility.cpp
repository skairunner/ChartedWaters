#include "utility.h"
#include <regex>
#include <iostream>

using namespace std;

string rightAlignNumber(const int& input, const int& size)
  {
  char price_cstr[50];
  _snprintf(price_cstr, sizeof(price_cstr), "%d", input);
  string buffer(price_cstr);
  if (size == -1) // default
    return buffer;

  string returnval;
  if (buffer.size() < size)
    for (int counter = 0; counter < size - buffer.size(); counter++)
      returnval += " ";
  if (buffer.size() > size)
    for (int counter = 0; counter < size; counter++)
      returnval += "x";
  else returnval += buffer.substr(0, size);
  return returnval;
  }

std::string rightAlign(const string& input, const int& size)
  {
  string returnval;
  if(input.size() > size)
    {
    for (int counter = 0; counter < size; counter++)
      returnval += "x";
    return returnval;
    }
  if(input.size() < size)
    for (int counter = 0; counter < size - input.size(); counter++)
      returnval += " ";
  returnval += input.substr(0, size);
  return returnval;
  }

std::string leftAlign(const std::string& input, const int& size)
  {
  string returnval;
  returnval += input.substr(0, size);
  if(input.size() < size)
    for (int counter = 0; counter < size - input.size(); counter++)
      returnval += " ";
  return returnval;
  }

std::string changeToDecimal(const std::string& input)
  {
  regex rgx;
  rgx.assign("\\.\\d");
  if(!regex_search(input, rgx)) // If the regex wasn't matched -> if there is no decimal point in it.
    return input + string(".0");
  else
    return input;
  }

int stringToDecimal(const std::string& input)
  {
  int result = -555555;
  try {
    result = stoi(input);
    
    }
  catch (invalid_argument e)
    {
    cout << "<debug> Invalid argument in utility.cpp: " << e.what() << endl;
    }

  return result;
  }

template<typename T> 
void removeInPlace(const int& i, std::vector<T>& v)
{
	for (int it = i; it < v.size()-1; it++)
	{
		v[it] = v[it + 1];
	}
	v.pop_back();
}