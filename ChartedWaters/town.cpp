#include "town.h"
#include <cstdlib>
#include <limits>
#include <iostream>
#pragma warning (disable : 4244)

using namespace std;

Town::Town()
  :taxRate(0.10f), isLuxury(false), isOther(false), isIndustrial(false), isAgri(false), faction(0)
  {
  spawncounter = rand()%14 + 1;

  }

Town::Town(const std::string& newName, const double& tax, const int& ffaction)
  : TownName(newName), taxRate(tax), faction(ffaction), isLuxury(false), isOther(false), isIndustrial(0), isAgri(0)
  {
  TownName[0] = toupper(TownName[0]);
  spawncounter = rand()%14 + 1;
  }

string Town::getName()
  {
  return TownName;
  }

vector<EconomyItemTuple> Town::returnListOfItems(bool isHometown)
  {
  vector<EconomyItemTuple> returnVal;
  double tax = getTaxRate(isHometown);
  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    {
    EconomyItemTuple buffer;
    int temporary;
    double secondtemp; // To change the raw floats into 'clean', 1-decimal point numbers.
    buffer.itemID = it->ID;
    buffer.ItemName = it->name;

    buffer.numberOfItems = to_string((long double)it->howMany());
    temporary = getBuyPrice(it->ID) * (1 + tax) * 10;
    secondtemp = temporary / 10.0f;

    buffer.BuyPrice = to_string((long double)secondtemp);
    temporary = getSellPrice(it->ID) * 10 * (1 - tax);
    secondtemp = temporary / 10.0f;
    buffer.SellPrice = to_string((long double)secondtemp);
    int percentage = (double)it->getPrice()/it->basePrice * 100;
    buffer.percentageOfBasePrice = to_string((long double)percentage) + string("%%");

    returnVal.push_back(buffer);
    }

  return returnVal;
  }

void Town::addItems(const std::string& ID, const int& numberOf)
  {
  auto it = itemlist.begin();
  for (; it < itemlist.end(); it++)
    if (it->ID == ID)
      {
      it->addItem(numberOf);
      return;
      }
  // since the item doesn't exist
  itemlist.push_back(EconomyItem(ID, numberOf, demandList[ID]));
  demandList.erase(ID);
  }

vector<EconomyItem>::iterator Town::getItemIterator(const string& ID)
  {
  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    {
    if (it->ID == ID)
      return it;
    }
  return itemlist.end();
  }

int Town::buyItems(Ship& ship, const std::string& ID, int numberOf, bool hometown)
  {
  if (numberOf == 0)
    return 0;
  lastTransaction = 0;
  lastTransactionItemID.clear();
  auto it = itemlist.begin();

  // implicit else, as if will return.
  for (; it < itemlist.end(); it++)
    if (it->ID == ID)
      {
  //    it->addItem(numberOf);
      break;
      }

  if (it == itemlist.end())
    return twNO_SUCH_ITEM; // item doesn't exist!

  double currentTax = getTaxRate(hometown);
  if (numberOf < 0)
    {
    numberOf = ship.getMoney() / (getBuyPrice(it->ID) * (1 + currentTax)); // FLOOR ( money / price ) = number of items buyable.
    numberOf = numberOf > it->howMany() ? it->howMany() : numberOf; // If the possible number of items buyable is more than the total.
    }
  int price = getBuyPrice(it->ID) * numberOf * (1 + currentTax);
  if (price > ship.getMoney())
    return twNOT_ENOUGH_MONEY; // don't have money to buy all that
  if (numberOf > it->howMany())
    return twNOT_ENOUGH_ITEMS; // not enough items to buy!

  /// Finally, actually buy them items.
  ship.addItem(Item(ID), numberOf, it->getPrice() * (1 + currentTax));
  ship.addMoney(-price);
  it->addItem(-numberOf);
  it->addDemand(numberOf);
  lastTransaction = price;
  numberOfLastTransaction = numberOf;
  unitPurchasePriceOfSell = 0;
  lastTransactionItemID = ID;
  return twSUCCESS;
  }

int Town::sellItems(Ship& ship, const std::string& ID, int numberOf, bool hometown)
  {
  lastTransaction = 0;
  if (numberOf == 0)
    return 0;
  if (numberOf < 0)
    numberOf = ship.getNumberOfItems(ID);
  lastTransactionItemID.clear();
  // Set tax rate to 0 if it's hometown.

  double currentTax = getTaxRate(hometown);
  unitPurchasePriceOfSell = ship.getPurchasePriceOf(ID);
  bool success = ship.removeItem(ID, numberOf);
  if (!success)
    return twNOT_ENOUGH_ITEMS;
  int earned = numberOf * getSellPrice(ID) * (1 - currentTax);
  addItems(ID, numberOf);
  ship.addMoney(earned);
  lastTransaction = earned;
  numberOfLastTransaction = numberOf;
  lastTransactionItemID = ID;
  
  return twSUCCESS;
  }


double Town::getDistanceFromNearestSource(const std::string& ID)
  {
  auto list = ItemDict.getCitiesForItem(ID);
  if (list.size() == 0)
    return 99999; // max :D
  double distance = numeric_limits<double>::max(); // Squared at first.
  for (auto it = list.begin()+1; it < list.end(); it++)
    {
    double potentialDistance = pow((double)myPosition.first - it->first, 2) + pow((double)myPosition.second - it->second, 2);
    if (potentialDistance <= distance)
      distance = potentialDistance;
    }

  return sqrt(distance);
  }

int Town::getPriceOf(const std::string& ID)
  {
  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    {
    if (it->ID == ID)
      return it->getPrice();
    }
  // If it doesn't exist, return the base price influenced by demand
  EconomyItem temp(ID, 1, 1 + demandList[ID]);
  return temp.getPrice();
  } 

// Must consider: (1) if it's produced in that city (2) supply vs demand for said item (3) if the city is in a 'zone' (4) closest city that produces said good
double Town::getSellPrice(const std::string& ID)
  {
  double producedHere = 1.0f;
  double produceZone = 1.0f;
  double distanceMult = 1.0f;

  // Produced here -> half
  auto spawnlistIt = spawnList.begin();
  while (spawnlistIt < spawnList.end() && *spawnlistIt != ID)
    spawnlistIt++;
  if (spawnlistIt != spawnList.end()) 
    producedHere = 0.5f;
  
  // Is in a zone -> 90%
  string category = ItemDict.findItemCategory(ID);

  if (isAgri && category == string("Food"))
    produceZone = 0.9f;
  else if (isIndustrial && category == string("Raw materials"))
    produceZone = 0.9f;
  else if (isOther && category == string("Other"))
    produceZone = 0.9f;
  else if (isLuxury && category == string("Luxury"))
    produceZone = 0.9f;

  // Distance bonus
  double distance = getDistanceFromNearestSource(ID);
  distance = distance > 200 ? 200 : distance;
  distance = distance < 0 ? 0 : distance;
  distanceMult = distance / 80 + 0.7f;

  return getPriceOf(ID) * distanceMult * produceZone * producedHere;
  }

double Town::getBuyPrice(const std::string& ID)
  {
  double produceZone = 1.0f;
  double distanceMult = 1.0f;
  
  // Is in a zone -> 90%
  string category = ItemDict.findItemCategory(ID);

  if (isAgri && category == string("Food"))
    produceZone = 0.9f;
  else if (isIndustrial && category == string("Raw materials"))
    produceZone = 0.9f;
  else if (isOther && category == string("Other"))
    produceZone = 0.9f;
  else if (isLuxury && category == string("Luxury"))
    produceZone = 0.9f;

  // Distance bonus
  
  double distance = getDistanceFromNearestSource(ID);
  if (getNumberOf(ID) == -1)
    distanceMult = 1;
  else
    {
    distance = distance > 200 ? 200 : distance;
    distance = distance < 0 ? 0 : distance;
    distanceMult = distance / 80 + 0.7f;
    }

  return getPriceOf(ID) * distanceMult * produceZone;
  }

double Town::getTaxRate(bool hometown)
  {
  if (hometown)
    return 0;
  return taxRate;
  }

void Town::setTaxRate(const double& newTaxRate)
  {
  taxRate = newTaxRate;
  }

int Town::getFactionID()
  {
  return faction;
  }

int Town::getNumberOf(const std::string& itemID)
  {
  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    if (it->ID == itemID)
      {
      return it->howMany();
      }
  return -1;
  }

void Town::addDemandToItem(const std::string& itemID, const int& add)
  {
  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    {
    if (it->ID == itemID)
      {
      it->addDemand(add);
      }
    }
  // since by this point we haven't found it in the list.
  demandList[itemID] += add;
  }

void Town::spawnItems()
  {
  for (auto it = spawnList.begin(); it < spawnList.end(); it++)
    {
    if ((*it)[0] == 'l') // If it's a luxury item.
      addItems(*it, population);
    else addItems(*it, rand()%200 + population * 2);
    }
  }

void Town::step()
  {
  spawncounter++;
  for (auto it = spawnList.begin(); it < spawnList.end(); it++)
    {
    if ((*it)[0] == 'l') // If it's a luxury item.
      {if (getNumberOf(*it) < 8192)
      addItems(*it, population * 0.05);}
    else 
      if (getNumberOf(*it) < 8192)
        addItems(*it, population * 0.1);
    }
  // Remove items by demand. 
  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    {
    int toRemove = it->getDemand() * 0.1;
    it->addItem(toRemove * -1);
    it->addDemand(toRemove * -0.5f);
    }
  vector<string> toRemove;
  for (auto it = demandList.begin(); it != demandList.end(); it++)
    {
    it->second *= 0.9f;
    if (it->second <= 0)
      toRemove.push_back(it->first);
    }
  for (auto it = toRemove.begin(); it < toRemove.end(); it++)
    demandList.erase(*it);

  // Randomly spawn demand
  if (spawncounter > 14)
    {
    spawncounter = 0;
    map<string, vector<string>>& itemlist = ItemDict.itemsPerCategory;
    for (int counter = 0; counter < 5; counter++) // make 5 items in demand.
      {
      int first = rand()%(itemlist.size()-1);
      auto outerIt = itemlist.begin();
      for (int counter = 0; counter < first; counter++)
        outerIt++;
      int position = rand()%(outerIt->second.size());
      string ID = outerIt->second.at(position);
      auto item = ItemDict.getItemTemplate(ID);
      auto category = itemlist.find(item.category);

      addDemandToItem(item.ID, 400);
      //cout << "Spawned demand for " << item.ID << " in city " << TownName << endl;
      for (auto it = category->second.begin(); it < category->second.end(); it++)
        {
        
        addDemandToItem(*it, 100);
        if (ItemDict.findItemType(*it) == item.type)
          addDemandToItem(*it, 200);
        }      
      }
    }
  }