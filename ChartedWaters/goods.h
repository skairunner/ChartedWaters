#pragma once
#include <vector>
#include <string>
#include <map>

// These are item archetypes, so to speak. They shouldn't hold any variables other than price.

enum ItemIDs {IID_NULL = 0, IID_FRIED_CHICKEN, IID_COOKED_LLAMA_CHOPS};

class ItemDictionary
  {
  public:
    ItemDictionary();
    std::string findItemName(const int& ID);
    double findBasePrice(const int& ID);
    std::pair<double, double> findDecayRates(const int& ID);
  private:
    std::map<int, std::string> ItemNames;
    std::map<int, double> BasePrice;
    std::map<int, std::pair<double, double>> DecayRates;
  };

extern ItemDictionary ItemDict;

class Item
  {
  public:
    Item();
    Item(const int& newID);
    bool operator==(const Item& right) const; // An economy item is identical to an Item if the IDs are the same.
    bool operator<(const Item& right) const; // This one sorts by alphabetical order
    std::string getName() const;
    double getBasePrice() const;
    int getID() const;

  protected:
    int ID;
    std::string itemName;
    int basePrice;
    double decayRatePositive; // When the demand > supply
    double decayRateNegative; // when the demand < supply.
    // So that demand slowly restores itself to the central.
  };

class EconomyItem: public Item
  {
  public:
    EconomyItem(const int& ID, const int& newSupply = 50, const int& newDemand = 50);

    void decayDemand();
    int getPrice();
    int howMany();
    void addItem(const int& howMany);
    void setDemand(const int& equi);

  private:
    int supply; // this is how many of me I have!
    int demand; // how much demand is current!
    int spawnRate; // how many are added each time a spawn event is fired
  };

class LedgerItem : public Item
  {
  public:
    LedgerItem(const int& ID);
    LedgerItem(const Item& item, const int& howMany, const int& averagePrice);
    void addItem(const int& howMany, const int& averagePrice); // FILO
    bool removeItems(const int& howMany);
    int getAveragePrice();
    int howMany();

  private:
    int totalItems;
    void countItems();
    std::vector<std::pair<int, int>> ledger; // numberOf, price.
  };