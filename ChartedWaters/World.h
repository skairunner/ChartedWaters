#pragma once
#include "worldMap.h"
#include "Ship.h"
#include "town.h"
#include <map>
#include <libtcod.hpp>
#include "nameFactory.h"
#include "A_star.h"
#include "include/noise.h"
#include <random>
#include "AI_ship.h"
#include "entitymap.h"
#include "Fleet.h"

class Renderer;


class World
  {
  friend class Renderer;
  public:
    World(const int& w, const int& h);
    ~World();
    std::string getCityName(const coord& coords); // Return empty string if there is no such town.
    void regen();
    void step();
    Ship& getPlayerShip();
    Fleet& getPlayerFleet();
    Pather *pathfinder;
    bool queryTown(Ship& ship);
    bool queryTown(Fleet& fleet);
    Town& getTown(Ship& ship);
    Town& getTown(Fleet& fleet);
    Town& getFirstTown();
    int random(const int& min, const int& max);
    coord getRandomCityCoord();
    std::map<coord, Town> cityList;
    std::vector<AIShip> shipList;
    WorldMapClass WorldMap;
    EntityMap entityMap;




  private:    
    void populateCities(); // With items.
    void populateShips(); // AI ships.
    void pickCannons(Ship& ship);
    void pickArmor(Ship& ship);
    void pickSails(Ship& ship);
    
    noise::module::Perlin ItemMaps; 
    std::mt19937 gen;

    bool first;

    Ship PlayerShip;
    Fleet PlayerFleet;
    int width, height;
  };

class Renderer // Accesses World and returns bitmaps.
  {
  public:
    static void getTerrainBitmap(TCODConsole* map, WorldMapClass& wm);
    static void getHighResTerrainBitmap(TCODConsole* map, WorldMapClass& wm);
    static void getCityBitmap(TCODConsole* cities, World& world);
    static void getTrailBitmap(TCODConsole* cities, World& world);
    static TCODColor findFactionColor(const int& faction);
    static std::string findCityName(const coord& coords, World& world);
    static void getMoistureBitmap(TCODConsole* accessmap, WorldMapClass& wm);
    static void getBiomeBitmap(TCODConsole* biomemap, WorldMapClass& wm);
    static void getShipBitmap(TCODConsole* shipmap, World& world);
    static void getTooltip(TCODConsole* tooltip, World& world, const int& mouseX, const int& mouseY);
  private:
  };
