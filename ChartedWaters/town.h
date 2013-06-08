#pragma once
#include <map>
#include "goods.h"
#include "Ship.h"

enum TOWN_ERRORS {twSUCCESS = 1, twNO_SUCH_ITEM, twNOT_ENOUGH_MONEY, twNOT_ENOUGH_ITEMS};

class Town
  {
  public:
    Town();
    Town(const std::string& newName, const double& tax);
    std::string getName();
    int buyItems(Ship& ship, const int& ID, int numberOf); // if numberOf = -1, buy all items possible with the money.
    int sellItems(Ship& ship, const int& ID, int numberOf);
    void addItems(const int& ID, const int& numberOf);
    std::vector<std::string> returnListOfItems();
    int getPriceOf(const int& ID);
    double getTaxRate();
    void setTaxRate(const double& newTaxRate);

    int lastTransaction;
    int numberOfLastTransaction;
    int unitPurchasePriceOfSell;

  private:
    std::vector<EconomyItem> itemlist;
    std::string TownName;
    double taxRate;
  };