#pragma once
#include "GameState.h"
#include "Fleet.h"
#include "goods.h"

class State_ShipStatus : public GameState
  // Displays ship stuff.
  {
  public:
    State_ShipStatus(Fleet& fleet);    
    ~State_ShipStatus();
    virtual void Update();
    virtual void Render(TCODConsole *root);
    virtual void End();
    virtual bool Init();
    //virtual void RecoverFromPush() {if (!nextState) delete nextState; pushSomething = false;}
    //
    virtual void RecoverFromPush();
		virtual void Resize(int new_w,int new_h);
    virtual void WindowActive();
    virtual void WindowInactive();
    virtual void KeyUp(const int &key,const int &unicode);
		virtual void KeyDown(const int &key,const int &unicode);
		virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
		virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
		virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);

    TCODConsole* console;
    TCODConsole* debug;

  private:
    int selector;
    bool showDebug;
    
    bool redraw;
    void redrawList();
    void invertLine(const int& line);
    void printStats(TCODConsole* con, int& line);
    void swapLineColors(TCODConsole* con, const int& line);
    void drawDebug();

    Fleet &refToFleet;
    std::vector<int> pages;
    int page;
    int pageit; // the position in the pages vector that yields page

    std::string newname;
    std::string header();
    std::string assembleOutput(const LedgerItemTuple& tuple);
  };