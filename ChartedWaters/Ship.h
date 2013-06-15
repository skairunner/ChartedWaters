#pragma once
#include <string>
#include "goods.h"

struct LedgerItemTuple
  {
  std::string itemID;
  std::string ItemName;
  std::string numberOfItems;
  std::string averagePurchasePrice;
  };

class Ship
  {
  public:
    Ship();
    void setName(const std::string& newName);
    std::string getName();
    bool addMoney(const int& amount); // false if the total money would go under 0.
    void addItem(const Item& item, const int& numberOf, const int& averagePrice);
    bool removeItem(const int ItemID, const int& numberOf); // return false in case there are not enough items to remove.
    int getNumberOfItems(const int& ID);
    int getPurchasePriceOf(const int& ID);
    int getMoney();
    int getTotalStorageUsed();
    int getMaxStorage();
    std::vector<LedgerItemTuple> returnListOfItems();

  private:

    bool removeFromList(const int& itemID);
    std::string shipName;
    int ducats;
    int storage;
    int maxstorage;
    std::vector<LedgerItem> itemList;
    std::pair<int, int> position;
  };