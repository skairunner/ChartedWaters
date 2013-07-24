#include "State_tavern.h"
#include <SDL.h>
#include <string>
#include "State_stringIn.h"
#include "utility.h"
#include "State_prompt.h"

using std::string;

State_Tavern::State_Tavern(Ship& ship)
  :refToShip(ship)
  {
  console = new TCODConsole(10, 6);
  }

void State_Tavern::End()
  {
  delete console;
  }

bool State_Tavern::Init()
  {
  drawMenu();
  return true;
  }

void State_Tavern::Render(TCODConsole* root)
  {
  TCODConsole::blit(console, 0, 0, 0, 0, root, root->getWidth() / 2 - console->getWidth() / 2, root->getHeight() / 2 - console->getHeight() / 2, 1.0f, 0.95f);
  }

void State_Tavern::KeyDown(const int &key,const int &unicode)
  {
  switch (key)
    {
  case SDLK_s:
    nextState = new State_buySailors(refToShip);
    pushSomething = true;
    break;
  /*case SDLK_r:
    nextState = new State_buyRations(refToShip);
    pushSomething = true;
    break;*/
  case SDLK_ESCAPE:
    popMe = true;
    break;
  default:
    break;
    }
  }

void State_Tavern::drawMenu()
  {
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);

  int line = 1;
  console->print(1, line++, "Tavern");
  line++;
  console->print(1, line++, "%cS%cailors", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cR%cations", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }

//////
//////
//////

State_buySailors::State_buySailors(Ship& ship)
  :refToShip(ship), green(0), exp(0), buy(0), yesno(0), price(0), getSomething(0), master(0), sell(0)
  {
  console = new TCODConsole(35, 12);
  }

void State_buySailors::End()
  {
  delete console;
  }

bool State_buySailors::Init()
  {
  drawMenu();
  return true;
  }

void State_buySailors::Update()
  {
  if (getSomething && !prompt.empty() && sell)
    {
    int result = stringToDecimal(prompt);
    if (result > 0)
      {
      if (refToShip.sailors - result < 0)
        {
        string output = "You can't fire that many sailors.";
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        }
      else
        {
        string output = "Fired " + rightAlignNumber(result) + " sailors.";
        refToShip.removeSailors(result);
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        pushSomething = true;
        }
      prompt.clear();
      sell = false;
      getSomething = false;
      }
    }
    else if (getSomething && prompt.empty() && sell)
      {
      sell = false;
      getSomething = false;
      prompt.clear();
      }
    else if (sell)
      {
      string output = "Fire how many sailors?";
      nextState = new state_StringIn(output.length()+4, prompt, output);
      pushSomething = true;
      getSomething = true;
      }

  if (getSomething && !prompt.empty() && buy)
    {
    int result = stringToDecimal(prompt);
    if (result > 0)
      {
      if (refToShip.sailors + result < refToShip.getMaxSailors())
        {
        string output = "You can't hire that many sailors.";
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        }
      else if (result * price > refToShip.captain.ducats)
        {
        string output = "You don't have enough money to hire that many sailors.";
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        }
      else
        {
        string output = "Hired " + rightAlignNumber(result) + " sailors for " + rightAlignNumber(result * price) + " ducats.";
        if (master)
          refToShip.addSailors(result, 800);
        else if (exp)
          refToShip.addSailors(result, 500);
        else if (green)
          refToShip.addSailors(result, 200);
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        pushSomething = true;
        }
      prompt.clear();
      price = 0;
      result = 0;
      buy = false;
      getSomething = false;
      green = exp = master = false;
      }
    }
  else if (getSomething && prompt.empty() && buy)
    {
    buy = false;
    getSomething = false;
    green = exp = false;
    prompt.clear();
    }
  else if (buy)
    {
    string output = "Hire how many ";
    if (exp)
      {
      output += "experienced sailors?";
      price = 1000;
      }
    else if (green) 
      {
      output += "green sailors?";
      price = 100;
      }
    else if (master)
      {
      output += "master sailors?";
      price = 5000;
      }
    nextState = new state_StringIn(output.length()+4, prompt, output);
    pushSomething = true;
    getSomething = true;
    }
  }

void State_buySailors::Render(TCODConsole* root)
  {
  TCODConsole::blit(console, 0, 0, 0, 0, root, root->getWidth() / 2 - console->getWidth() / 2, root->getHeight() / 2 - console->getHeight() / 2, 1.0f, 0.98f);
  }

void State_buySailors::KeyDown(const int &key,const int &unicode)
  {
  switch (key)
    {
  case SDLK_ESCAPE:
    popMe = true;
    break;
  case SDLK_g:
    green = true;
    buy = true;
    break;
  case SDLK_e:
    exp = true;
    buy = true;
    break;
  case SDLK_m:
    master = true;
    buy = true;
    break;
  case SDLK_f:
    sell = true;
    break;
  default:
    break;
    }
  }

void State_buySailors::drawMenu()
  {
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
  console->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);

  int line = 1;
  console->print(1, line++, "Tavern: hire");
  console->setDefaultForeground(TCODColor::grey);
  console->print(1, line++, "Sailors are paid 10 ducats/day.");
  console->setDefaultForeground(TCODColor::white);
  line+= 2;
  console->print(1, line++, "%cG%creen sailors -  100", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cE%cxperienced sailors - 1000", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cM%caster sailors - 5000", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  line += 2;
  console->setDefaultForeground(TCODColor::lightestRed);
  console->print(1, line++, "%cF%cire sailors", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }