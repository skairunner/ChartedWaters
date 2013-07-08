#pragma once
#include <string>
#include <map>
//// Basically ship types that can be pasted over player/merchant ships to change their attributes.

class JSONToShip;

class ShipPrototype
  {
  public:
    std::string ID;
    std::string type; //eg, 'Caravel', 'Carrack'
    std::string desc;
    int price;
    int maxstorage;
    int maxcargo, maxsailors, maxcannons;
    int baseSpeed;
    int waveResistance;
  };

class ShipDictionary
  {
  friend class JSONToShip;
  public:
    ShipPrototype getShip(const std::string& ID);
  private:
    std::map<std::string, ShipPrototype> ships;
  };

class JSONToShip
  {
  public:
    void readShips(ShipDictionary& dict);
  private:
    std::string slurp(const std::string& filename);
  };

extern ShipDictionary ShipDict;