#pragma once
#include "GameState.h"
#include "Ship.h"
#include <libtcod.hpp>

class State_Tavern: public GameState
  {
  public:
    State_Tavern::State_Tavern(Ship& ship);
    virtual bool Init();

    virtual void Update(){}
    virtual void Render(TCODConsole *root);
    virtual void End();
    virtual void RecoverFromPush() {if (!nextState) delete nextState; pushSomething = false;}
    //
    virtual void Resize(int new_w,int new_h){}
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    virtual void KeyUp(const int &key,const int &unicode){}
    virtual void KeyDown(const int &key,const int &unicode);
    virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}

  private:
    Ship& refToShip;
    TCODConsole* console;
    void drawMenu();
  };

class State_buySailors: public GameState
  {
  public:
    State_buySailors(Ship& ship);

    virtual bool Init();

    virtual void Update();
    virtual void Render(TCODConsole *root);
    virtual void End();
    virtual void RecoverFromPush() {if (!nextState) delete nextState; pushSomething = false;}
    //
    virtual void Resize(int new_w,int new_h){}
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    virtual void KeyUp(const int &key,const int &unicode){}
    virtual void KeyDown(const int &key,const int &unicode);
    virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
  private:
    void drawMenu();
    std::string prompt;
    bool yesno;
    bool getSomething;
    bool master;
    Ship& refToShip;
    TCODConsole* console;
    int price;
    bool green;
    bool sell;
    bool exp;
    bool buy;
  };

class State_buyRations: public GameState
  {
  public:
    State_buyRations(Ship& ship);
    virtual bool Init(){return true;}

    virtual void Update(){}
    virtual void Render(TCODConsole *root){}
    virtual void End(){}
    virtual void RecoverFromPush() {if (!nextState) delete nextState; pushSomething = false;}
    //
    virtual void Resize(int new_w,int new_h){}
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    virtual void KeyUp(const int &key,const int &unicode){}
    virtual void KeyDown(const int &key,const int &unicode);
    virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
  };