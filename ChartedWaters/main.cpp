#include <iostream>
#include "Engine.h"
#include <time.h>
#include <cstdlib>
#include <fstream>
#include "utility.h"


using namespace std;

int main(int argc,char *argv[])
  {  
  srand((unsigned int)time(0));

  CursesEngine.Init("Chartered Waters","resources/asciisquare.bmp", screenwidth, screenheight);
  CursesEngine.Start();
  CursesEngine.End();
  return 0;  
}