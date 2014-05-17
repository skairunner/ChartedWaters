#pragma once
#include <map>
#include "goods.h"
#include "Ship.h"
#include "Fleet.h"
#include <vector>

enum TOWN_ERRORS {twNOT_ENOUGH_MONEY = -1, twSUCCESS = 1, twNO_SUCH_ITEM, twNOT_ENOUGH_ITEMS};

struct EconomyItemTuple
  {
  std::string itemID;
  std::string ItemName;
  std::string numberOfItems;
  std::string BuyPrice;
  std::string SellPrice;
  std::string percentageOfBasePrice;
  };

struct AIEconomyItemTuple
  {
  std::string itemID;
  std::string ItemName;
  int numberofItems;
  double BuyPrice;
  double SellPrice;
  double percentageOfBasePrice;
  };

class Town
  {
  private:
    std::map<std::string, EconomyItem> itemlist;
    std::string TownName;
    double taxRate;
    int faction;
    std::map<std::string, EconomyItem>::iterator getItemIterator(const std::string& ID); //return itemlist.end() if not found.
    double getDistanceFromNearestSource(const std::string& ID);
    int spawncounter; // for demand
    void addDemandToItem(const std::string& ID, const int& add);
    int getDemand(const std::string& ID);
    std::map<std::string, int> demandList;

  public:
    Town();
    ~Town();
    Town(const std::string& newName, const double& tax, const int& ffaction = 0);
    std::string getName();
    int buyItems(Ship& ship, const std::string& ID, int numberOf, bool hometown = false); // if numberOf = -1, buy all items possible with the money.
    int sellItems(Ship& ship, const std::string& ID, int numberOf, bool hometown = false);
    void addItems(const std::string& ID, const int& numberOf);
    int getFactionID();
    void spawnItems(); // For now, items spawn 100 each except for the Luxury category, which spawns 50 each.
    void step();
    int buyRations(Ship& ship, const int& number); // actual rations added = 10 * number
    static int recoverFatigue(Ship& ship, const int& number);
    static int recoverFatigue(Fleet& fleet, const int& number, int index = -1); // Recovers for all ships, divides fatigue recovery
    static int getCost(const int& amount);//for fatigue

    std::vector<EconomyItemTuple> returnListOfItems(bool isHometown = false);
    std::vector<AIEconomyItemTuple> returnListOfItems_AI(bool isHometown = false);
    int getPriceOf(const std::string& ID);
    double  getSellPrice(const std::string& ID, const double& bonus = 1);
    double getBuyPrice(const std::string& ID);
    double getTaxRate(bool hometown = false);
    int getNumberOf(const std::string& ID);

    void setTaxRate(const double& newTaxRate);

    int numberOfLastTransaction;
    int lastTransaction;
    int unitPurchasePriceOfSell;
    std::string lastTransactionItemID;

    int population;
    bool isAgri, isIndustrial, isOther, isLuxury;
    std::pair<int, int> myPosition;
    std::vector<std::string> spawnList;
    std::vector<ShipPrototype> shipList;
    std::vector<ShipPart*> partList;
  };