#pragma once
#include "GameState.h"
#include "town.h"
#include "Ship.h"
#include <libtcod.h>

class State_Shop : public GameState
  {
  public:
    State_Shop(Town& town, Ship& ship);
    bool Init();
    void Render(TCODConsole *root);
    void Update();
    void End();
    virtual void KeyUp(const int &key,const int &unicode){}
    virtual void KeyDown(const int &key,const int &unicode);
    virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}

  private:
    TCODConsole* consoleLeft; // ship
    TCODConsole* consoleRight; // shop
    bool whichConsole; // false: left, true: right
    bool redraw;
    int selector;
    Town& refToTown;
    Ship& refToShip;
    std::vector<EconomyItemTuple> goods;
    void redrawLeft();
    void redrawRight();
    void invertLine(const int& line, TCODConsole* console);
  };