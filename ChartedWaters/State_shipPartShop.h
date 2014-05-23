#pragma once
#include "GameState.h"
#include "Ship.h"
#include "town.h"
#include <string>
#include <libtcod.hpp>

class State_shipPartShop : public GameState
  {
  public:
    State_shipPartShop(Town* town = 0, Player* player = 0);
        
    virtual bool Init();

    virtual void Update();
    virtual void Render(TCODConsole *root);
    virtual void End();
    //
    virtual void Resize(int new_w,int new_h){}
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    virtual void KeyUp(const int &key,const int &unicode){}
    virtual void KeyDown(const int &key,const int &unicode);
    virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}

  protected:
    void invertLine(const int& line, TCODConsole* console);
    virtual std::string shopHeader();
    void redrawLeft(std::vector<ShipPart*>& partList);
    void redrawRight(ShipPart* part = 0);

    void swapLineColors(TCODConsole* con, const int& counter);

    void getProfile(ShipArmor& item, int& line);
    void getProfile(ShipSails& item, int& line);
    void getProfile(ShipStatue& item, int& line);
    void getProfile(ShipCannons& item, int& line);

    bool redraw;
    bool result;
    bool getSomething;

    Town* pToTown;
    Player* pToPlayer;

    TCODConsole* consoleLeft, *consoleRight;
    int selector;
    bool attemptBuy;
  };