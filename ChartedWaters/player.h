#pragma once
#include <string>
#include "shipparts.h"
#include <map>

class Skill
{
public:
    Skill();
    Skill(const std::string& Name);
    std::string name;
    void train(const int& val);
    double getValue(); // Returns level plus % to next level
    int getLevel();
    static int pointsNeeded(int level); // Points needed to attain the level "level+1"
    int netPointsNeeded(); // Points left until next level
private:
    int training; // current points
    int level;
};

class Player
  {
  public:
      Player();
    std::string name;
    int ducats;
    int faction;
    void addPart(ShipSails part);
    void addPart(ShipArmor part);
    void addPart(ShipStatue part);
    void addPart(ShipCannons part);

    std::map<std::string, Skill> skills;
    std::vector<ShipSails> sailInventory;
    std::vector<ShipArmor> armorInventory;
    std::vector<ShipStatue> figureheadInventory;
    std::vector<ShipCannons> cannonInventory;
  };