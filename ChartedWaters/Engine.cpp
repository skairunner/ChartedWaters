#include "Engine.h"
#include "TCODTools.h"
#include "worldMap.h"

Engine CursesEngine;

WorldMapClass* worldmap;
TCODConsole* mapscreen;
TCODColor sandBrown(193, 177, 127);
bool redo = false;
double thresholdMoisture = 0;

bool Engine::EngineInit()
{
worldmap = new WorldMapClass(128, 64);
worldmap->gen();
mapscreen = new TCODConsole(128, 64);

for (int ycounter = 0; ycounter < worldmap->getHeight(); ycounter++)
  for (int xcounter = 0; xcounter < worldmap->getWidth(); xcounter++)
    {
    auto it = worldmap->ref(xcounter, ycounter);
    if (it.altitude > 0)
      {
      TCODColor fore = TCODColor::green;
      if (it.moisture < thresholdMoisture)
        fore = sandBrown;
      mapscreen->putCharEx(xcounter, ycounter, 219, TCODColor::lerp(fore, TCODColor::black, it.altitude/40.0f), TCODColor::black);
      }
    else
      mapscreen->putCharEx(xcounter, ycounter, 178, TCODColor::blue, TCODColor::darkBlue);
    }

	return true;
}
void Engine::Update()
{
if (redo)
  {
  worldmap->gen();
  redo = false;
  for (int ycounter = 0; ycounter < worldmap->getHeight(); ycounter++)
  for (int xcounter = 0; xcounter < worldmap->getWidth(); xcounter++)
    {
    auto it = worldmap->ref(xcounter, ycounter);
    if (it.altitude > 0)
      {
      TCODColor fore = TCODColor::green;
      if (it.moisture < thresholdMoisture)
        fore = sandBrown;
      mapscreen->putCharEx(xcounter, ycounter, 219, TCODColor::lerp(fore, TCODColor::black, it.altitude/40.0f), TCODColor::black);
      }
    else
      mapscreen->putCharEx(xcounter, ycounter, 178, TCODColor::blue, TCODColor::darkBlue);
    }
  }
}
void Engine::Render(TCODConsole *root)
{
//root->setKeyColor(TCODColor::black);
TCODConsole::blit(mapscreen, 0, 0, 0, 0, root, 0, 0, 1.0f, 1.0f);
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
