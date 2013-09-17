#include "AI_ship.h"
#include <iostream>
#include <boost/random/uniform_int_distribution.hpp>

Town AIShip::nullTown = Town();

AIShip::AIShip()
  : state(STATE_PLOT), timer(0), date(0)
  {
  captain.ducats = 10000;
  gen.seed(rand());
  timer = random(0, 10);
  }

void AIShip::initItemDB(std::map<coord, Town>& world)
  {
  std::map<std::string, bool> masterlist;
  for each (auto city in cityList)
    {
    auto town = world.find(city.first);
    if (town != world.end())
      {
      updateDB(town->second);
      // Put all the items the merchant will ever encounter into a map, to automatically remove duplicates
      for each (AIEconomyItemTuple item in town->second.returnListOfItems_AI())
        {
        masterlist[item.itemID] = 1;
        }
      }
    }

  // Then, copy the list to the class one for faster access
  for (auto it = masterlist.begin(); it != masterlist.end(); it++)
    {
    masterItemList.push_back(it->first);
    }

  //Finally, run updateDB again for the sell prices.
  for each (auto city in cityList)
    {
    auto town = world.find(city.first);
    if (town != world.end())
      updateDB(town->second);
    }
  }

void AIShip::updateDB(Town& town)
  {
  bool home = false;
  if (town.getFactionID() == captain.faction)
    home = true;
  auto items = town.returnListOfItems_AI(home);
  for each (AIEconomyItemTuple item in items)
    {
    MemoryItem& ref = getItemFromDB(item.itemID);
    ref.cities[town.myPosition] = MemoryItem::City(item.BuyPrice, item.SellPrice, date);
    }

  for each (std::string ID in masterItemList)
    {
    MemoryItem& ref = getItemFromDB(ID);
    ref.cities[town.myPosition].sell = town.getSellPrice(ID);
    }
  }

MemoryItem& AIShip::getItemFromDB(std::string& ID)
  {
  auto iterator = ItemDB.find(ID);
  if (iterator == ItemDB.end())
    masterItemList.push_back(ID);
    
  return ItemDB[ID];
  }

void AIShip::think(Pather& pather, std::map<coord, Town>& cityList, Town& currentTown)
  {
  switch (state)
    {
  case STATE_WAIT:
    wait();
    break;
  case STATE_MOVE:
    move();
    break;
  case STATE_PLOT:
    plot(pather, cityList);
    break;
  case STATE_RESTOCK:
    restock(currentTown);
    break;
  case STATE_PURCHASE:
    purchase(currentTown);
    break;
  case STATE_ERROR:
    std::cout << "AI_ship.cpp error: error state\n";
    break;
  default:
    std::cout << "AI_ship.cpp error: undefined state\n";
    break;
    }
  }

void AIShip::wait()
  {
  timer--;
  if (timer <= 0)
    state = STATE_MOVE;
  }

void AIShip::startingWait()
  {
  timer--;
  if (timer <= 0)
    state = STATE_PLOT;
  }

void AIShip::DoMerchantLogic(Town& currentTown)
  {
  int MaxPotentialLoad = getMaxGoods() * 0.8f;

  std::map<coord, std::vector<std::pair<std::string, double>>> CitiesAndPrices;

  bool home = false;
  if (town.getFactionID() == captain.faction)
    home = true;

  auto itemsBuyable = currentTown.returnListOfItems_AI(home);

  
  };

int AIShip::random(const int& min, const int& max)
  {
  boost::random::uniform_int_distribution<> dist(min, max);
  return dist(gen);
  }

void AIShip::plot(Pather& pather, std::map<coord, Town>& cityList) // currently it chooses a random city out of known cities.
  {
  if (cityList.size() == 0)
    state = STATE_ERROR;
  int number = random(0, cityList.size());
  auto it = cityList.begin();
  for (int counter = 0; counter < number; counter++)
    it++;
  setPath(pather.path(position, it->first, getWaveResistance()));
  state = STATE_RESTOCK;
  }

void AIShip::restock(Town& currentTown)
  {
  if (captain.ducats < 3000)
    captain.ducats *= 52;

  double rationsNeeded = getEstimatedRationsNeeded();;
  rationsNeeded *= 1.1f;
  rationsNeeded += 5;
  rationsNeeded /= 10;
  int actualRations = (int)rationsNeeded * 10;
  actualRations -=  rations;
  if (actualRations > 0)
    currentTown.buyRations(*this, actualRations / 10);
  timer = random(0, 10);

  
  state = STATE_PURCHASE;
  }

void AIShip::purchase(Town& currentTown)
  {

  state = STATE_WAIT;
  }

void AIShip::move()
  {
  int counter = getMovementCounters();
  date++;
  for (auto num = 0; num < counter; num++)
    updatePos();
  step();
  if (path.size() <= 1)
    state = STATE_PLOT; // change to STATE_SELL later.
  }