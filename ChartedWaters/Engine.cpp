#include "Engine.h"
#include "TCODTools.h"
#include "World.h"
#include "State_ShipStatus.h"
#include <string>
#include "State_shop.h"
#include "State_MainGame.h"
#include "shipPrototype.h"

Engine CursesEngine;

int mouseX, mouseY;

const int width = 256;
const int height = 256;

const int screenwidth = 100;
const int screenheight = 48;
int focusX = screenwidth/2;
int focusY = screenheight/2;

int playerMovement = 0;

World* TheWorld;
TCODConsole* cityscreen;
TCODConsole* mapscreen;
TCODConsole* ZOCscreen;
TCODConsole* tooltip;
TCODConsole* PathScreen;
TCODConsole* AccessibleScreen;
TCODConsole* ShipScreen;

bool redo = true;
typedef pair<int, int> coord;
GameState* newState;
bool mouseClick = false;
bool pressedPeriod = false;
bool lockedToShip = false;

void lockToShip () // sets camera to ship.
  {
  auto pos = TheWorld->getPlayerShip().getPosition();
  focusX = pos.first < screenwidth / 2 ? screenwidth/2 : pos.first; // Make sure it's within bounds.
  focusY = pos.second < screenheight /2 ? screenheight /2 : pos.second;
  focusX = focusX > width - screenwidth / 2 ? width - screenwidth /2 : focusX;
  focusY = focusY > height - screenheight / 2 ? height - screenheight /2 : focusY;
  }

bool Engine::EngineInit()
{
JSONToItem jsonParser;
jsonParser.readItems(ItemDict); // Read items into dictionary.
JSONToShip parser;
parser.readShips(ShipDict);

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

AccessibleScreen = new TCODConsole(width, height);
Renderer::getAccessBitmap(AccessibleScreen, TheWorld->pathfinder->map);

Renderer::getTerrainBitmap(mapscreen, *TheWorld);
Renderer::getCityBitmap(cityscreen, *TheWorld);
Renderer::getAccessBitmap(AccessibleScreen, TheWorld->pathfinder->map);
Renderer::getShipBitmap(ShipScreen, *TheWorld);



ZOCscreen = new TCODConsole(width, height);
tooltip = new TCODConsole(30, 1);

Ship& ship = TheWorld->getPlayerShip();
ship.faction = 1;
ship.addMoney(65536);
ship.changeShip(ShipDict.getShip(string("sloop")));
pressedPeriod = true;
lockToShip();

	return true;
} 
void Engine::Update()
{
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
  mapscreen->clear();
  AccessibleScreen->clear();
  PathScreen->clear();
  lockToShip();
  redo = false;
  Renderer::getTerrainBitmap(mapscreen, *TheWorld);
  Renderer::getCityBitmap(cityscreen, *TheWorld);
  Renderer::getAccessBitmap(AccessibleScreen, TheWorld->pathfinder->map);
  for (int ycounter = 0; ycounter < height; ycounter++)
    for (int xcounter = 0; xcounter < width; xcounter++)
      if (TheWorld->WorldMap.ref(xcounter, ycounter).isInZOC > 0)
        {
        int faction = TheWorld->WorldMap.ref(xcounter, ycounter).isInZOC;
        auto color = Renderer::findFactionColor(faction);
        ZOCscreen->putCharEx(xcounter, ycounter, 219, color, color);
        }
  }

std::string name = Renderer::findCityName(coord(mouseX, mouseY), *TheWorld); 
tooltip->clear();
tooltip->print(0, 0, name.c_str());

if (mouseClick)
  {
  mouseClick = false;
  auto it = TheWorld->pathfinder->path(TheWorld->getPlayerShip().getPosition(), coord(mouseX, mouseY), 6);
  TheWorld->getPlayerShip().setPath(it);
  PathScreen->clear();
  for (auto iterator = it.begin()+1; iterator < it.end(); iterator++)
    PathScreen->putCharEx(iterator->first, iterator->second, 251, TCODColor::yellow, TCODColor::black);
  }
if (pressedPeriod)
  {
  playerMovement = TheWorld->getPlayerShip().getSpeed();
  TheWorld->step();
  pressedPeriod = false;
  }
if (playerMovement > 0)
  {
  TheWorld->getPlayerShip().updatePos();
  playerMovement--;
  Renderer::getShipBitmap(ShipScreen, *TheWorld);
  }

// Write the tooltip!
//tooltip->clear();
//tooltip->print(0, 0, to_string((long double)worldmap->ref(mouseX, mouseY).owner).c_str());
}

void Engine::Render(TCODConsole *root)
{
root->setKeyColor(TCODColor::magenta);
TCODConsole::blit(mapscreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 1.0f);
//TCODConsole::blit(AccessibleScreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);

//TCODConsole::blit(ZOCscreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 0.8f, 0.0f);
//TCODConsole::blit(TheWorld->pathfinder->console, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);
TCODConsole::blit(cityscreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.5f);
TCODConsole::blit(tooltip, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);

TCODConsole::blit(PathScreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);
TCODConsole::blit(ShipScreen, focusX - screenwidth/2, focusY - screenheight/2, screenwidth, screenheight, root, 0, 0, 1.0f, 0.0f);
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

void Engine::KeyDown(const int &key,const int &unicode)
{
if (key == SDLK_RIGHT)
  focusX = focusX + scrollspeed <= width - screenwidth/2 ? focusX + scrollspeed : width - screenwidth /2;
else if (key == SDLK_LEFT)
  focusX = focusX - scrollspeed >= screenwidth /2 ? focusX - scrollspeed : screenwidth / 2;
else if (key == SDLK_UP)
  focusY = focusY - scrollspeed >= screenheight/2 ? focusY - scrollspeed : screenheight/2;
else if (key == SDLK_DOWN)
  focusY = focusY + scrollspeed <= height - screenheight/2 ? focusY + scrollspeed : height - screenheight /2;
else if (key == SDLK_RETURN)
  TheWorld->queryShop(TheWorld->getPlayerShip());

switch (unicode)
  {
case 'S':
  newState = new State_ShipStatus(TheWorld->getPlayerShip());
  PushState(newState);
  break;

case '.':
  pressedPeriod = true;
  break;

case 'T':  // Test shop
  newState = new State_Shop(TheWorld->getFirstTown(), TheWorld->getPlayerShip());
  PushState(newState);
  break;

case 's': // Check for shop.
  if (TheWorld->queryShop(TheWorld->getPlayerShip()))
    {
    newState = new State_Shop(TheWorld->getTown(TheWorld->getPlayerShip()), TheWorld->getPlayerShip());
    PushState(newState);
    }
    break;
    
case 'Y':
  lockedToShip = !lockedToShip;
  break;

case 'R': //Test button to spawn items in shops.
  for (auto it = TheWorld->cityList.begin(); it != TheWorld->cityList.end(); it++)
    it->second.spawnItems();
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

}
