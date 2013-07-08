#include "Ship.h"
#include <algorithm>

using namespace std;

Ship::Ship()
  : ducats(1000), storage(0), maxstorage(212), baseSpeed(6), character(127), waveResistance(6), faction(0)
  {

  }

string Ship::getName()
  {
  return shipName;
  }

void Ship::setName(const string& newName)
  {
  shipName = newName;
  shipName[0] = toupper(shipName[0]);
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

  storage += numberOf;
  }

int Ship::getMoney()
  {
  return ducats;
  }

int Ship::getNumberOfItems(const std::string& ID)
  {
  for (auto it = itemList.begin(); it < itemList.end(); it++)
    if (ID == it->ID)
      return it->howMany();
  return 0; // if it's not in the list.
  }

vector<LedgerItemTuple> Ship::returnListOfItems()
  {
  vector<LedgerItemTuple> returnVal;
  LedgerItemTuple buffer;
  for (auto it = itemList.begin(); it < itemList.end(); it++)
    {
    buffer.itemID = it->ID;
    buffer.ItemName = it->name;
    buffer.averagePurchasePrice = to_string((long double)it->getAveragePrice());
    buffer.numberOfItems = to_string((long double)it->howMany());
    returnVal.push_back(buffer);
    }
 /* if(returnVal.size() == 0)
    returnVal.push_back(string("Nothing to see here!"));*/

  return returnVal;
  }

bool Ship::removeItem(const std::string& ItemID, const int& numberOf)
  {
  auto it = itemList.begin();
  for (; it < itemList.end(); it++)
    if (it->ID == ItemID)
      break;
  if (it == itemList.end())
    return false;
  bool result = it->removeItems(numberOf);
  if (it->howMany() == 0)
    removeFromList(ItemID);
  if (result) // if it succeeded
    storage -= numberOf;
  return result;
  }

int Ship::getPurchasePriceOf(const std::string& ID)
  {
  auto it = itemList.begin();
  for (; it < itemList.end(); it++)
    if (it->ID == ID)
      break;
  if (it == itemList.end())
    return false;
  return it->getAveragePrice();
  }

bool Ship::removeFromList(const std::string& itemID)
  {
  int counter = 0;
  for (; counter < itemList.size(); counter++)
    if (itemID == itemList[counter].ID)
      break;
  if (counter == itemList.size())
    return false;
  for (int it = counter + 1; it < itemList.size(); it++)
    itemList[it-1] = itemList[it];
  itemList.pop_back();
  return true;
  }

int Ship::getTotalStorageUsed()
  {
  return storage;
  }

int Ship::getMaxStorage()
  {
  return maxstorage;
  }

void Ship::setPosition(const std::pair<int, int>& newPos)
  {
  position = newPos;
  }

int Ship::getSpeed()
  {
  double mult;
  mult = (1-(double)getTotalStorageUsed()/getMaxStorage()) + 0.5; // if total == max, 0.5f. 
  mult = mult > 1 ? 1 : mult;
  return mult * baseSpeed;
  }

pair<int, int> Ship::getPosition()
  {
  return position;
  }

void Ship::setPath(const std::vector<std::pair<int, int>>& ppath)
  {
  if (ppath.size() > 0)
    {
    path.clear();
    path.reserve(sizeof(pair<int, int>) * ppath.size());

    for (auto it = ppath.rbegin(); it < ppath.rend()-1; it++) // Skip the last, as it's 0,0.
      {
      path.push_back(*it);
      }
    }
  }

void Ship::updatePos()
  {
  if (path.size() > 0){
    setPosition(path.back());
    path.pop_back();
    }
  }