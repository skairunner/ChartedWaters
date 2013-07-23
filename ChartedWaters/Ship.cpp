#include "Ship.h"
#include <algorithm>
#include <cmath>

using namespace std;

Ship::Ship()
  : storage(0), character(127), waveResistance(6), rations(50), sailors(5), fatigue(0), training(500), durability(0)
  {
  captain.faction = 0;
  captain.ducats = 1000;
  maxstorage = 212;
  turning = 0;
  }

Ship::Ship(const ShipPrototype& prototype)
  : storage(0), character(127), rations(50), sailors(5), fatigue(0), training(500), durability(0)
  {
  captain.faction = 0;
  captain.ducats = 1000;
  changeShip(prototype);
  }

void Ship::changeShip(const ShipPrototype& prototype)
  {
  typeID = prototype.typeID;
  typeName = prototype.typeName;
  specialization = prototype.specialization;
  desc = prototype.desc;
  size = prototype.size;

  if (size == std::string("small"))
    character = 9;
  else if (size == std::string("medium"))
    character = 127;
  else if (size == std::string("large"))
    character = 30;
  else character = 63;

  price = prototype.price;
  maxstorage = prototype.maxstorage;
  maxcargo = prototype.maxcargo;
  maxsailors = prototype.maxsailors;
  maxcannons = prototype.maxcannons;
  minimumsailors = prototype.minimumsailors;
  lateen = prototype.lateen;
  square = prototype.square;
  waveResistance = prototype.waveResistance;
  turning = prototype.turning;
  baseArmor = prototype.baseArmor;
  maxDurability = prototype.maxDurability;
  durability = maxDurability;
  }

string Ship::getName()
  {
  return shipName;
  }

string Ship::getType()
  {
  return typeName;
  }

void Ship::setName(const string& newName)
  {
  shipName = newName;
  shipName[0] = toupper(shipName[0]);
  }

bool Ship::addMoney(const int& amount)
  {
  if (amount + captain.ducats < 0)
    return false;
  else
    captain.ducats += amount;
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
  else
    {
    it->addItem(numberOf, averagePrice);
    }

  storage += numberOf;
  }

int Ship::getMoney()
  {
  return captain.ducats;
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

int Ship::getTotalGoods()
  {
  return storage + ceil((float)rations/10.0f);
  }

int Ship::getMaxGoods()
  {
  return maxcargo;
  }

void Ship::setPosition(const std::pair<int, int>& newPos)
  {
  position = newPos;
  }

double Ship::getArmorSlowing()
  {
  double armorSlowing = 0;
  for (auto it = armorList.begin(); it != armorList.end(); it++)
    armorSlowing += it->second.speed;
  return armorSlowing;
  }

double Ship::getSpeed()
  {
  double weight, crew, starved, trained;
  crew = starved = trained = 1;
  weight = (1-(double)getTotalStorageUsed()/getMaxStorage()) + 0.5; // if total == max, 0.5f. 
  weight = weight > 1 ? 1 : weight;
  double basespeed = baseSpeed_d(getLateen(), getSquare()) + getArmorSlowing();

  if (sailors < getMinSailors())
    {
    crew = pow((double)sailors / getMinSailors(), (double)1.0f/3.0f);
    crew = crew > 1 ? 1 : crew;
    }

  if (starving)
    starved = 0.8;
  trained = 1 + training / 1000.0f;

  double result = weight * crew * basespeed * starved;

  if (result < 1)
    return 1;
  else return result;
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

void Ship::step()
  {
  starving = false;
  sailorsDied = 0;
  fatigue++;
  training += 5;
  rations -=  sailors;
  if (rations < 0)
    {
    rations = 0;
    starving = true;
    }
  if (starving)
    fatigue+=100;
  if (fatigue >= 1000)
    {
    fatigue = 1000;
    sailors -= sailors * 0.1 + 1.5;
    sailors = sailors < 0 ? 0 : sailors;
    }
  }

int Ship::getShipPrice()
  {
  return price;
  }

int Ship::getMaxCannons()
  {
  return maxcannons;
  }

int Ship::getTotalStorageUsed()
  {
  return storage + ceil((float)rations/10.0f);
  }

int Ship::getMaxStorage()
  {
  return maxstorage;
  }

int Ship::getMinSailors()
  {
  return minimumsailors;
  }

int Ship::getMaxSailors()
  {
  return maxsailors;
  }

int Ship::getLateen()
  {
  int addedLateen = 0;
  for (auto it = sailList.begin(); it != sailList.end(); it++)
    addedLateen += it->second.lateen;
  return lateen + addedLateen;
  }

int Ship::getSquare()
  {
  int addedSquare = 0;
  for (auto it = sailList.begin(); it != sailList.end(); it++)
    addedSquare += it->second.square;
  return square + addedSquare;
  }

int Ship::getArmor()
  {
  int addedArmor = 0;
  for (auto it = armorList.begin(); it != armorList.end(); it++)
    addedArmor += it->second.armor;
  return baseArmor + addedArmor;
  }

int Ship::getWaveResistance()
  {
  return waveResistance;
  }

int Ship::getMaxDurability()
  {
  return maxDurability;
  }

std::string Ship::getDescription()
  {
  return desc;
  }

int Ship::getTurning()
  {
  int addedTurning = 0;
  for (auto it = sailList.begin(); it != sailList.end(); it++)
    addedTurning += it->second.turning;
  if (addedTurning + turning < 1)
    return 1;
  else return addedTurning + turning;
  }

string Ship::getSize()
  {
  return size;
  }

int Ship::getBaseSpeed()
  {
  return baseSpeed(lateen, square);
  }

int Ship::addSail(int pos, ShipSails& sail)
  {
  auto it = sailList.find(pos);
  if (it == sailList.end())
    {
    sailList[pos] = sail;
    return shipSUCCESS;
    }
  // since it already exists...
  ShipSails temp = it->second;
  sailList[pos] = sail;
  sail = temp;
  return shipSWAPPED;
  }

int Ship::addArmor(int pos, ShipArmor& armor)
  {
  auto it = armorList.find(pos);
  if (it == armorList.end())
    {
    armorList[pos] = armor;
    return shipSUCCESS;
    }
  // since it already exists...
  ShipArmor temp = it->second;
  armorList[pos] = armor;
  armor = temp;
  return shipSWAPPED;
  }

int Ship::removeSail(int pos, ShipSails& sail)
  {
  auto it = sailList.find(pos);
  if (it == sailList.end())
    {
    return 0;
    }
  sail = it->second;
  sailList.erase(it);
  return shipSUCCESS;
  }

int Ship::getEstimatedRationsNeeded()
  {
  int daysNeeded = ceil(path.size() / floor((double)getSpeed()));
  return daysNeeded * sailors;
  }

int Ship::getETA()
  {
  return ceil(path.size() / floor((double)getSpeed()));;
  }