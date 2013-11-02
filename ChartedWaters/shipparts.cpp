#include "shipparts.h"
#include <fstream>
#include "utility.h"

using namespace std;

ShipPartDictionary ShipPartDict;

ShipPart::ShipPart()
  {
  ID = desc = string("null");
  price = 0;
  durability = 0;
  }

std::string ShipPart::shopName()
  {
  return name;
  }

int ShipPart::shopPrice()
  {
  return price;
  }

ShipCannons::ShipCannons()
  {
  baseDamage = pairs = penetration = range = speed = explosion = reload = 0;
  }

ShipCannons::ShipCannons(ShipCannons& sc, int numPairs)
  {
  *this = ShipCannons(sc);
  pairs = numPairs;
  }

std::string ShipCannons::shopName()
  {
  if (pairs == 0)
    return name;
  return rightAlignNumber(pairs*2) + " " + name;
  }

int ShipCannons::shopPrice()
  {
  if (pairs == 0)
    return price;
  return price * pairs;
  }

int ShipCannons::getDamage()
  {
  return pairs * baseDamage;
  }

std::string ShipPart::type()
  {
  return string("base");
  }

std::string ShipCannons::type()
  {
  return string("cannon");
  }

std::string ShipArmor::type()
  {
  return string("armor");
  }

std::string ShipSails::type()
  {
  return string("sails");
  }

std::string ShipStatue::type()
  {
  return string("statue");
  }

ShipSails::ShipSails()
  {
  lateen = square = turning = 0;
  }

ShipArmor::ShipArmor()
  {
  armor = 0;
  speed = 0;
  }

ShipStatue::ShipStatue()
  {
  protection = healing = dodging = 0;
  }

ShipSails ShipPartDictionary::getSail(const std::string& ID)
  {
  auto it = sailList.find(ID);
  if (it == sailList.end())
    return ShipSails();
  return it->second;
  }

ShipSails ShipPartDictionary::getRandomSail()
  {
  int size = sailList.size();
  int rnd = rand()%size;
  auto it = sailList.begin();
  while (rnd > 0)
    {
    rnd--;
    it++;
    }
  return it->second;
  }

ShipStatue ShipPartDictionary::getFigurehead(const std::string& ID)
  {
  auto it = figureheadList.find(ID);
  if (it == figureheadList.end())
    return ShipStatue();
  return it->second;
  }

ShipArmor ShipPartDictionary::getArmor(const std::string& ID)
  {
  auto it = armorList.find(ID);
  if (it == armorList.end())
    return ShipArmor();
  return it->second;
  }

ShipArmor ShipPartDictionary::getRandomArmor()
  {
  int size = armorList.size();
  int rnd = rand()%size;
  auto it = armorList.begin();
  while (rnd > 0)
    {
    rnd--;
    it++;
    }
  return it->second;
  }

ShipCannons ShipPartDictionary::getCannons(const std::string& ID)
  {
  auto it = cannonList.find(ID);
  if (it == cannonList.end())
    return ShipCannons();
  return it->second;
  }

std::vector<std::string> ShipPartDictionary::getCannonList()
  {
  std::vector<std::string> list;

  for(auto it = cannonList.begin(); it != cannonList.end(); it++)
    list.push_back(it->second.ID);

  return list;
  }

ShipCannons ShipPartDictionary::getRandomCannon()
  {
  int size = cannonList.size();
  int rnd = rand()%size;
  auto it = cannonList.begin();
  while (rnd > 0)
    {
    rnd--;
    it++;
    }
  return it->second;
  }

string JSONToShipPart::slurp(const string& filename)
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

void JSONToShipPart::readShipParts(ShipPartDictionary& dict)
  {
  string index;
  Json::Value root;
  Json::Reader reader;
  index = slurp(string("resources/shipparts.json"));
  bool parsingSuccess = reader.parse(index, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  while (!root[counter].isNull())
    {
    Json::Value shippart = root[counter];
    string type = shippart["type"].asString();

    if (type == string("sail"))
      {
      ShipSails sails;
      sails.ID = shippart["ID"].asString();
      sails.name = shippart["name"].asString();
      sails.price = shippart["price"].asInt();
      sails.lateen = shippart["lateen"].asInt();
      sails.square = shippart["square"].asInt();
      sails.turning = shippart["turning"].asDouble();
      if(!shippart["desc"].isNull())
        sails.desc = shippart["desc"].asString();
      dict.sailList[sails.ID] = sails;
      }
    else if (type == string("plate"))
      {
      ShipArmor armor;
      armor.ID = shippart["ID"].asString();
      armor.price = shippart["price"].asInt();
      armor.name = shippart["name"].asString();
      armor.durability = shippart["durability"].asInt();
      armor.armor = shippart["armor"].asInt();
      armor.speed = shippart["speed"].asDouble();
      if(!shippart["desc"].isNull())
        armor.desc = shippart["desc"].asString();
      dict.armorList[armor.ID] = armor;
      }
    else if (type == string("statue"))
      {
      ShipStatue statue;
      statue.ID = shippart["ID"].asString();
      statue.name = shippart["name"].asString();
      statue.durability = shippart["durability"].asInt();
      statue.price = shippart["price"].asInt();
      statue.protection = shippart["protection"].asInt();
      statue.healing = shippart["healing"].asInt();
      statue.dodging = shippart["dodging"].asInt();
      if(!shippart["desc"].isNull())
        statue.desc = shippart["desc"].asString();
      dict.figureheadList[statue.ID] = statue;
      }
    counter++;
    }
  }

void JSONToShipPart::readCannons(ShipPartDictionary& dict)
  {
  string index;
  Json::Value root;
  Json::Reader reader;
  index = slurp(string("resources/cannons.json"));
  bool parsingSuccess = reader.parse(index, root);
  if (!parsingSuccess)
    {
    cout << "Didn't succeed: " << reader.getFormattedErrorMessages();
    cin.ignore();
    return;
    }
  int counter = 0;

  while (!root[counter].isNull())
    {
    Json::Value shippart = root[counter];
    string type = shippart["type"].asString();

    ShipCannons cannon;
    cannon.ID = shippart["ID"].asString();
    cannon.name = shippart["name"].asString();
    cannon.price = shippart["price"].asInt();
    cannon.durability = shippart["durability"].asInt();
    cannon.baseDamage = shippart["damage"].asInt();
    cannon.penetration = shippart["penetration"].asInt();
    cannon.range = shippart["max_range"].asInt();
    cannon.speed = shippart["speed"].asInt();
    cannon.explosion = shippart["explosion"].asInt();
    cannon.reload = shippart["reload_speed"].asInt();

    if(!shippart["desc"].isNull())
      cannon.desc = shippart["desc"].asString();
    dict.cannonList[cannon.ID] = cannon;


    counter++;
    }
  }