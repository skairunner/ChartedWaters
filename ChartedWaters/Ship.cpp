#include "Ship.h"
#include <algorithm>

using namespace std;

Ship::Ship()
  : ducats(0)
  {

  }

string Ship::getName()
  {
  return shipName;
  }

void Ship::setName(const string& newName)
  {
  shipName = newName;
  }

bool Ship::addMoney(const int& amount)
  {
  if (amount + ducats < 0)
    return false;
  else
    ducats += amount;
  return true;
  }

void Ship::addItem(const Item& item, const int& numberOf, const int& averagePrice)
  {
  auto it = itemList.begin();
  for (; it < itemList.end(); it++)
    {
    if (item == *it)
      break;
    }
  if (it == itemList.end())
    {
    itemList.push_back(LedgerItem(item, numberOf, averagePrice));
    it = itemList.end() - 1;
    }
  }

int Ship::getMoney()
  {
  return ducats;
  }

int Ship::getNumberOfItems(const int& ID)
  {
  for (auto it = itemList.begin(); it < itemList.end(); it++)
    if (ID == it->getID())
      return it->howMany();
  return 0; // if it's not in the list.
  }

vector<string> Ship::returnListOfItems()
  {
  vector<string> returnVal;
  string buffer;
  for (auto it = itemList.begin(); it < itemList.end(); it++)
    {
    buffer.assign(to_string((long double)it->getID()));
    buffer += " " + it->getName();
    buffer = buffer + "    Purchased at " + to_string((long double)it->getAveragePrice()) + " ducats    x" + to_string((long double)it->howMany()) + "\n";
    returnVal.push_back(buffer);
    }
  if(returnVal.size() == 0)
    returnVal.push_back(string("Nothing to see here!"));

  return returnVal;
  }

bool Ship::removeItem(const int ItemID, const int& numberOf)
  {
  auto it = itemList.begin();
  for (; it < itemList.end(); it++)
    if (it->getID() == ItemID)
      break;
  if (it == itemList.end())
    return false;
  bool result = it->removeItems(numberOf);
  if (it->howMany() == 0)
    removeFromList(ItemID);

  return result;
  }

int Ship::getPurchasePriceOf(const int& ID)
  {
  auto it = itemList.begin();
  for (; it < itemList.end(); it++)
    if (it->getID() == ID)
      break;
  if (it == itemList.end())
    return false;
  return it->getAveragePrice();
  }

void Ship::removeFromList(const int& itemID)
  {

  }