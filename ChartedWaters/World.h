#pragma once
#include "worldMap.h"
#include "Ship.h"
#include "town.h"
#include <map>
#include <libtcod.hpp>
#include "nameFactory.h"
#include "A_star.h"

class Renderer;

class World
  {
  friend class Renderer;
  public:
    World(const int& w, const int& h);
    ~World();
    std::string getCityName(const coord& coords); // Return empty string if there is no such town.
    void regen();
    Ship& getPlayerShip();
    Pather *pathfinder;

  private:
    NameFactory nameFactory;
    WorldMapClass WorldMap;
    PathMap *pathmap;
    
    std::map<coord, Town> cityList;
    std::vector<Ship> shipList;
    Ship PlayerShip;
    int width, height;
  };

class Renderer // Accesses World and returns bitmaps.
  {
  public:
    static void getTerrainBitmap(TCODConsole* map, World& world);
    static void getCityBitmap(TCODConsole* cities, World& world);
    static TCODColor findFactionColor(const int& faction);
    static std::string findCityName(const coord& coords, World& world);
    
  private:

  };
