#include "State_ShipStatus.h"
#include "State_stringIn.h"
#include <SDL.h>
#include <string>
#include "utility.h"

using namespace std;

State_ShipStatus::State_ShipStatus(Ship& ship)
  : refToShip(ship), selector(5), redraw(false), showDebug(false)
  {
  console = new TCODConsole(64, 46);
  debug = new TCODConsole(64, 23);
  }

State_ShipStatus::~State_ShipStatus()
  {
    
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
    for (size_t counter = 0; counter < 30 - tuple.ItemName.size(); counter++)
      returnval += blank;

  returnval += blank;

  if (tuple.averagePurchasePrice.size() > 5)
    returnval += string("xxxx");
  else returnval += tuple.averagePurchasePrice.substr(0, 5);

  if (tuple.averagePurchasePrice.size() < 5)
    for (size_t counter = 0; counter < 5 - tuple.averagePurchasePrice.size(); counter++)
      returnval += blank;
  
  returnval += string(" x");
  returnval += tuple.numberOfItems;

  return returnval;
  }

void State_ShipStatus::swapLineColors(TCODConsole* con, const int& line)
  {
  if (line %2)
      con->setDefaultForeground(TCODColor::lightestGreen);
  else con->setDefaultForeground(TCODColor::lightestBlue);
  }

void State_ShipStatus::printStats(TCODConsole* con, int& line)
  {
  con->setColorControl(TCOD_COLCTRL_1, TCODColor::grey, TCODColor::black);
  con->setColorControl(TCOD_COLCTRL_2, TCODColor::red, TCODColor::black);
  con->setColorControl(TCOD_COLCTRL_3, TCODColor::lighterYellow, TCODColor::black);
  TCOD_colctrl_t typeToUse = (TCOD_colctrl_t)8;

  swapLineColors(con, line);
  con->print(1, line++, "Type : %s", refToShip.getType().c_str()); swapLineColors(con, line);
  con->print(1, line++, "Size : %s", refToShip.getSize().c_str()); swapLineColors(con, line);
  con->print(1, line++, "Total storage: %d", refToShip.getMaxStorage()); swapLineColors(con, line);
  con->print(1, line++, "Goods: %d/%d    Sailors: %d%c(%d)%c/%d    Cannons: %d%c/%d", refToShip.getTotalGoods(), refToShip.getMaxGoods(), refToShip.sailors, TCOD_COLCTRL_1, refToShip.getMinSailors(), TCOD_COLCTRL_STOP, refToShip.getMaxSailors(), refToShip.getCannons(), TCOD_COLCTRL_1, refToShip.getMaxCannons()); swapLineColors(con, line);
  con->print(1, line++, "Lateen sails: %d    Square sails: %d", refToShip.getLateen(), refToShip.getSquare()); swapLineColors(con, line);
  con->print(1, line++, "Speed: %.1f%c/%d.0", refToShip.getSpeed(), TCOD_COLCTRL_1, refToShip.getBaseSpeed()); swapLineColors(con, line);
  con->print(1, line++, "Wave resistance: %d", refToShip.getWaveResistance()); swapLineColors(con, line);
  con->print(1, line++, "Turning: %d", refToShip.getTurning()); swapLineColors(con, line);
  con->print(1, line++, "Armor: %d", refToShip.getArmor()); swapLineColors(con, line);
  line++;

  if (refToShip.fatigue > 900)
    typeToUse = (TCOD_colctrl_t)2;
  else if (refToShip.fatigue > 500)
    typeToUse = (TCOD_colctrl_t)3;
  
  con->print(1, line++, "Fatigue: %c%.1f%c/100%c", typeToUse, refToShip.fatigue/10.0f, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  con->print(1, line++, "Training: %.1f%c/100%c", refToShip.training/10.0f, TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);

  typeToUse = (TCOD_colctrl_t)8;
  if (refToShip.rations < 50)
    typeToUse = (TCOD_colctrl_t)2;

  con->print(1, line++, "Rations: %c%.1f%c", typeToUse, refToShip.rations/10.0f, TCOD_COLCTRL_STOP);
  
  typeToUse = (TCOD_colctrl_t)8;
  if (refToShip.durability / (double)refToShip.getMaxDurability() < 0.1)
    typeToUse = (TCOD_colctrl_t)2;
  else if (refToShip.durability / (double)refToShip.getMaxDurability() < 0.5)
    typeToUse = (TCOD_colctrl_t)3;
  con->print(1, line++, "Durability: %c%d%c/%d", typeToUse, refToShip.durability, TCOD_COLCTRL_STOP, refToShip.getMaxDurability()); swapLineColors(con, line);
  line+=2;
  }

void State_ShipStatus::redrawList()
  {
  console->clear();
  console->setDefaultForeground(TCODColor::white);
  console->setColorControl(TCOD_COLCTRL_1, TCODColor::silver, TCODColor::black);

  int line = 1;
  console->print(1, line++, ("The " + refToShip.getType() + " " + refToShip.getName()).c_str());
  line++;
  console->print(1, line++,  ("%c" + rightAlignNumber(refToShip.captain.ducats) + string("%c ducats")).c_str(), TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
  printStats(console, line);
  line++; // skip a line
  auto list = refToShip.returnListOfItems();
  /// 
  console->setDefaultForeground(TCODColor::yellow);
  console->print(1, line++, header().c_str());
  console->setDefaultForeground(TCODColor::white);

  for (auto it = list.begin(); it < list.end(); it++)
    {
    swapLineColors(console, line);
    console->print(1, line++, assembleOutput(*it).c_str());
    }


  console->setDefaultForeground(TCODColor(96,71,64));
  console->printFrame(0, 0, 64, 46, false);
  }

void State_ShipStatus::drawDebug()
  {
  debug->clear();
  debug->setDefaultForeground(TCODColor::white);
  debug->setColorControl(TCOD_COLCTRL_1, TCODColor::yellow, TCODColor::black);

  int line = 1;
  if (refToShip.cannonList.size() == 0)
    debug->print(1, line++, "No cannons! o_O");
  else
  for each (ShipCannons cannon in refToShip.cannonList)
    debug->print(1, line++, "%c%s%c    %d",TCOD_COLCTRL_1, cannon.name.c_str(), TCOD_COLCTRL_STOP, cannon.pairs * 2);

  line++;

  if (refToShip.armorList.size() == 0)
    debug->print(1, line++, "No armor! O_o");
  else for each (auto it in refToShip.armorList)
    debug->print(1, line++, "%c%s%c    %d armor", TCOD_COLCTRL_1, it.second.name.c_str(), TCOD_COLCTRL_STOP, it.second.armor);

  line++;

  if (refToShip.sailList.size() == 0)
    debug->print(1, line++, "No sails! o_o");
  else for each (auto it in refToShip.sailList)
    debug->print(1, line++, "%c%s%c    %dsq, %dla", TCOD_COLCTRL_1, it.second.name.c_str(), TCOD_COLCTRL_STOP, it.second.square, it.second.lateen);

  debug->setDefaultForeground(TCODColor(96,71,64));
  debug->printFrame(0, 0, 64, 23, false);
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
  /*console->setDefaultForeground(TCODColor::white);

  int line = 1;
  console->print(1, line++, (string("The ") + refToShip.getName()).c_str());
  console->print(1, line++, (to_string((long double)refToShip.getMoney()) + string(" ducats")).c_str());
  console->print(1, line++, (string("Storage: ") + to_string((long double)refToShip.getTotalStorageUsed()) + string("/") +
                             to_string(
refToShip.getMaxStorage())).c_str());
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
  console->printFrame(0, 0, 64, 46, false);*/
  redrawList();
  drawDebug();

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
  if (showDebug)
    TCODConsole::blit(debug, 0, 0, 0, 0, root, root->getWidth() / 2 - 31, 24, 1.0f, 0.78f);
  }
void State_ShipStatus::End()
  {
  delete console;
  delete debug;
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
     {
     if (showDebug)
       showDebug = false;
     else
       popMe = true;
     }
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

   if (unicode == (int)'D')
     showDebug = !showDebug;

  }

void State_ShipStatus::MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void State_ShipStatus::MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
void State_ShipStatus::MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}