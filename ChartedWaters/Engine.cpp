#include "Engine.h"
#include "TCODTools.h"
#include "MainGameState.h"

Engine CursesEngine;

TCODConsole *console;
TCODImage *pic;

int counter = 0;
int timer = 0;

bool Engine::EngineInit()
{
  console = new TCODConsole(64, 40);
  console->setDefaultBackground(TCODColor((int) 255, 255, 255));
  console->setDefaultForeground(TCODColor((int) 0, 0, 0));
  pic = new TCODImage("resources/port.png");
  const int zoom = 8;
  for (int ycounter = 0; ycounter < consoleHeight; ycounter++)
    for (int xcounter = 0; xcounter < consoleWidth; xcounter++)
      {
      console->setCharBackground(xcounter, ycounter, pic->getMipmapPixel(zoom * xcounter, zoom * ycounter, zoom * (xcounter + 1), zoom * (ycounter+1)));
      }
  console->print(0, 0, "Charted Waters");

	return true;
}
void Engine::Update()
{
console->putCharEx(1, 1, counter, TCODColor::white, TCODColor::black);
}
void Engine::Render(TCODConsole *root)
{
TCODConsole::blit(console, 0, 0, 0, 0, root, 0, 0, 1, 1);
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