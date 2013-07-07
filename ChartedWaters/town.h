#pragma once
#include <map>
#include "goods.h"
#include "Ship.h"
#include <vector>

enum TOWN_ERRORS {twSUCCESS = 1, twNO_SUCH_ITEM, twNOT_ENOUGH_MONEY, twNOT_ENOUGH_ITEMS};

struct EconomyItemTuple
  {
  std::string itemID;
  std::string ItemName;
  std::string numberOfItems;
  std::string BuyPrice;
  std::string SellPrice;
  std::string percentageOfBasePrice;
  };

class Town
  {
  private:
    std::vector<EconomyItem> itemlist;
    std::string TownName;
    double taxRate;
    int faction;
    std::vector<EconomyItem>::iterator getItemIterator(const std::string& ID); //return itemlist.end() if not found.
    double getDistanceFromNearestSource(const std::string& ID);
    

  public:
    Town();
    Town(const std::string& newName, const double& tax, const int& ffaction = 0);
    std::string getName();
    int buyItems(Ship& ship, const std::string& ID, int numberOf, bool hometown = false); // if numberOf = -1, buy all items possible with the money.
    int sellItems(Ship& ship, const std::string& ID, int numberOf, bool hometown = false);
    void addItems(const std::string& ID, const int& numberOf);
    int getFactionID();
    void spawnItems(); // For now, items spawn 100 each except for the Luxury category, which spawns 50 each.

    std::vector<EconomyItemTuple> returnListOfItems(bool isHometown = false);
    int getPriceOf(const std::string& ID);
    double  getSellPrice(const std::string& ID);
    double getTaxRate(bool hometown = false);
    int getNumberOf(const std::string& ID);

    void setTaxRate(const double& newTaxRate);

    int numberOfLastTransaction;
    int lastTransaction;
    int unitPurchasePriceOfSell;
    std::string lastTransactionItemID;

    bool isAgri, isIndustrial, isOther, isLuxury;
    std::pair<int, int> myPosition;
    std::vector<std::string> spawnList;
  };