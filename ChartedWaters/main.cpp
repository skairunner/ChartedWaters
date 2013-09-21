#include <iostream>
#include "Engine.h"
#include <time.h>
#include <cstdlib>
#include "nameFactory.h"

// dependancies: SDL.lib;SDLmain.lib;libtcod-VS.lib;libtcod-gui-VS.lib;%(AdditionalDependencies)
// temporarily copied away.

using namespace std;
/*
ostream& operator<<(ostream& o, vector<string> right)
  {
  for (auto it = right.begin(); it < right.end(); it++)
    cout << *it;
  return o;
  }*/

int main(int argc,char *argv[])
  {  
  srand(time(0));

 /* KoreanNameFactory nf(rand());
  for (int counter = 0; counter < 30; counter++)
    {
    cout << nf.getName() << "\n";
    }

  cin.ignore(1);
  return 0;*/

  CursesEngine.Init("Chartered Waters","resources/asciisquare.bmp",100,50);
  //CursesEngine.Init("Charted Waters","resources/terminal.png",100,48);
  CursesEngine.Start();
  CursesEngine.End();
  return 0;
  
  
}