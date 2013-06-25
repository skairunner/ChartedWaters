#include "town.h"
#pragma warning (disable : 4244)
using namespace std;

Town::Town()
  :taxRate(0.05f)
  {
  /// test items
  

  }

Town::Town(const std::string& newName, const double& tax, const int& ffaction)
  : TownName(newName), taxRate(tax), faction(ffaction)
  {
  TownName[0] = toupper(TownName[0]);
  }

string Town::getName()
  {
  return TownName;
  }

vector<EconomyItemTuple> Town::returnListOfItems()
  {
  vector<EconomyItemTuple> returnVal;

  for (auto it = itemlist.begin(); it < itemlist.end(); it++)
    {
    EconomyItemTuple buffer;
    int temporary;
    double secondtemp; // To change the raw floats into 'clean', 1-decimal point numbers.
    buffer.itemID = it->ID;
    buffer.ItemName = it->name;
    buffer.numberOfItems = to_string((long double)it->howMany());
    temporary = (it->getPrice()) * (1 + taxRate) * 10;
    secondtemp = temporary / 10.0f;
    buffer.BuyPrice = to_string((long double)secondtemp);
    temporary = it->getPrice() * (1 - taxRate) * 10;
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

int Town::buyItems(Ship& ship, const std::string& ID, int numberOf)
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

  if (it == itemlist.end())
    return twNO_SUCH_ITEM; // item doesn't exist!

  if (numberOf < 0)
    {
    numberOf = ship.getMoney() / (it->getPrice() * (1 + taxRate)); // FLOOR ( money / price ) = number of items buyable.
    numberOf = numberOf > it->howMany() ? it->howMany() : numberOf; // If the possible number of items buyable is more than the total.
    }
  int price = it->getPrice() * numberOf * (1 + taxRate);
  if (price > ship.getMoney())
    return twNOT_ENOUGH_MONEY; // don't have money to buy all that
  if (numberOf > it->howMany())
    return twNOT_ENOUGH_ITEMS; // not enough items to buy!

  /// Finally, actually buy them items.
  ship.addItem(Item(ID), numberOf, it->getPrice() * (1 + taxRate));
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
  // If it doesn't exist, return the price ...
  return -1;
  }

int Town::sellItems(Ship& ship, const std::string& ID, int numberOf)
  {
  lastTransaction = 0;
  if (numberOf == 0)
    return 0;
  if (numberOf < 0)
    numberOf = ship.getNumberOfItems(ID);
  unitPurchasePriceOfSell = ship.getPurchasePriceOf(ID);
  bool success = ship.removeItem(ID, numberOf);
  if (!success)
    return twNOT_ENOUGH_ITEMS;
  int earned = numberOf * getPriceOf(ID) * (1 - taxRate);
  addItems(ID, numberOf);
  ship.addMoney(earned);
  lastTransaction = earned;
  numberOfLastTransaction = numberOf;
  
  return twSUCCESS;
  }

double Town::getTaxRate()
  {
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
      addItems(*it, 50);
    else addItems(*it, 100);
    }
  }