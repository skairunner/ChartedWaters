#include "AI_ship.h"
#include <iostream>
#include <boost/random/uniform_int_distribution.hpp>

Town AIShip::nullTown = Town();

AIShip::AIShip()
  : state(STATE_PLOT), timer(0)
  {
  captain.ducats = 10000;
  gen.seed(rand());
  timer = random(0, 10);
  }

void AIShip::think(Pather& pather, std::map<coord, Town>& cityList, Town& currentTown)
  {
  switch (state)
    {
  case STATE_WAIT:
    wait();
    break;
  case STATE_MOVE:
    move();
    break;
  case STATE_PLOT:
    plot(pather, cityList);
    break;
  case STATE_RESTOCK:
    restock(currentTown);
    break;
  case STATE_ERROR:
    std::cout << "AI_ship.cpp error: error state\n";
    break;
  default:
    std::cout << "AI_ship.cpp error: invalid state\n";
    break;
    }
  }

void AIShip::wait()
  {
  timer--;
  if (timer <= 0)
    state = STATE_MOVE;
  }

void AIShip::startingWait()
  {
  timer--;
  if (timer <= 0)
    state = STATE_PLOT;
  }

int AIShip::random(const int& min, const int& max)
  {
  boost::random::uniform_int_distribution<> dist(min, max);
  return dist(gen);
  }

void AIShip::plot(Pather& pather, std::map<coord, Town>& cityList) // currently it chooses a random city out of known cities.
  {
  if (cityList.size() == 0)
    state = STATE_ERROR;
  int number = random(0, cityList.size());
  auto it = cityList.begin();
  for (int counter = 0; counter < number; counter++)
    it++;
  setPath(pather.path(position, it->first, getWaveResistance()));
  state = STATE_RESTOCK;
  }

void AIShip::restock(Town& currentTown)
  {
  if (captain.ducats < 3000)
    captain.ducats *= 52;

  double rationsNeeded = getEstimatedRationsNeeded();;
  rationsNeeded *= 1.1f;
  rationsNeeded += 5;
  rationsNeeded /= 10;
  int actualRations = (int)rationsNeeded * 10;
  actualRations -=  rations;
  if (actualRations > 0)
    currentTown.buyRations(*this, actualRations / 10);
  timer = random(0, 10);

  
  state = STATE_WAIT;
  }

void AIShip::move()
  {
  int counter = getMovementCounters();
  for (auto num = 0; num < counter; num++)
    updatePos();
  step();
  if (path.size() <= 1)
    state = STATE_PLOT; // change to STATE_SELL later.
  }