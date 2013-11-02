#pragma once
#include "npcCombat.h"
#include "shipPrototype.h"
#include "shipparts.h"

class WinRateSimulation
  {
  public:
  WinRateSimulation();
  std::string print();

  private:
  ShipDictionary ships;
  ShipPartDictionary shipparts;
  std::map<std::string, std::map<std::string, int>> wins; /// map[aggressor][defender]

  int games;
  };