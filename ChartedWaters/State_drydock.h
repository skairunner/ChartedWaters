#pragma once
#include "GameState.h"
#include "town.h"
#include "Ship.h"
#include <libtcod.h>

class State_Drydocks : public GameState
  {
  public:
    State_Drydocks(Town& town, Ship& ship);
    bool Init();
    void Render(TCODConsole *root);
    void Update();
    void End();
    virtual void KeyUp(const int &key,const int &unicode){}
    virtual void KeyDown(const int &key,const int &unicode);
    virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
    virtual void RecoverFromPush();

  private:
    void updateDrydocks();
    void keydownDrydocks(const int &key,const int &unicode);

    std::string assembleOutput(const LedgerItemTuple& tuple);
    std::string assembleOutput(const EconomyItemTuple& tuple);
    std::string assembleOutput(const ShipPrototype& sp);
    void swapLineColors(TCODConsole* con, const int& counter);

    TCODConsole* consoleLeft; // ship
    TCODConsole* consoleRight; // shop
    bool redraw; bool swappedToShop;
    bool startbuy; // Start the buying process! Get a number.
    bool startsell;
    bool calculatebuy; // Now see if the player can buy/sell the amount he wants to.
    bool calculatesell;
    int selector;

    int numberToTrade;
    std::string itemIDToTrade;
    bool throwawayBool;

    std::string promptResult;
    bool yesNo; // result from a y/n prompt
    bool getPrompt; // Fetch a result from a y/n prompt.
    bool isHometown;

    Town& refToTown;
    Ship& refToShip;
    std::vector<EconomyItemTuple> goods;
    std::vector<LedgerItemTuple> inventory;
    void redrawLeft();
    void redrawRight();
    void invertLine(const int& line, TCODConsole* console);
  };