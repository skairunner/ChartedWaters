#include "goods.h"
#include <cmath>
#include <fstream>
#include "json/json.h"

ItemDictionary ItemDict;
using namespace std;

///////////////////////
//////////
//////////  JSONToItem
//////////
///////////////////////

void JSONToItem::readItems(ItemDictionary& dict)
  {
  string index;
  Json::Value root;
  Json::Reader reader;
  index = slurp(string("resources/index.json"));
  bool parsingSuccess = reader.parse(index, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  Json::Value list = root["files"];
  while (!list[counter].isNull())
    {
    slurpItems(string("resources/") + list[counter].asString(), dict);
    cout << endl;
    counter++;
    }
  }

void JSONToItem::readItems()
  {
  string index;
  Json::Value root;
  Json::Reader reader;
  index = slurp(string("resources/index.json"));
  bool parsingSuccess = reader.parse(index, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  Json::Value list = root["files"];
  while (!list[counter].isNull())
    {
    slurpItemsAndPrint(string("resources/") + list[counter].asString());
    cout << endl;
    counter++;
    }
  }

void JSONToItem::slurpItems(const std::string& filename, ItemDictionary& dict)
  {
  string itemlist;
  Json::Value root;
  Json::Reader reader;
  itemlist = slurp(filename);
  bool parsingSuccess = reader.parse(itemlist, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  while (!root[counter].isNull())
    {
    Json::Value item = root[counter];
    
    string ID = item["ID"].asString();
    dict.ItemNames[ID] = item["name"].asString();
    dict.BasePrice[ID] = item["base price"].asInt();
    if (!item["desc"].isNull())
      dict.ItemDesc[ID] = item["dest"].asString();
    counter++;
    }
  }

void JSONToItem::slurpItemsAndPrint(const std::string& filename)
  {
  string itemlist;
  Json::Value root;
  Json::Reader reader;
  itemlist = slurp(filename);
  bool parsingSuccess = reader.parse(itemlist, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;
  cout << "In " << filename << "---------------------------\n";
  while (!root[counter].isNull())
    {
    Json::Value item = root[counter];
    
    cout << item["name"].asString() << " (" << item["ID"].asString() << ")";
    if (!item["desc"].isNull())
      cout << " : " << item["desc"].asString();

    cout << endl;
    counter++;
    }

  }

string JSONToItem::slurp(const string& filename)
  {
  fstream file(filename.c_str());
  string output; output.clear();
  string buffer; buffer.clear();
  while (!file.eof())
    {
    getline(file, buffer);
    output.append(buffer + string("\n"));
    }
  return output;
  }


///////////////////////
//////////
//////////  ItemDictionary
//////////
///////////////////////

/*ItemDictionary::ItemDictionary()
  {
  ItemNames[IID_NULL] = std::string("Invalid item/null");
  ItemNames[IID_FRIED_CHICKEN] = std::string("Fried chicken");
  ItemNames[IID_COOKED_LLAMA_CHOPS] = std::string("Cooked llama chops");
  ItemNames[IID_CHICKEN_CHEESE] = std::string("Chicken-cheese");

  BasePrice[IID_NULL] = 0;
  BasePrice[IID_FRIED_CHICKEN] = 100;
  BasePrice[IID_COOKED_LLAMA_CHOPS] = 100;
  BasePrice[IID_CHICKEN_CHEESE] = 16;

  DecayRates[IID_NULL] = std::pair<double, double>(0, 0);
  DecayRates[IID_FRIED_CHICKEN] = std::pair<double, double>(0.1, 0.1);
  DecayRates[IID_COOKED_LLAMA_CHOPS] = std::pair<double, double>(0.1, 0.1);
  DecayRates[IID_CHICKEN_CHEESE] = std::pair<double, double>(0.1, 0.1);
  }*/





ItemDictionary::ItemDictionary()
  {
  categories.push_back(string("Food"));
  categories.push_back(string("Raw materials"));
  categories.push_back(string("Other"));
  categories.push_back(string("Luxury items"));

  types.push_back(string("Alcohol"));
  types.push_back(string("Foodstuffs"));
  types.push_back(string("Seasonings"));
  types.push_back(string("Livestock"));
  types.push_back(string("Luxury food"));

  types.push_back(string("Textiles"));
  types.push_back(string("Fabric"));
  types.push_back(string("Dyes"));
  types.push_back(string("Ores"));
  types.push_back(string("Industrial goods"));

  types.push_back(string("Medicine"));
  types.push_back(string("Sundries"));
  types.push_back(string("Weapons"));
  types.push_back(string("Firearms"));
  types.push_back(string("Crafts"));

  types.push_back(string("Artwork"));
  types.push_back(string("Spices"));
  types.push_back(string("Precious metals"));
  types.push_back(string("Fragrances"));
  types.push_back(string("Jewellery"));
  types.push_back(string("Precious stones"));

  BasePrice[string("null")] = 0;
  ItemNames[string("null")] = string("invalid item");
  DecayRates[string("null")] = pair<double, double>(0, 0);
  ItemDesc[string("null")] = string("No description.");
  }

string ItemDictionary::findItemName(const std::string& ID)
  {
  auto it = ItemNames.find(ID);
  if (it == ItemNames.end())
    return ItemNames[string("null")];
  else
    return it->second;
  }

int ItemDictionary::findBasePrice(const std::string& ID)
  {
  auto it = BasePrice.find(ID);
  if (it == BasePrice.end())
    return BasePrice[string("null")];
  else
    return it->second;
  }

pair<double, double> ItemDictionary::findDecayRates(const std::string& ID)
  {
  auto it = DecayRates.find(ID);
  if (it == DecayRates.end())
    return DecayRates[string("null")];
  else
    return it->second;
  }

///////////////////////
//////////
//////////  Item
//////////
///////////////////////

Item::Item()
  : itemName(string("Null")), basePrice(0), decayRatePositive(0), decayRateNegative(0)
  {
  }

Item::Item(const std::string& newID)
  :ID(newID)
  {
  itemName = ItemDict.findItemName(newID);
  basePrice = ItemDict.findBasePrice(newID);
  pair<double, double> buffer = ItemDict.findDecayRates(newID);
  decayRatePositive = buffer.first;
  decayRateNegative = buffer.second;
  }


bool Item::operator==(const Item& right) const
  {
  if (right.getID() == ID)
    return true;
  else return false;
  }

bool Item::operator<(const Item& right) const
  {
  if (itemName < right.getName())
    return true;
  else return false;
  }

std::string Item::getID() const
  {
  return ID;
  }

string Item::getName() const
  {
  return itemName;
  }

double Item::getBasePrice() const
  {
  return basePrice;
  }



///////////////////////
//////////
//////////  EconomyItem
//////////
///////////////////////

EconomyItem::EconomyItem(const std::string& ID, const int& newSupply, const int& newDemand)
  : Item(ID), supply(newSupply), demand(newDemand)
  {

  }

void EconomyItem::decayDemand()
  {
  if (demand > supply)
    demand *= 1 - decayRatePositive;
  else if (demand < supply)
    demand *= 1 + decayRateNegative;
  }

int EconomyItem::getPrice()
  {
  const double VERTICAL_SHIFT = 0.25f;
  const double HORIZONTAL_SHIFT = 500.0f;
  const double HORIZONTAL_SCALE = 0.01f;
  const double PRICE_RANGE = 1.5f;
  double adjustedDemand = (1500 + 500 * 0.5 * log(supply / (double)demand)) / 3.0f;
  double multiplier = VERTICAL_SHIFT + PRICE_RANGE / (1 + exp(HORIZONTAL_SCALE * (adjustedDemand - HORIZONTAL_SHIFT)));
  return multiplier * basePrice;
  }

int EconomyItem::howMany()
  {
  return supply;
  }

void EconomyItem::addItem(const int& howMany)
  {
  supply += howMany;
  }

void EconomyItem::setDemand(const int& equi)
  {
  demand = equi;
  }

void EconomyItem::addDemand(const int& increased)
  {
  demand += increased;
  }

///////////////////////
//////////
//////////  LedgerItem
//////////
///////////////////////

LedgerItem::LedgerItem(const std::string& ID)
  :Item(ID), totalItems(0)
  {

  }

LedgerItem::LedgerItem(const Item& item, const int& howMany, const int& averagePrice)
  :Item(item), totalItems(howMany)
  {
  ledger.push_back(pair<int, int>(howMany, averagePrice));
  }

void LedgerItem::addItem(const int& howMany, const int& averagePrice)
  {
  ledger.push_back(pair<int, int>(howMany, averagePrice));
  totalItems += howMany;
  }

bool LedgerItem::removeItems(const int& howMany)
  {
  if (totalItems < howMany)
    return false;
  int counter = howMany;
  for (auto it = ledger.rbegin(); it < ledger.rend(); it++)
    {
    if (counter - it->first <= 0) // If the contents of it can handle the need.
      {
      it->first -= counter;
      break;
      }
    else
      {
      counter -= it->first;
      it->first = 0;
      }
    }

  // Finally, pop the back members that are 0.
  while (ledger.size() != 0 && ledger.at(ledger.size()-1).first == 0)
    ledger.pop_back();
  totalItems -= howMany;
  return true;
  }

int LedgerItem::getAveragePrice()
  {
  int total = 0;
  for (auto it = ledger.begin(); it < ledger.end(); it++)
    {
    total += it->first * it->second;
    }
  return total / (int)totalItems;
  }

void LedgerItem::countItems()
  {
  int total = 0;
  for (auto it = ledger.begin(); it < ledger.end(); it++)
    {
    total += it->first;
    }
  totalItems = total;
  }

int LedgerItem::howMany()
  {
  return totalItems;
  }