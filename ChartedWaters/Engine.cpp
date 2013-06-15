#include "Engine.h"
#include "TCODTools.h"
#include "World.h"
#include <string>

Engine CursesEngine;

int mouseX, mouseY;

World TheWorld(128,64);
TCODConsole* cityscreen;
TCODConsole* mapscreen;
TCODConsole* ZOCscreen;
TCODConsole* tooltip;
bool redo = true;
typedef pair<int, int> coord;

/*
void drawMap(TCODConsole* map, TCODConsole* cities, TCODConsole* ZOC, WorldMapClass* wm)
  {
  for (int ycounter = 0; ycounter < wm->getHeight(); ycounter++)
    for (int xcounter = 0; xcounter < wm->getWidth(); xcounter++)
      {
      int character = 219;
      auto it = wm->ref(xcounter, ycounter);
      if (it.altitude > 0)
        {
        TCODColor fore = TCODColor::green;
        TCODColor back = TCODColor::black;
        if (it.moisture < thresholdMoisture)
          {
          fore = sandBrown;
          }
        if (it.isCoastal)
          {
          character = 178;
          fore = coastalSand;
          back = TCODColor::black;
          }
        else
          back = fore;
        if (it.isInZOC)
          {
          ZOC->putCharEx(xcounter, ycounter, it.isInZOC + 48, findFactionColor(it.isInZOC), TCODColor::white);
          //character = it.isInZOC + 48;
          }
        


        map->putCharEx(xcounter, ycounter, character, TCODColor::lerp(fore, TCODColor::black, it.altitude/40.0f), back);
        }
      else
        map->putCharEx(xcounter, ycounter, 178, TCODColor::lerp(TCODColor::blue, TCODColor::black, -1 * it.altitude / 40.0f), TCODColor::darkBlue);
      }
    for (auto it = wm->cities.begin(); it < wm->cities.end(); it++)
      {
      TCODColor citycolor = findFactionColor(wm->ref(it->first, it->second).isInZOC);
      cities->putCharEx(it->first % wm->getWidth(), it->second, 99, citycolor, TCODColor::black);
      }
  }
  */

bool Engine::EngineInit()
{
TheWorld.regen();
TheWorld.regen();

mapscreen = new TCODConsole(128, 64);
mapscreen->clear();
mapscreen->setKeyColor(TCODColor::magenta);
mapscreen->setDefaultBackground(TCODColor::magenta);

cityscreen = new TCODConsole(128, 64);
cityscreen->clear();
cityscreen->setKeyColor(TCODColor::magenta);
cityscreen->setDefaultBackground(TCODColor::magenta);

Renderer::getTerrainBitmap(mapscreen, TheWorld);
Renderer::getCityBitmap(cityscreen, TheWorld);

ZOCscreen = new TCODConsole(128, 64);
tooltip = new TCODConsole(30, 1);


	return true;
}
void Engine::Update()
{
if (redo)
  {
  TheWorld.regen();
  ZOCscreen->clear();
  cityscreen->clear();
  mapscreen->clear();
  redo = false;
  Renderer::getTerrainBitmap(mapscreen, TheWorld);
  Renderer::getCityBitmap(cityscreen, TheWorld);
  }

std::string name = Renderer::findCityName(coord(mouseX, mouseY), TheWorld);
if (name.size() > 0)
  name.front() -= 32; 
tooltip->clear();
tooltip->print(0, 0, name.c_str());
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
  }
