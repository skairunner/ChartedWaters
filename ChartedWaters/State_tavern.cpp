#include "State_tavern.h"
#include <SDL.h>
#include <string>
#include "State_stringIn.h"
#include "utility.h"
#include <cmath>
#include "State_prompt.h"

using std::string;

State_Tavern::State_Tavern(Ship& ship)
  :refToShip(ship)
  {
  console = new TCODConsole(21, 9);
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
  case SDLK_r:
    nextState = new State_buyRations(refToShip);
    pushSomething = true;
    break;
  case SDLK_f:
    nextState = new State_recoverFatigue(refToShip);
    pushSomething = true;
    break;
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
  line+=2;
  console->print(1, line++, "%cS%cailors", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cR%cations", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

  console->print(1, line++, "Recover %cf%catigue", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

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
        pushSomething = true;
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
      if (refToShip.sailors + result > refToShip.getMaxSailors())
        {
        string output = "You can't hire that many sailors.";
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        pushSomething = true;
        }
      else if (result * price > refToShip.captain.ducats)
        {
        string output = "You don't have enough money to hire that many sailors.";
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        pushSomething = true;
        }
      else
        {
        string output = "Hired " + rightAlignNumber(result) + " sailors for " + rightAlignNumber(result * price) + " ducats.";
        int fatigue = refToShip.fatigue * refToShip.sailors;
               
        if (master)
          refToShip.addSailors(result, 800);
        else if (exp)
          refToShip.addSailors(result, 500);
        else if (green)
          refToShip.addSailors(result, 200);
        refToShip.captain.ducats -= result * price;

        refToShip.fatigue = fatigue / refToShip.sailors;
        
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
    green = exp = master = false;
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


//////
//////
//////

State_buyRations::State_buyRations(Ship& ship)
  :refToShip(ship), buy(0), yesno(0), getSomething(0), sell(0), daysworth(0)
  {
  console = new TCODConsole(45, 12);
  }

void State_buyRations::End()
  {
  delete console;
  }

bool State_buyRations::Init()
  {
  drawMenu();
  return true;
  }

void State_buyRations::Update()
  {
  if (getSomething && !prompt.empty() && sell)
    {
    int result = stringToDecimal(prompt);
    if (result > 0)
      {
      if (refToShip.rations < 10 * result)
        {
        string output = "You can't sell that many rations.";
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        pushSomething = true;
        }
      else
        {
        string output = "Sold " + rightAlignNumber(result) + " rations.";
        refToShip.rations -= 10 * result;
        refToShip.captain.ducats += 40 * result;
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
      string output = "Sell how many cargo units of rations?";
      nextState = new state_StringIn(output.length()+4, prompt, output);
      pushSomething = true;
      getSomething = true;
      }

  if (getSomething && !prompt.empty() && buy)
    {
    int result = stringToDecimal(prompt);
    if (result > 0)
      {
      if (daysworth)
        {
        double tempresult = result * refToShip.sailors;
        result = (int)ceil(tempresult/10.0f);
        }

      if (result * 50 > refToShip.captain.ducats)
        {
        string output = "You don't have enough money to buy that much.";
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        pushSomething = true;
        }
      else
        {
        string output = "Bought " + rightAlignNumber(result) + " rations for " + rightAlignNumber(result * 50) + " ducats.";
        refToShip.rations += 10 * result;
        refToShip.captain.ducats -= 50 * result;
        nextState = new State_Prompt(output.length()+4, 4, output, yesno);
        pushSomething = true;
        }
      prompt.clear();
      result = 0;
      buy = false;
      getSomething = false;
      daysworth = false;
      }
    }
  else if (getSomething && prompt.empty() && buy)
    {
    buy = false;
    getSomething = false;
    daysworth = false;
    prompt.clear();
    }
  else if (buy && daysworth)
    {
    string output = "Buy how many day's worth of rations?";
    nextState = new state_StringIn(output.length()+4, prompt, output);
    pushSomething = true;
    getSomething = true;
    }
  else if (buy)
    {
    string output = "Buy how many cargo units of rations?";
    nextState = new state_StringIn(output.length()+4, prompt, output);
    pushSomething = true;
    getSomething = true;
    }
  }

void State_buyRations::Render(TCODConsole* root)
  {
  TCODConsole::blit(console, 0, 0, 0, 0, root, root->getWidth() / 2 - console->getWidth() / 2, root->getHeight() / 2 - console->getHeight() / 2, 1.0f, 0.98f);
  }

void State_buyRations::KeyDown(const int &key,const int &unicode)
  {
  switch (key)
    {
  case SDLK_ESCAPE:
    popMe = true;
    break;
  case SDLK_s:
    sell = true;
    break;
  case SDLK_b:
    buy = true;
    break;
  case SDLK_g:
    buy = true;
    daysworth = true;
    break;
  default:
    break;
    }
  }

void State_buyRations::drawMenu()
  {
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
  console->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);

  int line = 1;
  console->print(1, line++, "Tavern: ration");
  console->setDefaultForeground(TCODColor::grey);
  console->print(1, line++, "1 sailor consumes .1 rations per day.");
  console->print(1, line++, "1 units of rations takes up 1 cargo unit.");
  console->print(1, line++, "You may buy or sell rations in units of 1.");
  console->setDefaultForeground(TCODColor::white);
  line+= 2;
  console->print(1, line++, "%cB%cuy rations -  50 / 1 cargo unit", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "    %cG%cet day's worth of rations", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->setDefaultForeground(TCODColor::lightestRed);
  console->print(1, line++, "%cS%cell rations - 40 / 1 cargo unit", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }

//////
//////
//////

State_recoverFatigue::State_recoverFatigue(Ship& ship)
  :refToShip(ship), yesno(0), getSomething(0), recover(0)
  {
  console = new TCODConsole(36, 10);
  }

void State_recoverFatigue::End()
  {
  delete console;
  }

bool State_recoverFatigue::Init()
  {
  drawMenu();
  return true;
  }

void State_recoverFatigue::Update()
  {
  if (recover && !getSomething)
    {
    string msg("Recover ");

    msg += rightAlignNumber(recover) + " fatigue for " + rightAlignNumber(getCost(recover)) + " ducats?";
    nextState = new State_Prompt(msg.size()+2, 4, msg, yesno);
    pushSomething = true;
    getSomething = true;
    return;
    }
  else if (recover && getSomething)
    if (yesno)
      {
      if (refToShip.captain.ducats < getCost(recover))
        {
        string msg("You don't have enough money.");
        pushSomething = true;
        nextState = new State_Prompt(msg.size()+2, 4, msg, yesno);
        }
      else
        {
        if (refToShip.fatigue > 0)
          {
          refToShip.fatigue -= recover * 10;
          refToShip.captain.ducats -= getCost(recover);
          }
        }    
      }

    recover = 0;
    yesno = false;
    getSomething = false;  
  }

void State_recoverFatigue::Render(TCODConsole* root)
  {
  TCODConsole::blit(console, 0, 0, 0, 0, root, root->getWidth() / 2 - console->getWidth() / 2, root->getHeight() / 2 - console->getHeight() / 2, 1.0f, 0.98f);
  }

void State_recoverFatigue::KeyDown(const int &key,const int &unicode)
  {
  switch (key)
    {
  case SDLK_ESCAPE:
    popMe = true;
    break;
  case SDLK_1:
    recover = 1;
    break;
  case SDLK_2:
    recover = 20;
    break;
  case SDLK_5:
    recover = 50;
    break;
  default:
    break;
    }
  }

void State_recoverFatigue::drawMenu()
  {
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);

  int line = 1;
  console->print(console->getWidth()/2-8+1, line++, "Tavern: fatigue");
  line+= 2;
  console->print(2, line++, "Recover %c1%c fatigue -   200 ducats", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(2, line++, "Recover %c2%c0 fatigue - 3200 ducats", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(2, line++, "Recover %c5%c0 fatigue - 6400 ducats", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }

int State_recoverFatigue::getCost(const int& rec)
  {
  switch (recover)
      {
    case 1:
      return 200;
      break;
    case 20:
      return 3200;
      break;
    case 50:
      return 6400;
      break;
    default:
      return 0;
      break;
      }
  }