#include "town.h"
#include <iostream>
#include "Engine.h"

// dependancies: SDL.lib;SDLmain.lib;libtcod-VS.lib;libtcod-gui-VS.lib;%(AdditionalDependencies)
// temporarily copied away.

using namespace std;

ostream& operator<<(ostream& o, vector<string> right)
  {
  for (auto it = right.begin(); it < right.end(); it++)
    cout << *it;
  return o;
  }

int main(int argc,char *argv[])
  {
  /*cout << "Controls: b - buy, s - sell, -1 for \"how many\" means \"buy as much as I can\", and i - inventory.\n\n\n";
  Town TestTown(string("Barcelona"), 0.05);
  TestTown.addItems(1, 1000);
  TestTown.addItems(2, 800);
  TestTown.addItems(3, 6000);

  bool quit = false;
  char chr[100];
  Ship myShip;
  myShip.setName(string("Santa Maria"));
  myShip.addMoney(15000);

  while (!quit)
    {
    cout << "-----Town of " << TestTown.getName() << "-----" << endl;
    cout << "Ship name: " << myShip.getName() << endl;
    cout << "You have: " << myShip.getMoney() << " ducats.\n" << "Tax rate is " << TestTown.getTaxRate() * 100.0f << "%.\n";
    cout << TestTown.returnListOfItems();
    cout << "\n>> ";
    cin >> chr;

    if (chr[0] == 'b')
      {
      int ID, howMany;
      cout << "Product ID?  ";
      cin >> ID;
      cout << "How many?  ";
      cin >> howMany;
      int error = TestTown.buyItems(myShip, ID, howMany);
      switch (error)
        {
      case twSUCCESS:
        cout << "Bought " << TestTown.numberOfLastTransaction << " " << ItemDict.findItemName(ID) << " for " << TestTown.lastTransaction << " ducats" << endl;
        break;
      case twNO_SUCH_ITEM:
        cout << "Invalid item ID.\n";
        break;
      case twNOT_ENOUGH_MONEY:
        cout << "You don't have enough money.\n";
        break;
      case twNOT_ENOUGH_ITEMS:
        cout << "The town doesn't sell that many items.\n";
        break;
      default:
        cout << "Error error: unrecognized error\n";
        break;
        }
      cout << endl;
      }
    else if (chr[0] == 's')
      {
      int ID, howMany;
      cout << "ID, how many?";
      cin >> ID >> howMany;
      int error = TestTown.sellItems(myShip, ID, howMany);
      switch (error)
        {
      case twNOT_ENOUGH_ITEMS:
        cout << "You don't have that many items.\n";
        break;
      case twSUCCESS:
        cout << TestTown.numberOfLastTransaction << " " << ItemDict.findItemName(ID) << " sold for " << TestTown.lastTransaction << " ducats.";
        cout << "\nNet revenue " << TestTown.lastTransaction - TestTown.unitPurchasePriceOfSell*TestTown.numberOfLastTransaction << " ducats.\n";
        break;
      default:
        cout << "Error error: unrecognized error\n";
        break;
        }
      }
    else if (chr[0] == 'i')
      {
      cout << myShip.returnListOfItems() << endl;
      }
    cout << "\n\n\n";
    }*/
  
  CursesEngine.Init("yis!","resources/asciisquare.bmp",64,40);
  CursesEngine.Start();
  CursesEngine.End();
  return 0;
  cin.ignore(1);
  return 0;
}