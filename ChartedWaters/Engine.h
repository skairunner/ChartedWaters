#include "TCODEngine.h"
#include <libtcod.hpp>

class Engine: public TCODEngine
{
	bool EngineInit();
	void Update();
	void Render(TCODConsole *root);
	void EngineEnd();
	//
	void Resize(int new_w,int new_h);
	void WindowActive();
	void WindowInactive();
	void KeyUp(const int &key,const int &unicode);
	void KeyDown(const int &key,const int &unicode);
	void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
	void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);
	void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY);

  int focusX, focusY;
  void lockToShip ();
};
extern Engine CursesEngine;