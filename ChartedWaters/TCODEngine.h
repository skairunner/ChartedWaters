#pragma once

#include "GameState.h"
#include <SDL.h>
#include <SDL_rwops.h>
#include <stdio.h>
#include <vector>
#include <dirent.h>
#include <libtcod.hpp>
#include <fstream>

using namespace std;

extern char *charstring;
extern char *charstring2;

class TCODEngine
{
private:
	bool quit;//Exit game loop?
	//Window Properties

	bool minimized;//Is the window minimized?
	//
	void DoInput();
	void DoUpdate();
	void DoRender();
protected:
  int window_width;//Window width
	int window_height;//Window Height
  int consoleWidth, consoleHeight;
public:
	TCODEngine();
	~TCODEngine();
	bool Init(char *name,char *ascii,int winwidth,int winheight);
	void Start();
	void End();
  void QuitEngine(){quit = true;}
	//Virtual Functions
	virtual bool EngineInit(){return true;}
	virtual void Update(){}
	virtual void Render(TCODConsole *root){}
	virtual void EngineEnd(){}
	//
	virtual void Resize(int new_w,int new_h){}
	virtual void WindowActive(){}
	virtual void WindowInactive(){}
	virtual void KeyUp(const int &key,const int &unicode){}
	virtual void KeyDown(const int &key,const int &unicode){}
	virtual void MouseMoved(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
	virtual void MouseButtonUp(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
	virtual void MouseButtonDown(const int &iButton,const int &iX,const int &iY,const int &iRelX,const int &iRelY){}
	//Game States
	vector<GameState*>* pointerToStack;
private:
	vector<GameState *> StateQueue;
	GameState *currentstate;
public:
	void PushState(GameState *state);
	void PopState();
	void PopAllStates();
	GameState *GetState();
};
extern TCOD_color_t GetColor(int r,int g,int b);
extern inline int clamp(int x,int lo,int hi);