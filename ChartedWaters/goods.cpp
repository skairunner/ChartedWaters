#include "goods.h"
#include <cmath>

ItemDictionary ItemDict;
using namespace std;

///////////////////////
//////////
//////////  ItemDictionary
//////////
///////////////////////

ItemDictionary::ItemDictionary()
  {
  ItemNames[IID_NULL] = std::string("Invalid item/null");
  ItemNames[IID_FRIED_CHICKEN] = std::string("Fried chicken");
  ItemNames[IID_COOKED_LLAMA_CHOPS] = std::string("Cooked llama chops");

  BasePrice[IID_NULL] = 0;
  BasePrice[IID_FRIED_CHICKEN] = 100;
  BasePrice[IID_COOKED_LLAMA_CHOPS] = 100;

  DecayRates[IID_NULL] = std::pair<double, double>(0, 0);
  DecayRates[IID_FRIED_CHICKEN] = std::pair<double, double>(0.1, 0.1);
  DecayRates[IID_COOKED_LLAMA_CHOPS] = std::pair<double, double>(0.1, 0.1);
  }

string ItemDictionary::findItemName(const int& ID)
  {
  auto it = ItemNames.find(ID);
  if (it == ItemNames.end())
    return ItemNames[0];
  else
    return it->second;
  }

double ItemDictionary::findBasePrice(const int& ID)
  {
  auto it = BasePrice.find(ID);
  if (it == BasePrice.end())
    return BasePrice[0];
  else
    return it->second;
  }

pair<double, double> ItemDictionary::findDecayRates(const int& ID)
  {
  auto it = DecayRates.find(ID);
  if (it == DecayRates.end())
    return DecayRates[0];
  else
    return it->second;
  }

///////////////////////
//////////
//////////  Item
//////////
///////////////////////

Item::Item()
  : itemName(string("Null")), basePrice(0), decayRatePositive(0), decayRateNegative(0)
  {
  }

Item::Item(const int& newID)
  :ID(newID)
  {
  itemName = ItemDict.findItemName(newID);
  basePrice = ItemDict.findBasePrice(newID);
  pair<double, double> buffer = ItemDict.findDecayRates(newID);
  decayRatePositive = buffer.first;
  decayRateNegative = buffer.second;
  }


bool Item::operator==(const Item& right) const
  {
  if (right.getID() == ID)
    return true;
  else return false;
  }

bool Item::operator<(const Item& right) const
  {
  if (itemName < right.getName())
    return true;
  else return false;
  }

int Item::getID() const
  {
  return ID;
  }

string Item::getName() const
  {
  return itemName;
  }

double Item::getBasePrice() const
  {
  return basePrice;
  }



///////////////////////
//////////
//////////  EconomyItem
//////////
///////////////////////

EconomyItem::EconomyItem(const int& ID, const int& newSupply, const int& newDemand)
  : Item(ID), supply(newSupply), demand(newDemand)
  {

  }

void EconomyItem::decayDemand()
  {
  if (demand > supply)
    demand *= 1 - decayRatePositive;
  else if (demand < supply)
    demand *= 1 + decayRateNegative;
  }

int EconomyItem::getPrice()
  {
  const double VERTICAL_SHIFT = 0.25f;
  const double HORIZONTAL_SHIFT = 500.0f;
  const double HORIZONTAL_SCALE = 0.01f;
  const double PRICE_RANGE = 1.5f;
  double adjustedDemand = (1500 + 500 * 0.5 * log(supply / (double)demand)) / 3.0f;
  double multiplier = VERTICAL_SHIFT + PRICE_RANGE / (1 + exp(HORIZONTAL_SCALE * (adjustedDemand - HORIZONTAL_SHIFT)));
  return multiplier * basePrice;
  }

int EconomyItem::howMany()
  {
  return supply;
  }

void EconomyItem::addItem(const int& howMany)
  {
  supply += howMany;
  }

void EconomyItem::setDemand(const int& equi)
  {
  demand = equi;
  }

///////////////////////
//////////
//////////  LedgerItem
//////////
///////////////////////

LedgerItem::LedgerItem(const int& ID)
  :Item(ID), totalItems(0)
  {

  }

LedgerItem::LedgerItem(const Item& item, const int& howMany, const int& averagePrice)
  :Item(item), totalItems(howMany)
  {
  ledger.push_back(pair<int, int>(howMany, averagePrice));
  }

void LedgerItem::addItem(const int& howMany, const int& averagePrice)
  {
  ledger.push_back(pair<int, int>(howMany, averagePrice));
  totalItems += howMany;
  }

bool LedgerItem::removeItems(const int& howMany)
  {
  if (totalItems < howMany)
    return false;
  int counter = howMany;
  for (auto it = ledger.rbegin(); it < ledger.rend(); it++)
    {
    if (counter - it->first <= 0) // If the contents of it can handle the need.
      {
      it->first -= counter;
      break;
      }
    else
      {
      counter -= it->first;
      it->first = 0;
      }
    }

  // Finally, pop the back members that are 0.
  while (ledger.size() != 0 && ledger.at(ledger.size()-1).first == 0)
    ledger.pop_back();
  totalItems -= howMany;
  return true;
  }

int LedgerItem::getAveragePrice()
  {
  int total = 0;
  for (auto it = ledger.begin(); it < ledger.end(); it++)
    {
    total += it->first * it->second;
    }
  return total / (int)totalItems;
  }

void LedgerItem::countItems()
  {
  int total = 0;
  for (auto it = ledger.begin(); it < ledger.end(); it++)
    {
    total += it->first;
    }
  totalItems = total;
  }

int LedgerItem::howMany()
  {
  return totalItems;
  }