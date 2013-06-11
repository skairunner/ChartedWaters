#include "Engine.h"
#include "TCODTools.h"
#include "worldMap.h"

Engine CursesEngine;

WorldMapClass* worldmap;
TCODConsole* mapscreen;
TCODColor sandBrown(193, 177, 127);
TCODColor coastalSand(231,194,141);
TCODConsole* cityscreen;
bool redo = false;
double thresholdMoisture = 0;

TCODColor findFactionColor(const int& faction)
  {
  switch(faction) // Choose the city color
    {
  case 1: return TCODColor::red; break;
  case 2: return TCODColor::sky; break;
  case 3: return TCODColor::orange; break;
  case 4: return TCODColor::black; break;
  case 5: return TCODColor::darkGrey; break;
  case 6: return TCODColor::azure; break;
  case 7: return TCODColor::darkChartreuse; break;
  default: return TCODColor::white; break;
    }

  }

void drawMap(TCODConsole* map, TCODConsole* cities, WorldMapClass* wm)
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
          fore = sandBrown;
        if (it.isCoastal)
          {
          character = 178;
          fore = coastalSand;
          back = coastalSand;
          }
        if (it.isInZOC)
          fore = findFactionColor(it.isInZOC);

        map->putCharEx(xcounter, ycounter, character, TCODColor::lerp(fore, TCODColor::black, it.altitude/40.0f), back);
        }
      else
        map->putCharEx(xcounter, ycounter, 178, TCODColor::lerp(TCODColor::blue, TCODColor::black, -1 * it.altitude / 40.0f), TCODColor::darkBlue);
      }
    for (auto it = wm->cities.begin(); it < wm->cities.end(); it++)
      {
      TCODColor citycolor = findFactionColor(wm->ref(it->first, it->second).isInZOC);
      cities->putCharEx(it->first, it->second, 99, citycolor, TCODColor::darkBlue);
      }
  }

bool Engine::EngineInit()
{
worldmap = new WorldMapClass(128, 64);
worldmap->gen();
mapscreen = new TCODConsole(128, 64);
cityscreen = new TCODConsole(128, 64);

drawMap(mapscreen, cityscreen, worldmap);

	return true;
}
void Engine::Update()
{
if (redo)
  {
  worldmap->gen();
  cityscreen->clear();
  redo = false;
  drawMap(mapscreen, cityscreen, worldmap);
  }
}

void Engine::Render(TCODConsole *root)
{
//root->setKeyColor(TCODColor::black);
TCODConsole::blit(mapscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 1.0f);
TCODConsole::blit(cityscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.0f);
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

}

void Engine::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{

}

void Engine::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
  }
