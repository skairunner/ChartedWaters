#include "TCODTools.h"


void TCODTools::drawRect(TCODConsole* con, const int x, const int y, const int w,const  int h, const TCODColor& fore, const TCODColor& back, int chara)
  {
    for (int ycounter = y; ycounter < h+y; ycounter++)
      for (int xcounter = x; xcounter < w+x; xcounter++)
        con->putCharEx(x + xcounter, y + ycounter, chara, fore, back);
  }

void TCODTools::fillWithChar(TCODConsole* con, const int x, const int y, const int w,const int h, const TCODColor& fore, int chara)
  {
    for (int ycounter = 0; ycounter < h; ycounter++)
      for (int xcounter = 0; xcounter < w; xcounter++)
        {
        con->putChar(x + xcounter, y + ycounter, chara);
        con->setCharForeground(x + xcounter, y + ycounter, fore);
        }
        //con->putCharEx(x + xcounter, y + ycounter, chara, fore, back);
  }