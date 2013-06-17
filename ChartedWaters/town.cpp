#include "town.h"
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
    buffer.itemID = it->getID();
    buffer.ItemName = it->getName();
    buffer.numberOfItems = to_string((long double)it->howMany());
    buffer.BuyPrice = string("~") + to_string((long double)(it->getPrice() * (1 + taxRate)));
    buffer.SellPrice = string("~") + to_string((long double)(it->getPrice() * (1 + taxRate)));

    returnVal.push_back(buffer);
    }

  return returnVal;
  }

void Town::addItems(const std::string& ID, const int& numberOf)
  {
  auto it = itemlist.begin();
  for (; it < itemlist.end(); it++)
    if (it->getID() == ID)
      {
      it->addItem(numberOf);
      return;
      }
  // since the item doesn't exist
  itemlist.push_back(EconomyItem(ID, numberOf, numberOf));
  }

int Town::buyItems(Ship& ship, const std::string& ID, int numberOf)
  {
  lastTransaction = 0;
  auto it = itemlist.begin();

  // implicit else, as if will return.
  for (; it < itemlist.end(); it++)
    if (it->getID() == ID)
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
    if (it->getID() == ID)
      return it->getPrice();
    }
  return -1; // since it does not exist.
  }

int Town::sellItems(Ship& ship, const std::string& ID, int numberOf)
  {
  lastTransaction = 0;

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