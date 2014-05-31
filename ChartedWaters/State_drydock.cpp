#include "State_drydock.h"
#include "State_stringIn.h"
#include <string>
#include <SDL.h>
#include "State_prompt.h"
#include "utility.h"
#include <iostream>

using namespace std;

State_Drydocks::State_Drydocks(Town& town, Fleet& fleet)
: refToTown(town), refToFleet(fleet), selector(3), redraw(false), startbuy(false), startsell(false),
calculatebuy(false), calculatesell(false), isHometown(false), getPrompt(false)
{
    consoleLeft = new TCODConsole(50, 48);
    consoleRight = new TCODConsole(50, 48);
    if (fleet.captain.faction == town.getFactionID())
        isHometown = true;

    pages = getKeys(fleet.ships);
    pageit = 0;
    page = pages[pageit];
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
    for (size_t counter = 0; counter < 15 - sp.typeName.size(); counter++)
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
    if (getPrompt && calculatebuy)
    {
        if (yesNo)
        {
            int price = 0;
            if (page == -1)
                price = refToTown.shipList.at(selector - 3).price;
            else
                price = refToTown.shipList.at(selector - 3).price - refToFleet.ships[page].getShipPrice();

            if (refToFleet.getMoney() < price)
            {
                nextState = new State_Prompt(21, 4, string("Not enough money."), yesNo);
                pushSomething = true;
            }
            else
            {
                if (page != -1)
                {
                    refToFleet.changeShip(refToTown.shipList.at(selector - 3), page);
                    refToFleet.addMoney(-price);
                    string output = "You have purchased a " + refToFleet.ships[page].getType() + ".";
                    nextState = new State_Prompt(output.size() + 4, 4, output, yesNo);
                    pushSomething = true;
                }
                else // gotta buy a new ship.
                {
                    // find an empty number, from 0 to 3.
                    int newIndex = 0;
                    for (; newIndex < 4; newIndex++)
                    {
                        if (refToFleet.ships.find(newIndex) == refToFleet.ships.end())
                            break;
                    }
                    refToFleet.addMoney(-price);
                    refToFleet.ships[newIndex] = Ship(refToTown.shipList.at(selector - 3));     
                    pages = getKeys(refToFleet.ships);
                    page = pages[pageit];
                }

                redraw = true;
            }
        }
        getPrompt = false;
    }
    if (startbuy && selector >= 3 && (size_t)selector < refToTown.shipList.size() + 3)
    {
        promptResult.clear();
        int price = 0;
        if (page == -1)
            price = refToTown.shipList.at(selector - 3).price;
        else
            price = refToTown.shipList.at(selector - 3).price - refToFleet.ships[page].getShipPrice();
        string output = string("Really buy a ") + refToTown.shipList.at(selector - 3).typeName + " for " + to_string(price) + " ducats?";
        yesNo = false;
        nextState = new State_Prompt(output.size() + 4, 4, output, yesNo);
        pushSomething = true;
        getPrompt = true;
        startbuy = false;
        calculatebuy = true;
    }
    if (getPrompt && calculatesell)
    {
        if (yesNo)
        {
            Ship& ship = refToFleet.ships[page];
            ship.returnParts(refToFleet.captain);
            int rations = ship.rations;
            int sailors = ship.sailors;
            int training = ship.training;
            auto items = ship.returnListOfItems();

            refToFleet.ships.erase(page);
            pages = getKeys(refToFleet.ships);
            page = pages[pageit];
            refToFleet.addRations(rations);
            refToFleet.addSailors(sailors, training);
            for (auto it : items)
            {
                Item temp = ItemDict.getItemTemplate(it.itemID);
                refToFleet.addItem(temp, it.numberOfItems, it.averagePurchasePrice);
            }
            redraw = true;
        }
        getPrompt = false;
    }
    if (startsell && page != -1 && refToFleet.ships.size() != 1)
    {
        promptResult.clear();
        Ship& ship = refToFleet.ships[page];
        int price = int(ship.getShipPrice() * .8);        
        string output = "Do you really want to sell the " + ship.getType() + " " + ship.getName() + "for " + to_string(price) + " ducats?";
        getPrompt = true;
        startsell = false;
        calculatesell = true;
        nextState = new State_Prompt(output.size() + 4, 4, output, yesNo);
    }

    if (redraw)
    {
        redrawLeft();
        redrawRight();
        redraw = false;
    }
}

void State_Drydocks::KeyDown(const int &key, const int &unicode)
{
    if (key == SDLK_ESCAPE)
    {
        popMe = true;
    }
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
    // The left and right keys flip between ships. If pageit >= size of pages, that means purchase a new ship. however, pageit cannot be larger than 3.
    else if (key == SDLK_LEFT)
    {
        if (pageit > 0)
        {
            pageit--;
            page = pages[pageit];
            redraw = true;
        }            
    }
    else if (key == SDLK_RIGHT)
    {
        if (pageit < 3 && pageit <= (int)pages.size() - 1)
        {
            pageit++;
            if (pageit == pages.size())
                page = -1;
            else
                page = pages[pageit];
            redraw = true;
        }
    }
    else if (key == SDLK_d)
    {
        startsell = true;
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
        string price;
        if (page == -1)
            price = to_string(it->price);
        else
            price = to_string(it->price - refToFleet.ships[page].getShipPrice());

        buffer += rightAlign(price, 14) + " ";
        consoleLeft->print(1, line++, buffer.c_str());
    }

    if (selector > 1)
        invertLine(selector, consoleLeft);

    consoleLeft->setDefaultForeground(TCODColor(96, 71, 64));
    consoleLeft->printFrame(0, 1, 50, 47, false);
}

void State_Drydocks::redrawRight()
  {
  consoleRight->clear();
  consoleRight->setDefaultForeground(TCODColor::white);

  int line = 2;
  if (selector >= 3 && (size_t)(selector-3) < refToTown.shipList.size())
    {
    auto ship = refToTown.shipList.at(selector-3);
    
    swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Type : %s", ship.typeName.c_str()); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Size : %s", ship.size.c_str()); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Price: %d", ship.price); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Total storage: %d", ship.maxstorage); swapLineColors(consoleRight, line);
    consoleRight->print(1, line++, "Goods: %d    Sailors: min %d/%d    Cannons: %d", ship.maxcargo, ship.minimumsailors, ship.maxsailors, ship.maxcannons); swapLineColors(consoleRight, line);
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


  if (page != -1) // means we are upgrading an existing ship
  {
      auto& ship = refToFleet.ships[page];
      line = 24;

      consoleRight->setDefaultForeground(TCODColor::yellow);
      consoleRight->print(1, line++, "<Current ship>");

      swapLineColors(consoleRight, line);
      consoleRight->print(1, line++, "Name : The %s %s", ship.getType().c_str(), ship.getName().c_str()); swapLineColors(consoleRight, line);
      consoleRight->print(1, line++, "Size : %s", ship.getSize().c_str()); swapLineColors(consoleRight, line);
      consoleRight->print(1, line++, "Price: %d", ship.getShipPrice()); swapLineColors(consoleRight, line);
      consoleRight->print(1, line++, "Total storage: %d", ship.getMaxStorage()); swapLineColors(consoleRight, line);
      consoleRight->print(1, line++, "Goods: %d    Sailors: min %d/%d    Cannons: %d", ship.getMaxGoods(), ship.getMinSailors(), ship.getMaxSailors(), ship.getMaxCannons()); swapLineColors(consoleRight, line);
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
      line += 3;
      consoleRight->setDefaultForeground(TCODColor::yellow);
      consoleRight->print(1, line++, "Current money = %d", refToFleet.getMoney());
  }
  else // else, we are buying a new ship
  {
      line = 24;
      consoleRight->setDefaultForeground(TCODColor::yellow);
      consoleRight->print(1, line++, "<Purchase new ship>");
      line++;
      consoleRight->print(1, line++, "Current money = %d", refToFleet.getMoney());
  }
  
  
  drawPageDots(consoleRight, 1, consoleRight->getHeight() - 2, pageit, pages.size());
  consoleRight->setDefaultForeground(TCODColor(96,71,64));
  consoleRight->printFrame(0, 1, 50, 22, false);
  consoleRight->printFrame(0, 23, 50, 25, false);
}