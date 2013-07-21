#pragma once
#include <string>
#include "shipparts.h"
#include <map>

class Player
  {
  public:
    std::string name;
    int ducats;
    int faction;
    std::map<int, ShipSails> sailInventory;
    std::map<int, ShipArmor> armorInventory;
    std::map<int, ShipStatue> figureheadInventory;
  };