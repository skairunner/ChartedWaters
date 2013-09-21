#include "shipPrototype.h"
#include <fstream>
#include "json/json.h"
#include <cstdlib>

using namespace std;

ShipDictionary ShipDict;

ShipDictionary::ShipDictionary()
  {
  null.typeID = "null";
  null.maxcannons = null.maxcargo = null.lateen = null.square = null.maxsailors = null.minimumsailors = null.price = null.waveResistance = 0;
  }

ShipPrototype ShipDictionary::getShip(const string& ID)
  {
  auto it = ships.find(ID);
  if (it == ships.end())
    return null;
  else return it->second;
  }

string JSONToShip::slurp(const string& filename)
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

void JSONToShip::readShips(ShipDictionary& dict)
  {
  string index;
  Json::Value root;
  Json::Reader reader;
  index = slurp(string("resources/ships.json"));
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
    Json::Value ship = root[counter];
    ShipPrototype buffer;
    buffer.typeID = ship["ID"].asString();
    buffer.typeName = ship["name"].asString();
    buffer.size = ship["size"].asString();
    buffer.specialization = ship["specialization"].asString();
    buffer.price = ship["price"].asInt();
    if (!ship["desc"].isNull())
      buffer.desc = ship["desc"].asString();
    buffer.maxstorage = ship["max storage"].asInt();
    buffer.maxcannons = ship["max cannons"].asInt();
    buffer.maxcargo = ship["max cargo"].asInt();
    buffer.maxsailors = ship["max sailors"].asInt();
    buffer.lateen = ship["lateen sail"].asInt();
    buffer.square = ship["square sail"].asInt();
    buffer.minimumsailors = ship["min sailors"].asInt();
    buffer.waveResistance = ship["wave resistance"].asInt();
    buffer.turning = ship["turning"].asInt();
    buffer.baseArmor = ship["base armor"].asInt();
    buffer.maxDurability = ship["max durability"].asInt();
    if (buffer.maxstorage != buffer.maxcannons + buffer.maxcargo + buffer.maxsailors)
      cerr << "Storage != cargo + sailors + cannons for " << buffer.typeID << "!\n";
    dict.ships[buffer.typeID] = buffer;

    dict.shipsByType[buffer.specialization].push_back(buffer);


    counter++;
    }
  };

int ShipPrototype::baseSpeed(const int& lateens, const int& squares)
  {
  return baseSpeed_d(lateens, squares);
  }

ShipPrototype ShipDictionary::getRandomShip()
  {
  int size = ships.size();
  auto it = ships.begin();
  int counter = rand()%size;
  for(; counter > 0; counter--)
    it++;
  return it->second;
  }

ShipPrototype ShipDictionary::getRandomMerchantShip()
  {
  int general = shipsByType["general"].size();
  int speed = shipsByType["speed"].size();
  int storage = shipsByType["storage"].size();

  int counter = rand()%(general+speed+storage);
  if (counter < general)
    return shipsByType["general"].at(counter);
  else
    {
    counter -= general;
    if (counter < speed)
      return shipsByType["speed"].at(counter);
    else
      {
      counter -= speed;
      return shipsByType["storage"].at(counter);
      }
    }
  }

double ShipPrototype::baseSpeed_d(const int& lateens, const int& squares)
  {
  double temp = 1.3f * lateens + 0.7f * squares;
  temp /= 49.1666666666667f;
  temp += 0.5f;
  return temp;
  }