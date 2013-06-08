#pragma once
#include <libtcod.hpp>

class TCODTools
  {
  public:
  static void drawRect(TCODConsole* con, const int x, const int y, const int w,const  int h, const TCODColor& fore, const TCODColor& back, int chara = 32);
  static void fillWithChar(TCODConsole* con, const int x, const int y, const int w,const  int h, const TCODColor& fore, int chara = 32);
  };