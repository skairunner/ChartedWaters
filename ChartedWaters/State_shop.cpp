#include "State_shop.h"
#include "State_stringIn.h"
#include <string>
#include <SDL.h>
#include "State_prompt.h"
#include <iostream>
#pragma warning(disable : 4018)

using namespace std;

State_Shop::State_Shop(Town& town, Ship& ship)
  : refToTown(town), refToShip(ship), selector(5), whichConsole(false), redraw(false), startbuy(false), startsell(false),
  calculatebuy(false), calculatesell(false)
  {
  consoleLeft = new TCODConsole(64, 64);
  consoleRight = new TCODConsole(64, 64);
  
  }

string header() // The one that says ID ... name ... price ... numberof
  {
  string returnval;
  string blank(" ");

  returnval += string("Item name                      Price #");
  return returnval;
  }

string shopHeader()
  {
  string returnval;
  string blank(" ");

  returnval += string("Item name                      Buy    Sell    #");
  return returnval;
  }

string assembleOutput(const LedgerItemTuple& tuple)
  {
  /// i: item id, _: space, n:name, p: purchase price, m: how many
  /// ID
  /// iiii_n{30}_ppppp_xmmm~
  string blank(" ");

  string returnval;

  returnval += tuple.ItemName.substr(0, 30);
  if(tuple.ItemName.size() < 30)
    for (int counter = 0; counter < 30 - tuple.ItemName.size(); counter++)
      returnval += blank;

  returnval += blank;

  if (tuple.averagePurchasePrice.size() > 5)
    returnval += string("xxxx");
  else returnval += tuple.averagePurchasePrice.substr(0, 5);

  if (tuple.averagePurchasePrice.size() < 5)
    for (int counter = 0; counter < 5 - tuple.averagePurchasePrice.size(); counter++)
      returnval += blank;
  
  returnval += string(" x");
  returnval += tuple.numberOfItems;

  return returnval;
  }

string assembleOutput(const EconomyItemTuple& tuple)
  {
  /// i: item id, _: space, n:name, p: purchase price, m: how many
  /// ID
  /// iiii_n{30}_ppppp_xmmm~
  string blank(" ");

  string returnval;

  returnval += tuple.ItemName.substr(0, 30);
  if(tuple.ItemName.size() < 30)
    for (int counter = 0; counter < 30 - tuple.ItemName.size(); counter++)
      returnval += blank;

  returnval += blank;

  if (tuple.BuyPrice.size() > 7)
    returnval += string("xxxxxx");
  else returnval += tuple.BuyPrice.substr(0, 7);

  if (tuple.BuyPrice.size() < 7)
    for (int counter = 0; counter < 7 - tuple.BuyPrice.size(); counter++)
      returnval += blank;

  if (tuple.SellPrice.size() > 7)
    returnval += string("xxxxxx");
  else returnval += tuple.SellPrice.substr(0, 7);

  if (tuple.SellPrice.size() < 7)
    for (int counter = 0; counter < 7 - tuple.SellPrice.size(); counter++)
      returnval += blank;
  
  returnval += string(" x");
  returnval += tuple.numberOfItems;

  return returnval;
  }

bool State_Shop::Init()
  {
  redrawLeft();
  redrawRight();
  return true;
  }

void State_Shop::End()
  {
  delete consoleLeft;
  delete consoleRight;
  }

void State_Shop::invertLine(const int& line, TCODConsole* console)
  {
  
  for (int counter = 1; counter < 63; counter++)
    {
    console->setCharBackground(counter, line, TCODColor::white);
    console->setCharForeground(counter, line, TCODColor::black);
    }
  }

void State_Shop::redrawLeft() // Similar to State_shipstatus
  {
  consoleLeft->clear();
  consoleLeft->setDefaultForeground(TCODColor::white);

  int line = 1;
  consoleLeft->print(1, line++, (string("The ") + refToShip.getName()).c_str());
  consoleLeft->print(1, line++, (to_string((long double)refToShip.getMoney()) + string(" ducats")).c_str());
  consoleLeft->print(1, line++, (string("Storage: ") + to_string((long double)refToShip.getTotalStorageUsed()) + string("/") +
                             to_string((long double)refToShip.getMaxStorage())).c_str());
  line++; // skip a line
  auto list = refToShip.returnListOfItems();
  /// 
  consoleLeft->setDefaultForeground(TCODColor::yellow);
  consoleLeft->print(1, line++, header().c_str());
  consoleLeft->setDefaultForeground(TCODColor::white);

  for (auto it = list.begin(); it < list.end(); it++)
    {
    consoleLeft->print(1, line++, assembleOutput(*it).c_str());
    }


  consoleLeft->setDefaultForeground(TCODColor(96,71,64));
  consoleLeft->printFrame(0, 0, 64, 64, false);
  }

void State_Shop::redrawRight()
  {
  consoleRight->clear();
  consoleRight->setDefaultForeground(TCODColor::white);
  int line = 1;
  consoleRight->print(1, line++, (string("The city of ") + refToTown.getName()).c_str());

  line++;
  line++;
  line++; // skip a line
  goods = refToTown.returnListOfItems();
  /// 
  consoleRight->setDefaultForeground(TCODColor::yellow);
  consoleRight->print(1, line++, shopHeader().c_str());
  consoleRight->setDefaultForeground(TCODColor::white);

  for (auto it = goods.begin(); it < goods.end(); it++)
    {
    consoleRight->print(1, line++, assembleOutput(*it).c_str());
    }


  consoleRight->setDefaultForeground(TCODColor(96,71,64));
  consoleRight->printFrame(0, 0, 64, 64, false);
  }

void State_Shop::Render(TCODConsole *root)
  {
  if (redraw && !whichConsole)
    {
    redrawLeft();
    invertLine(selector, consoleLeft);
    redraw = false;
    }
  else if (redraw&& whichConsole)
    {
    redrawRight();
    invertLine(selector, consoleRight);
    redraw = false;
    }
  TCODConsole::blit(consoleLeft, 0, 0, 0, 0, root, 0, 0, 1.0f, 0.7f);
  TCODConsole::blit(consoleRight, 0, 0, 0, 0, root, 64, 0, 1.0f, 0.7f);
  }

void State_Shop::RecoverFromPush()
  {
  if (!nextState) 
    delete nextState; 
  pushSomething = false;
  }

void State_Shop::Update()
  {
  if (getPrompt && !promptResult.empty()) // If the player input something...
      {
      numberToTrade = 0;
      try {
        numberToTrade = stoi(promptResult);
        string itemName = goods.at(selector-6).ItemName;
        itemIDToTrade = goods.at(selector-6).itemID;
        if (numberToTrade < 0)
          {
          numberToTrade = refToShip.getMoney() / (refToTown.getPriceOf(itemIDToTrade) * (1 + refToTown.getTaxRate()));
          numberToTrade = numberToTrade > refToTown.getNumberOf(itemIDToTrade) ? refToTown.getNumberOf(itemIDToTrade) : numberToTrade;
          }

        int total = numberToTrade * refToTown.getPriceOf(itemIDToTrade) * (double)(1 + refToTown.getTaxRate());
        
        string print = string("Really buy ") + to_string((long double)numberToTrade) + string(" ") + itemName + string(" for ") + to_string((long double)total) + string(" ducats?");
        nextState = new State_Prompt(print.size()+4, 5, print, yesNo);
        pushSomething = true;

        promptResult.clear();
        
        getPrompt = false;
        calculatebuy = true;
        startbuy = false;
        }
      catch (invalid_argument e)
        {
        cout << "<debug> Invalid argument: " << e.what() << endl;
        startbuy = false;
        }
      }
  else if (startbuy)
    {
      if (selector >= 6 && selector-6 < goods.size()) 
      {
      promptResult.clear();
      nextState = new state_StringIn(32, promptResult, string("Buy how many? (-1 is 'all')"));
      pushSomething = true;
      getPrompt = true;
      }
    else startbuy = false;
    }
  else if (calculatebuy)
    {
    if (yesNo) // if said yes
      {
      int errors = refToTown.buyItems(refToShip, itemIDToTrade, numberToTrade);
      string print;
      switch (errors)
        {
      case twSUCCESS:
        print = string("Successfully bought ") + to_string((long double)numberToTrade) + string(" ") + goods.at(selector-6).ItemName + string(" for ") + to_string((long double)refToTown.lastTransaction) + string(" ducats.");
        nextState = new State_Prompt(print.size()+4, 4, print, throwawayBool);
        pushSomething = true;
        redrawLeft();
        redrawRight();
        break;
      case twNOT_ENOUGH_MONEY:
        print = string("You don't have enough money.");
        nextState = new State_Prompt(print.size()+4, 4, print, throwawayBool);
        pushSomething = true;
        break;
      case twNOT_ENOUGH_ITEMS:
        print = string("There aren't that many items to sell.");
        nextState = new State_Prompt(print.size()+4, 4, print, throwawayBool);
        pushSomething = true;
        break;
      case 0:
        break;
      default:
        cout << "How did you get here? o_O Error is: " << errors << endl;
        break;
        }
      }

    calculatebuy = false;
    yesNo = false;
    }
  else if (startsell)
    {

    startsell = false;
    }
  }



void State_Shop::KeyDown(const int &key,const int &unicode)
  {
  if (key == SDLK_ESCAPE)
    popMe = true;
  else if (key == SDLK_LEFT && whichConsole) // If the right side is selected, the left key swaps to the left.
    {
    whichConsole = false;
    selector = 6;
    
    redrawLeft();
    redrawRight();
    invertLine(selector, consoleLeft);
    }
  else if (key == SDLK_RIGHT && !whichConsole) // opposite: swap to right
    {
    whichConsole = true;
    selector = 6;
    
    redrawLeft();
    redrawRight();
    invertLine(selector, consoleRight);
    }
  else if (key == SDLK_DOWN && selector < 60)
     {
     selector++;
     redraw = true;
     }
   else if (key == SDLK_UP && selector > 6)
     {
     selector--;
     redraw = true;
     }
   else if (key == SDLK_RETURN) // Right side, ie shop.
     {
     if (whichConsole)
       startbuy = true;
     else
       startsell = true;
     }
  }