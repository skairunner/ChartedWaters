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
    void addPart(ShipSails part);
    void addPart(ShipArmor part);
    void addPart(ShipStatue part);
    void addPart(ShipCannons part);

    std::vector<ShipSails> sailInventory;
    std::vector<ShipArmor> armorInventory;
    std::vector<ShipStatue> figureheadInventory;
    std::vector<ShipCannons> cannonInventory;
  };