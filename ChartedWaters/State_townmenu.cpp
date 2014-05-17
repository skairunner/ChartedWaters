#include "State_townmenu.h"
#include "State_shop.h"
#include <SDL.h>
#include <string>
#include "State_prompt.h"
#include "State_drydock.h"
#include "State_tavern.h"
#include "State_shipPartShop.h"
using namespace std;

State_TownMenu::State_TownMenu(Town& town, Fleet& fleet)
: refToTown(town), refToFleet(fleet)
  {
  int potenwidth = 14 + town.getName().size();
  potenwidth = potenwidth > 18 ? potenwidth : 18;
  console = new TCODConsole(potenwidth, 9);
  }

bool State_TownMenu::Init()
  {
  if (refToFleet.lastVisitedCity != refToTown.getName())
    {
    refToFleet.lastVisitedCity = refToTown.getName();
    refToFleet.lastVisitedCityCoords = refToTown.myPosition;
    refToFleet.removeFatigue(-50); // Reduces all fatigue of ships by 50. 
    }
  
  drawMenu();
  return true;
  }

void State_TownMenu::Render(TCODConsole* root)
  {
  TCODConsole::blit(console, 0, 0, 0, 0, root, root->getWidth() / 2 - console->getWidth() / 2, root->getHeight() / 2 - console->getHeight() / 2, 1.0f, 0.95f);;
  }

void State_TownMenu::End()
  {
  if (console)
    delete console;
  }

void State_TownMenu::KeyDown(const int &key,const int &unicode)
  {
  if (key == SDLK_ESCAPE)
    {
    if (refToFleet.isLoadedProperly()) 
      popMe = true;
    else // too many items!
      {
      string message("You have too many items.");
      nextState = new State_Prompt(message.size()+4, 4, message, throwawayBool);
      pushSomething = true;
      }
    }

  switch (key)
    {
 /* case SDLK_t:
    nextState = new State_Shop(refToTown, refToFleet);
    pushSomething = true;
    break;
  case SDLK_d:
    nextState = new State_Drydocks(refToTown, refToFleet);
    pushSomething = true;
    break;
  case SDLK_h:
    break;*/
  case SDLK_v:
    nextState = new State_Tavern(refToFleet);
    pushSomething = true;
    break;
  /*case SDLK_s:
    nextState = new State_shipPartShop(&refToTown, &refToFleet);
    pushSomething = true;
    break;*/
  default:
    break;
    }
  }

void State_TownMenu::drawMenu()
  {
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);

  int line = 1;
  console->print(1, line++, "Welcome to %s!", refToTown.getName().c_str());
  line++;
  console->print(1, line++, "%cT%crading post", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cD%crydocks", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cS%chip parts", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  //console->print(1, line++, "%cH%carbor", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->setDefaultForeground(TCODColor::grey);
  //console->print(1, line++, "Ship parts");
  console->print(1, line++, "Harbor");
  console->setDefaultForeground(TCODColor::white);
  console->print(1, line++, "Ta%cv%cern", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }