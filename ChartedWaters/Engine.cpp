#include "Engine.h"
#include "TCODTools.h"
#include "MainGameState.h"
#include "A_star_test.h"

Engine CursesEngine;

TCODConsole *console;
TCODImage *pic;

int counter = 0;
int timer = 0;
int mouseX, mouseY; // for clicks
int mouseMovedX, mouseMovedY; // for movements
bool mouseLeft, mouseRight;
bool changeStart, changeEnd;
pair<int, int> start(0,0);
pair<int, int> eend(1,1);

/////// Astar testing
TCODConsole *screen;
TCODConsole *pathscreen;
Map* Amap;

bool Engine::EngineInit()
{
mouseLeft = mouseRight = false;
mouseX = mouseY = 0;

const int ww = 10;
const int hh = 10;
screen = new TCODConsole(ww, hh);
pathscreen = new TCODConsole(ww, hh);

Amap = new Map(ww, hh);
pathscreen->clear();
for (int ycounter = 0; ycounter < hh; ycounter++)
  for (int xcounter = 0; xcounter < ww; xcounter++)
    {
    if (Amap->ref(xcounter, ycounter).accessible)
      screen->putCharEx(xcounter, ycounter, 178, TCODColor::blue, TCODColor::lightBlue);
    else
      screen->putCharEx(xcounter, ycounter, 178, TCODColor::black, TCODColor::lightBlue);
    }
  

  /*console = new TCODConsole(64, 40);
  console->setDefaultBackground(TCODColor((int) 255, 255, 255));
  console->setDefaultForeground(TCODColor((int) 0, 0, 0));
  pic = new TCODImage("resources/port.png");
  const int zoom = 8;
  for (int ycounter = 0; ycounter < consoleHeight; ycounter++)
    for (int xcounter = 0; xcounter < consoleWidth; xcounter++)
      {
      console->setCharBackground(xcounter, ycounter, pic->getMipmapPixel(zoom * xcounter, zoom * ycounter, zoom * (xcounter + 1), zoom * (ycounter+1)));
      }
  console->print(0, 0, "Charted Waters");*/

	return true;
}
void Engine::Update()
{
//console->putCharEx(1, 1, counter, TCODColor::white, TCODColor::black);

////////
//////// This is for updating screen
////////

bool update = false;
if (mouseLeft)
  {
  Amap->ref(mouseX, mouseY).accessible = true;
  update = true;
  }
else if (mouseRight)
  {
  Amap->ref(mouseX, mouseY).accessible = false;
  update = true;
  }

if (update)
  {
  if (Amap->ref(mouseX, mouseY).accessible)
      screen->putCharEx(mouseX, mouseY, 178, TCODColor::blue, TCODColor::lightBlue);
    else
      screen->putCharEx(mouseX, mouseY, 178, TCODColor::black, TCODColor::lightBlue);
  update = false;
  }

////////
////////  This is for updating the path.
////////

if (changeStart)
  {
  start.first = mouseMovedX;
  start.second = mouseMovedY;
  }
else if (changeEnd)
  {
  eend.first = mouseMovedX;
  eend.second = mouseMovedY;
  }
if (changeStart || changeEnd)
  {
  auto it = Amap->pathfind(start.first, start.second, eend.first, eend.second);
  pathscreen->clear();
  for (auto path = it.begin(); path < it.end(); path++)
    {
    pathscreen->putCharEx(path->first, path->second, 251, TCODColor::yellow, TCODColor::orange);
    }
  pathscreen->putCharEx(start.first, start.second, 64, TCODColor::gold, TCODColor::orange);
  pathscreen->putCharEx(eend.first, eend.second, 67, TCODColor::darkRed, TCODColor::red);
  changeStart = changeEnd = false;
  }
}
void Engine::Render(TCODConsole *root)
{
root->setKeyColor(TCODColor::black);
//TCODConsole::blit(console, 0, 0, 0, 0, root, 0, 0, 1, 1);
TCODConsole::blit(screen, 0, 0, 0, 0, root, 0, 0, 1, 1);
TCODConsole::blit(pathscreen, 0, 0, 0, 0, root, 0, 0, 1, 0);
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
if (key == SDLK_1)
  {
  MainGameState* maingame = new MainGameState(consoleWidth, consoleHeight, pointerToStack, 0);
  PushState(maingame);
  }
else if (key == SDLK_a)
  {
  changeStart = true;
  }
else if (key == SDLK_b)
  {
  changeEnd = true;
  }
}
void Engine::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
mouseMovedX = iX / 12;
mouseMovedY = iY / 12;
}

void Engine::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
if (iButton == SDL_BUTTON_LEFT)
  mouseLeft = false;
else if (iButton == SDL_BUTTON_RIGHT)
  mouseRight = false;

}

void Engine::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY)
{
if (iButton == SDL_BUTTON_LEFT)
  {
  mouseLeft = true;
  mouseX = iX / 12;
  mouseY = iY / 12;
  }
else if (iButton == SDL_BUTTON_RIGHT)
  {
  mouseRight = true;
  mouseX = iX / 12;
  mouseY = iY / 12;
  }
}
