#include <iostream>
#include "Engine.h"
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include "goods.h"

#include "json/json-forwards.h"
#include "json/json.h"

// dependancies: SDL.lib;SDLmain.lib;libtcod-VS.lib;libtcod-gui-VS.lib;%(AdditionalDependencies)
// temporarily copied away.

using namespace std;

ostream& operator<<(ostream& o, vector<string> right)
  {
  for (auto it = right.begin(); it < right.end(); it++)
    cout << *it;
  return o;
  }

string slurp(const string& filename)
  {
  fstream file(filename.c_str());
  string output; output.clear();
  string buffer; buffer.clear();
  while (!file.eof())
    {
    getline(file, buffer);
    output.append(buffer + string("\n"));
    }
  return output;
  }

int main(int argc,char *argv[])
  {  
  srand(time(0));
 CursesEngine.Init("Charted Waters","resources/asciisquare.bmp",100,48);
  //CursesEngine.Init("Charted Waters","resources/terminal.png",100,48);
  CursesEngine.Start();
  CursesEngine.End();
  return 0;

  cin.ignore(1);
  return 0;
}