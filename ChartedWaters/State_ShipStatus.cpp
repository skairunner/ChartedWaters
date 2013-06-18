#include "State_ShipStatus.h"
#include "State_stringIn.h"
#include <SDL.h>
#include <string>

using namespace std;

State_ShipStatus::State_ShipStatus(Ship& ship)
  : refToShip(ship), selector(5), redraw(false)
  {
  console = new TCODConsole(64, 62);
  }

State_ShipStatus::~State_ShipStatus()
  {
    delete console;
    if (!stateStack) 
      delete stateStack;
  }

string State_ShipStatus::header() // The one that says ID ... name ... price ... numberof
  {
  string returnval;
  string blank(" ");

  returnval += string("Item name                      Price #");
  return returnval;
  }

string State_ShipStatus::assembleOutput(const LedgerItemTuple& tuple)
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

void State_ShipStatus::redrawList()
  {
  console->clear();
  console->setDefaultForeground(TCODColor::white);

  int line = 1;
  console->print(1, line++, (string("The ") + refToShip.getName()).c_str());
  console->print(1, line++, (to_string((long double)refToShip.getMoney()) + string(" ducats")).c_str());
  console->print(1, line++, (string("Storage: ") + to_string((long double)refToShip.getTotalStorageUsed()) + string("/") +
                             to_string((long double)refToShip.getMaxStorage())).c_str());
  line++; // skip a line
  auto list = refToShip.returnListOfItems();
  /// 
  console->setDefaultForeground(TCODColor::yellow);
  console->print(1, line++, header().c_str());
  console->setDefaultForeground(TCODColor::white);

  for (auto it = list.begin(); it < list.end(); it++)
    {
    console->print(1, line++, assembleOutput(*it).c_str());
    }


  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, 64, 62, false);
  }

void State_ShipStatus::invertLine(const int& line)
  {
  
  for (int counter = 1; counter < 63; counter++)
    {
    console->setCharBackground(counter, line, TCODColor::white);
    console->setCharForeground(counter, line, TCODColor::black);
    }
  }

bool State_ShipStatus::Init()
  {
  console->clear();
  console->setDefaultForeground(TCODColor::white);

  int line = 1;
  console->print(1, line++, (string("The ") + refToShip.getName()).c_str());
  console->print(1, line++, (to_string((long double)refToShip.getMoney()) + string(" ducats")).c_str());
  console->print(1, line++, (string("Storage: ") + to_string((long double)refToShip.getTotalStorageUsed()) + string("/") +
                             to_string((long double)refToShip.getMaxStorage())).c_str());
  line++; // skip a line
  auto list = refToShip.returnListOfItems();
  /// 
  console->setDefaultForeground(TCODColor::yellow);
  console->print(1, line++, header().c_str());
  console->setDefaultForeground(TCODColor::white);

  for (auto it = list.begin(); it < list.end(); it++)
    {
    console->print(1, line++, assembleOutput(*it).c_str());
    }


  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, 64, 62, false);

  return true;
  }
void State_ShipStatus::Update()
  {
  
  }

void State_ShipStatus::Render(TCODConsole *root)
  {
  if (redraw)
    {
    redrawList();
    invertLine(selector);
    redraw = false;
    }
  TCODConsole::blit(console, 0, 0, 0, 0, root, root->getWidth() / 2 - 31, 1, 1.0f, 0.7f);
  }
void State_ShipStatus::End()
  {
  delete console;
    if (!stateStack) 
      delete stateStack;
  }
  //
void State_ShipStatus::Resize(int new_w,int new_h){}
void State_ShipStatus::WindowActive(){}
void State_ShipStatus::WindowInactive(){}
void State_ShipStatus::KeyUp(const int &key,const int &unicode){}
void State_ShipStatus::RecoverFromPush()
  {
  if (newname != string(""))
    {
    refToShip.setName(newname);
    Init();
    newname.clear();
    }
  }

void State_ShipStatus::KeyDown(const int &key,const int &unicode)
  {
   if (key == SDLK_ESCAPE)
     popMe = true;
   else if (unicode == (int)'R')
     {
     newname.clear();
     nextState = new state_StringIn(30, newname);
     pushSomething = true;
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
  }

void State_ShipStatus::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void State_ShipStatus::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void State_ShipStatus::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}