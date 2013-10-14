#include "AI_ship.h"
#include <iostream>
#include <boost/random/uniform_int_distribution.hpp>
#include <tuple>

#pragma warning(disable: 4244)


Town AIShip::nullTown = Town();

AIShip::AIShip()
  : state(STATE_PLOT), timer(0), date(0),
  ratio_mostProfit(0.5f), ratio_secondProfit(0.4f), ratio_hedge(0.1f)
  {
  captain.ducats = 10000;
  gen.seed(rand());
  timer = random(0, 10);
  }

void AIShip::battle(AIShip& ship1, AIShip& ship2)
  {
  int distance = rand()%500 + 300;
  bool fightOrFlight1, fightOrFlight2;
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
      initUpdateDB(town->second);
    }
  }

void AIShip::initUpdateDB(Town& town)
  {
  bool home = false;
  if (town.getFactionID() == captain.faction)
    home = true;
  auto items = town.returnListOfItems_AI(home);
  for each (AIEconomyItemTuple item in items)
    {
    MemoryItem& ref = getItemFromDB(item.itemID);
    ref.ID = item.itemID;
    ref.category = ItemDict.findItemCategory(ref.ID);
    ref.type = ItemDict.findItemType(ref.ID);
    
    ref.cities[town.myPosition] = MemoryItem::City(item.BuyPrice, item.SellPrice, date);
    }

  for each (std::string ID in masterItemList)
    {
    MemoryItem& ref = getItemFromDB(ID);
    ref.cities[town.myPosition].sell = town.getSellPrice(ID);
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
  case STATE_PURCHASE:
    purchase(currentTown);
    break;
  case STATE_SELL:
    sell(currentTown);
    break;
  case STATE_PLOT:
    plotRandom(pather);
    break;
  case STATE_RESTOCK:
    restock(currentTown);
    break;
  case STATE_MERCHANTLOGIC:
    DoMerchantLogic(currentTown, pather);
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

void AIShip::DoMerchantLogic(Town& currentTown, Pather& pather)
  {
  int MaxPotentialLoad = getMaxGoods() * 0.8f;
  int MaxMoney = captain.ducats * 0.8f;

  //std::map<coord, std::vector<std::pair<std::string, double>>> CitiesAndPrices;
  std::vector<std::tuple<coord, std::string, double>> CitiesAndProfit;

  bool home = false;
  if (currentTown.getFactionID() == captain.faction)
    home = true;

  auto itemsBuyable = currentTown.returnListOfItems_AI(home);

  for each (AIEconomyItemTuple item in itemsBuyable)
    {
    for each (auto citycoord in cityList)
      {
      auto memorizedItem = getItemFromDB(item.itemID);
      double profit = memorizedItem.cities[citycoord.first].sell - item.BuyPrice;
      int numberOf = MaxPotentialLoad;
      if (item.BuyPrice * MaxPotentialLoad > MaxMoney)
        numberOf = MaxMoney / item.BuyPrice;
      profit *= numberOf;

      CitiesAndProfit.push_back(std::tuple<coord, std::string, double>(citycoord.first, item.itemID, profit));
      }
    }

  // Find city with best profit.
  double bestProfit = 0;
  int bestProfitPosition = -1;
  for (int counter = 0; counter < CitiesAndProfit.size(); counter++)
    {
    double tentativeProfit = std::get<2>(CitiesAndProfit[counter]);
    if (tentativeProfit > bestProfit)
      {
      bestProfitPosition = counter;
      bestProfit = tentativeProfit;
      }
    }

  if (bestProfitPosition >= 0)
    // Let's go to that city!
    plot(pather, std::get<0>(CitiesAndProfit[bestProfitPosition]));
  else
    {
    plotRandom(pather);
    restock(currentTown);
    return;
    }


  // now extract the item to buy from memory
  coord destination = std::get<0>(CitiesAndProfit[bestProfitPosition]);
  int numberToBuy = MaxPotentialLoad;
  std::string profitItemID = std::get<1>(CitiesAndProfit[bestProfitPosition]);
  AIEconomyItemTuple profitItemTuple;
  
  for (auto it = itemsBuyable.begin(); it < itemsBuyable.end(); it++)
    if (it->itemID == profitItemID)
      profitItemTuple = *it;
  
  // redo budget
  if (profitItemTuple.BuyPrice * MaxPotentialLoad > MaxMoney)
        numberToBuy = MaxMoney / profitItemTuple.BuyPrice;
  MaxMoney -= numberToBuy * profitItemTuple.BuyPrice; 

  


  
  
  


  // Restock first.
  restock(currentTown);
  numberToBuy = (getMaxGoods() - getTotalGoods()) * 0.8f;
  if (numberToBuy != 0)
    {
    if (profitItemTuple.BuyPrice * numberToBuy > captain.ducats) // fill 80% of the left-over space with the item
      numberToBuy = captain.ducats / profitItemTuple.BuyPrice * 0.8f;
    currentTown.buyItems(*this, profitItemID, numberToBuy, home);
    }
  //Next, find out the second-best projected sell in this city.
  CitiesAndProfit.clear();
  double secondProfit = 0;
  int secondProfitNumber = 0; // how many to buy
  std::string secondProfitItemID;
  
  MaxPotentialLoad = getMaxGoods() - getTotalGoods();
  MaxMoney = captain.ducats;

  for each (AIEconomyItemTuple item in itemsBuyable)
    {
    auto memorizedItem = getItemFromDB(item.itemID);
    
    double profit = memorizedItem.cities[destination].sell - item.BuyPrice;
    int numberOf = MaxPotentialLoad;
    if (item.BuyPrice * MaxPotentialLoad > MaxMoney)
      numberOf = MaxMoney / item.BuyPrice;
    profit *= numberOf;

    if (profit > secondProfit)
      {
      secondProfit = profit;
      secondProfitItemID = memorizedItem.ID;
      secondProfitNumber = numberOf;
      }
    }

  if (secondProfit != 0 && secondProfitNumber != 0)
    {
    currentTown.buyItems(*this, secondProfitItemID, secondProfitNumber, home);
    }



  state = STATE_WAIT;
  };

int AIShip::random(const int& min, const int& max)
  {
  boost::random::uniform_int_distribution<> dist(min, max);
  return dist(gen);
  }

void AIShip::sell(Town& currentTown)
  {
  std::vector<std::string> itemsToSell;
  for (auto it = itemList.begin(); it < itemList.end(); it++)
    itemsToSell.push_back(it->ID);
  bool isHome = false;
  if (captain.faction == currentTown.getFactionID())
    isHome = true;
  for (auto it = itemsToSell.begin(); it < itemsToSell.end(); it++)
    currentTown.sellItems(*this, *it, -1, isHome);
  state = STATE_MERCHANTLOGIC;
  }

void AIShip::plotRandom(Pather& pather) // currently it chooses a random city out of known cities.
  {
  if (cityList.size() == 0)
    state = STATE_ERROR;
  int number = random(0, cityList.size()-1);
  auto it = cityList.begin();
  for (int counter = 0; counter < number; counter++)
    it++;
  setPath(pather.path(position, it->first, getWaveResistance()));
  state = STATE_RESTOCK;
  }

void AIShip::plot(Pather& pather, coord destination)
  {
  setPath(pather.path(position, destination, getWaveResistance()));
  state = STATE_RESTOCK;
  }

void AIShip::restock(Town& currentTown)
  {
  if (sailors < getMinSailors())
    {
    int numberToHire = (getMinSailors() - sailors) / 2 + (getMaxSailors() - getMinSailors())/2;
    captain.ducats -= 1000 * numberToHire;
    addSailors(numberToHire, 500);
    }

  double rationsNeeded = getEstimatedRationsNeeded();;
  rationsNeeded *= 1.1f;
  rationsNeeded += 5;
  rationsNeeded /= 10;
  int actualRations = (int)rationsNeeded * 10;
  actualRations -=  rations;
  if (actualRations > 0)
    currentTown.buyRations(*this, actualRations / 10);
  timer = random(0, 10);
  state = STATE_WAIT;
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
    state = STATE_SELL;
  }