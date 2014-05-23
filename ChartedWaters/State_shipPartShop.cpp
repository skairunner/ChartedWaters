#include "State_drydock.h"
#include "State_stringIn.h"
#include <string>
#include <SDL.h>
#include "State_prompt.h"
#include "utility.h"
#include <iostream>
#include "State_shipPartShop.h"

#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

using namespace std;

State_shipPartShop::State_shipPartShop(Town* town, Player* player)
  : selector(3), pToTown(town), pToPlayer(player), redraw(true), attemptBuy(true), getSomething(true), result(false)
  {
  consoleLeft = new TCODConsole(50, 48);
  consoleRight = new TCODConsole(50, 48);
  }

string State_shipPartShop::shopHeader()
  {
  string returnval;

  returnval += string("Part           Type      Price");
  return returnval;
  }

bool State_shipPartShop::Init()
{
    if (pToTown)
    {
        redrawLeft(pToTown->partList);
        ShipPart* part = 0;
        if (selector >= 3 && selector - 3 < pToTown->partList.size())
            part = pToTown->partList[selector - 3];
        redrawRight(part);
    }
    else return false;

    return true;
}

void State_shipPartShop::End()
  {
  delete consoleLeft;
  delete consoleRight;
  }

void State_shipPartShop::invertLine(const int& line, TCODConsole* console)
  {
  
  for (int counter = 1; counter < 50; counter++)
    {
    console->setCharBackground(counter, line, TCODColor::white);
    console->setCharForeground(counter, line, TCODColor::black);
    }
  }

void State_shipPartShop::Render(TCODConsole *root)
  {
  TCODConsole::blit(consoleLeft, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.7f);
  TCODConsole::blit(consoleRight, 0, 0, 0, 0, root, 50, 0, 1.0f, 0.7f);
  }

void State_shipPartShop::Update()
{
    if (pToTown)
    {
        /*consoleLeft->clear();
        consoleLeft->setDefaultForeground(TCODColor::white);*/
        if (attemptBuy && selector >= 3 && selector - 3 < pToTown->partList.size() && !getSomething)
        {
            pushSomething = true;
            std::string msg("Are you sure you want to buy ");
            msg += pToTown->partList.at(selector - 3)->shopName() + " for " + rightAlignNumber(pToTown->partList.at(selector - 3)->shopPrice()) + " ducats?";
            nextState = new State_Prompt(msg.size() + 4, 4, msg, result);
            getSomething = true;
        }
        else if (attemptBuy && getSomething)
        {
            std::string msg;
            if (result)
            {
                if (pToPlayer->ducats < pToTown->partList.at(selector - 3)->shopPrice())
                {
                    msg += "Not enough money.";
                    nextState = new State_Prompt(msg.size() + 4, 4, msg, result);
                    pushSomething = true;
                }
                else
                {
                    msg += "Bought " + pToTown->partList.at(selector - 3)->shopName() + ".";
                    nextState = new State_Prompt(msg.size() + 4, 4, msg, result);
                    pushSomething = true;

                    pToPlayer->ducats -= pToTown->partList.at(selector - 3)->shopPrice();

                    auto part = pToTown->partList.at(selector - 3);
                    if (part->type() == "cannon")
                    {
                        auto partpointer = dynamic_cast<ShipCannons*>(part);
                        pToPlayer->addPart(*partpointer);
                    }
                    else if (part->type() == "armor")
                    {
                        auto partpointer = dynamic_cast<ShipArmor*>(part);
                        pToPlayer->addPart(*partpointer);
                    }
                    else if (part->type() == "statue")
                    {
                        auto partpointer = dynamic_cast<ShipStatue*>(part);
                        pToPlayer->addPart(*partpointer);
                    }
                    else if (part->type() == "sails")
                    {
                        auto partpointer = dynamic_cast<ShipSails*>(part);
                        pToPlayer->addPart(*partpointer);
                    }
                }

            }

            attemptBuy = false;
            result = false;
            getSomething = false;
        }




        if (redraw)
        {
            redrawLeft(pToTown->partList);

            ShipPart* part = 0;
            if (selector >= 3 && selector - 3 < pToTown->partList.size())
                part = pToTown->partList[selector - 3];
            redrawRight(part);

            redraw = false;
        }
    }
}

void State_shipPartShop::KeyDown(const int &key,const int &unicode)
  {
  if (key == SDLK_ESCAPE)
    {popMe = true;}
  else if (key == SDLK_RETURN)
    {
    attemptBuy = true;
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
  }

/////////////////
/////////////////
/////////////////

void State_shipPartShop::getProfile(ShipArmor& item, int& line)
  {
  swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Name : %s", item.name.c_str()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Price : %d", item.price); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Durability : %d/%d", item.durability, item.durability); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Armor : %d", item.armor); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Speed : %.2f", item.speed); swapLineColors(consoleRight, line);  
  line += 3;
  consoleRight->setDefaultForeground(TCODColor::silver);
  int height = consoleRight->printRect(1, line, 48, 0, "%s", item.desc.c_str());
  line += height; swapLineColors(consoleRight, line);
  }

void State_shipPartShop::getProfile(ShipSails& item, int& line)
  {
  swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Name : %s", item.name.c_str()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Price : %d", item.price); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Durability : %d/%d", item.durability, item.durability); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Lateen : +%d    Square : +%d", item.lateen, item.square); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Turning : %d", item.turning); swapLineColors(consoleRight, line);  
  line += 3;
  consoleRight->setDefaultForeground(TCODColor::silver);
  int height = consoleRight->printRect(1, line, 48, 0, "%s", item.desc.c_str());
  line += height; swapLineColors(consoleRight, line);
  }

void State_shipPartShop::getProfile(ShipStatue& item, int& line)
  {
  swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Name : %s", item.name.c_str()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Price : %d", item.price); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Durability : %d/%d", item.durability, item.durability); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Dodge : %d    Healing : %d    Protection : %d", item.dodging, item.healing, item.protection); swapLineColors(consoleRight, line);
  line += 3;
  consoleRight->setDefaultForeground(TCODColor::silver);
  int height = consoleRight->printRect(1, line, 48, 0, "%s", item.desc.c_str());
  line += height; swapLineColors(consoleRight, line);
  }

void State_shipPartShop::getProfile(ShipCannons& item, int& line)
  {
  swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Name : %s", item.shopName().c_str()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Price : %d", item.shopPrice()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Durability : %d/%d", item.durability, item.durability); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Damage : %d", item.getDamage()); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Penetration : %d", item.penetration); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Range : %d", item.range); swapLineColors(consoleRight, line);
  consoleRight->print(1, line++, "Reload speed : %d", item.reload); swapLineColors(consoleRight, line);
  line += 3;
  consoleRight->setDefaultForeground(TCODColor::silver);
  int height = consoleRight->printRect(1, line, 48, 0, "%s", item.desc.c_str());
  line += height; swapLineColors(consoleRight, line);
  }


/////////////////
/////////////////
/////////////////

void State_shipPartShop::redrawLeft(std::vector<ShipPart*>& partList)
  {
  consoleLeft->clear();

  consoleLeft->setDefaultForeground(TCODColor::grey);

  int line = 2;

  consoleLeft->setDefaultForeground(TCODColor::yellow);
  consoleLeft->print(1, line++, shopHeader().c_str());
  consoleLeft->setDefaultForeground(TCODColor::white);

  for (auto it = partList.begin(); it < partList.end(); it++)
    {
    string buffer;

    buffer += leftAlign((**it).shopName(), 25) + " ";
    buffer += leftAlign((**it).type(), 10) + " ";
    buffer += rightAlignNumber((**it).shopPrice()) + " ";
    consoleLeft->print(1, line++, buffer.c_str());
    }

  if (selector > 1)
    invertLine(selector, consoleLeft);

  consoleLeft->setDefaultForeground(TCODColor(96,71,64));
  consoleLeft->printFrame(0, 1, 50, 47, false);
  }

void State_shipPartShop::redrawRight(ShipPart* part)
  {
  consoleRight->clear();
  consoleRight->setDefaultForeground(TCODColor::white);

  int line = 2;
  if (part)
    {
    if (part->type() == "cannon")
      {
      auto partpointer = dynamic_cast<ShipCannons*>(part);
      getProfile(*partpointer, line);
      }
    else if (part->type() == "armor")
      {
      auto partpointer = dynamic_cast<ShipArmor*>(part);
      getProfile(*partpointer, line);
      }
    else if (part->type() == "statue")
      {
      auto partpointer = dynamic_cast<ShipStatue*>(part);
      getProfile(*partpointer, line);
      }
    else if (part->type() == "sails")
      {
      auto partpointer = dynamic_cast<ShipSails*>(part);
      getProfile(*partpointer, line);
      }
    }
  line=24;

  consoleRight->setDefaultForeground(TCODColor::yellow);
  consoleRight->print(1, line++, "Current money = %d", pToPlayer->ducats);
  consoleRight->setDefaultForeground(TCODColor(96,71,64));
  consoleRight->printFrame(0, 1, 50, 22, false);
  consoleRight->printFrame(0, 23, 50, 25, false);

  }


void State_shipPartShop::swapLineColors(TCODConsole* con, const int& counter)
  {
  if (counter %2)
      con->setDefaultForeground(TCODColor::lightestGreen);
  else con->setDefaultForeground(TCODColor::lightestBlue);
  }