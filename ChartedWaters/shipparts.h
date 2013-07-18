#pragma once
#include <string>
#include "json/json.h"
#include <map>

class JSONToShipPart;

struct ShipPart
  {
  ShipPart();
  std::string ID;
  int price;
  std::string desc;
  };

struct ShipSails: public ShipPart
  {
  ShipSails();
  int lateen;
  int square;
  int turning;
  };

struct ShipArmor: public ShipPart
  {
  ShipArmor();
  int armor;
  double speed;
  };

struct ShipStatue: public ShipPart
  {
  ShipStatue();
  int protection; // from events
  int healing; // fatigue
  int dodging; //cannons
  };

class ShipPartDictionary
  {
  friend class JSONToShipPart;
  public:
    ShipSails getSail(const std::string& ID);
    ShipArmor getArmor(const std::string& ID);
    ShipStatue getFigurehead(const std::string& ID);
  private:
    std::map<std::string, ShipSails> sailList;
    std::map<std::string, ShipArmor> armorList;
    std::map<std::string, ShipStatue> figureheadList;
  };

class JSONToShipPart
  {
  public:
    void readShipParts(ShipPartDictionary& dict);
  private:
    std::string slurp(const std::string& filename);

  };

extern ShipPartDictionary ShipPartDict;