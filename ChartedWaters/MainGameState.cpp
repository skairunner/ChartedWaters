#include "MainGameState.h"
#include <SDL.h>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

MainGameState::MainGameState(const int& w, const int& h, std::vector<GameState*>* pointerToStack, const int debugCounter)
  : width (w), height (h), GameState(pointerToStack), debugcounter(debugCounter)
  {
  debugcounter = debugCounter;
  }

MainGameState::~MainGameState()
  {
  delete console;
  }

bool MainGameState::Init()
  {
  console = new TCODConsole(width, height);
  console->setDefaultForeground(TCODColor::white);
  return true;
  }
void MainGameState::Update()
  {
  string output("<ingame, current state ");
  output.append(to_string((long long)debugcounter));
  output.append(" >");
   console->print(0, debugcounter, output.c_str());
  }

void MainGameState::Render(TCODConsole *root)
  {
  TCODConsole::blit(console, 0, 0, 0, 0, root, 0, 0, 1, 1);
  }
void MainGameState::End(){}
  //
void MainGameState::Resize(int new_w,int new_h){}
void MainGameState::WindowActive(){}
void MainGameState::WindowInactive(){}
void MainGameState::KeyUp(const int &key,const int &unicode){}

void MainGameState::KeyDown(const int &key,const int &unicode)
  {
  if (key == SDLK_ESCAPE)
    popMe = true;
  if (key == SDLK_1)
    {
    cout << " " << 0 << " ";
    nextState = new MainGameState(width, height, stateStack, 0);
    pushSomething = true;
    }
  }

void MainGameState::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void MainGameState::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void MainGameState::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}