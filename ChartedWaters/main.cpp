#include <iostream>
#include "Engine.h"
#include <time.h>
#include <cstdlib>

//#include "cannon_win_rates.h"
#include <fstream>


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
  srand((unsigned int)time(0));

 /* WinRateSimulation wrs;

  string result = wrs.print
  cout << result;
  fstream file;
  file.open("outputs/winrates.txt", ios::trunc | ios::out);
  file << result;
  file.close();

  cin.ignore();
  return 0;*/

  CursesEngine.Init("Chartered Waters","resources/asciisquare.bmp",100,50);
  CursesEngine.Start();
  CursesEngine.End();
  return 0;
  
  
}