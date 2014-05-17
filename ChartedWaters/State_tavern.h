#pragma once
#include "GameState.h"
#include "Fleet.h"
#include <libtcod.hpp>

class State_Tavern: public GameState
  {
  public:
    State_Tavern::State_Tavern(Fleet& fleet);
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
      Fleet& refToFleet;


    TCODConsole* console;
    void drawMenu();
  };

class State_buySailors: public GameState
  {
  public:
    State_buySailors(Fleet& fleet);

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

    Fleet& refToFleet;
    std::vector<int> pages;
    int page;
    int pageit; // the position in the pages vector that yields page

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
    State_buyRations(Fleet& fleet);
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
    bool buy;
    bool sell;
    bool daysworth;
    bool getSomething;

    Fleet& refToFleet;
    std::vector<int> pages;
    int page;
    int pageit; // the position in the pages vector that yields page

    TCODConsole* console;
  };

class State_recoverFatigue: public GameState
  {
  public:
    State_recoverFatigue(Fleet& fleet);
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
    int getCost(const int& rec);
    std::string prompt;
    int recover;
    bool yesno;
    bool getSomething;

    Fleet& refToFleet;
    std::vector<int> pages;
    int page;
    int pageit; // the position in the pages vector that yields page

    TCODConsole* console;
  };