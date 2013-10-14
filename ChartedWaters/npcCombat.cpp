#include "npcCombat.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

ShipCounter::ShipCounter(AIShip& ref)
  : refToShip(ref), fightOrFlee(0), velocity(0), acceleration(0)
  {
  maxSpeed = refToShip.getSpeed() * 10;
  cannonTimer = 0;
  distance = 0;
  
  if (refToShip.cannonList.size() == 0)
    {
    damage = pen = 0;
    range = 99900;
    }
  else
    {
    damage = refToShip.getCannons() / 2;
    damage *= refToShip.cannonList.front().baseDamage;
    pen = refToShip.cannonList.front().penetration;
    range = refToShip.cannonList.front().range;
    }

  history.push_back(position);
  HPHistory.push_back(refToShip.durability);
  }

void ShipCounter::step()
  {
  cannonTimer--;
  if (cannonTimer < 0) cannonTimer = 0;

  if (fightOrFlee == stateFLIGHT)
    {
    acceleration -= 5 * distance / abs(distance); // Run in the opposite direction of the other guy.
    }
  else
    seek();

  velocity += acceleration; 
  if (abs(velocity) > abs(maxSpeed))
      {
      velocity = maxSpeed * (velocity / abs(velocity));
      }
  if (abs(acceleration) > 20)
      acceleration = 20 * (acceleration / abs(acceleration));    

  position += velocity;
  history.push_back(position);
  HPHistory.push_back(refToShip.durability);
  }

void ShipCounter::seek()
  {
  int stopping = 0;

  if (acceleration != 0)
    stopping = abs(0.5 * velocity * velocity / acceleration);
  else if (velocity == 0)
    stopping = 0;

  if ( (abs(distance) > range * 2) || (abs(distance) > range + stopping) )
    {
    acceleration += 3 * distance / abs(distance); // get closer
    }
  else if (abs(distance) < range / 4 * 3)
    {
    acceleration -= 3 * distance / abs(distance); // get further away
    }
  else
    {
    acceleration = 0;
    velocity -= 0.2f * velocity;
    }
  }

Battle::Battle(AIShip& ship1, AIShip& ship2)
  : s1(ship1), s2(ship2), done(false)
  {
  fightOrFlightHeuristic();
  gen.seed(rand());
  std::uniform_int_distribution<> dist(200, 400);
  /*s1.position = dist(gen);
  s2.position = -dist(gen);*/
  s1.position = -500;
  s2.position = 500;
  s1.distance = s2.distance = abs(s1.position - s2.position);
  }

void Battle::fightOrFlightHeuristic()
  {/*
  double powerlevel1, powerlevel2;
  powerlevel1 = s1.refToShip.getCannons()/(s2.refToShip.getCannons()+0.001f);
  powerlevel2 = s2.refToShip.getCannons()/(s1.refToShip.getCannons()+0.001f);

  powerlevel1 += s1.refToShip.durability/(s2.refToShip.durability+0.001f);
  powerlevel2 += s2.refToShip.durability/(s1.refToShip.durability+0.001f);

  if (powerlevel1 * 2 < powerlevel2)
    s1.fightOrFlee = ShipCounter::stateFIGHT;
  else s1.fightOrFlee = ShipCounter::stateFLIGHT;

  if (powerlevel2 * 2 < powerlevel1)
    s2.fightOrFlee = ShipCounter::stateFIGHT;
  else s2.fightOrFlee = ShipCounter::stateFLIGHT;*/

  s1.fightOrFlee = s2.fightOrFlee = ShipCounter::stateFIGHT;
  }

void Battle::runAwayHeuristic()
  {
  /*if (s1.refToShip.durability > 2 * s2.refToShip.durability)
    {
    s2.fightOrFlee = ShipCounter::stateFLIGHT;
    }
  if (s2.refToShip.durability > 2 * s1.refToShip.durability)
    {
    s1.fightOrFlee = ShipCounter::stateFLIGHT;
    }*/

 /* if (s1.refToShip.durability > 2 * s2.refToShip.durability)
    {
    s2.fightOrFlee = ShipCounter::stateFLIGHT;
    }
  if (s2.refToShip.durability > 2 * s1.refToShip.durability)
    {
    s1.fightOrFlee = ShipCounter::stateFLIGHT;
    }*/
  }

void Battle::step()
  {
  if (done)
    return;
  s1.distance = s2.position - s1.position;
  s2.distance = s1.position - s2.position;


  shootEachOther();
  runAwayHeuristic();
  s1.step();
  s2.step();

 

  if (s1.refToShip.durability <= 0 || s2.refToShip.durability <= 0)
    done = true;

 // printf("s1: %d(%d)\ts2: %d(%d)\tdistance: %d\n", s1.position, s1.velocity, s2.position, s2.velocity, abs(s1.distance));
  //std::cout << "s1: " << s1.position << ", s2: " << s2.position << "\tdistance: " << s2.distance;

  if (abs(s1.distance) > 2000 || s1.damage == 0 || s2.damage == 0 || s1.history.size() > 150)
    done = true;


  }

int Battle::getDamage(ShipCounter& sc1, ShipCounter& sc2)
  {
  double rawDamage = sc1.damage;
  int armor = sc2.refToShip.getArmor();

  armor = armor < sc1.pen ? 0 : armor - sc1.pen;

  double damage = 50.0f / (armor+50) * rawDamage;

  return damage;
  }

void Battle::shootEachOther()// implied "if possible"
  {
  if (abs(s1.distance) < s1.range && !s1.cannonTimer)
    {
    s2.refToShip.durability -= getDamage(s1, s2);
    if (s1.refToShip.cannonList.size() > 0)
      s1.cannonTimer = s1.refToShip.cannonList.front().reload;
    }

  if (abs(s2.distance) < s2.range && !s2.cannonTimer)
    {
    s1.refToShip.durability -= getDamage(s2, s1);
    if (s2.refToShip.cannonList.size() > 0)
      s2.cannonTimer = s2.refToShip.cannonList.front().reload;
    }
  }

void Battle::print(std::string filename)
  {
  std::fstream file;
  file.open(filename.c_str(), std::fstream::trunc | std::fstream::out);
  if (s1.history.size() != s2.history.size())
    {
    file << "Error: history sizes do not match.";
    file.close();
    return;
    }

  file << s1.refToShip.getType() << " armor:" << s1.refToShip.getArmor();
  if (s1.refToShip.getCannons() > 0)
    file << " \"" << s1.refToShip.cannonList.front().name << "\"";
  file << " cannon damage:" << s1.damage << "(" << s1.range << ")\n";
  
  file << s2.refToShip.getType() << " armor:" << s2.refToShip.getArmor();
  if (s2.refToShip.getCannons() > 0)
    file << " \"" << s2.refToShip.cannonList.front().name << "\"";
  file << " cannon damage:" << s2.damage << "(" << s2.range << ")\n";

  for (int i = 0; i < s1.history.size(); i++)
    {
    file << s1.history[i] << "\t" << s2.history[i] << "\t" << s1.HPHistory[i] << "\t" << s2.HPHistory[i] << "\n";
    }
  file.close();
  }