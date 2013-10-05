#include "World.h"
#include <cstdlib>
#include <time.h>

World::World(const int& w, const int& h)
  : width(w), height(h), WorldMap(w, h), nameFactory(rand()), first(true), entityMap(w, h)
  {
  nameFactory = KoreanNameFactory(rand());

  regen();
  pathfinder = new Pather(WorldMap);

  PlayerShip = Ship();
  PlayerShip.setName(nameFactory.getName());
  int size = cityList.size();
  int city = rand()%size;
  auto it = cityList.begin();
  for (; city > 1; city--)
    it++;
  auto pos = it->first;
  PlayerShip.setPosition(pos);
  }

World::~World()
  {
  delete pathfinder;
  }

void World::regen()
  {
  cityList.clear();
  shipList.clear();
  ItemDict.clearCitiesList();
  WorldMap.gen();
  ItemMaps.SetSeed(rand());
  gen.seed(rand()); 
  entityMap.clear();

  for (auto it = WorldMap.cities.begin(); it < WorldMap.cities.end(); it++)
    {
    cityList[*it] = Town(nameFactory.getName(), 0.15f, WorldMap.ref(it->first, it->second).isInZOC);
    cityList[*it].myPosition = *it;
    }

  populateCities();
  for (auto it = cityList.begin(); it != cityList.end(); it++)
    it->second.spawnItems();
  int size = cityList.size();
  int city = rand()%size;
  auto it = cityList.begin();
  for (; city > 1; city--)
    it++;
  auto pos = it->first;
  PlayerShip.setPosition(pos);
  populateShips();

  if (!pathfinder)
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
  return cityList.begin()->second;
  }

int World::random(const int& min, const int& max)
  {
  std::uniform_int_distribution<> dist(min, max);
  return  dist(gen);
  }

coord World::getRandomCityCoord()
  {
  int number = random(0, cityList.size()-1);
  auto it = cityList.begin();
  for (int counter = 0; counter < number; counter++)
    it++;

  return it->first;
  }

void World::step()
  {
  entityMap.step();
  for (auto it = cityList.begin(); it != cityList.end(); it++)
    {
    it->second.step();
    }
  int position = 0;
  for (auto it = shipList.begin(); it != shipList.end(); it++)
    {
    if (queryShop(*it))
      it->think(*pathfinder, cityList, getTown(*it));
    else it->think(*pathfinder, cityList);
    entityMap.setEntity(position, it->getPosition());
    position++;
    }
  
  }

void World::populateCities()
  {
  ///// Use z=1.5 for food, 3.5 for industrial, 5.5 for other, 7.5 for luxury;
  ///// Use z= 10.5 for population.
  const double zoom = 0.01;

  auto foodlist = ItemDict.getItemsPerCategory(std::string("Food"));
  auto otherlist = ItemDict.getItemsPerCategory(std::string("Other"));
  auto industlist = ItemDict.getItemsPerCategory(std::string("Raw materials"));
  auto luxurylist = ItemDict.getItemsPerCategory(std::string("Luxury"));
  for (auto it = cityList.begin(); it != cityList.end(); it++)
    {
    
    auto pos = it->first;

    // Pick ships to stock in the drydocks.
    for (int counter = 0; counter < 5; counter++)
      {
      it->second.shipList.push_back(ShipDict.getRandomShip());
      }

    // First, Food.
    double food = ItemMaps.GetValue(pos.first * zoom + 0.001, pos.second * zoom + 0.001, 1.5);
    double indust = ItemMaps.GetValue(pos.first * zoom + 0.001, pos.second * zoom + 0.001, 3.5);
    double other = ItemMaps.GetValue(pos.first * zoom + 0.001, pos.second * zoom + 0.001, 5.5);
    double luxury = ItemMaps.GetValue(pos.first * zoom + 0.001, pos.second * zoom + 0.001, 7.5);
    double population = ItemMaps.GetValue(pos.first * zoom + 0.001, pos.second * zoom + 0.001, 10.5);

    population += 2;
    population = (int)(100 * abs(population));
    it->second.population = population;

    food += 2; // bump into positive.
    other += 2;
    indust += 2;
    luxury += 2;

    food = (int)(2 * abs(food)); // Remove any vestigal negatives.
    indust = (int)(2 * abs(indust));
    other = (int)(2 * abs(other));
    luxury = (int)(1.5 * abs(luxury));

    if (food >= 4)
      it->second.isAgri = true;
    if (indust >= 4)
      it->second.isIndustrial = true;
    if (luxury >= 4)
      it->second.isLuxury = true;
    if (other >= 4)
      it->second.isOther = true;


    while (food > 0)
      {
      food -= 1;
      int number = random(0, foodlist.size()-1);
      it->second.spawnList.push_back(foodlist.at(number));
      }
    while (indust > 0)
      {
      indust -= 1;
      int number = random(0, industlist.size()-1);
      it->second.spawnList.push_back(industlist.at(number));
      }
    while (other > 0)
      {
      other -= 1;
      int number = random(0, otherlist.size()-1);
      it->second.spawnList.push_back(otherlist.at(number));
      }
    while (luxury > 0)
      {
      luxury -= 1;
      int number = random(0, luxurylist.size() -1);
      it->second.spawnList.push_back(luxurylist.at(number));
      }

    for (auto itemID = it->second.spawnList.begin(); itemID < it->second.spawnList.end(); itemID++)
      {
      ItemDict.addCityToItem(*itemID, it->first);
      }
    }
  }

void World::populateShips()
  {
#ifdef NDEBUG
  for (int counter = 0; counter < 50; counter++)
#endif
#ifndef NDEBUG
  for (int counter = 0; counter < 1; counter++)
#endif
    {
   // auto position = getRandomCityCoord();
    coord position;
    position = getPlayerShip().getPosition();
    AIShip ship;
    //ship.changeShip(ShipDict.getRandomShip());
    ship.changeShip(ShipDict.getRandomMerchantShip());
    ship.setName(nameFactory.getName());
    ship.setPosition(position);
    ship.sailors = ship.getMinSailors();
    ship.rations = 500;
    ship.captain.faction = random(0, 8);
    // Let's arm 80% of the ships to the teeth!
    if (rand()%10000 < 8000)
      pickCannons(ship);
    pickArmor(ship);


    for (int counter = 0; counter < 5; counter++)
      ship.cityList[getRandomCityCoord()] = true;
    ship.initItemDB(cityList);
    shipList.push_back(ship);
    }
  }

void World::pickCannons(Ship& ship)
  {
  ShipCannons cannon_temp = ShipPartDict.getRandomCannon();
  int cannons = ship.getMaxCannons();
  int pairs = 8;
  bool abort = false;
  while (!abort)
    {
    if (cannons < pairs * 2)
      pairs--;
    else if (pairs == 0)
      abort = true;
    else if (ship.cannonList.size() > 5)
      abort = true;
    else
      {
      cannon_temp.pairs = pairs;
      cannons -= pairs * 2;
      ship.cannonList.push_back(cannon_temp);
      }
    }
  }

void World::pickArmor(Ship& ship)
  {
  double probability[3];
  probability[0] = 0.9f;
  probability[1] = 0.5f;
  probability[2] = 0.2f;

  int number = 0;
  if (ship.getSize() == "small")
    number = 1;
  else if (ship.getSize() == "medium")
    number = 2;
  else if (ship.getSize() == "large")
    number = 3;
  int counter = 0;

  while (number > 0)
    {   
    if (rand()%10000 < probability[counter]*10000)
      {
      auto armor = ShipPartDict.getRandomArmor();
      ship.addArmor(counter, armor);
      }
    number--;
    counter++;
    }

  }

/////
///
/// Renderer
///
/////

void Renderer::getHighResTerrainBitmap(TCODConsole* map, WorldMapClass& wm)
  {
  const double thresholdMoisture = 0;
  const TCODColor sandcolor = TCODColor(123, 107, 57);
  const TCODColor coastalBrown = TCODColor(141,124, 71);

  // Originals
  /*const TCODColor sandcolor = TCODColor(193, 177, 127);
  const TCODColor coastalBrown = TCODColor(231,194,141);*/

  for (int ycounter = 0; ycounter < wm.getHeight(); ycounter++)
    for (int xcounter = 0; xcounter < wm.getWidth(); xcounter++)
      {
      int character = 219;
      auto it = wm.ref(xcounter, ycounter);
      if (it.altitude >= 0)
        {
        TCODColor fore = TCODColor::desaturatedGreen;
        TCODColor back;

        if (it.moisture < thresholdMoisture)
          {
          fore = sandcolor;
          }
        fore = TCODColor::lerp(fore, TCODColor::black, it.altitude / 20.0f);
        back = fore;

        if (it.isCoastal)
          {
          character = 178;
          back = coastalBrown;
          fore = sandcolor;
          }

        map->putCharEx(xcounter, ycounter, character, fore, back);
        }

      else
        map->putCharEx(xcounter, ycounter, 178, TCODColor::lerp(TCODColor::blue, TCODColor::black, -1 * it.altitude / 40.0f), TCODColor::darkBlue);
      }
  }

void Renderer::getTerrainBitmap(TCODConsole* map, WorldMapClass& wm)
  {
  const double thresholdMoisture = 0;
  const TCODColor sandcolor = TCODColor(123, 107, 57);
  const TCODColor coastalBrown = TCODColor(141,124, 71);

  // Originals
  /*const TCODColor sandcolor = TCODColor(193, 177, 127);
  const TCODColor coastalBrown = TCODColor(231,194,141);*/

  for (int ycounter = 0; ycounter < wm.getHeight(); ycounter++)
    for (int xcounter = 0; xcounter < wm.getWidth(); xcounter++)
      {
      int character = 219;
      auto it = wm.ref(xcounter, ycounter);
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
          back = coastalBrown;
          fore = sandcolor;
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
  for (auto it = world.shipList.begin(); it != world.shipList.end(); it++)
    {
    auto pos = it->getPosition();   

    shipmap->putCharEx(pos.first, pos.second, it->character, findFactionColor(it->captain.faction), TCODColor::black);
    }
  Ship& ship = world.getPlayerShip();
  auto pos = ship.getPosition();
  shipmap->putCharEx(pos.first, pos.second, ship.character, findFactionColor(ship.captain.faction), TCODColor::black);
  }

void Renderer::getTooltip(TCODConsole* tooltip, World& world, const int& mouseX, const int& mouseY)
  {
  std::string name = findCityName(coord(mouseX, mouseY), world); 
  tooltip->clear();
  int line = 0;
  tooltip->setDefaultForeground(TCODColor::yellow);

  if (name.size() != 0)
    tooltip->print(0, line, "City of %s", name.c_str());
    line++;

  auto shiplist = world.entityMap.getEntityList(mouseX, mouseY);
  tooltip->setDefaultForeground(TCODColor::white);
  for (auto it = shiplist.begin(); it < shiplist.end(); it++)
    tooltip->print(0, line++, "The %s", world.shipList[*it].getName().c_str());

  auto pos = world.getPlayerShip().getPosition();
  if (pos.first == mouseX && pos.second == mouseY)
    tooltip->print(0, line++, "The %s", world.getPlayerShip().getName().c_str());
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

