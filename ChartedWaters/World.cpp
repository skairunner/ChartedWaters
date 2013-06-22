#include "World.h"
#include <cstdlib>
#include <time.h>

World::World(const int& w, const int& h)
  : width(w), height(h), WorldMap(w, h), nameFactory(312515), first(true)
  {
  nameFactory = NameFactory(rand());

  regen();
  pathfinder = new Pather(WorldMap);

  PlayerShip = Ship();
  PlayerShip.setName(nameFactory.getName());
  randomBoat rb(coord(w, h));
  rb.setRandomPosUntilSea(WorldMap);
  PlayerShip.setPosition(rb.currentPosition);
  }

World::~World()
  {
  delete pathfinder;
  }

void World::regen()
  {
  cityList.clear();
  WorldMap.gen();
  ItemMaps.SetSeed(rand());

  for (auto it = WorldMap.cities.begin(); it < WorldMap.cities.end(); it++)
    {
    cityList[*it] = Town(nameFactory.getName(), 0.05f, WorldMap.ref(it->first, it->second).isInZOC);
    }
  populateCities();
  delete pathfinder;
  pathfinder = new Pather(WorldMap);
  }

Ship& World::getPlayerShip()
  {
  return PlayerShip;
  }

bool World::queryShop(Ship& ship)
  {
  auto it = cityList.find(ship.getPosition());
  if (it == cityList.end())
    return false;
  else return true;
  }

Town& World::getTown(Ship& ship)
  {
  auto it = cityList.find(ship.getPosition());
  return it->second;
  }

Town& World::getFirstTown()
  {
  if (first)
    {
    Town& it = cityList.begin()->second;
    it.addItems(std::string("luxury_leg"), 9);
    it.addItems(std::string("other_tylenol"), 992);
    it.addItems(std::string("luxury_demonicmark"), 666);
    it.addItems(std::string("food_tallowroast"), 400);
    it.addItems(std::string("luxury_remalle"), 35);
    it.addItems(std::string("luxury_allspice"), 120);
    it.addItems(std::string("food_cookedllamachops"), 190);
    it.addItems(std::string("food_chickencheese"), 1269);
    first = false;
    }
  return cityList.begin()->second;
  }

void World::populateCities()
  {
  ///// Use z=1.5 for food, 3.5 for industrial, 5.5 for other, 7.5 for luxury;
  const double zoom = 0.01;
  for (auto it = cityList.begin(); it != cityList.end(); it++)
    {
    auto pos = it->first;
    // First, Food.
    double food = ItemMaps.GetValue(pos.first * zoom + 0.001, pos.second * zoom + 0.001, 1.5);
    food += 3; // bump into positive.
    food = abs(food); // Remove any vestigal negatives.

    }
  }

/////
///
/// Renderer
///
/////

void Renderer::getTerrainBitmap(TCODConsole* map, World& world)
  {
  const double thresholdMoisture = 0;
  const TCODColor sandcolor = TCODColor(123, 107, 57);
  const TCODColor coastalBrown = TCODColor(141,124, 71);

  // Originals
  /*const TCODColor sandcolor = TCODColor(193, 177, 127);
  const TCODColor coastalBrown = TCODColor(231,194,141);*/

  for (int ycounter = 0; ycounter < world.height; ycounter++)
    for (int xcounter = 0; xcounter < world.width; xcounter++)
      {
      int character = 219;
      auto it = world.WorldMap.ref(xcounter, ycounter);
      if (it.altitude >= 0)
        {
        TCODColor fore = TCODColor::desaturatedGreen;
        TCODColor back;

        if (it.moisture < thresholdMoisture)
          {
          fore = sandcolor;
          }
        fore = TCODColor::lerp(fore, TCODColor::black, it.altitude / 40.0f);
        back = fore;

        if (it.isCoastal)
          {
          character = 178;
          fore = coastalBrown;
          }

        map->putCharEx(xcounter, ycounter, character, fore, back);
        }

      else
        map->putCharEx(xcounter, ycounter, 178, TCODColor::lerp(TCODColor::blue, TCODColor::black, -1 * it.altitude / 40.0f), TCODColor::darkBlue);
      }
  }

void Renderer::getCityBitmap(TCODConsole* cities, World& world)
  {
  for (auto it = world.cityList.begin(); it != world.cityList.end(); it++)
      {
      int coordx = it->first.first;
      int coordy = it->first.second;

      TCODColor citycolor = findFactionColor(it->second.getFactionID());
      cities->putCharEx(coordx % world.width, coordy, 99, citycolor, TCODColor::lerp(citycolor, TCODColor::black, 0.7f));
      }
  }

void Renderer::getAccessBitmap(TCODConsole* accessmap, PathMap& pm)
  {
  coord dimensions = pm.getDimensions();
  for (int ycounter = 0; ycounter < dimensions.second; ycounter++)
    for (int xcounter = 0; xcounter < dimensions.first; xcounter++)
      {
      if(!pm.ref(xcounter, ycounter).accessible)
        accessmap->putCharEx(xcounter, ycounter, 249, TCODColor::red, TCODColor::black);
      }
  }

void Renderer::getShipBitmap(TCODConsole* shipmap, World& world)
  {
  shipmap->clear();
  for (auto it = world.shipList.begin(); it < world.shipList.end(); it++)
    {
    auto pos = it->getPosition();
    shipmap->putCharEx(pos.first, pos.second, it->character, findFactionColor(it->faction), TCODColor::black);
    }
  Ship& ship = world.getPlayerShip();
  auto pos = ship.getPosition();
  shipmap->putCharEx(pos.first, pos.second, ship.character, findFactionColor(1), TCODColor::black);
  }

TCODColor Renderer::findFactionColor(const int& faction)
  {
  switch(faction) // Choose the city color
    {
  case 0: return TCODColor::gold;break;
  case 1: return TCODColor::red; break;
  case 2: return TCODColor::sky; break;
  case 3: return TCODColor::orange; break;
  case 4: return TCODColor::yellow; break;
  case 5: return TCODColor::darkGrey; break;
  case 6: return TCODColor::azure; break;
  case 7: return TCODColor::darkChartreuse; break;
  default: return TCODColor::white; break;
    }

  }

std::string Renderer::findCityName(const coord& coords, World& world)
  {
  auto it = world.cityList.find(coords); 
  if (it != world.cityList.end())
    return it->second.getName();
  return std::string("");
  }