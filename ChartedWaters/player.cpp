#include "player.h"

void Player::addPart(ShipSails part)
  {
  sailInventory.push_back(part);
  }

void Player::addPart(ShipArmor part)
  {
  armorInventory.push_back(part);
  }

void Player::addPart(ShipStatue part)
  {
  figureheadInventory.push_back(part);
  }

void Player::addPart(ShipCannons part)
  {
  cannonInventory.push_back(part);
  }