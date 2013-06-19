#pragma once
#include <map>
#include "goods.h"
#include "Ship.h"

enum TOWN_ERRORS {twSUCCESS = 1, twNO_SUCH_ITEM, twNOT_ENOUGH_MONEY, twNOT_ENOUGH_ITEMS};

struct EconomyItemTuple
  {
  std::string itemID;
  std::string ItemName;
  std::string numberOfItems;
  std::string BuyPrice;
  std::string SellPrice;
  };

class Town
  {
  private:
    std::vector<EconomyItem> itemlist;
    std::string TownName;
    double taxRate;
    int faction;

  public:
    Town();
    Town(const std::string& newName, const double& tax, const int& ffaction = 0);
    std::string getName();
    int buyItems(Ship& ship, const std::string& ID, int numberOf); // if numberOf = -1, buy all items possible with the money.
    int sellItems(Ship& ship, const std::string& ID, int numberOf);
    void addItems(const std::string& ID, const int& numberOf);
    int getFactionID();

    std::vector<EconomyItemTuple> returnListOfItems();
    int getPriceOf(const std::string& ID);
    double getTaxRate();
    int getNumberOf(const std::string& ID);

    void setTaxRate(const double& newTaxRate);

    int numberOfLastTransaction;
    int lastTransaction;
    int unitPurchasePriceOfSell;

  
  };