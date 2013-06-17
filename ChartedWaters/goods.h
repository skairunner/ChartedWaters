#pragma once
#include <vector>
#include <string>
#include <map>



enum ItemIDs {IID_NULL = 0, IID_FRIED_CHICKEN, IID_COOKED_LLAMA_CHOPS, IID_CHICKEN_CHEESE};

class ItemDictionary;

class JSONToItem
  {
  public:
    void readItems(); // reads the file names from index.json first.
  private:
    std::string slurp(const std::string& filename);
    void slurpItems(const std::string& filename);
  };

class ItemDictionary
  {
  friend class JSONToItem;
  public:
   // ItemDictionary();
    ItemDictionary();
    std::string findItemName(const std::string& ID);
    double findBasePrice(const std::string& ID);
    std::pair<double, double> findDecayRates(const std::string& ID);
  private:
    std::map<std::string, std::string> ItemNames;
    std::map<std::string, double> BasePrice;
    std::map<std::string, std::pair<double, double>> DecayRates;
  };

extern ItemDictionary ItemDict;

// These are item archetypes, so to speak. They shouldn't hold any variables other than price.
class Item
  {
  public:
    Item();
    //Item(const int& newID);
    Item(const std::string& newID);
    bool operator==(const Item& right) const; // An economy item is identical to an Item if the IDs are the same.
    bool operator<(const Item& right) const; // This one sorts by alphabetical order
    std::string getName() const;
    double getBasePrice() const;
    std::string getID() const;

  protected:
    //int ID;
    std::string ID;
    std::string itemName;

    std::string* category;
    std::string* type;

    int basePrice;
    double decayRatePositive; // When the demand > supply
    double decayRateNegative; // when the demand < supply.
    // So that demand slowly restores itself to the central.
  };

class EconomyItem: public Item
  {
  public:
    EconomyItem(const std::string& ID, const int& newSupply = 50, const int& newDemand = 50);

    void decayDemand();
    int getPrice();
    int howMany();
    void addItem(const int& howMany);
    void setDemand(const int& equi);
    void addDemand(const int& increased);

  private:
    int supply; // this is how many of me I have!
    int demand; // how much demand is current!
    int spawnRate; // how many are added each time a spawn event is fired
  };

class LedgerItem : public Item
  {
  public:
    LedgerItem(const std::string& ID);
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