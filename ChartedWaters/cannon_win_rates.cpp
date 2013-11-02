#include "cannon_win_rates.h"
#include "utility.h"

using namespace std;

WinRateSimulation::WinRateSimulation()
  {
  JSONToShip shipReader;
  shipReader.readShips(ships);
  JSONToShipPart shippartReader;
  shippartReader.readCannons(shipparts);
  shippartReader.readShipParts(shipparts);

  AIShip templateship;
  templateship.changeShip(ships.getShip(string("galleon_large")));
  vector<string> cannons = shipparts.getCannonList();

  int numCannonPairs = templateship.getMaxCannons() / 2;

  for (auto it1 = cannons.begin(); it1 < cannons.end(); it1++)
    for (auto it2 = cannons.begin(); it2 < cannons.end(); it2++)
      {
      AIShip ship1 = templateship;
      AIShip ship2 = templateship;

      ShipCannons cannons1 = shipparts.getCannons(*it1);
      cannons1.pairs = numCannonPairs;
      ShipCannons cannons2 = shipparts.getCannons(*it2);
      cannons2.pairs = numCannonPairs;

      ship1.cannonList.push_back(cannons1);
      ship2.cannonList.push_back(cannons2);

      Battle bt(ship1, ship2);

      while (!bt.done)
        bt.step();


      if (ship1.durability > 0 && ship2.durability > 0)
        {
        wins[*it1][*it2] = wins[*it2][*it1] = 1234;
        }
      else if (ship1.durability <= 0 && ship2.durability <= 0)
        {
        wins[*it1][*it2] = 0;
        wins[*it2][*it1] = 0;
        }
      else if (ship1.durability > 0 && ship2.durability <= 0)
        {
        wins[*it1][*it2] = 1;
        wins[*it2][*it1] = -1;
        }
      else 
        {
        wins[*it1][*it2] = -1;
        wins[*it2][*it1] = 1;
        }
        
      }
  }

string WinRateSimulation::print()
  {
  string output;

  vector<string> cannons = shipparts.getCannonList();

  output = "\t";
  for each (string cannonID in cannons)
    {
    output += shipparts.getCannons(cannonID).name + "\t";
    }

  for (auto it1 = cannons.begin(); it1 < cannons.end(); it1++)
    {
    output += "\n";
    output += shipparts.getCannons(*it1).name;
    for (auto it2 = cannons.begin(); it2 < cannons.end(); it2++)
      {
      output += "\t" + rightAlignNumber(wins[*it1][*it2]);
      }
    }

  return output;
  }