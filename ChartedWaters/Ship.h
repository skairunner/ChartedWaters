#pragma once
#include <string>
#include "goods.h"
#include "shipPrototype.h"

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
    Ship(const ShipPrototype& prototype);
    void changeShip(const ShipPrototype& prototype);

    void setName(const std::string& newName);
    std::string getName();
    bool addMoney(const int& amount); // false if the total money would go under 0.
    void addItem(const Item& item, const int& numberOf, const int& averagePrice);
    bool removeItem(const std::string& ItemID, const int& numberOf); // return false in case there are not enough items to remove.
    int getNumberOfItems(const std::string& ID);
    int getPurchasePriceOf(const std::string& ID);
    int getMoney();
    int getTotalStorageUsed();
    int getMaxStorage();
    std::vector<LedgerItemTuple> returnListOfItems();
    int getSpeed(); // Later should be used to calculate speed from sails.
    void setPosition(const std::pair<int, int>& newPos);

    void setPath(const std::vector<std::pair<int, int>>& ppath);
    void updatePos(); // If a day went by.

    std::pair<int, int> getPosition();

    int character; // default 127, a triangle
    int waveResistance;
    std::vector<std::pair<int,int>> path;
    int faction; //default 0


  private:   
    bool removeFromList(const std::string& itemID);
    std::string shipName;
    int ducats;
    int storage;
    int maxstorage;
    int baseSpeed;
    int maxcargo, maxsailors, maxcannons; // Must add up to maxStorage.
    std::vector<LedgerItem> itemList;
    std::pair<int, int> position;
  };