#include "Engine.h"
#include "TCODTools.h"
#include "World.h"
#include "State_ShipStatus.h"
#include <string>
#include "State_shop.h"

Engine CursesEngine;

int mouseX, mouseY;

World TheWorld(128,64);
TCODConsole* cityscreen;
TCODConsole* mapscreen;
TCODConsole* ZOCscreen;
TCODConsole* tooltip;
TCODConsole* PlayerShip;
TCODConsole* PathScreen;
TCODConsole* AccessibleScreen;

bool redo = true;
typedef pair<int, int> coord;
GameState* newState;
bool mouseClick = false;
bool pressedPeriod = false;

bool Engine::EngineInit()
{
JSONToItem jsonParser;
jsonParser.readItems(ItemDict); // Read items into dictionary.

TheWorld.regen();
TheWorld.regen();

TheWorld.getPlayerShip().addItem(Item("food_friedchicken"), 50, ItemDict.findBasePrice(string("food_friedchicken")));
TheWorld.getPlayerShip().addItem(Item("food_fruityloops"), 50, ItemDict.findBasePrice(string("food_fruityloops")));
TheWorld.getPlayerShip().addItem(Item("luxury_carbonnano"), 7, 7391);

PlayerShip = new TCODConsole(1, 1);
auto shippos = TheWorld.getPlayerShip().getPosition();
PlayerShip->putCharEx(0, 0, TheWorld.getPlayerShip().character, TCODColor::red, TCODColor::black);

PathScreen = new TCODConsole(128, 64);
PathScreen->clear();
PathScreen->setKeyColor(TCODColor::magenta);
PathScreen->setDefaultBackground(TCODColor::magenta);

mapscreen = new TCODConsole(128, 64);
mapscreen->clear();
mapscreen->setKeyColor(TCODColor::magenta);
mapscreen->setDefaultBackground(TCODColor::magenta);

cityscreen = new TCODConsole(128, 64);
cityscreen->clear();
cityscreen->setKeyColor(TCODColor::magenta);
cityscreen->setDefaultBackground(TCODColor::magenta);

AccessibleScreen = new TCODConsole(128,64);

Renderer::getTerrainBitmap(mapscreen, TheWorld);
Renderer::getCityBitmap(cityscreen, TheWorld);
Renderer::getAccessBitmap(AccessibleScreen, TheWorld.pathfinder->map);

ZOCscreen = new TCODConsole(128, 64);
tooltip = new TCODConsole(30, 1);

Ship& ship = TheWorld.getPlayerShip();
ship.addMoney(13370);


	return true;
}
void Engine::Update()
{
if (!newState)
  delete newState;

if (redo)
  {
  TheWorld.regen();
  ZOCscreen->clear();
  cityscreen->clear();
  mapscreen->clear();
  AccessibleScreen->clear();
  PathScreen->clear();
  redo = false;
  Renderer::getTerrainBitmap(mapscreen, TheWorld);
  Renderer::getCityBitmap(cityscreen, TheWorld);
  Renderer::getAccessBitmap(AccessibleScreen, TheWorld.pathfinder->map);
  }

std::string name = Renderer::findCityName(coord(mouseX, mouseY), TheWorld);
if (name.size() > 0)
  name.front() -= 32; 
tooltip->clear();
tooltip->print(0, 0, name.c_str());

if (mouseClick)
  {
  mouseClick = false;
  auto it = TheWorld.pathfinder->path(TheWorld.getPlayerShip().getPosition(), coord(mouseX, mouseY), 6);
  TheWorld.getPlayerShip().setPath(it);
  PathScreen->clear();
  for (auto iterator = it.begin(); iterator < it.end()-1; iterator++)
    PathScreen->putCharEx(iterator->first, iterator->second, 251, TCODColor::yellow, TCODColor::black);
  }
if (pressedPeriod)
  {
  TheWorld.getPlayerShip().updatePos();
  pressedPeriod = false;
  }

// Write the tooltip!
//tooltip->clear();
//tooltip->print(0, 0, to_string((long double)worldmap->ref(mouseX, mouseY).owner).c_str());
}

void Engine::Render(TCODConsole *root)
{
root->setKeyColor(TCODColor::magenta);
TCODConsole::blit(mapscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 1.0f);
//TCODConsole::blit(ZOCscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
TCODConsole::blit(cityscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.5f);
TCODConsole::blit(tooltip, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
coord playerpos = TheWorld.getPlayerShip().getPosition();
TCODConsole::blit(AccessibleScreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
TCODConsole::blit(PathScreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
TCODConsole::blit(PlayerShip, 0, 0, 0, 0, root, playerpos.first, playerpos.second, 1.0f, 0.0f);
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

void Engine::KeyDown(const int &key,const int &unicode)
{
if (key == SDLK_RIGHT)
  redo = true;
else if (key == SDLK_s)
  {
  newState = new State_ShipStatus(TheWorld.getPlayerShip());
  PushState(newState);
  }
else if (unicode == '.')
  {
  pressedPeriod = true;
  }
else if (key == SDLK_RETURN)
  {
  TheWorld.queryShop(TheWorld.getPlayerShip());
  }
else if (unicode == 'T') // Test shop
  {
  newState = new State_Shop(TheWorld.getFirstTown(), TheWorld.getPlayerShip());
  PushState(newState);
  }
}

void Engine::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
mouseX = iX / 12;
mouseY = iY / 12;
}

void Engine::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{

}

void Engine::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
if (iButton == SDL_BUTTON_LEFT)
  mouseClick = true;

}
