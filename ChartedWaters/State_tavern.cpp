#include "State_tavern.h"
#include <SDL.h>
#include <string>
#include "State_stringIn.h"
#include "utility.h"
#include <cmath>
#include "town.h"
#include "State_prompt.h"

using std::string;

State_Tavern::State_Tavern(Fleet& fleet)
  :refToFleet(fleet)
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
    nextState = new State_buySailors(refToFleet);
    pushSomething = true;
    break;
  case SDLK_r:
    nextState = new State_buyRations(refToFleet);
    pushSomething = true;
    break;
  case SDLK_f:
    nextState = new State_recoverFatigue(refToFleet);
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
    console->clear();
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

State_buySailors::State_buySailors(Fleet& fleet)
  :refToFleet(fleet), green(0), exp(0), buy(0), yesno(0), price(0), getSomething(0), master(0), sell(0)
  {
  console = new TCODConsole(35, 12);
  pageit = 0;
  pages = getKeys(fleet.ships);
  page = pages[0];
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
        if (page == -1) // do the operations on the fleet
        {
            if (refToFleet.getNumSailors() < result)
            {
                string output = "You can't fire that many sailors.";
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
            else
            {
                string output = "Fired " + rightAlignNumber(result) + " sailors.";
                refToFleet.addSailors(-result, 0);
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
        }
        else // do the operations on a ship
        {
            if (refToFleet.refSailors(page) < result)
            {
                string output = "You can't fire that many sailors.";
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
            else
            {
                string output = "Fired " + rightAlignNumber(result) + " sailors.";
                refToFleet.ships[page].removeSailors(result);
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
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
        if (page == -1) // operations on fleet
        {
            if (refToFleet.getNumSailors() + result > refToFleet.getMaxSailors())
            {
                string output = "You can't hire that many sailors.";
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
            else if (result * price > refToFleet.captain.ducats)
            {
                string output = "You don't have enough money to hire that many sailors.";
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
            else
            {
                string output = "Hired " + rightAlignNumber(result) + " sailors for " + rightAlignNumber(result * price) + " ducats.";
                if (master)
                    refToFleet.addSailors(result, 800);
                else if (exp)
                    refToFleet.addSailors(result, 500);
                else if (green)
                    refToFleet.addSailors(result, 200);
                refToFleet.captain.ducats -= result * price;

                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
        }
        else // operations on a ship
        {
            Ship& ship = refToFleet.ships[page];
            if (ship.sailors + result > ship.getMaxSailors())
            {
                string output = "You can't hire that many sailors.";
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
            else if (result * price > refToFleet.captain.ducats)
            {
                string output = "You don't have enough money to hire that many sailors.";
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
            else
            {
                string output = "Hired " + rightAlignNumber(result) + " sailors for " + rightAlignNumber(result * price) + " ducats.";
                if (master)
                    ship.addSailors(result, 800);
                else if (exp)
                    ship.addSailors(result, 500);
                else if (green)
                    ship.addSailors(result, 200);
                refToFleet.captain.ducats -= result * price;

                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
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
      nextState = new state_StringIn(output.length() + 4, prompt, output);
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
  case SDLK_LEFT:
      if (pageit > -1)
      {
          pageit--;
          if (pageit == -1)
              page = -1;
          else
              page = pages[pageit];
          drawMenu();
      }
      break;
  case SDLK_RIGHT:
      if (pageit < int(pages.size()) - 1)
      {
          pageit++;
          page = pages[pageit];
          drawMenu();
      }
      break;
  default:
    break;
    }
  }

void State_buySailors::drawMenu()
  {
    console->clear();
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
  console->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);

  int line = 1;
  console->print(1, line++, "Tavern: hire");
  console->setDefaultForeground(TCODColor::grey);
  console->print(1, line++, "Sailors are paid 10 ducats/day.");
  if (pageit == -1)
  {
      console->setDefaultForeground(TCODColor::grey);
      console->print(1, line++, "For fleet");
  }
  else
  {
      Ship& ship = refToFleet.ships[page];
      console->setDefaultForeground(TCODColor::grey);
      console->print(1, line++, ("For " + ship.getType() + " " + ship.getName()).c_str());
  }
  console->setDefaultForeground(TCODColor::white);

  line++;
  console->print(1, line++, "%cG%creen sailors -  100", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cE%cxperienced sailors - 1000", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "%cM%caster sailors - 5000", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  line++;
  console->setDefaultForeground(TCODColor::lightestRed);
  console->print(1, line++, "%cF%cire sailors", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);

  int size = pages.size();
  if (pageit == -1)
  {
      for (int c = 0; c < size; c++)
          console->putCharEx(1 + c, console->getHeight() - 2, 7, TCODColor::white, TCODColor::black);
  }
  else
  {
      for (int c = 0; c < size; c++)
          console->putCharEx(1 + c, console->getHeight() - 2, 7, TCODColor::lightGrey, TCODColor::black);
      console->putCharEx(1 + pageit, console->getHeight() - 2, 7, TCODColor::white, TCODColor::black);
  }

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }


//////
//////
//////

State_buyRations::State_buyRations(Fleet& fleet)
  :refToFleet(fleet), buy(0), yesno(0), getSomething(0), sell(0), daysworth(0)
  {
  console = new TCODConsole(45, 12);
  pageit = 0;
  pages = getKeys(fleet.ships);
  page = pages[0];
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
            if (page == -1)
            {
                if (refToFleet.getRations() < 10 * result)
                {
                    string output = "You can't sell that many rations.";
                    nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                    pushSomething = true;
                }
                else
                {
                    string output = "Sold " + rightAlignNumber(result) + " rations.";
                    refToFleet.addRations(-10 * result);
                    refToFleet.captain.ducats += 40 * result;
                    nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                    pushSomething = true;
                }
            }
            else // For a single ship
            {
                if (refToFleet.ships[page].rations < 10 * result)
                {
                    string output = "You can't sell that many rations.";
                    nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                    pushSomething = true;
                }
                else
                {
                    string output = "Sold " + rightAlignNumber(result) + " rations.";
                    refToFleet.addRations(-10 * result, page);
                    refToFleet.captain.ducats += 40 * result;
                    nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                    pushSomething = true;
                }
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
        nextState = new state_StringIn(output.length() + 4, prompt, output);
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
                double tempresult = 0;
                if (pageit == -1)
                    tempresult = result * refToFleet.getNumSailors();
                else
                    tempresult = result * refToFleet.ships[page].sailors;
                result = (int)ceil(tempresult / 10.0f);
            }

            if (result * 50 > refToFleet.captain.ducats)
            {
                string output = "You don't have enough money to buy that much.";
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
                pushSomething = true;
            }
            else
            {
                string output = "Bought " + rightAlignNumber(result) + " rations for " + rightAlignNumber(result * 50) + " ducats.";
                refToFleet.addRations(10 * result, page);
                refToFleet.captain.ducats -= 50 * result;
                nextState = new State_Prompt(output.length() + 4, 4, output, yesno);
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
        nextState = new state_StringIn(output.length() + 4, prompt, output);
        pushSomething = true;
        getSomething = true;
    }
    else if (buy)
    {
        string output = "Buy how many cargo units of rations?";
        nextState = new state_StringIn(output.length() + 4, prompt, output);
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
  case SDLK_LEFT:
      if (pageit > -1)
      {
          pageit--;
          if (pageit == -1)
              page = -1;
          else
              page = pages[pageit];

          drawMenu();
      }          
      break;
  case SDLK_RIGHT:
      if (pageit < int(pages.size()) - 1)
      {
          pageit++;
          page = pages[pageit];

          drawMenu();
      }          

      break;
  default:
    break;
    }
  }

void State_buyRations::drawMenu()
  {
    console->clear();
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);
  console->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);

  int line = 1;
  console->print(1, line++, "Tavern: rations");

  console->setDefaultForeground(TCODColor::grey);
  console->print(1, line++, "1 sailor consumes .1 rations per day.");
  console->print(1, line++, "1 units of rations takes up 1 cargo unit.");
  console->print(1, line++, "You may buy or sell rations in units of 1.");


  if (pageit == -1)
  {
      console->setDefaultForeground(TCODColor::grey);
      console->print(1, line++, "For fleet");
  }
  else
  {
      Ship& ship = refToFleet.ships[page];
      console->setDefaultForeground(TCODColor::grey);
      console->print(1, line++, ("For " + ship.getType() + " " + ship.getName()).c_str());
  }

  console->setDefaultForeground(TCODColor::white);
  line++;
  console->print(1, line++, "%cB%cuy rations -  50 / 1 cargo unit", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(1, line++, "    %cG%cet day's worth of rations", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->setDefaultForeground(TCODColor::lightestRed);
  console->print(1, line++, "%cS%cell rations - 40 / 1 cargo unit", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);

  int size = pages.size();
  if (pageit == -1)
  {
      for (int c = 0; c < size; c++)
          console->putCharEx(1 + c, console->getHeight() - 2, 7, TCODColor::white, TCODColor::black);
  }                                      
  else 
  {
      for (int c = 0; c < size; c++)
          console->putCharEx(1 + c, console->getHeight() - 2, 7, TCODColor::lightGrey, TCODColor::black);
      console->putCharEx(1 + pageit, console->getHeight() - 2, 7, TCODColor::white, TCODColor::black);
  }

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }

//////
//////
//////

State_recoverFatigue::State_recoverFatigue(Fleet& fleet)
  :refToFleet(fleet), yesno(0), getSomething(0), recover(0)
  {
  console = new TCODConsole(36, 10);
  pageit = 0;
  pages = getKeys(fleet.ships);
  page = pages[0];
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

    msg += rightAlignNumber(recover) + " fatigue for " + rightAlignNumber(Town::getCost(recover)) + " ducats?";
    nextState = new State_Prompt(msg.size()+2, 4, msg, yesno);
    pushSomething = true;
    getSomething = true;
    return;
    }
  else if (recover && getSomething && yesno)
  {
      int result = 12535;
      result = Town::recoverFatigue(refToFleet, recover, page); // Because if page == -1, that's when it does the recovering on the whole fleet.
      if (result == twNOT_ENOUGH_MONEY)
      {
          string msg("You don't have enough money.");
          pushSomething = true;
          nextState = new State_Prompt(msg.size() + 2, 4, msg, yesno);
      }
      else if (result == twSUCCESS)
      {
          std::cout << "fatigue_recover_success\n";
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
  case SDLK_LEFT:
      if (pageit > -1)
      {
          pageit--;
          if (pageit == -1)
              page = -1;
          else
              page = pages[pageit];
          drawMenu();
      }
      break;
  case SDLK_RIGHT:
      if (pageit < int(pages.size()) - 1)
      {
          pageit++;
          page = pages[pageit];
          drawMenu();
      }
      break;
  default:
    break;
    }
  }

void State_recoverFatigue::drawMenu()
  {
    console->clear();
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);

  int line = 1;
  console->print(console->getWidth()/2-8+1, line++, "Tavern: fatigue");

  if (pageit == -1)
  {
      console->setDefaultForeground(TCODColor::grey);
      console->print(1, line++, "For fleet");
  }
  else
  {
      Ship& ship = refToFleet.ships[page];
      console->setDefaultForeground(TCODColor::grey);
      console->print(1, line++, ("For " + ship.getType() + " " + ship.getName()).c_str());
  }
  console->setDefaultForeground(TCODColor::white);

  line++;
  console->print(2, line++, "Recover %c1%c fatigue -   200 ducats", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(2, line++, "Recover %c2%c0 fatigue - 3200 ducats", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  console->print(2, line++, "Recover %c5%c0 fatigue - 6400 ducats", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  int size = pages.size();
  if (pageit == -1)
  {
      for (size_t c = 0; c < size; c++)
          console->putCharEx(1 + c, console->getHeight() - 2, 7, TCODColor::white, TCODColor::black);
  }
  else
  {
      for (size_t c = 0; c < size; c++)
          console->putCharEx(1 + c, console->getHeight() - 2, 7, TCODColor::lightGrey, TCODColor::black);
      console->putCharEx(1 + pageit, console->getHeight() - 2, 7, TCODColor::white, TCODColor::black);
  }

  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, console->getWidth(), console->getHeight(), false);
  }

