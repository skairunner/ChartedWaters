#pragma once
#include <vector>
#include <string>
#include <map>



enum ItemIDs {IID_NULL = 0, IID_FRIED_CHICKEN, IID_COOKED_LLAMA_CHOPS, IID_CHICKEN_CHEESE};

class ItemDictionary;
class Item;

class JSONToItem
  {
  public:
    void readItems(); // reads the file names from index.json first.
    void readItems(ItemDictionary& dict); // Reads items into an item dictionary.

  private:
    std::string slurp(const std::string& filename);
    void slurpItemsAndPrint(const std::string& filename);
    void slurpItems(const std::string& filename, ItemDictionary& dict);
  };

class ItemDictionary
  {
  friend class JSONToItem;
  public:
   // ItemDictionary();
    ItemDictionary();
    std::string findItemName(const std::string& ID);
    int findBasePrice(const std::string& ID);
    std::pair<double, double> findDecayRates(const std::string& ID);
    std::string findItemDesc(const std::string& ID);
    Item& getItemTemplate(const std::string& itemID);
    void sortIntoLists(); // Make a map of vector of item IDs.
    std::vector<std::string> getItemsPerCategory(const std::string& category);
    

  private:
    std::map<std::string, Item> ItemList;
    std::map<std::string, std::vector<std::string>> categories;
    std::map<std::string, std::vector<std::string>> itemsPerCategory;

  };

extern ItemDictionary ItemDict;

// These are item archetypes, so to speak. They shouldn't hold any variables other than price.
class Item
  {
  public:
    Item();
    Item(const Item& item);
    //Item(const int& newID);
    Item(const std::string& newID);
    bool operator==(const Item& right) const; // An economy item is identical to an Item if the IDs are the same.
    bool operator<(const Item& right) const; // This one sorts by alphabetical order

    std::string ID;
    std::string name;
    std::string desc;
    std::string category;
    std::string type;
   
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