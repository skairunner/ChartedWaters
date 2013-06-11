#include "worldMap.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

WorldMapClass::WorldMapClass(const int& width, const int& height)
  :w(width), h(height)
  {
  grid.reserve(sizeof(maptile) * w * h);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      grid.push_back(maptile());
  null.isNull = true;
  srand(time(0));
  }

void WorldMapClass::gen()
  {
  for (int counter = 0; counter < 50; counter++)
    rand();
  cities.clear();
  noise::module::Perlin moisture;
  noise::module::Perlin altitude;
  moisture.SetSeed(rand());
  altitude.SetSeed(rand());
  vector<float> buffer(w * h, 0);
  double zoom = 0.07;
  // altitude!
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      double temp = altitude.GetValue((xcounter+1) * zoom + 0.01, (ycounter+1) * zoom + 0.01, 2.5);
      temp -= 0.25;
      if (temp < 0)
        {
        if (-temp > 0.3)
          temp *= 20;
        else
          temp *= 10;
        buffer[xcounter+ycounter*w] = temp;
        }
      else
        buffer[xcounter + ycounter * w] = temp * 20;// > 0 ? temp * 20 : 0; // we don't need buffer right now
      ref(xcounter, ycounter).altitude = buffer[xcounter + ycounter * w];
      ref(xcounter, ycounter).isCoastal = false;
      ref(xcounter, ycounter).isCity = false;
      ref(xcounter, ycounter).isInZOC = false;
      }

    // moisture!
    double zoom2 = 0.05;
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      
      double temp = moisture.GetValue((xcounter+1) * zoom2 + 0.01, (ycounter+1) * zoom2 + 0.01, 0.5);
      if (ycounter > h * 0.32)
        temp += 0.4;
      else if (ycounter > h * 0.6 && ycounter < h * 0.8)
        temp += 0.7;
      temp += 0.0;
      // buffer[xcounter + ycounter * w] = temp > 0 ? temp * 20 : 0;
      ref(xcounter, ycounter).moisture = temp;
      }

    setCoastFlags();
    setCityFlags();
  }

maptile& WorldMapClass::ref(const int& x, const int& y)
  {
  if (x >= 0 && y >= 0 && x < w && y < h)
    return grid[x + y * w];
  else
    return null;
  }

int WorldMapClass::getWidth()
  {
  return w;
  }

int WorldMapClass::getHeight()
  {
  return h;
  }

void WorldMapClass::setCoastFlags()
  {
  for ( int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      {
      auto thisCell = ref(xcounter, ycounter);
      if (thisCell.isCoastal)
        continue;
      if (thisCell.altitude <= 0) // if this is a sea tile
        {
        if (ref(xcounter -1, ycounter+1).altitude > 0 && !ref(xcounter-1, ycounter+1).isNull) // If land and not null
          ref(xcounter-1, ycounter+1).isCoastal = true;
        if (ref(xcounter, ycounter+1).altitude > 0 && !ref(xcounter, ycounter+1).isNull) // If land and not null
          ref(xcounter, ycounter+1).isCoastal = true;
        if (ref(xcounter+1, ycounter+1).altitude > 0 && !ref(xcounter+1, ycounter+1).isNull) // If land and not null
          ref(xcounter+1, ycounter+1).isCoastal = true;
        continue;
        }
      else // this is a land tile
        {
        if (ref(xcounter-1, ycounter).altitude == 0 && !ref(xcounter-1, ycounter).isNull) // if the tile behind is sea
          ref(xcounter, ycounter).isCoastal = true;
        else if (ref(xcounter+1, ycounter).altitude == 0 && !ref(xcounter+1, ycounter).isNull) // tile ahead is sea
          ref(xcounter, ycounter).isCoastal = true;
        else if (ref(xcounter+1, ycounter+1).altitude == 0 && !ref(xcounter+1, ycounter+1).isNull) // tile ahead+below is sea
          ref(xcounter, ycounter).isCoastal = true;
        else if (ref(xcounter, ycounter+1).altitude == 0 && !ref(xcounter, ycounter+1).isNull) // tile below is sea
          ref(xcounter, ycounter).isCoastal = true;
        else if (ref(xcounter-1, ycounter+1).altitude == 0 && !ref(xcounter-1, ycounter+1).isNull) // tile below and behind is sea
          ref(xcounter, ycounter).isCoastal = true;
        }
      }
  }

void WorldMapClass::setFactionsCity(const int& faction, const int& numberOfCities)
  { // Faction's
  randomBoat f(coord(w, h), faction); f.setRandomPosUntilSea(*this);
  bool result = f.explore(*this);
  int num_cities = numberOfCities;
  do {
    result = f.explore(*this);
    num_cities -= result;
    if (result)
      cities.push_back(f.currentPosition);
    } while (num_cities > 0); // 
  }

void WorldMapClass::setCityFlags()
  {
  /// Let's have four factions right now.
  const int number = 4;
  setFactionsCity(1, number);
  setFactionsCity(2, number);
  setFactionsCity(3, number);
  setFactionsCity(4, number);
  setFactionsCity(5, number);
  setFactionsCity(6, number);
  setFactionsCity(7, number);

  setFactionsCity(1, number);
  setFactionsCity(2, number);
  setFactionsCity(3, number);
  setFactionsCity(4, number);
  setFactionsCity(5, number);
  setFactionsCity(6, number);
  setFactionsCity(7, number);

  setFactionsCity(1, number);
  setFactionsCity(2, number);
  setFactionsCity(3, number);
  setFactionsCity(4, number);
  setFactionsCity(5, number);
  setFactionsCity(6, number);
  setFactionsCity(7, number);  
  }


////////
//////
////// Random boat
//////
////////

randomBoat::randomBoat(const coord& dim, const int& ffaction, const coord& start)
  : startingPosition(start), faction(ffaction)
  {
  if (start == coord(-1, -1))
    setRandomPos(dim);
  }

void randomBoat::setRandomPos(const coord& dimensions)
  {
  uniform_int_distribution<> xdist(0, dimensions.first);
  uniform_int_distribution<> ydist(0, dimensions.second);
  startingPosition = currentPosition = coord(xdist(gen), ydist(gen));
  }

void randomBoat::setRandomPosUntilSea(WorldMapClass& wm)
  {
  while (wm.ref(currentPosition.first, currentPosition.second).altitude > 0)
    setRandomPos(coord(wm.w, wm.h));
  }

bool randomBoat::explore(WorldMapClass& wm)
  {
  uniform_int_distribution<> dist(50, 100);
  int moves = dist(gen);
  while (moves > 0)
    {
    bool result = tryMove(wm); /// discount failed moves
    auto it = wm.ref(currentPosition.first, currentPosition.second);
    if (it.isCoastal && it.isInZOC == 0)
      {
      wm.ref(currentPosition.first, currentPosition.second).isCity = true;
      markZOC(wm);
      
      return true;
      }
    if (!result)
      moves++;
    moves--;
    }

  return false;
  }

void randomBoat::recursiveZOC(const int& x,const int& y, WorldMapClass& wm, double budget)
  {
  if (budget <= 0)
    return;
  else if (wm.ref(x, y).altitude <= 0)
    return;
  else
    {
    wm.ref(x, y).isInZOC = faction;
    double cost = wm.ref(x, y).altitude;
    recursiveZOC(x-1, y, wm, budget - cost);
    recursiveZOC(x+1, y, wm, budget - cost);
    recursiveZOC(x, y-1, wm, budget - cost);
    recursiveZOC(x, y+1, wm, budget - cost);
    return;
    }
  }

void randomBoat::markZOC(WorldMapClass& wm)
  {
  wm.ref(currentPosition.first-1, currentPosition.second-1).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second-1).isInZOC = faction;
  wm.ref(currentPosition.first+1, currentPosition.second-1).isInZOC = faction;
  wm.ref(currentPosition.first-1, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first+1, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first-1, currentPosition.second+1).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second+1).isInZOC = faction;
  wm.ref(currentPosition.first+1, currentPosition.second+1).isInZOC = faction;

  // 
  wm.ref(currentPosition.first+2, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first-2, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second-2).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second+2).isInZOC = faction;
 // recursiveZOC(currentPosition.first, currentPosition.second, wm, 5);
  }

bool randomBoat::tryMove(WorldMapClass& wm) // return true if succeeded.
  {
  uniform_int_distribution<> dist(0, 2);
  int originX = startingPosition.first - currentPosition.first;
  if (originX != 0)
    originX = originX > 0 ? 1 : -1;

  int originY = startingPosition.second - currentPosition.second;
  if (originY != 0)
    originY = originY > 0 ? 1 : -1;

  int vertical, horizontal;
  do {
    vertical = dist(gen) - 1;
    horizontal = dist(gen) - 1;
  } while (vertical == 0 && horizontal == 0 && (vertical == originY || horizontal == originX)); //While invalid & not in the direction of origin

  auto it = wm.ref(currentPosition.first + horizontal, currentPosition.second + vertical);
  if (!it.isNull && (it.isCoastal || it.altitude < 0))
    {
    currentPosition.first += horizontal;
    currentPosition.second += vertical;
    if (it.isCoastal)
      startingPosition = currentPosition;
    return true;
    }

  return false;
  }

