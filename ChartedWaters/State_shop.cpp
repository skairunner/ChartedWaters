#include "State_shop.h"
#include "State_stringIn.h"
#include <string>
#include <SDL.h>
#include "State_prompt.h"
#include <iostream>
#include <regex>
#pragma warning(disable : 4018)

using namespace std;

State_Shop::State_Shop(Town& town, Ship& ship)
  : refToTown(town), refToShip(ship), selector(5), whichConsole(false), redraw(false), startbuy(false), startsell(false),
  calculatebuy(false), calculatesell(false), isHometown(false)
  {
  consoleLeft = new TCODConsole(50, 48);
  consoleRight = new TCODConsole(50, 48);
  if(ship.faction == town.getFactionID())
    isHometown = true;
  }

string header() // The one that says ID ... name ... price ... numberof
  {
  string returnval;
  string blank(" ");

  returnval += string("Typ  Item name          Bought at  Sells at  #");
  return returnval;
  }

string shopHeader()
  {
  string returnval;
  string blank(" ");

  returnval += string("Typ Item name           Buy    Sell   #     %");
  return returnval;
  }

string changeToDecimal(const string& input)
  {
  regex rgx;
  rgx.assign("\\.\\d");
  if(!regex_search(input, rgx)) // If the regex wasn't matched -> if there is no decimal point in it.
    return input + string(".0");
  else
    return input;
  }

string State_Shop::assembleOutput(const LedgerItemTuple& tuple)
  {
  /// i: item id, _: space, n:name, p: purchase price, m: how many
  /// ID
  /// iiii_n{30}_ppppp_xmmm~
  string blank(" ");
  string type = ItemDict.findItemTypeInitials(tuple.itemID);

  string returnval;
  returnval += type;
  returnval += blank;

  returnval += tuple.ItemName.substr(0, 20);
  if(tuple.ItemName.size() < 20)
    for (int counter = 0; counter < 20 - tuple.ItemName.size(); counter++)
      returnval += blank;

  returnval += blank;

  if (tuple.averagePurchasePrice.size() > 5)
    returnval += string("xxxx");
  if (tuple.averagePurchasePrice.size() <= 5)
    {
    for (int counter = 0; counter < 5 - tuple.averagePurchasePrice.size(); counter++)
      returnval += blank;
    returnval += tuple.averagePurchasePrice.substr(0, 5);
    }

  

  returnval += string("   ");

  string sellprice = to_string((long double)refToTown.getSellPrice(tuple.itemID) * (1.0f - refToTown.getTaxRate(isHometown)));

  //returnval += string("~") + sellprice;
  
  if (sellprice.size() < 6)
    for (int counter = 0; counter < 6 - sellprice.size() - 1; counter++)
      returnval += blank;
  returnval += string("~") + sellprice.substr(0, 6);

  returnval += string("   x");
  returnval += tuple.numberOfItems;


  return returnval;
  }

string State_Shop::assembleOutput(const EconomyItemTuple& tuple)
  {
  /// i: item id, _: space, n:name, p: purchase price, m: how many
  /// ID
  /// iiii_n{30}_ppppp_xmmm~
  string blank(" ");

  string type = ItemDict.findItemTypeInitials(tuple.itemID);

  string returnval;
  returnval += type;
  returnval += blank;

  returnval += tuple.ItemName.substr(0, 19);
  if(tuple.ItemName.size() < 19)
    for (int counter = 0; counter < 19 - tuple.ItemName.size(); counter++)
      returnval += blank;

  returnval += blank;
  string buffer = tuple.BuyPrice;
  buffer = changeToDecimal(buffer);
  if (buffer.size() < 6)
    for (int counter = 0; counter < 6 - buffer.size(); counter++)
      returnval += blank;
  if (buffer.size() > 6)
    returnval += string("xxxxxxx");
  else returnval += buffer.substr(0, 6);

  returnval += blank;

  buffer = tuple.SellPrice;
  buffer = changeToDecimal(buffer);
  if (buffer.size() < 6)
    for (int counter = 0; counter < 6 - buffer.size(); counter++)
      returnval += blank;
  if (buffer.size() > 6)
    returnval += string("xxxxxx");
  else returnval += buffer.substr(0, 6);

   
  
  returnval += string(" x");
  returnval += tuple.numberOfItems;
  if (tuple.numberOfItems.size() < 4)
    for (int counter = 0; counter < 4 - tuple.numberOfItems.size(); counter++)
      returnval += blank;
  returnval += blank;
  returnval += tuple.percentageOfBasePrice;
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
  
  for (int counter = 1; counter < 50; counter++)
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
  inventory = refToShip.returnListOfItems();
  /// 
  consoleLeft->setDefaultForeground(TCODColor::yellow);
  consoleLeft->print(1, line++, header().c_str());
  consoleLeft->setDefaultForeground(TCODColor::white);

  for (auto it = inventory.begin(); it < inventory.end(); it++)
    {
    if (line %2)
      consoleLeft->setDefaultForeground(TCODColor::lightestGreen);
    else consoleLeft->setDefaultForeground(TCODColor::lightestBlue);
    consoleLeft->print(1, line++, assembleOutput(*it).c_str());
    }


  consoleLeft->setDefaultForeground(TCODColor(96,71,64));
  consoleLeft->printFrame(0, 0, 50, 48, false);
  }

void State_Shop::redrawRight()
  {
  consoleRight->clear();
  consoleRight->setDefaultForeground(TCODColor::white);
  int line = 1;
  consoleRight->print(1, line++, (string("The city of ") + refToTown.getName()).c_str());

  consoleRight->print(1, line++, (string("Tax rate is ") + to_string((long double)refToTown.getTaxRate(isHometown) * 100) + string("%%")).c_str());
  line++;
  line++; // skip a line
  goods = refToTown.returnListOfItems(isHometown);
  /// 
  consoleRight->setDefaultForeground(TCODColor::yellow);
  consoleRight->print(1, line++, shopHeader().c_str());
  consoleRight->setDefaultForeground(TCODColor::white);

  for (auto it = goods.begin(); it < goods.end(); it++)
    {
    if (line %2)
      consoleRight->setDefaultForeground(TCODColor::lightestGreen);
    else consoleRight->setDefaultForeground(TCODColor::lightestBlue);
    consoleRight->print(1, line++, assembleOutput(*it).c_str());
    }


  consoleRight->setDefaultForeground(TCODColor(96,71,64));
  consoleRight->printFrame(0, 0, 50, 48, false);
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
  TCODConsole::blit(consoleRight, 0, 0, 0, 0, root, 50, 0, 1.0f, 0.7f);
  }

void State_Shop::RecoverFromPush()
  {
  if (!nextState) 
    delete nextState; 
  pushSomething = false;
  }

void State_Shop::Update()
  {
  
  if (getPrompt  && startbuy) // If the player input something...
      {
      if (promptResult.empty())
      {
      getPrompt = false;
      startbuy = false;
      goto exit;
      }
      numberToTrade = 0;
      try {
        numberToTrade = stoi(promptResult);
        string itemName = goods.at(selector-6).ItemName;
        itemIDToTrade = goods.at(selector-6).itemID;
        if (numberToTrade < 0)
          {
          numberToTrade = refToShip.getMoney() / (refToTown.getPriceOf(itemIDToTrade) * (1 + refToTown.getTaxRate(isHometown)));
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
        getPrompt = false;
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
      int errors = refToTown.buyItems(refToShip, itemIDToTrade, numberToTrade, isHometown);
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
  else if (getPrompt && startsell)
    {
    if (promptResult.empty())
      {
      getPrompt = false;
      startsell = false;
      goto exit;
      }
    numberToTrade = 0;
      try {
        numberToTrade = stoi(promptResult);
        string itemName = inventory.at(selector-6).ItemName;
        itemIDToTrade = inventory.at(selector-6).itemID;
        if (numberToTrade < 0)
          {
          numberToTrade = stoi(inventory.at(selector-6).numberOfItems);
          }

        int total = numberToTrade * refToTown.getSellPrice(itemIDToTrade) * (double)(1 - refToTown.getTaxRate(isHometown));
        
        string print = string("Really sell ") + to_string((long double)numberToTrade) + string(" ") + itemName + string(" for ") + to_string((long double)total) + string(" ducats?");
        nextState = new State_Prompt(print.size()+4, 5, print, yesNo);
        pushSomething = true;

        promptResult.clear();
        
        getPrompt = false;
        calculatesell = true;
        startsell = false;
        }
      catch (invalid_argument e)
        {
        cout << "<debug> Invalid argument: " << e.what() << endl;
        startbuy = false;
        getPrompt = false;
        }
    }
  else if (startsell)
    {
    if (selector >= 6 && selector-6 < inventory.size()) 
      {
      promptResult.clear();
      nextState = new state_StringIn(32, promptResult, string("Sell how many? (-1 is 'all')"));
      pushSomething = true;
      getPrompt = true;
      }
    else startsell = false;
    }
  else if (calculatesell)
    {
    if (yesNo) // if said yes
      {
      int errors = refToTown.sellItems(refToShip, itemIDToTrade, numberToTrade, isHometown);
      string print;
      switch (errors)
        {
      case twSUCCESS:
        print = string("Successfully sold ") + to_string((long double)numberToTrade) + string(" ") + goods.at(selector-6).ItemName + string(" for ") + to_string((long double)refToTown.lastTransaction) + string(" ducats.");
        nextState = new State_Prompt(print.size()+4, 4, print, throwawayBool);
        pushSomething = true;
        redrawLeft();
        redrawRight();
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

    calculatesell = false;
    yesNo = false;
    }

  exit:;
  }

void State_Shop::KeyDown(const int &key,const int &unicode)
  {
  if (key == SDLK_ESCAPE)
    {
    if (refToShip.getMaxStorage() >= refToShip.getTotalStorageUsed())
      popMe = true;
    else // too many items!
      {
      string message("You have too many items.");
      nextState = new State_Prompt(message.size()+4, 4, message, throwawayBool);
      pushSomething = true;
      }
    }
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
  else if (key == SDLK_DOWN && selector < 46)
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