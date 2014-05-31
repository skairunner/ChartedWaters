#include "player.h"
#include "goods.h"

Skill::Skill()
{
    training = 0;
    name = "";
    level = 1;
}

Skill::Skill(const std::string& Name)
{
    name = Name;
    training = 0;
    level = 1;
}

void Skill::train(const int& val)
{
    training += val;
    while (training > pointsNeeded(level))
        level++;
}

int Skill::pointsNeeded(int level)
{
    if (level < 1)
        return 0;
    return int(pow(1.5, level) * 1000);
}

int Skill::netPointsNeeded()
{
    int points = pointsNeeded(level);
    return points - training;
}

int Skill::getLevel()
{
    return level;
}

double Skill::getValue()
{
    return level + (training - pointsNeeded(level - 1)) / (double)(pointsNeeded(level)  - pointsNeeded(level-1));
}

Player::Player()
: ducats(0)
{
    auto keys = ItemDict.getCategories();
    for (std::string s : keys)
    {
        skills[s] = Skill(s);
    }
}

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