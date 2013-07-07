#include "town.h"
#include <cstdlib>
#include <limits>
#pragma warning (disable : 4244)

using namespace std;

Town::Town()
  :taxRate(0.10f), isLuxury(false), isOther(false), isIndustrial(false), isAgri(false), faction(0)
  {
 

  }

Town::Town(const std::string& newName, const double& tax, const int& ffaction)
  : TownName(newName), taxRate(tax), faction(ffaction), isLuxury(false), isOther(false), isIndustrial(0), isAgri(0)
  {
  TownName[0] = toupper(TownName[0]);
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
    temporary = (it->getPrice()) * (1 + tax) * 10;
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
  itemlist.push_back(EconomyItem(ID, numberOf, numberOf * 1.1f));
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
  auto it = itemlist.begin();

  // implicit else, as if will return.
  for (; it < itemlist.end(); it++)
    if (it->ID == ID)
      {
  //    it->addItem(numberOf);
      break;
      }

  double currentTax = getTaxRate(hometown);

  if (it == itemlist.end())
    return twNO_SUCH_ITEM; // item doesn't exist!

  if (numberOf < 0)
    {
    numberOf = ship.getMoney() / (it->getPrice() * (1 + currentTax)); // FLOOR ( money / price ) = number of items buyable.
    numberOf = numberOf > it->howMany() ? it->howMany() : numberOf; // If the possible number of items buyable is more than the total.
    }
  int price = it->getPrice() * numberOf * (1 + currentTax);
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
  return twSUCCESS;
  }


int Town::getPriceOf(const std::string& ID)
  {
  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    {
    if (it->ID == ID)
      return it->getPrice();
    }
  // If it doesn't exist, return the base price ...
  return ItemDict.findBasePrice(ID);
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
  else if (isLuxury && category == string("Luxury items"))
    produceZone = 0.9f;

  // Distance bonus
  double distance = getDistanceFromNearestSource(ID);
  distance = distance > 200 ? 200 : distance;
  distance = distance < 0 ? 0 : distance;
  distanceMult = distance / 80 + 0.7f;

  return getPriceOf(ID) * distanceMult * produceZone * producedHere;
  }



int Town::sellItems(Ship& ship, const std::string& ID, int numberOf, bool hometown)
  {
  lastTransaction = 0;
  if (numberOf == 0)
    return 0;
  if (numberOf < 0)
    numberOf = ship.getNumberOfItems(ID);
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
  
  return twSUCCESS;
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

void Town::spawnItems()
  {
  for (auto it = spawnList.begin(); it < spawnList.end(); it++)
    {
    if ((*it)[0] == 'l') // If it's a luxury item.
      addItems(*it, 100);
    else addItems(*it, rand()%200 + 300);
    }
  }