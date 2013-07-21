#include "GameState.h"
#include <libtcod.hpp>
#include "town.h"
#include "Ship.h"

class State_TownMenu : public GameState
  {
  public:
    State_TownMenu(Town& town, Ship& ship);
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
    TCODConsole* console;
    Town& refToTown;
    Ship& refToShip;
    void drawMenu();
    bool throwawayBool;
  };