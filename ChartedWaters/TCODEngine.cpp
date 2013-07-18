#include "TCODEngine.h"
#include <iostream>

using namespace std;

char *charstring;
char *charstring2;

TCODEngine::TCODEngine()
{
	charstring=new char[300];
	charstring2=new char[300];
	window_width=640;
	window_height=480;
	quit=false;
	minimized=false;
}
TCODEngine::~TCODEngine()
{
	delete[] charstring;
	delete[] charstring2;
	StateQueue.clear();
}

bool TCODEngine::Init(char *name,char *ascii,int winwidth,int winheight)
{
  consoleWidth = winwidth;
  consoleHeight = winheight;
	//SDL_Init(SDL_INIT_EVERYTHING);
	TCODConsole::setCustomFont(ascii,TCOD_FONT_LAYOUT_ASCII_INROW,16,16);
  //TCODConsole::setCustomFont(ascii,TCOD_FONT_LAYOUT_ASCII_INCOL,16,16);
	TCODConsole::initRoot(winwidth,winheight,name,false);
	TCODSystem::setFps(30);
  //pointerToStack = &StateQueue;
	return EngineInit();
}

void TCODEngine::Start()
{
	quit=false;
	while(!quit)
	{
		DoInput();
		DoUpdate();
		if(!minimized)
			DoRender();
	}
}

void TCODEngine::DoInput()
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_KEYDOWN:
			{
				if(currentstate)
					currentstate->KeyDown(ev.key.keysym.sym,ev.key.keysym.unicode);
				else KeyDown(ev.key.keysym.sym,ev.key.keysym.unicode);
				break;
			}
			case SDL_KEYUP:
			{
				if(currentstate)
					currentstate->KeyUp(ev.key.keysym.sym,ev.key.keysym.unicode);
				else KeyUp(ev.key.keysym.sym,ev.key.keysym.unicode);
				break;
			}
			case SDL_QUIT:
			{
				quit=true;
				break;
			}
			case SDL_MOUSEMOTION:
			{
				if(currentstate)
					currentstate->MouseMoved(ev.button.button,ev.motion.x,ev.motion.y,ev.motion.xrel,ev.motion.yrel);
				else MouseMoved(ev.button.button,ev.motion.x,ev.motion.y,ev.motion.xrel,ev.motion.yrel);
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if(currentstate)
					currentstate->MouseButtonUp(ev.button.button,ev.motion.x,ev.motion.y,ev.motion.xrel,ev.motion.yrel);
				else MouseButtonUp(ev.button.button,ev.motion.x,ev.motion.y,ev.motion.xrel,ev.motion.yrel);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if(currentstate)
					currentstate->MouseButtonDown(ev.button.button,ev.motion.x,ev.motion.y,ev.motion.xrel,ev.motion.yrel);
				else MouseButtonDown(ev.button.button,ev.motion.x,ev.motion.y,ev.motion.xrel,ev.motion.yrel);
				break;
			}
			case SDL_VIDEORESIZE:
			{
				if(currentstate)
					currentstate->Resize(ev.resize.w,ev.resize.h);
				else Resize(ev.resize.w,ev.resize.h);
			}
			case SDL_ACTIVEEVENT:
			{
				if(ev.active.state & SDL_APPACTIVE)
				{
					if(ev.active.gain )
					{
						minimized=false;
						if(currentstate)
							currentstate->WindowActive();
						else WindowActive();
					}
					else
					{
						minimized=true;
						if(currentstate)
							currentstate->WindowInactive();
						else WindowInactive();
					}
				}
				break;
			}
		}
	}
}
void TCODEngine::DoUpdate() 
{
  if(currentstate)
    {
    if (currentstate->popMe)
      PopState();
    else if (currentstate->pushSomething)
      {
      currentstate->pushSomething = false;
      PushState(currentstate->nextState);
      }
    }
  
  if(currentstate)
		currentstate->Update();
  else Update();
}
void TCODEngine::DoRender()
{
	TCODConsole::root->clear();
  Render(TCODConsole::root);
  for(auto it = StateQueue.begin(); it < StateQueue.end(); it++)
    (*it)->Render(TCODConsole::root);
	TCODConsole::flush();
}
void TCODEngine::End()
{
	PopAllStates();
	EngineEnd();
	//
	SDL_Quit();
}

///////////////
//Game States//
///////////////
void TCODEngine::PushState(GameState *state)
{
	if(StateQueue.size())
		state->prev=StateQueue[StateQueue.size()-1];
	StateQueue.push_back(state);
  cout << "Pushed state.\n";
	if(StateQueue.size())
		currentstate=StateQueue[StateQueue.size()-1];
	else
		currentstate=NULL;
	currentstate->Init();
}
void TCODEngine::PopState()
{
cout << "Popped state.\n";
	currentstate->End();
  delete StateQueue.back();
	StateQueue.pop_back();

	if(StateQueue.size())
		currentstate=StateQueue[StateQueue.size()-1];
	else
		currentstate=NULL;
  if (currentstate != NULL)
    currentstate->RecoverFromPush();
}
void TCODEngine::PopAllStates()
{
	while(StateQueue.size())
		PopState();
}
GameState *TCODEngine::GetState()
{
	return currentstate;
}

TCOD_color_t GetColor(int r,int g,int b)
{
	TCOD_color_t color;
	color.r=r;
	color.g=g;
	color.b=b;
	return color;
}

inline int clamp(int x,int lo,int hi)
{
	return x<lo?lo:x>hi?hi:x;
}