#include "Engine.h"
#include "TCODTools.h"
#include "World.h"
#include "State_ShipStatus.h"
#include <string>
//#include "State_shop.h"
#include "State_MainGame.h"
#include "shipPrototype.h"
#include "State_Combat.h"
#include "State_townmenu.h"
#include "npcCombat.h"
#include "State_shipPartInventory.h"
#include "State_showskills.h"
#include "Fleet.h"

Engine CursesEngine;

int mouseX, mouseY;

const int width = 256;
const int height = 256;

/*const int screenwidth = 100;
const int screenheight = 48;*/

const int screenwidth = 150;
const int screenheight = 80;


int playerMovement = 0;
int daysPassed = 0;

World* TheWorld;
TCODConsole* cityscreen;
TCODConsole* trailsscreen; // A demo console to show off the settler boat pathing.
TCODConsole* mapscreen;
TCODConsole* ZOCscreen;
TCODConsole* tooltip;
TCODConsole* PathScreen;
TCODConsole* MoistureScreen;
TCODConsole* ShipScreen;
TCODConsole* BiomeScreen;

bool redo = false;
typedef pair<int, int> coord;
GameState* newState;
bool mouseClick = false;
bool pressedPeriod = false;
bool lockedToShip = false;
bool pressedArrow = false; // ">"
bool mouseRightClick = false;
bool showTrails = false;
bool showZOC = false;
bool showMoisture = false;
bool showBiomes = false;
bool checkBiomeDebug = false;

Fleet* lockedFleet;
Ship* lockedShip;

void Engine::lockToShip () // sets camera to ship.
  {
  auto pos = lockedFleet->getPosition();

  focusX = pos.first < screenwidth / 2 ? screenwidth/2 : pos.first; // Make sure it's within bounds.
  focusY = pos.second < screenheight /2 ? screenheight /2 : pos.second;
  focusX = focusX > width - screenwidth / 2 ? width - screenwidth /2 : focusX;
  focusY = focusY > height - screenheight / 2 ? height - screenheight /2 : focusY;
  }

bool Engine::EngineInit()
  {
  int focusX = screenwidth/2;
  int focusY = screenheight/2;

  JSONToItem itemparser;
  itemparser.readItems(ItemDict); // Read items into dictionary.
  JSONToShip shipparser;
  shipparser.readShips(ShipDict);
  JSONToShipPart partparser;
  partparser.readShipParts(ShipPartDict);
  partparser.readCannons(ShipPartDict);

  TheWorld = new World(width, height);

  TheWorld->regen();

  ShipScreen = new TCODConsole(width, height);


  PathScreen = new TCODConsole(width, height);
  PathScreen->clear();
  PathScreen->setKeyColor(TCODColor::magenta);
  PathScreen->setDefaultBackground(TCODColor::magenta);

  mapscreen = new TCODConsole(width, height);
  mapscreen->clear();
  mapscreen->setKeyColor(TCODColor::magenta);
  mapscreen->setDefaultBackground(TCODColor::magenta);

  cityscreen = new TCODConsole(width, height);
  cityscreen->clear();
  cityscreen->setKeyColor(TCODColor::magenta);
  cityscreen->setDefaultBackground(TCODColor::magenta);

  trailsscreen = new TCODConsole(width, height);
  trailsscreen->clear();
  trailsscreen->setKeyColor(TCODColor::magenta);
  trailsscreen->setDefaultBackground(TCODColor::magenta);

  MoistureScreen = new TCODConsole(width, height);

  BiomeScreen = new TCODConsole(width, height);
  BiomeScreen->setKeyColor(TCODColor::magenta);

  Renderer::getMoistureBitmap(MoistureScreen, TheWorld->WorldMap);
  Renderer::getTerrainBitmap(mapscreen, TheWorld->WorldMap);
  Renderer::getCityBitmap(cityscreen, *TheWorld);
  Renderer::getTrailBitmap(trailsscreen, *TheWorld);
  Renderer::getShipBitmap(ShipScreen, *TheWorld);
  Renderer::getBiomeBitmap(BiomeScreen, TheWorld->WorldMap);

  ZOCscreen = new TCODConsole(width, height);
  for (int ycounter = 0; ycounter < height; ycounter++)
  for (int xcounter = 0; xcounter < width; xcounter++)
  if (TheWorld->WorldMap.ref(xcounter, ycounter).isInZOC > 0)
  {
      int faction = TheWorld->WorldMap.ref(xcounter, ycounter).isInZOC;
      auto color = Renderer::findFactionColor(faction);
      ZOCscreen->putCharEx(xcounter, ycounter, 219, color, color);
  }


  tooltip = new TCODConsole(30, 6);

  Fleet& fleet = TheWorld->getPlayerFleet();
  fleet.captain.faction = 1;
  fleet.addMoney(424242);
  fleet.changeShip(ShipDict.getShip("sloop"), 0);
  fleet.changeShip(ShipDict.getShip("clipper_light"), 1);

  fleet.refSailors(0) = 15;
  fleet.refSailors(1) = 14;
  fleet.addRations(500);

  pressedPeriod = true;
  lockedFleet = &(TheWorld->getPlayerFleet());
  lockToShip();


  return true;
  } 


void Engine::Update()
  {
    if (checkBiomeDebug && mouseClick)
    {
        auto& tile = TheWorld->WorldMap.ref(mouseX, mouseY);
        cout << "moisture: " << tile.moisture * 200 << " temp: " << tile.temp << "biome: " << BiomeDict.NameFromEnum(tile.biome) << "\n";        
    }

  if (!newState)
    delete newState;

  if (lockedToShip)
    lockToShip();

  if (redo)
    {
    TheWorld->regen();
#ifdef NDEBUG
    TheWorld->pathfinder->completeFloodfill();
#endif
    ZOCscreen->clear();
    cityscreen->clear();
    trailsscreen->clear();
    mapscreen->clear();
    MoistureScreen->clear();
    BiomeScreen->clear();
    PathScreen->clear();
    lockToShip();
    redo = false;
    Renderer::getTerrainBitmap(mapscreen, TheWorld->WorldMap);
    Renderer::getCityBitmap(cityscreen, *TheWorld);
    Renderer::getTrailBitmap(trailsscreen, *TheWorld);
    Renderer::getMoistureBitmap(MoistureScreen, TheWorld->WorldMap);
    Renderer::getBiomeBitmap(BiomeScreen, TheWorld->WorldMap);

    for (int ycounter = 0; ycounter < height; ycounter++)
      for (int xcounter = 0; xcounter < width; xcounter++)
        if (TheWorld->WorldMap.ref(xcounter, ycounter).isInZOC > 0)
          {
          int faction = TheWorld->WorldMap.ref(xcounter, ycounter).isInZOC;
          auto color = Renderer::findFactionColor(faction);
          ZOCscreen->putCharEx(xcounter, ycounter, 219, color, color);
          }
    }

  Renderer::getTooltip(tooltip, *TheWorld, mouseX, mouseY);

  if (mouseClick)
    {
    mouseClick = false;
    Fleet& refToFleet = TheWorld->getPlayerFleet();
    auto path = TheWorld->pathfinder->path(refToFleet.getPosition(), coord(mouseX, mouseY), refToFleet.getWaveResistance());
    TheWorld->getPlayerFleet().setPath(path);

    PathScreen->clear();
    for (auto iterator = path.begin()+1; iterator < path.end(); iterator++)
      PathScreen->putCharEx(iterator->first, iterator->second, 251, TCODColor::yellow, TCODColor::black);
    }

  if (mouseRightClick)
    {
    mouseRightClick = false;
    auto entityList = TheWorld->entityMap.getEntityList(mouseX, mouseY);
    if (entityList.size() != 0 && entityList.front() != -10)
      {
      Ship& refToShip = TheWorld->shipList[entityList.front()];
      //newState = new State_ShipStatus(refToShip);
      //PushState(newState);
      }
    }

  if (pressedArrow)
    {
      if (TheWorld->getPlayerFleet().path.size() <= 1 || TheWorld->getPlayerFleet().getSpeed() < 1)
      {
      pressedArrow = false;
      }
    if (!pressedPeriod && playerMovement <= 0)
      pressedPeriod = true;
    }
  if (pressedPeriod)
  {
      playerMovement = (int)TheWorld->getPlayerFleet().getMovementCounters();
      TheWorld->getPlayerFleet().step();
      TheWorld->step();
      pressedPeriod = false;
      daysPassed++;
  }
  if (playerMovement > 0)
  {
      TheWorld->getPlayerFleet().updatePos();
      playerMovement--;
      Renderer::getShipBitmap(ShipScreen, *TheWorld);
  }
}

void Engine::Render(TCODConsole *root)
  {
  TCODConsole::blit(mapscreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 1.0f);

  if (showZOC)
      TCODConsole::blit(ZOCscreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 0.8f, 0.0f);
  if (showTrails)
      TCODConsole::blit(trailsscreen, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.5f);
  TCODConsole::blit(cityscreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.5f);
  TCODConsole::blit(PathScreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);
  TCODConsole::blit(ShipScreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);
  if (showMoisture)
      TCODConsole::blit(MoistureScreen, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 0.0f, 1.0f);
  if (showBiomes)
      TCODConsole::blit(BiomeScreen, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, 0, 0, 0.0f, 1.0f);
  TCODConsole::blit(tooltip, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);

  Fleet& refToFleet = TheWorld->getPlayerFleet();
  root->setColorControl(TCOD_COLCTRL_1, TCODColor::grey, TCODColor::black);
  root->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);
  root->setColorControl(TCOD_COLCTRL_3, TCODColor::lighterYellow, TCODColor::black);
  root->setColorControl(TCOD_COLCTRL_4, TCODColor::lightGreen, TCODColor::black);
  root->setColorControl(TCOD_COLCTRL_5, TCODColor::yellow, TCODColor::black);

  TCOD_colctrl_t fatiguecol = (TCOD_colctrl_t)8;
  TCOD_colctrl_t rationcol = (TCOD_colctrl_t)8;
  TCOD_colctrl_t durabilitycol = (TCOD_colctrl_t)8;
  TCOD_colctrl_t sailorcol = (TCOD_colctrl_t)8;

  int fatigue = refToFleet.getFatigue();
  if (fatigue > 900)
    fatiguecol = (TCOD_colctrl_t)2;
  else if (fatigue > 500)
    fatiguecol = (TCOD_colctrl_t)3;
  if (refToFleet.getRations() < 50 * refToFleet.numShips())
    rationcol = (TCOD_colctrl_t)2;

  int durability = refToFleet.getDurability();
  int maxDurability = refToFleet.getMaxDurability();

  if (durability / (double)maxDurability < 0.1)
    durabilitycol = (TCOD_colctrl_t)2;
  else if (durability / (double)maxDurability < 0.5)
    durabilitycol = (TCOD_colctrl_t)3;
  if (!refToFleet.hasEnoughSailors())
    sailorcol = TCOD_COLCTRL_2;

  root->setDefaultForeground(TCODColor::lightestGrey);
  root->print(0, screenheight-2, "Dur.: %c%d%c/%d   Storage %d/%d   Fatigue: %c%.1f%c/100%c   Rations: %c%.1f%c   Est. rations req.: %c%.1f%c", durabilitycol, durability, TCOD_COLCTRL_STOP, maxDurability, refToFleet.getTotalGoods(), refToFleet.getMaxGoods(),
      fatiguecol, fatigue / 10.0f, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, rationcol, refToFleet.getRations() / 10.0f, TCOD_COLCTRL_STOP,
    TCOD_COLCTRL_4, refToFleet.getEstimatedRationsNeeded()/10.0f, TCOD_COLCTRL_STOP);

  root->print(0, screenheight-1, "Day %d   ETA: %c%d%c days   Sailors: %c%d%c(%d)%c/%d   Ducats: %c%d%c", daysPassed, TCOD_COLCTRL_4, refToFleet.getETA(), TCOD_COLCTRL_STOP, 
    sailorcol, refToFleet.getNumSailors(), TCOD_COLCTRL_1, refToFleet.getMinSailors(), TCOD_COLCTRL_STOP, refToFleet.getMaxSailors(),
    TCOD_COLCTRL_5, refToFleet.captain.ducats, TCOD_COLCTRL_STOP);
  }

void Engine::EngineEnd()
  {
  }

//
void Engine::Resize(int new_w,int new_h)
  {
  }

void Engine::WindowActive()
  {
  }

void Engine::WindowInactive()
  {
  }

void Engine::KeyUp(const int &key,const int &unicode)
  {
  }

const int scrollspeed = 2;

ShipSails testsail;

void Engine::KeyDown(const int &key, const int &unicode)
{
    if (key == SDLK_RIGHT)
        focusX = focusX + scrollspeed <= width - screenwidth / 2 ? focusX + scrollspeed : width - screenwidth / 2;
    else if (key == SDLK_LEFT)
        focusX = focusX - scrollspeed >= screenwidth / 2 ? focusX - scrollspeed : screenwidth / 2;
    else if (key == SDLK_UP)
        focusY = focusY - scrollspeed >= screenheight / 2 ? focusY - scrollspeed : screenheight / 2;
    else if (key == SDLK_DOWN)
        focusY = focusY + scrollspeed <= height - screenheight / 2 ? focusY + scrollspeed : height - screenheight / 2;
    else if (key == SDLK_RETURN)
        TheWorld->queryTown(TheWorld->getPlayerFleet());
    else if (key == SDLK_SPACE)
    {
        auto pos = TheWorld->getPlayerFleet().getPosition();
        auto shipTokenList = TheWorld->entityMap.getEntityList(pos);
        vector<Ship*> shipPointers;
        for (auto it = shipTokenList.begin(); it < shipTokenList.end(); it++)
        {
            shipPointers.push_back(&TheWorld->shipList.at(*it));
        }

        newState = new State_Combat(TheWorld->getPlayerFleet(), TheWorld->WorldMap.altitudeSeed, TheWorld->WorldMap.moistureSeed, pos.first, pos.second);
        PushState(newState);
    }

    switch (unicode)
    {
    case '>':
        pressedArrow = true;
        break;

    case '.':
        pressedPeriod = true;
        break;

    case 'b':
        checkBiomeDebug = !checkBiomeDebug;
        break;

    case 'B':
        showBiomes = !showBiomes;
        break;

    case 'k':
        newState = new State_ShowSkills(TheWorld->getPlayerFleet().captain);
        PushState(newState);
        break;

    case 'm':
        showMoisture = !showMoisture;
        break;

    case 'R': //Test button to give player 100k money
        TheWorld->getPlayerFleet().captain.ducats += 100000;
        break;

    case 'S':
        newState = new State_ShipStatus(TheWorld->getPlayerFleet());
        PushState(newState);
        break;

    case 's': // Check for city.
        if (TheWorld->queryTown(TheWorld->getPlayerFleet()))
        {
            daysPassed = 0;
            newState = new State_TownMenu(TheWorld->getTown(TheWorld->getPlayerFleet()), TheWorld->getPlayerFleet());
            PushState(newState);
        }
        break;

    case 'T':
        testsail = ShipPartDict.getSail(string("sail_maintoproyal"));
        TheWorld->getPlayerFleet().captain.sailInventory.push_back(testsail);
        break;

    case 't':
        showTrails = !showTrails;
        break;

    case 'Y':
        if (!lockedToShip)
        {
            auto entityList = TheWorld->entityMap.getEntityList(mouseX, mouseY);
            if (entityList.size()) // if there is a ship on the tile, and we're not following a ship
                lockedShip = &(TheWorld->shipList[entityList.front()]);
            else
                lockedFleet = &(TheWorld->getPlayerFleet());
        }

        lockedToShip = !lockedToShip;
        break;

    case 'z':
        showZOC = !showZOC;
        break;

    default: break;
    }
}

void Engine::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
  {
  mouseX = focusX + (iX / 12 - screenwidth/2);
  mouseY = focusY + (iY / 12 - screenheight/2);
  }

void Engine::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
  {

  }

void Engine::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
  {
  if (iButton == SDL_BUTTON_LEFT)
    mouseClick = true;
  if (iButton == SDL_BUTTON_RIGHT)
    mouseRightClick = true;
  }

