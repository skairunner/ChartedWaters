#include "worldMap.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

BiomeDictionary BiomeDict;

BiomeDictionary::BiomeDictionary()
{
    enum_nameMap[BIOME::none] = "none";
    enum_nameMap[BIOME::tropicalForest] = "tropical forest";
    enum_nameMap[BIOME::savanna] = "savanna";
    enum_nameMap[BIOME::desert] = "desert";
    enum_nameMap[BIOME::forest] = "forest";
    enum_nameMap[BIOME::grassland] = "grassland";
    enum_nameMap[BIOME::mediterranean] = "mediterranean";
    enum_nameMap[BIOME::coniferousForest] = "coniferous forest";
    enum_nameMap[BIOME::tundra] = "tundra";

    for (auto it = enum_nameMap.begin(); it != enum_nameMap.end(); it++)
        name_enumMap[it->second] = it->first;
}

BIOME BiomeDictionary::EnumFromName(const std::string& name)
{
    if (name_enumMap.find(name) == name_enumMap.end())
        return BIOME::none;
    return name_enumMap[name];
}

std::string BiomeDictionary::NameFromEnum(const BIOME& id)
{
    if (enum_nameMap.find(id) == enum_nameMap.end())
        return "none";
    return enum_nameMap[id];
}

////////////
////////////
////////////

WorldMapClass::WorldMapClass(const int& width, const int& height)
  :w(width), h(height)
  {
  grid.reserve(sizeof(maptile) * w * h);
  for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
      grid.push_back(maptile());
  null.isNull = true;
  null.moisture = 0;
  }

double WorldMapClass::getValue(noise::module::Perlin& gen, const int& x, const int& y, const double& zoom)
{
    return gen.GetValue((x + 1) * zoom + 0.01, (y + 1) * zoom + 0.01, 2.5);
}

void WorldMapClass::gen()
{
    cities.clear();
    boatpaths.clear();
    noise::module::Perlin moisture;
    noise::module::Perlin altitude;
    noise::module::Perlin amplitude;
    noise::module::Perlin offset;
    moistureSeed = rand();
    altitudeSeed = rand();
    ampSeed = rand();
    offsetSeed = rand();
    moisture.SetSeed(moistureSeed);
    moisture.SetNoiseQuality(noise::QUALITY_BEST);
    altitude.SetSeed(altitudeSeed);
    altitude.SetNoiseQuality(noise::QUALITY_BEST);
    amplitude.SetSeed(ampSeed);
    offset.SetSeed(offsetSeed);    

    double zoom = 0.09; // originally 0.07
    // altitude!
    for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
    {
        float buffer;
        float temp = getValue(altitude, xcounter, ycounter, zoom) * abs(getValue(amplitude, xcounter, ycounter, zoom / 10));
        temp -= 0.25;
        if (temp < 0)
        {
            if (-temp > 0.3)
                temp *= 20;
            else
                temp *= 10;
            buffer = temp;
        }
        else
            buffer = temp * 20;
        ref(xcounter, ycounter).altitude = buffer;
        ref(xcounter, ycounter).isCoastal = false;
        ref(xcounter, ycounter).isCity = false;
        ref(xcounter, ycounter).isInZOC = false;
    }

    // moisture!
    double zoom2 = 0.05;
    for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
    {

        double m = getValue(moisture, xcounter, ycounter, zoom2);
        m += 1.0;
        ref(xcounter, ycounter).moisture = abs(m);
    }

    // Next do biome assignment. Temperature is 45 * {1 - (distance from equator) / (distance from equator to top of map)} - 15
    // as in, the further from the equator the colder it is.
    // Moisture is x200.

    for (int ycounter = 0; ycounter < h; ycounter++)
    for (int xcounter = 0; xcounter < w; xcounter++)
    {
        auto& tile = ref(xcounter, ycounter);
        float moisture = tile.moisture * 200;
        float temp = (1 - abs(ycounter - getHeight() * 0.5) / (getHeight()/2.0)) * 45 - 15;
        tile.temp = temp;

        if (tile.altitude < 0) // oceans have no biomes.
            tile.biome = BIOME::none;
        // referencing forseth_biome.jpeg
        else if (temp > 18)
        {
            if (moisture < 50)
                tile.biome = BIOME::desert;
            else if (moisture < 100)
                tile.biome = BIOME::savanna;
            else if (-12 / 250.0 * (moisture - 100) + 30 < temp)
                tile.biome = BIOME::tropicalForest;
            else
                tile.biome = BIOME::savanna;
        }
        else if (temp > 0)
        {
            if (moisture < 40)
                tile.biome = BIOME::desert;
            else if (moisture < 80)
                tile.biome = BIOME::mediterranean;
            else if (moisture < 120)
                tile.biome = BIOME::grassland;
            else
                tile.biome = BIOME::forest;
        }
        else if (temp > -5)
        {
            if (moisture < 10)
                tile.biome = BIOME::desert;
            else if (moisture < 50)
                tile.biome = BIOME::mediterranean;
            else
                tile.biome = BIOME::coniferousForest;
        }
        else
            tile.biome = BIOME::tundra;
    }

    setCoastFlags();
    setCityFlags();
}

maptile& WorldMapClass::ref(const int& x, const int& y)
  {
  if (y >= 0 && y < h)
    {
    if (x >= 0 && x < w)
      return grid[x + y * w];
    else if (x < 0)
      return grid[w + x % w + y * w];
    else
      return grid[x % w + y * w];
    }
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
      setCoastal(xcounter, ycounter);
      }
  }

bool WorldMapClass::setCoastal(const int& xcounter, const int& ycounter)
  {
  if (ref(xcounter, ycounter).altitude <= 0 || ref(xcounter, ycounter).isCoastal)
    return true;
  for (int y = ycounter - 1; y < ycounter + 2; y++)
    for (int x = xcounter - 1; x < xcounter + 2; x++)
      {
      if (x == xcounter && y == ycounter)
        continue;
      if (ref(x, y).altitude <= 0)
        {
        ref(xcounter, ycounter).isCoastal = true;
        return true;
        }
      }
    return false;
  }

void WorldMapClass::setFactionsCity(const int& faction, const int& numberOfCities, const long long& seed)
{ // Set faction cities.
    int tries = 0;
    randomBoat f(coord(w, h), faction, seed);
    f.setRandomPosUntilSea(*this);
    bool result = f.explore(*this);
    int num_cities = numberOfCities;
    do {
        tries++;
        result = f.explore(*this);
        if (result)
            num_cities -= result;
        if (tries > numberOfCities * numberOfCities)
            return;

    } while (num_cities > 0); // 

    // Finally, add this to the list of paths taken.
    boatpaths.reserve(boatpaths.size() + distance(f.path.begin(), f.path.end()));
    boatpaths.insert(boatpaths.end(), f.path.begin(), f.path.end());
}

void WorldMapClass::setCityFlags()
  {
  int number = 15;
  auto it = time(0);
  setFactionsCity(1, number, it++);
  setFactionsCity(2, number, it++);
  setFactionsCity(3, number, it++);
  setFactionsCity(4, number, it++);
  setFactionsCity(5, number, it++);
  setFactionsCity(6, number, it++);
  setFactionsCity(7, number, it++);
  number /= 2;
  setFactionsCity(1, number, it++);
  setFactionsCity(2, number, it++);
  setFactionsCity(3, number, it++);
  setFactionsCity(4, number, it++);
  setFactionsCity(5, number, it++);
  setFactionsCity(6, number, it++);
  setFactionsCity(7, number, it++);
  number /= 2;
  setFactionsCity(1, number, it++);
  setFactionsCity(2, number, it++);
  setFactionsCity(3, number, it++);
  setFactionsCity(4, number, it++);
  setFactionsCity(5, number, it++);
  setFactionsCity(6, number, it++);
  setFactionsCity(7, number, it++);
  }


////////
//////
////// Random boat
//////
////////

randomBoat::randomBoat(const coord& dim, const int& ffaction,const unsigned long int seed, const coord& start)
  : startingPosition(start), faction(ffaction)
  {
  gen.seed(seed);
  if (start == coord(-1, -1))
    setRandomPos(dim);

  path.push_back(pair<coord, int>(start, faction));
  }

void randomBoat::setRandomPos(const coord& dimensions)
  {
  uniform_int_distribution<> xdist(0, dimensions.first-1);
  uniform_int_distribution<> ydist(0, dimensions.second-1);
  startingPosition = currentPosition = coord(xdist(gen), ydist(gen));
  }

void randomBoat::setRandomPosUntilSea(WorldMapClass& wm)
  {
  while (wm.ref(currentPosition.first, currentPosition.second).altitude >= 0)
    setRandomPos(coord(wm.w, wm.h));
  }

void randomBoat::setSeed(unsigned long int seed)
  {
  gen.seed(seed);
  }

bool randomBoat::explore(WorldMapClass& wm)
{
    uniform_int_distribution<> dist(50, 100);
    int moves = dist(gen);
    int failures = 0;
    while (moves > 0)
    {
        bool result = tryMove(wm); /// discount failed moves
        auto it = wm.ref(currentPosition.first, currentPosition.second);

        path.push_back(pair<coord, int>(currentPosition, faction));

        if (it.isCoastal && it.isInZOC == 0)
        {
            wm.ref(currentPosition.first, currentPosition.second).isCity = true;
            wm.cities.push_back(currentPosition);
            int cityCode = wm.cities.size();
            recursiveZOC(currentPosition.first, currentPosition.second, wm, 5);
            return true;
        }

        if (!result)
        {
            moves++;
            failures++;
            if (failures > 20)
                return false;
        }
        moves--;
    }

    return false;
}

void randomBoat::recursiveZOC(const int& x,const int& y, WorldMapClass& wm, double size)
  {
  if (size < 0)
    return;
 /* else if (wm.ref(x, y).altitude < 0)
    return;*/
  else
    {
    wm.ref(x, y).isInZOC = faction;
    recursiveZOC(x-1, y, wm, size -1);
    recursiveZOC(x+1, y, wm, size -1);
    recursiveZOC(x, y-1, wm, size -1);
    recursiveZOC(x, y+1, wm, size -1);
    return;
    }
  }

void randomBoat::markZOC(WorldMapClass& wm, int cityCode)
  {
  // Set faction
  wm.ref(currentPosition.first-1, currentPosition.second-1).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second-1).isInZOC = faction;
  wm.ref(currentPosition.first+1, currentPosition.second-1).isInZOC = faction;
  wm.ref(currentPosition.first-1, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first+1, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first-1, currentPosition.second+1).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second+1).isInZOC = faction;
  wm.ref(currentPosition.first+1, currentPosition.second+1).isInZOC = faction;

  // Set owner
  wm.ref(currentPosition.first-1, currentPosition.second-1).owner = cityCode;
  wm.ref(currentPosition.first, currentPosition.second-1).owner = cityCode;
  wm.ref(currentPosition.first+1, currentPosition.second-1).owner = cityCode;
  wm.ref(currentPosition.first-1, currentPosition.second).owner = cityCode;
  wm.ref(currentPosition.first, currentPosition.second).owner = cityCode;
  wm.ref(currentPosition.first+1, currentPosition.second).owner = cityCode;
  wm.ref(currentPosition.first-1, currentPosition.second+1).owner = cityCode;
  wm.ref(currentPosition.first, currentPosition.second+1).owner = cityCode;
  wm.ref(currentPosition.first+1, currentPosition.second+1).owner = cityCode;

  // 
  wm.ref(currentPosition.first+2, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first-2, currentPosition.second).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second+2).isInZOC = faction;
  wm.ref(currentPosition.first, currentPosition.second-2).isInZOC = faction;

  wm.ref(currentPosition.first+2, currentPosition.second).owner = cityCode;
  wm.ref(currentPosition.first-2, currentPosition.second).owner = cityCode;
  wm.ref(currentPosition.first, currentPosition.second+2).owner = cityCode;
  wm.ref(currentPosition.first, currentPosition.second-2).owner = cityCode;
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
  } while (vertical == 0 && horizontal == 0 && (vertical == originY || horizontal == originX)); //While invalid & in the direction of origin

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

