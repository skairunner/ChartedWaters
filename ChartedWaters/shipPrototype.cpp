#include "shipPrototype.h"
#include <fstream>
#include "json/json.h"

using namespace std;

ShipDictionary ShipDict;




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
    buffer.ID = ship["ID"].asString();
    buffer.name = ship["name"].asString();
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
    buffer.baseArmor = ship["base armor"].asInt();
    buffer.maxDurability = ship["max durability"].asInt();
    if (buffer.maxstorage != buffer.maxcannons + buffer.maxcargo + buffer.maxsailors)
      cerr << "Storage != cargo + sailors + cannons for " << buffer.ID << "!\n";
    dict.ships[buffer.ID] = buffer;
    counter++;
    }
  };