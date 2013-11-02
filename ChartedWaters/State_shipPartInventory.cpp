#include "State_shipPartInventory.h"


State_shipPartInventory::State_shipPartInventory(Ship& ship)
  : State_shipPartShop(0, &ship)
  {
  for (auto it = ship.captain.cannonInventory.begin(); it != ship.captain.cannonInventory.end(); it++)
    partList.push_back(&*it);
  for (auto it = ship.captain.armorInventory.begin(); it != ship.captain.armorInventory.end(); it++)
    partList.push_back(&*it);
  for (auto it = ship.captain.sailInventory.begin(); it != ship.captain.sailInventory.end(); it++)
    partList.push_back(&*it);
  for (auto it = ship.captain.figureheadInventory.begin(); it != ship.captain.figureheadInventory.end(); it++)
    partList.push_back(&*it);
  }


bool State_shipPartInventory::Init()
  {

  redrawLeft(partList);
  ShipPart* part = 0;
  if (selector >= 3 && selector - 3 < partList.size())
    part = partList[selector-3];
  redrawRight(part);

  return true;
  }

void State_shipPartInventory::Update()
  {
  if (redraw)
      {
      redrawLeft(partList);

      ShipPart* part = 0;
      if (selector >= 3 && selector - 3 < partList.size())
        part = partList[selector-3];
      redrawRight(part);

      redraw = false;
      }
  }

