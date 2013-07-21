#include "State_drydock.h"
#include "State_stringIn.h"
#include <string>
#include <SDL.h>
#include "State_prompt.h"
#include "utility.h"
#include <iostream>

#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

using namespace std;

State_Drydocks::State_Drydocks(Town& town, Ship& ship)
  : refToTown(town), refToShip(ship), selector(3), redraw(false), startbuy(false), startsell(false),
  calculatebuy(false), calculatesell(false), isHometown(false), getPrompt(false)
  {
  consoleLeft = new TCODConsole(50, 48);
  consoleRight = new TCODConsole(50, 48);
  if(ship.captain.faction == town.getFactionID())
    isHometown = true;
  }

string drydocksHeader()
  {
  string returnval;

  returnval += string("Ship name                  Size        Net price");
  return returnval;
  }

string State_Drydocks::assembleOutput(const ShipPrototype& sp)
  {
  /// 
  /// Name:n Size:s Maxstorage: M, max cargo C, max sailors S, min sailor m, max cannon c, lateen sail l, square sail s, base armor A, durability D
  /// n{15}_s_MMM_CCC_mm/SS_cc_lll_sss_AA_DDD
  string blank(" ");

  string returnval;

  returnval += sp.typeName.substr(0, 15);
  if(sp.typeName.size() < 15)
    for (int counter = 0; counter < 15 - sp.typeName.size(); counter++)
      returnval += blank;

  returnval += blank;
  if (sp.size == string("small"))
    returnval += "S";
  else if (sp.size == string("medium"))
    returnval += "M";
  else if (sp.size == string("large"))
    returnval += "L";
  else returnval += "n";
  
  returnval += blank;

  returnval += rightAlignNumber(sp.maxstorage, 3) + " ";
  returnval += rightAlignNumber(sp.maxcargo, 3) + " ";
  returnval += rightAlignNumber(sp.minimumsailors, 2);
  returnval += "/";
  returnval += rightAlignNumber(sp.maxsailors, 2) + " ";
  returnval += rightAlignNumber(sp.maxcannons, 2) + " ";
  returnval += rightAlignNumber(sp.lateen, 3) + " ";
  returnval += rightAlignNumber(sp.square, 3) + " ";
  returnval += rightAlignNumber(sp.baseArmor, 2) + " ";
  returnval += rightAlignNumber(sp.maxDurability, 3);

  return returnval;
  }

bool State_Drydocks::Init()
  {
  redrawLeft();
  redrawRight();
  redraw = true;
  return true;
  }

void State_Drydocks::End()
  {
  delete consoleLeft;
  delete consoleRight;
  }

void State_Drydocks::invertLine(const int& line, TCODConsole* console)
  {
  
  for (int counter = 1; counter < 50; counter++)
    {
    console->setCharBackground(counter, line, TCODColor::white);
    console->setCharForeground(counter, line, TCODColor::black);
    }
  }

void State_Drydocks::Render(TCODConsole *root)
  {
  TCODConsole::blit(consoleLeft, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.7f);
  TCODConsole::blit(consoleRight, 0, 0, 0, 0, root, 50, 0, 1.0f, 0.7f);
  }

void State_Drydocks::RecoverFromPush()
  {
  if (!nextState) 
    delete nextState; 
  pushSomething = false;
  }

void State_Drydocks::Update()
  {
  consoleLeft->clear();
  consoleLeft->setDefaultForeground(TCODColor::white);

  if (getPrompt && calculatebuy)
    {
    if (yesNo)
      {
      int price = refToTown.shipList.at(selector-3).price - refToShip.getShipPrice();
      if (refToShip.getMoney() < price)
        {
        nextState = new State_Prompt(21, 4, string("Not enough money."), yesNo);
        pushSomething = true;
        }
      else
        {
        refToShip.changeShip(refToTown.shipList.at(selector-3));
        refToShip.addMoney(price);
        string output = "You have purchased a " + refToShip.getType() + ".";
        nextState = new State_Prompt(output.size() + 4, 4, output, yesNo);
        pushSomething = true;
        }
      }
    getPrompt = false;
    }
  if(startbuy && selector >= 3 && selector < refToTown.shipList.size()+3)
    {
    promptResult.clear();
    int price = refToTown.shipList.at(selector-3).price - refToShip.getShipPrice();
    int digits = floor(log10((float)abs(price))) + 1;
    if (price < 0) digits++;
    string output = string("Really buy a ") + refToTown.shipList.at(selector-3).typeName + " for " + rightAlignNumber(price, digits) + " ducats?";
    yesNo = false;
    nextState = new State_Prompt(output.size() + 4, 4, output, yesNo);
    pushSomething = true;
    getPrompt = true;
    startbuy = false;
    calculatebuy = true;
    }


  if (redraw)
    {
    redrawLeft();
    redrawRight();
    }
  }

void State_Drydocks::KeyDown(const int &key,const int &unicode)
  {
    if (key == SDLK_ESCAPE)
      {popMe = true;}
  else if (key == SDLK_DOWN && selector < 46)
     {
     selector++;
     redraw = true;
     }
   else if (key == SDLK_UP && selector > 2)
     {
     selector--;
     redraw = true;
     }
   else if (key == SDLK_RETURN)
     {
     startbuy = true;
     }
  }

/////////////////
/////////////////
/////////////////

void State_Drydocks::redrawLeft()
  {
  consoleLeft->clear();

  consoleLeft->setDefaultForeground(TCODColor::grey);
  consoleLeft->print(1, 0, "(0) Trade shop (1) Drydocks");
  int line = 2;

  consoleLeft->setDefaultForeground(TCODColor::yellow);
  consoleLeft->print(1, line++, drydocksHeader().c_str());
  consoleLeft->setDefaultForeground(TCODColor::white);

  for (auto it = refToTown.shipList.begin(); it < refToTown.shipList.end(); it++)
    {
    string buffer;
    buffer += leftAlign(it->typeName, 25) + " ";
    buffer += leftAlign(it->size, 7) + " ";
    char price_cstr[14];
    _snprintf(price_cstr, sizeof(price_cstr), "%d", it->price - refToShip.getShipPrice());
    string price(price_cstr);
    buffer += rightAlign(price, 14) + " ";
    consoleLeft->print(1, line++, buffer.c_str());
    }

  if (selector > 1)
    invertLine(selector, consoleLeft);

  consoleLeft->setDefaultForeground(TCODColor(96,71,64));
  consoleLeft->printFrame(0, 1, 50, 47, false);
  }

void State_Drydocks::redrawRight()
  {
  consoleRight->clear();
  consoleRight->setDefaultForeground(TCODColor::white);

  int line = 2;
  if (selector >= 3 && selector-3 < refToTown.shipList.size())
    {
    auto ship = refToTown.shipList.at(selector-3);
    
    swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Name : %s", ship.typeName.c_str()); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Size : %s", ship.size.c_str()); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Price: %d", ship.price); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Total storage: %d", ship.maxstorage); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Goods: %d    Sailors: %d/%d    Cannons: %d", ship.maxcargo, ship.minimumsailors, ship.maxsailors, ship.maxcannons); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Lateen sails: %d    Square sails: %d", ship.lateen, ship.square); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Base speed: %d", ship.baseSpeed(ship.lateen, ship.square)); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Wave resistance: %d", ship.waveResistance); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Turning: %d", ship.turning); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Base armor: %d", ship.baseArmor); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Max durability: %d", ship.maxDurability); swapLineColors(consoleRight, line);
    consoleRight->setDefaultForeground(TCODColor::silver);
    line++;
    int height = consoleRight->printRect(1, line, 48, 0, "%s", ship.desc.c_str());
    line += height;
    }
  auto ship = refToShip;
  line=24;

  consoleRight->setDefaultForeground(TCODColor::yellow);
  consoleRight->print(1, line++, "<Current ship>");

  swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Name : %s", ship.getType().c_str()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Size : %s", ship.getSize().c_str()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Price: %d", ship.getShipPrice()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Total storage: %d", ship.getMaxStorage()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Goods: %d    Sailors: %d/%d    Cannons: %d", ship.getMaxGoods(), ship.getMinSailors(), ship.getMaxSailors(), ship.getMaxCannons()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Lateen sails: %d    Square sails: %d", ship.getLateen(), ship.getSquare()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Base speed: %d", ship.getBaseSpeed()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Wave resistance: %d", ship.getWaveResistance()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Turning: %d", ship.getTurning()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Base armor: %d", ship.getArmor()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Max durability: %d", ship.getMaxDurability()); swapLineColors(consoleRight, line);
  consoleRight->setDefaultForeground(TCODColor::silver);
  line++;
  int height = consoleRight->printRect(1, line, 48, 0, "%s", ship.getDescription().c_str());
  line += height;
  line+=3;
  consoleRight->setDefaultForeground(TCODColor::yellow);
  consoleRight->print(1, line++, "Current money = %d", ship.getMoney());
  consoleRight->setDefaultForeground(TCODColor(96,71,64));
  consoleRight->printFrame(0, 1, 50, 22, false);
  consoleRight->printFrame(0, 23, 50, 25, false);
  }

void State_Drydocks::swapLineColors(TCODConsole* con, const int& counter)
  {
  if (counter %2)
      con->setDefaultForeground(TCODColor::lightestGreen);
  else con->setDefaultForeground(TCODColor::lightestBlue);
  }